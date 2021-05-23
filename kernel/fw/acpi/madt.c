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

#include <fw/acpi/acpi.h>
#include <misc/kcon.h>
#include <tools/assert.h>
#include <tools/vector.h>

#define MODULE_NAME "madt"

struct vector madt_ioapics;
struct vector madt_lapics;
struct vector madt_isos;

void madt_init() {
    struct acpi_madt *madt = acpi_get_table("APIC", 0);
    assert(madt != NULL, MODULE_NAME, "MADT not found");
    vector_create(&madt_isos);
    vector_create(&madt_ioapics);
    vector_create(&madt_isos);
    size_t madt_entries_size = (madt->header.length - sizeof(struct acpi_madt));
    for (size_t i = 0; i < madt_entries_size;) {
        struct acpi_madt_entry_header *entry_header = (struct acpi_madt_entry_header *) &madt->entries[i];
        switch (entry_header->type) {
            case ACPI_MADT_ENTRY_LAPIC: {
                struct acpi_madt_entry_lapic *lapic = (struct acpi_madt_entry_lapic *) entry_header;
                kcon_log(KCON_LOG_INFO, MODULE_NAME, "Found LAPIC with ID %d", lapic->apic_id);
                vector_push(&madt_lapics, &lapic, sizeof(struct acpi_madt_entry_lapic));
                break;
            }
            case ACPI_MADT_ENTRY_IOAPIC: {
                struct acpi_madt_entry_ioapic *ioapic = (struct acpi_madt_entry_ioapic *) entry_header;
                kcon_log(KCON_LOG_INFO, MODULE_NAME, "Found IOAPIC with ID %d", ioapic->apic_id);
                vector_push(&madt_ioapics, &ioapic, sizeof(struct acpi_madt_entry_ioapic));
                break;
            }
            case ACPI_MADT_ENTRY_ISO: {
                struct acpi_madt_entry_iso *iso = (struct acpi_madt_entry_iso *) entry_header;
                kcon_log(KCON_LOG_INFO, MODULE_NAME, "Found ISO (IRQ %d redirected to GSI %d)", iso->irq_source, iso->gsi);
                vector_push(&madt_isos, &iso, sizeof(struct acpi_madt_entry_iso));
                break;
            }
            default:
                break;
        }
        i += entry_header->length;
    }
    kcon_log(KCON_LOG_INFO, MODULE_NAME, "MADT parsed successfully");
}

