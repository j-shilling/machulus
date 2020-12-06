set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(triple x86_64-elf)

set(CMAKE_C_COMPILER ${triple}-gcc)
set(CMAKE_LINKER ${triple}-ld)
set(CMAKE_AR ${triple}-ar)
set(CMAKE_OBJCOPY ${triple}-objcopy)
set(CMAKE_RANLIB ${triple}-ranlib)
set(CMAKE_SIZE ${triple}-size)
set(CMAKE_STRIP ${triple}-strip)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
