#include <stdio.h>

int
vprintf (const char *__fmt, va_list ap)
{
  return vfprintf (stdout, __fmt, ap);
}
