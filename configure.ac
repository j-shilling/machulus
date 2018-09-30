dnl Copyright (C) 2018 Jake Shilling <shilling.jake@gmail.com>
dnl 
dnl This program is free software: you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation, either version 3 of the License, or
dnl (at your option) any later version.
dnl 
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl 
dnl You should have received a copy of the GNU General Public License
dnl along with this program.  If not, see <http://www.gnu.org/licenses/>.

dnl file: configure.ac
dnl
dnl   This is the main file used by autoconf to generate the configure
dnl script. It initializes autoconf and automake, checks for programs
dnl relevant to all system types, then includes other configfrag.ac files
dnl do to more specialized tasks.

dnl
dnl INITIALIZE AUTOTOOLS
dnl

AC_INIT([machulus], [0.0.1])
AC_CONFIG_HEADERS([./config.h])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])

AM_SILENT_RULES([yes])

dnl
dnl GET CPU TYPE
dnl

AC_CANONICAL_HOST
AS_CASE([$host_cpu],
	[i?86],		[systype=i386],
	[systype=$host_cpu])
AC_SUBST([systype])

dnl
dnl FIND PROGRAMS
dnl

AM_PROG_AS
AC_PROG_CC
AC_PROG_CPP
AC_PROG_RANLIB
AM_PROG_AR

dnl grub-mkrescue IS USED TO GENERATE AN ISO FILE
AC_CHECK_PROG([GRUB_MKRESCUE], [grub-mkrescue], [grub-mkrescue], [no])
AS_IF([test "x$GRUB_MKRESCUE" = "xno"],
        [AC_CHECK_PROG([GRUB_MKRESCUE], [grub2-mkrescue], [grub2-mkrescue], [no])]
     )
AM_CONDITIONAL([HAVE_GRUB_MKRESCUE], [test "x$GRUB_MKRESCUE" != "xno"])

dnl ADD GLOBAL CONFIG OPTIONS
m4_include([configfrag.ac])

dnl ADD AMD64 CONFIG OPTIONS
m4_include([x86_64/configfrag.ac])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
