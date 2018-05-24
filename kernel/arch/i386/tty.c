#include <stddef.h>
#include <stdint.h>

#include <stdio.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

/* initialized in boot.S */
uint16_t *const video_memory;

static const uint16_t vga_blank_entry = ' ' | 7 << 8;

static uint16_t *tty_cur;
static uint16_t *tty_start;
static uint16_t *tty_end;

static inline uint16_t
vga_entry (unsigned char c)
{
  return (uint16_t) c | (uint16_t) 7 << 8;
}

static inline void
vga_scroll (void)
{
  uint16_t *cur;
  for (cur = tty_start; cur < (tty_end - VGA_WIDTH); cur ++)
    cur[0] = cur[1];

  tty_cur = tty_end - VGA_WIDTH;
  for (uint16_t *cur = tty_cur; cur < tty_end; cur ++)
    *cur = vga_blank_entry;
}

void
tty_init (void)
{
  /* Find vga memory boundries */
  tty_start = video_memory;
  tty_end = tty_start + (VGA_WIDTH * VGA_HEIGHT);

  /* Clear screen */
  for (tty_cur = tty_start; tty_cur < tty_end; tty_cur ++)
    *tty_cur = vga_blank_entry;

  tty_cur = tty_start;

  /* disable curser */
  asm (
    "pushf;"
    "push %eax;"
    "push %edx;"
    "mov $0x3D4, %dx;"
    "mov $0xA, %al;"
    "out %al, %dx;"
    "inc %dx;"
    "mov $0x20, %al;"
    "out %al, %dx;"
    "pop %edx;"
    "pop %eax;"
    "popf;"
  );

}

int
fputc (int c, FILE *stream __attribute__ ((unused)))
{
  if (c > 31) /* Printable Character */
    {
      *tty_cur = vga_entry (c);
      tty_cur ++;
    }
  else /* Control Character */
    {
      ptrdiff_t index = tty_cur - tty_start;
      size_t pos_in_line = index % VGA_WIDTH;

      switch (c)
	{
	case '\b':
	  if (pos_in_line > 0)
	    tty_cur --;
	  *tty_cur = vga_blank_entry;
	  break;

	case '\t':
	  while (pos_in_line % 8)
	    {
	      tty_cur ++;
	      pos_in_line ++;
	      *tty_cur = vga_blank_entry;
	    }
	  break;

	case '\n':
	  tty_cur += (VGA_WIDTH - pos_in_line);
	  break;

	case '\r':
	  tty_cur -= pos_in_line;
	  break;

	default:
	  return EOF; /* Unrecognized character */
	}
    }

  if (tty_cur == tty_end)
    vga_scroll();

  return c;
}

int
fputs (const char *s, FILE *stream __attribute__ ((unused)))
{
  int ret = 0;
  for (; (*s) != '\0'; s++)
    {
      if (EOF == fputc ((int) (*s), NULL))
	return EOF;
      ret ++;
    }
  return ret;
}

int
putchar (int c)
{
  return fputc (c, NULL);
}

int
puts (const char *s)
{
  int ret = fputs (s, NULL);
  if (EOF == ret)
    return EOF;

  if (EOF == fputc ('\n', NULL))
    return EOF;

  return ret + 1;
}
