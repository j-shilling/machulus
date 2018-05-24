extern void tty_init (void);

void
arch_init(void)
{
  tty_init();
}
