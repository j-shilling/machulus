#include <stdio.h>

int
printf (const char * __fmt, ...)
{
  va_list ap;
  va_start (ap, __fmt);

  int ret = vprintf (__fmt, ap);

  va_end (ap);
  return ret;
}
