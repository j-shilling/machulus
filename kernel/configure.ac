AC_INIT([jkernel], [0.0.1])
AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])

AC_CANONICAL_BUILD
if test "x$host_alias" = "x"; then
	if test "x$build_cpu" = "xx86_64"; then
		build_cpu="i686"
	fi
	host_alias="$build_cpu-$build_vendor-elf"
fi
AC_CANONICAL_HOST

if test "$host_os" != "elf"; then
	AC_MSG_ERROR("$host_os is not supported")
fi

AS_CASE([$host_cpu],
	[i?86],		[ARCHDIR=x86],
	[AC_MSG_ERROR("$host_cpu is not supported")])
AC_SUBST([ARCHDIR])

ARCH_CFLAGS="-ffreestanding -fno-pie"
ARCH_LDFLAGS="-nostdlib -no-pie"

AC_SUBST([ARCH_CFLAGS])
AC_SUBST([ARCH_LDFLAGS])

CFLAGS_ORIG="$CLFAGS"
CFLAGS="$ARCH_CFLAGS"
LDFLAGS_ORIG="$LDFLAGS"
LDFLAGS="$ARCH_LDFLAGS"
cross_compiling=yes

AC_PROG_CC([$host-gcc])
AM_PROG_AS
AC_PROG_RANLIB
AM_PROG_AR

AC_CHECK_LIB([gcc], [__umoddi3])

CFLAGS="$CFLAGS_ORIG"
LDFLAGS="$LDFLAGS_ORIG"

AC_ARG_ENABLE([debug], AS_HELP_STRING([--enable-debug], [build in debug mode]), [], [])
AS_IF([test "$enable_debug" = "yes"], [
  AC_DEFINE([DEBUG], [1], [debug mode])
  DEBUG_CFLAGS="-ggdb -O0 -Werror"
], [
  DEBUG_CFLAGS=""
])
AC_SUBST([DEBUG_CFLAGS])

AC_CONFIG_FILES([Makefile
		 arch/Makefile
		 arch/x86/Makefile])
AC_OUTPUT
