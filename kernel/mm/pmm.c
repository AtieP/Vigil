#include <stddef.h>
#include <stdint.h>
#include <boot/stivale2.h>
#include <misc/kcon.h>
#include <mm/mm.h>
#include <mp/mutex.h>
#include <tools/assert.h>
#include <tools/builtins.h>
#include <tools/panic.h>

static struct mutex pmm_mutex;

extern char _kernel_start[];
extern char _kernel_end[];

#define MODULE_NAME "pmm"

#define ALIGN_UP(__number) (((__number) + MM_PAGE_SIZE - 1) & ~(MM_PAGE_SIZE - 1))
#define ALIGN_DOWN(__number) ((__number) & ~(MM_PAGE_SIZE - 1))

#define BIT_SET(__bit) (bitmap[(__bit) / 8] |= (1 << ((__bit) % 8)))
#define BIT_CLEAR(__bit) (bitmap[(__bit) / 8] &= ~(1 << ((__bit) % 8)))
#define BIT_TEST(__bit) ((bitmap[(__bit) / 8] >> ((__bit) % 8)) & 1)

static uint8_t *bitmap;
static uintptr_t highest_page;

void pmm_init(struct stivale2_struct_tag_memmap *memory_map) {
    kcon_log(KCON_LOG_INFO, MODULE_NAME, "Initializing");
    for (uint64_t i = 0; i < memory_map->entries; i++) {
        if (
            memory_map->memmap[i].type != STIVALE2_MMAP_USABLE
            && memory_map->memmap[i].type != STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE
        ) {
            continue;
        }
        uintptr_t top = memory_map->memmap[i].base + memory_map->memmap[i].length;
        if (top > highest_page) {
            highest_page = top;
        }
    }
    size_t bitmap_size = ALIGN_UP(ALIGN_DOWN(highest_page) / MM_PAGE_SIZE / 8);
    for (uint64_t i = 0; i < memory_map->entries; i++) {
        if (memory_map->memmap[i].type != STIVALE2_MMAP_USABLE) {
            continue;
        }
        if (memory_map->memmap[i].length >= bitmap_size) {
            bitmap = (uint8_t *) (memory_map->memmap[i].base + MM_HIGHER_BASE);
            memory_map->memmap[i].base += bitmap_size;
            memory_map->memmap[i].length -= bitmap_size;
            goto bitmap_allocated;
        }
    }
    panic(MODULE_NAME, "Could not allocate %d bytes for the bitmap", bitmap_size);
bitmap_allocated:
    memset(bitmap, 0xff, bitmap_size);
    for (uint64_t i = 0; i < memory_map->entries; i++) {
        if (memory_map->memmap[i].type != STIVALE2_MMAP_USABLE) {
            continue;
        }
        // thing above may have broken an usable entry
        if (!(memory_map->memmap[i].length)) {
            continue;
        }
        for (
            uint64_t addr = memory_map->memmap[i].base;
            addr < memory_map->memmap[i].base + memory_map->memmap[i].length;
            addr += MM_PAGE_SIZE
        ) {
            BIT_CLEAR(addr / MM_PAGE_SIZE);
        }
    }
    kcon_log(KCON_LOG_INFO, MODULE_NAME, "Finished initializing");
}

static const char *get_memmap_entry_type(int type) {
    switch (type) {
        case STIVALE2_MMAP_USABLE:
            return "Usable";
        case STIVALE2_MMAP_RESERVED:
            return "Reserved";
        case STIVALE2_MMAP_ACPI_RECLAIMABLE:
            return "ACPI Reclaimable";
        case STIVALE2_MMAP_ACPI_NVS:
            return "ACPI Non Volatile Storage";
        case STIVALE2_MMAP_BAD_MEMORY:
            return "Bad Memory";
        case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE:
            return "Bootloader Reclaimable";
        case STIVALE2_MMAP_KERNEL_AND_MODULES:
            return "Kernel And Modules";
        case STIVALE2_MMAP_FRAMEBUFFER:
            return "Framebuffer";
        default:
            return "???";
    }
}

void pmm_print_memmap(struct stivale2_struct_tag_memmap *memory_map) {
    kcon_log(KCON_LOG_INFO, MODULE_NAME, "Printing memory map:");
    for (uint64_t i = 0; i < memory_map->entries; i++) {
        kcon_log(
            KCON_LOG_NONE,
            NULL,
            "Base: %x Length: %x Type: %s",
            memory_map->memmap[i].base,
            memory_map->memmap[i].length,
            get_memmap_entry_type(memory_map->memmap[i].type)
        );
    }
}

void *pmm_alloc(size_t pages) {
    mutex_lock(&pmm_mutex);
    assert(pages != 0, MODULE_NAME, "Cannot allocate 0 pages");
    size_t pages_found = 0;
    for (size_t i = 0; i < highest_page / MM_PAGE_SIZE; i++) {
        if (!BIT_TEST(i)) {
            pages_found++;
        } else if (pages_found != pages) {
            pages_found = 0;
            continue;
        }
        if (pages_found == pages) {
            size_t j;
            for (j = 0; j < pages; j++) {
                BIT_SET(i - j);
            }
            i -= j - 1;
            mutex_unlock(&pmm_mutex);
            return (void *) (MM_PAGE_SIZE * i);
        }
    }
    panic(MODULE_NAME, "Could not allocate %d pages", pages);
}

void *pmm_calloc(size_t pages) {
    void *allocated = pmm_alloc(pages);
    allocated = (void *) ((uintptr_t) allocated + MM_HIGHER_BASE);
    memset(allocated, 0, pages * MM_PAGE_SIZE);
    allocated = (void *) ((uintptr_t) allocated - MM_HIGHER_BASE);
    return allocated;
}

void *pmm_realloc(void *old, size_t newpages, size_t oldpages) {
    void *new = pmm_alloc(newpages) + MM_HIGHER_BASE;
    memcpy(new, old + MM_HIGHER_BASE, oldpages / MM_PAGE_SIZE);
    pmm_free(old, oldpages);
    return new - MM_HIGHER_BASE;
}

void pmm_free(void *ptr, size_t pages) {
    assert(pages != 0, MODULE_NAME, "Cannot free 0 pages");
    mutex_lock(&pmm_mutex);
    for (size_t i = 0; i < pages; i++, ptr += MM_PAGE_SIZE) {
        BIT_CLEAR((uintptr_t) ptr / MM_PAGE_SIZE);
    }
    mutex_unlock(&pmm_mutex);
}

struct pmm_memory pmm_get_memory() {
    mutex_lock(&pmm_mutex); // to get current stats
    struct pmm_memory memory = { 0 };
    for (size_t i = 0; i < highest_page / MM_PAGE_SIZE; i++) {
        memory.total += MM_PAGE_SIZE;
        if (!BIT_TEST(i)) {
            memory.usable += MM_PAGE_SIZE;
        }
    }
    mutex_unlock(&pmm_mutex);
    return memory;
}
