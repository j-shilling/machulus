#include <stdio.h>
#include <limits.h>
#include <string.h>

#include "printf-fspec.h"
#include "printf.h"

int
vfprintf (FILE *out, const char *__fmt, va_list ap)
{
  int ret = 0;
  size_t index = 0;
  
  while (__fmt[index] != '\0')
    {
      if (__fmt[index] == '%')
        {
          struct __fspec fspec;
          memset (&fspec, 0, sizeof (struct __fspec));
          
          if (printf_parse_fspec (__fmt, &index, &fspec, ap))
            return -1;
          
          char *buf = __builtin_alloca (100);
          printf_expand_fspec (buf, 100, fspec);
          
          while (buf && *buf)
            {
              fputc (*buf, out);
              buf++;
              ret ++;
            }
        }
      else
        {
          fputc (__fmt[index], out);
          ret ++;
          index ++;
        }
    }
  
  return ret;
}
