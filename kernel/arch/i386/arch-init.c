#include <kernel/panic.h>
#include <stddef.h>

#include "multiboot.h"

extern void tty_init (void);

void
arch_init(multiboot_info_t *mbi)
{
  tty_init();

  if (NULL == mbi)
    panic ("We were not loaded by a multiboot compliant bootloader.");
}
