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
#include <cpu/idt.h>
#include <misc/kcon.h>
#include <fw/acpi/tables/common.h>
#include <fw/acpi/tables/fadt.h>
#include <fw/acpi/tables/madt.h>
#include <fw/acpi/sci.h>
#include <tools/assert.h>
#include <lai/helpers/sci.h>

#define MODULE_NAME "acpi"

static void sci_handler(struct interrupt_frame *int_frame, uint8_t vector, uint64_t error_code) {
    (void) int_frame;
    (void) vector;
    (void) error_code;
    uint16_t sci_event = lai_get_sci_event();
    if (sci_event & ACPI_POWER_BUTTON) {
        kcon_log(KCON_LOG_INFO, MODULE_NAME, "Power button pressed!");
    } else {
        kcon_log(KCON_LOG_WARN, MODULE_NAME, "Unhandled SCI event: %x", sci_event);
    }
    lapic_eoi();
}

void acpi_sci_install() {
    struct acpi_tables_fadt *fadt = acpi_get_table("FACP", 0);
    struct acpi_tables_madt *madt = acpi_get_table("APIC", 0);
    assert(fadt != NULL, MODULE_NAME, "FADT not found");
    assert(madt != NULL, MODULE_NAME, "MADT not found");
    if (madt->flags & 1) {
        ioapic_redirect_irq(lapic_get_id(), fadt->sci_int, idt_allocate_interrupt(sci_handler), 1 << 15);
    } else {
        ioapic_redirect_gsi(lapic_get_id(), fadt->sci_int, idt_allocate_interrupt(sci_handler), 1 << 15);
    }
    lai_enable_acpi(1); // 0: PIC, 1: IO/APIC, 2: IO/SAPIC (only for IA-64)
}
