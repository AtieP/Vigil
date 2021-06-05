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

#ifndef __CPU_HALT_H__
#define __CPU_HALT_H__

#include <cpu/idt.h>

#define HALT_VECTOR 0xff
#define HALT_TEMPLATE() { \
    asm volatile( \
        "1:\n\t" \
        "cli\n\t" \
        "hlt\n\t" \
        "jmp 1b\n\t" \
    ); \
    __builtin_unreachable(); \
}

__attribute__((__noreturn__)) void halt();
void halt_interrupt_handler(struct interrupt_frame *int_frame, uint8_t vector, uint64_t error_code);

#endif
