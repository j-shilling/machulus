/*
 * Copyright (C) 2018 Jake Shilling
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* file: stdlib/stdio/vfprintf.c
 * 
 *   Implements vfprintf() which prints a formated string to a file. On success,
 * it returns the number of bytes transmitted; on failure, it return a negative 
 * value. */

#include <stdio.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Formatting options */
static const uint_fast32_t MINUS_FLAG = (1 << 1);
static const uint_fast32_t PLUS_FLAG = (1 << 2);
static const uint_fast32_t SPACE_FLAG = (1 << 3);
static const uint_fast32_t ZERO_FLAG = (1 << 4);
static const uint_fast32_t HASH_FLAG = (1 << 5);

/* Size options */
static const uint_fast32_t CHAR_SIZE_FLAG = (1 << 6);
static const uint_fast32_t SHORT_FLAG = (1 << 7);
static const uint_fast32_t LONG_FLAG = (1 << 8);
static const uint_fast32_t LONG_LONG_FLAG = (1 << 9);
static const uint_fast32_t LONG_DOUBLE_FLAG = (1 << 10);
static const uint_fast32_t SIZE_T_FLAG = (1 << 11);
static const uint_fast32_t INTMAX_T_FLAG = (1 << 12);
static const uint_fast32_t PTRDIFF_T_FLAG = (1 << 13);

/* Type options */
static const uint_fast32_t PERCENT_FLAG = (1 << 14);
static const uint_fast32_t SIGNED_DECIMAL_FLAG = (1 << 15);
static const uint_fast32_t UNSIGNED_DECIMAL_FLAG = (1 << 16);
static const uint_fast32_t FLOAT_NORMAL_DOWNCASE_FLAG = (1 << 17);
static const uint_fast32_t FLOAT_NORMAL_UPCASE_FLAG = (1 << 18);
static const uint_fast32_t FLOAT_EXPONENT_DOWNCASE_FLAG = (1 << 19);
static const uint_fast32_t FLOAT_EXPONENT_UPCASE_FLAG = (1 << 20);
static const uint_fast32_t FLOAT_CHOOSE_DOWNCASE_FLAG = (1 << 21);
static const uint_fast32_t FLOAT_CHOOSE_UPCASE_FLAG = (1 << 22);
static const uint_fast32_t INT_HEX_DOWNCASE_FLAG = (1 << 23);
static const uint_fast32_t INT_HEX_UPCASE_FLAG = (1 << 24);
static const uint_fast32_t OCT_FLAG = (1 << 25);
static const uint_fast32_t STRING_FLAG = (1 << 26);
static const uint_fast32_t CHAR_TYPE_FLAG = (1 << 27);
static const uint_fast32_t POINTER_FLAG = (1 << 28);
static const uint_fast32_t FLOAT_HEX_DOWNCASE_FLAG = (1 << 29);
static const uint_fast32_t FLOAT_HEX_UPCASE_FLAG = (1 << 30);
static const uint_fast32_t COUNT_FLAG = (1 << 31);

static inline bool
__isdigit (char x)
{
  return (x == '0')
          || (x == '1')
          || (x == '2')
          || (x == '3')
          || (x == '4')
          || (x == '5')
          || (x == '6')
          || (x == '7')
          || (x == '8')
          || (x == '9');
}

static inline int
__get_width_or_precision (const char **__fmt, va_list ap)
{
  const char *fmt = (*__fmt);
  int ret = 0;

  if ((*fmt) == '*')
    {
      ret = va_arg (ap, int);
      fmt++;
    }
  else
    {
      while (__isdigit ((*fmt)))
        {
          ret = (ret * 10) + ((*fmt) - '0');
          fmt++;
        }
    }

  
  (*__fmt) = fmt;
  return ret;
}

