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

.global start
.global mbi
 
.section .multiboot
  .align 4
  .long		MULTIBOOT_HEADER_MAGIC
  .long		MULTIBOOT_HEADER_FLAGS
  .long		-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
 
.section .bss
	.align 16

	mbi: .long  0 /* Address of multiboot_info_t */

	stack_bottom:
		.skip STACK_SIZE
	stack_top:
 
.section .text
	start:
		/* Set up the stack */
		mov $stack_top, %esp

		/* Check that this was loaded by a multiboot bootloader */
		mov $MULTIBOOT_BOOTLOADER_MAGIC, %edx
		cmp %eax, %edx
		je load_multiboot

		/* Multiboot check failed */
		mov $mbi, %edx
		movl $0, (%edx) /* mbi = NULL */
		jmp function_calls

		/* Save the multiboot_info_t structure */
load_multiboot:	mov $mbi, %edx
		mov %ebx, (%edx) /* mbi = ebx */

		/* Call constructors & start program */
function_calls:	
		call EXT_C(kmain)
 
		hang:
			cli
			hlt
			jmp hang
