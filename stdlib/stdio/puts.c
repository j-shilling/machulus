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

/* file: stdlib/stdio/puts.c
 * 
 *   Implement puts() which prints a string followed by a newline. On success, 
 * it returns a nonnegative number; on failure, it returns EOF and set errno. */

#include <stdio.h>

int 
puts (const char *s)
{
  for (char *cur = (char *)s; (*cur) != '\0'; cur ++) 
    {
      if (putchar(*cur) == EOF)
        {
          /* errno is set by putchar. */
          return EOF;
        }
    }
  
  /* Now print the newline */
  if (putchar('\n') == EOF)
    return EOF;
  else
    return 0;
}