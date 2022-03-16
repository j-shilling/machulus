#pragma once

#include <sys/types.h>
#include <stddef.h>

void serial_init(void);
ssize_t serial_write(const void *, size_t);
