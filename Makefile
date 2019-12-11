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
TESTDIR   := test
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

TCC = clang
TCPP = $(TCC) -E
TLD = $(TCC)

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

kernel_CFLAGS   := $(OPTIMIZATION_LEVEL) $(DEBUG_LEVEL) $(COMPILE_FLAGS) $(WARNINGS)
kernel_ASFLAGS  := $(DEBUG_LEVEL)
kernel_CPPFLAGS := -I$(INCDIR) -I$(CONTRIBDIR) $(CONFIG_MACROS)
kernel_LDFLAGS  := -nostdlib
kernel_DEPFLAGS  = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

#####
### COMPILE COMMANDS
#####

XCOMPILE.c  = $(XCC) $(kernel_DEPFLAGS) $(kernel_CFLAGS) $(kernel_CPPFLAGS) -c
XCOMPILE.S  = $(XCC) $(kernel_DEPFLAGS) $(kernel_ASFLAGS) $(kernel_CPPFLAGS) -c
XCOMPILE.in = $(XCPP) $(kernel_DEPFLAGS) $(kernel_CPPFLAGS) -P -x c

TCOMPILE.c  = $(TCC) $(DEPFLAGS) -c
TLINK       = $(TLD)

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
# Find all test files
TEST_SRC := $(shell find $(TESTDIR) -name '*.test.c')

# Collect all object files to be built from C or ASM sources
OBJ      := $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%.o, $(basename $(SRC)))
TEST_OBJ := $(patsubst $(TESTDIR)/%, $(BUILDDIR)/%.o, $(basename $(TEST_SRC)))

# Build .d files for anything passed to CPP
DEPS     := $(patsubst $(BUILDDIR)/%.o, $(DEPDIR)/%.d, $(OBJ))
DEPS     += $(patsubst $(BUILDDIR)/%.o, $(DEPDIR)/%.d, $(TEST_OBJ))
DEPS     += $(patsubst $(RESDIR)/%.in, $(DEPDIR)/%.d, $(INFILES))

#####
### OUTPUT FILES
#####

LDSCRIPT := $(TARGETDIR)/linker.ld
GRUBCFG  := $(TARGETDIR)/grub.cfg
KERNEL   := $(TARGETDIR)/$(TARGET)
DOXYFILE := $(TARGETDIR)/Doxyfile

TESTS    := $(patsubst $(TESTDIR)/%.test.c, %, $(TEST_SRC))

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

.PHONY: all clean info docs info check
.SECONDARY: $(OBJ) $(TEST_OBJ) $(TESTS)

all: $(KERNEL)

qemu: $(ISO)
	$(QEMU) -cdrom $(ISO)

debug: $(ISO)
	$(QEMU) -cdrom $(ISO) -monitor stdio -d int -s -S

docs: $(DOXYFILE)
	doxygen $(DOXYFILE)

check: $(TESTS)

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


define TEST_template
$(1)_PROG = $$(TARGETDIR)/$(1).test
$(1)_SRC  = $$(TESTDIR)/$(1).test.c
$(1)_OBJ  = $$(BUILDDIR)/$(1).test.o
$(1)_DEP  = $$(DEPDIR)/$(1).test.d

$(1)_DEPFLAGS = -MT $$@ -MMD -MP -MF $$($(1)_DEP)

$(1): $$($(1)_PROG)
	@echo "  [TEST] $$@"
	@./$$($(1)_PROG) &> test.log
	-@rm test.log

$$($(1)_PROG): $$($(1)_OBJ)
	@mkdir -p $$(dir $$@)
	@echo "  [LINK] $$@"
	@$$(TCC) -o $$@ $$^

$$($(1)_OBJ): $$($(1)_SRC) $$($(1)_DEP)
	@mkdir -p $$(dir $$@)
	@mkdir -p $$(dir $$($(1)_DEP))
	@echo "  [CC] $$@"
	@$$(TCC) $$($(1)_DEPFLAGS) $$(kernel_CPPFLAGS) -c -o $$@ $$<

.PHONY: $(1)

endef

$(foreach test,$(TESTS),$(eval $(call TEST_template,$(test))))

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

# $(TARGETDIR)/%: $(RESDIR)/%
# 	@mkdir -p $(dir $@)
# 	@cp -v $< $@

# $(BUILDDIR)/%.test.o: $(TESTDIR)/%.test.c $(DEPDIR)/%.test.d
# 	@mkdir -p $(dir $@)
# 	@mkdir -p $(dir $(patsubst $(TESTDIR)/%, $(DEPDIR)/%, $<))
# 	$(TCOMPILE.c) -o $@ $<

####
## DEPENDENCY FILES
####

$(DEPS):

include $(wildcard $(DEPS))
