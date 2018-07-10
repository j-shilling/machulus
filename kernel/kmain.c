#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

extern int _kernel_start;
extern int _kernel_end;

void
kmain(void)
{
  printf ("OK\n");
}
