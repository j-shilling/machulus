#include <stdlib.h>
#include <stdio.h>

extern int _kernel_start;
extern int _kernel_end;

void
kmain(void)
{
  printf ("Kernel at %p - %p\n", 
          &_kernel_start, 
          &_kernel_end);
}
