/* Copyright (C) 2018 Jake Shilling <shilling.jake@gmail.com>
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. */

/* file: x86_64/early-idt.S
 *
 *   This is where we define an IDT to handle interrupts early in the
 * boot up process. We only need to check for the built in CPU exceptions
 * and pass them to an exception handler */

#define ASM_FILE 1

/* The TSS on x86_64 systems handles stack-switching on interrupts. We
   want to use this one only to store a clean stack for our exception
   handler. To make it easier to free later, we want it to use exactly
   one page, with the interrupt stack using everything after the TSS
   object itself. */
.section .boot_tss
.align 0x1000
.global boot_tss
.type boot_tss, @object
boot_tss:
	/* Early PL stacks don't matter */
	.skip	0x24, 0
	/* Start of the Interrupt Stack Table */
	.8byte	interrupt_stack_top
	/* The rest of the table doesn't matter */
	.skip	0x3c, 0
boot_tss_end:
.size boot_tss, boot_tss_end - boot_tss
interrupt_stack_bottom:
	.skip	0xf98, 0
interrupt_stack_top:

/* Here's the IDT itself, which holds information about finding
   interrupt handlers. In long mode, each entry is 16-bytes:

2-bytes:	handler address 0-15
2-bytes:	segment selector (on this system 0x10)
1-byte:		index of the interrupt stack (here 0x01)
1-byte:		present-flag, dpl, and type (here 0x8f)
2-bytes:	handler address 16-31
4-bytes:	handler address 32-63
4-bytes:	reserved

   We will leave the handler address values at 0x0 and fill them
   in at run time. The other values can be hard coded. */

.macro	RESERVE_ENTRY
	.2byte	0x0
	.2byte	0x10
	.byte	0x01
	.byte	0x8f
	.2byte	0x0
	.4byte	0x0
	.4byte	0x0
.endm

.macro	RESERVE_ENTRIES count
	RESERVE_ENTRY
	.ifgt	\count
	RESERVE_ENTRIES "(\count - 1)"
	.endif
.endm

/* We only want to handle internal cpu interrupts 0-32. Vectors
   2, 9, 15, 20-28, 31 are reserved */
.section .boot_idt
.align 0x1000
.global boot_idt
.type boot_idt, @object
boot_idt:
	RESERVE_ENTRIES 2
	.skip	16,0
	RESERVE_ENTRIES 6
	.skip	16,0
	RESERVE_ENTRIES 5
	.skip 	16,0
	RESERVE_ENTRIES 4
	.skip	(16 * 9),0
	RESERVE_ENTRIES 2
boot_idt_end:

boot_idt_descr:
	.word	boot_idt_end - boot_idt
	.quad	boot_idt

/* Here are the handlers themselves. They serve to normalize
	a call to kernel_exception_handler. */
.extern kernel_exception_handler

.macro	TRAP	vector name
\name :
	movq	$\vector,%rdi
	movq	$0,%rsi
	call	kernel_exception_handler
.endm

.macro TRAP_E	vector	name
\name :
	movq	$\vector,%rdi
	popq	%rsi
	call	kernel_exception_handler
.endm

.section .text
	
	TRAP	0	divide_by_zero
	TRAP	1	debug
	TRAP	3	breakpoint
	TRAP	4	overflow
	TRAP	5	bound_range
	TRAP	6	invalid_opcode
	TRAP	7	device_not_available
	TRAP_E	8	double_fault
	
	TRAP_E	10	invalid_tss
	TRAP_E	11	segment_not_present
	TRAP_E	12	stack
	TRAP_E	13	general_protection
	TRAP_E	14	page

	TRAP	16	floating_point
	TRAP_E	17	alignment_check
	TRAP	18	machine_check
	TRAP	19	simd_floating_point
	TRAP_E	29	vmm_communication
	TRAP_E	30	security
	
