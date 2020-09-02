#include <stdio.h>

extern void stdout_init(void);

void kernel_main() {
  stdout_init();

  puts("Hello, World!");
  puts("Welcome to the kernel.");
}
