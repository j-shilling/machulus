#define ASM_FILE 1
#include <multiboot2.h>
#include <ldscript.h>

#if __64bits__
OUTPUT_FORMAT("elf64-x86-64")
#else
OUTPUT_FORMAT("elf32-i386")
#endif /* __64bits__ */

ENTRY(_start)

SECTIONS
{
  /* Load kernel at KERNEL_OFFSET + 1MiB VMA */
  . = KERNEL_OFFSET + 0x00100000;

  .text : AT(ADDR(.text) - KERNEL_OFFSET) {
    KEEP(*(.multiboot_header))
    *(.text)
  }

  .rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_OFFSET) {
    *(.rodata)
  }

  .data ALIGN(4K) : AT(ADDR(.data) - KERNEL_OFFSET) {
    *(.boot_gdt)
    *(.boot_paging)
    *(.boot_tss)
    *(.boot_idt)
    *(.data)
  }
 
  .bss ALIGN(4K) : AT(ADDR(.bss) - KERNEL_OFFSET) {
    *(.bss)
  }
}
