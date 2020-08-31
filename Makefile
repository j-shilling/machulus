#####
### VERSION NUMBERS
#####

MAJOR_VERSION := 0
MINOR_VERSION := 1

VERSION       := $(MAJOR_VERSION).$(MINOR_VERSION)

PROJECT_NAME  := machulus

#####
### OUTPUT FILE NAMES
#####

TARGET    := $(PROJECT_NAME)-$(VERSION)
ISO       := $(TARGET).iso

#####
### DIRECTORY NAMES
#####

SRCDIR    := src
INCDIR    := inc
RESDIR    := res
BUILDDIR  := obj
TARGETDIR := bin
ISODIR    := iso
DEPDIR    := .deps
DOCSDIR   := docs
CONTRIBDIR:= $(INCDIR)/contrib

#####
### TOOLS
#####

XCC = clang -target x86_64-pc-none-elf
XCPP = $(XCC) -E
XLD = ld.lld

#####
### FLAGS AND OPTIONS
#####

OPTIMIZATION_LEVEL := -O0
DEBUG_LEVEL        := -g3

WARNINGS           := -Werror -Wall -Wextra -Wshadow -Wdouble-promotion -Wformat=2 \
                      -Wundef -fno-common -Wno-unused-parameter

COMPILE_FLAGS      := -ffreestanding -mcmodel=kernel -mno-red-zone -fno-pic

CONFIG_MACROS      := -D_DOCSDIR=$(DOCSDIR) -D_PROJECT_NAME=$(PROJECT_NAME) \
                      -D_VERSION=$(VERSION) -D_INCDIR=$(INCDIR) -D_CONTRIB=$(CONTRIBDIR)

CFLAGS   := $(OPTIMIZATION_LEVEL) $(DEBUG_LEVEL) $(COMPILE_FLAGS) $(WARNINGS)
ASFLAGS  := $(DEBUG_LEVEL)
CPPFLAGS := -I$(INCDIR) -I$(CONTRIBDIR) $(CONFIG_MACROS)
LDFLAGS  := -nostdlib
DEPFLAGS  = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

#####
### COMPILE COMMANDS
#####

XCOMPILE.c  = $(XCC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) -c
XCOMPILE.S  = $(XCC) $(DEPFLAGS) $(ASFLAGS) $(CPPFLAGS) -c
XCOMPILE.in = $(XCPP) $(DEPFLAGS) $(CPPFLAGS) -P -x c

#####
### INSTALL LOCATIONS
#####

BOOTDIR := $(TARGETDIR)/boot

#####
### INPUT FILES
#####

# Find all C or ASM files
SRC      := $(shell find $(SRCDIR) -name '*.[cS]')
# Grab any files to preprocess
INFILES  := $(shell find $(RESDIR) -name '*.in')

# Collect all object files to be built from C or ASM sources
OBJ      := $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%.o, $(basename $(SRC)))

# Build .d files for anything passed to CPP
DEPS     := $(patsubst $(BUILDDIR)/%.o, $(DEPDIR)/%.d, $(OBJ))
DEPS     += $(patsubst $(RESDIR)/%.in, $(DEPDIR)/%.d, $(INFILES))

#####
### OUTPUT FILES
#####

LDSCRIPT := $(TARGETDIR)/linker.ld
GRUBCFG  := $(TARGETDIR)/grub.cfg
KERNEL   := $(TARGETDIR)/$(TARGET)
DOXYFILE := $(TARGETDIR)/Doxyfile

ISOFILES := /boot/$(TARGET) \
	/boot/grub/grub.cfg

# -cpu host only works with KVM on Linux
UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
	QEMU := qemu-system-x86_64 -cpu host -enable-kvm
else
	QEMU := qemu-system-x86_64
endif

#################################################
### RULES                                     ###
#################################################

.PHONY: all clean info docs

all: $(KERNEL)

qemu: $(ISO)
	$(QEMU) -cdrom $(ISO)

debug: $(ISO)
	$(QEMU) -cdrom $(ISO) -monitor stdio -d int -s -S

docs: $(DOXYFILE)
	doxygen $(DOXYFILE)

clean:
	-rm -rf $(TARGETDIR)
	-rm -rf $(BUILDDIR)
	-rm -rf $(DEPDIR)
	-rm -rf $(ISODIR)
	-rm -f  $(ISO)
	-rm -rf docs

####
## BUILD BOOTABLE ISO
####

iso: $(ISO)

$(ISO): $(addprefix $(ISODIR), $(ISOFILES))
	grub-mkrescue -o $@ $(ISODIR)

$(addsuffix %,$(addprefix $(ISODIR), $(dir $(ISOFILES)))): $(TARGETDIR)/%
	@mkdir -p $(dir $@)
	@cp -v $< $@

####
## TARGETS
####

$(KERNEL): $(OBJ) $(LDSCRIPT)
	@mkdir -p $(dir $@)
	$(XLD) $(LDFLAGS) -T $(LDSCRIPT) -o $@ $(OBJ)

####
## PATTERN RULES
####

$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(DEPDIR)/%.d
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(patsubst $(SRCDIR)/%, $(DEPDIR)/%, $<))
	$(XCOMPILE.c) $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.S $(DEPDIR)/%.d
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(patsubst $(SRCDIR)/%, $(DEPDIR)/%, $<))
	$(XCOMPILE.S) $< -o $@

$(TARGETDIR)/%: $(RESDIR)/%.in
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(patsubst $(RESDIR)/%, $(DEPDIR)/%, $<))
	$(XCOMPILE.in) -o $@ $<

$(TARGETDIR)/%: $(RESDIR)/%
	@mkdir -p $(dir $@)
	@cp -v $< $@

####
## DEPENDENCY FILES
####

$(DEPS):

include $(wildcard $(DEPS))
