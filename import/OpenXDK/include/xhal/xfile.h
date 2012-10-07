// ******************************************************************
// * 
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : xfile.h
// *
// * note : XBox File Access
// *
// ******************************************************************
// * TODO: most of this belongs in a different .h
// ******************************************************************
#ifndef	XFILE_H
#define	XFILE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "xboxkrnl\xboxkrnl.h"

#define	INVALID_HANDLE_VALUE			((HANDLE)-1)
#define INVALID_FILE_SIZE				((HANDLE)-1)
#define DELETE							(0x00010000L)

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif 
#define ALIGN_SIZE			(0x10000)			// NT's allocation alignment size (64k)

// PAGE_ALIGN:
// Returns an address rounded down to the nearest page boundary.
// Differences from NT: None.
#define NT_SUCCESS(Status) ((NTSTATUS) (Status) >= 0)
#define PAGE_ALIGN(Va)				((PVOID)((ULONG_PTR)(Va) & ~(PAGE_SIZE - 1)))


// Initializes an OBJECT_ATTRIBUTES.  I added this because it's familiar to
// NT kernel mode programmers.  I just changed it to the XBOX style.
// Works as if it were this function:
#define InitializeObjectAttributes( p, n, a, r ) { \
	(p)->RootDirectory = r; \
	(p)->Attributes = a; \
	(p)->ObjectName = n; \
	}


//unsure where these really belong
#define	MAX_PATH						(2048)
#define ERROR_NEGATIVE_SEEK				(131)
#define	ERROR_INVALID_NAME				(123)
#define	ERROR_OPEN_FAILED				(110)
#define	ERROR_DRIVE_LOCKED				(108)
#define	ERROR_HANDLE_DISK_FULL			(39)
#define ERROR_ALREADY_EXISTS			(183)
#define	ERROR_INVALID_PARAMETER			(1)
#define	ERROR_FILE_NOT_FOUND			(2)
#define	ERROR_TOO_MANY_OPEN_FILES		(4)
#define	ERROR_INVALID_HANDLE			(6)
#define	ERROR_NOT_ENOUGH_MEMORY			(0x100000)
#define NO_ERROR						(0)

typedef	char*							LPCSTR;

// ==============================================
// end stuff that belong in some xboxwindows.h
// ==============================================

// these structs need to be here as they are used as parameters
typedef struct SSecurity_Attributes{
    u32		nLength;
    void*	lpSecurityDescriptor;
    u32		bInheritHandle;
} SSecurity_Attributes, *PSecurity_Attributes ;


typedef struct _OVERLAPPED {
    u32		Internal;
    u32		InternalHigh;
    u32		Offset;
    u32		OffsetHigh;
    HANDLE  hEvent;
} OVERLAPPED, *LPOVERLAPPED;


// Standard
#define FILE_FLAG_WRITE_THROUGH					(0x80000000)
#define FILE_FLAG_OVERLAPPED					(0x40000000)
#define FILE_FLAG_NO_BUFFERING					(0x20000000)
#define FILE_FLAG_RANDOM_ACCESS					(0x10000000)
#define FILE_FLAG_SEQUENTIAL_SCAN				(0x08000000)
#define FILE_FLAG_DELETE_ON_CLOSE				(0x04000000)
#define FILE_FLAG_BACKUP_SEMANTICS				(0x02000000)
#define FILE_FLAG_POSIX_SEMANTICS				(0x01000000)
#define FILE_FLAG_OPEN_REPARSE_POINT			(0x00200000)
#define FILE_FLAG_OPEN_NO_RECALL				(0x00100000)
												
#define CREATE_NEW								(0x00000001)
#define CREATE_ALWAYS							(0x00000002)
#define OPEN_EXISTING							(0x00000003)
#define OPEN_ALWAYS								(0x00000004)
#define TRUNCATE_EXISTING						(0x00000005)
												
												
#define GENERIC_READ							(0x80000000)
#define GENERIC_WRITE							(0x40000000)

#define	FILE_BEGIN						(0x00000000)
#define	FILE_CURRENT					(0x00000001)
#define	FILE_END						(0x00000002)


HANDLE CreateFile(char* lpFilename,
					u32		dwDesiredAccess, 
					u32		dwShareMode,
					SSecurity_Attributes	*lpSecurityAttributes,
					u32		dwCreationDisposition,
					u32		dwFlagsAndAttributes,
					HANDLE	hTemplateFile
					);
int	ReadFile(	HANDLE	hFile,					// file handle
				void*	lpBuffer,				// Dest buffer to put file
				u32		nNumberOfBytesToRead,	// read "n" bytes
				u32*	lpNumberOfBytesRead,	// pointer to a place to store bytes read
				LPOVERLAPPED lpOverlapped		// NULL unless overlap
			);

int WriteFile(	HANDLE			hFile, 
				PVOID			lpBuffer,
				u32				nNumberOfBytesToWrite,
				u32*			lpNumberOfBytesWritten,
				LPOVERLAPPED	lpOverlapped
				);

int	CloseHandle(HANDLE Handle);
int	GetFileSizeEx(HANDLE hFile, PLARGE_INTEGER lpFileSize);
int	SetFilePointerEx(HANDLE hFile, LARGE_INTEGER liDistanceToMove, PLARGE_INTEGER lpNewFilePointer, DWORD dwMoveMethod);



char* GetLastErrorMessage( void );
void SetLastError( u32 ErrorCode );


#ifdef	__cplusplus
};
#endif

#endif




