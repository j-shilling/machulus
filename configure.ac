AC_INIT([jkernel], [0.0.1])
AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])

if test -z "$CFLAGS"; then
	CFLAGS="-std=gnu99 -ffreestanding -I$(pwd)/include"
fi
if test -z "$LDFLAGS"; then
	LDFLAGS="-ffreestanding -nostdlib -lgcc"
fi

AC_CANONICAL_HOST

if test "$host_os" != "elf"; then
	AC_MSG_ERROR("$host_os is not supported")
fi

AS_CASE([$host_cpu],
	[i?86], [ARCHDIR=arch/i386],
	[AC_MSG_ERROR("$host_cpu is not supported")])
AC_SUBST([ARCHDIR])

AC_PROG_CC

if test "$GCC" != "yes"; then
	AC_MSG_ERROR("gcc is required")
fi

AM_PROG_AS
AC_PROG_RANLIB
AM_PROG_AR

AC_MSG_CHECKING([for crtbegin.o])
CRTBEGIN=`${CC} ${CFLAGS} -print-file-name=crtbegin.o`
if test -z "$CRTBEGIN"; then
	AC_MSG_ERROR(["Could not find crtbegin.o"])
else
	AC_MSG_RESULT([$CRTBEGIN])
	AC_SUBST([CRTBEGIN])
fi

AC_MSG_CHECKING([for crtend.o])
CRTEND=`${CC} ${CFLAGS} -print-file-name=crtend.o`
if test -z "$CRTEND"; then
	AC_MSG_ERROR(["Could not find crtend.o"])
else
	AC_MSG_RESULT([$CRTEND])
	AC_SUBST([CRTEND])
fi

AC_CHECK_HEADER([stdint.h], [],
		 [AC_MSG_ERROR([Missing stdint.h])])

AC_CHECK_HEADER([stddef.h], [],
		 [AC_MSG_ERROR([Missing stddef.h])])

AC_CONFIG_FILES([Makefile \
		 src/Makefile \
		 src/libc/Makefile \
		 src/kernel/Makefile])
AC_OUTPUT
