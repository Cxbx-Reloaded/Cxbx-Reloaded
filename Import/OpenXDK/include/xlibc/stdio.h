// ******************************************************************
// * 
// * proj : openLIBC
// *
// * desc : Totally Free LIC replacement
// *
// * file : stdio.h
// *
// ******************************************************************

#ifndef	__STDIO_H__
#define	__STDIO_H__	1


# ifdef __cplusplus
extern "C" {
# endif

#include	<openxdk.h>


// ******************************************************************
// Low level input/output functions
// ******************************************************************

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

typedef unsigned int size_t;

int _open( char *filename, int oflag, int permission );
int _read( int handle, void *buffer, unsigned int count );
int _write( int handle, void* buffer, unsigned int count );
int _close( int handle );
int _lseek( int handle, u32 offset, int base );

//int printf(const char *format, ...);
//int vsnprintf( char *buffer, unsigned int count, const char *format, va_list ap );
int sprintf(char*, const char *format, ...);
int snprintf (char *str, size_t size, const char *format, ...);



// ******************************************************************
// Standard C file input/output functions
// (Which are just wrappers for the low level ones)
// ******************************************************************

#define EOF (-1)

struct __stdio_file {
	int fd;
	int flags;
	unsigned int bs;      /* read: bytes in buffer */
	unsigned int bm;      /* position in buffer */
	unsigned int buflen;  /* length of buf */
	char *buf;
	unsigned char ungetbuf;
	char ungotten;
};

typedef struct __stdio_file FILE;


FILE *fopen(char *path, const char *mode);
size_t fread( void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite( void *ptr, size_t size, size_t nmemb, FILE *stream);
int fclose(FILE *stream);

int fprintf(FILE *f,const char *format,...);
int fseek(FILE *stream, long offset, int whence);
long ftell(FILE *stream) ;




# ifdef __cplusplus
}
# endif

#endif // __STDIO_H__



