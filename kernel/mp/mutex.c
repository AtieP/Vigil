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
