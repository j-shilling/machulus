set(LIBUBSAN_SRCS
  ubsan.c
  alignment_assumption.c
  type_mismatch.c
  overflow.c)

set(LIBUBSAN_COMPILE_OPTIONS ${DEBUG_COMPILE_OPTIONS})
list(APPEND LIBUBSAN_COMPILE_OPTIONS ${FREESTANDING_COMPILE_OPTIONS})

add_library(ubsan ${LIBUBSAN_SRCS})
target_link_libraries(ubsan libc)
target_include_directories(ubsan PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(ubsan PRIVATE
  ${LIBUBSAN_COMPILE_OPTIONS})
