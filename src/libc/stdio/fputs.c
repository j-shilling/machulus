
#include <stdio.h>

int
fputs (const char *s, FILE *stream)
{
  for (size_t i = 0; s && s[i]; i++)
    {
      if (EOF == fputc (s[i], stream))
        return EOF;
    }

  return 0;
}
