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

#ifndef __CPU_LOCALS_H__
#define __CPU_LOCALS_H__

#include <stdbool.h>
#include <cpu/msr.h>

struct cpu_locals {
    uint8_t lapic_id;
};

static inline struct cpu_locals *locals_cpu_get() {
    return (struct cpu_locals *) rdmsr(0xc0000101);
}

static inline void locals_cpu_set(struct cpu_locals *local) {
    wrmsr(0xc0000101, (uint64_t) local);
}

#endif
