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
 
#define ASM_FILE 1
#include <arch/cdefs.h>

.section .gdt, "wda", @progbits
.align 8
  .type initial_gdt, @object
  .global initial_gdt
  initial_gdt:
    /* Null entry */
    .8byte          0x0
    
    /* 32-bit code segment */
    initial_code_32:
      .2byte        0xFFFF
      .2byte        0x0000
      .byte         0x00
      .byte         0b10011010
      .byte         0b11001111
      .byte         0x00
      
    /* data segment */
    initial_data:
      .2byte        0xFFFF
      .2byte        0x0000
      .byte         0x00
      .byte         0b10010010
      .byte         0b11001111
      .byte         0x00
      
    /* Optional 64-bit code segment */
#if __x86_64__
    intial_code_64:
      .2byte        0xFFFF
      .2byte        0x0000
      .byte         0x00
      .byte         0b10011010
      .byte         0b10101111
      .byte         0x00
#endif

    /* base = initial_tss; limit = SIZEOF_TSS */
    initial_tss_entry:
      .2byte        (SIZEOF_TSS & 0xFFFF)
      .2byte        0x0000
      .byte         0x00
      .byte         0b10001001
      .byte         ((SIZEOF_TSS & 0xFF0000) >> 16)
      .byte         0x00
 
  end_initial_gdt:
  
  .type initial_gdtr, @object
  .global initial_gdtr
  initial_gdtr:
    .2byte        (end_initial_gdt - initial_gdt - 1)
#if __x86_64__
    .8byte        PHYS_ADDR(initial_gdt)
#else
    .4byte        PHYS_ADDR(initial_gdt)
#endif

.extern initial_tss

.section .text
  .type start_initial_gdt, @function
  .global start_initial_gdt
  start_initial_gdt:
    /* Add initial TSS entry */
    mov           $PHYS_ADDR(initial_tss_entry), %edx
    mov           $PHYS_ADDR(initial_tss), %eax

    mov           %eax, %ebx
    shl           $16, %ebx
    or            %ebx, (%edx)
    add           $4, %edx
  
    mov           %eax, %ebx
    shr           $16, %ebx
    and           $0xFF, %ebx
    and           $0xFF000000, %eax
    or            %eax, %ebx
    or            %ebx, (%edx)

    /* Load GDT */
    mov           $PHYS_ADDR(initial_gdtr), %eax
    lgdt          (%eax)
    
    /* Switch selector registers */
    mov           $0x10, %ax
    mov           %ax, %ds
    mov           %ax, %es
    mov           %ax, %fs
    mov           %ax, %gs
    
    pop           %ebx
    pushf
    push          $0x08
    push          %ebx
    iret
  .size start_initial_gdt, . - start_initial_gdt
