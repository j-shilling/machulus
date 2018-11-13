#include <stdio.h>

void
kernel_main (void)
{
  char *str = "Hello, World!";
  printf("\"%10.5s\"\n", str);
}
