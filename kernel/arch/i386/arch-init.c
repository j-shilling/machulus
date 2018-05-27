#include <kernel/panic.h>
#include <stddef.h>
#include <stdint.h>

#include "multiboot.h"
#include "frames.h"

extern void tty_init (void);
//extern void paging_init (void);

void
arch_init(multiboot_memory_map_t  *mmap, uint32_t mmap_length)
{
  tty_init();

  if (NULL == mmap)
    panic ("We did not get a memory map from the bootloader.");

 // frames_init ((uint32_t)mmap, mmap_length);
  //paging_init ();
}
