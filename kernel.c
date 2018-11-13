#include <stdio.h>

void
kernel_main (void)
{
  int val = 0;
  printf ("123%n456\n", &val);
  printf ("val = %d", val);
}
