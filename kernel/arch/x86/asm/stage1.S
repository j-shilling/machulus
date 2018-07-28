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
#include <paging.h>

.extern stack_top
.extern stack_bottom
.extern paging_init
.extern higher_half
.extern start_initial_gdt
.extern start_initial_tss

.section .text
  .global _start
  .type _start, @function
    _start:
      movl $(PHYS_ADDR(stack_top)), %esp
      movl $(PHYS_ADDR(stack_bottom)), %ebp
      push %eax
      push %ebx
      
      cli
      call start_initial_gdt
      call start_initial_tss
      call paging_init

      pop %ebx
      pop %eax
      lea higher_half, %ecx
      jmp *%ecx
  .size _start, . - _start
