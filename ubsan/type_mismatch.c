#include <stdint.h>
#include <stdio.h>

#include <ubsan.h>

#include <machulus/panic.h>

const char *type_check_kinds[] = {"load of",
                                  "store to",
                                  "reference binding to",
                                  "member access within",
                                  "member call on",
                                  "constructor call on",
                                  "downcast of",
                                  "downcast of",
                                  "upcast of",
                                  "cast to virtual base of",
                                  "_Nonnull binding to",
                                  "dynamic operation on"};

void __ubsan_handle_type_mismatch_v1(struct ubsan_type_mismatch_data *data,
                                     void *ptr) {
  const char *type_check_kind = type_check_kinds[data->type_check_kind];
  uintptr_t alignment = (1 << data->log_alignment);
  if (!ptr) {
    __ubsan_printf(&data->loc, "%s null pointer of type %s\n", type_check_kind,
                   data->type->type_name);
  } else if ((uintptr_t)ptr & alignment) {
    __ubsan_printf(&data->loc,
                   "%s misaligned address %p for type %s which requires %p "
                   "byte alignment\n",
                   type_check_kind, ptr, data->type->type_name,
                   (void *)alignment);
  } else {
    __ubsan_printf(
        &data->loc,
        "%s address %p with insufficient space for an object of type %s\n",
        type_check_kind, ptr, data->type->type_name);
  }
}

void __ubsan_handle_type_mismatch_v1_abort(
    struct ubsan_type_mismatch_data *data, void *ptr) {
  __ubsan_handle_type_mismatch_v1(data, ptr);
  panic("Aborted after type mismatch");
}
