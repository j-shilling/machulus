#include <sys/cdefs.h>

#if __STD_C11
#  define noreturn		_Noreturn
#elif __GNUC_PREREQ (2,8)
#  define noreturn		__attribute__ ((__noreturn__))
#else
#  define noreturn		/* ignore */
#endif
