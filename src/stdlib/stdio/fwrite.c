#include <file.h>
#include <stdio.h>

/**
 * @brief      Writes bytes to a stream.
 *
 * @details    Writes nmemb elements of data, each size bytes long, from
 *             the location given by pointer to the stream.
 *
 * @param      ptr The location from which to read data.
 * @param      size The size of each elements to write.
 * @param      nmemb The number of elements to write.
 * @param      stream The FILE to write data to.
 *
 * @return     The number of elements written.
 */
size_t fwrite_unlocked(const void *ptr, size_t size, size_t nmemb,
                       FILE *stream) {
  if (stream->fwrite) {
    return stream->fwrite(ptr, size, nmemb, stream);
  } else {
    return 0;
  }
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
  __lock_file(stream);
  return fwrite_unlocked(ptr, size, nmemb, stream);
  __unlock_file(stream);
}
