noinst_LIBRARIES = lib.a

lib_a_SOURCES = boot.S \
		tty.c \
		arch-init.c \
		paging.c \
		frames.c \
		heap.c \
		acpi.c \
		gdt.c

lib_a_CFLAGS = -I$(top_srcdir)/include $(ARCH_CFLAGS) $(DEBUG_CFLAGS)
lib_a_CCASFLAGS = $(ARCH_CFLAGS)
