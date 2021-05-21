#include <boot/stivale2.h>
#include <cpu/apic/apic.h>
#include <cpu/exceptions.h>
#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <cpu/pio.h>
#include <fs/vfs.h>
#include <fw/acpi/acpi.h>
#include <fw/pcie/pcie.h>
#include <misc/kcon.h>
#include <mm/mm.h>
#include <tools/panic.h>
#include <tools/vector.h>
#include <lai/helpers/pm.h>

extern uint8_t font[];

void kmain(struct stivale2_struct *bootloader_info) {
    struct stivale2_struct_tag_framebuffer *fb = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
	struct stivale2_struct_tag_rsdp *rsdp = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_RSDP_ID);
	struct stivale2_struct_tag_memmap *memory_map = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_MEMMAP_ID);

	gdt_init();
    kcon_init((uint32_t *) fb->framebuffer_addr, fb->framebuffer_width, fb->framebuffer_height, fb->framebuffer_pitch);
	kcon_set_font((uint8_t *) &font, 8, 16);
	pmm_init(memory_map);
	pmm_print_memmap(memory_map);
	vmm_init(memory_map);
	kheap_init();
	idt_init();
	exceptions_init();
	acpi_init(rsdp);
	pcie_init();
	lapic_enable();
	kcon_log(KCON_LOG_INFO, "kernel", "Total memory: %d MB, Usable memory: %d MB", pmm_get_memory().total / 1024 / 1024, pmm_get_memory().usable / 1024 / 1024);
	lai_init();
	vfs_init();
	kheap_walkthrough();
	asm volatile("sti");
	for (;;) {}
	panic("kernel", "End of kernel");
}
