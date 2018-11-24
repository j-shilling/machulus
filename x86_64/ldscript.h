/* Copyright (C) 2018 Jake Shilling <shilling.jake@gmail.com>
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. */

/* file: x86_64/ldscript.h
 *
 *   Header file associated with the linker script. This is were we
 * will define any CPP macros relevant to other files as well as declare
 * any additional symbols defined in the linker script */

#pragma once

#if __64bits__
#  define KERNEL_OFFSET     0xFFFFFFFF80000000
#else
#  define KERNEL_OFFSET     0xC0000000
#endif /* __64bits__ */

#ifndef ASM_FILE

extern int kernel_start;
extern int kernel_end;

#endif 
