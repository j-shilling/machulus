#include <memory.h>

	.extern boot_p4
	.extern boot_p3
	.extern boot_gdt_descr_higher_half
	.extern kernel_main

	.section .text
	.code64
	.global _start64
	.type _start64, @function
_start64:
	// jump to higher half
	movabsq	$higher_half,%rax
	jmp	*%rax

higher_half:
	// Adjust the stack to point to kernel address space
	movq	$KERNEL_OFFSET,%rax
	addq	%rax,%rsp
	addq	%rax,%rbp

	// Reload GDT at its higher half address
	lgdt	boot_gdt_descr_higher_half

	// Remove the lower memory mappings from boot_p4
	movq	$boot_p4,%rax
	movq	$0x0,(%rax)
	movq	$boot_p3,%rax
	movq	$0x0,(%rax)
	invlpg	0

	// Enter C code
	callq	kernel_main

	cli
1:	hlt
	jmp 1b
	.size _start64, . - _start64
