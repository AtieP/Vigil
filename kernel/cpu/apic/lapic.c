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
#include <cpu/apic/apic.h>
#include <cpu/msr.h>
#include <mm/mm.h>

static uintptr_t get_lapic_base() {
    return (rdmsr(0x1b) & 0xfffff000) + MM_HIGHER_BASE;
}

void lapic_enable() {
    lapic_write(LAPIC_SPURIOUS_IVR, lapic_read(LAPIC_SPURIOUS_IVR) | (1 << 8) | 0xf0);
}

uint32_t lapic_read(size_t reg) {
    return *((volatile uint32_t *) (get_lapic_base() + reg));
}

void lapic_write(size_t reg, uint32_t data) {
    *((volatile uint32_t *) (get_lapic_base() + reg)) = data;
}

uint8_t lapic_get_id() {
    return (uint8_t) (lapic_read(LAPIC_APIC_ID) >> 24);
}

void lapic_ipi(uint8_t lapic_id, uint8_t vector) {
    lapic_write(LAPIC_ICR1, (uint32_t) lapic_id << 24);
    lapic_write(LAPIC_ICR0, vector);
}

void lapic_eoi() {
    lapic_write(LAPIC_EOI, 0x00);
}
