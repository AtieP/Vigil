#ifndef __TOOLS_ASSERT_H__
#define __TOOLS_ASSERT_H__

#include <stddef.h>

__attribute__((__noreturn__)) void assert_fail(const char *assertion, const char *module, const char *message, const char *function, const char *filename, size_t line);

#define assert(assertion, module, message) \
    ((void) ((assertion) || (assert_fail(#assertion, module, message, __func__, __FILE__, __LINE__), 0)));

#endif
