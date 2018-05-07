#include <string.h>

char *
strcat(char *dest, const char *src)
{
  size_t i = strlen (dest);
  const char *cur;
  
  for (cur = src; *cur != '\0'; cur ++)
    dest[i++] = *cur;
  
  dest[i] = '\0';
  
  return dest;
}