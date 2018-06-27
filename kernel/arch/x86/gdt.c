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

#include "tss.h"
#include "gdt.h"

/* GDT FLAGS */
#define PAGE_GRANULARITY        (0x00008000)
#define KERNEL                  (DPL_KERNEL << 5)
#define SERVER                  (DPL_SERVER << 5)
#define MODULE                  (DPL_MODULE << 5)
#define USER                    (DPL_USER   << 5)
#define CODE                    (0x0000001A)
#define DATA                    (0x00000012)
#define TSS                     (0x00000089)

static inline uint64_t
gdt_entry (uint32_t base, uint32_t limit, uint32_t flags)
{
  return ((uint64_t)(limit & 0x0000FFFF))
       | ((uint64_t)(limit & 0x000F0000) << (48 - 16))
       | ((uint64_t)(base  & 0x00FFFFFF) << 16)
       | ((uint64_t)(base  & 0xFF000000) << (56 - 32))
       | ((uint64_t)(flags & 0x0000F0FF) << 40)
       | ((uint64_t)(        0x00004080) << 40);
}

char __gdt_descriptor[6];
uint64_t __gdt[10];
tss_t tss;

void
gdt_init (void)
{
  __gdt[0] = 0;
  
  __gdt[GDT_KERNEL_CODE / 8] 
    = gdt_entry (0, 0xFFFFFFFF, PAGE_GRANULARITY | KERNEL | CODE);
  __gdt[GDT_KERNEL_DATA / 8] 
    = gdt_entry (0, 0xFFFFFFFF, PAGE_GRANULARITY | KERNEL | DATA);
  __gdt[GDT_SERVER_CODE / 8] 
    = gdt_entry (0, 0xFFFFFFFF, PAGE_GRANULARITY | SERVER | CODE);
  __gdt[GDT_SERVER_DATA / 8] 
    = gdt_entry (0, 0xFFFFFFFF, PAGE_GRANULARITY | SERVER | DATA);
  __gdt[GDT_MODULE_CODE / 8] 
    = gdt_entry (0, 0xFFFFFFFF, PAGE_GRANULARITY | MODULE | CODE);
  __gdt[GDT_MODULE_DATA / 8] 
    = gdt_entry (0, 0xFFFFFFFF, PAGE_GRANULARITY | MODULE | DATA);
  __gdt[GDT_USER_CODE / 8] 
    = gdt_entry (0, 0xFFFFFFFF, PAGE_GRANULARITY | USER | CODE);
  __gdt[GDT_USER_DATA / 8] 
    = gdt_entry (0, 0xFFFFFFFF, PAGE_GRANULARITY | USER | DATA);
  __gdt[GDT_TSS / 8]
    = gdt_entry ((uint32_t)(&tss), sizeof (tss), TSS);
  
  tss_init (&tss);
  
  uint16_t *__gdt_size = (uint16_t*)__gdt_descriptor;
  uint32_t *__gdt_addr = (uint32_t*)(__gdt_descriptor + 2);
  
  *__gdt_size = sizeof (__gdt) - 1;
  *__gdt_addr = (uint32_t)__gdt;
  
  asm volatile ("lgdt (%0)" :: "r" (__gdt_descriptor));
  asm volatile ("ltr %0" :: "r" ((uint16_t) GDT_TSS));
}