#include <string.h>

char *
strrchr (const char *s, int c)
{
  if (NULL == s)
    return NULL;
  
  char *ret = NULL;
  
  const char *cur;
  for (cur = s; *cur != '\0'; cur ++)
    if (*cur == (char) c)
      ret = (char *) cur;
  
  return ret;
}