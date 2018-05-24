SUBDIRS = arch

noinst_PROGRAMS = jkernel
jkernel_SOURCES = kmain.c
jkernel_LDADD = arch/@ARCHDIR@/lib.a
jkernel_LDFLAGS = -T $(srcdir)/arch/@ARCHDIR@/linker.ld
