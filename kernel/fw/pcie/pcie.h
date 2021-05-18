#ifndef __FW_PCIE_H__
#define __FW_PCIE_H__

#include <stdbool.h>
#include <stdint.h>

// Core functions
void pcie_init();

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
