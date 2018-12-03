#include <stdio.h>

void
kernel_exception_handler(int vector, int error_code)
{
  printf("INT %d, ERR: %x\n", vector, error_code);
  return;
}
