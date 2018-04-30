#include <stdio.h>
#include <limits.h>

#include "printf-fspec.h"
#include "printf.h"

#define __putc(x)                                                   \
    do                                                              \
      {                                                             \
        const int __out_char = (int) x;                             \
        if (EOF == fputc (__out_char, out) || ret == INT_MAX)       \
          {                                                         \
            ret = -1;                                               \
            goto done;                                              \
          }                                                         \
        ret ++;                                                     \
      }                                                             \
    while (0)

int
vfprintf (FILE *out, const char *__fmt, va_list ap)
{

  int ret = 0;
  for (size_t i = 0; __fmt[i] != '\0'; i++)
    {
      if (__fmt[i] == '%')
        {
          struct fspec fspec;
          if (printf_parse_fspec (__fmt, &i, &fspec))
            break;
          
          if (fspec.width == -1)
            fspec.width = va_arg (ap, int);
          
          if (fspec.prec == -1)
            fspec.prec = va_arg (ap, int);
          
          char *text = __printf_parse (fspec, ap);
          while (text && *text)
            {
              __putc (*text);
              text ++;
            }
        }
      else
        {
          __putc (__fmt[i]);
        }
    }
  
done:
  return ret;

}
