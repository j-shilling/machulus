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

/* The framebuffer is really just an array of 2-byte entries. Visually it has 
   80 columns and 25 rows. */
static uint16_t *const framebuffer = (uint16_t *)FRAMEBUFFER_ADDR;
static const int FB_COLUMNS = 80;
static const int FB_ROWS    = 25;
/* Keep track of the current cursor position. Initialize to an invalid value so 
   that we know when to clear the screen. */
static int fb_cur_row = -1;
static int fb_cur_column = -1;

/* Styling Constants -- there's no need to be pretty, everything is grey on black */
static const uint8_t FB_COLOR_BLACK      = 0;
static const uint8_t FB_COLOR_LIGHT_GREY = 7;

/* The style part of a character is 1 byte: bits 0-4 are the foreground and the
   rest are background */
static const uint16_t FB_ENTRY_STYLE = FB_COLOR_LIGHT_GREY | (FB_COLOR_BLACK << 4);

/* It will be useful to have a constant value for a blank character, so we don't
   need to calculate it every time we clear the screen. This is just a space. */
static const uint16_t FB_BLANK_ENTRY = ' ' | (FB_ENTRY_STYLE << 8);

/* Convert a character to an entry. Assume the character is valid ascii */
static uint16_t 
fb_entry(int c) 
{
  return ((uint16_t)c) | (FB_ENTRY_STYLE << 8);
}

/* Updated the cursor by applying the given changes to the row and column. This
   should also handle scrolling. Return 0 on success and -1 on failure. */
static int
fb_update_cursor(int delta_x, int delta_y)
{
  /* Do the update */
  fb_cur_column += delta_x;
  fb_cur_row    += delta_y;
  
  /* Check if we moved to the previous lines */
  if (fb_cur_column < 0)
    {
      fb_cur_row    -= (-fb_cur_column / FB_COLUMNS) + 1;
      fb_cur_column  = FB_COLUMNS + (fb_cur_column % FB_COLUMNS);
    }
  /* Check if we moved to the next lines */
  else if (fb_cur_column >= FB_COLUMNS)
    {
      fb_cur_row    += (fb_cur_column / FB_COLUMNS) + 1;
      fb_cur_column %= FB_COLUMNS;
    }
  
  /* TODO: Update row and handle scrolling */
  
  return -1;
}

/* Fill the framebuffer with blanks */
static int
fb_clear(void)
{
  /* Add blank entries to framebuffer */
  for (int i = 0; i < (FB_COLUMNS * FB_ROWS); i++) 
    framebuffer[i] = FB_BLANK_ENTRY;
  return 0;
}

/* Initialize the framebuffer. Return 0 on success and -1 on failure. */
static int
fb_init(void)
{
  if (fb_cur_column >= 0)
    return 0;
  
  fb_cur_column = 0;
  fb_cur_row = 0;
  
  fb_clear();
  
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
  
  return 0;
}

/* Write the character to the screen. Return 0 on success and -1 on failure */
static int
fb_write(int c)
{
  framebuffer[(fb_cur_row * FB_COLUMNS) + fb_cur_column] = fb_entry(c);
  return fb_update_cursor(1, 0);
}

/* This is the only public interface with the framebuffer. It needs decide when
   to initialize the module, validate the parameter, and call fb_write. */
int
framebuffer_putchar(int c)
{
  if (fb_init())
    return -1;
  
  /* If this is a printable character (32-127), simply print to the screen. */
  if (c >= 32 && c <= 127)
    {
      return fb_write(c);
    }
  
  /* Handle control characters (only treat this subset as valid) */
  switch (c)
    {
    /* Bell: Do nothing */
    case '\a':
      return c;
    /* Null: Do nothing */
    case '\0':
      return c;
    /* Backspace: Move the cursor back one */
    case '\b':
      return fb_update_cursor(-1, 0);
    /* Tab: Move horizontally to the next tab stop */
    case '\t':
      return fb_update_cursor(8 - (fb_cur_column % 8), 0);
    /* Line feed: Move to the beginning of the next line */
    case '\n':
      return fb_update_cursor(0, 1);
    /* Form Feed: Clear the screen */
    case '\f':
      return fb_clear();
    /* Carriage Return: Move to the beginning of the current line */
    case '\r':
      return fb_update_cursor(-fb_cur_column, 0);
    /* Anything else is invalid. */
    default:
        return -1;
    }
}