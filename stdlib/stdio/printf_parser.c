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
#include <stddef.h>

#include "printf_parser.h"

/* Formatting options */
static const uint32_t MINUS_FLAG                   = (1 << 1);
static const uint32_t PLUS_FLAG                    = (1 << 2);
static const uint32_t SPACE_FLAG                   = (1 << 3);
static const uint32_t ZERO_FLAG                    = (1 << 4);
static const uint32_t HASH_FLAG                    = (1 << 5);

/* Size options */
static const uint32_t CHAR_SIZE_FLAG               = (1 << 6);
static const uint32_t SHORT_FLAG                   = (1 << 7);
static const uint32_t LONG_FLAG                    = (1 << 8);
static const uint32_t LONG_LONG_FLAG               = (1 << 9);
static const uint32_t LONG_DOUBLE_FLAG             = (1 << 10);
static const uint32_t SIZE_T_FLAG                  = (1 << 11);
static const uint32_t INTMAX_T_FLAG                = (1 << 12);
static const uint32_t PTRDIFF_T_FLAG               = (1 << 13);

/* Type options */
static const uint32_t PERCENT_FLAG                 = (1 << 14);
static const uint32_t SIGNED_DECIMAL_FLAG          = (1 << 15);
static const uint32_t UNSIGNED_DECIMAL_FLAG        = (1 << 16);
static const uint32_t FLOAT_NOMRAL_DOWNCASE_FLAG   = (1 << 17);
static const uint32_t FLOAT_NORMAL_UPCASE_FLAG     = (1 << 18);
static const uint32_t FLOAT_EXPONENT_DOWNCASE_FLAG = (1 << 19);
static const uint32_t FLOAT_EXPONENT_UPCASE_FLAG   = (1 << 20);
static const uint32_t FLOAT_CHOOSE_DOWNCASE_FLAG   = (1 << 21);
static const uint32_t FLOAT_CHOOSE_UPCASE_FLAG     = (1 << 22);
static const uint32_t INT_HEX_DOWNCASE_FLAG        = (1 << 23);
static const uint32_t INT_HEX_UPCASE_FLAG          = (1 << 24);
static const uint32_t OCT_FLAG                     = (1 << 25);
static const uint32_t STRING_FLAG                  = (1 << 26);
static const uint32_t CHAR_TYPE_FLAG               = (1 << 27);
static const uint32_t POINTER_FLAG                 = (1 << 28);
static const uint32_t FLOAT_HEX_DOWNCASE_FLAG      = (1 << 29);
static const uint32_t FLOAT_HEX_UPCASE_FLAG        = (1 << 30);
static const uint32_t COUNT_FLAG                   = (1 << 31);

/* Compile a formatted section of a printf-string. fmt should point to the beginning
   of the formatted section and (*fmt) should equal '%'. We will start parsing the
   string from that point, taking any necessary argumenst from ap if necessary, and
   building an object at the address specified by fs. Returns a pointer to the last
   parsed character, or NULL on error.
 
   The idea here is to let us implement a printf like function without a heap. The
   resulting structure should be put on the caller's stack and then filled by this
   function. */
char *
__printf_parser_compile(__format_string *fs, char *fmt, va_list ap)
{
  return NULL;
}

/* Returns the nexted character of section of formatted output from a printf function. 
   The state of (*fs) will be updated so that a subsequent call will result in a 
   different return value. If no more chars should be printed or if there's an error
   return '\0'. */
char
__printf_parser_next_char(__format_string *fs)
{
  return '\0';
}