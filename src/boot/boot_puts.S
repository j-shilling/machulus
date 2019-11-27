	.global _boot_puts
	// We need a function that can print an error to the screen while still
	// in 32-bit mode.
	.code32
	.type _boot_puts, @function
_boot_puts:
	push	%ebp
	movl	%esp,%ebp

	// Clear the screen by filling it with spaces (ascii = 0x20)
	// Write (80 * 25) / 2 = 1000 times
	movl	$0xb8000,%edi
	movl	$0x07200720,%edx
	movl	$1000,%ecx
clear_screen:
	movl	%edx,(%edi)
	addl	$4,%edi
	loop	clear_screen

	// Next we need the address of the string passed on the stack
	movl	8(%esp),%esi
	movl	$0xb8000,%edi
	movb	$0x07,%ah
putchar:
	movb	(%esi),%al
	cmpb	$0,%al
	je	no_more_chars

	movw	%ax,(%edi)
	add	$2,%edi
	inc	%esi
	jmp	putchar

no_more_chars:
	pop	%ebp
	ret

	.size _boot_puts, . - _boot_puts
