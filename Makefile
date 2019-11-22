#####
### OUTPUT FILE NAMES
#####

TARGET    := machulus
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

#####
### TOOLS
#####

XCC = x86_64-elf-gcc
XCPP = $(XCC) -E

#####
### FLAGS AND OPTIONS
#####

CFLAGS   := -ggdb3 -O0 -ffreestanding -mcmodel=kernel -mno-red-zone -fno-pic
ASFLAGS  := -ggdb3
CPPFLAGS := -I$(INCDIR)
LDFLAGS  := -n -nostdlib -lgcc
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
# Get all files generated by CPP
BUILTRES := $(patsubst $(RESDIR)/%.in, $(BUILDDIR)/%, $(INFILES))

# Build .d files for anything passed to CPP
DEPS     := $(patsubst $(BUILDDIR)/%.o, $(DEPDIR)/%.d, $(OBJ))
DEPS     += $(patsubst $(RESDIR)/%.in, $(DEPDIR)/%.d, $(INFILES))

#####
### OUTPUT FILES
#####

LDSCRIPT := $(TARGETDIR)/linker.ld
GRUBCFG  := $(TARGETDIR)/grub.cfg
KERNEL   := $(TARGETDIR)/$(TARGET)

#################################################
### RULES                                     ###
#################################################

.PHONY: all clean

all: $(KERNEL)

qemu: $(ISO)
	qemu-system-x86_64 -cpu host -enable-kvm -cdrom $(ISO)

debug: $(ISO)
	qemu-system-x86_64 -cpu host -enable-kvm -cdrom $(ISO) -monitor stdio -d int -s -S

clean:
	-rm -rf $(TARGETDIR)
	-rm -rf $(BUILDDIR)
	-rm -rf $(DEPDIR)
	-rm -rf $(ISODIR)
	-rm -f  $(ISO)

####
## BUILD BOOTABLE ISO
####

iso: $(ISO)

$(ISO): $(ISODIR)/boot/$(TARGET) $(ISODIR)/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(ISODIR)

$(ISODIR)/boot/$(TARGET): $(KERNEL)
	install -D $< $@

$(ISODIR)/boot/grub/grub.cfg: $(GRUBCFG)
	install -D $< $@

####
## TARGETS
####

$(KERNEL): $(OBJ) $(LDSCRIPT)
	@mkdir -p $(dir $@)
	$(XCC) $(LDFLAGS) -T $(LDSCRIPT) -o $@ $(OBJ)

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
	$(XCOMPILE.in) -o $@ $<

####
## DEPENDENCY FILES
####

$(DEPS):

include $(wildcard $(DEPS))
