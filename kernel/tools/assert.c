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

#include <stddef.h>
#include <misc/kcon.h>
#include <tools/assert.h>

__attribute__((__noreturn__)) void assert_fail(const char *assertion, const char *module, const char *message, const char *function, const char *filename, size_t line) {
    if (message) {
        kcon_log(KCON_LOG_NONE, "assert", "Assertion %s failed\nLocation: %s:%s:%s:%d (%s)", assertion, module, function, filename, line, message);
    } else {
        kcon_log(KCON_LOG_NONE, "assert", "Assertion %s failed\nLocation: %s:%s:%s:%d", assertion, module, function, filename, line);
    }
    __builtin_unreachable();
}