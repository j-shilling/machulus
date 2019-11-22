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

BOOTDIR := $(TARGETDIR)/boot

SRC      := $(shell find $(SRCDIR) -name '*.[cS]')
INFILES  := $(shell find $(RESDIR) -name '*.in')
OBJ      := $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%.o, $(basename $(SRC)))
DEPS     := $(patsubst $(BUILDDIR)/%.o, $(DEPDIR)/%.d, $(OBJ))
DEPS     += $(patsubst $(RESDIR)/%.in, $(DEPDIR)/%.d, $(INFILES))

LDSCRIPT := linker.ld
GRUBCFG  := grub.cfg

KERNEL   := $(BOOTDIR)/$(TARGET)

ISOFILES := $(KERNEL) $(BOOTDIR)/grub/$(GRUBCFG)

all: $(ISO)

qemu: $(ISO)
	qemu-system-x86_64 -cpu host -enable-kvm -cdrom $(ISO)

$(ISO) : $(ISOFILES)
	grub-mkrescue -o $@ $(TARGETDIR)

$(KERNEL): $(OBJ) $(LDSCRIPT)
	@mkdir -p $(dir $@)
	$(XCC) $(LDFLAGS) -T $(LDSCRIPT) -o $@ $(OBJ)

$(BOOTDIR)/grub/$(GRUBCFG): $(GRUBCFG)
	install -D $< $@

$(BUILDDIR)/%.o : $(SRCDIR)/%.c $(DEPDIR)/%.d
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(patsubst $(SRCDIR)/%, $(DEPDIR)/%, $<))
	$(XCOMPILE.c) $< -o $@

$(BUILDDIR)/%.o : $(SRCDIR)/%.S $(DEPDIR)/%.d
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(patsubst $(SRCDIR)/%, $(DEPDIR)/%, $<))
	$(XCOMPILE.S) $< -o $@

% : $(RESDIR)/%.in
	$(XCOMPILE.in) -o $@ $<

$(DEPS):

clean:
	-rm -rf $(TARGETDIR)
	-rm -rf $(BUILDDIR)
	-rm -rf $(DEPDIR)
	-rm -f $(ISO)
	-rm -f $(LDSCRIPT)
	-rm -f $(GRUBCFG)

.PHONY: all clean

include $(wildcard $(DEPS))
