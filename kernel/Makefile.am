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

ACLOCAL_AMFLAGS = -I m4
SUBDIRS = arch

noinst_PROGRAMS = machulus
machulus_SOURCES = kmain.c \
		  printf.c \
		  panic.c \
		  malloc.c

machulus_LDADD = arch/@ARCHDIR@/lib.a
machulus_LDFLAGS = -T $(srcdir)/arch/@ARCHDIR@/linker.ld $(ARCH_LDFLAGS)
machulus_CFLAGS = -I$(srcdir)/include $(ARCH_CFLAGS) $(DEBUG_CFLAGS)
