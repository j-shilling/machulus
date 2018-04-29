#include <kernel/tty.h>

#include <stddef.h>
#include <string.h>

#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t *) 0xB8000;

static size_t tty_row;
static size_t tty_col;
static uint8_t tty_color;
static uint16_t *tty_buf;

void __attribute__ ((constructor))
tty_init (void)
{
  tty_row = 0;
  tty_col = 0;
  tty_color = vga_entry_color (VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
  tty_buf = VGA_MEMORY;

  for (size_t y = 0; y < VGA_HEIGHT; y ++)
    {
      for (size_t x = 0; x < VGA_WIDTH; x++)
	{
	  const size_t index = y * VGA_WIDTH + x;
	  tty_buf[index] = vga_entry (' ', tty_color);
	}
    }

  tty_putchar (':');
  tty_putchar (')');
}

void
tty_putchar (char c)
{
  switch (c)
    {
    case '\n':
	{
	  tty_row ++;
	  tty_col = 0;
	  break;
	}

    default:
	{
	  const size_t index = tty_row * VGA_WIDTH + tty_col;
	  tty_buf[index] = vga_entry (c, tty_color);
	  tty_col ++;
	  break;
	}
    }

  if (tty_col >= VGA_WIDTH)
    {
      tty_col = 0;
      tty_row ++;
    }

  if (tty_row >= VGA_HEIGHT)
    {
      tty_col = 0;
      tty_row = 0;
    }
}
