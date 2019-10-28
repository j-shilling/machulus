	.extern kernel_main
	.extern _multiboot_memory_map
	.extern _multiboot_available_ram

	.global start

	.set KERNEL_OFFSET, 0xffffffff80000000

	.set MB_HEADER_MAGIC, 0xe85250d6
	.set MB_BOOTLOADER_MAGIC, 0x36d76289
	.set MB_ARCH, 0x0
	.set MB_HEADER_SIZE, (multiboot_header_end - multiboot_header_start)
	.set MB_CHECKSUM, -(MB_HEADER_MAGIC + MB_ARCH + MB_HEADER_SIZE)
	.set MB_TAG_END, 0x0

	.section .multiboot_header
	.align 8
multiboot_header_start:
	.4byte MB_HEADER_MAGIC
	.4byte MB_ARCH
	.4byte MB_HEADER_SIZE
	.4byte MB_CHECKSUM

	.2byte MB_TAG_END
	.2byte 0
	.4byte 8
multiboot_header_end:

	.section .boot_gdt
	.align 16
boot_gdt:
	.8byte 0			// null descriptor

	.2byte 0xffff		// 32-bit code
	.2byte 0x0
	.byte 0x0
	.byte 0x9a
	.byte 0xcf
	.byte 0x0

	.2byte 0xffff		// 64-bit code
	.2byte 0x0
	.byte 0x0
	.byte 0x9a
	.byte 0xaf
	.byte 0

	.2byte 0xffff		// data segment
	.2byte 0x0
	.byte 0x0
	.byte 0x92
	.byte 0xcf
	.byte 0x0

	.2byte 0x0068		// TSS selector
	.2byte 0x0
	.byte 0x0
	.byte 0x89
	.2byte 0x0
	.8byte 0x0
	.8byte 0x0
boot_gdt_end:

boot_gdt_descr:
	.2byte boot_gdt_end -boot_gdt
	.8byte boot_gdt - KERNEL_OFFSET
boot_gdt_descr_higher_half:
	.2byte boot_gdt_end -boot_gdt
	.8byte boot_gdt

	.section .bss
	.align 16
boot_stack_bottom:
	.skip 0x1000
boot_stack_top:

	.section .rodata
error_no_multiboot_message:	.asciz "This was not booted from a Multiboot2 bootloaded."
error_no_cpuid_message:		.asciz "Cannot detect system features."
error_no_long_mode_message:	.asciz "Cannot start long mode on this system."
debug_message:			.asciz "Hello, World"

	.section .text
	.code32
	.global _start
	.type _start, @function
_start:
	cli
	// Check Multiboot2 magic number
	cmpl	$(MB_BOOTLOADER_MAGIC),%eax
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

	// Try to find the memory map
	// ebx was already pushed
	call	_multiboot_memory_map
	addl	$4,%esp

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

	// We need a function that can print an error to the screen while still
	// in 32-bit mode.
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
