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

#include "gdt.h"
#include "tss.h"

void
tss_init (tss_t *tss)
{
  for (int i = 0; i < sizeof (tss_t); i ++)
    tss->bytes[i] = 0;
  
  *((uint16_t *)(tss->bytes + 0x0A)) = GDT_KERNEL_DATA;
}

void 
tss_set_stack (tss_t *tss, uint32_t stack)
{
  *((uint32_t *)(tss->bytes + 0x04)) = stack;
}
