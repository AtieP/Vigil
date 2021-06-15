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
#include <mm/mm.h>
#include <misc/kcon.h>
#include <proc/mutex.h>
#include <tools/nummap.h>

static void swap_nummap_entries(struct nummap_entry *entry1, struct nummap_entry *entry2) {
    kcon_log(KCON_LOG_NONE, "d", "%d %d", entry1->number, entry2->number);
    size_t tempnumber = entry1->number;
    struct nummap_entry *tempnext = entry1->next;
    entry1->number = entry2->number;
    entry1->data = entry2->data;
    entry2->number = tempnumber;
    entry2->data = tempnext;kcon_log(KCON_LOG_NONE, "d", "%d %d", entry1->number, entry2->number);
}

static void sort_nummap(struct nummap *nummap) {
    if (!nummap->first) {
        return;
    }
    bool swapped;
    struct nummap_entry *ptr1;
    struct nummap_entry *ptr2 = NULL;
    do {
        swapped = false;
        ptr1 = nummap->first;
        while (ptr1->next != ptr2) {
            if (ptr1->number > ptr1->next->number) {
                swap_nummap_entries(ptr1, ptr1->next);
                swapped = true;
            }
            ptr1 = ptr1->next;
        }
        ptr2 = ptr1;
    } while (swapped);
}

void nummap_create(struct nummap *nummap) {
    nummap->entries = 0;
    nummap->first = NULL;
    nummap->mutex.locked = false;
}

bool nummap_insert(struct nummap *nummap, size_t number, void *data) {
    mutex_lock(&nummap->mutex);
    struct nummap_entry *entry = nummap->first;
    if (!entry) {
        nummap->first = kheap_calloc(sizeof(struct nummap_entry));
        if (!nummap->first) {
            mutex_unlock(&nummap->mutex);
            return false;
        }
        nummap->first->number = number;
        nummap->first->data = data;
        nummap->entries++;
        mutex_unlock(&nummap->mutex);
        return true;
    }
    while (entry->next) {
        entry = entry->next;
    }
    entry->next = kheap_calloc(sizeof(struct nummap_entry));
    if (!entry->next) {
        mutex_unlock(&nummap->mutex);
        return false;
    }
    entry->next->number = number;
    entry->next->data = data;
    nummap->entries++;
    mutex_unlock(&nummap->mutex);
    return true;
}

bool nummap_remove(struct nummap *nummap, size_t number) {
    mutex_lock(&nummap->mutex);
    if (!nummap->first) {
        mutex_unlock(&nummap->mutex);
        return false;
    }
    struct nummap_entry *entry1 = nummap->first;
    struct nummap_entry *entry2;
    if (nummap->first->number == number) {
        entry2 = nummap->first;
        nummap->first = nummap->first->next;
        kheap_free(entry2);
        mutex_unlock(&nummap->mutex);
        return true;
    }
    if (entry1->next->number == number) {
        entry2 = entry1->next;
        entry1->next = entry1->next->next;
        kheap_free(entry2);
        mutex_unlock(&nummap->mutex);
        return true;
    }
    while (entry1->next) {
        if (entry1->next->number == number) {
            entry2 = entry1->next;
            entry1->next = entry1->next->next;
            kheap_free(entry2);
            mutex_unlock(&nummap->mutex);
            return true;
        }
        entry1 = entry1->next;
    }
    mutex_unlock(&nummap->mutex);
    return false;
}

size_t nummap_get_entries(struct nummap *nummap) {
    mutex_lock(&nummap->mutex);
    size_t entries = nummap->entries;
    mutex_unlock(&nummap->mutex);
    return entries;
}

void *nummap_get_by_number(struct nummap *nummap, size_t number) {
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

void *nummap_get_by_index(struct nummap *nummap, size_t index) {
    mutex_lock(&nummap->mutex);
    struct nummap_entry *entry = nummap->first;
    if (!entry) {
        return NULL;
    }
    size_t counter = 0;
    while (entry) {
        if (counter == index) {
            mutex_unlock(&nummap->mutex);
            return entry->data;
        }
        counter++;
        entry = entry->next;
    }
    mutex_unlock(&nummap->mutex);
    return NULL;
}

size_t nummap_add(struct nummap *nummap, void *data, bool *added) {
    mutex_lock(&nummap->mutex);
    size_t lowest_unused_number = ~1;
    struct nummap_entry *entry = nummap->first;
    if (!entry) {
        nummap->first = kheap_calloc(sizeof(struct nummap_entry));
        if (!nummap->first) {
            *added = false;
            mutex_unlock(&nummap->mutex);
            return 0;
        }
        lowest_unused_number = 0;
        entry = nummap->first; // update
        entry->number = lowest_unused_number;
        entry->data = data;
        *added = true;
        mutex_unlock(&nummap->mutex);
        return lowest_unused_number;
    }
    sort_nummap(nummap);
    // get number
    if (!entry->next) {
        if (entry->number == 0) {
            lowest_unused_number = entry->number + 1;
        } else {
            lowest_unused_number = entry->number - 1;
        }
    } else {
        while (entry->next) {
            if (entry->number < lowest_unused_number) {
                lowest_unused_number = entry->number + 2;
            }
            entry = entry->next;
        }
    }
    // create
    entry->next = kheap_calloc(sizeof(struct nummap_entry));
    if (!entry->next) {
        *added = false;
        mutex_unlock(&nummap->mutex);
        return lowest_unused_number;
    }
    entry->next->number = lowest_unused_number;
    entry->next->data = data;
    *added = true;
    mutex_unlock(&nummap->mutex);
    return lowest_unused_number;
}
