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
	struct vfs_node *root = vfs_get_node("/");
	if (!root) {
		panic("kernel", "Root node not found (wtf)");
	}
	struct vfs_node *node = vfs_create_node(root, "piss");
	if (!node) {
		panic("kernel", "just fucking died because of cringe");
	}
	node = vfs_get_node("/piss");
	if (!node) {
		panic("kernel", "how come there is no piss in the balls??");
	}
	node = vfs_create_node(node, "penis");
	if (!node) {
		panic("kernel", "No pingus?");
	}
	node = vfs_get_node("/piss/penis");
	if (!node) {
		panic("kernel", "lmao");
	}
	kheap_walkthrough();
	asm volatile("sti");
	for (;;) {}
	panic("kernel", "End of kernel");
}
