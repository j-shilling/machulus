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

#ifndef __ARCH_CDEFS_H__
#define __ARCH_CDEFS_H__

#ifdef HAVE_CONFIG_H
#include <config.h>
#else
#define __x86_64__ 0
#endif /* HAVE_CONFIG_H */

/* Kernel loads at 1MB */
#define KERNEL_LMA (0x100000)

/* Kernel's virtual address depends on processor mode */
#if __x86_64__
#define KERNEL_VMA (0xFFFFFFFF80100000)
#else
#define KERNEL_VMA (0xC0100000)
#endif /* __x86_64__ */

/* Kernels virtual address offset */
#define KERNEL_VBASE (KERNEL_VMA - KERNEL_LMA)
#define PHYS_ADDR(x) ((x)-KERNEL_VBASE)

#define PAGE_SIZE (4096)
#define SIZEOF_TSS (0x68)

#endif /* __ARCH_CDEFS_H__ */
