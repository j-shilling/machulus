#define ASM_FILE		1
#include "multiboot.h"
#include "paging.h"

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
#define VRTL_ADDR(x,index)	(KERNEL_VBASE | ((index) << 12) | ((x) & 0x00000FFF))

#define KPAGE_FLAGS		(PAGE_FLAG_PRESENT | PAGE_FLAG_READ_WRITE)
#define KPAGE_DIR_FLAGS		(PAGE_FLAG_PRESENT | PAGE_FLAG_READ_WRITE)
#define KPAGE_DIR_INDEX		(KERNEL_VBASE >> 22)

#define CR0_PAGING		(1 << 31)
#define CR0_WRITE_PROTECT	(1 << 16)

#define VIDEO_MEMORY		(0x000B8000)

.extern video_memory		/* virtual address of x86 video memory */
.extern arch_init

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
		movl $1022, %ecx				/* VIDEO_MEMORY will be at index 1023 */

	map_kernel:
		/* Find all page frames with our kernel */
		cmpl $(PHYS_ADDR(_kernel_start)), %esi		// if esi holds an address before the kernel
		jl get_next_frame				// continue
		cmpl $(PHYS_ADDR(_kernel_end)), %esi		// if esi holds an address after the kernel
		jge end_loop					// end

		movl %esi, %edx
		orl $(KPAGE_FLAGS), %edx
		movl %edx, (%edi)

	get_next_frame:
		addl $PAGE_SIZE, %esi
		addl $4, %edi
		loop map_kernel

	end_loop:
		/* Add entry for the multiboot address */
		cmp $(MULTIBOOT_BOOTLOADER_MAGIC), %eax
		jne no_multiboot
		
		/* put address in %ebx into the page table */
		movl $0xFFFFF000, %ecx
		and  %ebx, %ecx
		or   $(KPAGE_FLAGS), %ecx
		movl %ecx, PHYS_ADDR(boot_page_table) + 1022 * 4

		/* replace %ebx with virtual address */
		andl $0x00000FFF, %ebx		// isolate just the offset
		movl $1022, %eax
		shl  $12, %eax			// (1022 << 12)
		orl  %eax, %ebx			// apply page number
		orl  $KERNEL_VBASE, %ebx	// apply page dir number
		jmp  map_video_memory

	no_multiboot:
		movl $0, %ebx 			// ebx = NULL

	map_video_memory:
		/* Add final entry for video memory */
		movl $(VIDEO_MEMORY | KPAGE_FLAGS), PHYS_ADDR(boot_page_table) + 1023 * 4
		movl $PHYS_ADDR(video_memory), %ecx		// initialize variable used in tty.c
		movl $(VRTL_ADDR(VIDEO_MEMORY, 1023)), (%ecx)

		/* Install page directory */
		movl $(PHYS_ADDR(boot_page_table) + KPAGE_DIR_FLAGS), PHYS_ADDR(boot_page_directory)
		movl $(PHYS_ADDR(boot_page_table) + KPAGE_DIR_FLAGS), PHYS_ADDR(boot_page_directory) + (KPAGE_DIR_INDEX * 4)

		movl $(PHYS_ADDR(boot_page_directory)), %ecx
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

		push %ebx
		call EXT_C (arch_init)
		call EXT_C (kmain)
		
		/* kmain should never return and this should be unreachable */
		cli

	inf_loop:
		hlt
		jmp inf_loop
.size _start, . - _start
