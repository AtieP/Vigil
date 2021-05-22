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

#ifndef __TOOLS_PANIC_H__
#define __TOOLS_PANIC_H__

#include <misc/kcon.h>

#define panic(module, message, ...) \
    kcon_log(KCON_LOG_NONE, "panic", message "\nLocation: %s:%s:%s:%d", ##__VA_ARGS__, module, __func__, __FILE__, __LINE__); \
    for (;;) { asm volatile("1: cli\nhlt\njmp 1b"); } \
    __builtin_unreachable();

#endif
