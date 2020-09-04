#include <stdio.h>
#include <halt.h>

void panic(const char *fmt, ...) {
  /* printf("PANIC: "); */

  /* va_list ap; */
  /* va_start(ap, fmt); */
  /* vprintf(fmt, ap); */
  /* va_end(ap); */
  puts("Kernel Panic");

  halt();
}
