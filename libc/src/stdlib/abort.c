#include <stdlib.h>

_Noreturn void
abort (void)
{
  while (1) {}
  __builtin_unreachable();
}
