#include <string.h>

void *
memset (void *__ptr, int value, size_t size)
{
  unsigned char *ptr = (unsigned char *) __ptr;

  for (size_t i = 0; i < size; i++)
    ptr[i] = (unsigned char) value;

  return __ptr;
}
