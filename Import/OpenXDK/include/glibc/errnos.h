#ifdef _ERRNO_H
#endif /* <errno.h> included.  */

#if !defined(__Emath_defined) &&  (defined(_ERRNO_H) || defined(__need_Emath))
#define EDOM 1
#define ERANGE 2
#else /* Emath not defined and <errno.h> included or need Emath.  */
#define ENOSYS 3
#endif /* <errno.h> included.  */

#undef __need_Emath
#ifndef __Emath_defined
#define __Emath_defined 1
#endif
