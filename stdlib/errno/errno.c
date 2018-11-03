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

/* file: stdlib/errno/errno.c 
 *
 *   Implements necessary methods to make errno thread safe. Declares errno 
 * itself. */

/* Return the address of this thread's errno */
int *
__errno_address(void)
{
  /* Currently the kernel is single threaded, so don't worry about it. */
  static int errno = 0;
  return &errno;
}