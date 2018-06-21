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

#include <stdint.h>
#include <stdio.h>

#include "idt.h"
#include "gdt.h"

#define TASK_GATE       (5)
#define INTERRUPT_GATE  (6)
#define TRAP_GATE       (7)

uint64_t idt[256] = { 0 };

static uint64_t
__idt_entry (uint32_t isr_addr, uint16_t selector, uint8_t dpl, uint8_t type)
{
  uint64_t ret = 0;
  
  uint8_t type_attr = 0x80 | (dpl << 5) | type;  
  if (TASK_GATE != type)
    type_attr |= 0x08;
  
  ret  = (uint64_t)(isr_addr & 0x0000FFFF);
  ret |= (uint64_t)(selector) << 16;
  ret |= (uint64_t)(type_attr) << 40;
  ret |= (uint64_t)(isr_addr & 0xFFFF0000) << 32;
  
  return ret;
}

extern void test_handler (void);

void
idt_init (void)
{
  idt[13] = __idt_entry ((uint32_t)test_handler, 8, DPL_KERNEL, TRAP_GATE);
  
  char idt_desc[6];
  uint16_t *idt_size = (uint16_t *)(idt_desc);
  uint32_t *idt_addr = (uint32_t *)(idt_desc + 2);
  
  *idt_size = 255;
  *idt_addr = (uint32_t)(&idt);
  
  asm volatile ("lidt (%0)" :: "r" (idt_desc));
}