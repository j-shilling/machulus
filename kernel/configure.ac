#  Copyright (C) 2018 Jake Shilling <shilling.jake@gmail.com>
# 
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
# 
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

AC_INIT([machulus], [0.0.1])
AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])
AC_CONFIG_HEADERS([./include/config.h])

# We are always cross compiling. If no host has been specified then
# we need to set it automatically based on the current build os.

AC_CANONICAL_BUILD
if test -z "$host_alias"; then
	if test "$build_cpu" = "x86_64"; then
		build_cpu="i686"
	fi
	host_alias="$build_cpu-$build_vendor-elf"
fi
AC_CANONICAL_HOST

# We currently only support building elf

if test "$host_os" != "elf"; then
	AC_MSG_ERROR("$host_os is not supported")
fi

# Check the cpu type to find the arch directory

AS_CASE([$host_cpu],
	[i?86],		[ARCHDIR=x86],
	[AC_MSG_ERROR("$host_cpu is not supported")])
AC_SUBST([ARCHDIR])

# Set arch CFLAGS

AS_CASE([$host_cpu],
	[i?86],	
		[ARCH_CFLAGS="-ffreestanding -fno-pie"
		 ARCH_LDFLAGS="-nostdlib -no-pie"],

	[AC_MSG_ERROR("$host_cpu is not supported")])

AC_SUBST([ARCH_CFLAGS])
AC_SUBST([ARCH_LDFLAGS])

# Preserve the user variable CFLAGS so we can change it
# for the compiler tests

CFLAGS_ORIG="$CLFAGS"
CFLAGS="$ARCH_CFLAGS"
LDFLAGS_ORIG="$LDFLAGS"
LDFLAGS="$ARCH_LDFLAGS"
cross_compiling=yes

AC_PROG_CC([$host-gcc], [$host_cpu-elf-gcc])
AM_PROG_AS
AC_PROG_RANLIB
AM_PROG_AR

# Add arch-depenant libraries

AS_CASE([$host_cpu],
	[i?86],		[AC_CHECK_LIB([gcc], [__umoddi3])],
	[AC_MSG_ERROR("$host_cpu is not supported")])
AC_SUBST([ARCHDIR])

# Restore user CFLAGS

CFLAGS="$CFLAGS_ORIG"
LDFLAGS="$LDFLAGS_ORIG"

# Add --enable-debug option

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
