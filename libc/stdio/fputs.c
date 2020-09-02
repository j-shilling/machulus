#include <stdio.h>
#include <file.h>

/**
 * @brief      Writes a string to a stream without checking the file lock.
 *
 * @details    Writes a string to a stream without its terminating null
 *             byte. Completely ignores the file lock.
 *
 * @param      s The null terminated string.
 * @param      stream The stream to write to.
 *
 * @return     0 on success and @verbatim EOF @endverbatim on failure.
 */
int fputs_unlocked(const char *s, FILE *stream) {
  size_t len = 0;
  for (const char *cur = s; *cur; cur++)
    len ++;

  if (fwrite(s, 1, len, stream) < len)
    return EOF;
  else
    return 0;
}

/**
 * @brief      Writes a string to a stream.
 *
 * @details    Writes a string to a stream without its terminating null
 *             byte.
 *
 * @param      s The null terminated string.
 * @param      stream The stream to write to.
 *
 * @return     0 on success and @verbatim EOF @endverbatim on failure.
 */
int fputs(const char *s, FILE *stream) {
  __lock_file(stream);
  int ret = fputs_unlocked(s, stream);
  __unlock_file(stream);
  return ret;
}
