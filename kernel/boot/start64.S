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

    /* Set up the TSS descriptor */

	movq	$(boot_gdt + 32),%rdi
.extern boot_tss
	movq	$boot_tss,%rax
	movw	%ax,2(%rdi)
	shrq	$16,%rax
	movb	%al,4(%rdi)
	movb	%ah,7(%rdi)
	shrq	$16,%rax
	movl	%eax,8(%rdi)

	/* Load the TSS */
	movw	$32,%ax
	ltr	%ax

/* Init and load the IDT */
.extern init_early_idt
	callq	init_early_idt

	// Enter C code
	callq	kernel_main

	cli
1:	hlt
	jmp 1b
	.size _start64, . - _start64
