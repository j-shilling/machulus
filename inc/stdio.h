/**
 * @file stdio.h
 * @brief Provides a subset of the standard stdio.h definitions.
 */

#pragma once

#include <stdarg.h>
#include <stddef.h>

/**
 * @brief End-of-file return value.
 */
#define EOF -1

/**
 * An opaque structure containing information about a file.
 */
typedef struct FILE FILE;

/**
 * Standard output stream.
 */
extern FILE *stdout;

/**
 * @brief Writes binary data to a stream.
 *
 * @details Writes @c nmemb items of data, each @c size bytes long, to
 * the stream pointed to by @c stream, obtaining them from the
 * location given by @c ptr.
 *
 * @param[in] ptr The location of the data to write.
 * @param[in] size The length of each item.
 * @param[in] nmemb The number of items.
 * @param[in] stream The stream to write to.
 *
 * @return The number of items written. On success, this should be the
 * same value as @c nmemb.
 */
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);

/**
 * @brief The non-locking version of @c fwrite.
 *
 * @details The equivalent of @c fwrite, but does not check whether
 * @c stream is locked.
 *
 * @see fwrite
 */
size_t fwrite_unlocked(const void *ptr, size_t size, size_t nmemb,
                       FILE *stream);

/**
 * @brief Writes a character to a stream.
 *
 * @details Writes a character, cast to an @c unsigned @c char, to @c
 * stream.
 *
 * @param[in] c Character to write.
 * @param[in] stream Stream to write to.
 *
 * @return On success, returns the character written as an @c unsigned
 * @c char cast to an @c int. On falure, returns @c EOF.
 *
 * @see fputc_unlocked
 * @see putc
 * @see putchar
 */
int fputc(int c, FILE *stream);

/**
 * @brief The non-locking version of @c fputc.
 *
 * @details The equivalent of @c fputc, but does not check whether @c
 * stream is locked.
 *
 * @see fputc_unlocked
 * @see putc_unlocked
 * @see putchar
 */
int fputc_unlocked(int c, FILE *stream);

/**
 * @brief Writes a string to a stream.
 *
 * @details Writes the string @c s to @c stream, without its
 * terminated null byte.
 *
 * @param[in] s A null-terminated string to write.
 * @param[in] stream A stream to write to.
 *
 * @return On success, returns 0. On failure, returns @c EOF.
 *
 * @see fputs_unlocked
 * @see puts
 */
int fputs(const char *s, FILE *stream);

/**
 * @brief The non-locking version of @c fputs.
 *
 * @details The equivalent of @c fputs, but does not check whether @c
 * stream is locked.
 *
 * @see fputs
 * @see puts_unlocked
 */
int fputs_unlocked(const char *s, FILE *stream);

/**
 * @brief The same as @c fputc, but implemented as a macro.
 *
 * @see fputc
 * @see putc_unlocked
 */
#define putc(c, stream) fputc(c, stream)

/**
 * @brief The same as @c fputc_unlocked, but implemented as a macro.
 *
 * @see putc
 * @see fputc
 * @see fputc_unlocked
 */
#define putc_unlocked(c, stream) fputc_unlocked(c, stream)

/**
 * @brief Prints a single character to @c stdout.
 *
 * @details Takes an ascii code, @c c, casts it to an @c unsigned @c
 * char, and writes it to @c stdout.
 *
 * This:
 * @code{.c}
 * putchar(c)
 * @endcode
 *
 * Is equivalent to:
 * @code{.c}
 * putc(c, stdout)
 * @endcode
 *
 * @param[in] c The character code to write.
 *
 * @return The character written on success and @c EOF on failure.
 *
 * @see fputc
 * @see putc
 * @see putchar_unlocked
 */
#define putchar(c) putc(c, stdout)

/**
 * @brief The non-locking version of @c putchar.
 *
 * @details The equivalent of @c putchar, but does not check whether
 * @c stdout is locked.
 *
 * @see fputc_unlocked
 * @see putc_unlocked
 * @see putchar
 */
#define putchar_unlocked(c) putc_unlocked(c, stdout)

/**
 * @brief Writes a string and a newline to @c stdout.
 *
 * @details Writes a null-terminated string @c s to @c stdout,
 * followed by a newline character ('\\n').
 *
 * @return On success, returns 0. On failure, returns @c EOF.
 *
 * @see fputs
 * @see puts_unlocked
 */
int puts(const char *s);

/**
 * @brief The non-locking version of @c puts.
 *
 * @details The equivalent of @c puts, but does not check whether @c
 * stdout is locked.
 *
 * @see fputs_unlocked
 * @see puts
 */
int puts_unlocked(const char *s);

