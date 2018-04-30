#ifndef _STDIO_H_
#define _STDIO_H_

#include <sys/cdefs.h>

#include <stddef.h> /* size_t */
#include <stdarg.h> /* va_arg */

#ifndef EOF
# define EOF (-1)
#endif

__BEGIN_DECLS

/* Types */
typedef int		FILE; /* to-do */

/* Variables */
extern FILE *const stdin;
extern FILE *const stdout;
extern FILE *const stderr;

/* Unformated output */
int fputc		(int, FILE *);
int fputs		(const char *, FILE *);

#define putc(x,y)	fputc (x, y)
#define putchar(x)	fputc (x, stdout)

int puts		(const char *);

/* Formatted output */
int __attribute__ ((format (printf, 1, 2))) printf (const char *,...);
int __attribute__ ((format (printf, 2, 3))) fprintf (FILE *, const char *, ...);

int vprintf (const char *, va_list);
int vfprintf (FILE *, const char *, va_list);

__END_DECLS

#endif /* _STDIO_H_ */
