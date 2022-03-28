#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

struct ubsan_source_location {
  const char *filename;
  uint32_t line;
  uint32_t column;
};

typedef uint16_t ubsan_type_descriptor_kind;
static const ubsan_type_descriptor_kind UBSAN_TK_INTEGER = 0x0000;
static const ubsan_type_descriptor_kind UBSAN_TK_FLOAT = 0x00001;
static const ubsan_type_descriptor_kind UBSAN_TK_UNKOWN = 0xffff;

struct ubsan_type_descriptor {
  ubsan_type_descriptor_kind type_kind;
  uint16_t type_info;
  char type_name[];
};

struct ubsan_type_mismatch_data {
  struct ubsan_source_location loc;
  const struct ubsan_type_descriptor *type;
  unsigned char log_alignment;
  unsigned char type_check_kind;
};

struct ubsan_alignment_assumption_data {
  struct ubsan_source_location loc;
  struct ubsan_source_location assumption_loc;
  const struct ubsan_type_descriptor *type;
};

struct ubsan_overflow_data {
  struct ubsan_source_location loc;
  const struct ubsan_type_descriptor *type;
};

struct ubsan_shift_out_of_bounds_data {
  struct ubsan_source_location loc;
  const struct ubsan_type_descriptor *lhs_type;
  const struct ubsan_type_descriptor *rhs_type;
};

struct ubsan_out_of_bounds_data {
  struct ubsan_source_location loc;
  const struct ubsan_type_descriptor *array_type;
  const struct ubsan_type_descriptor *index_type;
};

struct ubsan_unreachable_data {
  struct ubsan_source_location loc;
};

struct ubsan_vla_bound_data {
  struct ubsan_source_location loc;
  const struct ubsan_type_descriptor *type;
};

struct ubsan_float_cast_overflow_data {
  const struct ubsan_type_descriptor *from_type;
  const struct ubsan_type_descriptor *to_type;
};

struct ubsan_float_cast_overflow_data_v2 {
  struct ubsan_source_location loc;
  const struct ubsan_type_descriptor *from_type;
  const struct ubsan_type_descriptor *to_type;
};

struct ubsan_invalid_value_data {
  struct ubsan_source_location loc;
  const struct ubsan_type_descriptor *type;
};

typedef unsigned char ubsan_implicit_conversion_check_kind;
static const ubsan_implicit_conversion_check_kind
    UBSAN_ICCK_INTEGER_TRUNCATION = 0; // Legacy, was only used by clang 7.
static const ubsan_implicit_conversion_check_kind
    UBSAN_ICCK_UNSIGNED_INTEGER_TRUNCATION = 1;
static const ubsan_implicit_conversion_check_kind
    UBSAN_ICCK_SIGNED_INTEGER_TRUNCATION = 2;
static const ubsan_implicit_conversion_check_kind
    UBSAN_ICCK_INTEGER_SIGN_CHANGE = 3;
static const ubsan_implicit_conversion_check_kind
    UBSAN_ICCK_SIGNED_INTEGER_TRUNCATION_OR_SIGN_CHANGE = 4;

struct ubsan_implicit_conversion_data {
  struct ubsan_source_location loc;
  const struct ubsan_type_descriptor *from_type;
  const struct ubsan_type_descriptor *to_type;
  ubsan_implicit_conversion_check_kind kind;
};

// unsigned char
typedef enum {
  BCK_CTZPassedZero,
  BCK_CLZPassedZero,
} ubsan_buildin_check_kind;

struct ubsan_invalid_builtin_data {
  struct ubsan_source_location loc;
  unsigned char kind;
};

struct ubsan_obj_c_cast {
  struct ubsan_source_location loc;
  const struct ubsan_type_descriptor *expected_type;
};

struct ubsan_non_null_return_data {
  struct ubsan_source_location attr_loc;
};

struct ubsan_non_null_arg_data {
  struct ubsan_source_location loc;
  struct ubsan_source_location attr_loc;
  int arg_index;
};

struct ubsan_pointer_overflow_data {
  struct ubsan_source_location loc;
};

typedef unsigned char ubsan_cfi_type_check_kind;
static const ubsan_cfi_type_check_kind UBSAN_CFITCK_VCALL = 0;
static const ubsan_cfi_type_check_kind UBSAN_CFITCK_NVCALL = 1;
static const ubsan_cfi_type_check_kind UBSAN_CFITCK_DERIVEDCAST = 2;
static const ubsan_cfi_type_check_kind UBSAN_CFITCK_UNRELATEDCAST = 3;
static const ubsan_cfi_type_check_kind UBSAN_CFITCK_ICALL = 4;
static const ubsan_cfi_type_check_kind UBSAN_CFITCK_NVMFCALL = 5;
static const ubsan_cfi_type_check_kind UBSAN_CFITCK_VMFCALL = 6;

struct ubsan_cfi_bad_icall_data {
  struct ubsan_source_location loc;
  const struct ubsan_type_descriptor *type;
};

struct ubsan_check_fail_data {
  ubsan_cfi_type_check_kind check_kind;
  struct ubsan_source_location loc;
  const struct ubsan_type_descriptor *type;
};

struct ubsan_report_options {
  bool from_unrecoverable_handler;
  void *pc;
  void *bp;
};

