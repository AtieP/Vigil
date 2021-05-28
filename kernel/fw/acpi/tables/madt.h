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

#ifndef __FW_ACPI_TABLES_MADT_H__
#define __FW_ACPI_TABLES_MADT_H__

#include <stdint.h>
#include <fw/acpi/tables/common.h>
#include <tools/vector.h>

struct acpi_tables_madt {
    struct acpi_tables_sdt sdt;
    uint32_t lapic_address; // do not use this
    uint32_t flags;
    uint8_t ics_entries[];
} __attribute__((__packed__));

// There many other kinds, but they're useless to us for now
#define ACPI_TABLES_MADT_ISC_LAPIC 0x00
#define ACPI_TABLES_MADT_ISC_IOAPIC 0x01
#define ACPI_TABLES_MADT_ISC_ISO 0x02

struct acpi_tables_madt_isc_header {
    uint8_t type;
    uint8_t length;
} __attribute__((__packed__));

struct acpi_tables_madt_isc_lapic {
    struct acpi_tables_madt_isc_header header;
    uint8_t acpi_id;
    uint8_t apic_id;
    uint32_t flags;
} __attribute__((__packed__));

struct acpi_tables_madt_isc_ioapic {
    struct acpi_tables_madt_isc_header header;
    uint8_t ioapic_id;
    uint8_t reserved;
    uint32_t address;
    uint32_t gsi;
} __attribute__((__packed__));

struct acpi_tables_madt_isc_iso {
    struct acpi_tables_madt_isc_header header;
    uint8_t bus;
    uint8_t irq_source;
    uint32_t gsi;
    uint16_t flags;
} __attribute__((__packed__));

extern struct vector madt_lapics;
extern struct vector madt_ioapics;
extern struct vector madt_isos;

void madt_init();

#endif
