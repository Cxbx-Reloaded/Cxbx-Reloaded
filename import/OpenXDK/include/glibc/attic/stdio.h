



#ifndef	__STDIO_H__
#define	__STDIO_H__


# ifdef __cplusplus
extern "C" {
# endif

#include	<openxdk.h>

	/*

//int sprintf(char*, const char *format, ...);
int __cdecl sprintf(char *, const char *, ...);

//#include	<tamtypes.h>
#include	<stddef.h>
#include	<features.h>


#define _O_RDONLY					(0x00000001)
#define _O_WRONLY					(0x00000002)
#define _O_RDWR						(0x00000003)
#define _O_NBLOCK					(0x00000010)
#define _O_APPEND					(0x00000100)
#define _O_CREAT					(0x00000200)
#define _O_TRUNC					(0x00000400)
#define	_O_EXCL						(0x00000800)
#define	_O_TEMPORARY				(0x00001000)
#define	_O_TEXT						(0x00004000)
#define _O_BINARY					(0x00008000)

#define O_RDONLY					_O_RDONLY
#define O_WRONLY					_O_WRONLY
#define O_RDWR						_O_RDWR	
#define O_NBLOCK					_O_NBLOCK
#define O_APPEND					_O_APPEND
#define O_CREAT						_O_CREAT
#define	O_EXCL						_O_EXCL
#define O_TRUNC						_O_TRUNC
#define	O_TEMPORARY					_O_TEMPORARY
#define	O_TEXT						_O_TEXT	
#define O_BINARY					_O_BINARY


#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2

extern u32	LastErrorCode;

int _open( char *filename, int oflag, int permission );
int _read( int handle, void *buffer, unsigned int count );
int _write( int handle, void* buffer, unsigned int count );
int _close( int handle );
int _lseek( int handle, u32 offset, int base );

int nprintf(const char *format, ...);
int printf(const char *format, ...);
//int sprintf(char*, const char *format, ...);
*/

# ifdef __cplusplus
}
# endif

#endif 			// __STDIO_H__



