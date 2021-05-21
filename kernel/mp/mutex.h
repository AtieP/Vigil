#ifndef __MP_MUTEX_H__
#define __MP_MUTEX_H__

#include <stdbool.h>
#include <stddef.h>

struct mutex {
    volatile bool locked;
    volatile size_t refcount;
};

void mutex_lock(struct mutex *mutex);
void mutex_unlock(struct mutex *mutex);

#endif
