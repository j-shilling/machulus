#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include <machulus/panic.h>

__attribute__((noreturn))
__attribute__((naked))
static void halt() {
    asm ("cli");
    asm ("hlt");
}

__attribute__((noreturn))
__attribute__((format (printf, 1, 2)))
void panic(const char * fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    halt();
    __builtin_unreachable();
}

__attribute__((noreturn))
void panic_kill(int __attribute__((unused)) pid, int sig) {
    panic("Kernel killed with signal: %s\n", strsignal(sig));
    __builtin_unreachable();
}

__attribute__((noreturn))
void panic_exit(int status) {
    if (status == EXIT_SUCCESS) {
        panic("Kernel exited with status: success\n");
    } else {
        panic("Kernel exited with status: %s\n", strerror(status));
    }

    __builtin_unreachable();
}
