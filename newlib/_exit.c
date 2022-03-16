#include <machulus/panic.h>

__attribute__((noreturn))
void _exit(int status) {
    panic_exit(status);
    __builtin_unreachable();
}
