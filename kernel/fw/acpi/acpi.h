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

#ifndef __FW_ACPI_ACPI__
#define __FW_ACPI_ACPI__

#include <stddef.h>
#include <stdint.h>
#include <boot/stivale2.h>
#include <cpu/idt.h>

struct acpi_rsdp {
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_address;

    // acpi 2.0+
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    char reserved[3];
} __attribute__((__packed__));

struct acpi_sdt {
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

struct acpi_gas {
    uint8_t address_space;
    uint8_t register_bit_width;
    uint8_t register_bit_offset;
    uint8_t reserved;
    uint64_t address;
} __attribute__((__packed__));

struct acpi_rsdt {
    struct acpi_sdt header;
    uint32_t entries[];
} __attribute__((__packed__));

struct acpi_xsdt {
    struct acpi_sdt header;
    uint64_t entries[];
} __attribute__((__packed__));

struct acpi_mcfg_entry {
    uint64_t base;
    uint16_t segment_number;
    uint8_t start_pci_bus;
    uint8_t end_pci_bus;
    uint32_t reserved;
} __attribute__((__packed__));

struct acpi_mcfg {
    struct acpi_sdt header;
    uint64_t reserved;
    struct acpi_mcfg_entry entries[];
} __attribute__((__packed__));

// Section 5.2.9
struct acpi_fadt {
    struct acpi_sdt header;
    uint32_t firmware_ctrl;
    uint32_t dsdt;
    uint8_t reserved;
    uint8_t preferred_pm_profile;
    uint16_t sci_int;
    uint32_t sci_cmd;
    uint8_t acpi_enable;
    uint8_t acpi_disable;
    uint8_t s4bios_req;
    uint8_t pstate_cnt;
    uint32_t pm1a_evt_blk;
    uint32_t pm1b_evt_blk;
    uint32_t pm1a_cnt_blk;
    uint32_t pm1b_cnt_blk;
    uint32_t pm2_cnt_blk;
    uint32_t pm_tmr_blk;
    uint32_t gpe0_blk;
    uint32_t gpe1_blk;
    uint8_t pm1_evt_len;
    uint8_t pm1_cnt_len;
    uint8_t pm2_cnt_len;
    uint8_t pm_tmr_len;
    uint8_t gpe0_blk_len;
    uint8_t gpe1_blk_len;
    uint8_t gpe1_base;
    uint8_t cst_cnt;
    uint16_t p_lv2_lat;
    uint16_t p_lv3_lat;
    uint16_t flush_size;
    uint16_t flush_stride;
    uint8_t duty_offset;
    uint8_t duty_width;
    uint8_t day_alrm;
    uint8_t mon_alrm;
    uint8_t century;
    uint16_t iapc_boot_arch;
    uint8_t reserved_2;
    uint32_t flags;
    uint8_t reset_reg[12];
    uint8_t reset_value;
    uint16_t arm_boot_arch;
    uint8_t fadt_minor_version;
    uint64_t x_firmware_ctrl;
    uint64_t x_dsdt;

    struct acpi_gas x_pm1a_evt_blk;
    struct acpi_gas x_pm1b_evt_blk;
    struct acpi_gas x_pm1a_cnt_blk;
    struct acpi_gas x_pm1b_cnt_blk;
    struct acpi_gas x_pm2_cnt_blk;
    struct acpi_gas x_pm_tmr_blk;
    struct acpi_gas x_gpe0_blk;
    struct acpi_gas x_gpe1_blk;
    struct acpi_gas sleep_control_reg;
    struct acpi_gas sleep_status_reg;
    char hypervisor_id[8];
} __attribute__((__packed__));

struct acpi_madt {
    struct acpi_sdt header;
    uint32_t lapic_address;
    uint32_t flags;
    uint8_t entries[]; // uint8_t[] because entries do not share the same length
} __attribute__((__packed__));

#define ACPI_MADT_ENTRY_LAPIC 0
#define ACPI_MADT_ENTRY_IOAPIC 1
#define ACPI_MADT_ENTRY_ISO 2
#define ACPI_MADT_ENTRY_NMI 4

struct acpi_madt_entry_header {
    uint8_t type;
    uint8_t length;
} __attribute__((__packed__));

struct acpi_madt_entry_lapic {
    struct acpi_madt_entry_header header;
    uint8_t acpi_id;
    uint8_t apic_id;
    uint32_t flags;
} __attribute__((__packed__));

struct acpi_madt_entry_ioapic {
    struct acpi_madt_entry_header header;
    uint8_t apic_id;
    uint8_t reserved;
    uint32_t ioapic_address;
    uint32_t gsi;
} __attribute__((__packed__));

struct acpi_madt_entry_iso {
    struct acpi_madt_entry_header header;
    uint8_t bus_source;
    uint8_t irq_source;
    uint32_t gsi;
    uint16_t flags;
} __attribute__((__packed__));

struct acpi_madt_entry_nmi {
    struct acpi_madt_entry_header header;
    uint8_t acpi_id;
    uint16_t flags;
    uint8_t lint;
} __attribute__((__packed__));

extern uint8_t rsdp_revision;

// ACPI Generic
void acpi_init(struct stivale2_struct_tag_rsdp *rsdp);
void *acpi_get_table(const char *signature, size_t index);
void acpi_sci_install();
void acpi_sci_handler(struct interrupt_frame *int_frame, uint8_t vector, uint64_t error_code);

// Custom LAI functions
void lai_init();

// MADT
void madt_init();

#endif // !__ACPI__
