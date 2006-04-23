// ******************************************************************
// * 
// * proj : openLIBC
// *
// * desc : Totally Free LIC replacement
// *
// * file : ansidecl.h
// *
// * note : This LIBC is TOTALLY free - do what you like with it!!
// *
// ******************************************************************

#ifndef		__OPENXDK_ANSIDECL__
#define		__OPENXDK_ANSIDECL__

#ifdef	__cplusplus
extern "C" {
#endif



// ******************************************************************
// * Bigboy types
// ******************************************************************
#ifndef	_BIGBOY_TYPES_
#define	_BIGBOY_TYPES_
typedef	unsigned char		byte;
typedef	unsigned char		u8;
typedef	signed char			s8;
typedef	unsigned short		u16;
typedef	signed short		s16;
typedef	unsigned int		u32;
typedef	signed int			s32;
#endif	//_BIGBOY_TYPES_



#ifndef	NULL
#define	NULL		0x00
#endif

#ifndef	true
#define	true		1
#endif
#ifndef	false
#define	false		0
#endif

#ifndef	__SIZE_T_
#define	__SIZE_T_
typedef	unsigned int	size_t;
#endif	//__SIZE_T_










#ifdef	__cplusplus
};
#endif

#endif		// __OPENXDK_ANSIDECL__



