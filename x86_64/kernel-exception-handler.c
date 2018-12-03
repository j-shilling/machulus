#include <stdio.h>
#include <stdint.h>

#include <panic.h>

enum exceptions
  {
   DIVIDE_BY_ZERO       =  0,
   DEBUG                =  1,
   
   BREAKPOINT           =  3,
   OVERFLOW             =  4,
   BOUND_RANGE          =  5,
   INVALID_OPCODE       =  6,
   DEVICE_NOT_AVAILABLE =  7,
   DOUBLE_FAULT         =  8,
   
   INVALID_TSS          = 10,
   SEGMENT_NOT_PRESENT  = 11,
   STACK                = 12,
   GENERAL_PROTECTION   = 13,
   PAGE                 = 14,
   
   X87_FLOATING_POINT   = 16,
   ALIGNMENT_CHECK      = 17,
   MACHINE_CHECK        = 18,
   SIMD_FLOATING_POINT  = 19,

   VMM_COMMUNICATION    = 29,
   SECURITY_EXCEPTION   = 30
  };

struct interrupt_frame
{
  uint64_t rip;
  uint16_t cs;
  uint8_t padding0[6];
  uint64_t rflags;
  uint64_t rsp;
  uint16_t ss;
  uint8_t padding1[6];
} __attribute__((packed));

void
kernel_exception_handler(int const vector, int const error_code, struct interrupt_frame *const iframe)
{
  switch (vector)
    {
    default:
      panic ("Unkown trap catch in kernel code at vector %#x", vector);
      break;
    }
}
