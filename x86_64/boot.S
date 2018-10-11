#define ASM_FILE 1
#include <ldscript.h>
#include <multiboot2.h>

/* GRUB2 wants a multiboot2 header at the beginning of the kernel image. The
   linker script will put this in the right place */
.section .multiboot_header
.align 8
multiboot_header_start:
	.long	MULTIBOOT2_HEADER_MAGIC
	.long	MULTIBOOT_ARCHITECTURE_I386
	.long	multiboot_header_end - multiboot_header_start
	.long	-(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT_ARCHITECTURE_I386 + (multiboot_header_end - multiboot_header_start))
  
	.short	MULTIBOOT_HEADER_TAG_END
	.short	0
	.long	8
multiboot_header_end:

.section .boot_gdt
.align 16
boot_gdt:
	/* null descriptor */
	.quad 0
	/* 32-bit code */
	.word 0xFFFF
	.word 0x0
	.byte 0x0
	.byte 0x9a
	.byte 0xcf
	.byte 0x0
	/* 64-bit code */
	.word 0xFFFF
	.word 0x0
	.byte 0x0
	.byte 0x9a
	.byte 0xaf
	.byte 0x0
	/* data segment */
	.word 0xFFFF
	.word 0x0
	.byte 0x0
	.byte 0x92
	.byte 0xcf
	.byte 0x0
boot_gdt_end:

boot_gdt_descr:
	.word boot_gdt - boot_gdt_end
	.long boot_gdt - KERNEL_OFFSET

.section .bss
.align 16
boot_stack_bottom:
	.skip 16384 # 16 KiB
boot_stack_top:
 
/*
The linker script specifies _start as the entry point to the kernel and the
bootloader will jump to this position once the kernel has been loaded. It
doesn't make sense to return from this function as the bootloader is gone.
*/
.section .text
.code32
.global _start
.type _start, @function
_start:
	/* Multiboot2 does not gaurentee a valid gdt, so we need to load our own */
	lgdt	boot_gdt_descr - KERNEL_OFFSET
	ljmp	$8,$(cs_set - KERNEL_OFFSET)
cs_set:
	movw	$0,%ax
	movw	%ax,%ds
	movw	%ax,%es
	movw	%ax,%fs
	movw	%ax,%gs
	movw	$24,%ax
	movw	%ax,%ds
	movw	%ax,%es
	movw	%ax,%ss

	/* Set up a stack */
	movl	$(boot_stack_top - KERNEL_OFFSET), %esp

	/* Reset EFLAGS */

	cli
1:	hlt
	jmp 1b
 
.size _start, . - _start
