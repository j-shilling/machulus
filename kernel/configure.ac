AC_INIT([jkernel], [0.0.1])
AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])

CFLAGS="-ffreestanding -I$(pwd)/include"
LDFLAGS="-ffreestanding -nostdlib"

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

AC_CANONICAL_HOST

if test "$host_os" != "elf"; then
	AC_MSG_ERROR("$host_os is not supported")
fi

AS_CASE([$host_cpu],
	[i?86], [ARCHDIR=i386],
	[AC_MSG_ERROR("$host_cpu is not supported")])
AC_SUBST([ARCHDIR])

AC_PROG_CC
AM_PROG_AS
AC_PROG_RANLIB
AM_PROG_AR

AC_CONFIG_FILES([Makefile
		 arch/Makefile
		 arch/i386/Makefile])
AC_OUTPUT
