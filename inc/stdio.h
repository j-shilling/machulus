#pragma once

#include <stddef.h>

#define EOF (-1)

typedef struct FILE FILE;

extern FILE *stdout;

size_t fwrite(const void *, size_t, size_t, FILE *);
size_t fwrite_unlocked(const void *, size_t, size_t, FILE *);

int fputc(int, FILE *);
int fputc_unlocked(int, FILE *);

int fputs(const char *, FILE *);
int fputs_unlocked(const char *, FILE *);

#define putc(c, stream) fputc(c, stream)
#define putc_unlocked(c, stream) fputc_unlocked(c, stream)

#define putchar(c) putc(c, stdout)
#define putchar_unlocked(c) putc_unlocked(c, stdout)

int puts(const char *);
int puts_unlocked(const char *);
