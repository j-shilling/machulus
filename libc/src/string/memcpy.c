#include <string.h>

void *
memcpy (void *restrict __dest, const void * restrict __src, size_t size)
{
  unsigned char *dest = (unsigned char *) __dest;
  const unsigned char *src = (const unsigned char *) __src;

  for (size_t i = 0; i < size; i++)
    dest[i] = src[i];

  return __dest;
}
