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

/* file: x86_64/init.c
 *
 *  Defines the init() function, which is uses to perform architecture
 * specific initialization. It should be called as part of the boot.S
 * process after the C environment has been set up and before kmain. */

#include <page-frames.h>

#include <stdio.h>

void
init(void *multiboot_addr)
{
  int res = init_page_frames (multiboot_addr);
  if (res == 0)
    puts ("Success!");
  else
    puts ("Failure!");
  return;
}