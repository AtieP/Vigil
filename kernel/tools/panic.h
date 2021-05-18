#ifndef __TOOLS_PANIC_H__
#define __TOOLS_PANIC_H__

#include <misc/kcon.h>

#define panic(module, message, ...) \
    kcon_log(KCON_LOG_NONE, "panic", message "\nLocation: %s:%s:%s:%d", ##__VA_ARGS__, module, __func__, __FILE__, __LINE__); \
    for (;;) {} \
    __builtin_unreachable();

#endif
