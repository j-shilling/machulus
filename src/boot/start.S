#define ASM_FILE
#include <memory.h>
#include <multiboot2.h>

	.extern kernel_main
	.extern boot_gdt_descr
	.extern boot_gdt_descr_higher_half

	.global _start

	.section .bss
	.align 16
boot_stack_bottom:
	.skip PAGE_SIZE
boot_stack_top:

	.section .rodata
error_no_multiboot_message:	.asciz "This was not booted from a Multiboot2 bootloaded."
error_no_cpuid_message:		.asciz "Cannot detect system features."
error_no_long_mode_message:	.asciz "Cannot start long mode on this system."
debug_message:			.asciz "Hello, World!!!!"

	.section .text
	.code32
	.global _start
	.type _start, @function
_start:
	cli
	// Check Multiboot2 magic number
	cmpl	$(MULTIBOOT2_BOOTLOADER_MAGIC),%eax
	jne	error_no_multiboot

	// Load out GDT and set the code segment descriptor
	lgdt	boot_gdt_descr - KERNEL_OFFSET
	ljmp	$8,$(cs_set - KERNEL_OFFSET)
cs_set:
	// Now clear the data segment descriptor
	movw	$0,%ax
	movw	%ax,%ds
	movw	%ax,%es
	movw	%ax,%fs
	movw	%ax,%gs
	// Set data segment descriptor
	movw	$24,%ax
	movw	%ax,%ds
	movw	%ax,%es
	movw	%ax,%fs
	movw	%ax,%gs

	// Set up a stack
	movl	$(boot_stack_top - KERNEL_OFFSET),%esp
	movl	$(boot_stack_bottom - KERNEL_OFFSET),%ebp

	// push the address of multiboot structure.
	push	%ebx

	// Check for the existance of CPUID by seeing if we can flip bit 21 in EFLAGS
	// First get EFLAGS in eax, save a copy in ecx, then try to change bit 21
	pushf
	pop	%eax
	movl	%eax,%ecx
	xor	$(1 << 21),%eax
	push	%eax
	popf
	// Get the potentially new value of EFLAGS and return it to it's original state
	pushf
	pop	%eax
	push	%ecx
	popf
	// If ecx == eax then CPUID is not supported
	xor	%eax,%ecx
	jz	error_no_cpuid

	// Use CPUID to check whether long mode is supported. Long mode is checked by
	// calling cpuid with eax = 0x8000_0001, so we first need to check that functions
	// greater than 0x8000_0000 are supported
	movl	$0x80000000,%eax
	cpuid
	cmpl	$0x80000001,%eax
	jl	error_no_long_mode

	jmp	debug

error_no_multiboot:
	movl	$(error_no_multiboot_message - KERNEL_OFFSET),%eax
	push	%eax
	call	_boot_puts
	addl	$4,%esp
	jmp	halt32
error_no_cpuid:
	movl	$(error_no_cpuid_message - KERNEL_OFFSET),%eax
	push	%eax
	call	_boot_puts
	addl	$4,%esp
	jmp	halt32
error_no_long_mode:
	movl	$(error_no_long_mode_message - KERNEL_OFFSET),%eax
	push	%eax
	call	_boot_puts
	addl	$4,%esp
	jmp	halt32
debug:
	movl	$(debug_message - KERNEL_OFFSET),%eax
	push	%eax
	call	_boot_puts
	addl	$4,%esp
	jmp	halt32
halt32:
	cli
1:	hlt
	jmp 1b

	.size _start, . - _start
