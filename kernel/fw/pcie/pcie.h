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
    bool present;
    uint16_t segment;
    uint8_t bus;
    uint8_t slot;
    uint8_t function;
};

struct pcie_device pcie_get_device(uint8_t class, uint8_t subclass, uint8_t programming_interface);
uintptr_t pcie_get_bar(struct pcie_device *device, int bar);

#endif
