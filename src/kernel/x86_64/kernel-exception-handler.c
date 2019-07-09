/*
 * Copyright (C) 2018 Jake Shilling
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <stdio.h>

#include <panic.h>

enum exception {
  DIVIDE_BY_ZERO = 0,
  DEBUG = 1,

  BREAKPOINT = 3,
  OVERFLOW = 4,
  BOUND_RANGE = 5,
  INVALID_OPCODE = 6,
  DEVICE_NOT_AVAILABLE = 7,
  DOUBLE_FAULT = 8,

  INVALID_TSS = 10,
  SEGMENT_NOT_PRESENT = 11,
  STACK = 12,
  GENERAL_PROTECTION = 13,
  PAGE = 14,

  X87_FLOATING_POINT = 16,
  ALIGNMENT_CHECK = 17,
  MACHINE_CHECK = 18,
  SIMD_FLOATING_POINT = 19,

  VMM_COMMUNICATION = 29,
  SECURITY_EXCEPTION = 30
};

/* Return a string representing the type of exception that occured */
static const char *exception_name(const enum exception type) {
  switch (type) {
  case DIVIDE_BY_ZERO:
    return "Divide by Zero Exception";
  case DEBUG:
    return "Debug Exception";
  case BREAKPOINT:
    return "Breakpoint Exception";
  case OVERFLOW:
    return "Overflow Exception";
  case BOUND_RANGE:
    return "Bound-Range Exception";
  case INVALID_OPCODE:
    return "Invalid-Opcode Exception";
  case DEVICE_NOT_AVAILABLE:
    return "Device-Not-Available Exception";
  case DOUBLE_FAULT:
    return "Double Fault";
  case INVALID_TSS:
    return "Invalid TSS Exception";
  case SEGMENT_NOT_PRESENT:
    return "Segment Not Present Exception";
  case STACK:
    return "Stack Exception";
  case GENERAL_PROTECTION:
    return "General-Protection Exception";
  case PAGE:
    return "Page Fault";
  case X87_FLOATING_POINT:
    return "x87 Floating-Point Exception-Pending";
  case ALIGNMENT_CHECK:
    return "Alignment Check Exception";
  case MACHINE_CHECK:
    return "Machine Check Exception";
  case SIMD_FLOATING_POINT:
    return "SIMD Floating-Point Exception";
  case VMM_COMMUNICATION:
    return "VMM Communication Exception";
  case SECURITY_EXCEPTION:
    return "Security Exception";
  default:
    return "Unkown Exception";
  }
}

struct interrupt_frame {
  uint64_t rip;
  uint16_t cs;
  uint8_t padding0[6];
  uint64_t rflags;
  uint64_t rsp;
  uint16_t ss;
  uint8_t padding1[6];
} __attribute__((packed));

/* Return a string representing a Selector Error Code */
static const char *selector_error_message(int const error_code) {
  static const char *IDT = "IDT";
  static const char *GDT = "GDT";
  static const char *LDT = "LDT";

  static const char *fmt = "The exception source is "         /* 24 chars */
                           "%8.8s"                            /*  8 chars */
                           "to the processor and references " /* 32 chars */
                           "%3.3s"                            /*  3 chars */
                           "selector "                        /*  9 chars */
                           "%6.4x.";                          /*  6 chars */
  /* total:     82 chars */
  static char buffer[83];

  const char *table;
  if (error_code & (1 << 1))
    table = IDT;
  else if (error_code & (1 << 2))
    table = LDT;
  else
    table = GDT;

  snprintf(buffer, 83, fmt, (error_code & 1) ? "external" : "internal", table,
           (error_code & 0xFFF4));

  return buffer;
}

/* Return a string representing a page fault error code */
static const char *page_fault_error_message(int const error_code) {
  static const char *fmt =
      "Trying to "        /* 10 chars */
      "%s"                /* 9 chars: 'read from' / 'write to' */
      " a page that was " /* 17 chars */
      "%s"                /* 4chars '' / 'not ' */
      "present while in " /* 17 chars */
      "%s"                /* 10 chars 'supervisor' / 'user' */
      "mode. This is "    /* 14 chars */
      "%s"                /* 4 chars '' / 'not ' */
      "the result of reading a 1 from a reserved field within a " /* 57 chars */
      "page-translation-table entry and access was "              /* 44 chars */
      "%s"                     /* 4 chars '' / 'not ' */
      "an instruction fetch."; /* 21 chars */
                               /* Total: 211 */
  static char buffer[212];

  snprintf(buffer, 212, fmt, (error_code & (1 << 1)) ? "write to" : "read from",
           (error_code & (1 << 0)) ? "" : "not ",
           (error_code & (1 << 2)) ? "user" : "supervisor",
           (error_code & (1 << 3)) ? "" : "not ",
           (error_code & (1 << 4)) ? "" : "not ");

  return buffer;
}

/* Return a string representing an interrupt stack frame */
static const char *interrupt_stack_frame(struct interrupt_frame *const iframe) {
  static const char *fmt = "Interrupt Stack Frame:\n" /* 23 chars */
                           "  RIP    = %.16llx\n"     /* 28 chars */
                           "  CS     = %16.4x\n"      /* 28 chars */
                           "  RFLAGS = %.16llx\n"     /* 28 chars */
                           "  SS     = %16.4x\n";     /* 28 chars */
                                                      /* Total:  135 chars */
  static char buffer[136];

  snprintf(buffer, 136, fmt, iframe->rip, iframe->cs, iframe->rflags,
           iframe->ss);

  return buffer;
}

void kernel_exception_handler(int const vector, int const error_code,
                              struct interrupt_frame *const iframe) {
  switch (vector) {
  case STACK:
  case INVALID_TSS:
  case SEGMENT_NOT_PRESENT:
  case GENERAL_PROTECTION:
    panic("%s! %s\n\n%s", exception_name(vector),
          selector_error_message(error_code), interrupt_stack_frame(iframe));
    break;
  case PAGE:
    panic("Page Fault! %s\n\n%s", page_fault_error_message(error_code),
          interrupt_stack_frame(iframe));
    break;
  default:
    panic("%s! Vector: %u Error Code: %#x\n\n%s", exception_name(vector),
          vector, error_code, interrupt_stack_frame(iframe));
  }
}
