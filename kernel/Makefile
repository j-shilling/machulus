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

#  Version variables copied from linux/Makefile. We are not creative enough to
#  give our releases a name though.

VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 1

KERNELVERSION = $(VERSION).$(PATCHLEVEL).$(SUBLEVEL)
export VERSION PATCHLEVEL SUBLEVEL KERNELVERSION

#  Set the default target (GNU make uses the first target which does not start
#  with a '.' as its default: 
#
#  https://www.gnu.org/software/make/manual/html_node/How-Make-Works.html

PHONY := _all
_all:

#  Following Linux's example to disable builtins and to look for include files
#  from this directory

MAKEFLAGS += --no-builtin-rules --no-builtin-variables --include-dir=$(CURDIR)

#  Following Linux's example to ensure that we are using locale C

unexport LC_ALL
LC_COLLATE=C
LC_NUMERIC=C
export LC_COLLATE LC_NUMERIC

#  Beautify output (Copy-pasted from linux/Makefile)
#  ---------------------------------------------------------------------------
#
#  Normally, we echo the whole command before executing it. By making
#  that echo $($(quiet)$(cmd)), we now have the possibility to set
#  $(quiet) to choose other forms of output instead, e.g.
#
#          quiet_cmd_cc_o_c = Compiling $(RELDIR)/$@
#          cmd_cc_o_c       = $(CC) $(c_flags) -c -o $@ $<
#
#  If $(quiet) is empty, the whole command will be printed.
#  If it is set to "quiet_", only the short version will be printed.
#  If it is set to "silent_", nothing will be printed at all, since
#  the variable $(silent_cmd_cc_o_c) doesn't exist.
#
#  A simple variant is to prefix commands with $(Q) - that's useful
#  for commands that shall be hidden in non-verbose mode.
#
#	$(Q)ln $@ :<
#
#  If KBUILD_VERBOSE equals 0 then the above command will be hidden.
#  If KBUILD_VERBOSE equals 1 then the above command is displayed.
#
#  To put more focus on warnings, be less verbose as default
#  Use 'make V=1' to see the full commands

ifeq ("$(origin V)", "command line")
  KBUILD_VERBOSE = $(V)
endif
ifndef KBUILD_VERBOSE
  KBUILD_VERBOSE = 0
endif

ifeq ($(KBUILD_VERBOSE),1)
  quiet =
  Q =
else
  quiet=quiet_
  Q = @
endif

# If the user is running make -s (silent mode), suppress echoing of
# commands

ifneq ($(findstring s,$(filter-out --%,$(MAKEFLAGS))),)
  quiet=silent_
  tools_silent=s
endif

export quiet Q KBUILD_VERBOSE

srctree		:= .
objtree		:= .
src		:= $(srctree)
obj		:= $(objtree)
VPATH		:= $(srctree)

export srctree objtree VPATH

#  Do not display message when we enter a subdirector. This line is down here
#  away from the others in case we want to add in the same KBUILD_OUTPUT support
#  that Linux has. They wanted to add this flag only after entring the output
#  directory.

MAKEFLAGS += --no-print-directory

#  Copied from linux/Makefile. KBuild.include sets up the KBuild system
scripts/KBuild.include: ;
include scripts/KBuild.include

#  From linux/Makefile
#  SUBARCH tells the usermode build what the underlying arch is.  That is set
#  first, and if a usermode build is happening, the "ARCH=um" on the command
#  line overrides the setting of ARCH below.  If a native build is happening,
#  then ARCH is assigned, getting whatever value it gets normally, and
#  SUBARCH is subsequently ignored.

#  Unlike Linux, we currently only think about x86 machines
SUBARCH := $(shell uname -m | sed -e s/i.86/x86/ -e s/x86_64/x86/)

#  From linux/Makefile
# Cross compiling and selecting different set of gcc/bin-utils
# ---------------------------------------------------------------------------
#
# When performing cross compilation for other architectures ARCH shall be set
# to the target architecture. (See arch/* for the possibilities).
# ARCH can be set during invocation of make:
# make ARCH=ia64
# Another way is to have ARCH set in the environment.
# The default ARCH is the host where make is executed.

# CROSS_COMPILE specify the prefix used for all executables used
# during compilation. Only gcc and related bin-utils executables
# are prefixed with $(CROSS_COMPILE).
# CROSS_COMPILE can be set on the command line
# make CROSS_COMPILE=ia64-linux-
# Alternatively CROSS_COMPILE can be set in the environment.
# Default value for CROSS_COMPILE is not to prefix executables
# Note: Some architectures assign CROSS_COMPILE in their arch/*/Makefile
ARCH		?= $(SUBARCH)

# Architecture as present in compile.h
UTS_MACHINE 	:= $(ARCH)
SRCARCH 	:= $(ARCH)

# Additional ARCH settings for x86
ifeq ($(ARCH),i386)
        SRCARCH := x86
endif
ifeq ($(ARCH),x86_64)
        SRCARCH := x86
endif

# Make variables (CC, etc...)
AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump

NOSTDINC_FLAGS   =
CFLAGS_KERNEL	 =
AFLAGS_KERNEL	 =
LDFLAGS_vmkernel =

# Use USERINCLUDE when you must reference the UAPI directories only.
USERINCLUDE    :=
	
# Use KERNELINCLUDE when you must reference the include/ directory.
KERNELINCLUDE  := $(USERINCLUDE)

KBUILD_AFLAGS   := -D__ASSEMBLY__
KBUILD_CFLAGS   := 
KBUILD_CPPFLAGS := -D__KERNEL__
KBUILD_AFLAGS_KERNEL :=
KBUILD_CFLAGS_KERNEL :=
KBUILD_LDFLAGS :=

