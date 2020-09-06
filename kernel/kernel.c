#include <stdio.h>
#include <gdt.h>
#include <idt.h>


extern void stdout_init(void);

void kernel_main() {
  gdt_init();
  idt_init();
  stdout_init();

  puts("Hello, World!");
  puts("Welcome to the kernel.");
}
