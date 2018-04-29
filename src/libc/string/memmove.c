#include <string.h>

void *
memmove (void *__dest, const void *__src, size_t size)
{
  unsigned char *dest = (unsigned char *) __dest;
  const unsigned char * src = (const unsigned char *) __src;
  if (dest < src)
    {
      for (size_t i = 0; i < size; i++)
	dest[i] = src[i];
    } else {
      for (size_t i = size; i != 0; i --)
	dest[i-1] = src[i-1];
    }
  return __dest;
}
