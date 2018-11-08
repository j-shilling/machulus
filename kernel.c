#include <stdio.h>

void
kernel_main (void)
{
  printf("Hello, World\n");
  
  printf("\n\n-- printf tests --\n");
  int x = 100;
  printf("\"%10d\"\n", x);
}
