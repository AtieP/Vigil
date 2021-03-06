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

#ifndef __TOOLS_VECTOR_H__
#define __TOOLS_VECTOR_H__

#include <stddef.h>
#include <proc/mutex.h>

struct vector {
    void *data;
    size_t items;
    size_t item_size;
    struct mutex mutex;
};

void vector_create(struct vector *vector, size_t item_size);
void *vector_get(struct vector *vector, size_t index);
size_t vector_get_items(struct vector *vector);
bool vector_push(struct vector *vector, void *data);
void vector_remove(struct vector *vector, size_t index);
void vector_delete(struct vector *vector);

#endif
