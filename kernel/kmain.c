#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

extern int _kernel_start;
extern int _kernel_end;

void
kmain(void)
{
  size_t ksize = (uintptr_t)(&_kernel_end) - (uintptr_t)(&_kernel_start);
  printf ("Kernel at %p - %p (%lu B %lu K)\n", 
          &_kernel_start, 
          &_kernel_end,
          ksize,
          (ksize / 1024));
}
