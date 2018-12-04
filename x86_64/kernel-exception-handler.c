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

static const char *exception_names[] = {
					"Divide by Zero Error",
					"Debug",
					"Non-Maskable Interrupt",
					"Breakpoint",
					"Overflow",
					"Bound Range Exception",
					"Invalid Opcode",
					"Device Not Available",
					"Double Fault",
					NULL,
					"Invalid TSS",
					"Segment Not Present",
					"Stack Exception",
					"General Protection Exception",
					"Page Fault",
				        NULL,
					"x87 Floating-Point Exception Pending",
					"Alignment Check",
					"Machine Check",
					"SIMD Floating-Point Exception",
					NULL,
					NULL,
					NULL,
					NULL,
					NULL,
					NULL,
					NULL,
					NULL,
					NULL,
					"VMM Communication Exception",
					"Security Exception"
					
};

static const char *selector_type_names[] = {
					  "GDT",
					  "IDT",
					  "LDT",
					  "IDT"
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
    case STACK:
    case INVALID_TSS:
    case SEGMENT_NOT_PRESENT:
    case GENERAL_PROTECTION:
      if (error_code)
	{
	  panic ("%s! From %s the processor\n"
		 "%s Selector = %u\n\n"
		 "Interrupt Stack Frame:\n"
		 "  RIP    = %.16llx\n"
		 "  CS     = %16.4x\n"
		 "  RFLAGS = %.16llx\n"
		 "  SS     = %16.4x\n",
		 exception_names[vector],
		 (error_code & (1 << 0)) ? "outside" : "inside",
		 selector_type_names[(error_code >> 1) & 0x3],
		 (error_code >> 3) & 0x1FFF,
		 iframe->rip,
		 iframe->cs,
		 iframe->rflags,
		 iframe->ss);
	}
      else
	{
	  panic ("%s! No selector index passed\n",
		 "Interrupt Stack Frame:\n"
		 "  RIP    = %.16llx\n"
		 "  CS     = %16.4x\n"
		 "  RFLAGS = %.16llx\n"
		 "  SS     = %16.4x\n",
		 exception_names[vector],
		 iframe->rip,
		 iframe->cs,
		 iframe->rflags,
		 iframe->ss);
	}
		 
      break;
    case PAGE:
      panic ("Page Fault!\n"
	     "Illegal %s on a %s page while in %s mode.\n"
	     "%s%s"
	     "Interrupt Stack Frame:\n"
	     "  RIP    = %.16llx\n"
	     "  CS     = %16.4x\n"
	     "  RFLAGS = %.16llx\n"
	     "  SS     = %16.4x\n",
	     (error_code & (1 << 1)) ? "write" : "read",
	     (error_code & (1 << 0)) ? "present" : "non-present",
	     (error_code & (1 << 2)) ? "user" : "supervisor",
	     (error_code & (1 << 3)) ? "Processor read a 1 from a reserved field within a page-translation-table entry\n" : "",
	     (error_code & (1 << 4)) ? "This fault was caused by an instruction fetch\n" : "\n",
	     iframe->rip,
	     iframe->cs,
	     iframe->rflags,
	     iframe->ss);
      break;
    default:
      panic ("%s! Vector: %u Error Code: %#x\n\n"
	     "Interrupt Stack Frame:\n"
	     "  RIP    = %.16llx\n"
	     "  CS     = %16.4x\n"
	     "  RFLAGS = %.16llx\n"
	     "  SS     = %16.4x",
	     exception_names[vector],
	     vector,
	     error_code,
	     iframe->rip,
	     iframe->cs,
	     iframe->rflags,
	     iframe->ss);
    }
}
