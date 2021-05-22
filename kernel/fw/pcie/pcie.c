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
#include <fw/acpi/acpi.h>
#include <fw/pcie/pcie.h>
#include <misc/kcon.h>
#include <tools/assert.h>
#include <tools/panic.h>
#include <mm/mm.h>

#define MODULE_NAME "pcie"

struct acpi_mcfg *pcie_mcfg;

void pcie_init() {
    kcon_log(KCON_LOG_INFO, MODULE_NAME, "Initializing");
    pcie_mcfg = acpi_get_table("MCFG", 0);
    assert(pcie_mcfg != 0, MODULE_NAME, "MCFG not found");
    kcon_log(KCON_LOG_INFO, MODULE_NAME, "Finished initializing");
}

uintptr_t pcie_get_device_address(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function) {
    size_t mcfg_entries = (pcie_mcfg->header.length - sizeof(struct acpi_mcfg)) / sizeof(struct acpi_mcfg_entry);
    for (size_t i = 0; i < mcfg_entries; i++) {
        struct acpi_mcfg_entry *entry = (struct acpi_mcfg_entry *) &pcie_mcfg->entries[i];
        if (entry->segment_number == segment && entry->start_pci_bus <= bus && entry->end_pci_bus >= bus) {
            return (uintptr_t) (
                entry->base
                + (((bus - entry->start_pci_bus) << 20) | (slot << 15) | (function << 12))
                + MM_HIGHER_BASE
            );
        }
    }
    panic(MODULE_NAME, "Could not get PCIE segment %d", segment);
}

uint8_t pcie_read_byte(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset) {
    return *((volatile uint8_t *) (pcie_get_device_address(segment, bus, slot, function) + offset));
}

uint16_t pcie_read_word(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset) {
    return *((volatile uint16_t *) (pcie_get_device_address(segment, bus, slot, function) + offset));
}

uint32_t pcie_read_dword(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset) {
    return *((volatile uint32_t *) (pcie_get_device_address(segment, bus, slot, function) + offset));
}

void pcie_write_byte(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset, uint8_t data) {
    *((volatile uint8_t *) (pcie_get_device_address(segment, bus, slot, function) + offset)) = data;
}

void pcie_write_word(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset, uint16_t data) {
    *((volatile uint16_t *) (pcie_get_device_address(segment, bus, slot, function) + offset)) = data;
}

void pcie_write_dword(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset, uint32_t data) {
    *((volatile uint32_t *) (pcie_get_device_address(segment, bus, slot, function) + offset)) = data;
}
