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

#ifndef __TOOLS_NUMMAP_H__
#define __TOOLS_NUMMAP_H__

#include <stdbool.h>
#include <stddef.h>
#include <proc/mutex.h>

struct nummap_entry {
    size_t number;
    void *data;
    struct nummap_entry *next;
};

struct nummap {
    size_t entries;
    struct nummap_entry *first;
    struct mutex mutex;
};

void nummap_create(struct nummap *nummap);
bool nummap_insert(struct nummap *nummap, size_t number, void *data);
bool nummap_remove(struct nummap *nummap, size_t number);
size_t nummap_get_entries(struct nummap *nummap);
void *nummap_get_by_number(struct nummap *nummap, size_t number);
void *nummap_get_by_index(struct nummap *nummap, size_t index);
void nummap_delete(struct nummap *nummap);
size_t nummap_add(struct nummap *nummap, void *data, bool *added);

#endif
