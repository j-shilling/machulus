#include <stdint.h>

#include "frames.h"

extern void tty_init (void);

void
arch_init(uint32_t mmap_addr, uint32_t mmap_length)
{
  tty_init();
  frames_init (mmap_addr, mmap_length);
  acpi_init();
}
