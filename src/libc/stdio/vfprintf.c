#include <stdio.h>
#include <limits.h>

#include "printf-fspec.h"
#include "printf.h"

int
vfprintf (FILE *out, const char *__fmt, va_list ap)
{
  int ret = 0;
  
  for (char *fmt = (char *)__fmt; fmt && *fmt; fmt ++)
    {
      fputc (*fmt, out);
      ret ++;
    }
  
  return ret;
}
