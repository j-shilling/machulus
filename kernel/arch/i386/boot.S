#define ASM_FILE		1
#include "multiboot.h"

#ifdef HAVE_ASM_USCORE
#  define EXT_C(sym)		_ ## sym
#else
#  define EXT_C(sym)		sym
#endif

/* Multboot Header Flags */
#define MULTIBOOT_HEADER_FLAGS	MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_VIDEO_MODE

#define KERNEL_VBASE		0xC0000000
#define PAGE_SIZE		0x1000
#define PHYS_ADDR(x)		((x) - KERNEL_VBASE)

.section .multiboot
	.align 4
	.long		MULTIBOOT_HEADER_MAGIC
	.long		MULTIBOOT_HEADER_FLAGS
	.long		-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

.section .bss, "aw", @nobits

	.type boot_page_directory, @object
	.align PAGE_SIZE
	boot_page_directory:
		.skip PAGE_SIZE
	.size boot_page_directory, . - boot_page_directory

	.type boot_page_table, @object
	.align PAGE_SIZE
	boot_page_table:
		.skip PAGE_SIZE
	.size boot_page_table, . - boot_page_table

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
		movl $(PHYS_ADDR(boot_page_table)), %edi	// Store page table at edi
		movl $0, %esi					// esi iterates frames, start at 0
		movl $1023, %ecx

	map_kernel:
		/* Find all page frames with our kernel */
		cmpl $(PHYS_ADDR(_kernel_start)), %esi		// if esi holds an address before the kernel
		jmp get_next_frame				// continue
		cmpl $(PHYS_ADDR(_kernel_end)), %esi		// if esi holds an address after the kernel
		jmp end_loop					// end

		movl %esi, %edx
		orl $0x003, %edx
		movl %edx, (%edi)

	get_next_frame:
		addl $PAGE_SIZE, %esi
		addl $4, %edi
		loop map_kernel

	end_loop:
		movl $(0x000B8000 | 0x003), boot_page_table - KERNEL_VBASE + 1023 * 4
		movl $(PHYS_ADDR(boot_page_table) + 0x003), PHYS_ADDR(boot_page_directory) + 0
		movl $(PHYS_ADDR(boot_page_table) + 0x003), PHYS_ADDR(boot_page_directory) + 768 * 4

		movl $(PHYS_ADDR(boot_page_directory)), %ecx
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
		
		/* kmain should never return and this should be unreachable */
		cli

	inf_loop:
		hlt
		jmp inf_loop
.size _start, . - _start
