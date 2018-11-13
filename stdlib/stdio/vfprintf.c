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
__ctoi (char c)
{
  return c - '0';
}

static int
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
          ret = (ret * 10) + __ctoi ((*fmt));
          fmt++;
        }
    }

  (*__fmt) = fmt;
  return ret;
}

static int
__atoi (int *__done, FILE *stream, uint_fast32_t flags, int width, int precision, va_list ap)
{
  int done = (*__done);
  bool is_negative;
  uintmax_t value;

  /* Get the value from ap */
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
      else if (flags & SIZE_T_FLAG)
        {
          size_t x = va_arg (ap, size_t);
          is_negative = false;
          value = (uintmax_t) x;
        }
      else
        {
          int x = va_arg (ap, int);
          is_negative = x < 0;
          value = (uintmax_t) (is_negative ? -x : x);
        }
    }
  else if (flags & POINTER_FLAG)
    {
      is_negative = false;
      value = (uintmax_t) ((intptr_t) va_arg (ap, void *));
      flags |= INT_HEX_DOWNCASE_FLAG | HASH_FLAG | ZERO_FLAG;
      width = sizeof (void *) / 4;
      precision = 0;
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
      else if (flags & INTMAX_T_FLAG)
        value = va_arg (ap, uintmax_t);
      else if (flags & PTRDIFF_T_FLAG)
        value = (uintmax_t) (va_arg (ap, ptrdiff_t));
      else if (flags & SIZE_T_FLAG)
        value = (uintmax_t) (va_arg (ap, size_t));
      else
        value = (uintmax_t) (va_arg (ap, unsigned int));
    }

  /* Get the base */
  int base;
  if (flags & (INT_HEX_DOWNCASE_FLAG | INT_HEX_UPCASE_FLAG))
    base = 16;
  else if (flags & OCT_FLAG)
    base = 8;
  else
    base = 10;

  /* Find the total number of needed digits */
  int needed_digits;
  if (value == 0)
    {
      needed_digits = 1;
    }
  else
    {
      uintmax_t temp = value;
      do
        {
          needed_digits++;
          temp /= base;
        }
      while (temp > 0);
    }

  /* Find the number of needed padding chars */
  int needed_padding = width > precision ? width : precision;
  needed_padding -= needed_digits;

  /* we might need a prefix */
  if (flags & HASH_FLAG)
    {
      if (base == 16)
        needed_padding -= 2;
      else if (base == 8)
        needed_padding--;
    }

  /* We might need a sign char */
  if (flags & SIGNED_DECIMAL_FLAG)
    {
      if (is_negative || (flags & (SPACE_FLAG | PLUS_FLAG)))
        needed_padding--;
    }

  /* If we are right-justified and space padding print spaces */
  if (!(flags & MINUS_FLAG) && !(flags & ZERO_FLAG))
    {
      while (needed_padding > 0)
        {
          if (EOF == fputc (' ', stream))
            return -1;
          done++;
          needed_padding--;
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
      if (flags & INT_HEX_DOWNCASE_FLAG)
        {
          if (EOF == fputs ("0x", stream))
            return -1;
          done++;
        }
      else if (flags & INT_HEX_UPCASE_FLAG)
        {
          if (EOF == fputs ("0X", stream))
            return -1;
          done++;
        }
      else if (flags & OCT_FLAG)
        {
          if (EOF == fputc ('0', stream))
            return -1;
          done++;
        }
    }

  /* If we are right-justified and zero padded that comes next */
  if (!(flags & MINUS_FLAG) && (flags & ZERO_FLAG))
    {
      while (needed_padding > 0)
        {
          if (EOF == fputc ('0', stream))
            return -1;
          done++;
          needed_padding--;
        }
    }

  /* Finally we get to the actual digits */
  char digits[needed_digits];
  if (value == 0)
    {
      digits[0] = '0';
    }
  else
    {
      int i = 0;
      do
        {
          int cur_digit = value % base;
          if (cur_digit < 10)
            digits[i++] = '0' + cur_digit;
          else if (flags & INT_HEX_DOWNCASE_FLAG)
            digits[i++] = 'a' + (cur_digit - 10);
          else
            digits[i++] = 'A' + (cur_digit - 10);
          value /= base;
        }
      while (value > 0);
    }
  
  /* Print the digits */
  for (int i = (needed_digits - 1); i >= 0; i --)
    {
      if (EOF == fputc(digits[i], stream))
        return -1;
      done ++;
    }
  
  /* If we are left justified print spaces */
  if (flags & MINUS_FLAG)
    {
      while (needed_padding > 0)
        {
          if (EOF == fputc(' ', stream))
            return -1;
          done ++;
          needed_padding --;
        }
    }

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
              if (__atoi (&done, stream, flags | SIGNED_DECIMAL_FLAG, width, precision, ap))
                return -1;
              break;
            case 'u':
              if (__atoi (&done, stream, flags | UNSIGNED_DECIMAL_FLAG, width, precision, ap))
                return -1;
              break;
            case 'f':
              break;
            case 'F':
              break;
            case 'e':
              break;
            case 'E':
              break;
            case 'g':
              break;
            case 'G':
              break;
            case 'x':
              if (__atoi (&done, stream, flags | INT_HEX_DOWNCASE_FLAG, width, precision, ap))
                return -1;
              break;
            case 'X':
              if (__atoi (&done, stream, flags | INT_HEX_UPCASE_FLAG, width, precision, ap))
                return -1;
              break;
            case 'o':
              if (__atoi (&done, stream, flags | OCT_FLAG, width, precision, ap))
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
              if (__atoi (&done, stream, flags | POINTER_FLAG, width, precision, ap))
                return -1;
              break;
            case 'a':
            case 'A':
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