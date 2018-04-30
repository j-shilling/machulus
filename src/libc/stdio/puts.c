#include <stdio.h>

int puts (const char *str) {
  if (EOF == fputs (str, stdout))
    return EOF;
  if (EOF == fputc ('\n', stdout))
    return EOF;
  
  return 0;
}
