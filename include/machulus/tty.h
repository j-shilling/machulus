#pragma once

#include <sys/types.h>
#include <stddef.h>

void tty_init(void);
ssize_t tty_write(const void *, size_t);
