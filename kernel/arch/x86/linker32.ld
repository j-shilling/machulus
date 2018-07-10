/*
 * Copyright (C) 2018 Jake Shilling <shilling.jake@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)
SECTIONS {
  . = 0xC0100000;
   
	_kernel_start = .;
	_kernel_phys_start = . - 0xC00000000;

	.text ALIGN (4K) : AT(ADDR(.text)-0xC0000000) 
	{
    *(.multiboot)
		*(.text)
	}

	.rodata ALIGN (4K) : AT(ADDR(.rodata)-0xC0000000)
	{
		*(.rodata)
	}

	.data ALIGN (4K) : AT(ADDR(.data)-0xC0000000)
	{
		*(.data)
	}

	.bss ALIGN (4K): AT(ADDR(.bss)-0xC0000000)
	{
    *(COMMON)
		*(.bss)
	}
  
  /DISCARD/ :
  {
    *(*)
  }

	_kernel_end = .;
	_kernel_phys_end = . - 0xC00000000;
}
