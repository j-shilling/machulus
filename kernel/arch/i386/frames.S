#define ASM_FILE

#include "paging.h"
#include "multiboot.h"

.section .bss, "aw", @nobits

	.type frame_stack_page_table, @object
	.global frame_stack_page_table
	.align PAGE_SIZE
	frame_stack_page_table:
		.skip PAGE_SIZE
	.size frame_stack_page_table, . - frame_stack_page_table

.extern _kernel_end
.section .text
	.global _frames_init
	.type _frames_init, @function
	_frames_init:
	
	/* void frames_init (uint32_t mmap_addr, uint32_t mmap_length); */

	/* Initialize the frame stack in assembly so that we can execute,
	   this code before entering the higher half kernel environment. */
	
	pop  %eax		// mmap_length
	pop  %ebx		// mmap_addr
	addl %ebx, %eax		// end addr
	movl %ebx, %esi

	/* Get frame after kernel end */
	movl $PHYS_ADDR(_kernel_end), %edi
	andl $0x00000FFF, %edi
	addl $0x00001000, %edi

	/* Get the mem block with the end of the kernel */
1:	cmpl $1, 20(%esi)	// .type == 1 (Available RAM)
	je   2f
	/* Go to next block */
	movl %esi, %edx
	addl (%esi), %edx
	addl $4, %edx		// sizeof (.size)
	movl %edx, %esi
	jmp  1b

	/* Check if this block contains the kernel end */
2:	movl 4(%esi), %edx
	addl 12(%esi), %edx	// end of block
	cmpl %edi, %edx
	jg   3f

	/* go to next block */
	movl %esi, %edx
	addl (%esi), %edx
	addl $4, %edx
	movl %edx, %esi
	jmp  1b

	/* check that this block can store a frame */
3:	subl $PAGE_SIZE, %edx
	cmpl %edi, %edx
	jge  5f

	/* we need the next block to get a frame from */
	movl %esi, %edx
	addl (%esi), %edx
	addl $4, %edx
	movl %edx, %esi
	cmpl %esi, %eax
	jl   end_of_mmap

	movl 4(%esi), %edi
	movl %edi, %edx
	andl 0x00000FFF, %edi
	cmpl %edi, %edx
	je   4f
	addl $0x00001000, %edi
4:	movl 4(%esi), %edx
	addl 12(%esi), %edx
	jmp  3b

	/* Recap eax = end of mmap; ebx = start of mmap;
	   edi = first available frame after kernel;
	   edx = end of memory block;
	   esi = mmap */

	/* insert first free frame into the frame_stack_page_table */
5:	movl %edi, %ecx
	orl  $KPAGE_FLAGS, %ecx
	movl $frame_stack_page_table, %edi
	movl %ecx, (%edi)

	ret
	.size _frames_init, . - _frames_init

end_of_mmap:
	ret
