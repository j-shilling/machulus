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

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "printf_parser.h"

/* Formatting options */
static const uint32_t MINUS_FLAG = (1 << 1);
static const uint32_t PLUS_FLAG = (1 << 2);
static const uint32_t SPACE_FLAG = (1 << 3);
static const uint32_t ZERO_FLAG = (1 << 4);
static const uint32_t HASH_FLAG = (1 << 5);

/* Size options */
static const uint32_t CHAR_SIZE_FLAG = (1 << 6);
static const uint32_t SHORT_FLAG = (1 << 7);
static const uint32_t LONG_FLAG = (1 << 8);
static const uint32_t LONG_LONG_FLAG = (1 << 9);
static const uint32_t LONG_DOUBLE_FLAG = (1 << 10);
static const uint32_t SIZE_T_FLAG = (1 << 11);
static const uint32_t INTMAX_T_FLAG = (1 << 12);
static const uint32_t PTRDIFF_T_FLAG = (1 << 13);

/* Type options */
static const uint32_t PERCENT_FLAG = (1 << 14);
static const uint32_t SIGNED_DECIMAL_FLAG = (1 << 15);
static const uint32_t UNSIGNED_DECIMAL_FLAG = (1 << 16);
static const uint32_t FLOAT_NORMAL_DOWNCASE_FLAG = (1 << 17);
static const uint32_t FLOAT_NORMAL_UPCASE_FLAG = (1 << 18);
static const uint32_t FLOAT_EXPONENT_DOWNCASE_FLAG = (1 << 19);
static const uint32_t FLOAT_EXPONENT_UPCASE_FLAG = (1 << 20);
static const uint32_t FLOAT_CHOOSE_DOWNCASE_FLAG = (1 << 21);
static const uint32_t FLOAT_CHOOSE_UPCASE_FLAG = (1 << 22);
static const uint32_t INT_HEX_DOWNCASE_FLAG = (1 << 23);
static const uint32_t INT_HEX_UPCASE_FLAG = (1 << 24);
static const uint32_t OCT_FLAG = (1 << 25);
static const uint32_t STRING_FLAG = (1 << 26);
static const uint32_t CHAR_TYPE_FLAG = (1 << 27);
static const uint32_t POINTER_FLAG = (1 << 28);
static const uint32_t FLOAT_HEX_DOWNCASE_FLAG = (1 << 29);
static const uint32_t FLOAT_HEX_UPCASE_FLAG = (1 << 30);
static const uint32_t COUNT_FLAG = (1 << 31);

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
__get_width_or_precision (char **__fmt, va_list ap)
{
  char *fmt = (*__fmt);
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

static void
__assign_signed_integer_argument (format_string *fs, va_list ap)
{
  if (fs->flags & CHAR_SIZE_FLAG)
    fs->argument.as_char = (char) va_arg (ap, int);
  else if (fs->flags & SHORT_FLAG)
    fs->argument.as_short = (short) va_arg (ap, int);
  else if (fs->flags & LONG_FLAG)
    fs->argument.as_long = va_arg (ap, long);
  else if (fs->flags & LONG_LONG_FLAG)
    fs->argument.as_long_long = va_arg (ap, long long);
  else if (fs->flags & SIZE_T_FLAG)
    fs->argument.as_size_t = va_arg (ap, size_t);
  else if (fs->flags & INTMAX_T_FLAG)
    fs->argument.as_intmax_t = va_arg (ap, intmax_t);
  else if (fs->flags & PTRDIFF_T_FLAG)
    fs->argument.as_ptrdiff_t = va_arg (ap, ptrdiff_t);
  else
    fs->argument.as_int = va_arg (ap, int);
}

static void
__assign_unsigned_integer_argument (format_string *fs, va_list ap)
{
  if (fs->flags & CHAR_SIZE_FLAG)
    fs->argument.as_unsigned_char = (unsigned char) va_arg (ap, unsigned int);
  else if (fs->flags & SHORT_FLAG)
    fs->argument.as_unsigned_short = (short) va_arg (ap, unsigned int);
  else if (fs->flags & LONG_FLAG)
    fs->argument.as_unsigned_long = va_arg (ap, unsigned long);
  else if (fs->flags & LONG_LONG_FLAG)
    fs->argument.as_unsigned_long_long = va_arg (ap, unsigned long long);
  else if (fs->flags & SIZE_T_FLAG)
    fs->argument.as_size_t = va_arg (ap, size_t);
  else if (fs->flags & INTMAX_T_FLAG)
    fs->argument.as_uintmax_t = va_arg (ap, uintmax_t);
  else if (fs->flags & PTRDIFF_T_FLAG)
    fs->argument.as_ptrdiff_t = va_arg (ap, ptrdiff_t);
  else
    fs->argument.as_unsigned_int = va_arg (ap, unsigned int);
}

static void
__assign_float_argument (format_string *fs, va_list ap)
{
  if (fs->flags & LONG_DOUBLE_FLAG)
    fs->argument.as_long_double = va_arg (ap, long double);
  else
    fs->argument.as_double = va_arg (ap, double);
}

static void
__init_itoa_state (format_string *fs)
{
  fs->state.itoa.printed_sign = false;
  fs->state.itoa.printed_digits = 0;
  fs->state.itoa.printed_padding = 0;

  uintmax_t temp;
  if (fs->flags & SIGNED_DECIMAL_FLAG)
    {
      if (fs->flags & CHAR_SIZE_FLAG)
        {
          fs->state.itoa.is_negative = fs->argument.as_char < 0;
          temp = (uintmax_t) (fs->state.itoa.is_negative ? fs->argument.as_char * -1 : fs->argument.as_char);
        }
      else if (fs->flags & SHORT_FLAG)
        {
          fs->state.itoa.is_negative = fs->argument.as_short < 0;
          temp = (uintmax_t) (fs->state.itoa.is_negative ? fs->argument.as_short * -1 : fs->argument.as_short);
        }
      else if (fs->flags & LONG_FLAG)
        {
          fs->state.itoa.is_negative = fs->argument.as_long < 0;
          temp = (uintmax_t) (fs->state.itoa.is_negative ? fs->argument.as_long * -1 : fs->argument.as_long);
        }
      else if (fs->flags & LONG_LONG_FLAG)
        {
          fs->state.itoa.is_negative = fs->argument.as_long_long < 0;
          temp = (uintmax_t) (fs->state.itoa.is_negative ? fs->argument.as_long_long * -1 : fs->argument.as_long_long);
        }
      else if (fs->flags & INTMAX_T_FLAG)
        {
          fs->state.itoa.is_negative = fs->argument.as_intmax_t < 0;
          temp = (uintmax_t) (fs->state.itoa.is_negative ? fs->argument.as_intmax_t * -1 : fs->argument.as_intmax_t);
        }
      else if (fs->flags & PTRDIFF_T_FLAG)
        {
          fs->state.itoa.is_negative = fs->argument.as_ptrdiff_t < 0;
          temp = (uintmax_t) (fs->state.itoa.is_negative ? fs->argument.as_ptrdiff_t * -1 : fs->argument.as_ptrdiff_t);
        }
      else if (fs->flags & SIZE_T_FLAG)
        {
          fs->state.itoa.is_negative = false;
          temp = fs->argument.as_uintmax_t;
        }
      else
        {
          fs->state.itoa.is_negative = fs->argument.as_int < 0;
          temp = (uintmax_t) (fs->state.itoa.is_negative ? fs->argument.as_int * -1 : fs->argument.as_int);
        }
    }
  else
    {
      fs->state.itoa.is_negative = false;
      temp = fs->argument.as_uintmax_t;
    }

  int base;
  if (fs->flags & OCT_FLAG)
    base = 8;
  else if ((fs->flags & INT_HEX_DOWNCASE_FLAG) || (fs->flags & INT_HEX_UPCASE_FLAG))
    base = 16;
  else
    base = 10;

  fs->state.itoa.needed_digits = 0;
  do
    {
      fs->state.itoa.needed_digits++;
      temp /= base;
    }
  while (temp > 0);
}

/* Compile a formatted section of a printf-string. fmt should point to the beginning
   of the formatted section and (*fmt) should equal '%'. We will start parsing the
   string from that point, taking any necessary argumenst from ap if necessary, and
   building an object at the address specified by fs. Returns a pointer to the last
   parsed character, or NULL on error.
 
   The idea here is to let us implement a printf like function without a heap. The
   resulting structure should be put on the caller's stack and then filled by this
   function. */
char *
printf_parser_compile (format_string *fs, char *fmt, va_list ap)
{
  /* Check input as much as we can */
  if (NULL == fs || NULL == fmt || '%' != (*fmt))
    return fmt;

  /* Initialize fs */
  fs->flags = 0;
  fs->precision = -1;
  fs->width = -1;

  /* Things look good. Move a step through fmt */
  fmt++;

  /* Start with the optional flags: '-', '+', ' ', '0', and '#' */
read_flags:
  switch ((*fmt))
    {
    case '-':
      fs->flags |= MINUS_FLAG;
      fmt++;
      goto read_flags;
    case '+':
      fs->flags |= PLUS_FLAG;
      fmt++;
      goto read_flags;
    case ' ':
      fs->flags |= SPACE_FLAG;
      fmt++;
      goto read_flags;
    case '0':
      fs->flags |= ZERO_FLAG;
      fmt++;
      goto read_flags;
    case '#':
      fs->flags |= HASH_FLAG;
      fmt++;
      goto read_flags;
    default:
      break;
    }

  /* Next we might have a width field, which can either be a positive decimal
     integer or an '*' */
  if ((*fmt) == '*' || __isdigit ((*fmt)))
    fs->width = __get_width_or_precision (&fmt, ap);

  /* If the next char is a '.' then we expect a precision field */
  if ((*fmt) == '.')
    {
      fmt++;
      fs->precision = __get_width_or_precision (&fmt, ap);
    }

  /* We then have the optional length field */
  switch ((*fmt))
    {
    case 'h':
      fmt++;
      if ((*fmt) == 'h')
        {
          fmt++;
          fs->flags |= CHAR_SIZE_FLAG;
        }
      else
        {
          fs->flags |= SHORT_FLAG;
        }
      break;
    case 'l':
      fmt++;
      if ((*fmt) == 'l')
        {
          fmt++;
          fs->flags |= LONG_LONG_FLAG;
        }
      else
        {
          fs->flags |= LONG_FLAG;
        }
      break;
    case 'L':
      fs->flags |= LONG_DOUBLE_FLAG;
      break;
    case 'z':
      fs->flags |= SIZE_T_FLAG;
      break;
    case 'j':
      fs->flags |= INTMAX_T_FLAG;
      break;
    case 't':
      fs->flags |= PTRDIFF_T_FLAG;
      break;
    default:
      break;
    }

  /* Finally we have the required type field */
  switch ((*fmt))
    {
    case '%':
      fs->flags |= PERCENT_FLAG;
      fs->state.percent.shown = false;
      break;
    case 'd':
    case 'i':
      fs->flags |= SIGNED_DECIMAL_FLAG;
      __assign_signed_integer_argument (fs, ap);
      __init_itoa_state (fs);
      break;
    case 'u':
      fs->flags |= UNSIGNED_DECIMAL_FLAG;
      __assign_unsigned_integer_argument (fs, ap);
      __init_itoa_state (fs);
      break;
    case 'f':
      fs->flags |= FLOAT_NORMAL_DOWNCASE_FLAG;
      __assign_float_argument (fs, ap);
      break;
    case 'F':
      fs->flags |= FLOAT_NORMAL_UPCASE_FLAG;
      __assign_float_argument (fs, ap);
      break;
    case 'e':
      fs->flags |= FLOAT_EXPONENT_DOWNCASE_FLAG;
      __assign_float_argument (fs, ap);
      break;
    case 'E':
      fs->flags |= FLOAT_EXPONENT_UPCASE_FLAG;
      __assign_float_argument (fs, ap);
      break;
    case 'g':
      fs->flags |= FLOAT_CHOOSE_DOWNCASE_FLAG;
      __assign_float_argument (fs, ap);
      break;
    case 'G':
      fs->flags |= FLOAT_CHOOSE_UPCASE_FLAG;
      __assign_float_argument (fs, ap);
      break;
    case 'x':
      fs->flags |= INT_HEX_DOWNCASE_FLAG;
      __assign_unsigned_integer_argument (fs, ap);
      __init_itoa_state (fs);
      break;
    case 'X':
      fs->flags |= INT_HEX_UPCASE_FLAG;
      __assign_unsigned_integer_argument (fs, ap);
      __init_itoa_state (fs);
      break;
    case 'o':
      fs->flags |= OCT_FLAG;
      __assign_unsigned_integer_argument (fs, ap);
      __init_itoa_state (fs);
      break;
    case 's':
      fs->flags |= STRING_FLAG;
      fs->argument.as_char_pointer = va_arg (ap, char *);
      break;
    case 'c':
      fs->flags |= CHAR_TYPE_FLAG;
      fs->argument.as_char = (char) va_arg (ap, int);
      break;
    case 'p':
      fs->flags |= POINTER_FLAG;
      fs->argument.as_pointer = va_arg (ap, void *);
      break;
    case 'a':
      fs->flags |= FLOAT_HEX_DOWNCASE_FLAG;
      if (fs->flags & LONG_DOUBLE_FLAG)
        fs->argument.as_long_double = va_arg (ap, long double);
      else
        fs->argument.as_double = va_arg (ap, double);
      break;
    case 'A':
      fs->flags |= FLOAT_HEX_UPCASE_FLAG;
      __assign_float_argument (fs, ap);
      break;
    case 'n':
      fs->flags |= COUNT_FLAG;
      fs->argument.as_int_pointer = va_arg (ap, int *);
      break;
    }

  /* fmt should still be pointing at the type character */
  return fmt;
}

static char
__itoa (format_string *fs)
{
  /* Get the base */
  int base;
  if (fs->flags & OCT_FLAG)
    base = 8;
  else if ((fs->flags & INT_HEX_DOWNCASE_FLAG) || (fs->flags & INT_HEX_UPCASE_FLAG))
    base = 16;
  else
    base = 10;
  
  /* Figure out the amount of padding we will need */
  int needed_prefix_chars = 0;
  if (fs->flags & HASH_FLAG)
    {
      if (base == 8)
        needed_prefix_chars = 1;
      else if (base == 16)
        needed_prefix_chars = 2;
    }
  int needed_chars = fs->width > fs->precision ? fs->width : fs->precision;
  if (fs->flags & SIGNED_DECIMAL_FLAG && (fs->state.itoa.is_negative || (fs->flags & SPACE_FLAG) || (fs->flags & PLUS_FLAG)))
    needed_chars--;
  int needed_padding =
          needed_chars - fs->state.itoa.needed_digits - fs->state.itoa.printed_padding;
  
  /* Space padding comes first */
  if (needed_padding > 0 && !(fs->flags & MINUS_FLAG) && !(fs->flags & ZERO_FLAG))
    {
      fs->state.itoa.printed_padding++;
      return ' ';
    }

  /* See if we need to print a prefix. */
  if ((base != 10) && (fs->flags & HASH_FLAG))
    {
      if (fs->state.itoa.printed_prefix_chars < 1)
        {
          fs->state.itoa.printed_prefix_chars = 1;
          return '0';
        }
      else if (base == 16 && fs->state.itoa.printed_prefix_chars == 1)
        {
          fs->state.itoa.printed_prefix_chars = 2;
          if (fs->flags & INT_HEX_DOWNCASE_FLAG)
            return 'x';
          else
            return 'X';
        }
    }

  /* See if we need to print a sign */
  if (fs->flags & SIGNED_DECIMAL_FLAG && !fs->state.itoa.printed_sign)
    {
      if (fs->state.itoa.is_negative)
        {
          fs->state.itoa.printed_sign = true;
          return '-';
        }
      else if (fs->flags & PLUS_FLAG)
        {
          fs->state.itoa.printed_sign = true;
          return '+';
        }
      else if (fs->flags & SPACE_FLAG)
        {
          fs->state.itoa.printed_sign = true;
          return ' ';
        }
    }

  /* Zero padding comes after the sign and prefix */
  if (needed_padding > 0 && !(fs->flags & MINUS_FLAG) && (fs->flags & ZERO_FLAG))
    {
      fs->state.itoa.printed_padding++;
      return '0';
    }

  if (fs->state.itoa.printed_digits < fs->state.itoa.needed_digits)
    {
      /* Get the argument as a uintmax_t. This is only tricky when trying to cast
         negative values */
      uintmax_t temp;
      if (fs->state.itoa.is_negative)
        {
          if (fs->flags && CHAR_SIZE_FLAG)
            temp = (uintmax_t) (fs->argument.as_char * -1);
          else if (fs->flags && SHORT_FLAG)
            temp = (uintmax_t) (fs->argument.as_short * -1);
          else if (fs->flags && LONG_FLAG)
            temp = (uintmax_t) (fs->argument.as_long * -1);
          else if (fs->flags && LONG_LONG_FLAG)
            temp = (uintmax_t) (fs->argument.as_long_long * -1);
          else if (fs->flags && INTMAX_T_FLAG)
            temp = (uintmax_t) (fs->argument.as_intmax_t * -1);
          else if (fs->flags && PTRDIFF_T_FLAG)
            temp = (uintmax_t) (fs->argument.as_ptrdiff_t * -1);
          else
            temp = (uintmax_t) (fs->argument.as_int * -1);
        }
      else
        {
          temp = fs->argument.as_uintmax_t;
        }

      if (temp == 0)
        {
          fs->state.itoa.printed_digits++;
          return '0';
        }

      /* We want to now get the nth digit where n = printed_digits */
      int cur_digit = fs->state.itoa.needed_digits;
      int cur_value;

      do
        {
          cur_value = temp % base;
          temp /= base;
          cur_digit--;
        }
      while ((temp > 0) && (cur_digit > fs->state.itoa.printed_digits));

      fs->state.itoa.printed_digits++;
      needed_chars--;
      if (cur_value < 10)
        return '0' + cur_value;
      else
        {
          if (fs->flags & INT_HEX_DOWNCASE_FLAG)
            {
              return 'a' + (cur_value - 10);
            }
          else
            {
              return 'A' + (cur_value - 10);
            }
        }
    }

  if ((needed_padding > 0) && (fs->flags & MINUS_FLAG))
    {
      fs->state.itoa.printed_padding++;
      return ' ';
    }

  return '\0';
}

/* Returns the next character of section of formatted output from a printf function. 
   The state of (*fs) will be updated so that a subsequent call will result in a 
   different return value. If no more chars should be printed or if there's an error
   return '\0'. */
char
printf_parser_next_char (format_string *fs)
{
  /* Sanity check the input */
  if (NULL == fs)
    return '\0';

  /* %% */
  if (fs->flags & PERCENT_FLAG)
    {
      if (fs->state.percent.shown)
        {
          return '\0';
        }
      else
        {
          fs->state.percent.shown = true;
          return '%';
        }
    }
  else if ((fs->flags & SIGNED_DECIMAL_FLAG) || (fs->flags & UNSIGNED_DECIMAL_FLAG)
        || (fs->flags & INT_HEX_DOWNCASE_FLAG) || (fs->flags & INT_HEX_UPCASE_FLAG)
        || (fs->flags & OCT_FLAG))
    {
      return __itoa(fs);
    }
}