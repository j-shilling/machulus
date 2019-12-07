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
 * followed by a newline character ('\n').
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
 * @details    detailed description
 *
 * @params[in] format The format string.
 *
 * @params[in] ... Additional arguments as required by the format
 * string.
 *
 * @return On success, returns the number of printed characters; On
 * failure, returns @c -1.
 */
int printf(const char *format, ...);

/**
 * @brief      function description
 *
 * @copydetails printf
 *
 * @return     return type
 */
#define vprintf(format, ap) vfprintf(stdout, format, ap);

/**
 * @brief      function description
 *
 * @details    detailed description
 *
 * @return     return type
 */
int vfprintf(FILE *stream, const char *format, va_list ap);
