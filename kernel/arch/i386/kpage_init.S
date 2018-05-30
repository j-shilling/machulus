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
