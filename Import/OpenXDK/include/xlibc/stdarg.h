// ******************************************************************
// * 
// * proj : openLIBC
// *
// * desc : Totally Free LIC replacement
// *
// * file : stdarg.h
// *
// * note : This LIBC is TOTALLY free - do what you like with it!!
// *
// ******************************************************************
// a
#ifndef		__OPENXDK_STDARG__
#define		__OPENXDK_STDARG__

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef  _MSC_VER
#pragma pack(push,8)
#endif  


#ifndef _VA_LIST_DEFINED
#ifdef  _M_ALPHA
typedef struct {
        char	*a0;			// pointer to first argument
        int		offset;			// byte offset of next arg
} va_list;
#else
typedef char *  va_list;
#endif
#define _VA_LIST_DEFINED
#endif



// GET rid of C4146 compiler warnings: unary minus operator applied to unsigned type, result still unsigned
#define	NEGU32(a)		((u32) (-((s32)(a))) )


#define _INTSIZEOF(n)			( (sizeof(n) + (sizeof(int) - 1)) & NEGU32(sizeof(int)) )
#define va_start(ap,v)			( ap = (va_list)&v + _INTSIZEOF(v) )
#define va_arg(ap,t)			( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define va_end(ap)				( ap = (va_list)0 )




#ifdef  _MSC_VER
#pragma pack(pop)
#endif




#ifdef	__cplusplus
};
#endif
#endif		// __OPENXDK_STDARG__




