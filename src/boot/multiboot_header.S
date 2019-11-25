	.set MB_HEADER_MAGIC, 0xe85250d6
	.set MB_BOOTLOADER_MAGIC, 0x36d76289
	.set MB_ARCH, 0x0
	.set MB_HEADER_SIZE, (multiboot_header_end - multiboot_header_start)
	.set MB_CHECKSUM, -(MB_HEADER_MAGIC + MB_ARCH + MB_HEADER_SIZE)
	.set MB_TAG_END, 0x0

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
