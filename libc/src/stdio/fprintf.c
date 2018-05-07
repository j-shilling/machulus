#include <stdio.h>

int
fprintf (FILE *out, const char * __fmt, ...)
{
  va_list ap;
  va_start (ap, __fmt);

  int ret = vfprintf (out, __fmt, ap);

  va_end (ap);
  return ret;
}
