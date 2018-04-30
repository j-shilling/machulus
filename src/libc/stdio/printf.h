#ifndef PRINTF_H
#define PRINTF_H

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>

#include "printf-fspec.h"

#ifdef __cplusplus
extern "C"
{
#endif
  
#define __int_arg(fspec,ap)                                                 \
({                                                                          \
    intmax_t val;                                                           \
                                                                            \
    if (fspec.is_char)                                                      \
      val = (char) va_arg (ap, int);                                        \
    else if (fspec.is_short)                                                \
      val = (short) va_arg (ap, int);                                       \
    else if (fspec.is_long)                                                 \
      val = va_arg (ap, long);                                              \
    else if (fspec.is_long_long)                                            \
      val = va_arg (ap, long long);                                         \
    else if (fspec.is_size_t)                                               \
      val = va_arg (ap, size_t);                                            \
    else if (fspec.is_intmax_t)                                             \
      val = va_arg (ap, intmax_t);                                          \
    else if (fspec.is_ptrdiff_t)                                            \
      val = va_arg (ap, ptrdiff_t);                                         \
    else                                                                    \
      val = va_arg (ap, int);                                               \
                                                                            \
    val;                                                                    \
})
  
#define __uint_arg(fspec,ap)                                                \
({                                                                          \
    uintmax_t val = (uintmax_t) __int_arg(fspec,ap);                        \
    val;                                                                    \
})
  
#define __double_arg(fspec,ap)                                              \
({                                                                          \
    long double val;                                                        \
                                                                            \
    if (fspec.is_long_double)                                               \
      val = va_arg (ap, long double);                                       \
    else                                                                    \
      val = va_arg (ap, double);                                            \
                                                                            \
    val;                                                                    \
})
  
#define __str_arg(ap)                                                       \
({                                                                          \
    char *str = va_arg (ap, char *);                                        \
    str;                                                                    \
})
  
#define __ptr_arg(ap)                                                       \
({                                                                          \
    void *ptr = va_arg (ap, void *);                                        \
    ptr;                                                                    \
})
  
#define __get_digit(x,upper)                                                \
({                                                                          \
    char ret;                                                               \
    switch (x)                                                              \
    {                                                                       \
    case 0:     ret = '0'; break;                                           \
    case 1:     ret = '1'; break;                                           \
    case 2:     ret = '2'; break;                                           \
    case 3:     ret = '3'; break;                                           \
    case 4:     ret = '4'; break;                                           \
    case 5:     ret = '5'; break;                                           \
    case 6:     ret = '6'; break;                                           \
    case 7:     ret = '7'; break;                                           \
    case 8:     ret = '8'; break;                                           \
    case 9:     ret = '9'; break;                                           \
    case 10:    ret = upper ? 'A' : 'a' ; break;                            \
    case 11:    ret = upper ? 'B' : 'b' ; break;                            \
    case 12:    ret = upper ? 'C' : 'c' ; break;                            \
    case 13:    ret = upper ? 'D' : 'd' ; break;                            \
    case 14:    ret = upper ? 'E' : 'e' ; break;                            \
    case 15:    ret = upper ? 'F' : 'f' ; break;                            \
    default:    ret = '?'; break;                                           \
    }                                                                       \
    ret;                                                                    \
})
  
#define __itoa(x,str,base,upper)                                            \
({                                                                          \
    __label__ done;                                                         \
                                                                            \
    if (x == 0)                                                             \
      {                                                                     \
        str[0] = '0';                                                       \
        str[1] = '\0';                                                      \
        goto done;                                                          \
      }                                                                     \
                                                                            \
    typeof (x) val = x;                                                     \
    int neg = (x < 0);                                                      \
    if (neg)                                                                \
      val *= -1;                                                            \
    neg == neg && (base == 10);                                             \
                                                                            \
    size_t i = 0;                                                           \
                                                                            \
    while (val != 0)                                                        \
      {                                                                     \
        unsigned int rem = val % base;                                      \
        str[i++] = __get_digit (rem, upper);                                \
        val = val / base;                                                   \
      }                                                                     \
    if (neg)                                                                \
      str[i++] = '-';                                                       \
    str[i++] = '\0';                                                        \
                                                                            \
    size_t len = i;                                                         \
    for (i = 0; i < (len / 2); i++)                                         \
      {                                                                     \
        char temp = str[i];                                                 \
        str[i] = str[len - i - 1];                                          \
        str[len - i - 1] = temp;                                            \
      }                                                                     \
                                                                            \
    done:                                                                   \
      str;                                                                  \
})
  
#define __printf_parse(fspec,ap)                                            \
({                                                                          \
    __label__ done;                                                         \
    char *ret;                                                              \
                                                                            \
    switch (fspec.type)                                                     \
    {                                                                       \
    case '%':                                                               \
      ret = "%";                                                            \
      goto done;                                                            \
                                                                            \
    case 'd':                                                               \
    case 'i': /* signed decimal integer */                                  \
    {                                                                       \
      ret = __builtin_alloca (25);                                          \
      intmax_t val = __int_arg (fspec, ap);                                 \
                                                                            \
      if (fspec.plus  && (val >= 0))                                        \
        ret[0] = '+';                                                       \
      else if (fspec.space && (val >= 0))                                   \
        ret[0] = ' ';                                                       \
                                                                            \
      if ((val >= 0) && (fspec.plus || fspec.space))                        \
        __itoa ((val + 1), ret, 10, 0);                                     \
      else                                                                  \
        __itoa (val, ret, 10, 0);                                           \
                                                                            \
      goto done;                                                            \
    }                                                                       \
                                                                            \
    case 'u': /* unsigned decimal integer */                                \
    {                                                                       \
      ret = __builtin_alloca (25);                                          \
      intmax_t val = __uint_arg (fspec, ap);                                \
                                                                            \
      if (fspec.plus)                                                       \
        ret[0] = '+';                                                       \
      else if (fspec.space)                                                 \
        ret[0] = ' ';                                                       \
                                                                            \
      if (fspec.plus || fspec.space)                                        \
        __itoa ((val + 1), ret, 10, 0);                                     \
      else                                                                  \
        __itoa (val, ret, 10, 0);                                           \
                                                                            \
      goto done;                                                            \
    }                                                                       \
                                                                            \
    case 'f':                                                               \
    case 'F': /* Fix-point decimal float */                                 \
      break;                                                                \
                                                                            \
    case 'e':                                                               \
    case 'E': /* exponent form decimal float */                             \
      break;                                                                \
                                                                            \
    case 'g':                                                               \
    case 'G': /* float in normal or exponent form */                        \
      break;                                                                \
                                                                            \
    case 'x':                                                               \
    case 'X': /* unsigned int in hex */                                     \
    {                                                                       \
      ret = __builtin_alloca (25);                                          \
      intmax_t val = __uint_arg (fspec, ap);                                \
      int upper = fspec.type == 'X';                                        \
                                                                            \
      if (fspec.plus)                                                       \
        ret[0] = '+';                                                       \
      else if (fspec.space)                                                 \
        ret[0] = ' ';                                                       \
                                                                            \
      if (fspec.plus || fspec.space)                                        \
        __itoa ((val + 1), ret, 16, upper);                                 \
      else                                                                  \
        __itoa (val, ret, 16, upper);                                       \
                                                                            \
      goto done;                                                            \
    }                                                                       \
                                                                            \
    case 'o': /* unsigned int in octal */                                   \
    {                                                                       \
      ret = __builtin_alloca (25);                                          \
      intmax_t val = __uint_arg (fspec, ap);                                \
                                                                            \
      if (fspec.plus)                                                       \
        ret[0] = '+';                                                       \
      else if (fspec.space)                                                 \
        ret[0] = ' ';                                                       \
                                                                            \
      if (fspec.plus || fspec.space)                                        \
        __itoa ((val + 1), ret, 8, 0);                                      \
      else                                                                  \
        __itoa (val, ret, 8, 0);                                            \
                                                                            \
      goto done;                                                            \
    }                                                                       \
                                                                            \
    case 's': /* null-terminated string */                                  \
    {                                                                       \
      ret = __str_arg (ap);                                                 \
      goto done;                                                            \
    }                                                                       \
                                                                            \
    case 'c': /* char */                                                    \
    {                                                                       \
      ret = __builtin_alloca (2);                                           \
      ret[0] = (char) __int_arg (fspec, ap);                                \
      ret[1] = '\0';                                                        \
      goto done;                                                            \
    }                                                                       \
                                                                            \
    case 'p': /* void * */                                                  \
      break;                                                                \
                                                                            \
    case 'a':         /usr/bin/qemu-system-i386 -kenel ./src/kernel/jkernel                                                      \
    case 'A': /* float in hex */                                            \
      break;                                                                \
                                                                            \
                                                                            \
    default:                                                                \
      ret = NULL;                                                           \
      goto done;                                                            \
    }                                                                       \
                                                                            \
    done:                                                                   \
      ret;                                                                  \
                                                                            \
})

#ifdef __cplusplus
}
#endif

#endif /* PRINTF_H */

