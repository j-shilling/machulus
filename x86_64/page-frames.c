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
#include <stdio.h>

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

  /* Next we need to know the total number of frames on
     the system. */
  size_t bytes;
  struct multiboot_mmap_entry *entry = mmap->entries;
  while ((char *)entry < ((char *)mmap + mmap->size))
    {
      /* Get last address of this entry */
      bytes += entry->len;
      entry = (struct multiboot_mmap_entry *)
	((char *)entry + mmap->entry_size);
    }

  size_t frames0 = bytes / 0x1000;
  size_t frames1 = frames0 / 2;
  size_t frames2 = frames1 / 2;
  size_t frames3 = frames2 / 2;
  size_t frames4 = frames3 / 2;
  size_t frames5 = frames4 / 2;
  size_t frames6 = frames5 / 2;
  size_t frames7 = frames6 / 2;
  size_t frames8 = frames7 / 2;
  size_t frames9 = frames8 / 2;
  size_t frames10 = frames9 / 2;

  printf ("Number of page frames:\n\n");
  printf ("0: %10zu\n", frames0);
  printf ("1: %10zu\n", frames1);
  printf ("2: %10zu\n", frames2);
  printf ("3: %10zu\n", frames3);
  printf ("4: %10zu\n", frames4);
  printf ("5: %10zu\n", frames5);
  printf ("6: %10zu\n", frames6);
  printf ("7: %10zu\n", frames7);
  printf ("8: %10zu\n", frames8);
  printf ("9: %10zu\n", frames9);
  printf ("10: %9zu\n", frames10);
  
  return 0;
}
