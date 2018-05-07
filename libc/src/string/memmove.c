#include <string.h>

void *
memmove (void *restrict dest, const void *restrict src, size_t n)
{
  size_t i;

  if (dest < src)
    {
      for (i = 0; i < n; i++)
	((char *)dest)[i] = ((const char *)src)[i];
    } else {
      for (i = n; i != 0; i --)
	((char *)dest)[i-1] = ((const char *)src)[i-1];
    }

  return dest;
}
