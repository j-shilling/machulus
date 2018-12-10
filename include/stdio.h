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
  
#define putc(x)     fputc(x)
  
#define EOF         (-1)
  
  typedef struct FILE FILE;
  
  extern FILE *stdout;
  
  /* Writes a formated string to a file stream. Returns the number of bytes transmitted,
     or a negative number on error and sets errno. */
  int fprintf (FILE *, const char *, ...);
  
  /* Writes a byte specified by the int argument to the screen cast to an unsigned
     char. On success returns the byte written; on failure it returns EOF and sets
     errno. */
  int fputc (int, FILE *);
  
  /* Writes a null-terminated string to the file, not including the terminating
     byte. On success it returns a non-negative value; on failure it returns EOF
     and sets errno. */
  int fputs (const char *, FILE *);

  /* Print a formated string to the screen. On success return the number of
     bytes transmitted; on failure return a negative value. */
  int printf (const char *, ...);
  
  /* Output a single character to the screen. On success return the printed
     character; on failure return EOF and set errno. */
  int putchar (int);
  
  /* Prints a string followed by a newline. On success return a nonnegative number;
     on failure return EOF and set errno. */
  int puts (const char *);

  /* Writes a formated string to a memory buffer. */
  int snprintf (char *, size_t, const char *, ...);
  
  /* Writes a formated string to a file stream. Returns the number of bytes transmitted,
     or a negative number on error and sets errno. */
  int vfprintf (FILE *, const char *, va_list);
  
  /* Print a formated string to the screen. On success return the number of
     bytes transmitted; on failure return a negative value. */
  int vprintf (const char *, va_list);

  /* Writes a formated string to a memory buffer. */
  int vsnprintf (char *, size_t, const char *, va_list);
  

#ifdef __cplusplus
}
#endif

#endif /* STDIO_H */

