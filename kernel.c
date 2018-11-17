#include <stdio.h>

void
kernel_main (void)
{
  int val = 0;
  printf ("\"%p\"", &val);
}
