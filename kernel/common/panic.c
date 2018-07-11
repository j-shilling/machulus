#include <kernel/panic.h>
#include <stdio.h>
#include <stdarg.h>

_Noreturn void __attribute__ ((format (printf, 1, 2)))
panic (const char *format, ...)
{
  printf ("KERNEL PANIC: ");
  
  va_list ap;
  va_start (ap, format);
  vprintf (format, ap);
  va_end (ap);

  for (;;)
    ;
  __builtin_unreachable();
}
