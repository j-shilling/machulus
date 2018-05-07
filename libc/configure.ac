AC_INIT([libc], [0.0.1])
AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])

AC_CONFIG_MACRO_DIRS([m4])

AC_PROG_CC
AM_PROG_AS
AM_PROG_AR

CFLAGS="-ffreestanding -I`pwd`/include"
LDFLAGS="-ffreestanding -nostdlib"

AC_ARG_ENABLE([libk],
	      AS_HELP_STRING([--enable-libk],[libc implementation to run in kernel-space]),
	      [case "${enable_libk}" in
	         yes) libk=true ;;
	         no)  libk=false ;;
	         *)   AC_MSG_ERROR([bad value ${enable_libk} for --enable-libk]) ;;
	       esac],[libk=true])
AM_CONDITIONAL([LIBK], [test x$libk = xtrue])

AC_ARG_ENABLE([libc],
	    AS_HELP_STRING([--enable-libc],[libc implementation to run in user-space]),
	    [case "${enable_libc}" in
	       yes) libc=true ;;
	       no)  libc=false ;;
	       *)   AC_MSG_ERROR([bad value ${enable_libc} for --enable-libc]) ;;
	     esac],[libc=false])
AM_CONDITIONAL([LIBC], [test x$libc = xtrue])

AM_CONDITIONAL([BUILD_SUBLIBS], [test x$libc = xtrue -o x$libk = xtrue])

AC_ARG_ENABLE([debug],
	    AS_HELP_STRING([--enable-debug],[build with GDB debuging support]),
	    [case "${enable_debug}" in
	       yes) debug=true ;;
	       no)  debug=false ;;
	       *)   AC_MSG_ERROR([bad value ${enable_debug} for --enable-debug]) ;;
	     esac],[debug=false])

if text "x$debug = xtrue"; then
	CFLAGS="$CFLAGS -O0 -ggdb"
else
	CFLAGS="$CFLAGS -O2"
fi

LT_INIT

AC_CONFIG_FILES([Makefile
		 src/Makefile
		 src/stdlib/Makefile
		 src/string/Makefile])

AC_OUTPUT
