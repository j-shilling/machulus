#ifndef PANIC_H
#define PANIC_H

void panic(const char *, ...) __attribute__((format(printf, 1, 2)));

#endif /* PANIC_H */
