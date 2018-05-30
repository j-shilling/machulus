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

int
vprintf (const char *format, va_list ap)
{
  int ret = 0;

#define __print(x) \
  do { if (EOF == putchar (x)) return EOF; else ret ++; if (ret == INT_MAX) return ret; } while (0)

  while ((*format) != '\0')
    {
      if ((*format) != '%')
	{
	  __print (*format);
	  format ++;
	}
      else
	{
	  format ++;

	  int flags = 0;
	  /* Format Flags */
#         define HASH   (1 << 0)
#         define ZERO   (1 << 1)
#         define LEFT   (1 << 2)
#         define SPACE  (1 << 3)
#         define SIGNED (1 << 4)

	  /* Length Flags */
#         define CHAR        (1 << 5)
#         define SHORT       (1 << 6)
#         define LONG        (1 << 7)
#         define LONG_LONG   (1 << 8)
#         define LONG_DOUBLE (1 << 9)
#         define INTMAX      (1 << 10)
#         define SIZE        (1 << 11)
#         define PTRDIFF     (1 << 12)

	  /* Get Format Flags */
	  for (int loop = 1; loop;)
	    {
	      switch ((*format))
		{
		case '#': flags |= HASH; break;
		case '0': flags |= ZERO; break;
		case '-': flags |= LEFT; break;
		case ' ': flags |= SPACE; break;
		case '+': flags |= SIGNED; break;
		default:  loop = 0;
		}

	      if (loop)
		format ++;
	    }

	  /* Get field width */
	  int width = 0;
	  if ((*format) == '*')
	    {
	      width = va_arg (ap, int);
	      format ++;
	    }
	  else if ((*format) >= '1' && (*format) <= '9')
	    {
	      while ((*format) >= '0' && (*format) <= '9')
		{
		  width = (width * 10) + ((*format) - '0');
		  format ++;
		}
	    }

	  /* Get Precision Field */
	  int precision = 0;
	  if ((*format) == '.')
	    {
	      format ++;
	      if ((*format) == '*')
		{
		  precision = va_arg (ap, int);
		  format ++;
		}
	      else if ((*format) >= '1' && (*format) <= '9')
		{
		  while ((*format) >= '0' && (*format) <= '9')
		    {
		      precision = (precision * 10) + ((*format) - '0');
		      format ++;
		    }
		}
	    }

	  /* Get length modifier */
	  switch ((*format))
	    {
	    case 'h':
	      format ++;
	      if ((*format) == 'h')
		flags |= CHAR;
	      else
		flags |= SHORT;
	      break;
	    case 'l':
	      format ++;
	      if ((*format) == 'l')
		flags |= LONG_LONG;
	      else
		flags |= LONG;
	      break;
	    case 'L':
	      format ++;
	      flags |= LONG_DOUBLE;
	      break;
	    case 'z':
	      format ++;
	      flags |= SIZE;
	      break;
	    case 'j':
	      format ++;
	      flags |= INTMAX;
	      break;
	    case 't':
	      format ++;
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
		    len ++;

		  if ((precision > 0) && (precision < len))
		    len = precision;

		  if (!(flags & LEFT))
		    for (size_t i = 0; i < (width - len); i ++)
		      __print (' ');
		  
		  for (const char *cur = s; cur && (*cur); cur ++)
		    __print (*cur);

                  if (flags & LEFT)
		    for (size_t i = 0; i < (width - len); i ++)
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
		      (*ptr) = (char)ret;
		    }
		  else if (flags & SHORT)
		    {
		      short *ptr = va_arg (ap, short *);
		      (*ptr) = (short)ret;
		    }
		  else if (flags & LONG)
		    {
		      long *ptr = va_arg (ap, long *);
		      (*ptr) = (long)ret;
		    }
		  else if (flags & LONG_LONG)
		    {
		      long long *ptr = va_arg (ap, long long *);
		      (*ptr) = (long long)ret;
		    }
		  else if (flags & INTMAX)
		    {
		      intmax_t *ptr = va_arg (ap, intmax_t *);
		      (*ptr) = (intmax_t)ret;
		    }
		  else if (flags & SIZE)
		    {
		      size_t *ptr = va_arg (ap, size_t *);
		      (*ptr) = (size_t)ret;
		    }
		  else if (flags & PTRDIFF)
		    {
		      ptrdiff_t *ptr = va_arg (ap, size_t *);
		      (*ptr) = (ptrdiff_t)ret;
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

	  format ++; /* move past conversion character */
	} /* (*format) == '%' */
    } /* main loop */

#undef __print

  return ret;
}
