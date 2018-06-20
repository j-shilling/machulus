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

#include <stddef.h>

#include "acpi.h"
#include "paging.h"

#include <stdio.h>

extern uint32_t _kernel_start;
static rsdp_descriptor_t *rsdp = NULL;

void
acpi_init (void)
{
  /* Look for the RSDP */
  uintptr_t vstart = (uintptr_t) KERNEL_VBASE;
  uintptr_t kstart = (uintptr_t) (&_kernel_start);
  uintptr_t cur = vstart;

  while (cur < kstart)
    {
      rsdp = (rsdp_descriptor_t *) cur;
      /* Check signature */
      if (
             (rsdp->signature[0] == 'R')
          && (rsdp->signature[1] == 'S')
          && (rsdp->signature[2] == 'D')
          && (rsdp->signature[3] == ' ')
          && (rsdp->signature[4] == 'P')
          && (rsdp->signature[5] == 'T')
          && (rsdp->signature[6] == 'R')
          && (rsdp->signature[7] == ' ')
          )
        {
          /* We have the pointer */
          break;
        }

      /* keep looking */
      rsdp = NULL;
      cur += 16;
    }

  /* Check that rsdp was found */
  if (NULL == rsdp)
    return;

  /* Validate the structure */
  char *rsdp_bytes = (char *) rsdp;
  int rsdp_sum = 0;
  for (int i = 0; i < sizeof (rsdp_descriptor_t); i++)
    rsdp_sum += rsdp_bytes[i];

  /* Lowest byte should be 0 */
  if (0 != ((char *) (&rsdp_sum))[0])
    {
      rsdp = NULL;
      return;
    }

  /* if version 1.0 is used then we are done */
  if (0 == rsdp->revision)
    return;

  /* Later versions include an extended structure */
  rsdp_sum = 0;
  for (int i = sizeof (rsdp_descriptor_t); i < sizeof (rsdp_descriptor_20_t); i++)
    rsdp_sum += rsdp_bytes[i];

  /* Lowest byte should be 0 */
  if (0 != ((char *) (&rsdp_sum))[0])
    {
      rsdp = NULL;
      return;
    }
  
  /* RSDP has been found and validated */
  return;
}