static inline uintmax_t
__get_integer_argument (bool *__is_negative, uint_fast32_t flags, va_list ap)
{
  bool is_negative;
  uintmax_t value;
  
  /* Arguments can only be signed if the SIGNED_DECIMAL_FLAG is
     set. Otherwise, we assume unsigned types. Remember that char
     and short is promotted to an int when passed in varargs. */
  if (flags & SIGNED_DECIMAL_FLAG)
    {
      if (flags & CHAR_SIZE_FLAG)
	{
	  char x = (char) va_arg (ap, int);
	  is_negative = x < 0;
	  value = (uintmax_t) (is_negative ? -x : x);
	}
      else if (flags & SHORT_FLAG)
	{
	  short x = (short) va_arg (ap, int);
	  is_negative = x < 0;
	  value = (uintmax_t) (is_negative ? -x : x);
	}
      else if (flags & LONG_FLAG)
	{
	  long x = va_arg (ap, long);
	  is_negative = x < 0;
	  value = (uintmax_t) (is_negative ? -x : x);
	}
      else if (flags & LONG_LONG_FLAG)
	{
	  long long x = va_arg (ap, long long);
	  is_negative = x < 0;
	  value = (uintmax_t) (is_negative ? -x : x);
	}
      else if (flags & SIZE_T_FLAG)
	{
	  size_t x = va_arg (ap, size_t);
	  is_negative = x < 0;
	  value = (uintmax_t) (is_negative ? -x : x);
	}
      else if (flags & INTMAX_T_FLAG)
	{
	  intmax_t x = va_arg (ap, intmax_t);
	  is_negative = x < 0;
	  value = (uintmax_t) (is_negative ? -x : x);
	}
      else if (flags & PTRDIFF_T_FLAG)
	{
	  ptrdiff_t x = va_arg (ap, ptrdiff_t);
	  is_negative = x < 0;
	  value = (uintmax_t) (is_negative ? -x : x);
	}
      else
	{
	  int x = va_arg (ap, int);
	  is_negative = x < 0;
	  value = (uintmax_t) (is_negative ? -x : x);
	}
    }
  else
    {
      is_negative = false;
      if (flags & CHAR_SIZE_FLAG)
	value = (uintmax_t) ((unsigned char) va_arg (ap, unsigned int));
      else if (flags & SHORT_FLAG)
	value = (uintmax_t) ((unsigned short) va_arg (ap, unsigned int));
      else if (flags & LONG_FLAG)
	value = (uintmax_t) (va_arg (ap, unsigned long));
      else if (flags & LONG_LONG_FLAG)
	value = (uintmax_t) (va_arg (ap, unsigned long long));
      else if (flags & SIZE_T_FLAG)
	value = (uintmax_t) (va_arg (ap, size_t));
      else if (flags & INTMAX_T_FLAG)
	value = va_arg (ap, uintmax_t);
      else if (flags & PTRDIFF_T_FLAG)
	value = (uintmax_t) (va_arg (ap, ptrdiff_t));
      else
	value = (uintmax_t) (va_arg (ap, unsigned int));
    }

  (*__is_negative) = is_negative;
  return value;
}

static inline int
__get_integer_digits (char *buf, size_t size, int base, bool upcase, uintmax_t value)
{
  /* Handle 0 as a stand alone case */
  if (value == 0)
    {
      if (size > 0)
	buf[0] = '0';
      return 1;
    }

  /* Put digits into the buffer string backwards */
  int len = 0;
  
  while (value > 0 && len < size)
    {
      int cur = value % base;
      if (cur < 10)
	buf[len++] = '0' + cur;
      else if (upcase)
	buf[len++] = 'A' + (cur - 10);
      else
	buf[len++] = 'a' + (cur - 10);
      value /= base;
    }

  return len;
}

