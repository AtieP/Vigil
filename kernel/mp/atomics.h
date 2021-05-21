#ifndef __MP_ATOMICS_H__
#define __MP_ATOMICS_H__

#include <stdbool.h>

#define ATOMIC_INC(value) { \
    asm volatile( \
        "lock inc %0" \
        :: "m"(value) \
    ); \
}

#define ATOMIC_DEC(value) { \
    asm volatile( \
        "lock dec %0" \
        :: "m"(value) \
    ); \
}

#endif
