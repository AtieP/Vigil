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

#ifndef __CPU_MSR_H__
#define __CPU_MSR_H__

#include <stdint.h>
#include <misc/kcon.h>

static inline void msr_check() {

}

static inline uint64_t rdmsr(uint32_t msr) {
    uint32_t eax;
    uint32_t edx;
    asm volatile("rdmsr" : "=a"(eax), "=d"(edx) : "c"(msr));
    return ((uint64_t) edx << 32) | eax;
}

static inline void wrmsr(uint32_t msr, uint64_t value) {
    uint32_t eax = (uint32_t) value;
    uint32_t edx = (uint32_t) (value >> 32);
    asm volatile("wrmsr" :: "a"(eax), "c"(msr), "d"(edx));
}

#endif
