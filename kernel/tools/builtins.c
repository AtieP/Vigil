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
#include <stdint.h>
#include <tools/builtins.h>

void *memset(void *s, int c, size_t n) {
    uint8_t *s_uint8 = (uint8_t *) s;
    for (size_t i = 0; i < n; i++) {
        s_uint8[i] = (uint8_t) c;
    }
    return s;
}

void *memcpy(void *dest, const void *src, size_t n) {
    const uint8_t *src_uint8 = (const uint8_t *) src;
    uint8_t *dest_uint8 = (uint8_t *) dest;
    for (size_t i = 0; i < n; i++) {
        dest_uint8[i] = src_uint8[i];
    }
    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    uint8_t *s1_uint8 = (uint8_t *) s1;
    uint8_t *s2_uint8 = (uint8_t *) s2;
    while (n - 1) {
        if (*s1_uint8 != *s2_uint8) {
            break;
        }
        s1_uint8++;
        s2_uint8++;
        n--;
    }
    return (int) (*s1_uint8 - *s2_uint8);
}

int strcmp(const char *s1, const char *s2) {
    while (*s1) {
        if (*s1 != *s2) {
            break;
        }
        s1++;
        s2++;
    }
    return (int) (*s1 - *s2);
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (n - 1) {
        if (*s1 != *s2) {
            break;
        }
        s1++;
        s2++;
        n--;
    }
    return (int) (*s1 - *s2);
}

size_t strlen(const char *s) {
    size_t n = 0;
    while (*s) {
        n++;
        s++;
    }
    return n;
}
