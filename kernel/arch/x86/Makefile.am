#  Copyright (C) 2018 Jake Shilling <shilling.jake@gmail.com>
# 
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
# 
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

INC_DIRS = -I$(top_srcdir)/include -I$(srcdir)/include -I$(srcdir)

noinst_LIBRARIES = lib.a

BUILT_SOURCES = linker.ld
CLEANFILES = linker.ld
linker.ld: Makefile
	$(CPP) -x c $(INC_DIRS) linker.ld.in | $(GREP) -v '^#' > linker.ld

lib_a_SOURCES = asm/boot.S \
                asm/paging-init.S \
                asm/stage1.S \
		asm/tss.S \
                asm/gdt.S \
		asm/idt.S \
		tty.c \
		arch-init.c \
		paging.c \
		frames.c \
		heap.c \
		tss.h \
		tss.c \
		gdt.h \
		gdt.c \
		isr.S \
		isr.h \
		idt.h \
		idt.c \
		interrupt-handler.c \
		exec.h \
		exec.S

lib_a_CFLAGS = $(INC_DIRS) $(ARCH_CFLAGS) $(DEBUG_CFLAGS)
lib_a_CCASFLAGS = $(INC_DIRS) $(ARCH_CFLAGS)
