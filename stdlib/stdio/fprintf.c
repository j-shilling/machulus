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

/* file: stdlib/stdio/fprintf.c
 * 
 *   Implements fprintf() which prints a formated string to a file. On success,
 * it returns the number of bytes transmitted; on failure, it return a negative 
 * value. */

#include <stdio.h>

int 
fprintf (FILE *stream, const char *fmt, ...)
{
  va_list ap;
  int done;
  
  va_start (ap, fmt);
  done = vfprintf(stream, fmt, ap);
  va_end(ap);
  
  return done;
}