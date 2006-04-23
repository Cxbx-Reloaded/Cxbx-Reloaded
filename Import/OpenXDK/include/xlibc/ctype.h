// ******************************************************************
// * 
// * proj : openLIBC
// *
// * desc : Totally Free LIC replacement
// *
// * file : ctype.h
// *
// * note : This LIBC is TOTALLY free - do what you like with it!!
// *
// ******************************************************************

#ifndef		__OPENXDK_CTYPE__
#define		__OPENXDK_CTYPE__

#ifdef	__cplusplus
extern "C" {
#endif

#include <ansidecl.h>

// *****************************************************************************
// 
//	Structures
//
// *****************************************************************************


// *****************************************************************************
// 
//	Functions
//
// *****************************************************************************

#define	tolower(c)			(c>='A' && c<='Z')?c+('a'-'A'):c
#define	toupper(c)			(c>='a' && c<='z')?c+('A'-'a'):c
#define	isdigit(c)			(c>='0' && c<='9')?1:0
#define	isxdigit(c)			(c>='0' && c<='9' || c>='a' && c<='f' || c>='A' && c<='F')?1:0




#ifdef	__cplusplus
};
#endif

#endif		// __OPENXDK_CTYPE__




