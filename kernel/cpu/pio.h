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

#ifndef __CPU_PIO_H__
#define __CPU_PIO_H__

#include <stdint.h>

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("in %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    asm volatile("in %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline uint32_t ind(uint16_t port) {
    uint32_t ret;
    asm volatile("in %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t data) {
    asm volatile("out %0, %1" :: "a"(data), "Nd"(port));
}

static inline void outw(uint16_t port, uint16_t data) {
    asm volatile("out %0, %1" :: "a"(data), "Nd"(port));
}

static inline void outd(uint16_t port, uint32_t data) {
    asm volatile("out %0, %1" :: "a"(data), "Nd"(port));
}

#endif
