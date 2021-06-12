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

#include <boot/stivale2.h>
#include <cpu/apic/apic.h>
#include <cpu/exceptions.h>
#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <cpu/pio.h>
#include <cpu/smp.h>
#include <fs/vfs.h>
#include <fw/acpi/tables/common.h>
#include <fw/acpi/tables/madt.h>
#include <fw/acpi/sci.h>
#include <fw/pcie/pcie.h>
#include <misc/kcon.h>
#include <mm/mm.h>
#include <proc/sched.h>
#include <tools/panic.h>
#include <tools/vector.h>
#include <lai/core.h>

extern uint8_t font[];

void kthread2() {
	kcon_puts("Hello from kernel thread 2!\n");
	for (;;) {}
}

void kthread1() {
	kcon_log(KCON_LOG_INFO, "kernel", "Everything initialized successfully, waiting for IRQs");
	sched_new_kernel_thread((uintptr_t) kthread2);
	kcon_puts("Hello from kernel thread 1!\n");
	for (;;) {}
}

void kmain(struct stivale2_struct *bootloader_info) {
    struct stivale2_struct_tag_framebuffer *fb = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
	struct stivale2_struct_tag_rsdp *rsdp = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_RSDP_ID);
	struct stivale2_struct_tag_memmap *memory_map = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_MEMMAP_ID);
	struct stivale2_struct_tag_smp *smp = stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_SMP_ID);

	gdt_init();
    kcon_init((uint32_t *) fb->framebuffer_addr, fb->framebuffer_width, fb->framebuffer_height, fb->framebuffer_pitch);
	kcon_set_font((uint8_t *) &font, 8, 16);
	pmm_init(memory_map);
	pmm_print_memmap(memory_map);
	vmm_init(memory_map);
	kheap_init();
	idt_init();
	exceptions_init();
	acpi_get_rsdt(rsdp->rsdp);
	madt_init();
	pcie_get_mcfg();
	lapic_enable();
	kcon_log(KCON_LOG_INFO, "kernel", "Total memory: %d MB, Usable memory: %d MB", pmm_get_memory().total / 1024 / 1024, pmm_get_memory().usable / 1024 / 1024);
	lai_set_acpi_revision(rsdp_revision);
	lai_create_namespace();
	acpi_sci_install();
	pcie_enumerate();
	vfs_init();
	kheap_walkthrough();
	devfs_init();
	dev_mem_init();
	smp_init(smp);
	sched_init((uintptr_t) kthread1);
	panic("kernel", "End of kernel");
}
