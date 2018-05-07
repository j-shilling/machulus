#include <string.h>

char *
strchr (const char *s, int c)
{
  if (NULL == s)
    return NULL;
  
  const char *cur;
  for (cur = s; *cur != '\0'; cur++)
    if (*cur == (char) c)
      return (char *) cur;
  
  return NULL;
}