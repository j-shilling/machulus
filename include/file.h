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

/* file: include/file.h
 * 
 *   This file declares the FILE structure used by the stdlib functions. It
 * should be included only in files which implement a specific file (i.e. stdout)
 * or which act on that implementation (i.e. fputc)
 */

#ifndef FILE_H
#define FILE_H

#ifdef __cplusplus
extern "C" {
#endif
  
#include <stdio.h>

  struct FILE
  {
    /* Writes a byte specified by the integer argument cast to an unsigned char */
    int (*fputc) (int, FILE *);
    /* Writes a null-terminated string to the file */
    int (*fputs) (const char *, FILE *);
  };


#ifdef __cplusplus
}
#endif

#endif /* FILE_H */

