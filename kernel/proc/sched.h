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

#ifndef __PROC_SCHED_H__
#define __PROC_SCHED_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <cpu/idt.h>
#include <proc/mutex.h>
#include <tools/vector.h>

typedef size_t pid_t;
typedef size_t tid_t;

struct sched_process {
    bool supervisor;
    pid_t pid;
    struct vector threads;
};

struct sched_thread {
    tid_t tid;
    struct interrupt_frame gprs;
    uint64_t cr3;
};

__attribute__((__noreturn__)) void sched_init(uintptr_t address);
tid_t sched_new_kernel_thread(uintptr_t address);

#endif
