#include <stdio.h>

void
kernel_main (void)
{
  size_t len = 0x1234;
  printf ("\"%zu\"\n", len);
}
