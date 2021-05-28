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

#ifndef __FW_ACPI_TABLES_COMMON_H__
#define __FW_ACPI_TABLES_COMMON_H__

#include <stddef.h>
#include <stdint.h>

struct acpi_tables_rsdp {
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_address;
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
} __attribute__((__packed__));

struct acpi_tables_sdt {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute__((__packed__));

struct acpi_tables_rsdt {
    struct acpi_tables_sdt sdt;
    uint32_t entries[];
} __attribute__((__packed__));

struct acpi_tables_xsdt {
    struct acpi_tables_sdt sdt;
    uint64_t entries[];
} __attribute__((__packed__));

struct acpi_gas {
    uint8_t address_space;
    uint8_t register_bit_width;
    uint8_t register_bit_offset;
    uint8_t reserved;
    uint64_t address;
} __attribute__((__packed__));

extern uint8_t rsdp_revision;

void acpi_get_rsdt(uintptr_t rsdp_address);
void *acpi_get_table(const char *signature, size_t index);

#endif
