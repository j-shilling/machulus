if BUILD_SUBLIBS

SUBDIRS = locale \
	  stdlib \
	  string

COMMON_LIBS = locale/lib.a \
	      stdlib/lib.la \
	      string/lib.la

endif

if LIBC

lib_LTLIBRARIES=libc.la
libc_la_SOURCES=
libc_la_LIBADD = $(COMMON_LIBS)

endif

if LIBK

lib_LIBRARIES = libk.a

libk_a_SOURCES=
libk_a_LIBADD = $(COMMON_LIBS)

endif
