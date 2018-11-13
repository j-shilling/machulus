#include <stdio.h>

void
kernel_main (void)
{
  int x = 100;
  printf("\"%p\"\n", &x);
}
