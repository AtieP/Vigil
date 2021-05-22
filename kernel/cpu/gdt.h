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

#ifndef __CPU_GDT_H__
#define __CPU_GDT_H__

#include <stdint.h>

#define GDT_KERNEL_CODE64_SEL (0x08)
#define GDT_KERNEL_DATA_SEL (0x10)
#define GDT_USER_CODE64_SEL (0x1b)
#define GDT_USER_DATA_SEL (0x23)

struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((__packed__));

struct gdt_register {
    uint16_t limit;
    uint64_t base;
} __attribute__((__packed__));

void gdt_init();
void gdt_reload_selectors(uint16_t code, uint16_t data);

#endif
