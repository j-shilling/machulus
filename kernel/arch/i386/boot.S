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
		/* Set up temporary stack */
		mov $(PHYS_ADDR(stack_top)), %esp
		mov $(PHYS_ADDR(stack_bottom)), %ebp

		/* Check for mmap */
		push %eax
		push %ebx
		call _get_mmap
		addl $8, %esp

		/* Save return values */
		movl %ebx, %edi // phys addr of mmap
		movl %eax, %esi // length of mmap

		/* Initialize kernel page table */
		call _kpage_init

		/* Insert mmap into page table */
		movl %eax, %ebx // save return
		push %eax // index
		push %edi // addr
		call _kpage_insert
		addl $8, %esp
		movl %eax, %edi // replace phys addr with vrtl addr

		/* Insert video memory into page table */
		inc  %ebx
		push %ebx // index
		push $(VIDEO_MEMORY) // addr
		call _kpage_insert
		addl $8, %esp

		/* Initialize video_memory with virtual addr */
		movl $(PHYS_ADDR(video_memory)), %ebx
		movl %eax, (%ebx)

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
		mov $stack_bottom, %ebp

		push %esi /* mmap_length */
		push %edi /* mmap_addr */
		call EXT_C (arch_init)
		addl $8, %esp

		call EXT_C (kmain)
		
		/* kmain should never return and this should be unreachable */
		cli

	out_of_memory:
	inf_loop:
		hlt
		jmp inf_loop
.size _start, . - _start
