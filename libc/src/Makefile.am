if BUILD_SUBLIBS

SUBDIRS = string

endif

if LIBC

lib_LTLIBRARIES=libc.la
libc_la_SOURCES=
libc_la_LIBADD = string/lib.la

endif

if LIBK

lib_LIBRARIES = libk.a

libk_a_SOURCES=
libk_a_LIBADD = string/lib.la

endif
