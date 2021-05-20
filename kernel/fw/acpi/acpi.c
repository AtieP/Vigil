#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <cpu/apic/apic.h>
#include <fw/acpi/acpi.h>
#include <misc/kcon.h>
#include <mm/mm.h>
#include <tools/assert.h>
#include <tools/builtins.h>
#include <lai/helpers/sci.h>
#include <lai/helpers/pm.h>

#define MODULE_NAME "acpi"

uint8_t rsdp_revision;
static struct acpi_rsdt *rsdt;
static struct acpi_xsdt *xsdt;
static bool use_xsdt = false;

void acpi_init(struct stivale2_struct_tag_rsdp *rsdp) {
    struct acpi_rsdp *rsdp_table = (struct acpi_rsdp *) (rsdp->rsdp + MM_HIGHER_BASE);
    kcon_log(KCON_LOG_INFO, MODULE_NAME, "Found RSDP at %p", rsdp_table);
    rsdp_revision = rsdp_table->revision;
    if (rsdp_revision >= 1) {
        use_xsdt = true;
        xsdt = (struct acpi_xsdt *) (rsdp_table->xsdt_address + MM_HIGHER_BASE);
        kcon_log(KCON_LOG_INFO, MODULE_NAME, "Using XSDT, located at: %p", xsdt);
    } else {
        rsdt = (struct acpi_rsdt *) (rsdp_table->rsdt_address + MM_HIGHER_BASE);
        kcon_log(KCON_LOG_INFO, MODULE_NAME, "Using RSDT, located at: %p", rsdt);
    }
    madt_init();
}

void *acpi_get_table(const char *signature, size_t index) {
    size_t entries;
    if (use_xsdt) {
        entries = (xsdt->header.length - sizeof(struct acpi_xsdt)) / sizeof(uint64_t);
    } else {
        entries = (rsdt->header.length - sizeof(struct acpi_rsdt)) / sizeof(uint32_t);
    }
    if (!strncmp(signature, "FACS", 4)) {
        struct acpi_fadt *fadt = acpi_get_table("FACP", 0);
        if (fadt->x_firmware_ctrl && rsdp_revision > 0) {
            return (void *) (uintptr_t) (fadt->x_firmware_ctrl + MM_HIGHER_BASE);
        }
        return (void *) (uintptr_t) (fadt->firmware_ctrl + MM_HIGHER_BASE);
    }
    if (!strncmp(signature, "DSDT", 4)) {
        struct acpi_fadt *fadt = acpi_get_table("FACP", 0);
        if (fadt->x_dsdt && rsdp_revision > 0) {
            return (void *) (uintptr_t) (fadt->x_dsdt + MM_HIGHER_BASE);
        }
        return (void *) (uintptr_t) (fadt->dsdt + MM_HIGHER_BASE);
    }
    for (size_t entry = 0, counter = 0; entry < entries; entry++) {
        struct acpi_sdt *table_header;
        if (use_xsdt) {
            table_header = (struct acpi_sdt *) (xsdt->entries[entry] + MM_HIGHER_BASE);
        } else {
            table_header = (struct acpi_sdt *) ((uintptr_t) rsdt->entries[entry] + MM_HIGHER_BASE);
        }
        if (!strncmp(table_header->signature, signature, 4)) {
            if (counter == index) {
                return (void *) table_header;
            } else {
                counter++;
            }
        }
    }
    return NULL;
}

void acpi_sci_install() {
    struct acpi_fadt *fadt = acpi_get_table("FACP", 0);
    struct acpi_madt *madt = acpi_get_table("APIC", 0);
    assert(fadt != NULL, MODULE_NAME, "FADT not found");
    assert(madt != NULL, MODULE_NAME, "MADT not found");
    if (madt->flags & 1) {
        ioapic_redirect_irq(lapic_get_id(), fadt->sci_int, idt_allocate_interrupt(acpi_sci_handler), 1 << 15);
    } else {
        ioapic_redirect_gsi(lapic_get_id(), fadt->sci_int, idt_allocate_interrupt(acpi_sci_handler), 1 << 15);
    }
    // not enabled until calling lai_init()!
}

void acpi_sci_handler(struct interrupt_frame *int_frame, uint8_t vector, uint64_t error_code) {
    (void) int_frame;
    (void) vector;
    (void) error_code;
    uint16_t sci_event = lai_get_sci_event();
    if (sci_event & ACPI_POWER_BUTTON) {
        kcon_log(KCON_LOG_INFO, MODULE_NAME, "Power off button pressed!");
    } else {
        kcon_log(KCON_LOG_INFO, MODULE_NAME, "Unhanled SCI event %d", sci_event);
    }
    lapic_eoi();
}
