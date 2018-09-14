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

#  Top level Makefile used to configure and build the kernel. This is based
#  heavily on the Linux kernel's build system. We will likely only test this
#  build system with GNU tools (i.e. GNU make and GCC)

PHONY := all
all:

MAKEFLAGS += -rR --include-dir=$(CURDIR) --no-print-directory

#
#  VERSION AND NAMING
#

VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 1

KERNELVERSION := $(VERSION).$(PATCHLEVEL).$(SUBLEVEL)
KERNELIMAGE   := machulus-$(KERNELVERSION)

PHONY += kernelversion kernelimage

kernelversion:
	@echo KERNELVERSION = $(KERNELVERSION)

kernelimage:
	@echo KERNELIMAGE = $(KERNELIMAGE)

export VERSION PATCHLEVEL SUBLEVEL KERNELVERSION

#
# DETECT ARCH AND FIND SUBDIRECTORY
#

SUBARCH := $(shell uname -m | sed -e s/i.86/x86/ -e s/x86_64/x86/)
ARCH    ?= $(SUBARCH)
SRCARCH := $(ARCH)

ifeq ($(ARCH),i386)
    SRCARCH := x86
endif
ifeq ($(ARCH),x86_64)
    SRCARCH := x86
endif

PHONY += subarch arch srcarch

subarch:
	@echo SUBARCH = $(SUBARCH)

arch:
	@echo ARCH = $(ARCH)

srcarch:
	@echo SRCARCH = $(SRCARCH)

export SUBARCH ARCH SRCARCH

# 
# DETECT BUILD TOOLS
#

AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar

export AS LD CC CPP AR

# 
# VARS TO TRACK BUILD MODULES
#

init            :=
core            :=
lib             :=

include arch/$(SRCARCH)/Makefile

#
# GET A LIST OF SUBDIRS TO GO THROUGH
#

vmkernel-dirs   := $(patsubst %/,%,$(filter %/, $(init) $(core) %(lib)))
PHONY += $(vmkernel-dirs)
$(vmkernel-dirs):
	@$(MAKE) -f $(CURDIR)/scripts/Makefile.build obj=$@

# 
# RULES TO BUILD THE KERNEL
#

init             := $(patsubst %/, %/module.a, $(init))
core             := $(patsubst %/, %/module.a, $(core))
lib              := $(patsubst %/, %/module.a, $(lib))

export KBUILD_VMKERNEL_INIT := $(init)
export KBUILD_VMKERNEL_MAIN := $(core)
export KBUILD_VMKERNEL_LIBS := $(lib)
export KBUILD_LDS           := arch/$(SRCARCH)/vmkernel.lds

vmkernel-deps := $(KBUILD_LDS) $(KBUILD_VMKERNEL_INIT) $(KBUILD_VMKERNEL_MAIN) $(KBUILD_VMKERNEL_LIBS)
$(sort $(vmkernel-deps)): $(vmkernel-dirs) ;

all: $(KERNELIMAGE)
$(KERNELIMAGE): $(vmkernel-deps)
	@echo $(KERNELIMAGE) depends on $(vmkernel-deps)

.PHONY: $(PHONY)
