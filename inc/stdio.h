#pragma once

#include <stddef.h>

typedef struct FILE FILE;

extern FILE *stdout;

size_t fwrite(const void *, size_t, size_t, FILE *);
size_t fwrite_unlocked(const void *, size_t, size_t, FILE *);
