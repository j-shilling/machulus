#include <stdio.h>
#include <stdarg.h>

int __attribute__ ((format (printf, 1, 2)))
printf (const char *format, ...)
{
  va_list ap;
  va_start (ap, format);

  int ret = vprintf (format, ap);

  va_end (ap);
  return ret;
}

int
vprintf (const char *format, va_list ap)
{
  int ret = 0;

#define __print(x)    do { if (EOF == putchar (x)) return EOF; else ret ++; } while (0)

  while ((*format) != '\0')
    {
      if ((*format) != '%')
	{
	  __print (*format);
	  format ++;
	}
      else
	{
	}
    }

#undef __print

  return ret;
}
