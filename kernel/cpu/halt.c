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
#include <cpu/apic/apic.h>
#include <cpu/halt.h>
#include <cpu/idt.h>
#include <fw/acpi/tables/madt.h>

__attribute__((__noreturn__)) void halt() {
    // call the halt interrupt to all cores
    for (size_t i = 0; i < madt_lapics.items; i++) {
        struct acpi_tables_madt_isc_lapic *lapic = vector_get(&madt_lapics, i);
        if (lapic->apic_id == lapic_get_id()) {
            continue;
        }
        lapic_ipi(lapic->apic_id, HALT_VECTOR);
    }
    HALT_TEMPLATE();
}

void halt_interrupt_handler(struct interrupt_frame *int_frame, uint8_t vector, uint64_t error_code) {
    (void) int_frame;
    (void) vector;
    (void) error_code;
    HALT_TEMPLATE();
}
