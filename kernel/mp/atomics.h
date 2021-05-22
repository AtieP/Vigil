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

#ifndef __MP_ATOMICS_H__
#define __MP_ATOMICS_H__

#include <stdbool.h>

#define ATOMIC_INC(value) { \
    asm volatile( \
        "lock inc %0" \
        :: "m"(value) \
    ); \
}

#define ATOMIC_DEC(value) { \
    asm volatile( \
        "lock dec %0" \
        :: "m"(value) \
    ); \
}

#endif
