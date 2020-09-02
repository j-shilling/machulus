#include <stdio.h>
#include <file.h>

/**
 * @brief      Writes a string to stdout without checking the file lock.
 *
 * @details    Writes a string to stdout followed by a '\n'. Completely
 *             ignores the file lock.
 *
 * @param      s The null terminated string.
 *
 * @return     0 on success and @verbatim EOF @endverbatim on failure.
 */
int puts_unlocked(const char *s) {
  if (EOF == fputs_unlocked(s, stdout))
    return EOF;
  if (EOF == fputc_unlocked('\n', stdout))
    return EOF;

  return 0;
}

/**
 * @brief      Writes a string to stdout followed by a '\n'.
 *
 * @details    Writes a string to stdout followed by a '\n'.
 *
 * @param      s The null terminated string.
 *
 * @return     0 on success and @verbatim EOF @endverbatim on failure.
 */
int puts(const char *s) {
  __lock_file(stdout);
  int ret = puts_unlocked(s);
  __unlock_file(stdout);
  return ret;
}
