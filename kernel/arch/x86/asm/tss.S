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

#if __x86_64__
#  define TSS_SEL	(8 * 4)
#else
#  define TSS_SEL	(8 * 3)
#endif

.section .tss, "wda", @progbits
.align PAGE_SIZE
  .type initial_tss, @object
  .global initial_tss
  initial_tss:
    .skip SIZEOF_TSS, 0
  end_initialtss:

  initial_tss_stack:
    .skip (PAGE_SIZE - SIZEOF_TSS), 0
  end_initial_tss_stack:

.section .text
  .type start_initial_tss, @function
  .global start_initial_tss
  start_initial_tss:
    mov	    $PHYS_ADDR(initial_tss), %eax
    mov	    $PHYS_ADDR(initial_tss_stack), %ebx
    mov	    %ebx, 4(%eax)
    mov	    $TSS_SEL, %ax
    ltr	    %ax
    ret
  .size start_initial_tss, . - start_initial_tss
