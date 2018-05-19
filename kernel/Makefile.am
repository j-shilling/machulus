SUBDIRS = arch

noinst_PROGRAMS = jkernel
jkernel_SOURCES = kmain.c
jkernel_LDFLAGS = -T $(srcdir)/arch/@ARCHDIR@/linker.ld
jkernel_LDADD  = $(srcdir)/arch/@ARCHDIR@/lib.a
