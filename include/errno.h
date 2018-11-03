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

/* file: include/errno.h
 * 
 *   This is a replacement for the errno.h of libc. */

#ifndef ERRNO_H
#define ERRNO_H

#ifdef __cplusplus
extern "C" {
#endif
  
#define errno (*__errno_address())

/* Return the address of this thread's errno */
int *__errno_address(void);
  
/* “Invalid argument.” This is used to indicate various kinds of problems with 
 * passing the wrong argument to a kernel function. */
#define EINVAL          (1)


#ifdef __cplusplus
}
#endif

#endif /* ERRNO_H */

