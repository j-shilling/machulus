#include <string.h>

char *
strcpy (char *restrict dest, const char *src)
{
  size_t i = 0;
  const char *cur;
  
  for (cur = src; *cur != '\0'; cur ++)
    dest[i++] = *cur;
  
  dest[i] = '\0';
  
  return dest;
}