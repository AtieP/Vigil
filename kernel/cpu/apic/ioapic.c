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

#include <cpu/apic/apic.h>
#include <fw/acpi/tables/madt.h>
#include <mm/mm.h>
#include <misc/kcon.h>
#include <tools/panic.h>
#include <tools/vector.h>

#define MODULE_NAME "IO/APIC"

static uint32_t get_gsi_count(uintptr_t ioapic_address) {
    return (ioapic_read(ioapic_address, IOAPIC_VER) & 0xff0000) >> 16;
}

static struct acpi_tables_madt_isc_ioapic *get_ioapic_by_gsi(uint32_t gsi) {
    for (size_t i = 0; i < vector_get_items(&madt_ioapics); i++) {
        struct acpi_tables_madt_isc_ioapic *ioapic = vector_get(&madt_ioapics, i);
        if (
            ioapic->gsi <= gsi
            && ioapic->gsi + get_gsi_count((uintptr_t) ioapic->address + MM_HIGHER_BASE) > gsi
        ) {
            return ioapic;
        }
    }
    panic(MODULE_NAME, "Unable to get IO/APIC via GSI %d", gsi);
}

uint32_t ioapic_read(uintptr_t ioapic_address, size_t reg) {
    *((volatile uint32_t *) (ioapic_address + IOAPIC_IOREGSEL)) = reg;
    return *((volatile uint32_t *) (ioapic_address + IOAPIC_IOWIN));
}

void ioapic_write(uintptr_t ioapic_address, size_t reg, uint32_t data) {
    *((volatile uint32_t *) (ioapic_address + IOAPIC_IOREGSEL)) = reg;
    *((volatile uint32_t *) (ioapic_address + IOAPIC_IOWIN)) = data;
}

void ioapic_redirect_gsi(uint8_t lapic_id, uint32_t gsi, uint8_t vector, uint64_t flags) {
    uint64_t ioredtbl_data = ((uint64_t) lapic_id << 56) | ((uint64_t) vector) | flags;
    struct acpi_tables_madt_isc_ioapic *ioapic = get_ioapic_by_gsi(gsi);
    size_t ioredtbl = (gsi - ioapic->gsi) * 2 + 16;
    ioapic_write(ioapic->address + MM_HIGHER_BASE, ioredtbl, (uint32_t) ioredtbl_data);
    ioapic_write(ioapic->address + MM_HIGHER_BASE, ioredtbl + 1, (uint32_t) (ioredtbl_data >> 32));
}

// Use this for legacy IRQs
void ioapic_redirect_irq(uint8_t lapic_id, uint8_t irq, uint8_t vector, uint64_t flags) {
    for (size_t i = 0; i < vector_get_items(&madt_isos); i++) {
        struct acpi_tables_madt_isc_iso *iso = vector_get(&madt_isos, i);
        if (iso->irq_source == irq) {
            ioapic_redirect_gsi(lapic_id, iso->gsi, vector, (uint64_t) iso->flags | flags);
            return;
        }
    }
    ioapic_redirect_gsi(lapic_id, (uint32_t) irq, vector, flags);
}
