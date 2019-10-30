OUTPUT_FORMAT("elf64-x86-64")

ENTRY(_start)

SECTIONS
{
	. = 0xffffffff80100000;
	_KERNEL_START = .;
	.text : AT(ADDR(.text) - 0xffffffff80000000) {
	      . = ALIGN(8);
	      KEEP(*(.multiboot_header))
	      *(.text)
	}

	.rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xffffffff80000000) {
		*(.rodata)
	}

	.data ALIGN(4k) : AT(ADDR(.data) - 0xffffffff80000000) {
	      *(.boot_gdt)
	      *(.data)
	}

	.bss ALIGN(4K) : AT(ADDR(.bss) - 0xffffffff80000000) {
		*(.bss)
	}
	_KERNEL_END = .;
}
