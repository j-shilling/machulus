include(ExternalProject)

set(NEWLIB_SRC ${CMAKE_CURRENT_BINARY_DIR}/src)
set(NEWLIB_BIN ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(NEWLIB_INSTALL ${CMAKE_CURRENT_BINARY_DIR}/install)
set(NEWLIB_INCLUDE ${NEWLIB_INSTALL}/x86_64-elf/include)
set(NEWLIB_LOCATION ${NEWLIB_INSTALL}/x86_64-elf/lib/no-red-zone/libc.a)
set(NOSYS_LOCATION ${NEWLIB_INSTALL}/x86_64-elf/lib/no-red-zone/libnosys.a)

set(NEWLIB_CONFIG_OPTIONS
  --prefix /
  --program-prefix x86-64-elf-
  --target=x86_64-elf)

set(NEWLIB_COMPILE_OPTIONS
  "-ggdb"
  "-O0"
  "-ffreestanding"
  "-mcmodel=kernel"
  "-mno-red-zone"
  "-fno-pic")
list(JOIN NEWLIB_COMPILE_OPTIONS " " NEWLIB_CFLAGS)

ExternalProject_Add(newlib
  SOURCE_DIR ${NEWLIB_SRC}
  BINARY_DIR ${NEWLIB_BIN}
  INSTALL_DIR ${NEWLIB_INSTALL}
  URL "ftp://sourceware.org/pub/newlib/newlib-4.2.0.20211231.tar.gz"
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env
      CFLAGS_FOR_TARGET=${NEWLIB_CFLAGS}
      AR_FOR_TARGET=x86_64-elf-ar
      AS_FOR_TARGET=x86_64-elf-as
      DLLTOOL_FOR_TARGET=x86_64-elf-dlltool
      LD_FOR_TARGET=x86_64-elf-ld
      LIPO_FOR_TARGET=x86_64-elf-lipo
      NM_FOR_TARGET=x86_64-elf-nm
      RANLIB_FOR_TARGET=x86_64-elf-ranlib
      STRIP_FOR_TARGET=x86_64-elf-strip
      WINDRES_FOR_TARGET=x86_64-elf-windres
      WINDMC_FOR_TARGET=x86_64-elf-windmc
      OBJCOPY_FOR_TARGET=x86_64-elf-objcopy
      OBJDUMP_FOR_TARGET=x86_64-elf-objdump
      READELF_FOR_TARGET=x86_64-elf-readelf
      ${NEWLIB_SRC}/configure ${NEWLIB_CONFIG_OPTIONS}
  BUILD_COMMAND ${CMAKE_COMMAND} -E env
      make
  INSTALL_COMMAND make DESTDIR=${NEWLIB_INSTALL} install)

file(MAKE_DIRECTORY ${NEWLIB_INCLUDE})

add_library(libc IMPORTED STATIC GLOBAL)
add_library(libnosys IMPORTED STATIC GLOBAL)
add_dependencies(libc newlib)

set_target_properties(libc PROPERTIES
  IMPORTED_LOCATION ${NEWLIB_LOCATION}
  INTERFACE_INCLUDE_DIRECTORIES ${NEWLIB_INCLUDE})

set(LIBSYS_SRCS
  sys.c)
add_library(sys ${LIBSYS_SRCS})
add_dependencies(sys libc)
target_include_directories(sys PUBLIC ${NEWLIB_INCLUDE})
target_compile_options(sys PUBLIC ${NEWLIB_COMPILE_OPTIONS})
