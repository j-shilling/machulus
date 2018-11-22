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

#include <stddef.h>
#include <stdint.h>

int
init_page_frames (void *multiboot_addr)
{
  /* Get the memory map */
  struct multiboot_tag *tag =
    (struct multiboot_tag *) ((char *)multiboot_addr + 8);
  struct multiboot_tag_mmap *mmap = NULL;
  
  while (MULTIBOOT_TAG_TYPE_END != tag->type)
    {
      if (MULTIBOOT_TAG_TYPE_MMAP == tag->type)
	{
	  mmap = (struct multiboot_tag_mmap *)tag;
	  break;
	}
      
      tag = (struct multiboot_tag *)
	( (char *)tag + ((tag->size + 7) & ~7) );
    }

  /* Check whether we found the memory map */
  if (NULL == mmap)
    return ENOMMAP;
  
  return 0;
}
