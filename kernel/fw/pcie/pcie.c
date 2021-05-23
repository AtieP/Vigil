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
#include <mm/mm.h>
#include <tools/assert.h>
#include <tools/panic.h>
#include <tools/vector.h>

#define MODULE_NAME "pcie"

static struct acpi_mcfg *pcie_mcfg;

void pcie_get_mcfg() {
    pcie_mcfg = acpi_get_table("MCFG", 0);
    assert(pcie_mcfg != NULL, MODULE_NAME, "MCFG not found");
    // enumerate after LAI has been initialized
    kcon_log(KCON_LOG_INFO, MODULE_NAME, "MCFG table found");
}

void pcie_enumerate() {
    kcon_log(KCON_LOG_INFO, MODULE_NAME, "TODO: Enumerate using AML");
}

static uintptr_t pcie_get_device_address(struct acpi_mcfg_entry *mcfg_entry, uint8_t bus, uint8_t slot, uint8_t function) {
    return (
        mcfg_entry->base
        + (((bus - mcfg_entry->start_pci_bus) << 20) | (slot << 15) | (function << 12))
        + MM_HIGHER_BASE
    );
}

static struct acpi_mcfg_entry *pcie_get_segment_bus_entry(uint16_t segment, uint8_t bus) {
    size_t mcfg_entries = (pcie_mcfg->header.length - sizeof(struct acpi_mcfg)) / sizeof(struct acpi_mcfg_entry);
    for (size_t i = 0; i < mcfg_entries; i++) {
        if (
            pcie_mcfg->entries[i].segment_number == segment
            && pcie_mcfg->entries[i].start_pci_bus <= bus
            && pcie_mcfg->entries[i].end_pci_bus >= bus
        ) {
            return &pcie_mcfg->entries[i];
        }
    }
    panic(MODULE_NAME, "Could not get address for segment %d bus %d", segment, bus);
}

uint8_t pcie_read_byte(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset) {
    return *(
        (uint8_t *) (
            pcie_get_device_address(
                pcie_get_segment_bus_entry(segment, bus),
                bus, slot, function
            )
            + offset
        )
    );
}

uint16_t pcie_read_word(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset) {
    return *(
        (uint16_t *) (
            pcie_get_device_address(
                pcie_get_segment_bus_entry(segment, bus),
                bus, slot, function
            )
            + offset
        )
    );
};

uint32_t pcie_read_dword(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset) {
    return *(
        (uint32_t *) (
            pcie_get_device_address(
                pcie_get_segment_bus_entry(segment, bus),
                bus, slot, function
            )
            + offset
        )
    );
}

void pcie_write_byte(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset, uint8_t data) {
    *(
        (uint8_t *) (
            pcie_get_device_address(
                pcie_get_segment_bus_entry(segment, bus),
                bus, slot, function
            )
            + offset
        )
    ) = data;
}

void pcie_write_word(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset, uint16_t data) {
    *(
        (uint16_t *) (
            pcie_get_device_address(
                pcie_get_segment_bus_entry(segment, bus),
                bus, slot, function
            )
            + offset
        )
    ) = data;
}

void pcie_write_dword(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset, uint32_t data) {
    *(
        (uint32_t *) (
            pcie_get_device_address(
                pcie_get_segment_bus_entry(segment, bus),
                bus, slot, function
            )
            + offset
        )
    ) = data;
}
