#pragma once

__attribute__((noreturn))
void panic(const char *, ...)
    __attribute__((format (printf, 1, 2)));

__attribute__((noreturn))
void panic_kill(int, int);

__attribute__((noreturn))
void panic_exit(int);
