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
#include <mp/atomics.h>
#include <mp/mutex.h>

// todo: make our own __sync_bool_compare_and_swap

void mutex_lock(struct mutex *mutex) {
    ATOMIC_INC(mutex->refcount);
    while (!__sync_bool_compare_and_swap(&mutex->locked, false, true));
    ATOMIC_DEC(mutex->refcount);
}

void mutex_unlock(struct mutex *mutex) {
    __sync_bool_compare_and_swap(&mutex->locked, true, false);
}
