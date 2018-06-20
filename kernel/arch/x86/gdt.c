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

#define GDT_ENTRIES                             (8)

/* Segment Type Flags */
#define SEG_TYPE_READ_ONLY_DATA                 (0)
#define SEG_TYPE_READ_WRITE_DATA                (2)
#define SEG_TYPE_READ_ONLY_EXPAND_DOWN_DATA     (4)
#define SEG_TYPE_READ_WRITE_EXPAND_DOWN_DATA    (6)
#define SEG_TYPE_EXECUTE_ONLY_CODE              (8)
#define SEG_TYPE_EXECUTE_READ_CODE              (10)
#define SEG_TYPE_EXECUTE_ONLY_CONFORMING_CODE   (12)
#define SEG_TYPE_EXECUTE_READ_CONFORMING_CODE   (14)
#define SEG_TYPE_SYSTEM                         (16)

/* Descriptor Privilege Levels */
#define DPL_KERNEL                              (0)
#define DPL_SERVER                              (1)
#define DPL_MODULE                              (2)
#define DPL_USERSPACE                           (3)

uint64_t gdt[GDT_ENTRIES];

static uint64_t
__gdt_entry (uint32_t base, uint32_t limit, int prvl, int type, int kbyte)
{
  uint64_t ret = 0;
  
  ret  = (uint64_t)(limit & 0x0000FFFF);
  ret |= (uint64_t)( base & 0x00FFFFFF) << 16;
  ret |= (uint64_t)(limit & 0x000F0000) << 32;
  ret |= (uint64_t)( base & 0xFF000000) << 32;
  
  /* Always set the present bit */
  ret |= ((uint64_t)1 << 48);
  
  /* Set privilege level */
  ret |= (uint64_t)(prvl & 0x03) << 45;
  
  /* Set segment type */
  if (SEG_TYPE_SYSTEM != type)
    {
      ret |= ((uint64_t)1 << 44);
      ret |= ((uint64_t)type & 0x0F) << 40;
    }
  
  /* Always set to 32-bit */
  ret |= ((uint64_t)1 << 54);
  
  /* Set grandularity */
  if (kbyte)
    {
      ret |= ((uint64_t)1 << 55);
    }
  
  return ret;
}

void
gdt_init (void)
{
  gdt[0] = 0;
  gdt[1] = __gdt_entry (0, 0xFFFFFFFF, 
    DPL_KERNEL, SEG_TYPE_READ_WRITE_DATA, 1);
  gdt[2] = __gdt_entry (0, 0xFFFFFFFF, 
    DPL_KERNEL, SEG_TYPE_EXECUTE_READ_CODE, 1);
  gdt[3] = __gdt_entry (0, 0xFFFFFFFF, 
    DPL_SERVER, SEG_TYPE_READ_WRITE_DATA, 1);
  gdt[4] = __gdt_entry (0, 0xFFFFFFFF, 
    DPL_SERVER, SEG_TYPE_EXECUTE_READ_CODE, 1);
  gdt[5] = __gdt_entry (0, 0xFFFFFFFF, 
    DPL_MODULE, SEG_TYPE_READ_WRITE_DATA, 1);
  gdt[6] = __gdt_entry (0, 0xFFFFFFFF, 
    DPL_MODULE, SEG_TYPE_EXECUTE_READ_CODE, 1);
  gdt[5] = __gdt_entry (0, 0xFFFFFFFF, 
    DPL_USERSPACE, SEG_TYPE_READ_WRITE_DATA, 1);
  gdt[6] = __gdt_entry (0, 0xFFFFFFFF, 
    DPL_USERSPACE, SEG_TYPE_EXECUTE_READ_CODE, 1);
  gdt[7] = __gdt_entry ((uint32_t)(&tss), (uint32_t)(tss_size),
    DPL_KERNEL, SEG_TYPE_SYSTEM, 0);
  
  char gdt_desc[6];
  uint16_t *gdt_size = (uint16_t *)gdt_desc;
  uint32_t *gdt_addr = (uint32_t *)(gdt_desc + 2);
  
  *gdt_size = GDT_ENTRIES - 1;
  *gdt_addr = (uint32_t)gdt;
  
  asm volatile ("lgdt (%0)" :: "r" (gdt_desc));
}