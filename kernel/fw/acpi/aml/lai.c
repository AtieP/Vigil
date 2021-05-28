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

#include <stddef.h>
#include <cpu/pio.h>
#include <fw/acpi/tables/common.h>
#include <fw/pcie/pcie.h>
#include <misc/kcon.h>
#include <mm/mm.h>
#include <lai/core.h>

#define MODULE_NAME "laihost"

void *laihost_malloc(size_t size) {
    return kheap_alloc(size);
}

void *laihost_realloc(void *base, size_t newsize, size_t oldsize) {
    return kheap_realloc(base, newsize, oldsize);
}

void laihost_free(void *base, size_t size) {
    (void) size;
    kheap_free(base);
}

void laihost_log(int level, const char *msg) {
    if (level == LAI_DEBUG_LOG) {
        kcon_log(KCON_LOG_DEBUG, MODULE_NAME, msg);
        return;
    }
    if (level == LAI_WARN_LOG) {
        kcon_log(KCON_LOG_WARN, MODULE_NAME, msg);
        return;
    }
}

void *laihost_scan(const char *signature, size_t index) {
    return acpi_get_table(signature, index);
}

void *laihost_map(size_t base, size_t length) {
    (void) length;
    return (void *) (base + MM_HIGHER_BASE);
}

void laihost_unmap(void *base, size_t length) {
    (void) base;
    (void) length;
} 

void laihost_outb(uint16_t port, uint8_t data) {
    outb(port, data);
}

void laihost_outw(uint16_t port, uint16_t data) {
    outw(port, data);
}

void laihost_outd(uint16_t port, uint32_t data) {
    outd(port, data);
}

uint8_t laihost_inb(uint16_t port) {
    return inb(port);
}

uint16_t laihost_inw(uint16_t port) {
    return inw(port);
}

uint32_t laihost_ind(uint16_t port) {
    return ind(port);
}

void laihost_pci_writeb(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset, uint8_t data) {
    pcie_write_byte(segment, bus, slot, function, offset, data);
}

uint8_t laihost_pci_readb(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset) {
    return pcie_read_byte(segment, bus, slot, function, offset);
}

void laihost_pci_writew(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset, uint16_t data) {
    pcie_write_word(segment, bus, slot, function, offset, data);
}

uint16_t laihost_pci_readw(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset) {
    return pcie_read_word(segment, bus, slot, function, offset);
}

void laihost_pci_writed(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset, uint32_t data) {
    pcie_write_dword(segment, bus, slot, function, offset, data);
}

uint32_t laihost_pci_readd(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset) {
    return pcie_read_dword(segment, bus, slot, function, offset);
}

// todo: poll a timer or something, this is insane
void laihost_sleep(uint64_t ms) {
    for (size_t i = 0; i < 1000 * ms; i++) {
        inb(0x80);
    }
}
