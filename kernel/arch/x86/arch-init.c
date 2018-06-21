#include <stdint.h>
#include <errno.h>

#include "gdt.h"
#include "idt.h"
#include "frames.h"
#include "paging.h"
#include "multiboot.h"

#include <stdio.h>
#include <kernel/panic.h>

extern void tty_init (void);

void
arch_init (uint32_t mbi_addr, uint32_t mbi_magic)
{
  gdt_init();
  idt_init();
  tty_init();
  
  for (int i = 0; i < 256; i ++)
    idt_register_trap (i, (uint32_t)test_handler);
  
  
  asm ("int $0");
  
  if (MULTIBOOT_BOOTLOADER_MAGIC != mbi_magic)
    panic ("We were not loaded by a multiboot compatible bootloader.");
  
  multiboot_info_t *mbi = (multiboot_info_t *)(mbi_addr + KERNEL_VBASE);
  if (0 == (mbi->flags & (1 << 6)))
    panic ("The bootloader did not give us a memory map.");
  
  uint32_t mmap_addr = (uint32_t) (mbi->mmap_addr) + KERNEL_VBASE;
  uint32_t mmap_length = mbi->mmap_length;
    
  frame_init (mmap_addr, mmap_length);
}
