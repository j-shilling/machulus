	.set KERNEL_OFFSET, 0xffffffff80000000

	.global boot_gdt_descr
	.global boot_gdt_descr_higher_half

	.section .boot_gdt
	.align 16
boot_gdt:
	.8byte 0			// null descriptor

	.2byte 0xffff		// 32-bit code
	.2byte 0x0
	.byte 0x0
	.byte 0x9a
	.byte 0xcf
	.byte 0x0

	.2byte 0xffff		// 64-bit code
	.2byte 0x0
	.byte 0x0
	.byte 0x9a
	.byte 0xaf
	.byte 0

	.2byte 0xffff		// data segment
	.2byte 0x0
	.byte 0x0
	.byte 0x92
	.byte 0xcf
	.byte 0x0

	.2byte 0x0068		// TSS selector
	.2byte 0x0
	.byte 0x0
	.byte 0x89
	.2byte 0x0
	.8byte 0x0
	.8byte 0x0
boot_gdt_end:

boot_gdt_descr:
	.2byte boot_gdt_end -boot_gdt
	.8byte boot_gdt - KERNEL_OFFSET
boot_gdt_descr_higher_half:
	.2byte boot_gdt_end -boot_gdt
	.8byte boot_gdt
