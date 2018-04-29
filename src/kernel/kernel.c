#include <kernel/tty.h>

void kernel_main()
{
  tty_init();
 
  tty_putchar (':');
  tty_putchar (')');
}
