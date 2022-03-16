#include <stdio.h>

#include <machulus/tty.h>

void kernel_main() {
  tty_init();

  puts("Hello, World!");
  puts("Welcome to the kernel.");
}
