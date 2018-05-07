#include <string.h>

int 
strcmp (const char *s1, const char *s2)
{
  size_t i;
  
  for (i = 0; s1[i] != '\0' || s2[i] != '\0'; i++)
    {
      int ret = s1[i] - s2[i];
      if (ret)
        return ret;
    }
  
  return 0;
}