static int
__itoa (int *__done, FILE *stream, uint_fast32_t flags, int width, int precision, va_list ap)
{
  int done = (*__done);

  /* Get the value from ap */
  bool is_negative;
  uintmax_t value = __get_integer_argument (&is_negative, flags, ap);
 
  /* Make formatting changes for pointers */
  if (flags & POINTER_FLAG)
    {
      flags = INT_HEX_DOWNCASE_FLAG | HASH_FLAG;
      precision = sizeof (void *) * 2;
    }
  
  /* Get the base */
  int base;
  if (flags & (INT_HEX_DOWNCASE_FLAG | INT_HEX_UPCASE_FLAG))
    base = 16;
  else if (flags & OCT_FLAG)
    base = 8;
  else
    base = 10;

  /* Get the digits */
  char digits[50];
  int n_digits = __get_integer_digits (digits, 50, base, flags & INT_HEX_UPCASE_FLAG, value);
 
  /* For integer arguments, precision specifies the minimum
     number of digits. If extra digits are needed, then 0s are
     prepended. Subtract actual digits from precision to figure
     out if this is needed. */
  precision -= n_digits;

  /* Width specifies the minimum number of output characters.
     subtract the number of characters we are going to print to
     see if this is needed. */

  /* Needed chars for digits */
  width -= n_digits;
  /* Needed chars for precision */
  if (precision > 0)
    width -= precision;
  /* Needed chars for prefix */
  if (flags & HASH_FLAG)
    {
      if (base == 16)
        width -= 2;
      else if (base == 8)
        width--;
    }
  /* Needed chars for sign */
  if (flags & SIGNED_DECIMAL_FLAG)
    {
      if (is_negative || (flags & (SPACE_FLAG | PLUS_FLAG)))
        width--;
    }

  /* If we are right-justified and space padding print spaces */
  if (!(flags & MINUS_FLAG) && !(flags & ZERO_FLAG))
    {
      while (width > 0)
        {
          if (EOF == fputc (' ', stream))
            return -1;
          done++;
          width--;
        }
    }

  /* We might need to print a sign */
  if (flags & SIGNED_DECIMAL_FLAG)
    {
      if (is_negative)
        {
          if (EOF == fputc ('-', stream))
            return -1;
          done++;
        }
      else if (flags & SPACE_FLAG)
        {
          if (EOF == fputc (' ', stream))
            return -1;
          done++;
        }
      else if (flags & PLUS_FLAG)
        {
          if (EOF == fputc ('+', stream))
            return -1;
          done++;
        }
    }

  /* We might need to print a prefix */
  if (flags & HASH_FLAG)
    {
      /* First char is always 0 */
      if (flags & (OCT_FLAG | INT_HEX_UPCASE_FLAG | INT_HEX_DOWNCASE_FLAG))
	{
	  if (EOF == fputc ('0', stream))
	    return -1;
	  done ++;
	}

      /* We might need an 'x' or 'X' */
      if (flags & INT_HEX_DOWNCASE_FLAG)
        {
          if (EOF == fputc ('x', stream))
            return -1;
          done ++;
        }
      else if (flags & INT_HEX_UPCASE_FLAG)
        {
          if (EOF == fputc ('X', stream))
            return -1;
          done++;
        }
    }

  /* If we are right-justified and zero padded that comes next */
  if (!(flags & MINUS_FLAG) && (flags & ZERO_FLAG))
    {
      while (width > 0)
        {
          if (EOF == fputc ('0', stream))
            return -1;
          done++;
          width--;
        }
    }

  /* If extra digits are needed, prepend 0s */
  while (precision > 0)
    {
      if (EOF == fputc ('0', stream))
	return -1;
      done ++;
      precision --;
    }

  /* Print the digits */
  for (int i = (n_digits - 1); i >= 0; i --)
    {
      if (EOF == fputc(digits[i], stream))
	return -1;
      done ++;
    }
  
  /* If we are left justified print spaces */
  if (flags & MINUS_FLAG)
    {
      while (width > 0)
        {
          if (EOF == fputc(' ', stream))
            return -1;
          done ++;
          width --;
        }
    }

  (*__done) = done;
  return 0;
}

static int
__ftoa (int *__done, FILE *stream, uint_fast32_t flags, int width, int precision, va_list ap)
{
  int done = (*__done);
  
  (*__done) = done;
  return 0;
}

