TARGET    := machulus
ISO       := $(TARGET).iso

SRCDIR    := src
INCDIR    := inc
RESDIR    := res
BUILDDIR  := obj
TARGETDIR := bin
ISODIR    := iso

XCC = x86_64-elf-gcc

CFLAGS ?= -O0 -ggdb3
CFLAGS += -ffreestanding -mcmodel=kernel -mno-red-zone -fno-pic
ASFLAGS ?= -ggdb3
LDFLAGS := -n -nostdlib -lgcc

BOOTDIR := $(TARGETDIR)/boot

SRC      := $(shell find $(SRCDIR) -name '*.[cS]')
OBJ      := $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%.o, $(basename $(SRC)))
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

$(BUILDDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(XCC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

$(BUILDDIR)/%.o : $(SRCDIR)/%.S
	@mkdir -p $(dir $@)
	$(XCC) -c $(ASFLAGS) $(CPPFLAGS) $< -o $@

% : $(RESDIR)/%.in
	$(XCC) -E $(CPPFLAGS) -P -x c -o $@ $<

clean:
	-rm -rf $(TARGETDIR)
	-rm -rf $(BUILDDIR)
	-rm -f $(ISO)
	-rm -f $(LDSCRIPT)
	-rm -f $(GRUBCFG)

.PHONY: all clean
