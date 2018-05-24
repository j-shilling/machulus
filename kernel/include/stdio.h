#pragma once

#include <sys/cdefs.h>

#include <stddef.h>

#define EOF	  (-1)

__BEGIN_DECLS

typedef int	  FILE;

int fputc (int, FILE *);

#define fputs(x,y)	__builtin_fputs (x, y)
#define putc(x)		fputc (x, NULL)
#define putchar(x)	__builtin_putchar (x)
#define puts(x)		__builtin_puts (x)

#define printf(x,...)	__builtin_printf (x, ##__VA_ARGS__)
#define vprintf(x,y)	__builtin_vprintf (x, y)

__END_DECLS
