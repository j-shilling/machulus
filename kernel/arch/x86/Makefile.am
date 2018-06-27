noinst_LIBRARIES = lib.a

lib_a_SOURCES = boot.S \
		tty.c \
		arch-init.c \
		paging.c \
		frames.c \
		heap.c \
		acpi.c \
		tss.h \
		tss.c \
		gdt.h \
		gdt.c \
		isr.S \
		isr.h \
		idt.h \
		idt.c \
		interrupt-handler.c \
		exec.h \
		exec.S

lib_a_CFLAGS = -I$(top_srcdir)/include $(ARCH_CFLAGS) $(DEBUG_CFLAGS)
lib_a_CCASFLAGS = $(ARCH_CFLAGS)
