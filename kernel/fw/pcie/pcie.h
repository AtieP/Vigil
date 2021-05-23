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

#ifndef __FW_PCIE_H__
#define __FW_PCIE_H__

#include <stdbool.h>
#include <stdint.h>

#define PCIE_CFG_VENDOR_ID 0x00
#define PCIE_CFG_DEVICE_ID 0x02
#define PCIE_CFG_COMMAND 0x04
#define PCIE_CFG_STATUS 0x06
#define PCIE_CFG_REVISION_ID 0x08
#define PCIE_CFG_PROGRAMMING_INTERFACE 0x09
#define PCIE_CFG_SUBCLASS 0x0a
#define PCIE_CFG_CLASS 0x0b
#define PCIE_CFG_CACHE_LINE_SIZE 0x0c
#define PCIE_CFG_LATENCY_TIMER 0x0d
#define PCIE_CFG_HEADER_TYPE 0x0e
#define PCIE_CFG_BIST 0x0f
#define PCIE_CFG_BAR0 0x10
#define PCIE_CFG_BAR1 0x14
#define PCIE_CFG_BAR2 0x18
#define PCIE_CFG_BAR3 0x1c
#define PCIE_CFG_BAR4 0x20
#define PCIE_CFG_BAR5 0x24

// Core functions
void pcie_get_mcfg();
void pcie_enumerate();

uint8_t pcie_read_byte(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset);
uint16_t pcie_read_word(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset);
uint32_t pcie_read_dword(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset);

void pcie_write_byte(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset, uint8_t data);
void pcie_write_word(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset, uint16_t data);
void pcie_write_dword(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset, uint32_t data);

// Helpers
struct pcie_device {
    uint16_t segment;
    uint8_t bus;
    uint8_t slot;
    uint8_t function;
};

struct pcie_device *pcie_get_device(uint8_t class, uint8_t subclass, uint8_t programming_interface);
uintptr_t pcie_get_bar(struct pcie_device *device, int bar);

#endif