export ARCH SRCARCH CROSS_COMPILE AS LD CC CPP AR NM STRIP OBJCOPY OBJDUMP MAKE
export UTS_MACHINE KBUILD_CPPFLAGS NOSTDINC_FLAGS KERNELINCLUDE KBUILD_LDFLAGS
export KBUILD_CFLAGS CFLAGS_KERNEL KBUILD_AFLAGS AFLAGS_KERNEL KBUILD_AFLAGS_KERNEL
export KBUILD_CFLAGS_KERNEL LDFLAGS_vmkernel

PHONY += all
_all: all
all: vmkernel

# The arch Makefile can set ARCH_{CPP,A,C}FLAGS to override the default
# values of the respective KBUILD_* variables
ARCH_CPPFLAGS :=
ARCH_AFLAGS :=
ARCH_CFLAGS :=
include arch/$(SRCARCH)/Makefile

# arch Makefile may override CC so keep this after arch Makefile is included
NOSTDINC_FLAGS += -nostdinc -isystem $(shell $(CC) -print-file-name=include)

# Add any arch overrides and user supplied CPPFLAGS, AFLAGS and CFLAGS as the
# last assignments
KBUILD_CPPFLAGS += $(ARCH_CPPFLAGS) $(KCPPFLAGS)
KBUILD_AFLAGS   += $(ARCH_AFLAGS)   $(KAFLAGS)
KBUILD_CFLAGS   += $(ARCH_CFLAGS)   $(KCFLAGS)

# Use --build-id when available.
LDFLAGS_BUILD_ID := $(call ld-option, --build-id)
KBUILD_LDFLAGS_MODULE += $(LDFLAGS_BUILD_ID)
LDFLAGS_vmkernel += $(LDFLAGS_BUILD_ID)

# Default kernel image to build when no specific target is given.
# KBUILD_IMAGE may be overruled on the command line or
# set in the environment
# Also any assignments in arch/$(ARCH)/Makefile take precedence over
# this default value
export KBUILD_IMAGE ?= vmkernel

#
# INSTALL_PATH specifies where to place the updated kernel and system map
# images. Default is /boot, but you can set it to other values
export	INSTALL_PATH ?= /boot

# Add non-arch-specific objects to core-y
core-y		+=

vmkernel-dirs	 := $(patsubst %/,%,$(filter %/, $(init-y) $(core-y) $(libs-y)))
vmkernel-alldirs := $(sort $(kernel-dirs) $(patsubst %/,%,$(filter %/, \
		     $(init-) $(core-) $(libs-))))

init-y		:= $(patsubst %/, %/built-in.a, $(init-y))
core-y		:= $(patsubst %/, %/built-in.a, $(core-y))
libs-y1		:= $(patsubst %/, %/lib.a, $(libs-y))
libs-y2		:= $(patsubst %/, %/built-in.a, $(filter-out %.a, $(libs-y)))

export KBUILD_VMKERNEL_INIT := $(head-y) $(init-y)
export KBUILD_VMKERNEL_MAIN := $(core-y) $(libs-y2) $(drivers-y) $(virt-y)
export KBUILD_VMKERNEL_LIBS := $(libs-y1)
export KBUILD_LDS          := arch/$(SRCARCH)/kernel/vmkernel.lds
export LDFLAGS_vmkernel

export KBUILD_ALLDIRS := $(sort $(filter-out arch/%,$(vmkernel-alldirs)) \
	arch include)

vmkernel-deps := $(KBUILD_LDS) $(KBUILD_VMKERNEL_INIT) $(KBUILD_VMKERNEL_MAIN) \
	$(KBUILD_VMKERNEL_LIBS)

vmkernel: $(vmkernel-deps)

# The actual objects are generated when descending,
# make sure no implicit rule kicks in
$(sort $(vmkernel-deps)): $(vmkernel-dirs) ;

# Handle descending into subdirectories listed in $(vmkernel-dirs)
PHONY += $(vmkernel-dirs)
$(vmkernel-dirs):
	$(Q)$(MAKE) $(build)=$@
	
# Directories & files removed with 'make clean'
CLEAN_DIRS  +=

clean: rm-dirs  := $(CLEAN_DIRS)
clean: rm-files := $(CLEAN_FILES)
clean-dirs      := $(addprefix _clean_, . $(vmkernel-alldirs))

PHONY += $(clean-dirs) clean archclean vmkernelclean
$(clean-dirs):
	$(Q)$(MAKE) $(clean)=$(patsubst _clean_%,%,$@)

vmkernelclean:

clean: archclean vmkernelclean

kernelversion:
	@echo $(KERNELVERSION)

image_name:
	@echo $(KBUILD_IMAGE)

%.s: %.c prepare scripts FORCE
	$(Q)$(MAKE) $(build)=$(build-dir) $(target-dir)$(notdir $@)
%.o: %.c prepare scripts FORCE
	$(Q)$(MAKE) $(build)=$(build-dir) $(target-dir)$(notdir $@)
%.s: %.S prepare scripts FORCE
	$(Q)$(MAKE) $(build)=$(build-dir) $(target-dir)$(notdir $@)
%.o: %.S prepare scripts FORCE
	$(Q)$(MAKE) $(build)=$(build-dir) $(target-dir)$(notdir $@)

quiet_cmd_rmdirs = $(if $(wildcard $(rm-dirs)),CLEAN   $(wildcard $(rm-dirs)))
      cmd_rmdirs = rm -rf $(rm-dirs)

quiet_cmd_rmfiles = $(if $(wildcard $(rm-files)),CLEAN   $(wildcard $(rm-files)))
      cmd_rmfiles = rm -f $(rm-files)

PHONY += FORCE
FORCE:

# Declare the contents of the PHONY variable as phony.  We keep that
# information in a variable so we can use it in if_changed and friends.
.PHONY: $(PHONY)
