#define ASM_FILE
#include <multiboot2.h>

	.set MB_HEADER_MAGIC, MULTIBOOT2_HEADER_MAGIC
	.set MB_ARCH, MULTIBOOT_ARCHITECTURE_I386
	.set MB_HEADER_SIZE, (multiboot_header_end - multiboot_header_start)
	.set MB_CHECKSUM, -(MB_HEADER_MAGIC + MB_ARCH + MB_HEADER_SIZE)
	.set MB_TAG_END, MULTIBOOT_HEADER_TAG_END

	.section .multiboot_header
	.align 8
multiboot_header_start:
	.4byte MB_HEADER_MAGIC
	.4byte MB_ARCH
	.4byte MB_HEADER_SIZE
	.4byte MB_CHECKSUM

	.2byte MB_TAG_END
	.2byte 0
	.4byte 8
multiboot_header_end:
