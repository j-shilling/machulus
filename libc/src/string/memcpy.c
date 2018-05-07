#include <string.h>

void *
memcpy (void *restrict dest, const void *restrict src, size_t n)
{
  size_t i;
  for (i = 0; i < n; i++)
    ((char *)dest)[i] = ((const char *)src)[i];

  return dest;
}