int
vfprintf (FILE *stream, const char *fmt, va_list ap)
{
  /* Check for null arguments */
  if (NULL == stream || NULL == fmt)
    {
      errno = EINVAL;
      return -1;
    }

  int done = 0;
  const char *cur = fmt;

  /* Main loop */
  while ('\0' != (*cur))
    {
      /* Check for formatted section */
      if ('%' == (*cur))
        {
          cur ++;
          uint_fast32_t flags = 0;
          int width = 0;
          int precision = 0;

          /* Get formatting options '-', '+', ' ', '0', or '#' */
read_flags:
          switch ((*cur))
            {
            case '-':
              flags |= MINUS_FLAG;
              cur++;
              goto read_flags;
            case '+':
              flags |= PLUS_FLAG;
              cur++;
              goto read_flags;
            case ' ':
              flags |= SPACE_FLAG;
              cur++;
              goto read_flags;
            case '0':
              flags |= ZERO_FLAG;
              cur++;
              goto read_flags;
            case '#':
              flags |= HASH_FLAG;
              cur++;
              goto read_flags;
            default:
              break;
            }

          /* Next we might have a width field, which can either be a positive decimal
             integer or an '*' */
          if ((*cur) == '*' || __isdigit ((*cur)))
            width = __get_width_or_precision (&cur, ap);

          /* If the next char is a '.' then we expect a precision field */
          if ((*cur) == '.')
            {
              cur++;
              precision = __get_width_or_precision (&cur, ap);
            }

          /* We then have the optional length field */
          switch ((*cur))
            {
            case 'h':
              cur++;
              if ((*cur) == 'h')
                {
                  cur++;
                  flags |= CHAR_SIZE_FLAG;
                }
              else
                {
                  flags |= SHORT_FLAG;
                }
              break;
            case 'l':
              cur++;
              if ((*cur) == 'l')
                {
                  cur++;
                  flags |= LONG_LONG_FLAG;
                }
              else
                {
                  flags |= LONG_FLAG;
                }
              break;
            case 'L':
              flags |= LONG_DOUBLE_FLAG;
              break;
            case 'z':
              flags |= SIZE_T_FLAG;
              break;
            case 'j':
              flags |= INTMAX_T_FLAG;
              break;
            case 't':
              flags |= PTRDIFF_T_FLAG;
              break;
            default:
              break;
            }

          /* Finally we have the required type field */
          switch ((*cur))
            {
            case '%':
              if (EOF == fputc ('%', stream))
                return -1;

              done++;
              break;
            case 'd':
            case 'i':
              if (__itoa (&done, stream, flags | SIGNED_DECIMAL_FLAG, width, precision, ap))
                return -1;
              break;
            case 'u':
              if (__itoa (&done, stream, flags | UNSIGNED_DECIMAL_FLAG, width, precision, ap))
                return -1;
              break;
            case 'f':
              if (__ftoa (&done, stream, flags | FLOAT_NORMAL_DOWNCASE_FLAG, width, precision, ap))
                return -1;
              break;
            case 'F':
              if (__ftoa (&done, stream, flags | FLOAT_NORMAL_UPCASE_FLAG, width, precision, ap))
                return -1;
              break;
            case 'e':
              if (__ftoa (&done, stream, flags | FLOAT_EXPONENT_DOWNCASE_FLAG, width, precision, ap))
                return -1;
              break;
            case 'E':
              if (__ftoa (&done, stream, flags | FLOAT_EXPONENT_UPCASE_FLAG, width, precision, ap))
                return -1;
              break;
            case 'g':
              if (__ftoa (&done, stream, flags | FLOAT_CHOOSE_DOWNCASE_FLAG, width, precision, ap))
                return -1;
              break;
            case 'G':
              if (__ftoa (&done, stream, flags | FLOAT_CHOOSE_UPCASE_FLAG, width, precision, ap))
                return -1;
              break;
            case 'x':
              if (__itoa (&done, stream, flags | INT_HEX_DOWNCASE_FLAG, width, precision, ap))
                return -1;
              break;
            case 'X':
              if (__itoa (&done, stream, flags | INT_HEX_UPCASE_FLAG, width, precision, ap))
                return -1;
              break;
            case 'o':
              if (__itoa (&done, stream, flags | OCT_FLAG, width, precision, ap))
                return -1;
              break;
            case 's':
                {
                  int len = 0;
                  const char *str = va_arg(ap, const char *);
                  while (str[len] != '\0')
                    len ++;
                  
                  int n_chars;
                  if (precision > 0 && precision < len)
                    n_chars = precision;
                  else
                    n_chars = len;
                  
                  int needed_padding = width - n_chars;
                  
                  while (needed_padding > 0 && !(flags & MINUS_FLAG))
                    {
                      if (EOF == fputc (' ', stream))
                        return -1;
                      done ++;
                      needed_padding --;
                    }
                  
                  for (int i = 0; i < n_chars; i ++)
                    {
                      if (EOF == fputc (str[i], stream))
                        return -1;
                      done ++;
                    }
                  
                  while (needed_padding > 0 && (flags & MINUS_FLAG))
                    {
                      if (EOF == fputc (' ', stream))
                        return -1;
                      done ++;
                      needed_padding --;
                    }
                }
              break;
            case 'c':
              if (width > 1 && !(flags & MINUS_FLAG))
                {
                  for (int i = 0; i < (width - 1); i++)
                    {
                      if (EOF == fputc(' ', stream))
                        return -1;
                      done ++;
                    }
                }
              
              if (EOF == fputc (va_arg (ap, int), stream))
                return -1;
              done ++;
              
              if (width > 1 && (flags & MINUS_FLAG))
                {
                  for (int i = 0; i < (width - 1); i++)
                    {
                      if (EOF == fputc(' ', stream))
                        return -1;
                      done ++;
                    }
                }
              break;
            case 'p':
              if (__itoa (&done, stream, flags | POINTER_FLAG, width, precision, ap))
                return -1;
              break;
            case 'a':
              if (__ftoa (&done, stream, flags | FLOAT_HEX_DOWNCASE_FLAG, width, precision, ap))
                return -1;
            case 'A':
              if (__ftoa (&done, stream, flags | FLOAT_HEX_UPCASE_FLAG, width, precision, ap))
                return -1;
              break;
            case 'n':
                {
                  int *arg = va_arg(ap, int *);
                  (*arg) = done;
                }
              break;
            }
          cur++;
        }
        /* This is not a formatted section, just print to the screen. */
      else
        {
          if (EOF == fputc ((*cur), stream))
            return -1;

          done++;
          cur++;
        }
    }

  return done;
}
