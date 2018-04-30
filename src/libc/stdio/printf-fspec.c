#include "printf-fspec.h"

static int
__read_width_or_precision (const char *__fmt, size_t *i)
{
  size_t index = *i;
  int result = 0;

  for (;;)
    {
      switch (__fmt[index])
        {
        case '*': result = -1;
          goto done;
        case '0': result *= 10;
          break;
        case '1': result = result * 10 + 1;
          break;
        case '2': result = result * 10 + 2;
          break;
        case '3': result = result * 10 + 3;
          break;
        case '4': result = result * 10 + 4;
          break;
        case '5': result = result * 10 + 5;
          break;
        case '6': result = result * 10 + 6;
          break;
        case '7': result = result * 10 + 7;
          break;
        case '8': result = result * 10 + 8;
          break;
        case '9': result = result * 10 + 9;
          break;
        default: goto done;
        }

      index++;
    }

done:
  *i = index;
  return result;
}

int
printf_parse_fspec (const char *__fmt,
                    size_t *i,
                    struct __fspec *fspec,
                    va_list ap)
{
  if (!__fmt || fspec)
    return -1;

  size_t index = *i;
  if (__fmt[index] != '%')
    return -1;

  index++;

  /* Get Flags */
  int cont = 1;
  while (cont)
    {
      switch (__fmt[index])
        {
        case '-': fspec->flag.left = 1;
          break;
        case '+': fspec->flag.plus = 1;
          break;
        case ' ': fspec->flag.space = 1;
          break;
        case '0': fspec->flag.zero = 1;
          break;
        case '#': fspec->flag.alt = 1;
          break;
        default: cont = 0;
          break;
        }
      index++;
    }

  /* Width Field */
  fspec->width = __read_width_or_precision (__fmt, &index);
  if (fspec->width = -1)
    fspec->width = va_arg (ap, int);

  /* Precision Field */
  if (__fmt[index] == '.')
    {
      index++;
      fspec->prec = __read_width_or_precision (__fmt, &index);
      if (fspec->prec = -1)
        fspec->prec = va_arg (ap, int);
    }

  /* Length Modifier */
  switch (__fmt[index])
    {
    case 'h':
      {
        if (__fmt[index + 1] == 'h')
          {
            fspec->length.is_char = 1;
            index += 2;
          }
        else
          {
            fspec->length.is_short = 1;
            index += 1;
          }
        break;
      }
    case 'l':
      {
        if (__fmt[index + 1] == 'l')
          {
            fspec->length.is_long_long = 1;
            index += 2;
          }
        else
          {
            fspec->length.is_long = 1;
            index += 1;
          }
        break;
      }
    case 'L':
      {
        fspec->length.is_long_double = 1;
        index += 1;
        break;
      }
    case 'z':
      {
        fspec->length.is_size_t = 1;
        index += 1;
        break;
      }
    case 'j':
      {
        fspec->length.is_intmax_t = 1;
        break;
      }
    case 't':
      {
        fspec->length.is_ptrdiff_t = 1;
        break;
      }
    }

  fspec->arg.tag = __fmt[index++];
  *i = index;

  switch (fspec->arg.tag)
    {
      /* Integer Values */
    case 'd':
    case 'i':
    case 'u':
    case 'x':
    case 'X':
    case 'o':
    case 'c':
      {
        if (fspec->length.is_long)
          fspec->arg.long_val = va_arg (ap, long);
        else if (fspec->length.is_long_long)
          fspec->arg.long_long_val = va_arg (ap, long long);
        else if (fspec->length.is_size_t)
          fspec->arg.size_val = va_arg (ap, size_t);
        else if (fspec->length.is_intmax_t)
          fspec->arg.intmax_val = va_arg (ap, intmax_t);
        else if (fspec->length.is_ptrdiff_t)
          fspec->arg.ptrdiff_val = va_arg (ap, ptrdiff_t);
        else
          fspec->arg.int_val = va_arg (ap, int);
        break;
      }

      /* Floating point value */
    case 'f':
    case 'F':
    case 'e':
    case 'E':
    case 'g':
    case 'G':
    case 'a':
    case 'A':
      {
        if (fspec->length.is_long_double)
          fspec->arg.long_double_val = va_arg (ap, long double);
        else
          fspec->arg.double_val = va_arg (ap, double);
        break;
      }

      /* Pointer value */
    case 'p':
    case 's':
      {
        fspec->arg.void_p_val = va_arg (ap, void *);
        break;
      }
    
      /* Misc */
    case 'n':
      break;
      
      /* Error */
    default:
      return -1;
    }
  
  return 0;
}

int
printf_expand_fspec (char *buf,
                     size_t buf_size,
                     struct __fspec fspec)
{
  return -1;
}