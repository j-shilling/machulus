_Noreturn void halt(void) {
  while (1) asm("hlt");
}
