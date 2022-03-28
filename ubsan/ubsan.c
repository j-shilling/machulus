#include <ubsan.h>

void __ubsan_handle_type_mismatch_v1(struct ubsan_type_mismatch_data *data,
                                     void *ptr) {}
void __ubsan_handle_type_mismatch_v1_abort(
    struct ubsan_type_mismatch_data *data, void *ptr) {}
void __ubsan_handle_alignment_assumption(
    struct ubsan_alignment_assumption_data *Data, void *Pointer,
    void *Alignment, void *Offset){}
void __ubsan_handle_alignment_assumption_abort(
    struct ubsan_alignment_assumption_data *Data, void *Pointer,
    void *Alignment, void *Offset){}
void __ubsan_handle_add_overflow(struct ubsan_overflow_data *Data, void *LHS,
                                  void *RHS){}
void __ubsan_handle_add_overflow_abort(struct ubsan_overflow_data *Data,
                                        void *LHS, void *RHS){}
void __ubsan_handle_sub_overflow(struct ubsan_overflow_data *Data, void *LHS,
                                  void *RHS){}
void __ubsan_handle_sub_overflow_abort(struct ubsan_overflow_data *Data,
                                        void *LHS, void *RHS){}
void __ubsan_handle_mul_overflow(struct ubsan_overflow_data *Data, void *LHS,
                                  void *RHS){}
void __ubsan_handle_mul_overflow_abort(struct ubsan_overflow_data *Data,
                                        void *LHS, void *RHS){}
void __ubsan_handle_negate_overflow(struct ubsan_overflow_data *Data,
                                     void *OldVal){}
void __ubsan_handle_negate_overflow_abort(struct ubsan_overflow_data *Data,
                                           void *OldVal){}
void __ubsan_handle_divrem_overflow(struct ubsan_overflow_data *Data,
                                     void *LHS, void *RHS){}
void __ubsan_handle_divrem_overflow_abort(struct ubsan_overflow_data *Data,
                                           void *LHS, void *RHS){}
void __ubsan_handle_shift_out_of_bounds(
    struct ubsan_shift_out_of_bounds_data *Data, void *LHS, void *RHS){}
void __ubsan_handle_shift_out_of_bounds_abort(
    struct ubsan_shift_out_of_bounds_data *Data, void *LHS, void *RHS){}
void __ubsan_handle_out_of_bounds(struct ubsan_out_of_bounds_data *Data,
                                   void *Index){}
void __ubsan_handle_out_of_bounds_abort(struct ubsan_out_of_bounds_data *Data,
                                         void *Index){}
void __ubsan_handle_builtin_unreachable_abort(
    struct ubsan_unreachable_data *Data){}
void __ubsan_handle_missing_return_abort(struct ubsan_unreachable_data *Data){}
void __ubsan_handle_vla_bound_not_positive(struct ubsan_vla_bound_data *Data,
                                            void *Bound){}
void __ubsan_handle_vla_bound_not_positive_abort(
    struct ubsan_vla_bound_data *Data, void *Bound){}
void __ubsan_handle_float_cast_overflow(void *Data, void *From){}
void __ubsan_handle_float_cast_overflow_abort(void *Data, void *From){}
void __ubsan_handle_load_invalid_value(struct ubsan_invalid_value_data *Data,
                                        void *Val){}
void __ubsan_handle_load_invalid_value_abort(
    struct ubsan_invalid_value_data *Data, void *Val){}
void __ubsan_handle_implicit_conversion(
    struct ubsan_implicit_conversion_data *Data, void *Src, void *Dst){}
void __ubsan_handle_implicit_conversion_abort(
    struct ubsan_implicit_conversion_data *Data, void *Src, void *Dst){}
void __ubsan_handle_invalid_builtin(struct ubsan_invalid_builtin_data *Data){}
void __ubsan_handle_invalid_builtin_abort(
    struct ubsan_invalid_builtin_data *Data){}
void __ubsan_handle_invalid_objc_cast(struct ubsan_obj_c_cast *Data,
                                       void *Pointer){}
void __ubsan_handle_invalid_objc_cast_abort(struct ubsan_obj_c_cast *Data,
                                             void *Pointer){}
void __ubsan_handle_nonnull_return_v1(struct ubsan_non_null_return_data *Data,
                                       struct ubsan_source_location *Loc){}
void __ubsan_handle_nonnull_return_v1_abort(
    struct ubsan_non_null_return_data *Data, struct ubsan_source_location *Loc){}
void __ubsan_handle_nullability_return_v1(
    struct ubsan_non_null_return_data *Data, struct ubsan_source_location *Loc){}
void __ubsan_handle_nullability_return_v1_abort(
    struct ubsan_non_null_return_data *Data, struct ubsan_source_location *Loc){}
void __ubsan_handle_nonnull_arg(struct ubsan_non_null_arg_data *Data){}
void __ubsan_handle_nonnull_arg_abort(struct ubsan_non_null_arg_data *Data){}
void __ubsan_handle_nullability_arg(struct ubsan_non_null_arg_data *Data){}
void __ubsan_handle_nullability_arg_abort(
    struct ubsan_non_null_arg_data *Data){}
void __ubsan_handle_pointer_overflow(struct ubsan_pointer_overflow_data *Data,
                                      void *Base, void *Result){}
void __ubsan_handle_pointer_overflow_abort(
    struct ubsan_pointer_overflow_data *Data, void *Base, void *Result){}
void __ubsan_handle_cfi_bad_icall(struct ubsan_cfi_bad_icall_data *Data,
                                   void *Function){}
void __ubsan_handle_cfi_bad_icall_abort(struct ubsan_cfi_bad_icall_data *Data,
                                         void *Function){}
void __ubsan_handle_cfi_check_fail(struct ubsan_check_fail_data *Data, void *Function,
                                    void *VtableIsValid){}
void __ubsan_handle_cfi_check_fail_abort(struct ubsan_check_fail_data *Data,
                                          void *Function, void *VtableIsValid){}
void __ubsan_handle_cfi_bad_type(struct ubsan_check_fail_data *Data, void *Vtable,
                                 bool ValidVtable,
                                 struct ubsan_report_options Opts){}
