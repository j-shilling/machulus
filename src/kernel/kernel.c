#include <stdio.h>
#include <kernel/multiboot.h>

void
kernel_main(multiboot_info_t *mbd, unsigned int magic)
{
  printf ("Hello, World!\n");
  printf ("\n");
  printf ("|%-10d|\n", -10);
  printf ("|% 010d|\n", 10);
}
