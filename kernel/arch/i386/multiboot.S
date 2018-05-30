#define ASM_FILE		1

#include "multiboot.h"

.section .text
	.global _get_mmap
	.type _get_mmap, @function
	/* int get_mmap (int multiboot_addr, int magic): check for multiboot's
	   memory map and return its physical address in ebx and length in eax */
	_get_mmap:
		push %ebp
		movl %esp, %ebp

		movl 12(%ebp), %edx
		cmpl $(MULTIBOOT_BOOTLOADER_MAGIC), %edx
		jne  2f

		movl 8(%ebp), %edx
		movl (%edx), %ecx
		andl $(1 << 6), %ecx
		jz   2f

		movl 48(%edx), %ebx
		movl 44(%edx), %eax
		jmp 3f

	2:	movl $0, %eax
		movl $0, %ebx

	3:	pop  %ebp
		ret
	.size _get_mmap, . - _get_mmap
