#include <stdio.h>
#include <kernel/multiboot.h>

void
kernel_main(multiboot_info_t *mbd, unsigned int magic)
{
  printf ("Hello, World!\n");
  printf ("\n");
  printf ("Lower memory:\t%lu KB\n", mbd->mem_lower);
  printf ("Upper memory:\t%lu KB\n", mbd->mem_upper);
}
