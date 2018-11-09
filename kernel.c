#include <stdio.h>

void
kernel_main (void)
{
  printf("Hello, World\n");
  
  printf("\n\n-- printf tests --\n");
  char *str = "Hello, World!";
  printf("\"%-5.3s\"\n", str);
}
