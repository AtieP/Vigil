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

#ifndef __TOOLS_ASSERT_H__
#define __TOOLS_ASSERT_H__

#include <stddef.h>

__attribute__((__noreturn__)) void assert_fail(const char *assertion, const char *module, const char *message, const char *function, const char *filename, size_t line);

#define assert(assertion, module, message) \
    ((void) ((assertion) || (assert_fail(#assertion, module, message, __func__, __FILE__, __LINE__), 0)));

#endif
