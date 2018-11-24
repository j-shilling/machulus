/*
 * Copyright (C) 2018 Jake Shilling
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

/* file: x86_64/page-frames.c
 *
 *   Defines the interface for working with physical page frames. This
 * includes traking free and used frames as well as allocating and 
 * deallocating frames as needed. */

#include <page-frames.h>
#include <multiboot2.h>
#include <ldscript.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>

/* Size of a 0-order page */
#define PAGE_SIZE 0x1000
/* Maximum order supported by our buddy allocator */
#define MAX_ORDER 10

/* Array of the number of frames at each order */
static size_t nframes[MAX_ORDER + 1];
/* Array of bitmaps for each order */
static uint8_t *bitmaps[MAX_ORDER + 1];

/* Physical address where the kernel starts */
static const uint64_t kernel_start_addr = (uint64_t)(&kernel_start) - KERNEL_OFFSET;
/* Physical address where the kernel ends */
static const uintptr_t kernel_end_addr = (uintptr_t)(&kernel_end) - KERNEL_OFFSET;

static inline void
__reserve_space (uint64_t addr, size_t len)
{
  uint64_t frame_addr = addr & (~(PAGE_SIZE - 1));

  for (int i = 0; i <= MAX_ORDER; i++)
    {
      size_t nframes = len / (PAGE_SIZE * (1 << i));
      size_t index = frame_addr / (PAGE_SIZE * (1 << i));

      for (size_t i = 0; i < nframes; i ++)
	{
	  bitmaps[i][index / 8] |= (1 << (index % 8));
	  index ++;
	}
    }
}

int
init_page_frames (void *multiboot_addr)
{
  /* Get the memory map */
  struct multiboot_tag *tag =
    (struct multiboot_tag *) ((char *)multiboot_addr + 8);
  struct multiboot_tag_mmap *mmap = NULL;
  uint32_t first_mod_after_kernel = 0;

  /* Iterate over the multiboot info looking for mmap and
     the start address of the first module after the kernel */
  while (MULTIBOOT_TAG_TYPE_END != tag->type)
    {
      if (MULTIBOOT_TAG_TYPE_MMAP == tag->type)
	{
	  mmap = (struct multiboot_tag_mmap *)tag;
	}
      else if (MULTIBOOT_TAG_TYPE_MODULE == tag->type)
	{
	  struct multiboot_tag_module *module =
	    (struct multiboot_tag_module *)tag;
	  if (module->mod_start > kernel_end_addr
	      && module->mod_start < first_mod_after_kernel)
	    first_mod_after_kernel = module->mod_start;
	}
      
      tag = (struct multiboot_tag *)
	( (char *)tag + ((tag->size + 7) & ~7) );
    }

  /* Check whether we found the memory map */
  if (NULL == mmap)
    return EINVAL;

  /* Next we need to know the total number of frames on
     the system. */
  size_t bytes = 0;
  struct multiboot_mmap_entry *entry = mmap->entries;
  struct multiboot_mmap_entry *kernel_entry = NULL;
  while ((char *)entry < ((char *)mmap + mmap->size))
    {
      /* Get last address of this entry */
      bytes += entry->len;

      /* Check if this is the block containing the kernel */
      if (MULTIBOOT_MEMORY_AVAILABLE == entry->type
	  && kernel_end_addr >= entry->addr
	  && kernel_end_addr < (entry->addr + entry->len))
	kernel_entry = entry;

      /* Go to next entry */
      entry = (struct multiboot_mmap_entry *)
	((char *)entry + mmap->entry_size);
    }

  if (NULL == kernel_entry)
    return EINVAL;
  
  uint64_t last_avail_addr = kernel_entry->addr + kernel_entry->len - 1;
  if (first_mod_after_kernel > 0 && first_mod_after_kernel < last_avail_addr)
    last_avail_addr = first_mod_after_kernel - 1;

  /* Initialize the nframes array */
  nframes[0] = bytes / PAGE_SIZE;
  for (int i = 1; i <= MAX_ORDER; i++)
    nframes[i] = nframes[i - 1] / 2;

  /* Initialize the bitmaps array */
  uint64_t cur_addr = kernel_end_addr + 1;
  size_t cur_size = last_avail_addr - cur_addr + 1;
  for (int i = 0; i <= MAX_ORDER; i++)
    {
      /* Find the needed size of the bitmap */
      size_t size = (nframes[i] / 8) + ((nframes[i] % 8) ? 0 : 1);
      
      if (size > cur_size)
	return ENOMEM;

      bitmaps[i] = (uint8_t *)(cur_addr + KERNEL_OFFSET);
      cur_addr += size;
      cur_size -= size;

      /* Mark all memory as free */
      for (size_t j = 0; j < size; j++)
	bitmaps[i][j] = 0x0;
    }

  /* Mark frames used by kernel */
  __reserve_space (kernel_start_addr, cur_addr - kernel_start_addr);

  /* Mark frames used by modules */
  for (tag = (struct multiboot_tag *) ((char *)multiboot_addr + 8);
       MULTIBOOT_TAG_TYPE_END == tag->type;
       tag = (struct multiboot_tag *)((char *)tag + ((tag->size + 7) & ~7)))
    {
      if (MULTIBOOT_TAG_TYPE_MODULE == tag->type)
	{
	  struct multiboot_tag_module *module =
	    (struct multiboot_tag_module *)tag;
	  __reserve_space (module->mod_start, module->mod_end - module->mod_start + 1);
	}
    }

  /* Mark frames used by reserved blocks */
  for (entry = mmap->entries;
       (char *)entry < ((char *)mmap + mmap->size);
       entry = (struct multiboot_mmap_entry *)((char *)entry + mmap->entry_size))
    {
      if (MULTIBOOT_MEMORY_AVAILABLE != entry->type)
	__reserve_space (entry->addr, entry->len);
    }
  
  return 0;
}
