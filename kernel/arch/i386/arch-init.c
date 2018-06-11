#include <stdint.h>
#include <stddef.h>

#include <kernel/panic.h>

#include "frames.h"
#include "multiboot.h"
#include "acpi.h"
#include "gdt.h"

extern void tty_init (void);

void
arch_init(multiboot_info_t *mbi, uint32_t magic)
{
  if (NULL == mbi || magic != MULTIBOOT_BOOTLOADER_MAGIC)
    panic ("We were not loaded by a multiboot compatible bootloader.\n");
  
  /* Initialize frame map */
  if (mbi->flags & (6 << 1))
    frames_init (mbi->mmap_addr, mbi->mmap_length);
  else
    panic ("We did not get a memory map from the bootloader.\n");
  
  /* Initialize tty output */
  tty_init();
  acpi_init();
  gdt_init();
}
