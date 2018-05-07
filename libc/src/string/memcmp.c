#include <string.h>

int memcmp (const void *__x, const void *__y, size_t size)
{
  const unsigned char *x = (const unsigned char *) x;
  const unsigned char *y = (const unsigned char *) y;

  for (size_t i = 0; i < size; i ++)
    {
      if (x[i] < y[i])
	return -1;
      else if (y[i] > x[i])
	return 1;
    }

  return 0;
}
