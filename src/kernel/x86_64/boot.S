/* Copyright (C) 2018 Jake Shilling <shilling.jake@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. */

/* file: x86_64/boot.S
 *
 *   This is where we define the entry point for our kernel. Execution
 * starts with the _start function in protected mode. From there we set
 * up paging and activate long mode */

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

/* Multiboot does not necessarily leave us with a valide GDT. This is a
   temporary one before we can figure out how many TSS entries we will
   need */
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
	/* TSS selector: limit = sizeof(tss) = 0x68 */
	.word 0x0068
	.word 0x0
	.byte 0x0
	.byte 0x89
	.word 0x0
	.quad 0x0
	.quad 0x0
boot_gdt_end:

boot_gdt_descr:
	.word boot_gdt_end - boot_gdt
	.long boot_gdt - KERNEL_OFFSET
boot_gdt_descr_higher_half:
	.word boot_gdt_end - boot_gdt
	.quad boot_gdt

/* We will start by mapping a single 1GiB page. This should be more
   than enough for everything we need before we can use the Multiboot
   information to map out available memory. */
.section .boot_paging
.align 0x1000
.type boot_pdpt, @object
boot_pdpt:
	.quad 0x83
        .skip (509 * 8), 0
	.quad 0x83
        .skip (1 * 8), 0

.align 0x1000
.type boot_pml4, @object
boot_pml4:
	.quad ((boot_pdpt - KERNEL_OFFSET) + 0x03)
	.skip (510 * 8), 0
	.quad ((boot_pdpt - KERNEL_OFFSET) + 0x03)

.section .bss
.align 16
boot_stack_bottom:
	.skip 0x1000
boot_stack_top:

.section .rodata
err_no_multiboot_msg:	.asciz "This was not booted from a Multiboot2 bootloader."
err_no_cpuid_msg:	.asciz "Cannot detect system features."
err_no_long_mode_msg:	.asciz "Cannot start long mode on this system."
err_no_huge_pages_msg:	.asciz "Cannot enable 1GiB pages on this system."
 
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
	cli
	/* Check Multiboot2 magic number */
	cmpl	$(MULTIBOOT2_BOOTLOADER_MAGIC),%eax
	jne	err_no_multiboot

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

	/* Check for the existance of CPUID by seeing if we can flip bit 21 in EFLAGS */
	/* First get the EFLAGS in eax, save a copy in ecx, then try to change bit 21 */
	pushf
	pop	%eax
	movl	%eax,%ecx
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
	movl	$0x80000000,%eax
	cpuid
	cmpl	$0x80000001,%eax
	jl	err_no_long_mode
	movl	$0x80000001,%eax
	cpuid
	test	$(1 << 29),%edx
	jz	err_no_long_mode

	/* Check whether 1GiB pages are supported. This information already stored in
	   bit 26 of edx from the last cpuid call */
	test	$(1 << 26),%edx
	jz	err_no_huge_pages

	/* Set the address of boot_pml4 in cr3 */
	movl	$(boot_pml4 - KERNEL_OFFSET),%edi
	movl	%edi,%cr3

	/* Enable PAE */
	movl	%cr4,%eax
	or	$(1 << 5),%eax
	movl	%eax,%cr4

	/* Switch to compatibility mode */
	movl	$0xc0000080,%ecx
	rdmsr
	or	$(1 << 8),%eax
	wrmsr

	/* Enable paging */
	movl	%cr0,%eax
	or	$(1 << 31),%eax
	movl	%eax,%cr0

	ljmp	$16,$(_64bit_entry - KERNEL_OFFSET)

halt32:
	cli
1:	hlt
	jmp 1b

err_no_multiboot:
	mov	$(err_no_multiboot_msg - KERNEL_OFFSET),%eax
	push	%eax
	call	_boot_puts
	add	$4,%esp
	jmp	halt32

err_no_cpuid:
	mov	$(err_no_cpuid_msg - KERNEL_OFFSET),%eax
	push	%eax
	call	_boot_puts
	add	$4,%esp
	jmp	halt32

err_no_long_mode:
	mov	$(err_no_long_mode_msg - KERNEL_OFFSET),%eax
	push	%eax
	call	_boot_puts
	add	$4,%esp
	jmp	halt32

err_no_huge_pages:
	mov	$(err_no_huge_pages_msg - KERNEL_OFFSET),%eax
	push	%eax
	call	_boot_puts
	add	$4,%esp
	jmp	halt32
 
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
	movl	%esp,%ebp

	/* First clear the screen, i.e. fill with spaces (ascii = 0x20) */
	/* Use the 4-byte register to write two spaces at a time. Loop over video
	   memory (80 * 25) / 2 times (1000) */
	movl	$0xb8000,%edi
	movl	$0x07200720,%edx
	movl	$1000,%ecx
clear_screen:
	movl	%edx,(%edi)
	add	$4,%edi
	loop	clear_screen

	/* Next get the physical address of the string passed on the stack */
	movl	8(%esp),%esi
	/* Set up for the loop */
	movl	$0xb8000,%edi
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

.type _64bit_entry, @function
.code64
_64bit_entry:
	movabs	$higher_half,%rax
	jmp	*%rax
higher_half:
	/* Adjust the stack to point to kernel memory */
	movq	%rsp,%rax
	addq	$KERNEL_OFFSET,%rax
	movq	%rax,%rsp
	movq	$boot_stack_bottom,%rbp

	/* Remove the lower memory mapings from pml4 */
	movq	$boot_pml4,%rdi
	movq	$0x0,(%rdi)
	movq	$boot_pdpt,%rdi
	movq	$0x0,(%rdi)
	invlpg	0

	/* Reload GDT at its higher half address */
	lgdt	boot_gdt_descr_higher_half

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

	/* Enter C code */
	callq	kernel_main
halt64:
	cli
1:	hlt
	jmp 1b


.size _64bit_entry, . - _64bit_entry
