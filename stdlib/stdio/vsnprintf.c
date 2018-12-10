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

struct char_stream
{
  struct FILE file;
  size_t size;
  char *buffer;
  size_t index;
};

static int
char_stream_putc (int c, FILE *stream)
{
  struct char_stream *string = (struct char_stream *)stream;

  if (string->index < string->size)
    string->buffer[string->index++] = (char) c;

  return c;
}

static int
char_stream_puts (const char *str, FILE *stream)
{
  struct char_stream *string = (struct char_stream *)stream;

  while ((string->index < string->size) && (*str != '\0'))
    string->buffer[string->index++] = (*str);

  return 0;
}

int
vsnprintf(char *buffer, size_t size, const char *fmt, va_list ap)
{
  struct char_stream string = { {char_stream_putc, char_stream_puts}, size, buffer, 0 };
  int done = vfprintf ((FILE *)(&string), fmt, ap);
  fputc ('\0', (FILE *)(&string));
  return done;
}
