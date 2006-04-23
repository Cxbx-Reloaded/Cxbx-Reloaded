// ******************************************************************
// * 
// * proj : openLIBC
// *
// * desc : Totally Free LIC replacement
// *
// * file : string.h
// *
// * note : This LIBC is TOTALLY free - do what you like with it!!
// *
// ******************************************************************

#ifndef		__OPENXDK_STRING__
#define		__OPENXDK_STRING__

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
char*	strcat( char *dest, const char *src );
char*	strncpy( char *dest, const char *src, unsigned int count );
char*	strcpy( char *dest, const char *src );
size_t	strlen( const char *string );
void*	memcpy( void *dest, const void *src, int count );
void*	memccpy( void *dest, const void *src, int c, unsigned int count );
int		memcmp( const void *buff1, const void *buff2, size_t count );
void	*memset( void *Dest, int fill, size_t length );





#ifdef	__cplusplus
};
#endif


#endif		// __OPENXDK_STRING__




