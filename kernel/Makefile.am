SUBDIRS = arch

noinst_PROGRAMS = jkernel
jkernel_SOURCES = kmain.c \
		  arch/@ARCHDIR@/boot.S \
		  arch/@ARCHDIR@/tty.c

jkernel_LDFLAGS = -T $(srcdir)/arch/@ARCHDIR@/linker.ld
