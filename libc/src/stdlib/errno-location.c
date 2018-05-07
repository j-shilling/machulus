#include <sys/cdefs.h>

int *
__errno_location (void)
{
  static __THREAD int errno = 0;
  return &errno;
}