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

 /* A macro to generate page directory entries using 4-MiB pages */
.macro page_entries start, count
  .ifgt \count
    .4byte (\start & 0xFFFFC0000) | 0x83
    page_entries "(\start + 0x00040000)", "(\count - 1)"
  .endif
.endm
 
 /* A temporary stack to use while we set up the kernel */
.section .bss, "aw", @nobits
  .align 4096
  stack_bottom:
    .skip 4096
  stack_top:

 /* Identity map the first GiB to both 0 and 0xC0000000 */
.section .bootstrap.data, "aw", @progbits
  .align 4096
  .type init_page_directory, @object
  init_page_directory:
    page_entries 0, 100  /* First set of mappings */
    page_entries (0x40000 * 100), 100
    page_entries (0x40000 * 200),  56

    .skip (4 * 512), 0   /* Skip the next unused mappings */

    page_entries 0, 100  /* Second set of mappings */
    page_entries (0x40000 * 100), 100
    page_entries (0x40000 * 200),  56
.size init_page_directory, . - init_page_directory

.section .bootstrap.text, "ax"
  .global _start
  .type _start, @function
  _start:
    /* Set page directory address */
    mov $init_page_directory, %edx
    mov %edx, %cr3

    /* Activate PSE for 4-MiB pages */
    mov %cr4, %edx
    or  $0x00000010, %edx
    mov %edx, %cr4

    /* Activate paging */
    mov %cr0, %edx
    or  $0x80000001, %edx
    mov %edx, %cr0

    /* Jump to higher half */
    lea higher_half, %edx
    jmp *%edx

.section .text
  .type higher_half, @function
  higher_half:
    /* Set up the stack */
    mov $stack_top, %esp
    mov $stack_bottom, %ebp

    push %eax /* multiboot magic */
    push %ebx /* multiboot */
    call arch_init
    addl $8, %esp

    call kmain
    
    /* kmain should never return and this should be unreachable */
    cli

  inf_loop:
    hlt
    jmp inf_loop

