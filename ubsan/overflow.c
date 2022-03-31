#include <stdint.h>
#include <string.h>
#include <ubsan.h>

#include <machulus/panic.h>

static inline bool __is_signed(struct ubsan_overflow_data *data) {
  return data->type->type_info & 1;
}

static inline int __get_bit_width(struct ubsan_overflow_data *data) {
  if (data->type->type_kind == UBSAN_TK_FLOAT)
    return data->type->type_info;
  else
    return (1 << (data->type->type_info >> 1));
}

static inline bool __is_inline_int(struct ubsan_overflow_data *data) {
  const int inline_bits = sizeof(void *) * 8;
  const int int_bit_width = __get_bit_width(data);
  return int_bit_width <= inline_bits;
}

static inline intmax_t __get_int_value(struct ubsan_overflow_data *data,
                                       void *val) {
  const int int_bit_width = __get_bit_width(data);

  if (__is_inline_int(data)) {
    const int extra_bits = sizeof(intmax_t) * 8 - int_bit_width;
    return (intmax_t)((uintmax_t)val << extra_bits) >> extra_bits;
  }

  if (int_bit_width == 64) {
    return (intmax_t)(*((int64_t *)val));
  }

  if (int_bit_width == 128) {
    return (intmax_t)(*((__int128 *)val));
  }

  panic("Invalid integer width in overflow handler");
  __builtin_unreachable();
}

static inline uintmax_t __get_uint_value(struct ubsan_overflow_data *data,
                                         void *val) {
  if (__is_inline_int(data)) {
    return (uintmax_t)val;
  }

  const int int_bit_width = __get_bit_width(data);

  if (int_bit_width == 64) {
    return (uintmax_t)(*((uint64_t *)val));
  }

  if (int_bit_width == 128) {
    return (uintmax_t)(*((unsigned __int128 *)val));
  }

  panic("Invalid integer width in overflow handler");
  __builtin_unreachable();
}

static inline long double __get_float_value(struct ubsan_overflow_data *data,
                                            void *val) {
  int bits = __get_bit_width(data);
  if (bits <= (int)(sizeof(void *) * 8)) {
    // Inline float value
    switch (bits) {
    case 32: {
      float value;
      memcpy(&value, &val, 4);
      return (long double)value;
    }
    case 64: {
      double value;
      memcpy(&value, &val, 8);
      return (long double)value;
    }
    }
  } else {
    switch (bits) {
    case 64:
      return (long double)*((double *)val);
    case 80:
    case 96:
    case 128:
      return *((long double *)val);
    }
  }
  return 0;
}

static inline void __handle_overflow_sl_sr(struct ubsan_overflow_data *data,
                                           const char *operator, intmax_t lhs,
                                           intmax_t rhs) {
  __ubsan_printf(
      &data->loc,
      "signed integer oveflow: %jd %s %jd cannot be represented in type %s",
      lhs, operator, rhs, data->type->type_name);
}
static inline void __handle_overflow_ul_ur(struct ubsan_overflow_data *data,
                                           const char *operator, uintmax_t lhs,
                                           uintmax_t rhs) {
  __ubsan_printf(
      &data->loc,
      "unsigned integer oveflow: %ju %s %ju cannot be represented in type %s",
      lhs, operator, rhs, data->type->type_name);
}

static inline void __handle_overflow(struct ubsan_overflow_data *data,
                                     const char *operator, void * lhs,
                                     void *rhs) {
  if (__is_signed(data)) {
    __handle_overflow_sl_sr(data, operator, __get_int_value(data, lhs),
                            __get_int_value(data, rhs));
  } else {
    __handle_overflow_ul_ur(data, operator, __get_uint_value(data, lhs),
                            __get_uint_value(data, rhs));
  }
}

void __ubsan_handle_add_overflow(struct ubsan_overflow_data *data, void *lhs,
                                 void *rhs) {
  __handle_overflow(data, "+", lhs, rhs);
}
void __ubsan_handle_add_overflow_abort(struct ubsan_overflow_data *data,
                                       void *lhs, void *rhs) {
  __ubsan_handle_add_overflow(data, lhs, rhs);
  panic("Aborted after add overflow");
  __builtin_unreachable();
}

void __ubsan_handle_sub_overflow(struct ubsan_overflow_data *data, void *lhs,
                                 void *rhs) {
  __handle_overflow(data, "-", lhs, rhs);
}

void __ubsan_handle_sub_overflow_abort(struct ubsan_overflow_data *data,
                                       void *lhs, void *rhs) {
  __ubsan_handle_sub_overflow(data, lhs, rhs);
  panic("Aborted after sub overflow");
  __builtin_unreachable();
}

void __ubsan_handle_mul_overflow(struct ubsan_overflow_data *data, void *lhs,
                                 void *rhs) {
  __handle_overflow(data, "*", lhs, rhs);
}

void __ubsan_handle_mul_overflow_abort(struct ubsan_overflow_data *data,
                                       void *lhs, void *rhs) {
  __ubsan_handle_mul_overflow(data, lhs, rhs);
  panic("Aborted after mul overflow");
  __builtin_unreachable();
}

void __ubsan_handle_negate_overflow(struct ubsan_overflow_data *data,
                                    void *old_val) {
  if (__is_signed(data)) {
    if (data->type->type_kind == UBSAN_TK_FLOAT) {
      __ubsan_printf(
          &data->loc,
          "negation of %Lf cannot be represented in type %s; cast to "
          "an unsigned type to negate this value to itself",
          __get_float_value(data, old_val), data->type->type_name);
    } else {
      __ubsan_printf(
          &data->loc,
          "negation of %jd cannot be represented in type %s; cast to "
          "an unsigned type to negate this value to itself",
          __get_int_value(data, old_val), data->type->type_name);
    }
  } else {
    __ubsan_printf(&data->loc,
                   "negation of %ju cannot be represented in type %s",
                   __get_uint_value(data, old_val), data->type->type_name);
  }
}

void __ubsan_handle_negate_overflow_abort(struct ubsan_overflow_data *data,
                                          void *old_val) {
  __ubsan_handle_negate_overflow(data, old_val);
  panic("Aborted after negate overflow");
  __builtin_unreachable();
}

void __ubsan_handle_divrem_overflow(struct ubsan_overflow_data *data, void *lhs,
                                    void *rhs) {
  if (data->type->type_kind == UBSAN_TK_INTEGER && __is_signed(data) &&
      __get_int_value(data, rhs) == -1) {

    __ubsan_printf(&data->loc,
                   "division of %ju by -1 cannot be represented in type %s",
                   __get_int_value(data, lhs), data->type->type_name);
  } else {
    __ubsan_printf(&data->loc, "division by zero");
  }
}

void __ubsan_handle_divrem_overflow_abort(struct ubsan_overflow_data *data,
                                          void *lhs, void *rhs) {
  __ubsan_handle_divrem_overflow(data, lhs, rhs);
  panic("Aborted after divem overflow");
  __builtin_unreachable();
}
