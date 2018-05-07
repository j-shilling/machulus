#include <string.h>

char *
strncat(char *dest, const char *src, size_t n)
{
  size_t i = strlen (dest);
  size_t j;
  
  for (j = 0; j < n; j ++)
    dest[i++] = src[j];
  
  dest[i] = '\0';
  
  return dest;
}