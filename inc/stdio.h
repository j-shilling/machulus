#pragma once

#include <stddef.h>

typedef struct FILE FILE;

extern FILE *stdout;

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
