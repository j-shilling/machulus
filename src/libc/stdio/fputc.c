#include <stdio.h>

#if defined (__is_libk)
#  include <kernel/tty.h>
#endif

FILE *const stdin = NULL;
FILE *const stdout = NULL;
FILE *const stderr = NULL;

int fputc (int ch, FILE *stream) {
#if defined (__is_libk)
  if (tty_putchar ((unsigned char) ch))
    return EOF;
  else
    return ch;
#else
  return EOF; /* TO-Do */
#endif
}

