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

#ifndef __FW_ACPI_TABLES_MCFG_H__
#define __FW_ACPI_TABLES_MCFG_H__

#include <stdint.h>
#include <fw/acpi/tables/common.h>

struct acpi_tables_mcfg_entry {
    uint64_t base;
    uint16_t segment_number;
    uint8_t start_pci_bus;
    uint8_t end_pci_bus;
    uint32_t reserved;
} __attribute__((__packed__));

struct acpi_tables_mcfg {
    struct acpi_tables_sdt sdt;
    uint64_t reserved;
    struct acpi_tables_mcfg_entry entries[];
} __attribute__((__packed__));

#endif
