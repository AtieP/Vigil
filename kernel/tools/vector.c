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
#include <mm/mm.h>
#include <mp/mutex.h>
#include <tools/builtins.h>
#include <tools/vector.h>

void vector_create(struct vector *vector) {
    vector->data = NULL;
    vector->size = 0;
}

void vector_push(struct vector *vector, void *data, size_t data_size) {
    mutex_lock(&vector->mutex);
    void *ptr = kheap_realloc(vector->data, vector->size + data_size, vector->size);
    memcpy(ptr + vector->size, data, data_size);
    vector->data = ptr;
    vector->size += data_size;
    mutex_unlock(&vector->mutex);
}

void vector_delete(struct vector *vector) {
    kheap_free(vector->data);
}
