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

#include <stdio.h>
#include <file.h>
#include <errno.h>

int
fputs (const char *s, FILE *stream)
{
  /* Sanity check params */
  if (NULL == stream || NULL == s)
    {
      errno = EINVAL;
      return EOF;
    }
  
  /* If a specific implementation of fputs was given, use that. */
  if (stream->fputs)
    {
      return stream->fputs(s, stream);
    }
  /* Otherwise, use fputc to write to the file. */
  else if (stream->fputc)
    {
      for (const char *cur = s; '\0' != (*cur); cur++)
        {
          if (EOF == fputc ((*cur), stream))
            return EOF;
        }
      
      return 0;
    }
  
  /* If we get here, this is an invalid FILE */
  errno = EBADF;
  return EOF;
}