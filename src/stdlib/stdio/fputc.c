#include <stdio.h>
#include <file.h>

/**
 * @brief      Writes a character to a stream without checking the file
 *             lock.
 *
 * @details    Writes a character, cast to an @verbatim unsigned char
 *             @endverbatim, to a stream. Completely ignores the file
 *             lock.
 *
 * @param      c Character to write.
 * @param      stream Stream to write to.
 *
 * @return     The character written on success and @verbatim EOF
 *             @endverbatim
 */
int fputc_unlocked(int c, FILE *stream) {
  unsigned char data = (unsigned char)c;
  if (1 == fwrite(&data, 1, 1, stream))
    return c;
  else
    return EOF;
}

/**
 * @brief      Writes a character to a stream.
 *
 * @details    Writes a character, cast to an @verbatim unsigned char
 *             @endverbatim, to a stream.
 *
 * @param      c Character to write.
 * @param      stream Stream to write to.
 *
 * @return     The character written on success and @verbatim EOF
 *             @endverbatim
 */
int fputc(int c, FILE *stream) {
  __lock_file(stream);
  int ret = fputc_unlocked(c, stream);
  __unlock_file(stream);
  return ret;
}
