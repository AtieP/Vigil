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
#include <cpu/pio.h>
#include <fw/acpi/tables/common.h>
#include <fw/acpi/tables/mcfg.h>
#include <fw/pcie/pcie.h>
#include <misc/kcon.h>
#include <mm/mm.h>
#include <tools/assert.h>
#include <tools/panic.h>
#include <tools/vector.h>
#include <lai/core.h>

#define MODULE_NAME "pcie"

static struct acpi_tables_mcfg *pcie_mcfg;
static struct vector pcie_devices;

static uintptr_t pcie_get_device_address(struct acpi_tables_mcfg_entry *mcfg_entry, uint8_t bus, uint8_t slot, uint8_t function) {
    return (
        mcfg_entry->base
        + (((bus - mcfg_entry->start_pci_bus) << 20) | (slot << 15) | (function << 12))
        + MM_HIGHER_BASE
    );
}

static struct acpi_tables_mcfg_entry *pcie_get_segment_bus_entry(uint16_t segment, uint8_t bus) {
    size_t mcfg_entries = (pcie_mcfg->sdt.length - sizeof(struct acpi_tables_mcfg_entry)) / sizeof(struct acpi_tables_mcfg_entry);
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

static void parse_function(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function) {
    // function exists?
    if (pcie_read_word(segment, bus, slot, function, PCIE_CFG_VENDOR_ID) == 0xffff) {
        return;
    }
    kcon_log(KCON_LOG_INFO, MODULE_NAME, "Found device with segment %d bus %d slot %d function %d", segment, bus, slot, function);
    struct pcie_device device;
    device.segment = segment;
    device.bus = bus;
    device.slot = slot;
    device.function = function;
    vector_push(&pcie_devices, &device);
}

static void enumerate_slot(uint16_t segment, uint8_t bus, uint8_t slot) {
    // slot exists?
    if (pcie_read_word(segment, bus, slot, 0, PCIE_CFG_HEADER_TYPE) == 0xffff) {
        return;
    }
    // is it a multi function slot?
    size_t functions;
    if (pcie_read_byte(segment, bus, slot, 0, PCIE_CFG_HEADER_TYPE) & (1 << 7)) {
        functions = 8;
    } else {
        functions = 1;
    }
    for (size_t function = 0; function < functions; function++) {
        parse_function(segment, bus, slot, function);
    }
}

static void enumerate_bus(uint16_t segment, uint8_t bus) {
    for (size_t slot = 0; slot < 32; slot++) {
        enumerate_slot(segment, bus, slot);
    }
}

void pcie_get_mcfg() {
    pcie_mcfg = acpi_get_table("MCFG", 0);
    assert(pcie_mcfg != NULL, MODULE_NAME, "MCFG not found");
    // enumerate after LAI has been initialized
    kcon_log(KCON_LOG_INFO, MODULE_NAME, "MCFG table found");
}

void pcie_enumerate() {
    // took reference from luna, a baremetal hypervisor made by thom_tl
    // github repo: https://github.com/thomtl/luna
    LAI_CLEANUP_STATE lai_state_t state;
    lai_init_state(&state);

    LAI_CLEANUP_VAR lai_variable_t pci_pnp;
    LAI_CLEANUP_VAR lai_variable_t pcie_pnp;
    lai_eisaid(&pci_pnp, ACPI_PCI_ROOT_BUS_PNP_ID);
    lai_eisaid(&pcie_pnp, ACPI_PCIE_ROOT_BUS_PNP_ID);

    lai_nsnode_t *sb = lai_resolve_path(NULL, "_SB_");
    assert(sb != NULL, MODULE_NAME, "Could not get _SB_");

    struct lai_ns_child_iterator iterator = LAI_NS_CHILD_ITERATOR_INITIALIZER(sb);
    lai_nsnode_t *node;

    while ((node = lai_ns_child_iterate(&iterator))) {
        if (lai_check_device_pnp_id(node, &pci_pnp, &state) && lai_check_device_pnp_id(node, &pcie_pnp, &state)) {
            continue;
        }
        uint64_t segment;
        uint64_t bus;
        lai_nsnode_t *handle;
        LAI_CLEANUP_VAR lai_variable_t variable;
        // get segment
        handle = lai_resolve_path(node, "_SEG");
        if (!handle) {
            segment = 0;
        } else {
            if (lai_eval(&variable, handle, &state) != LAI_ERROR_NONE) {
                panic(MODULE_NAME, "Could not evaluate AML method");
            }
            if (lai_obj_get_integer(&variable, (uint64_t *) &segment) != LAI_ERROR_NONE) {
                panic(MODULE_NAME, "Could not get integer from AML object");
            }
        }
        // get bus
        handle = lai_resolve_path(node, "_BBN");
        if (!handle) {
            bus = 0;
        } else {
            if (lai_eval(&variable, handle, &state) != LAI_ERROR_NONE) {
                panic(MODULE_NAME, "Could not evaluate AML method");
            }
            if (lai_obj_get_integer(&variable, (uint64_t *) &bus) != LAI_ERROR_NONE) {
                panic(MODULE_NAME, "Could not get integer from AML object");
            }
        }
        kcon_log(KCON_LOG_INFO, MODULE_NAME, "Found root with segment %d bus %d", segment, bus);
        enumerate_bus(segment, bus);
    }
    kcon_log(KCON_LOG_INFO, MODULE_NAME, "Enumerated successfully");
}

uint8_t pcie_read_byte(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset) {
    return *(
        (volatile uint8_t *) (
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
        (volatile uint16_t *) (
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
        (volatile uint32_t *) (
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
        (volatile uint8_t *) (
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
        (volatile uint16_t *) (
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
        (volatile uint32_t *) (
            pcie_get_device_address(
                pcie_get_segment_bus_entry(segment, bus),
                bus, slot, function
            )
            + offset
        )
    ) = data;
}

struct pcie_device *pcie_get_device(uint8_t class, uint8_t subclass, uint8_t programming_interface) {
    struct pcie_device *device = pcie_devices.data;
    for (size_t i = 0; i < pcie_devices.items; i++) {
        if (
            pcie_read_byte(device->segment, device->bus, device->slot, device->function, PCIE_CFG_CLASS) == class
            && pcie_read_byte(device->segment, device->bus, device->slot, device->function, PCIE_CFG_SUBCLASS) == subclass
            && pcie_read_byte(device->segment, device->bus, device->slot, device->function, PCIE_CFG_PROGRAMMING_INTERFACE) == programming_interface
        ) {
            return device;
        }
        device++;
    }
    return NULL;
}

struct pcie_bar pcie_get_bar(struct pcie_device *device, int index) {
    if (index > 5) {
        panic(MODULE_NAME, "BAR is greater than 5");
    }
    struct pcie_bar ret;
    uint32_t bar = pcie_read_dword(device->segment, device->bus, device->slot, device->function, 0x10 + (index * 4));
    if (bar & 1) {
        // bar is i/o
        ret.pio = true;
        ret.base = bar & 0xfffc;
        return ret;
    } else {
        // bar is mmio
        ret.pio = false;
        uint8_t kind = (bar >> 1) & 3;
        if (kind == 0) {
            // bar is 32 bit
            ret.base = bar & 0xffffff0;
            return ret;
        }
        if (kind == 2) {
            // bar is 64 bit
            ret.base = ((uint64_t) pcie_read_dword(device->segment, device->bus, device->slot, device->function, 0x10 + (index * 4 + 4)) << 32) | (bar & 0xfffffff0);
            return ret;
        }
        panic(MODULE_NAME, "BAR is not 32 nor 64 bit");
    }
}

uint8_t pcie_read_bar_byte(struct pcie_bar *bar, size_t offset) {
    if (bar->pio) {
        return inb(bar->base + offset);
    }
    return *((volatile uint8_t *) (bar->base + offset));
}

uint16_t pcie_read_bar_word(struct pcie_bar *bar, size_t offset) {
    if (bar->pio) {
        return inw(bar->base + offset);
    }
    return *((volatile uint16_t *) (bar->base + offset));
}

uint32_t pcie_read_bar_dword(struct pcie_bar *bar, size_t offset) {
    if (bar->pio) {
        return ind(bar->base + offset);
    }
    return *((volatile uint32_t *) (bar->base + offset));
}

void pcie_write_bar_byte(struct pcie_bar *bar, size_t offset, uint8_t data) {
    if (bar->pio) {
        outb(bar->base + offset, data);
        return;
    }
    *((volatile uint8_t *) (bar->base + offset)) = data;
}

void pcie_write_bar_word(struct pcie_bar *bar, size_t offset, uint16_t data) {
    if (bar->pio) {
        outw(bar->base + offset, data);
        return;
    }
    *((volatile uint16_t *) (bar->base + offset)) = data;
}

void pcie_write_bar_dword(struct pcie_bar *bar, size_t offset, uint32_t data) {
    if (bar->pio) {
        outd(bar->base + offset, data);
        return;
    }
    *((volatile uint32_t *) (bar->base + offset)) = data;
}
