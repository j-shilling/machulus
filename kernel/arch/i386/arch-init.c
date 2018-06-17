#include <kernel/panic.h>
#include <stddef.h>
#include <stdint.h>

#include "frames.h"
#include "paging.h"

extern void tty_init (void);

char *const test_page = (char *) 0xB0000000;

void
arch_init(uint32_t mmap_addr, uint32_t mmap_length)
{
  tty_init();
  frames_init (mmap_addr, mmap_length);
}
