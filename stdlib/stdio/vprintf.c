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

/* file: stdlib/stdio/vprintf.c
 * 
 *   Implements vprintf() which prints a formated string to the screen. On success,
 * it returns the number of bytes transmitted; on failure, it return a negative 
 * value. */

/* This is a shit implementation of vprintf. Its not thread safe and its
   inefficient. This is not a high priority though since it will really only
   be used for kernel panics and debugging. */

#include <stdio.h>

#define buffer_size 512
static char buffer[buffer_size];

int 
vprintf (const char *fmt, va_list ap)
{
  int ret = vsnprintf(buffer, buffer_size, fmt, ap);
  if (ret < 0)
    return ret;
  
  ret = 0;
  for (size_t i = 0; i < buffer_size && buffer[i] != '\0'; i ++)
    {
      if (EOF == putchar (buffer[i]))
        return EOF;
      ret ++;
    }
  
  return ret;
}