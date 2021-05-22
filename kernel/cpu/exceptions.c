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
#include <stdint.h>
#include <cpu/cr.h>
#include <cpu/exceptions.h>
#include <cpu/idt.h>
#include <misc/kcon.h>
#include <tools/panic.h>

#define MODULE_NAME "exception handler"

static char *exception_strings[] = {   
    "Divide By Zero",
    "Debug",
    "Non Maskable Interupt",
    "Breakpoint",
    "Overflow",
    "Bound Range",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack Exception",
    "General Protection Fault",
    "Page Fault",
    "???",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection",
    "???",
    "???",
    "???",
    "???",
    "???",
    "???",
    "???",
    "???",
    "Security"
};
  
static void exception_handler(struct interrupt_frame *int_frame, uint8_t vector, uint64_t error_code) {
    panic(
        MODULE_NAME,
        "%s (vec=%x, error code: %x)\n"
        "RAX: %x, RBX: %x, RCX: %x, RDX: %x\n"
        "RSI: %x, RDI: %x, R8: %x, R9: %x\n"
        "R10: %x, R11: %x, R12: %x R13: %x\n"
        "R14: %x, R15: %x\n"
        "CS: %x, RIP: %x, SS: %x, RSP: %x, RFLAGS: %x\n"
        "CR2: %x",
        exception_strings[vector], (uint64_t) vector, error_code,
        int_frame->rax, int_frame->rbx, int_frame->rcx, int_frame->rdx,
        int_frame->rsi, int_frame->rdi, int_frame->r8, int_frame->r9,
        int_frame->r10, int_frame->r11, int_frame->r12, int_frame->r13,
        int_frame->r14, int_frame->r15,
        int_frame->cs, int_frame->rip, int_frame->ss, int_frame->rsp, int_frame->rflags,
        read_cr2()
    );
}

void exceptions_init() {
    kcon_log(KCON_LOG_INFO, MODULE_NAME, "Initializing");
    for (size_t i = 0; i < 31; i++) {
        idt_register_handler((uint8_t) i, (idt_handler_t) exception_handler, 0, 0b10001111);
    }
    kcon_log(KCON_LOG_INFO, MODULE_NAME, "Finished initializing");
}
