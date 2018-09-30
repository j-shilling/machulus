# Copyright (C) 2018 Jake Shilling <shilling.jake@gmail.com>
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# file: Makefile.am
#
#   This is the main file used by automake to generate Makefile.in.
# Here is where we set the AM_CFLAGS and related variables, establish
# our _LIBRARIES and _PROGRAMS, and include other Makefrag.am files.

# 
# Define variables which will be used below
#

AM_CPPFLAGS =
AM_CFLAGS =
AM_CCASFLAGS =
AM_LDFLAGS =

CLEANFILES =

#
# Compilation Flags
#

AM_CPPFLAGS += \
    -ffreestanding -imacros config.h

AM_CPPFLAGS += \
    -I$(systype) \
    -I. \
    -I$(top_srcdir)/$(systype) \
    -I$(top_srcdir)/include

AM_CFLAGS += \
    -no-pie -fno-pic

AM_LDFLAGS += -nostdlib

#
# Main stuff
#

noinst_LIBRARIES = libkernel.a
libkernel_a_SOURCES =

machulus_SOURCES =
machulus_LINKFLAGS =

include Makefrag.am

machulus_LINK = $(LD) $(LDFLAGS) $(LINKFLAGS) $(machulus_LINKFLAGS) -o $@
machulus_LDADD = libkernel.a

#
# Installation
#

exec_bootdir = $(exec_prefix)/boot
exec_boot_PROGRAMS = machulus

#
# Misc build targets
#

if HAVE_GRUB_MKRESCUE

.PHONY: iso
iso: machulus.iso

machulus.iso: machulus iso/boot/grub/grub.cfg
	cp machulus iso/boot/machulus
	$(GRUB_MKRESCUE) -o $@ iso

CLEANFILES += iso/boot/machulus machulus.iso

endif
