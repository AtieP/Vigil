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
#include <proc/mutex.h>
#include <tools/builtins.h>
#include <tools/vector.h>

void vector_create(struct vector *vector, size_t item_size) {
    vector->data = NULL;
    vector->items = 0;
    vector->item_size = item_size;
    vector->mutex.locked = false;
}

size_t vector_get_items(struct vector *vector) {
    mutex_lock(&vector->mutex);
    size_t entries = vector->items;
    mutex_unlock(&vector->mutex);
    return entries;
}

void *vector_get(struct vector *vector, size_t index) {
    mutex_lock(&vector->mutex);
    void *data;
    if (index > vector->items) {
        data = NULL;
    } else {
        data = vector->data + (index * vector->item_size);
    }
    mutex_unlock(&vector->mutex);
    return data;
}

bool vector_push(struct vector *vector, void *data) {
    mutex_lock(&vector->mutex);
    size_t vector_size = vector->items * vector->item_size;
    void *ptr = kheap_realloc(vector->data, vector_size + vector->item_size, vector_size);
    if (!ptr) {
        mutex_unlock(&vector->mutex);
        return false;
    }
    memcpy(ptr + vector_size, data, vector->item_size);
    vector->data = ptr;
    vector->items++;
    mutex_unlock(&vector->mutex);
    return true;
}

void vector_remove(struct vector *vector, size_t index) {
    // just memcpy back
    mutex_lock(&vector->mutex);
    if (index < vector->items) {
        memcpy(vector->data + (index * vector->item_size), vector->data + ((index + 1) * vector->item_size), vector->item_size * vector->items - (index * vector->item_size));
    }
    mutex_unlock(&vector->mutex);
}

void vector_delete(struct vector *vector) {
    kheap_free(vector->data);
}
