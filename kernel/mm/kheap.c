/*  
    This file is part of Vigil.

    Vigil is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Vigil is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Vigil.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stddef.h>
#include <stdint.h>
#include <misc/kcon.h>
#include <mm/mm.h>
#include <proc/mutex.h>
#include <tools/assert.h>
#include <tools/builtins.h>
#include <tools/panic.h>

// Small object allocator, based on a slab allocator

#define MODULE_NAME "kheap"

#define OBJECT_COUNT (4096)

#define ALIGN_UP(__number) (((__number) + MM_PAGE_SIZE - 1) & ~(MM_PAGE_SIZE - 1))
#define ALIGN_DOWN(__number) ((__number) & ~(MM_PAGE_SIZE - 1))

#define BIT_SET(__bit, __bitmap) (__bitmap[(__bit) / 8] |= (1 << ((__bit) % 8)))
#define BIT_CLEAR(__bit, __bitmap) (__bitmap[(__bit) / 8] &= ~(1 << ((__bit) % 8)))
#define BIT_TEST(__bit, __bitmap) ((__bitmap[(__bit) / 8] >> ((__bit) % 8)) & 1)

struct slab {
    void *base;
    uint8_t *bitmap;
    size_t object_size;
    size_t object_count;
    size_t used_objects;
    struct slab *next;
    struct mutex mutex;
};

static struct slab *slabs = NULL;

void kheap_walkthrough() {
    kcon_log(KCON_LOG_INFO, MODULE_NAME, "Slabs walkthrough:");
    struct slab *slab = slabs;
    size_t counter = 0;
    while (slab) {
        kcon_log(
            KCON_LOG_NONE,
            NULL,
            "[%d] Base: %p Bitmap: %p Object size: %d Object count: %d Used objects: %d Next: %x",
            counter, slab->base, slab->bitmap, slab->object_size, slab->object_count, slab->used_objects, slab->next
        );
        counter++;
        slab = slab->next;
    }
}

static void create_slab(size_t size) {
    assert(size != 0, MODULE_NAME, "Cannot allocate slab with 0-sized objects");
    struct slab *slab;
    if (!slabs) {
        slabs = (void *) ((uintptr_t) pmm_calloc(ALIGN_UP(sizeof(struct slab)) / MM_PAGE_SIZE) + MM_HIGHER_BASE);
        slab = slabs;
        slab->next = NULL;
    } else {
        slab = slabs;
        while (slab->next) {
            slab = slab->next;
        }
        slab->next = (void *) ((uintptr_t) pmm_calloc(ALIGN_UP(sizeof(struct slab)) / MM_PAGE_SIZE) + MM_HIGHER_BASE);
        slab = slab->next;
    }
    slab->object_count = OBJECT_COUNT;
    slab->object_size = size;
    slab->base = (void *) ((uintptr_t) pmm_alloc(ALIGN_UP(slab->object_count * slab->object_size) / MM_PAGE_SIZE) + MM_HIGHER_BASE);
    slab->bitmap = (void *) ((uintptr_t) pmm_calloc(ALIGN_UP(slab->object_count / 8) / MM_PAGE_SIZE) + MM_HIGHER_BASE);
    slab->used_objects = 0;
    slab->mutex.locked = false;
    slab->mutex.refcount = 0;
}

void kheap_init() {
    // dummy
}

void *kheap_alloc(size_t size) {
    assert(size != 0, MODULE_NAME, "Cannot allocate 0-sized object");
    size--;
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    size |= size >> 32;
    size++;
    while (size < 32) {
        size++;
    }
allocate:
    if (!slabs) {
        create_slab(size);
    }
    struct slab *slab = slabs;
    mutex_lock(&slab->mutex);
    while (slab) {
        if (slab->object_size == size) {
            for (size_t i = 0; i < slab->object_count; i++) {
                if (!BIT_TEST(i, slab->bitmap)) {
                    BIT_SET(i, slab->bitmap);
                    slab->used_objects++;
                    mutex_unlock(&slab->mutex);
                    return slab->base + (i * slab->object_size);
                }
            }
        }
        mutex_unlock(&slab->mutex);
        slab = slab->next;
    }
    create_slab(size);
    goto allocate;
    __builtin_unreachable();
}

void *kheap_calloc(size_t size) {
    void *ret = kheap_alloc(size);
    if (!ret) {
        return NULL;
    }
    memset(ret, 0, size);
    return ret;
}

void *kheap_realloc(void *old, size_t newsize, size_t oldsize) {
    if (!old && !newsize) {
        return NULL;
    }
    if (!newsize) {
        kheap_free(old);
        return NULL;
    }
    if (!old) {
        return kheap_alloc(newsize);
    }
    void *ret = kheap_alloc(newsize);
    if (!ret) {
        return NULL;
    }
    memcpy(ret, old, oldsize);
    return ret;
}

void kheap_free(void *base) {
    struct slab *slab = slabs;
    while (slab) {
        mutex_lock(&slab->mutex);
        // check if it belongs to this slab
        if (base >= slab->base && base < slab->base + (slab->object_count * slab->object_size)) {
            size_t index = (base - slab->base) / slab->object_size;
            slab->used_objects--;
            BIT_CLEAR(index, slab->bitmap);
            mutex_unlock(&slab->mutex);
            return;
        }
        mutex_unlock(&slab->mutex);
        slab = slab->next;
    }
    panic(MODULE_NAME, "Could not free memory address %p", base);
}
