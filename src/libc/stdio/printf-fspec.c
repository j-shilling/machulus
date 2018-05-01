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
        case '*': result = -1; index ++;
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
  /*if (NULL == __fmt)
    return -1;
  
  if (NULL == fspec)
    return -1;*/

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
      
      if (cont)
        index++;
    }

  /* Width Field */
  fspec->width = __read_width_or_precision (__fmt, &index);
  if (-1 == fspec->width)
    fspec->width = va_arg (ap, int);

  /* Precision Field */
  if (__fmt[index] == '.')
    {
      index++;
      fspec->prec = __read_width_or_precision (__fmt, &index);
      if (-1 == fspec->prec)
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
    case '%':
    case 'n':
      break;

      /* Error */
    default:
      return -1;
    }

  return 0;
}

#define __putc(c)                                                           \
do                                                                          \
  {                                                                         \
    if (ret < size)                                                         \
      {                                                                     \
        *(buf) = ((char) (c));                                              \
        buf++;                                                              \
      }                                                                     \
    ret ++;                                                                 \
  }                                                                         \
while (0)

#define __get_digit(x,upper)                                                \
({                                                                          \
  char ret;                                                                 \
  switch (x)                                                                \
    {                                                                       \
    case  0: ret = '0'; break;                                              \
    case  1: ret = '1'; break;                                              \
    case  2: ret = '2'; break;                                              \
    case  3: ret = '3'; break;                                              \
    case  4: ret = '4'; break;                                              \
    case  5: ret = '5'; break;                                              \
    case  6: ret = '6'; break;                                              \
    case  7: ret = '7'; break;                                              \
    case  8: ret = '8'; break;                                              \
    case  9: ret = '9'; break;                                              \
    case 10: ret = (upper) ? 'A' : 'a'; break;                              \
    case 11: ret = (upper) ? 'B' : 'b'; break;                              \
    case 12: ret = (upper) ? 'C' : 'c'; break;                              \
    case 13: ret = (upper) ? 'D' : 'd'; break;                              \
    case 14: ret = (upper) ? 'E' : 'e'; break;                              \
    case 15: ret = (upper) ? 'F' : 'f'; break;                              \
    default: ret = '?'; break;                                              \
    }                                                                       \
  ret;                                                                      \
})

int
__itoa (char **__buf, uintmax_t val, int base, int upper)
{
  int digits = 0;                                                           
  int temp = val;                                                           
  while (temp)                                                              
    {                                                                       
      temp = temp / (base);                                                 
      digits ++;                                                            
    }                                                                       
                                                                            
  (*__buf) = (char *) __builtin_alloca (digits + 1);                             
  char *cur = (*__buf);                                                          
                                                                            
  temp = 1;                                                                 
  for (int i = 1; i < digits; i++)                                          
    temp *= base;  
  
  while (temp != 0)                                                          
    {                                                                       
      int x = val / temp;                                                   
      (*cur) = __get_digit (x, upper);                                      
      cur ++;                                                               
      val -= (x * temp);                                                    
      temp /= base;                                                         
    }        
  
  (*cur) = '\0';   
  
  return digits;                                                                   
}

static inline intmax_t
__get_int(struct __fspec fspec)                                                    
{                                                                          
  if (fspec.length.is_char)                                                 
    return (intmax_t)((char) fspec.arg.int_val);                             
  else if (fspec.length.is_short)                                           
    return (intmax_t)((short) fspec.arg.int_val);                            
  else if (fspec.length.is_long)                                            
    return (intmax_t) fspec.arg.long_val;                                    
  else if (fspec.length.is_long_long)                                       
    return (intmax_t) fspec.arg.long_long_val;                               
  else if (fspec.length.is_size_t)                                          
    return (intmax_t) fspec.arg.size_val;                                    
  else if (fspec.length.is_ptrdiff_t)                                       
    return (intmax_t) fspec.arg.ptrdiff_val;                                 
  else if (fspec.length.is_intmax_t)                                        
    return (intmax_t) fspec.arg.intmax_val;                                  
  else                                                                      
    return (intmax_t) fspec.arg.int_val;                                                                      
}

static inline uintmax_t
__get_uint (struct __fspec fspec)
{
  return (uintmax_t) __get_int (fspec);
}

int
printf_expand_fspec (char *__buf,
                     size_t size,
                     struct __fspec fspec)
{
  char *buf = __buf;
  int ret = 0;

  switch (fspec.arg.tag)
    {
    case '%':
      {
        __putc ('%');
        break;
      }
    
    case 'd':
    case 'i':
    case 'u':
    {
      char *str;
      int str_len;
      int neg;
      
      if (fspec.arg.tag == 'u')
        {
          uintmax_t val = __get_uint (fspec);
          str_len == __itoa (&str, val, 10, 0);
          neg = 0;
        }
      else
        {
          intmax_t val = __get_int (fspec);
          neg = val < 0;
          str_len == __itoa (&str, (neg ? val * -1 : val), 10, 0);
        }
      
      int needed = fspec.width - str_len;
      if (neg || fspec.flag.plus || fspec.flag.space)
        needed --;
      
      if (!fspec.flag.left && !fspec.flag.zero)
        {
          for (int i = 0; i < needed; i ++)
            __putc (' ');
          needed = 0;
        }
      
      if (neg)
        __putc ('-');
      else if (fspec.flag.plus)
        __putc ('+');
      else if (fspec.flag.space)
        __putc (' ');
      
      if (fspec.flag.zero)
        {
          for (int i = 0; i < needed; i ++)
            __putc ('0');
          needed = 0;
        }
      
      while (str && *str)
        {
          __putc (*str);
          str++;
        }
      
      for (int i = 0; i < needed; i ++)
        __putc (' ');
      
      break;
    }
    case 'f':
    case 'F':
    case 'e':
    case 'E':
    case 'g':
    case 'G':
    case 'x':
    case 'X':
    case 'o':
    case 's':
      {
        char *str = (char *) fspec.arg.void_p_val;
        int len = 0;

        for (int i = 0;
                str && str[i] != '\0' && (!fspec.prec || (i < fspec.prec));
                i++)
          len++;

        if (fspec.width && fspec.flag.left)
          for (int i = 0; i < (fspec.width - len); i++)
            __putc (' ');

        for (int i = 0; i < len; i++)
          __putc (str[i]);

        if (fspec.width && !fspec.flag.left)
          for (int i = 0; i < (fspec.width - len); i++)
            __putc (' ');

        break;
      }
    case 'c':
      {
        if (fspec.width && !fspec.flag.left)
          for (int i = 0; i < (fspec.width - 1); i++)
            __putc (' ');

        __putc (fspec.arg.int_val);

        if (fspec.width && fspec.flag.left)
          for (int i = 0; i < (fspec.width - 1); i++)
            __putc (' ');

        break;
      }
    case 'p':
    case 'a':
    case 'A':
    case 'n':
      {
        break;
      }
    default:
      {
        ret = -1;
      }
    }

  return ret;
}