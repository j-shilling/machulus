ENTRY(_start)                /* the name of the entry label */

SECTIONS {
	. = 0xC0100000;          
	_kernel_start = .;

	.text ALIGN (4K) : AT(ADDR(.text)-0xC0000000) 
	{
		*(.multiboot)
		*(.text)             /* all text sections from all files */
	}

	.rodata ALIGN (4K) : AT(ADDR(.rodata)-0xC0000000)
	{
		*(.rodata)
	}

	.data ALIGN (4K) : AT(ADDR(.data)-0xC0000000)  /* align at 4 KB */
	{
		*(.data)             /* all data sections from all files */
	}

	.bss ALIGN (4K): AT(ADDR(.bss)-0xC0000000)
	{
		*(COMMON)            /* all COMMON sections from all files */
		*(.bss)              /* all bss sections from all files */
		*(.bootstrap_stack)
	}
	_kernel_end = .;
}
