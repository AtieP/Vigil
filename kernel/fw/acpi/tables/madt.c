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

#include <fw/acpi/tables/common.h>
#include <fw/acpi/tables/madt.h>
#include <misc/kcon.h>
#include <tools/assert.h>
#include <tools/vector.h>

#define MODULE_NAME "madt"

struct vector madt_lapics;
struct vector madt_ioapics;
struct vector madt_isos;

void madt_init() {
    struct acpi_tables_madt *madt = acpi_get_table("APIC", 0);
    assert(madt != NULL, MODULE_NAME, "MADT not found");
    vector_create(&madt_lapics, sizeof(struct acpi_tables_madt_isc_lapic));
    vector_create(&madt_ioapics, sizeof(struct acpi_tables_madt_isc_ioapic));
    vector_create(&madt_isos, sizeof(struct acpi_tables_madt_isc_iso));
    for (size_t i = 0; i < (madt->sdt.length - sizeof(struct acpi_tables_madt));) {
        struct acpi_tables_madt_isc_header *isc = (struct acpi_tables_madt_isc_header *) &madt->ics_entries[i];
        switch (isc->type) {
            case ACPI_TABLES_MADT_ISC_LAPIC: {
                struct acpi_tables_madt_isc_lapic *lapic = (struct acpi_tables_madt_isc_lapic *) isc;
                // Do not take in count disabled cores
                if (lapic->apic_id == 0xff || lapic->flags & (1 << 1)) {
                    kcon_log(KCON_LOG_INFO, MODULE_NAME, "Found LAPIC with ID %d, but apparently it cannot be used for work", lapic->apic_id);
                    break;
                }
                kcon_log(KCON_LOG_INFO, MODULE_NAME, "Found LAPIC with ID %d", lapic->apic_id);
                vector_push(&madt_lapics, lapic);
                break;
            }
            case ACPI_TABLES_MADT_ISC_IOAPIC: {
                struct acpi_tables_madt_isc_ioapic *ioapic = (struct acpi_tables_madt_isc_ioapic *) isc;
                kcon_log(KCON_LOG_INFO, MODULE_NAME, "Found IO/APIC with ID %d", ioapic->ioapic_id);
                vector_push(&madt_ioapics, ioapic);
                break;
            }
            case ACPI_TABLES_MADT_ISC_ISO: {
                struct acpi_tables_madt_isc_iso *iso = (struct acpi_tables_madt_isc_iso *) isc;
                kcon_log(KCON_LOG_INFO, MODULE_NAME, "Legacy IRQ %d mapped to GSI %d", iso->irq_source, iso->gsi);
                vector_push(&madt_isos, iso);
                break;
            }
            default:
                break;
        }
        i += isc->length;
    }
}
