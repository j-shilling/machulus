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

#ifndef PRINTF_PARSER_H
#define PRINTF_PARSER_H

#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct
  {
    bool is_negative;
    bool is_signed;
    bool printed_sign;
    
    int base;
    int flags;

    int needed_digits;
    int needed_padding;
    int needed_prefix_chars;

    int printed_digits;
    int printed_prefix_chars;
    int printed_padding;
    
    uintmax_t value;
  } __itoa_state;
  
  typedef struct
  {
    bool char_shown;
    int needed_padding;
  } __char_state;

  typedef struct
  {
    int flags;
    int width;
    int precision;

    union
    {
      /* Signed Integer Values */
      char as_char;
      short as_short;
      int as_int;
      long as_long;
      long long as_long_long;
      intmax_t as_intmax_t;
      ptrdiff_t as_ptrdiff_t;

      /* Unsigned Integer Values */
      unsigned char as_unsigned_char;
      unsigned short as_unsigned_short;
      unsigned int as_unsigned_int;
      unsigned long as_unsigned_long;
      unsigned long long as_unsigned_long_long;
      uintmax_t as_uintmax_t;
      size_t as_size_t;

      /* Signed floating-points */
      double as_double;
      long double as_long_double;

      /* Pointer types */
      char *as_char_pointer;
      void *as_pointer;
      int *as_int_pointer;
    } argument;

    union
    {
      bool printed;
      __itoa_state itoa;
      __char_state chr;
    } state;

  } format_string;

  char *printf_parser_compile (format_string *, char *, va_list);
  char printf_parser_next_char (format_string *);

#ifdef __cplusplus
}
#endif

#endif /* PRINTF_PARSER_H */

