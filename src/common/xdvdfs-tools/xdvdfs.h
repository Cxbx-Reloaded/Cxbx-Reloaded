// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef __XDVDFS_H__
#define __XDVDFS_H__

#include <xboxkrnl/xboxkrnl.h> //#include <stdtypes.h>

#include "buffered_io.h"

using namespace xbox;

CONST char_t *XDVDFS_Signature = "MICROSOFT*XBOX*MEDIA";

//-- Defines ------------------------------------------------------------------

#define SIGNATURE_SIZE sizeof(XDVDFS_Signature)

#define FILENAME_SIZE 256

// Attributes
#define XDVDFS_ATTRIBUTE_READONLY	0x01
#define XDVDFS_ATTRIBUTE_HIDDEN		0x02
#define XDVDFS_ATTRIBUTE_SYSTEM		0x04
#define XDVDFS_ATTRIBUTE_DIRECTORY	0x10
#define XDVDFS_ATTRIBUTE_ARCHIVE	0x20
#define XDVDFS_ATTRIBUTE_NORMAL		0x80

// Error Codes
#define XDVDFS_NO_ERROR				0
#define XDVDFS_EXPIRED_SESSION		1
#define XDVDFS_NO_MORE_FILES		2
#define XDVDFS_DISK_ERROR			3
#define XDVDFS_FILE_NOT_FOUND		4
#define XDVDFS_INVALID_PARAMETER	5

// Seek Modes
#define SM_BEGIN					0
#define SM_CURRENT					1
#define SM_END						2

//-- Type Definitions ---------------------------------------------------------

// XDVDFS Volume descriptor
typedef struct {
	xbox::byte_t		Signature1[SIGNATURE_SIZE];
	DWORD		RootDirectory;
	DWORD		RootDirectorySize;
	FILETIME	ImageCreationTime;
	xbox::byte_t		Unused[1992];
	xbox::byte_t		Signature2[SIGNATURE_SIZE];
} XDVDFS_VOLUME_DESCRIPTOR, *PXDVDFS_VOLUME_DESCRIPTOR;

// XDVDFS session
typedef struct {
	// Start sector of current session
	DWORD						FileSystemBaseSector;

	// Volume Descriptor of the current session
	XDVDFS_VOLUME_DESCRIPTOR	Root;

	// Our little interface for reading sectors
	CDIO_READ					Read;

	// The dword below is incremented when the filesystem is unmounted
	// automatically invalidating all open files and search records
	DWORD						Magic;
} XDVDFS_SESSION, *PXDVDFS_SESSION;

// File Record
typedef struct {
	DWORD	Magic;
	xbox::byte_t	PartialData[SECTOR_SIZE];
	DWORD	PartialSector;
	DWORD	FileStartSector;
	DWORD	FileSize;
	DWORD	CurrentPosition;
} FILE_RECORD, *PFILE_RECORD;

// Search Record
typedef struct {
	DWORD	Magic;
	DWORD	SearchStartSector;
	DWORD	DirectorySize;
	DWORD	Position;
	xbox::byte_t	CurrentFilename[FILENAME_SIZE];
	DWORD	CurrentFileAttributes;
	DWORD	CurrentFileSize;
	DWORD	CurrentFileStartSector;
	DWORD	CurrentFileEndSector;
} SEARCH_RECORD, *PSEARCH_RECORD;

//-- Exported Functions -------------------------------------------------------

// XDVDFS init a session object
extern BOOL		XDVDFS_Mount(
					PXDVDFS_SESSION	Session,
					BOOL			(*ReadFunc)(PVOID, PVOID, DWORD, DWORD),
					PVOID			Data);

// XDVDFS deinit a session object
extern BOOL		XDVDFS_UnMount(
					PXDVDFS_SESSION	Session);

// Initialize a search record with root dir
// Note: Can return XDVDFS_NO_MORE_FILES if the image is empty
extern DWORD	XDVDFS_GetRootDir(
					PXDVDFS_SESSION	Session,
					PSEARCH_RECORD	SearchRecord);

// Enumerate files
extern DWORD	XDVDFS_EnumFiles(
					PXDVDFS_SESSION	Session,
					PSEARCH_RECORD	SearchRecord);

// Find a file given its path
extern DWORD	XDVDFS_GetFileInfo(
					PXDVDFS_SESSION	Session,
					LPSTR 			Filename,
					PSEARCH_RECORD	SearchRecord);

// Initialize a search record given a path
extern DWORD	XDVDFS_OpenFolder(
					PXDVDFS_SESSION	Session,
					LPSTR			Path,
					PSEARCH_RECORD	SearchRecord);

// Open a file
extern DWORD	XDVDFS_OpenFile(
					PXDVDFS_SESSION	Session,
					LPSTR			Filename,
					PFILE_RECORD	FileRecord);

// Open a file pointed by a search rec
extern DWORD	XDVDFS_OpenFileEx(
					PXDVDFS_SESSION	Session,
					PSEARCH_RECORD	SearchRecord,
					PFILE_RECORD	FileRecord);

// Read a file
extern DWORD	XDVDFS_FileRead(
					PXDVDFS_SESSION	Session,
					PFILE_RECORD	FileRecord,
					PVOID			Buffer,
					DWORD			Size);

// Close file
extern DWORD	XDVDFS_FileClose(
					PXDVDFS_SESSION	Session,
					PFILE_RECORD 	FileRecord);

// File seek
extern DWORD	XDVDFS_FileSeek(
					PXDVDFS_SESSION	Session,
					PFILE_RECORD	FileRecord,
					int				Delta,
					DWORD			SeekMode);

#endif // __XDVDFS_H__
