#include <stddef.h>
#include <stdint.h>
#include <cpu/exceptions.h>
#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <misc/kcon.h>
#include <tools/panic.h>

#define MODULE_NAME "idt"

// Todo: make this suitable for multiprocessor systems

static struct idt_entry idt[256];

// Differences between those two:
// The first one contains the addresses to the assembly stubs,
// and the second one to the C handlers
extern uint64_t idt_pre_handlers[256];
static idt_handler_t idt_handlers[256] = { 0 };

uint8_t idt_allocate_interrupt(idt_handler_t handler) {
    for (size_t i = 0; i < 256; i++) {
        if (!idt_handlers[i]) {
            idt_register_handler(i, handler, 0, 0b10001111);
            return i;
        }
    }
    panic(MODULE_NAME, "Unable to allocate interrupt vector");
}

void idt_register_handler(uint8_t vector, idt_handler_t handler, uint8_t ist_entry, uint8_t type) {
    if (handler) {
        idt_handlers[vector] = handler;
    }

    idt[vector].offset_low = (uint16_t) idt_pre_handlers[vector];
    idt[vector].offset_mid = (uint16_t) (idt_pre_handlers[vector] >> 16);
    idt[vector].offset_high = (uint32_t) (idt_pre_handlers[vector] >> 32);

    idt[vector].selector = GDT_KERNEL_CODE64_SEL;
    idt[vector].ist_entry = ist_entry;
    idt[vector].type = type;
    idt[vector].reserved = 0;
}

void idt_global_handler(struct interrupt_frame *int_frame, uint8_t vector, uint64_t error_code) {
    idt_handler_t handler = idt_handlers[vector];
    if (handler) {
        handler(int_frame, vector, error_code);
    }
}

// Sets up the pre handlers in the IDT itself
static void idt_populate_pre_handlers() {
    for (size_t i = 0; i < 256; i++) {
        idt_register_handler(i, NULL, 0, 0b10001111);
    }
}

void idt_init() {
    kcon_log(KCON_LOG_INFO, MODULE_NAME, "Initializing");
    idt_populate_pre_handlers();
    struct idt_register idt_reg;
    idt_reg.limit = (uint16_t) sizeof(idt) - 1;
    idt_reg.base = (uint64_t) &idt;
    idt_reload(idt_reg);
    kcon_log(KCON_LOG_INFO, MODULE_NAME, "Finished initializing");
}

void idt_reload(struct idt_register idt_reg) {
    asm volatile("lidt %0" :: "m"(idt_reg) : "memory");
}
