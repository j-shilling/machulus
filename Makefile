TARGET    := machulus
ISO       := $(TARGET).iso

SRCDIR    := src
INCDIR    := inc
RESDIR    := res
BUILDDIR  := obj
TARGETDIR := bin
DEPDIR    := .deps

XCC = x86_64-elf-gcc

CFLAGS ?= -O0 -ggdb3
CFLAGS += -ffreestanding -mcmodel=kernel -mno-red-zone -fno-pic
ASFLAGS ?= -ggdb3
LDFLAGS := -n -nostdlib -lgcc
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

BOOTDIR := $(TARGETDIR)/boot

SRC      := $(shell find $(SRCDIR) -name '*.[cS]')
OBJ      := $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%.o, $(basename $(SRC)))
DEPS     := $(patsubst $(BUILDDIR)/%.o, $(DEPDIR)/%.d, $(OBJ))

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

$(TARGETDIR)/boot/grub/grub.cfg: grub.cfg
	install -D $< $@

$(BUILDDIR)/%.o : $(SRCDIR)/%.c $(DEPDIR)/%.d | $(DEPDIR)
	@mkdir -p $(dir $@)
	$(XCC) -c $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $< -o $@

$(BUILDDIR)/%.o : $(SRCDIR)/%.S
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(patsubst $(SRCDIR)/%, $(DEPDIR)/%, $<))
	$(XCC) -c $(DEPFLAGS) $(ASFLAGS) $(CPPFLAGS) $< -o $@

% : $(RESDIR)/%.in
	$(XCC) -E $(CPPFLAGS) -P -x c -o $@ $<

$(DEPS):

$(DEPDIR):
	@mkdir -p $@

clean:
	-rm -rf $(TARGETDIR)
	-rm -rf $(BUILDDIR)
	-rm -rf $(DEPDIR)
	-rm -f $(ISO)
	-rm -f $(LDSCRIPT)
	-rm -f $(GRUBCFG)

.PHONY: all clean

include $(wildcard $(DEPS))
