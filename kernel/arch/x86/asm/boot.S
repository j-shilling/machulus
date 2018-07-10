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

#define ASM_FILE    1
#include <multiboot.h>
#include <paging.h>
#include <config.h>

#ifdef HAVE_ASM_USCORE
#  define EXT_C(sym)    _ ## sym
#else
#  define EXT_C(sym)    sym
#endif

#define MULTIBOOT_HEADER_FLAGS \
  (MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_VIDEO_MODE)

.extern kmain
.extern arch_init

.section .multiboot
  .align 4
  .long    MULTIBOOT_HEADER_MAGIC
  .long    MULTIBOOT_HEADER_FLAGS
  .long    -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

.section .bss, "aw", @nobits

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
      movl $(PHYS_ADDR(stack_top)), %esp
      movl $(PHYS_ADDR(stack_bottom)), %ebp
      
      call paging_init

      lea higher_half, %ecx
      jmp *%ecx

    higher_half:
      /* Set up the stack */
      movl $stack_top, %esp
      movl $stack_bottom, %ebp

      push %eax /* multiboot magic */
      push %ebx /* multiboot */
      call EXT_C (arch_init)
      addl $8, %esp

      call EXT_C (kmain)
    
      /* kmain should never return and this should be unreachable */
      cli

    inf_loop:
      hlt
      jmp inf_loop
.size _start, . - _start
