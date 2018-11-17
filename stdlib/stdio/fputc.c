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
fputc (int c, FILE *stream)
{
  if (NULL == stream)
    {
      errno = EINVAL;
      return EOF;
    }
  
  if (stream->fputc)
    return stream->fputc(c, stream);
  
  /* No stream->fputc was given */
  errno = EBADF;
  return EOF;
}