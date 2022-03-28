#include "machulus/panic.h"
#include <stdint.h>
#include <ubsan.h>

void __ubsan_handle_alignment_assumption(
    struct ubsan_alignment_assumption_data *data, void *ptr, void *alignment,
    void *offset) {

  if (!offset) {
    __ubsan_printf(
        &data->loc,
        "Assumption of %p byte alignment for pointer of type %s failed",
        alignment, data->type->type_name);
  } else {
    __ubsan_printf(&data->loc,
                   "Assumption of %p byte alignment (with offset of %p bytes) "
                   "for pointer of type %s failed",
                   alignment, offset, data->type->type_name);
  }

  if (data->assumption_loc.filename) {
    __ubsan_printf(&data->assumption_loc,
                   "Alignment assumption was specified here");
  }

  uintptr_t real_pointer = (uintptr_t)ptr - (uintptr_t)offset;
  uintptr_t lsb = __builtin_ctzl(real_pointer);
  uintptr_t actual_alignment = (uintptr_t)1 << lsb;

  uintptr_t mask = (uintptr_t)alignment - 1;
  uintptr_t misalignment_offset = real_pointer & mask;

  __ubsan_printf(&data->loc, "%saddress is %p, misalignment offset is %p bytes",
                 (offset ? "offset " : ""), (void *)actual_alignment,
                 (void *)misalignment_offset);
}

void __ubsan_handle_alignment_assumption_abort(
    struct ubsan_alignment_assumption_data *data, void *ptr, void *alignment,
    void *offset) {
  __ubsan_handle_alignment_assumption(data, ptr, alignment, offset);
  panic("Aborted after alignment assumption error");
}
