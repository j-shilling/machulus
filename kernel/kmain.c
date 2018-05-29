#include <stdlib.h>
#include <stdio.h>

void
kmain(void)
{
  const char *msg = "Hello, World!";
  const size_t len = 13;

  char *s = malloc (len + 1);
  for (size_t i = 0; i < (len + 1); i++)
    s[i] = msg[i];

  printf ("%20s", s);
  free (s);
}
