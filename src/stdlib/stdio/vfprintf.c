#include <ctype.h>
#include <errno.h>
#include <file.h>
#include <stdint.h>
#include <stdio.h>

static const int MINUS_FLAG = (1 << 1);
static const int PLUS_FLAG = (1 << 2);
static const int SPACE_FLAG = (1 << 3);
static const int ZERO_FLAG = (1 << 4);
static const int HASH_FLAG = (1 << 5);

static const int CHAR_SIZE = (1 << 6);
static const int SHORT_SIZE = (1 << 7);
static const int LONG_SIZE = (1 << 8);
static const int LONG_LONG_SIZE = (1 << 9);
static const int LONG_DOUBLE_SIZE = (1 << 10);

// I'm going to make some assumptions about the implementations of
// size_t, intmax_t and ptrdiff_t. These static asserts are here to
// ensure that the compiler agrees with my assumptions about the size
// of these types.
_Static_assert(sizeof(size_t) == sizeof(long), "size_t should = long");
_Static_assert(sizeof(ptrdiff_t) == sizeof(long), "ptrdiff_t should = long");
_Static_assert(sizeof(intmax_t) == sizeof(long long), "intax_t should = long");

static inline int __read_width_or_precision_field(const char **cur,
                                                  va_list ap) {
  return -1;
}

int vfprintf(FILE *stream, const char *format, va_list ap) {
  int done = 0;
  if (stream && format) {
    __lock_file(stream);
    const char *cur = format;
    while (*cur) {
      if (*cur == '%') {
        // We are at a place holder. Advance past the '%' char.
        cur++;

        // Check for any flags
        int flags = 0;
      read_flags_start:
        switch (*cur) {
        case '-':
          flags |= MINUS_FLAG;
          cur++;
          goto read_flags_start;
        case '+':
          flags |= PLUS_FLAG;
          cur++;
          goto read_flags_start;
        case ' ':
          flags |= SPACE_FLAG;
          cur++;
          goto read_flags_start;
        case '0':
          flags |= ZERO_FLAG;
          cur++;
          goto read_flags_start;
        case '#':
          flags |= HASH_FLAG;
          cur++;
          goto read_flags_start;
        default:
          break;
        } // switch (*cur)

        // If the next character is a digit, then we need to read a
        // width field.
        int width = -1;
        if (isdigit(*cur)) {
          width = __read_width_or_precision_field(&cur, ap);
          if (-1 == width) {
            errno = EINVAL;
            return -1;
          }
        }

        // If the next character is a . (dot), then we need to read a
        // precision field.
        int precision = -1;
        if (*cur == '.') {
          cur++;
          precision = __read_width_or_precision_field(&cur, ap);
          if (-1 == precision) {
            errno = EINVAL;
            return -1;
          }
        }

        // Check for a length field
        switch (*cur) {
        case 'h':
          cur++;
          if (*cur == 'h') {
            cur++;
            flags |= CHAR_SIZE;
          } else {
            flags |= SHORT_SIZE;
          }
          break;
        case 'l':
          cur++;
          if (*cur == 'l') {
            cur++;
            flags |= LONG_LONG_SIZE;
          } else {
            flags |= LONG_SIZE;
          }
          break;
        case 'L':
          cur++;
          flags |= LONG_DOUBLE_SIZE;
          break;
        case 'z':
          cur++;
          flags |= LONG_SIZE;
          break;
        case 'j':
          cur++;
          flags |= LONG_LONG_SIZE;
          break;
        case 't':
          cur++;
          flags |= LONG_SIZE;
          break;
        default:
          break;
        } // switch (*cur)

        // We should now be looking at a valid type char
        switch (*cur) {
        case '%':
        case 'd':
        case 'i':
        case 'u':
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
        case 'c':
        case 'p':
        case 'a':
        case 'A':
        case 'n':
        default:
          // Invalid type char
          errno = EINVAL;
          return -1;
        }

        // Increment past the type char
        cur++;
      } // if (*cur == '%')

      {
        // Count characters until the end of the string or the start
        // of a place holder.
        size_t nmemb = 0;
        while (cur[nmemb] && cur[nmemb] != '%')
          nmemb++;
        // Print these characters verbatim.
        size_t result = fwrite_unlocked(cur, 1, nmemb, stream);
        // Check for success or failure.
        if (result != nmemb)
          return -1;
        else
          done += result;
      }
    } // while (*cur)
    __unlock_file(stream);
  } // if (stream && format)
  return done;
}
