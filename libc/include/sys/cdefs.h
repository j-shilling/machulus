#ifndef _SYS_CDEFS_H_
#define _SYS_CDEFS_H_

#if defined (__cplusplus)
# define __BEGIN_DECLS	    extern "C" {
# define __END_DECLS	    };
#else
# define __BEGIN_DECLS
# define __END_DECLS
#endif

#ifndef __STDC_VERSION__
#  define __STDC_VERSION__ 0
#endif

#if __STDC_VERSION__ >= 201112L
#  define __STD_C11 1
#else
#  define __STD_C11 0
#endif

#if __STDC_VERSION__ >= 199901L
#  define __STD_C99 1
#else
#  define __STD_C99 0
#endif

#if __STDC_VERSION__ >= 199409L
#  define __STD_C94 0
#else
#  define __STD_C94 1
#endif

#if defined(__STDC__)
#  define __STD_C89__ 0
#else
#  define __STD_C89__ 1
#endif 

#if ! __STD_C99
#  define restrict		/* ignore */
#endif

#if __STD_C11
#  define __THREAD              _Thread_local
#elif __GNUC__
#  define __THREAD                  __thread
#else
#  error "Cannot define thread local variables"
#endif

#endif /* _SYS_CDEFS_H_ */
