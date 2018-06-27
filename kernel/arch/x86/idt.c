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

#include "stdint.h"

#include "gdt.h"
#include "isr.h"

#define AVAILABLE_TO_SERVER             (DPL_SERVER << 5)
#define AVAILABLE_TO_MODULE             (DPL_MODULE << 5)
#define AVAILABLE_TO_USER               (DPL_USER   << 5)
#define INTERRUPT_GATE                  (0x0E)
#define TRAP_GATE                       (0x0F)

uint64_t __idt[256] = { 0, };
char __idt_descriptor[6] = { 0, };

static inline uint64_t
idt_entry (uint32_t offset, uint16_t selector, uint8_t flags)
{
  return           ((uint64_t)(offset & 0xFFFF0000)  << (48 - 16))
                 | ((uint64_t)(offset & 0x0000FFFF))
                 | ((uint64_t)(selector)             << 16)
                 | ((uint64_t)(flags)                << 40)
                 | (0x0000800000000000);
}

void
idt_init (void)
{
  __idt[0]  = idt_entry ((uint32_t) isr_0,  GDT_KERNEL_CODE, TRAP_GATE);
  __idt[1]  = idt_entry ((uint32_t) isr_1,  GDT_KERNEL_CODE, TRAP_GATE);
  __idt[2]  = idt_entry ((uint32_t) isr_2,  GDT_KERNEL_CODE, TRAP_GATE);
  __idt[3]  = idt_entry ((uint32_t) isr_3,  GDT_KERNEL_CODE, TRAP_GATE);
  __idt[4]  = idt_entry ((uint32_t) isr_4,  GDT_KERNEL_CODE, TRAP_GATE);
  __idt[5]  = idt_entry ((uint32_t) isr_5,  GDT_KERNEL_CODE, TRAP_GATE);
  __idt[6]  = idt_entry ((uint32_t) isr_6,  GDT_KERNEL_CODE, TRAP_GATE);
  __idt[7]  = idt_entry ((uint32_t) isr_7,  GDT_KERNEL_CODE, TRAP_GATE);
  __idt[8]  = idt_entry ((uint32_t) isr_8,  GDT_KERNEL_CODE, TRAP_GATE);
  __idt[9]  = idt_entry ((uint32_t) isr_9,  GDT_KERNEL_CODE, TRAP_GATE);
  __idt[10] = idt_entry ((uint32_t) isr_10, GDT_KERNEL_CODE, TRAP_GATE);
  __idt[11] = idt_entry ((uint32_t) isr_11, GDT_KERNEL_CODE, TRAP_GATE);
  __idt[12] = idt_entry ((uint32_t) isr_12, GDT_KERNEL_CODE, TRAP_GATE);
  __idt[13] = idt_entry ((uint32_t) isr_13, GDT_KERNEL_CODE, TRAP_GATE);
  __idt[14] = idt_entry ((uint32_t) isr_14, GDT_KERNEL_CODE, TRAP_GATE);
  __idt[15] = idt_entry ((uint32_t) isr_15, GDT_KERNEL_CODE, TRAP_GATE);
  __idt[16] = idt_entry ((uint32_t) isr_16, GDT_KERNEL_CODE, TRAP_GATE);
  __idt[17] = idt_entry ((uint32_t) isr_17, GDT_KERNEL_CODE, TRAP_GATE);
  __idt[18] = idt_entry ((uint32_t) isr_18, GDT_KERNEL_CODE, TRAP_GATE);
  __idt[19] = idt_entry ((uint32_t) isr_19, GDT_KERNEL_CODE, TRAP_GATE);
  __idt[20] = idt_entry ((uint32_t) isr_20, GDT_KERNEL_CODE, TRAP_GATE);
  __idt[21] = idt_entry ((uint32_t) isr_21, GDT_KERNEL_CODE, TRAP_GATE);
  __idt[22] = idt_entry ((uint32_t) isr_22, GDT_KERNEL_CODE, TRAP_GATE);
  __idt[23] = idt_entry ((uint32_t) isr_23, GDT_KERNEL_CODE, TRAP_GATE);
  __idt[24] = idt_entry ((uint32_t) isr_24, GDT_KERNEL_CODE, TRAP_GATE);
  __idt[25] = idt_entry ((uint32_t) isr_25, GDT_KERNEL_CODE, TRAP_GATE);
  __idt[26] = idt_entry ((uint32_t) isr_26, GDT_KERNEL_CODE, TRAP_GATE);
  __idt[27] = idt_entry ((uint32_t) isr_27, GDT_KERNEL_CODE, TRAP_GATE);
  __idt[28] = idt_entry ((uint32_t) isr_28, GDT_KERNEL_CODE, TRAP_GATE);
  __idt[29] = idt_entry ((uint32_t) isr_29, GDT_KERNEL_CODE, TRAP_GATE);
  __idt[30] = idt_entry ((uint32_t) isr_30, GDT_KERNEL_CODE, TRAP_GATE);
  __idt[31] = idt_entry ((uint32_t) isr_31, GDT_KERNEL_CODE, TRAP_GATE);
  
  *((uint16_t *)__idt_descriptor) = (8 * 256) - 1;
  *((uint32_t *)(__idt_descriptor + 2)) = (uint32_t)(__idt);
  
  asm volatile ("lidt (%0)" :: "r" (__idt_descriptor));
}