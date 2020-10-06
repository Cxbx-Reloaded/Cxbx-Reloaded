// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef __XDVDFS_H__
#define __XDVDFS_H__

#include <core\kernel\exports\xboxkrnl.h> //#include <stdtypes.h>

#include "buffered_io.h"

using namespace xbox;

CONST char_xt *XDVDFS_Signature = "MICROSOFT*XBOX*MEDIA";

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
	xbox::byte_xt		Signature1[SIGNATURE_SIZE];
	xbox::dword_xt		RootDirectory;
	xbox::dword_xt		RootDirectorySize;
	FILETIME	ImageCreationTime;
	xbox::byte_xt		Unused[1992];
	xbox::byte_xt		Signature2[SIGNATURE_SIZE];
} XDVDFS_VOLUME_DESCRIPTOR, *PXDVDFS_VOLUME_DESCRIPTOR;

// XDVDFS session
typedef struct {
	// Start sector of current session
	xbox::dword_xt						FileSystemBaseSector;

	// Volume Descriptor of the current session
	XDVDFS_VOLUME_DESCRIPTOR	Root;

	// Our little interface for reading sectors
	CDIO_READ					Read;

	// The dword below is incremented when the filesystem is unmounted
	// automatically invalidating all open files and search records
	xbox::dword_xt						Magic;
} XDVDFS_SESSION, *PXDVDFS_SESSION;

// File Record
typedef struct {
	xbox::dword_xt	Magic;
	xbox::byte_xt	PartialData[SECTOR_SIZE];
	xbox::dword_xt	PartialSector;
	xbox::dword_xt	FileStartSector;
	xbox::dword_xt	FileSize;
	xbox::dword_xt	CurrentPosition;
} FILE_RECORD, *PFILE_RECORD;

// Search Record
typedef struct {
	xbox::dword_xt	Magic;
	xbox::dword_xt	SearchStartSector;
	xbox::dword_xt	DirectorySize;
	xbox::dword_xt	Position;
	xbox::byte_xt	CurrentFilename[FILENAME_SIZE];
	xbox::dword_xt	CurrentFileAttributes;
	xbox::dword_xt	CurrentFileSize;
	xbox::dword_xt	CurrentFileStartSector;
	xbox::dword_xt	CurrentFileEndSector;
} SEARCH_RECORD, *PSEARCH_RECORD;

//-- Exported Functions -------------------------------------------------------

// XDVDFS init a session object
extern BOOL		XDVDFS_Mount(
					PXDVDFS_SESSION	Session,
					BOOL			(*ReadFunc)(PVOID, PVOID, xbox::dword_xt, xbox::dword_xt),
					PVOID			Data);

// XDVDFS deinit a session object
extern BOOL		XDVDFS_UnMount(
					PXDVDFS_SESSION	Session);

// Initialize a search record with root dir
// Note: Can return XDVDFS_NO_MORE_FILES if the image is empty
extern xbox::dword_xt	XDVDFS_GetRootDir(
					PXDVDFS_SESSION	Session,
					PSEARCH_RECORD	SearchRecord);

// Enumerate files
extern xbox::dword_xt	XDVDFS_EnumFiles(
					PXDVDFS_SESSION	Session,
					PSEARCH_RECORD	SearchRecord);

// Find a file given its path
extern xbox::dword_xt	XDVDFS_GetFileInfo(
					PXDVDFS_SESSION	Session,
					LPSTR 			Filename,
					PSEARCH_RECORD	SearchRecord);

// Initialize a search record given a path
extern xbox::dword_xt	XDVDFS_OpenFolder(
					PXDVDFS_SESSION	Session,
					LPSTR			Path,
					PSEARCH_RECORD	SearchRecord);

// Open a file
extern xbox::dword_xt	XDVDFS_OpenFile(
					PXDVDFS_SESSION	Session,
					LPSTR			Filename,
					PFILE_RECORD	FileRecord);

// Open a file pointed by a search rec
extern xbox::dword_xt	XDVDFS_OpenFileEx(
					PXDVDFS_SESSION	Session,
					PSEARCH_RECORD	SearchRecord,
					PFILE_RECORD	FileRecord);

// Read a file
extern xbox::dword_xt	XDVDFS_FileRead(
					PXDVDFS_SESSION	Session,
					PFILE_RECORD	FileRecord,
					PVOID			Buffer,
					xbox::dword_xt			Size);

// Close file
extern xbox::dword_xt	XDVDFS_FileClose(
					PXDVDFS_SESSION	Session,
					PFILE_RECORD 	FileRecord);

// File seek
extern xbox::dword_xt	XDVDFS_FileSeek(
					PXDVDFS_SESSION	Session,
					PFILE_RECORD	FileRecord,
					int				Delta,
					xbox::dword_xt			SeekMode);

#endif // __XDVDFS_H__
