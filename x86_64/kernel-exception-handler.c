#include <stdio.h>
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

void
kernel_exception_handler(int const vector, int const error_code)
{
  switch (vector)
    {
    default:
      panic ("Unkown trap catch in kernel code at vector %#x", vector);
      break;
    }
}
