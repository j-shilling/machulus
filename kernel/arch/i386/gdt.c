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

#define GDT_ENTRIES                             (2)

/* Segment Type Flags */
#define SEG_TYPE_READ_ONLY_DATA                 (0)
#define SEG_TYPE_READ_WRITE_DATA                (2)
#define SEG_TYPE_READ_ONLY_EXPAND_DOWN_DATA     (4)
#define SEG_TYPE_READ_WRITE_EXPAND_DOWN_DATA    (6)
#define SEG_TYPE_EXECUTE_ONLY_CODE              (8)
#define SEG_TYPE_EXECUTE_READ_CODE              (10)
#define SEG_TYPE_EXECUTE_ONLY_CONFORMING_CODE   (12)
#define SEG_TYPE_EXECUTE_READ_CONFORMING_CODE   (14)

/* Descriptor Privilege Levels */
#define DPL_KERNEL                              (0)
#define DPL_SERVER                              (1)
#define DPL_MODULE                              (2)
#define DPL_USERSPACE                           (3)

/**
 * Describes a memory segment starting at base with size limit
 */
struct _segment_descriptor
{
  uint16_t segment_limit_0_15   : 16;
  uint16_t segment_base_0_15    : 16;
  
  uint8_t  segment_base_16_23   :  8;
  
  /* Type flag (see above) */
  uint8_t  type                 :  4;
  /* If set, the segment is code / data */
  uint8_t  code_or_data_seg     :  1;
  
  /* Descriptor Privilege Level */
  uint8_t  dpl                  :  2;
  /* Set if segment is present */
  uint8_t  present              :  1;
  
  uint8_t  segment_limit_16_19  :  4;
  uint8_t  avl                  :  1;
  
  /* If set, segment is 64-bit code */
  uint8_t  long_mode            :  1;
  
  /* For executable code: if set, code is 32-bit (16-bit otherwise).
   * For stack segment: If set, 32-bit stack pointer is used (16-bit otherwise)
   * For expand down data: If set upper bound is 4GiB (64KiB otherwise) 
   */
  uint8_t  default_size_or_bound:  1;
  
  /* If set, limit is in KiB */
  uint8_t  granularity          :  1;
  
  uint8_t  segment_base_24_31   :  8;
  
} __attribute__ ((packed));
typedef struct _segment_descriptor segment_descriptor_t;

struct _gdt_descriptor
{
  uint16_t size;
  uint32_t offset;
} __attribute__ ((packed));
typedef struct _gdt_descriptor gdt_descriptor_t;

static segment_descriptor_t gdt[GDT_ENTRIES + 1];

static segment_descriptor_t
get_seg_descriptor (unsigned int base, unsigned int limit, int type, int prvl)
{
  segment_descriptor_t seg;
  
  seg.segment_base_0_15  = (uint16_t)( base        & 0xFFFF);
  seg.segment_base_16_23 = (uint8_t) ((base >> 16) & 0xFF);
  seg.segment_base_24_31 = (uint8_t) ((base >> 24) & 0xFF);
  
  seg.segment_limit_0_15 = (uint16_t)( limit       & 0xFFFF);
  seg.segment_limit_16_19= (uint8_t) ((limit >>16) & 0x0F);
  
  seg.type = type;
  seg.dpl  = prvl;
  
  seg.code_or_data_seg = 1;
  seg.present = 1;
  seg.long_mode = 0;
  seg.default_size_or_bound = 1;
  
  return seg;
}

void
gdt_init (void)
{
  gdt[1] = 
    get_seg_descriptor (0, 0xFFFFFFFF, SEG_TYPE_EXECUTE_ONLY_CODE, DPL_KERNEL);
  gdt[2] =
    get_seg_descriptor (0, 0xFFFFFFFF, SEG_TYPE_READ_WRITE_DATA, DPL_KERNEL);
  
  
  gdt_descriptor_t gdtp = {
    .size = GDT_ENTRIES,
    .offset = (uint32_t) gdt
  };
  
  asm volatile (
    "lgdt (%0)" : /* no output */ : "p" (&gdtp)
  );
}

