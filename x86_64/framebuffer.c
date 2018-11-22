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

#include <framebuffer.h>

#include <stdint.h>
#include <ldscript.h>

/* The framebuffer is really just an array of 2-byte entries. Visually it has 
   80 columns and 25 rows. */
static uint16_t * const framebuffer = (uint16_t *) (0xb8000 + KERNEL_OFFSET);
static const int FB_COLUMNS = 80;
static const int FB_ROWS = 25;

/* Keep track of the current cursor position. Initialize to an invalid value so 
   that we know when to clear the screen. */
static int index = -1;

/* The style part of a character is 1 byte: bits 0-4 are the foreground and the
   rest are background. Light Grey on Black */
static const uint16_t FB_ENTRY_STYLE = 0x07;

/* It will be useful to have a constant value for a blank character, so we don't
   need to calculate it every time we clear the screen. This is just a space. */
static const uint16_t FB_BLANK_ENTRY = ' ' | (FB_ENTRY_STYLE << 8);

/* Return the index of our cursor within its row */
static inline int
fb_cur_column (void)
{
  return index % FB_COLUMNS;
}

/* Fill the framebuffer with blanks */
static inline void
fb_clear (void)
{
  /* Add blank entries to framebuffer */
  for (int i = 0; i < (FB_COLUMNS * FB_ROWS); i++)
    framebuffer[i] = FB_BLANK_ENTRY;
}

/* This is the only public interface with the framebuffer. It needs decide when
   to initialize the module, validate the parameter, and call fb_write. */
int
framebuffer_putchar (int c)
{
  if (index < 0)
    {
      index = 0;
      fb_clear ();

      /* Disable cursor */
      asm (
           "pushf;"
#if __32bits__
           "push %eax;"
           "push %edx;"
#else
           "push %rax;"
           "push %rdx;"
#endif

           "mov  $0x3d4,%dx;"
           "mov  $0xa,%al;"
           "out  %al,%dx;"

           "inc  %dx;"
           "mov  $0x20,%al;"
           "out  %al,%dx;"

#if __32bits__
           "pop  %edx;"
           "pop  %eax;"
#else
           "pop  %rdx;"
           "pop  %rax;"
#endif
           "popf;"
           );
    }

    /* If this is a printable character (32-127), simply print to the screen. */
  if (c >= 32 && c <= 127)
    {
      framebuffer[index++] = ((uint16_t) c) | (FB_ENTRY_STYLE << 8);
    }
    /* Handle backspace. Simply move the cursor back on character. Don't move
     * back past to the previous line. */
  else if (c == '\b')
    {
      if (fb_cur_column () > 0)
        index--;
    }
    /* Handle horizontal tabs. Move to the next tab stop (every 8 columns). If 
     * we are at the end of the current line, simply move to the next. */
  else if (c == '\t')
    {
      index++; /* Always advance at least once */
      if (fb_cur_column () > 0)
        index += 7 - (fb_cur_column () % 8);
    }
    /* Handle newlines. Move to the beginning of the next line. */
  else if (c == '\n')
    {
      index += FB_COLUMNS - fb_cur_column ();
    }
    /* Handle form feed. Clear the screen. */
  else if (c == '\f')
    {
      fb_clear ();
    }
    /* Handle carriage returns. Move to the beginning of the current line. */
  else if (c == '\r')
    {
      index -= fb_cur_column ();
    }
    /* Handle invalid characters. Return -1 */
  else
    {
      return -1;
    }
  
  /* Now that index has been updated we need to check whether to scroll the screen
   * to make a new blank line. */
  
  if (index == (FB_COLUMNS * FB_ROWS))
    {
      /* Set index to the start of the second to last row */
      index = FB_COLUMNS * (FB_ROWS - 1);
      
      /* Shift the framebuffer up one row. */
      for (int i = 0; i < index; i ++)
        framebuffer[i] = framebuffer[i + FB_COLUMNS];
      
      /* Clear the last line. */
      for (int i = index; i < (FB_COLUMNS * FB_ROWS); i++)
        framebuffer[i] = FB_BLANK_ENTRY;
    }

  return 0;
}