#define __ubsan_printf(loc, fmt, ...)                                          \
  fprintf(stderr, "UBSAN %s:%d:%d - " fmt, (loc)->filename, (loc)->line,       \
          (loc)->column, ##__VA_ARGS__)

void __ubsan_handle_type_mismatch_v1(struct ubsan_type_mismatch_data *, void *);
void __ubsan_handle_type_mismatch_v1_abort(struct ubsan_type_mismatch_data *,
                                           void *);
void __ubsan_handle_alignment_assumption(
    struct ubsan_alignment_assumption_data *, void *, void *, void *);
void __ubsan_handle_alignment_assumption_abort(
    struct ubsan_alignment_assumption_data *, void *, void *, void *);
void __ubsan_handle_add_overflow(struct ubsan_overflow_data *, void *, void *);
void __ubsan_handle_add_overflow_abort(struct ubsan_overflow_data *, void *,
                                       void *);
void __ubsan_handle_sub_overflow(struct ubsan_overflow_data *, void *, void *);
void __ubsan_handle_sub_overflow_abort(struct ubsan_overflow_data *, void *,
                                       void *);
void __ubsan_handle_mul_overflow(struct ubsan_overflow_data *, void *, void *);
void __ubsan_handle_mul_overflow_abort(struct ubsan_overflow_data *, void *,
                                       void *);
void __ubsan_handle_negate_overflow(struct ubsan_overflow_data *, void *);
void __ubsan_handle_negate_overflow_abort(struct ubsan_overflow_data *, void *);
void __ubsan_handle_divrem_overflow(struct ubsan_overflow_data *, void *,
                                    void *);
void __ubsan_handle_divrem_overflow_abort(struct ubsan_overflow_data *, void *,
                                          void *);
void __ubsan_handle_shift_out_of_bounds(struct ubsan_shift_out_of_bounds_data *,
                                        void *, void *);
void __ubsan_handle_shift_out_of_bounds_abort(
    struct ubsan_shift_out_of_bounds_data *, void *, void *);
void __ubsan_handle_out_of_bounds(struct ubsan_out_of_bounds_data *, void *);
void __ubsan_handle_out_of_bounds_abort(struct ubsan_out_of_bounds_data *,
                                        void *);
void __ubsan_handle_builtin_unreachable_abort(struct ubsan_unreachable_data *);
void __ubsan_handle_missing_return_abort(struct ubsan_unreachable_data *);
void __ubsan_handle_vla_bound_not_positive(struct ubsan_vla_bound_data *,
                                           void *);
void __ubsan_handle_vla_bound_not_positive_abort(struct ubsan_vla_bound_data *,
                                                 void *);
void __ubsan_handle_float_cast_overflow(void *, void *);
void __ubsan_handle_float_cast_overflow_abort(void *, void *);
void __ubsan_handle_load_invalid_value(struct ubsan_invalid_value_data *,
                                       void *);
void __ubsan_handle_load_invalid_value_abort(struct ubsan_invalid_value_data *,
                                             void *);
void __ubsan_handle_implicit_conversion(struct ubsan_implicit_conversion_data *,
                                        void *, void *);
void __ubsan_handle_implicit_conversion_abort(
    struct ubsan_implicit_conversion_data *, void *, void *);
void __ubsan_handle_invalid_builtin(struct ubsan_invalid_builtin_data *);
void __ubsan_handle_invalid_builtin_abort(struct ubsan_invalid_builtin_data *);
void __ubsan_handle_invalid_objc_cast(struct ubsan_obj_c_cast *, void *);
void __ubsan_handle_invalid_objc_cast_abort(struct ubsan_obj_c_cast *, void *);
void __ubsan_handle_nonnull_return_v1(struct ubsan_non_null_return_data *,
                                      struct ubsan_source_location *);
void __ubsan_handle_nonnull_return_v1_abort(struct ubsan_non_null_return_data *,
                                            struct ubsan_source_location *);
void __ubsan_handle_nullability_return_v1(struct ubsan_non_null_return_data *,
                                          struct ubsan_source_location *);
void __ubsan_handle_nullability_return_v1_abort(
    struct ubsan_non_null_return_data *, struct ubsan_source_location *);
void __ubsan_handle_nonnull_arg(struct ubsan_non_null_arg_data *);
void __ubsan_handle_nonnull_arg_abort(struct ubsan_non_null_arg_data *);
void __ubsan_handle_nullability_arg(struct ubsan_non_null_arg_data *);
void __ubsan_handle_nullability_arg_abort(struct ubsan_non_null_arg_data *);
void __ubsan_handle_pointer_overflow(struct ubsan_pointer_overflow_data *,
                                     void *, void *);
void __ubsan_handle_pointer_overflow_abort(struct ubsan_pointer_overflow_data *,
                                           void *, void *);
void __ubsan_handle_cfi_bad_icall(struct ubsan_cfi_bad_icall_data *, void *);
void __ubsan_handle_cfi_bad_icall_abort(struct ubsan_cfi_bad_icall_data *,
                                        void *);
void __ubsan_handle_cfi_check_fail(struct ubsan_check_fail_data *, void *,
                                   void *);
void __ubsan_handle_cfi_check_fail_abort(struct ubsan_check_fail_data *, void *,
                                         void *);
void __ubsan_handle_cfi_bad_type(struct ubsan_check_fail_data *, void *, bool,
                                 struct ubsan_report_options);
