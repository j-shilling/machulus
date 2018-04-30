
#include "printf-fspec.h"

#include <string.h>

#define __read_dec(fmt,i)               \
({                                      \
    __label__ done;                     \
    int result = 0;                     \
                                        \
    for (;;)                            \
      {                                 \
        switch (fmt[i])                 \
          {                             \
          case '0':                     \
            result = result * 10 ;      \
            break;                      \
          case '1':                     \
            result = result * 10 + 1;   \
            break;                      \
          case '2':                     \
            result = result * 10 + 2;   \
            break;                      \
          case '3':                     \
            result = result * 10 + 3;   \
            break;                      \
          case '4':                     \
            result = result * 10 + 4;   \
            break;                      \
          case '5':                     \
            result = result * 10 + 5;   \
            break;                      \
          case '6':                     \
            result = result * 10 + 6;   \
            break;                      \
          case '7':                     \
            result = result * 10 + 7;   \
            break;                      \
          case '8':                     \
            result = result * 10 + 8;   \
            break;                      \
          case '9':                     \
            result = result * 10 + 9;   \
            break;                      \
          default:                      \
            goto done;                  \
          }                             \
                                        \
        i++;                            \
        if (fmt[i] == '\0')             \
          goto done;                    \
      }                                 \
                                        \
    done:                               \
      result;                           \
})

int
printf_parse_fspec (const char *__fmt, size_t *i, struct fspec *fspec)
{
  size_t index = *i;
  
  if (__fmt[index] == '\0' || __fmt[index] != '%')
    return -1; /* Cannot parse if this does not point to a '%' */

  /* Initialize fspec */
  memset (fspec, 0, sizeof (struct fspec));
  
  index++;

  /* Get Flags Field */
  for (;;)
    {
      switch (__fmt[index])
        {
        case '-':
          fspec->left = 1;
          break;
        case '+':
          fspec->plus = 1;
          break;
        case ' ':
          fspec->space = 1;
          break;
        case '0':
          fspec->zero = 1;
          break;
        case '#':
          fspec->alt = 1;
          break;
        default:
          goto width;
        }
      index++;
      if (__fmt[index] == '\0')
        return -1;
    }

width:
  /* Get width field */
  fspec->width = __read_dec (__fmt, index);
  if (__fmt[index] == '\0')
    return -1;

  /* Get precision field */
  if (__fmt[index] == '.')
    {
      index++;
      if (__fmt[index] == '\0')
        return -1;
      else if (__fmt[index] == '*')
        {
          fspec->prec = -1;
          index++;
        }
      else
        {
          fspec->prec = __read_dec (__fmt, index);
        }
    }

  /* Length Field */
  switch (__fmt[index])
    {
    case 'h':
      if (__fmt[index + 1] == 'h')
        {
          fspec->is_char = 1;
          index += 2;
        }
      else
        {
          fspec->is_short = 1;
          index++;
        }
      break;
    case 'l':
      if (__fmt[index + 1] == 'l')
        {
          fspec->is_long_long = 1;
          index += 2;
        }
      else
        {
          fspec->is_long = 1;
          index++;
        }
      break;
    case 'L':
      fspec->is_long_double = 1;
      index++;
      break;
    case 'z':
      fspec->is_size_t = 1;
      index++;
      break;
    case 'j':
      fspec->is_intmax_t = 1;
      index++;
      break;
    case 't':
      fspec->is_ptrdiff_t = 1;
      index++;
      break;
    case '\0':
      return -1;
    default:
      break;
    }

  /* Type field */
  fspec->type = __fmt[index];
  
  *i = index + 1;

  return 0;
}