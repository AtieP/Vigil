#include <stdint.h>
#include <cpu/apic/apic.h>
#include <cpu/msr.h>
#include <mm/mm.h>

static uintptr_t get_lapic_base() {
    return (rdmsr(0x1b) & 0xfffff000) + MM_HIGHER_BASE;
}

void lapic_enable() {
    lapic_write(LAPIC_SPURIOUS_IVR, lapic_read(LAPIC_SPURIOUS_IVR) | (1 << 8) | 0xf0);
}

uint32_t lapic_read(size_t reg) {
    return *((volatile uint32_t *) (get_lapic_base() + reg));
}

void lapic_write(size_t reg, uint32_t data) {
    *((volatile uint32_t *) (get_lapic_base() + reg)) = data;
}

uint8_t lapic_get_id() {
    return (uint8_t) (lapic_read(LAPIC_APIC_ID) >> 24);
}

void lapic_eoi() {
    lapic_write(LAPIC_EOI, 0x00);
}
