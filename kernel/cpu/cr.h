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

#ifndef __CPU_CR_H__
#define __CPU_CR_H__

#include <stdint.h>

static inline uint64_t read_cr0() {
    uint64_t ret;
    asm volatile("mov %%cr0, %0" : "=r"(ret));
    return ret;
}

static inline uint64_t read_cr2() {
    uint64_t ret;
    asm volatile("mov %%cr2, %0" : "=r"(ret));
    return ret;
}

static inline uint64_t read_cr3() {
    uint64_t ret;
    asm volatile("mov %%cr3, %0" : "=r"(ret));
    return ret;
}

static inline uint64_t read_cr4() {
    uint64_t ret;
    asm volatile("mov %%cr4, %0" : "=r"(ret));
    return ret;
}

static inline uint64_t read_cr8() {
    uint64_t ret;
    asm volatile("mov %%cr8, %0" : "=r"(ret));
    return ret;
}

static inline void write_cr0(uint64_t value) {
    asm volatile("mov %0, %%cr0" :: "r"(value));
}

static inline void write_cr3(uint64_t value) {
    asm volatile("mov %0, %%cr3" :: "r"(value));
}

static inline void write_cr4(uint64_t value) {
    asm volatile("mov %0, %%cr4" :: "r"(value));
}

static inline void write_cr8(uint64_t value) {
    asm volatile("mov %0, %%cr8" :: "r"(value));
}

#endif
