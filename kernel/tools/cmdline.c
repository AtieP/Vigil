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

#include <stdbool.h>
#include <tools/builtins.h>
#include <tools/cmdline.h>

bool cmdline_is_present(const char *cmdline, const char *arg) {
    size_t arg_len = strlen(arg);
    while (*cmdline) {
        if (*cmdline == *arg) {
            if (!strncmp(arg, cmdline, arg_len)) {
                return true;
            }
        }
        cmdline++;
    }
    return false;
}
