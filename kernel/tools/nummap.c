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

#include <mm/mm.h>
#include <proc/mutex.h>
#include <tools/nummap.h>

void nummap_create(struct nummap *nummap) {
    nummap->first = NULL;
    nummap->mutex.locked = false;
}

void nummap_insert(struct nummap *nummap, size_t number, void *data) {
    mutex_lock(&nummap->mutex);
    struct nummap_entry *entry = nummap->first;
    if (!entry) {
        nummap->first = kheap_calloc(sizeof(struct nummap_entry));
        nummap->first->number = number;
        nummap->first->data = data;
        mutex_unlock(&nummap->mutex);
        return;
    }
    while (entry->next) {
        entry = entry->next;
    }
    entry->next = kheap_calloc(sizeof(struct nummap_entry));
    entry->next->number = number;
    entry->next->data = data;
    mutex_unlock(&nummap->mutex);
}

bool nummap_remove(struct nummap *nummap, size_t number) {
    mutex_lock(&nummap->mutex);
    if (!nummap->first) {
        mutex_unlock(&nummap->mutex);
        return false;
    }
    struct nummap_entry *entry;
    struct nummap_entry *entry2;
    if (nummap->first->number == number) {
        entry = nummap->first;
        nummap->first = nummap->first->next;
        kheap_free(entry);
        mutex_unlock(&nummap->mutex);
        return true;
    } 
    entry = nummap->first;
    while (entry->next) {
        if (entry->next->number == number) {
            entry2 = entry->next;
            entry->next = entry->next->next;
            kheap_free(entry2);
            mutex_unlock(&nummap->mutex);
            return true;
        }
        entry = entry->next;
    }
    mutex_unlock(&nummap->mutex);
    return false;
}

void *nummap_get(struct nummap *nummap, size_t number) {
    mutex_lock(&nummap->mutex);
    struct nummap_entry *entry = nummap->first;
    if (!entry) {
        return NULL;
    }
    while (entry) {
        if (entry->number == number) {
            mutex_unlock(&nummap->mutex);
            return entry->data;
        }
        entry = entry->next;
    }
    mutex_unlock(&nummap->mutex);
    return NULL;
}

size_t nummap_add(struct nummap *nummap, void *data) {
    mutex_lock(&nummap->mutex);
    size_t lowest_unused_number = 0;
    struct nummap_entry *entry = nummap->first;
    if (!entry) {
        nummap->first = kheap_calloc(sizeof(struct nummap_entry));
        nummap->first->number = lowest_unused_number;
        nummap->first->data = data;
        mutex_unlock(&nummap->mutex);
        return lowest_unused_number;
    }
    while (entry->next) {
        if (lowest_unused_number >= entry->number) {
            lowest_unused_number = entry->number + 1;
        }
        entry = entry->next;
    }
    entry->next = kheap_calloc(sizeof(struct nummap_entry));
    entry->next->number = lowest_unused_number;
    entry->next->data = data;
    mutex_unlock(&nummap->mutex);
    return lowest_unused_number;
}
