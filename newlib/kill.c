#include <machulus/panic.h>

__attribute__((noreturn))
int kill(int pid, int sig) {
    panic_kill(pid, sig);
    __builtin_unreachable();
}
