#define ASM_FILE		1
#include "multiboot.h"

/* Stack Size 4k */
#define STACK_SIZE		0x1000

#ifdef HAVE_ASM_USCORE
#  define EXT_C(sym)		_ ## sym
#else
#  define EXT_C(sym)		sym
#endif

/* Multboot Header Flags */
#define MULTIBOOT_HEADER_FLAGS	MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_VIDEO_MODE

#define KERNEL_VBASE		0xC0000000

.section .multiboot
	.align 4
	.long		MULTIBOOT_HEADER_MAGIC
	.long		MULTIBOOT_HEADER_FLAGS
	.long		-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

.section .bootstrap_stack, "aw", @nobits
	stack_bottom:
		.skip STACK_SIZE
	stack_top:
 
.section .bss, "aw", @nobits
	.align 4096
	boot_page_directory:
		.skip 4096
	boot_page_table:
		.skip 4096

.section .text
	.global _start
	.type _start, @function

	_start:
		movl $0xB8000, %eax
		movl $1824, (%eax)

		/* Start setting up our page table */
		movl $(boot_page_table - KERNEL_VBASE), %edi	// Store page table at edi
		movl $0, %esi					// esi iterates frames, start at 0
		movl $1023, %ecx

	map_kernel:
		/* Find all page frames with our kernel */
		cmpl $(_kernel_start - KERNEL_VBASE), %esi	// if esi holds an address before the kernel
		jmp get_next_frame				// continue
		cmpl $(_kernel_end - KERNEL_VBASE), %esi	// if esi holds an address after the kernel
		jmp end_loop					// end

		movl %esi, %edx
		orl $0x003, %edx
		movl %edx, (%edi)

	get_next_frame:
		addl $4096, %esi
		addl $4, %edi
		loop map_kernel

	end_loop:
		movl $(0x000B8000 | 0x003), boot_page_table - KERNEL_VBASE + 1023 * 4
		movl $(boot_page_table - KERNEL_VBASE + 0x003), boot_page_directory - KERNEL_VBASE + 0
		movl $(boot_page_table - KERNEL_VBASE + 0x003), boot_page_directory - KERNEL_VBASE + 768 * 4

		movl $(boot_page_directory - KERNEL_VBASE), %ecx
		movl %ecx, %cr3

		movl %cr0, %ecx
		orl $0x80010000, %ecx
		movl %ecx, %cr0

		lea higher_half, %ecx
		jmp *%ecx

	higher_half:
		movl $0, boot_page_directory + 0

		movl %cr3, %ecx
		movl %ecx, %cr3

		/* Set up the stack */
		mov $stack_top, %esp

		call EXT_C (kmain)
		
		cli
	inf_loop:
		hlt
		jmp inf_loop
