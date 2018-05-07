#include <string.h>

int
memcmp (const void *s1, const void *s2, size_t n)
{
  size_t i;

  for (i = 0; i < n; i ++)
    {
      char ret = ((const char *)s1)[i] - ((const char *)s2)[i];
      if (ret)
        return ret;
    }

  return 0;
}
