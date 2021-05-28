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

#include <stdbool.h>
#include <stddef.h>
#include <misc/kcon.h>
#include <mm/mm.h>
#include <fw/acpi/tables/common.h>
#include <fw/acpi/tables/fadt.h>
#include <tools/builtins.h>

#define MODULE_NAME "acpi"

uint8_t rsdp_revision;
static struct acpi_tables_rsdt *rsdt;
static struct acpi_tables_xsdt *xsdt;
static bool use_xsdt;

void acpi_get_rsdt(uintptr_t rsdp_address) {
    struct acpi_tables_rsdp *rsdp = (struct acpi_tables_rsdp *) (rsdp_address + MM_HIGHER_BASE);
    rsdp_revision = rsdp->revision;
    if (rsdp_revision > 0 && rsdp->xsdt_address) {
        use_xsdt = true;
        xsdt = (struct acpi_tables_xsdt *) (rsdp->xsdt_address + MM_HIGHER_BASE);
        kcon_log(KCON_LOG_INFO, MODULE_NAME, "Using XSDT at %p", xsdt);
    } else {
        use_xsdt = false;
        rsdt = (struct acpi_tables_rsdt *) ((uintptr_t) rsdp->rsdt_address + MM_HIGHER_BASE);
        kcon_log(KCON_LOG_INFO, MODULE_NAME, "Using RSDT at %p", rsdt);
    }
}

void *acpi_get_table(const char *signature, size_t index) {
    if (!strncmp(signature, "DSDT", 4)) {
        struct acpi_tables_fadt *fadt = acpi_get_table("FACP", 0);
        if (rsdp_revision > 0 && fadt->x_dsdt) {
            return (void *) (fadt->x_dsdt + MM_HIGHER_BASE);
        }
        return (void *) ((uintptr_t) fadt->dsdt_address + MM_HIGHER_BASE);
    };
    if (!strncmp(signature, "FACS", 4)) {
        struct acpi_tables_fadt *fadt = acpi_get_table("FACP", 0);
        if (rsdp_revision > 0 && fadt->x_firmware_ctrl) {
            return (void *) (fadt->x_firmware_ctrl + MM_HIGHER_BASE);
        }
        return (void *) ((uintptr_t) fadt->firmware_ctrl + MM_HIGHER_BASE);
    }
    size_t entries;
    if (use_xsdt) {
        entries = (xsdt->sdt.length - sizeof(struct acpi_tables_xsdt)) / sizeof(uint64_t);
    } else {
        entries = (rsdt->sdt.length - sizeof(struct acpi_tables_rsdt)) / sizeof(uint32_t);
    }
    for (size_t entry = 0, counter = 0; entry < entries; entry++) {
        struct acpi_tables_sdt *sdt;
        if (use_xsdt) {
            sdt = (struct acpi_tables_sdt *) (xsdt->entries[entry] + MM_HIGHER_BASE);
        } else {
            sdt = (struct acpi_tables_sdt *) ((uintptr_t) rsdt->entries[entry] + MM_HIGHER_BASE);
        }
        if (!strncmp(sdt->signature, signature, 4)) {
            if (counter == index) {
                return sdt;
            }
            counter++;
        }
    }
    return NULL;
}
