#include <stdbool.h>
#include <tools/builtins.h>
#include <tools/cmdline.h>

bool cmdline_is_present(const char *cmdline, const char *arg) {
    size_t arg_len = strlen(arg);
    while (*cmdline) {
        if (*cmdline == *arg) {
            if (!strncmp(arg, cmdline, arg_len)) {
                return true;
            }
        }
        cmdline++;
    }
    return false;
}
