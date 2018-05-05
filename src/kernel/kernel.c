#include <stdio.h>

#include "arch/i386/multiboot.h"

void
kernel_main(void)
{
  printf ("mbi = %p\n", mbi);
}
