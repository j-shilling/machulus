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
	[x86_64],	[ARCHDIR=x86],
	[AC_MSG_ERROR("$host_cpu is not supported")])
AC_SUBST([ARCHDIR])

# Set arch specific symbols

AS_CASE([$host_cpu],
	[i?86],	
		 [AC_DEFINE([__x86_64__], [0], [64-bit kernel])
		  AC_DEFINE_UNQUOTED([__CPU_VERSION__], [`echo $host_cpu | sed -e 's/i//'`], [x86 version])
      ARCH_CFLAGS="-ffreestanding -nostdlib"
      ARCH_LDFLAGS="-T ./arch/${ARCHDIR}/linker.ld -n"
      
      AC_PROG_GREP
      if test -z $GREP; then
        AC_MSG_ERROR(["grep is required to generate the x86 linker file"])
      fi],
	[x86_64],
		 [AC_DEFINE([__x86_64__], [1], [64-bit kernel])
      ARCH_CFLAGS="-ffreestanding -nostdlib"
      ARCH_LDFLAGS="-T ./arch/${ARCHDIR}/linker.ld -n"
      
      AC_PROG_GREP
      if test -z $GREP; then
        AC_MSG_ERROR(["grep is required to generate the x86 linker file"])
      fi],

	[AC_MSG_ERROR("$host_cpu is not supported")])
AC_SUBST([ARCH_CFLAGS])
AC_SUBST([ARCH_LDFLAGS])

AC_PROG_CC
AC_PROG_CPP
AM_PROG_AS
AC_PROG_RANLIB
AM_PROG_AR

# Add arch-depenant libraries

AS_CASE([$host_cpu],
	[i?86],		[AC_CHECK_LIB([gcc], [__umoddi3])],

	[])

# Add --enable-debug option

AC_ARG_ENABLE([debug], AS_HELP_STRING([--enable-debug], [build in debug mode]), [], [])
AS_IF([test "$enable_debug" = "yes"], [
  AC_DEFINE([__DEBUG__], [1], [debug mode])
  DEBUG_CFLAGS="-ggdb -O0 -Werror"
], [
  AC_DEFINE([__DEBUG__], [0], [debug mode])
  DEBUG_CFLAGS=""
])
AC_SUBST([DEBUG_CFLAGS])

# Add --enable-pae option

AC_ARG_ENABLE([pae], AS_HELP_STRING([--enable-pae], [use physical address extension on x86 machines]), [], [])
AS_IF([test "$enable_pae" = "yes" -o "$host_cpu" = "x86_64"], [

  AS_IF([test "$enable_pae" = "no"], [
    AC_MSG_WARN([PAE is required on x86_64])
  ])

  AC_DEFINE([__PAE__], [1], [use physical address extension])
], [
  AC_DEFINE([__PAE__], [0], [use physical address extension])
])

AC_CONFIG_FILES([Makefile
		 arch/Makefile
		 arch/x86/Makefile
                 common/Makefile])
AC_OUTPUT
