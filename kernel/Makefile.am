ACLOCAL_AMFLAGS = -I m4
SUBDIRS = arch

noinst_PROGRAMS = jkernel
jkernel_SOURCES = kmain.c \
		  printf.c \
		  panic.c \
		  malloc.c

jkernel_LDADD = arch/@ARCHDIR@/lib.a
jkernel_LDFLAGS = -T $(srcdir)/arch/@ARCHDIR@/linker.ld $(ARCH_LDFLAGS)
jkernel_CFLAGS = -I$(srcdir)/include $(ARCH_CFLAGS) $(DEBUG_CFLAGS)
