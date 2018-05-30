#define ASM_FILE			1

#include "paging.h"

#define MAX_INDEX			1024

.extern kernel_page_table
.extern _kernel_start
.extern _kernel_end

.section .text
	.global _kpage_init
	.type _kpage_init, @function

	/* int kpage_init (void): Initializes the kernel's page
	 * table and returns the next available index.
	 */
	_kpage_init:

		movl $(PHYS_ADDR(kernel_page_table)), %edx
		movl $0, %ecx
		movl $0, %eax

	1:	cmpl $MAX_INDEX, %eax
		jge  3f
		cmpl $(PHYS_ADDR(_kernel_start)), %ecx
		jl   2f
		cmpl $(PHYS_ADDR(_kernel_end)), %ecx
		jge  3f

		push %ecx
		orl  $(KPAGE_FLAGS), %ecx
		movl %ecx, (%edx)
		pop  %ecx

	2:	addl $PAGE_SIZE, %ecx
		addl $4, %edx
		inc  %eax
		jmp  1b

	3:	ret
	.size _kpage_init, . - _kpage_init

	.global _kpage_insert
	.type _kpage_insert, @function

	/* int kpage_insert (int addr, int index): Place addr into the page table
	 * at the given index and return the new virtual address
	 */
	_kpage_insert:
		push %ebp
		movl %esp, %ebp

		/* Get page table location */
		movl 12(%ebp), %ecx
		movl %ecx, %eax /* save index */
		imul $4, %ecx
		movl $(PHYS_ADDR(kernel_page_table)), %edx
		addl %ecx, %edx

		/* Get addr and place in table */
		movl 8(%ebp), %ecx
		push %ecx /* place another copy back on the stack */
		orl  $(KPAGE_FLAGS), %ecx
		movl %ecx, (%edx)

		/* Get virtual address */
		shl $12, %eax
		orl $(KERNEL_VBASE), %eax
		pop %ecx /* get that copy back */
		and $0x00000FFF, %ecx
		orl %ecx, %eax

		pop %ebp
		ret
	.size _kpage_insert, . - _kpage_insert
