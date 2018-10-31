/*
 * Copyright (C) 2018 Jake Shilling
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* file: include/stdio.h
 * 
 *   This is a replacement for the stdio.h of libc. Only the the functions which
 * output to stdout or a string buffer are supported, but those functions will
 * be implemented in accordance with the C standard */

#ifndef STDIO_H
#define STDIO_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdarg.h>
  
#define EOF         (-1)

  /* Print a formated string to the screen. On success return the number of
     bytes transmitted; on failure return a negative value. */
#ifdef __GNUC__
#  define printf(__fmt, ...)  __builtin_printf(__fmt, ##__VA_ARGS__)
#else
  int printf (const char *, ...);
#endif
  
  /* Output a single character to the screen. On success return the printed
     character; on failure return EOF and set errno. */
#ifdef __GNUC__
#  define putchar(c) __builtin_putchar(c)
#else
  int putchar (int);
#endif
  
  /* Prints a string followed by a newline. On success return a nonnegative number;
     on failure return EOF and set errno. */
#ifdef __GNUC__
#  define puts(s)  __builtin_puts(s)
#else
  int puts (const char *);
#endif
  
  /* Print a formated string to the given buffer. Do not print more that the
     specified number of bytes. On success return the number of bytes transmitted;
     on failure return a negative value. */
#ifdef __GNUC__
#  define snprintf(__buf, __size, __fmt, ...) __builtin_snprintf(__buf, __size, __fmt, ##__VA_ARGS__)
#else
  int snprintf (char *, size_t, const char *, ...);
#endif
  
  /* Print a formated string to the given buffer. On success return the number
     of bytes transmitted; on failure return a negative value. */
#ifdef __GNUC__
#  define sprintf(__buf, __fmt, ...) __builtin_sprintf(__buf, __fmt, ##__VA_ARGS__
#else
  int sprintf (char *, const char *, ...);
#endif
  
  /* Print a formated string to the screen. On success return the number of
     bytes transmitted; on failure return a negative value. */
#ifdef __GNUC__
#  define vprintf(__fmt, __args)  __builtin_vprintf(__fmt, __args)
#else
  int vprintf (const char *, va_list);
#endif
  
  /* Print a formated string to the given buffer. Do not print more that the
     specified number of bytes. On success return the number of bytes transmitted;
     on failure return a negative value. */
#ifdef __GNUC__
#  define vsnprintf(__buf, __size, __fmt, __args) __builtin_vsnprintf(__buf, __size, __fmt, __args)
#else
  int vsnprintf (char *, size_t, const char *, va_list);
#endif
  
  /* Print a formated string to the given buffer. On success return the number
     of bytes transmitted; on failure return a negative value. */
#ifdef __GNUC__
#  define vsprintf(__buf, __fmt, __args) __builtin_vsprintf(__buf, __fmt, __args)
#else
  int vsprintf (char *, const char *, va_list);
#endif


#ifdef __cplusplus
}
#endif

#endif /* STDIO_H */

