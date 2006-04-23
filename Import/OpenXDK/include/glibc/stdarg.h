/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */
#ifndef __dj_include_stdarg_h_
#define __dj_include_stdarg_h_

#ifdef __cplusplus
extern "C" {
#endif



#ifdef  _MSC_VER
#pragma pack(push,8)
#endif  



#ifndef _VA_LIST_DEFINED
#ifdef  _M_ALPHA
typedef struct {
        char *a0;       /* pointer to first homed integer argument */
        int offset;     /* byte offset of next parameter */
} va_list;
#else
typedef char *  va_list;
#endif
#define _VA_LIST_DEFINED
#endif




#define _INTSIZEOF(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )

#define va_start(ap,v)  ( ap = (va_list)&v + _INTSIZEOF(v) )
#define va_arg(ap,t)    ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define va_end(ap)      ( ap = (va_list)0 )


/*


#ifndef __dj_ENFORCE_ANSI_FREESTANDING

#ifdef __dj_include_varargs_h_
#error stdarg.h and varargs.h are mutually exclusive
#endif

#include <sys/djtypes.h>

__DJ_va_list
#undef __DJ_va_list
#define __DJ_va_list
  
#define __dj_va_rounded_size(T)  \
  (((sizeof (T) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))

#define va_arg(ap, T) \
    (ap = (va_list) ((char *) (ap) + __dj_va_rounded_size (T)),	\
     *((T *) (void *) ((char *) (ap) - __dj_va_rounded_size (T))))

#define va_end(ap)

#define va_start(ap, last_arg) \
 (ap = ((va_list) __builtin_next_arg (last_arg)))
  
#ifndef __STRICT_ANSI__

#ifndef _POSIX_SOURCE

#endif // !_POSIX_SOURCE 
#endif // !__STRICT_ANSI__ 
#endif // !__dj_ENFORCE_ANSI_FREESTANDING 

#ifndef __dj_ENFORCE_FUNCTION_CALLS
#endif //!__dj_ENFORCE_FUNCTION_CALLS 

*/

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */


#ifdef __cplusplus
}
#endif

#endif /* !__dj_include_stdarg_h_ */
