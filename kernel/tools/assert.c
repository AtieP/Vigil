#include <stddef.h>
#include <misc/kcon.h>
#include <tools/assert.h>

__attribute__((__noreturn__)) void assert_fail(const char *assertion, const char *module, const char *message, const char *function, const char *filename, size_t line) {
    kcon_log(KCON_LOG_NONE, "assert", "Assertion %s failed\n Location: %s:%s:%s:%d (%s)", module, assertion, function, filename, line, message);
    __builtin_unreachable();
}