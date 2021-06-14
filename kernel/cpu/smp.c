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

#include <stdbool.h>
#include <stddef.h>
#include <boot/stivale2.h>
#include <cpu/apic/apic.h>
#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <cpu/locals.h>
#include <cpu/smp.h>
#include <misc/kcon.h>
#include <mm/mm.h>

#define MODULE_NAME "smp"

static void core_init(struct stivale2_smp_info *smp_info) {
    smp_info = (struct stivale2_smp_info *) ((uintptr_t) smp_info + MM_HIGHER_BASE);
    gdt_reload_reg();
    gdt_reload_selectors(GDT_KERNEL_CODE64_SEL, GDT_KERNEL_DATA_SEL);
    idt_reload_reg();
    vmm_switch_pagemap(vmm_get_kernel_pagemap());
    lapic_enable();
    struct cpu_locals *ap_locals = kheap_alloc(sizeof(struct cpu_locals));
    ap_locals->lapic_id = smp_info->lapic_id;
    locals_cpu_set(ap_locals);
    kcon_log(KCON_LOG_INFO, MODULE_NAME, "Core with LAPIC ID %d brought up successfully", smp_info->lapic_id);
    asm volatile(
        "1:\n\t"
        "sti\n\t"
        "hlt\n\t"
        "jmp 1b\n\t"
    ); // core will be used for tasking
}

void smp_init(struct stivale2_struct_tag_smp *smp_info) {
    kcon_log(KCON_LOG_INFO, MODULE_NAME, "Bringing up all other cores");
    uint8_t bsp_lapic_id = smp_info->bsp_lapic_id;
    for (size_t i = 0; i < smp_info->cpu_count; i++) {
        if (smp_info->smp_info[i].lapic_id == bsp_lapic_id) {
            continue;
        }
        smp_info->smp_info[i].target_stack = (uint64_t) pmm_alloc(32) + (MM_PAGE_SIZE * 32) + MM_HIGHER_BASE;
        smp_info->smp_info[i].goto_address = (uint64_t) core_init;
    }
    struct cpu_locals *bsp_locals = kheap_alloc(sizeof(struct cpu_locals));
    bsp_locals->lapic_id = bsp_lapic_id;
    locals_cpu_set(bsp_locals);
}
