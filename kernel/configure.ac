AC_INIT([jkernel], [0.0.1])
AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])

CFLAGS="-O0 -ggdb -fno-pic -ffreestanding -I$(pwd)/include"
LDFLAGS="-ffreestanding -nostdlib"

AC_CANONICAL_HOST

if test "$host_os" != "elf"; then
	AC_MSG_ERROR("$host_os is not supported")
fi

AS_CASE([$host_cpu],
	[i?86], [ARCHDIR=i386],
	[AC_MSG_ERROR("$host_cpu is not supported")])
AC_SUBST([ARCHDIR])

AC_PROG_CC

if test "x$GCC" != "xyes"; then
	AC_MSG_ERROR("gcc is required")
fi

AM_PROG_AS
AC_PROG_RANLIB
AM_PROG_AR

AC_CONFIG_FILES([Makefile
		 arch/Makefile
		 arch/i386/Makefile])
AC_OUTPUT
