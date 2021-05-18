#ifndef __CPU_APIC_APIC_H__
#define __CPU_APIC_APIC_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*
    LAPIC
*/
#define LAPIC_MSR 0x1b

// Following macros are for the LAPIC's
// MMIO zone
#define LAPIC_APIC_ID 0x20
#define LAPIC_APIC_VERSION 0x30
#define LAPIC_TPR 0x80
#define LAPIC_APR 0x90
#define LAPIC_PPR 0xa0
#define LAPIC_EOI 0xb0
#define LAPIC_RRD 0xc0
#define LAPIC_LOGICAL_DESTINATION_REG 0xd0
#define LAPIC_DESTINATION_FORMAT_REG 0xe0
#define LAPIC_SPURIOUS_IVR 0xf0

#define LAPIC_ISR0 0x100
#define LAPIC_ISR1 0x110
#define LAPIC_ISR2 0x120
#define LAPIC_ISR3 0x130
#define LAPIC_ISR4 0x140
#define LAPIC_ISR5 0x150
#define LAPIC_ISR6 0x160
#define LAPIC_ISR7 0x170

#define LAPIC_TMR0 0x180
#define LAPIC_TMR1 0x190
#define LAPIC_TMR2 0x1a0
#define LAPIC_TMR3 0x1b0
#define LAPIC_TMR4 0x1c0
#define LAPIC_TMR5 0x1d0
#define LAPIC_TMR6 0x1e0
#define LAPIC_TMR7 0x1f0

#define LAPIC_IRR0 0x200
#define LAPIC_IRR1 0x210
#define LAPIC_IRR2 0x220
#define LAPIC_IRR3 0x230
#define LAPIC_IRR4 0x240
#define LAPIC_IRR5 0x250
#define LAPIC_IRR6 0x260
#define LAPIC_IRR7 0x270

#define LAPIC_ERROR_REG 0x280
#define LAPIC_LVT_CMCI_REGS 0x2f0
#define LAPIC_ICR0 0x300
#define LAPIC_ICR1 0x310

#define LAPIC_LVT_TIMER_REG 0x320
#define LAPIC_LVT_THERMAL_SENSOR_REG 0x330
#define LAPIC_LVT_PERF_MON_CNT_REG 0x340
#define LAPIC_LVT_LINT0 0x350
#define LAPIC_LVT_LINT1 0x360
#define LAPIC_LVT_ERROR_REG 0x370

#define LAPIC_TIMER_INIT_COUNT 0x380
#define LAPIC_TIMER_COUNTER 0x390
#define LAPIC_TIMER_DIVIDE_CFG_REG 0x3e0

void lapic_enable();
uint32_t lapic_read(size_t reg);
void lapic_write(size_t reg, uint32_t data);
uint8_t lapic_get_id();
void lapic_eoi();

/*
    IO/APIC
*/
#define IOAPIC_IOREGSEL 0x00
#define IOAPIC_IOWIN 0x10

#define IOAPIC_VER 0x01

uint32_t ioapic_read(uintptr_t ioapic_address, size_t reg);
void ioapic_write(uintptr_t ioapic_address, size_t reg, uint32_t data);
void ioapic_redirect_gsi(uint8_t lapic_id, uint32_t gsi, uint8_t vector, uint64_t flags);
void ioapic_redirect_irq(uint8_t lapic_id, uint8_t irq, uint8_t vector, uint64_t flags);

#endif
