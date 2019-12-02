#include <stdio.h>

extern void stdout_init(void);

static void term_print(const char *str) {
  size_t len = 0;
  for (const char *cur = str; *cur; cur++)
    len++;

  fwrite(str, 1, len, stdout);
}

void kernel_main() {
  stdout_init();

  term_print("Hello, World!\n");
  term_print("Welcome to the kernel.\n");
}
