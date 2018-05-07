#include <string.h>

int 
strncmp (const char *s1, const char *s2, size_t n)
{
  size_t i;
  
  for (i = 0; i < n; i ++)
    {
      int ret = s1[i] - s2[i];
      if (ret)
        return ret;
    }
  
  return 0;
}