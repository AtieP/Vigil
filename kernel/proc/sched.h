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

#include <stdint.h>
#include <cpu/idt.h>
#include <tools/nummap.h>

#define SCHED_KERNEL_PID 0
#define SCHED_STACK_SIZE 2097152

typedef signed long pid_t;
typedef signed long tid_t;

struct sched_process {
    pid_t pid;
    uint64_t pagemap;
    struct nummap threads;
    struct nummap fds;
};

struct sched_thread {
    bool running;
    int errno;
    tid_t tid;
    struct sched_process *process;
    struct interrupt_frame gprs;
    uintptr_t stack_bottom;
    uintptr_t stack_top;
};

pid_t sched_get_pid();
tid_t sched_get_tid();
pid_t sched_new_process();
tid_t sched_new_thread(pid_t pid, void *(*start)(void *));
struct sched_process *sched_get_process_info(pid_t pid);
struct sched_thread *sched_get_thread_info(pid_t pid, tid_t tid);

#endif
