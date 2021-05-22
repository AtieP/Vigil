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

#include <stdint.h>
#include <cpu/gdt.h>

static struct {
    struct gdt_entry entry[5];
    // add tss here later
} __attribute__((__packed__)) gdt;

void gdt_init() {
    // null descriptor
    gdt.entry[0].limit_low = 0;
    gdt.entry[0].base_low = 0;
    gdt.entry[0].base_mid = 0;
    gdt.entry[0].access = 0;
    gdt.entry[0].granularity = 0;
    gdt.entry[0].base_high = 0;

    // kernel code descriptor
    gdt.entry[1].limit_low = 0;
    gdt.entry[1].base_low = 0;
    gdt.entry[1].base_mid = 0;
    gdt.entry[1].access = 0b10011010; // present, dpl 0, descriptor, cs, rw
    gdt.entry[1].granularity = 0b00100000; // l bit
    gdt.entry[1].base_high = 0;

    // kernel data descriptor
    gdt.entry[2].limit_low = 0;
    gdt.entry[2].base_low = 0;
    gdt.entry[2].base_mid = 0;
    gdt.entry[2].access = 0b10010010; // present, dpl 0, descriptor, rw
    gdt.entry[2].granularity = 0;
    gdt.entry[2].base_high = 0;

    // user code descriptor
    gdt.entry[3].limit_low = 0;
    gdt.entry[3].base_low = 0;
    gdt.entry[3].base_mid = 0;
    gdt.entry[3].access = 0b11111010; // present, dpl 3, descriptor, cs, rw
    gdt.entry[3].granularity = 0b00100000; // l bit
    gdt.entry[3].base_high = 0;

    // user data descriptor
    gdt.entry[4].limit_low = 0;
    gdt.entry[4].base_low = 0;
    gdt.entry[4].base_mid = 0;
    gdt.entry[4].access = 0b11110010; // present, dpl 3, descriptor, rw
    gdt.entry[4].granularity = 0;
    gdt.entry[4].base_high = 0;

    struct gdt_register gdt_reg;
    gdt_reg.limit = (uint16_t) sizeof(gdt) - 1;
    gdt_reg.base = (uint64_t) &gdt;
    asm volatile("lgdt %0" :: "m"(gdt_reg) : "memory");

    gdt_reload_selectors(GDT_KERNEL_CODE64_SEL, GDT_KERNEL_DATA_SEL);
}

void gdt_reload_selectors(uint16_t code, uint16_t data) {
    asm volatile(
        "push %0\n\t"
        "push $1f\n\t"
        "lretq\n\t"
        "1:\n\t"
        "mov %1, %%ds\n\t"
        "mov %1, %%es\n\t"
        "mov %1, %%fs\n\t"
        "mov %1, %%gs\n\t"
        "mov %1, %%ss\n\t"
        :: "rmi"((uint64_t) code), "rm"((uint64_t) data)
        : "memory"
    );
}
