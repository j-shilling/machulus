#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <limits.h>

int __attribute__ ((format (printf, 1, 2)))
printf (const char *format, ...)
{
  va_list ap;
  va_start (ap, format);

  int ret = vprintf (format, ap);

  va_end (ap);
  return ret;
}

/* Format Flags */
#define HASH   (1 << 0)
#define ZERO   (1 << 1)
#define LEFT   (1 << 2)
#define SPACE  (1 << 3)
#define SIGNED (1 << 4)

/* Length Flags */
#define CHAR        (1 << 5)
#define SHORT       (1 << 6)
#define LONG        (1 << 7)
#define LONG_LONG   (1 << 8)
#define LONG_DOUBLE (1 << 9)
#define INTMAX      (1 << 10)
#define SIZE        (1 << 11)
#define PTRDIFF     (1 << 12)

/* Precision and Width flags */
#define PRECISION   (1 << 13)
#define WIDTH       (1 << 14)
#define UPPERCASE   (1 << 15)

#define BUF_MAX     (512)

int
vprintf (const char *format, va_list ap)
{
  int ret = 0;

#define __print(x)                      \
    do {                                \
        if (ret == INT_MAX)             \
            return ret;                 \
        if (EOF == putchar ((x)))       \
            return ret;                 \
        ret ++;                         \
    } while (0)


  while ((*format) != '\0')
    {
      if ((*format) != '%')
        {
          __print (*format);
          format++;
        }
      else
        {
          format++;

          int flags = 0;


          /* Get Format Flags */
          for (int loop = 1; loop;)
            {
              switch ((*format))
                {
                case '#': flags |= HASH;
                  break;
                case '0': flags |= ZERO;
                  break;
                case '-': flags |= LEFT;
                  break;
                case ' ': flags |= SPACE;
                  break;
                case '+': flags |= SIGNED;
                  break;
                default: loop = 0;
                }

              if (loop)
                format++;
            }

          /* Get field width */
          int width = 0;
          if ((*format) == '*')
            {
              width = va_arg (ap, int);
              format++;
              flags |= WIDTH;
            }
          else if ((*format) >= '1' && (*format) <= '9')
            {
              while ((*format) >= '0' && (*format) <= '9')
                {
                  width = (width * 10) + ((*format) - '0');
                  format++;
                }
              flags |= WIDTH;
            }

          /* Get Precision Field */
          int precision = 0;
          if ((*format) == '.')
            {
              format++;
              flags |= PRECISION;
              if ((*format) == '*')
                {
                  precision = va_arg (ap, int);
                  format++;
                }
              else if ((*format) >= '1' && (*format) <= '9')
                {
                  while ((*format) >= '0' && (*format) <= '9')
                    {
                      precision = (precision * 10) + ((*format) - '0');
                      format++;
                    }
                }
            }

          /* Get length modifier */
          switch ((*format))
            {
            case 'h':
              format++;
              if ((*format) == 'h')
                flags |= CHAR;
              else
                flags |= SHORT;
              break;
            case 'l':
              format++;
              if ((*format) == 'l')
                flags |= LONG_LONG;
              else
                flags |= LONG;
              break;
            case 'L':
              format++;
              flags |= LONG_DOUBLE;
              break;
            case 'z':
              format++;
              flags |= SIZE;
              break;
            case 'j':
              format++;
              flags |= INTMAX;
              break;
            case 't':
              format++;
              flags |= PTRDIFF;
              break;
            default:
              break;
            }

          switch ((*format))
            {
              /* Integers */
            case 'd':
            case 'i':
            case 'u':
            case 'o':
            case 'x':
            case 'X':
            case 'p':
              {
                /* Get base */
                int base;
                if ((*format) == 'x' || (*format) == 'X' || (*format) == 'p')
                  base = 16;
                else if ((*format) == 'o')
                  base = 8;
                else
                  {
                    base = 10;
                    /* No alt form for base 10 */
                    flags &= ~HASH;
                  }

                /* Check case */
                if ((*format) == 'X')
                  flags |= UPPERCASE;

                /* no signed or space for unsigned values */
                if (((*format) != 'i') && ((*format) != 'd'))
                  flags &= ~(SIGNED | SPACE);

                /* assert alt form for addresses */
                if ((*format) == 'p')
                  {
                    flags |= HASH;
                    precision = 2 * sizeof (void *);
                    flags |= PRECISION;
                  }

                /* Get value */
                int negative;
                uintmax_t value;
                if (((*format) == 'i') || ((*format) == 'd'))
                  {
                    intmax_t signed_value;

                    if (flags & CHAR)
                      {
                        int arg = va_arg (ap, int);
                        signed_value = (intmax_t) ((char) arg);
                      }
                    else if (flags & SHORT)
                      {
                        int arg = va_arg (ap, int);
                        signed_value = (intmax_t) ((short) arg);
                      }
                    else if (flags & LONG)
                      {
                        long arg = va_arg (ap, long);
                        signed_value = (intmax_t) arg;
                      }
                    else if (flags & LONG_LONG)
                      {
                        long long arg = va_arg (ap, long long);
                        signed_value = (intmax_t) arg;
                      }
                    else if (flags & INTMAX)
                      {
                        signed_value = va_arg (ap, intmax_t);
                      }
                    else if (flags & SIZE)
                      {
                        size_t arg = va_arg (ap, size_t);
                        signed_value = (intmax_t) arg;
                      }
                    else if (flags & PTRDIFF)
                      {
                        ptrdiff_t arg = va_arg (ap, ptrdiff_t);
                        signed_value = (intmax_t) arg;
                      }
                    else
                      {
                        int arg = va_arg (ap, int);
                        signed_value = (intmax_t) arg;
                      }

                    negative = (signed_value < 0);
                    value = (negative) ?
                            (uintmax_t) (signed_value * -1) : (uintmax_t) (signed_value);
                  }
                else
                  {
                    negative = 0;
                    if (flags & CHAR)
                      {
                        unsigned int arg = va_arg (ap, unsigned int);
                        value = (uintmax_t) ((unsigned char) arg);
                      }
                    else if (flags & SHORT)
                      {
                        unsigned int arg = va_arg (ap, unsigned int);
                        value = (uintmax_t) ((unsigned short) arg);
                      }
                    else if (flags & LONG)
                      {
                        unsigned long arg = va_arg (ap, unsigned long);
                        value = (uintmax_t) arg;
                      }
                    else if (flags & LONG_LONG)
                      {
                        unsigned long long arg = va_arg (ap, unsigned long long);
                        value = (uintmax_t) arg;
                      }
                    else if (flags & INTMAX)
                      {
                        value = va_arg (ap, uintmax_t);
                      }
                    else if (flags & SIZE)
                      {
                        size_t arg = va_arg (ap, size_t);
                        value = (uintmax_t) arg;
                      }
                    else if (flags & PTRDIFF)
                      {
                        ptrdiff_t arg = va_arg (ap, ptrdiff_t);
                        value = (uintmax_t) arg;
                      }
                    else
                      {
                        unsigned int arg = va_arg (ap, unsigned int);
                        value = (uintmax_t) arg;
                      }
                  }

                if (value == 0)
                  {
                    __print ('0');
                  }
                else
                  {
                    /* write digits to buf backwards */
                    char buf[BUF_MAX];
                    int len = 0;
                    do
                      {
                        const int digit = (value % base);
                        buf[len++] = (digit < 10) ?
                                '0' + digit :
                                (flags & UPPERCASE ? 'A' : 'a') + digit - 10;
                        value /= base;
                      }
                    while (value && (len < BUF_MAX));

                    /* Add zeros to fill out the precision specification */
                    while ((len < precision) && (len < BUF_MAX))
                      buf[len++] = '0';

                    /* Add zero padding if requested */
                    if (!(flags & LEFT) && (flags & ZERO))
                      {
                        int zeros = (width - len);
                        if (flags & HASH)
                          zeros -= (base == 16) ? 2 : 1;
                        for (; (zeros > 0) && (len < BUF_MAX); zeros--)
                          buf[len++] = '0';
                      }

                    /* Handle alt formating */
                    if (flags & HASH)
                      {
                        if ((base == 16) && (flags & UPPERCASE) && (len < BUF_MAX))
                          buf[len++] = 'X';
                        else if ((base == 16) && (len < BUF_MAX))
                          buf[len++] = 'x';

                        if (len < BUF_MAX)
                          buf[len++] = '0';
                      }

                    /* Handle sign */
                    if ((len == width) && (negative || (flags & SIGNED) || (flags & SPACE)))
                      {
                        len--;

                        if (len < BUF_MAX)
                          {
                            if (negative)
                              buf[len++] = '-';
                            else if (flags & SIGNED)
                              buf[len++] = '+';
                            else
                              buf[len++] = ' ';
                          }
                      }

                    /* Pad with spaces */
                    if (!(flags & LEFT) && !(flags & ZERO))
                      {
                        for (int i = len; i < width; i++)
                          __print (' ');
                      }

                    /* Print buf backwards */
                    for (int i = (len - 1); i >= 0; i--)
                      __print (buf[i]);

                    /* Pad with spaces */
                    if (flags & LEFT)
                      for (int i = len; i < width; i++)
                        __print (' ');
                  }
                break;
              }

              /* Doubles */
            case 'f':
            case 'F':
            case 'e':
            case 'E':
            case 'a':
            case 'A':

              /* Misc */
            case '%':
              {
                __print ('%');
                break;
              }

            case 's':
              {
                const char *s = va_arg (ap, char *);
                size_t len = 0;
                for (const char *cur = s; cur && (*cur); cur++)
                  len++;

                if ((precision > 0) && (precision < len))
                  len = precision;

                if (!(flags & LEFT))
                  for (size_t i = 0; i < (width - len); i++)
                    __print (' ');

                for (const char *cur = s; cur && (*cur); cur++)
                  __print (*cur);

                if (flags & LEFT)
                  for (size_t i = 0; i < (width - len); i++)
                    __print (' ');
                break;
              }
            case 'c':
              {
                __print (va_arg (ap, int));
                break;
              }

            case 'n':
              {
                if (flags & CHAR)
                  {
                    char *ptr = va_arg (ap, char *);
                    (*ptr) = (char) ret;
                  }
                else if (flags & SHORT)
                  {
                    short *ptr = va_arg (ap, short *);
                    (*ptr) = (short) ret;
                  }
                else if (flags & LONG)
                  {
                    long *ptr = va_arg (ap, long *);
                    (*ptr) = (long) ret;
                  }
                else if (flags & LONG_LONG)
                  {
                    long long *ptr = va_arg (ap, long long *);
                    (*ptr) = (long long) ret;
                  }
                else if (flags & INTMAX)
                  {
                    intmax_t *ptr = va_arg (ap, intmax_t *);
                    (*ptr) = (intmax_t) ret;
                  }
                else if (flags & SIZE)
                  {
                    size_t *ptr = va_arg (ap, size_t *);
                    (*ptr) = (size_t) ret;
                  }
                else if (flags & PTRDIFF)
                  {
                    ptrdiff_t *ptr = va_arg (ap, size_t *);
                    (*ptr) = (ptrdiff_t) ret;
                  }
                else
                  {
                    int *ptr = va_arg (ap, int *);
                    (*ptr) = ret;
                  }
                break;
              }

            default:
              /* Invalid conversion specifier */
              break;
            }

          format++; /* move past conversion character */
        } /* (*format) == '%' */
    } /* main loop */

#undef __print

  return ret;
}