/**
 * @brief Prints a formatted string to @c stdout.
 *
 * @details This function primarily acts as a wrapper around @c
 * vfprintf, initializing and cleaning up the necessary @c va_list.
 *
 * @param[in] format The format string.
 * @param[in] ... Additional arguments as required by the format
 * string.
 *
 * @return On success, returns the number of printed characters; On
 * failure, returns @c -1.
 *
n * @see vfprintf
 */
int printf(const char *format, ...) __attribute__((format (printf, 1, 2)));

/**
 * @copybrief printf
 *
 * @details This is a convenience macro used to call @c vfprintf with
 * @c stdout.
 *
 * @param[in] format The format string.
 * @param[in] ap An already started @c va_list pointing to additional
 * arguments as required by the format string. This function will not
 * call @c va_end.
 *
 * @return On success, returns the number of printed characters; On
 * failure, returns @c -1.
 *
 * @see printf
 */
#define vprintf(format, ap) vfprintf(stdout, format, ap)

/**
 * @brief Prints a formatted string to a @c FILE.
 *
 * @details This is where the magic happens for the printf-family
 * functions. The format string is parsed and printed here. This
 * implementations only implements the core features needed by the
 * kernel and not any POSIX extensions.
 *
 * ## The Format String
 * A placeholder in the format string can be represented as:
 * @code
 * %[flags][width][.precision][length]type
 * @endcode
 *
 * ### The Flags field can consist of zero or more of the following characters
 * | Character  |  Meaning |
 * | ---------: | :------- |
 * | - (minus)  | Left-align the output of this placeholder. |
 * | + (plus)   | Prepend a plus for positive signed-numeric types. |
 * |   (space)  | Prepend a space for positive signed-numeric types. |
 * | 0 (zero)   | When the width option is specified. prepend zeros for numeric types. |
 * | # (hash)   | Use alternate forms. |
 *
 * ### The Width field
 * The Width field specifies the *minimum* number of characters to
 * output, but does not cause oversized fields to be truncated.
 *
 * The width field may be ommited, an integer value, or a dynamic
 * value when passed as another argument when indicated by an
 * asterisk. A dynamic value must be passed in as an @c int.
 *
 * ### The Precision field
 * The precision field usually specifies a *maximum* limit on the
 * output, depending on the particular formatting type. For floating
 * point numbers, it specifies the number of digits to the right of
 * the decimal point. For strings, it limits the number of characters
 * that should be written.
 *
 * The precision field may be ommited, an integer value, or a dynamic
 * value when passed as another argument when indicated by an
 * asterisk. A dynamic value must be passed in as an @c int.
 *
 * ### The length field can be left out or any of:
 * | Character  |  Meaning |
 * | ---------: | :------- |
 * | hh | For integer types, expect an @c int sized integer argument which was promotted from a @c char. |
 * | h  | For integer types, expect an @c int sized integer argument which was promotted from a @c short. |
 * | l  | For integer types, expect a @c long sized integer argument. For floating point types, this has no effect. |
 * | ll | For integer types, expect a @c long @c long sized integer argument. For floating point types, this has no effect. |
 * | L  | For floating point types, expect a @c long @c double sized argument. |
 * | z  | For integer types, expect a @c size_t sized integer argument. |
 * | j  | For integer types, expect a @c intmax_t sized integer argument. |
 * | t  | For integer types, expect a @c ptrdiff_t size integer argument. |
 *
 * ### The Type field can be any of:
 * | Character  |  Meaning |
 * | ---------: | :------- |
 * | %    | Print a literal @c % character (this type ignores any flags, width, precision, and length fields). |
 * | d, i | Print an integer argument as a signed, decimal number. |
 * | u    | Print an unsigned integer argument as decimal number. |
 * | f, F | Print a @c double argument in normal notation. |
 * | e, E | Print a @c double argument in standard form. |
 * | g, G | Print a @c double argument in either normal or standard form, whichever is shorter. |
 * | x, X | Print an unsigned integer as a hexadecimal number. |
 * | o    | Print an unsigned integer as an octal number. |
 * | s    | Print an null-terminated string. |
 * | c    | Print a character. |
 * | p    | Print a pointer value. |
 * | a, A | Print a @c double argument in hexadecimal notation. |
 * | n    | Print nothing, but write the number of characters successfully written so far to an integer pointer. |
 *
 * @param[in] stream The stream to write to.
 * @param[in] format The format string.
 * @param[in] ap An already started @c va_list pointing to additional
 * arguments as required by the format string. This function will not
 * call @c va_end.
 *
 * @return On success, returns the number of printed characters; On
 * failure, returns @c -1.
 *
 * @see printf
 *
 * @todo Type %
 * @todo Type c
 * @todo Type s
 * @todo Types d, i, u
 * @todo Types o, x, X
 * @todo Types f, F
 * @todo Type p
 * @todo Types e, E
 * @todo Types g, G
 * @todo Types a, A
 * @todo Type n
 * @todo flags
 * @todo width and precision
 */
int vfprintf(FILE *stream, const char *format, va_list ap);
