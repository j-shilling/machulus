#include <stdio.h>

void
kernel_main (void)
{
  void *ptr = (void *)0xb80000;
  printf ("\"%40p\"\n", ptr);
  printf ("\"%40p\"\n", &ptr);
}
