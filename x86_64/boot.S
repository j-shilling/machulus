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
	.skip 0x1000
boot_stack_top:

.section .rodata
welcome_msg:	.asciz PACKAGE_STRING
 
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
	movl	$(boot_stack_top - KERNEL_OFFSET),%esp
	movl	$(welcome_msg - KERNEL_OFFSET),%eax
	push	%eax
	call	_boot_puts
	add	$4,%esp

	/* Check for the existance of CPUID by seeing if we can flip bit 21 in EFLAGS */
	/* First get the EFLAGS in eax, save a copy in ecx, then try to change bit 21 */
	pushf
	pop	%eax
	mov	%eax,%ecx
	xor	$(1 << 21),%eax
	push	%eax
	popf
	/* Get the result back from EFLAGS in eax, then push the original state back
	   from ecx */
	pushf
	pop	%eax
	push	%ecx
	popf
	/* If ecx == eax then CPUID is not supported */
	xor	%ecx,%eax
	jz	err_no_cpuid

	/* Now use CPUID to check whether long mode is supported. Long mode is checked
	   by using by calling cpuid with eax = 0x80000001, so we first check that functions
	   > 0x80000000 are supported. */
	mov	$0x80000000,%eax
	cpuid
	cmp	$0x80000001,%eax
	jl	err_no_long_mode
	mov	$0x80000001,%eax
	cpuid
	test	$(1 << 29),%edx
	jz	err_no_long_mode
	
	cli
1:	hlt
	jmp 1b

err_no_cpuid:
err_no_long_mode:
 
.size _start, . - _start

/* We will use these functions to write to the screen while still bootstraping. On
   x86 systems, video memory starts at 0xb8000. Each character is 2-bytes, with the first
   byte holding formating information, the second is the ascii code. There are 80 columns
   and 25 rows. (80 * 25) == 2000. sizeof(video memory) == 4000. */

/* Our screen will have a black background (0) and a light-grey foreground (7). The
   formating byte comes out to (7 | (0 << 4)) == 0x07. */

.type _boot_puts, @function
_boot_puts:
	push	%ebp
	mov	%esp,%ebp

	/* First clear the screen, i.e. fill with spaces (ascii = 0x20) */
	/* Use the 4-byte register to write two spaces at a time. Loop over video
	   memory (80 * 25) / 2 times (1000) */
	mov	$0xb8000,%edi
	mov	$0x07200720,%edx
	mov	$1000,%ecx
clear_screen:
	mov	%edx,(%edi)
	add	$4,%edi
	loop	clear_screen

	/* Next get the physical address of the string passed on the stack */
	mov	8(%esp),%esi
	/* Set up for the loop */
	mov	$0xb8000,%edi
	mov	$0x07,%ah
putchar:
	mov	(%esi),%al
	/* Check for null-termination */
	cmp	$0,%al
	je	end
	mov	%ax,(%edi)
	/* Next output-char location */
	add	$2,%edi
	/* Next char in string */
	inc	%esi
	/* Loop */
	jmp	putchar
end:

	pop	%ebp
	ret
.size _boot_puts, . - _boot_puts
