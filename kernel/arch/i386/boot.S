#define ASM_FILE		1
#include "multiboot.h"
#include "paging.h"

#ifdef HAVE_ASM_USCORE
#  define EXT_C(sym)		_ ## sym
#else
#  define EXT_C(sym)		sym
#endif

/* Multboot Header Flags */
#define MULTIBOOT_HEADER_FLAGS	MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO

#define VRTL_ADDR(x,index)	(KERNEL_VBASE | ((index) << 12) | ((x) & 0x00000FFF))

#define KPAGE_DIR_FLAGS		(PAGE_FLAG_PRESENT | PAGE_FLAG_READ_WRITE)
#define KPAGE_DIR_INDEX		(KERNEL_VBASE >> 22)
#define FRAME_STACK_DIR_INDEX	(FRAME_STACK_VBASE >> 22)
#define KHEAP_DIR_INDEX		(KHEAP_VBASE >> 22)

#define CR0_PAGING		(1 << 31)
#define CR0_WRITE_PROTECT	(1 << 16)

#define VIDEO_MEMORY		(0x000B8000)

.extern video_memory		/* virtual address of x86 video memory */
.extern arch_init
.extern kernel_page_directory
.extern kernel_page_table
.extern frames_page_table
.extern kheap_page_table

.section .multiboot
	.align 4
	.long		MULTIBOOT_HEADER_MAGIC
	.long		MULTIBOOT_HEADER_FLAGS
	.long		-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

.section .bss, "aw", @nobits

	.type stack, @object
	.align PAGE_SIZE
	stack:
		stack_bottom:
			.skip PAGE_SIZE
		stack_top:
	.size stack, . - stack

.section .text
	.global _start
	.type _start, @function

	_start:
		/* Start setting up our page table */
		movl $(PHYS_ADDR(kernel_page_table)), %edi	// Store page table at edi
		movl $0, %esi					// esi iterates frames, start at 0
		movl $0, %ecx					// ecx holds the current index in page table

	map_kernel:
		/* Find all page frames with our kernel */
		cmpl $1024, %ecx				// make sure there is still room
		jge  out_of_memory				
		cmpl $(PHYS_ADDR(_kernel_start)), %esi		// if esi holds an address before the kernel
		jl   get_next_frame				// continue
		cmpl $(PHYS_ADDR(_kernel_end)), %esi		// if esi holds an address after the kernel
		jge  end_loop					// end
		

		movl %esi, %edx
		orl  $(KPAGE_FLAGS), %edx
		movl %edx, (%edi)

	get_next_frame:
		addl $PAGE_SIZE, %esi
		addl $4, %edi
		inc  %ecx
		jmp map_kernel
	end_loop:

		/* Add entry for video memory */
		movl $(VIDEO_MEMORY), %edx
		orl  $(KPAGE_FLAGS), %edx
		movl %edx, (%edi)

		/* Calculate virtual address of video memory */
		/* video memory is page aligned so their is no offset */
		movl $(PHYS_ADDR(video_memory)), %esi	// initialize video_memory for tty.c
		movl $(KERNEL_VBASE), (%esi)		// page dir index
		movl %ecx, %edx
		shl  $12, %edx
		orl  %edx, (%esi)			// page table index

		/* Increment index and table pointer*/
		inc  %ecx
		addl $4, %edi

		/* check for multiboot */
		cmp $(MULTIBOOT_BOOTLOADER_MAGIC), %eax
		jne no_mmap

		/* check for mmap */
		movl (%ebx), %eax			// get flags at the start of the struct
		andl $(1 << 6), %eax			// check 6th bit for mmap
		je   no_mmap

		/* get mmap address and length */		
		movl 48(%ebx), %esi			// phys addr
		movl 44(%ebx), %eax			// length
		movl %esi, %edx
		andl $0xFFFFF000, %esi			// page align
		andl $0x00000FFF, %edx			// page offset

		/* add entry */
		orl  $(KPAGE_FLAGS), %esi
		movl %esi, (%edi)

		/* put virtual address of mmap in ebx */
		movl %ecx, %ebx
		shl  $12, %ebx
		orl  $(KERNEL_VBASE), %ebx
		orl  %edx, %ebx

		/* see if mmap spans more than one page */
		cmp $(PAGE_SIZE), %eax
		jge install_page_directory

		inc  %ecx
		addl $4, %edi
		movl 48(%ebx), %esi
		movl %esi, %edx
		addl %eax, %edx
		addl $(PAGE_SIZE), %esi
	map_mmap:
		cmp $(1024), %ecx
		jge out_of_memory
		cmp %esi, %edi
		jge install_page_directory
	
		movl %esi, (%edi)
		orl  $(KPAGE_FLAGS), (%edi)

		addl $(PAGE_SIZE), %esi
		addl $4, %edi
		inc  %ecx
		jmp map_mmap
		
	no_mmap:
		movl $0, %ebx 			// ebx = NULL

	install_page_directory:
		/* Install page directory */
		movl $(PHYS_ADDR(kernel_page_table) + KPAGE_DIR_FLAGS), PHYS_ADDR(kernel_page_directory)
		movl $(PHYS_ADDR(kernel_page_table) + KPAGE_DIR_FLAGS), PHYS_ADDR(kernel_page_directory) + (KPAGE_DIR_INDEX * 4)
		movl $(PHYS_ADDR(frames_page_table) + KPAGE_DIR_FLAGS), PHYS_ADDR(kernel_page_directory) + (FRAME_STACK_DIR_INDEX * 4)
		movl $(PHYS_ADDR(kheap_page_table) + KPAGE_DIR_FLAGS), PHYS_ADDR(kernel_page_directory) + (KHEAP_DIR_INDEX * 4)

		movl $(PHYS_ADDR(kernel_page_directory)), %ecx
		movl %ecx, %cr3

	activate_paging:
		movl %cr0, %ecx
		orl $(CR0_PAGING | CR0_WRITE_PROTECT), %ecx
		movl %ecx, %cr0

		lea higher_half, %ecx
		jmp *%ecx

	higher_half:
		/* Set up the stack */
		mov $stack_top, %esp

		push %eax
		push %ebx
		call EXT_C (arch_init)
		call EXT_C (kmain)
		
		/* kmain should never return and this should be unreachable */
		cli

	out_of_memory:
	inf_loop:
		hlt
		jmp inf_loop
.size _start, . - _start
