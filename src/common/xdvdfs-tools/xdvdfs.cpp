// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "xdvdfs.h"

#define UPPERCASE(a) (((a) > 0x60) ? ((a) & 0xDF) : (a))

#ifndef MIN
#define	MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef DIRECTORY_SEPARATOR
#ifdef _WIN32
#define DIRECTORY_SEPARATOR	'\\'
#else
#define DIRECTORY_SEPARATOR	'/'
#endif // _WIN32
#endif // DIRECTORY_SEPARATOR

#define TRANSFER_SIZE	32 // Read granularity (in sectors) = 32 * 2048 = 64 Kb

#define VOLUME_DESCRIPTOR_SECTOR_OFFSET 32

#if BYTE_ORDER == BIG_ENDIAN
//#define ENDIAN_SAFE16(a) ((((a)&0xFF00)>>8)|(((a)&0xFF)<<8))
#define ENDIAN_SAFE32(a) ((((a)&0xFF000000)>>24)|(((a)&0x00FF0000)>>8)|(((a)&0x0000FF00)<<8)|(((a)&0xFF)<<24))
#elif BYTE_ORDER == LITTLE_ENDIAN
//#define ENDIAN_SAFE16(a) (a)
#define ENDIAN_SAFE32(a) (a)
#else
#error "xdvdfs.c: Looks like endianness is not defined."
#endif

// Directory Entry
typedef struct {
	WORD		LeftSubTree;
	WORD		RightSubTree;
	DWORD		FileStartSector;
	DWORD		FileSize;
	BYTE		FileAttributes;
	BYTE		FilenameLength;
	BYTE		Filename[FILENAME_SIZE];
} XDVDFS_DIRECTORY_ENTRY, *PXDVDFS_DIRECTORY_ENTRY;

// XDVDFS init a session object
BOOL	XDVDFS_Mount(
			PXDVDFS_SESSION	Session,
			BOOL			(*ReadFunc)(PVOID, PVOID, DWORD, DWORD),
			PVOID			Data)
{
	Session->Read.Data = Data;
	Session->Read.Sectors = ReadFunc;

	XDVDFS_UnMount(Session);

	// scan sectors until the signature is found
	Session->FileSystemBaseSector = 0;
	while (1) {
		// Read in the volume descriptor
		if (!Session->Read.Sectors(
			Session->Read.Data,
			(PVOID)&Session->Root,
			// the actual root of the filesystem is 32 sectors back
			Session->FileSystemBaseSector + VOLUME_DESCRIPTOR_SECTOR_OFFSET,
			1))
			return FALSE;

		// Check signatures
		if ((memcmp(Session->Root.Signature1, XDVDFS_Signature, SIGNATURE_SIZE) == 0) &&
			(memcmp(Session->Root.Signature2, XDVDFS_Signature, SIGNATURE_SIZE) == 0)) {
			/* From https://github.com/multimediamike/xbfuse/blob/master/src/xdvdfs.c#L258 :
			// process the volume descriptor
			root_directory_sector = LE_32(&sector_buffer[0x14]);
			root_directory_size = LE_32(&sector_buffer[0x18]);
			memcpy(timestamp, &sector_buffer[0x1C], 8);

			fprintf(stderr, "root @ sector 0x%X, 0x%X bytes; time = %02X %02X %02X %02X %02X %02X %02X %02X\n",
				root_directory_sector, root_directory_size,
				timestamp[0], timestamp[1], timestamp[2], timestamp[3],
				timestamp[4], timestamp[5], timestamp[6], timestamp[7]);
			*/
			break;
		}

		Session->FileSystemBaseSector++;
	}

	return TRUE;
}

// XDVDFS deinit a session object
BOOL	XDVDFS_UnMount(
			PXDVDFS_SESSION	Session)
{
	// Reset ring buffer
	memset(Session->Read.SectorList, 0, sizeof(DWORD) * DISK_BUFFER);
	memset(Session->Read.LockList, 0, sizeof(DWORD) * DISK_BUFFER);
	Session->Read.WriteIndex = 0;
	// Invalidate all open files & search structures
	Session->Magic++;
	return TRUE;
}

// Initialize a search record with root dir
// Note: Can return XDVDFS_NO_MORE_FILES if the image is empty
DWORD	XDVDFS_GetRootDir(
			PXDVDFS_SESSION	Session,
			PSEARCH_RECORD	SearchRecord)
{
	if (!Session->Root.RootDirectorySize)
		return XDVDFS_NO_MORE_FILES;

	SearchRecord->Magic = Session->Magic;
	// Cxbx addition : Correct all sector-numbers with file system base sector :
	SearchRecord->SearchStartSector = ENDIAN_SAFE32(Session->Root.RootDirectory) + Session->FileSystemBaseSector;
	SearchRecord->DirectorySize = ENDIAN_SAFE32(Session->Root.RootDirectorySize);
	SearchRecord->Position = 0;
	return	XDVDFS_NO_ERROR;
}

// Enumerate files
DWORD	XDVDFS_EnumFiles(
			PXDVDFS_SESSION	Session,
			PSEARCH_RECORD	SearchRecord)
{
	PXDVDFS_DIRECTORY_ENTRY	Entry;
	DWORD					SectorNumber, Position, i;
	PBYTE					Ptr;

enum_retry:
	// Check structure validity
	if (SearchRecord->Magic != Session->Magic)
		return XDVDFS_EXPIRED_SESSION;

	// Check if we reached the end of the directory
	if (SearchRecord->Position >= SearchRecord->DirectorySize)
		return XDVDFS_NO_MORE_FILES;

	// Get current position in sector, remainder
	SectorNumber = SearchRecord->Position / SECTOR_SIZE;
	Position = SearchRecord->Position % SECTOR_SIZE;
	// Get the current sector in buffer and lock it
	Ptr = GetSectorBuffered(&Session->Read,
							SectorNumber + SearchRecord->SearchStartSector);
	if (!Ptr)
		return XDVDFS_DISK_ERROR;

	// If we're at the begining...
	if (!SearchRecord->Position)
	{
		// ...bufferize the whole dir.
		for(i = 1;i < MIN(DISK_BUFFER - 1, SearchRecord->DirectorySize / SECTOR_SIZE);i++)
		{
			GetSectorBuffered(&Session->Read, i + SearchRecord->SearchStartSector);
			ReleaseBufferedSector(&Session->Read, i + SearchRecord->SearchStartSector);
		}
	}

	Entry = (PXDVDFS_DIRECTORY_ENTRY)&Ptr[Position];
	// If Entry->FileStartSector = 0xFFFFFFFF or Position > 2040, we reached the last
	// entry of the sector
	if ((Position > (SECTOR_SIZE - offsetof(XDVDFS_DIRECTORY_ENTRY, FileStartSector)))||(Entry->FileStartSector == 0xFFFFFFFF))
	{
		// Let's get the next one
		ReleaseBufferedSector(&Session->Read,
								SectorNumber + SearchRecord->SearchStartSector);
		SearchRecord->Position = (SearchRecord->Position & ~(SECTOR_SIZE - 1)) + SECTOR_SIZE;
		goto enum_retry;
	}

	// Copy the filename (filenames up to 255 chars)
	memcpy(SearchRecord->CurrentFilename, Entry->Filename, Entry->FilenameLength);
	SearchRecord->CurrentFilename[Entry->FilenameLength] = 0;
	// Copy file parameters in the search_rec
	SearchRecord->CurrentFileAttributes = Entry->FileAttributes;
	SearchRecord->CurrentFileSize = ENDIAN_SAFE32(Entry->FileSize);
	// Cxbx addition : Correct all sector-numbers with file system base sector :
	SearchRecord->CurrentFileStartSector = ENDIAN_SAFE32(Entry->FileStartSector) + Session->FileSystemBaseSector;
	SearchRecord->CurrentFileEndSector = SearchRecord->CurrentFileStartSector
								 + (SearchRecord->CurrentFileSize / SECTOR_SIZE);
	if (SearchRecord->CurrentFileSize % SECTOR_SIZE)
		SearchRecord->CurrentFileEndSector++;

	// Advance to next entry
	SearchRecord->Position += Entry->FilenameLength + offsetof(XDVDFS_DIRECTORY_ENTRY, Filename);
	if (SearchRecord->Position & 3)
	{
		SearchRecord->Position &= ~3;
		SearchRecord->Position += 4;
	}

	// Free the buffer
	ReleaseBufferedSector(&Session->Read,
							SectorNumber + SearchRecord->SearchStartSector);
	return XDVDFS_NO_ERROR;
}

DWORD	XDVDFS_GetFileInfo(
			PXDVDFS_SESSION	Session,
			LPSTR 			Filename,
			PSEARCH_RECORD	SearchRecord)
{
	DWORD   Length, i, ReturnCode;

	// To begin, we will enter the root directory
	XDVDFS_GetRootDir(Session, SearchRecord);
	SearchRecord->CurrentFileStartSector = SearchRecord->SearchStartSector;
	SearchRecord->CurrentFileSize = SearchRecord->DirectorySize;
	SearchRecord->CurrentFileAttributes = XDVDFS_ATTRIBUTE_DIRECTORY;
	SearchRecord->CurrentFilename[0] = 0;
	// Skip leading backslash if present
	if (*Filename == DIRECTORY_SEPARATOR)
		Filename++;
	
	while(*Filename)
	{
		// Skip backslashes
		while(*Filename == DIRECTORY_SEPARATOR)
			Filename++;

		// If previously matched name is not a dir, fail
		if (!(SearchRecord->CurrentFileAttributes & XDVDFS_ATTRIBUTE_DIRECTORY))
			return XDVDFS_FILE_NOT_FOUND;

		// Enter that directory
		Length = 0;
		SearchRecord->SearchStartSector = SearchRecord->CurrentFileStartSector;
		SearchRecord->DirectorySize = SearchRecord->CurrentFileSize;
		SearchRecord->Position = 0;
		// Browse the contents of the dir
		while( (ReturnCode = XDVDFS_EnumFiles(Session, SearchRecord)) ==
			XDVDFS_NO_ERROR)
		{
			// Dxbx addition : Stop at first file if no further path is given
			if (*Filename == 0)
				break;

			// Calculate length of the filename
			Length = 0;
			while( (SearchRecord->CurrentFilename[Length])&&
				(SearchRecord->CurrentFilename[Length] != DIRECTORY_SEPARATOR) )
					Length++;

			// Match the filename against the one given
			for(i = 0;i < Length;i++)
			{
				if ((Filename[i] == DIRECTORY_SEPARATOR)||
					(!Filename[i])||
					(UPPERCASE(Filename[i]) !=
					UPPERCASE(SearchRecord->CurrentFilename[i])))
						break;
			}

			// If it matched, exit
			if ((i == Length)&&((Filename[i] == 0)
				||(Filename[i] == DIRECTORY_SEPARATOR)))
				break;
		}

		// If we reached the end of the dir without matching, fail
		if (ReturnCode == XDVDFS_NO_MORE_FILES)
			return XDVDFS_FILE_NOT_FOUND;

		// If any other error occured, fail
		if (ReturnCode != XDVDFS_NO_ERROR)
			return ReturnCode;

		// Match next part of the given filename
		Filename += Length;
	}

	// If we land here, everything matched and the SEARCH_RECORD structure is
	// filled.
	return XDVDFS_NO_ERROR;
}

// Initialize a search record given a path
DWORD	XDVDFS_OpenFolder(
			PXDVDFS_SESSION	Session,
			LPSTR			Path,
			PSEARCH_RECORD	SearchRecord)
{
	DWORD   ReturnCode;

	// Find a file matching the given path
	ReturnCode = XDVDFS_GetFileInfo(Session, Path, SearchRecord);
	// If an error occured, fail
	if (ReturnCode != XDVDFS_NO_ERROR)
		return ReturnCode;

	// If the returned file is not a dir, fail
	if (!(SearchRecord->CurrentFileAttributes & XDVDFS_ATTRIBUTE_DIRECTORY))
		return XDVDFS_FILE_NOT_FOUND;

	// Copy folder info into the SEARCH_RECORD structure
	SearchRecord->SearchStartSector = SearchRecord->CurrentFileStartSector;
	SearchRecord->DirectorySize = SearchRecord->CurrentFileSize;
	SearchRecord->Position = 0;
	return XDVDFS_NO_ERROR;
}

// Open a file
DWORD	XDVDFS_OpenFile(
			PXDVDFS_SESSION	Session,
			LPSTR			Filename,
			PFILE_RECORD	FileRecord)
{
	SEARCH_RECORD	SearchRecord;
	DWORD			ReturnCode;

	// Find a file matching the given path
	ReturnCode = XDVDFS_GetFileInfo(Session, Filename, &SearchRecord);
	// If an error occured, fail
	if (ReturnCode != XDVDFS_NO_ERROR)
		return ReturnCode;

	// If the returned file is a dir, fail
	if (SearchRecord.CurrentFileAttributes & XDVDFS_ATTRIBUTE_DIRECTORY)
		return XDVDFS_FILE_NOT_FOUND;

	// Copy file info into the FILE_RECORD structure
	FileRecord->Magic = SearchRecord.Magic;
	FileRecord->PartialSector = 0;
	FileRecord->FileStartSector = SearchRecord.CurrentFileStartSector;
	FileRecord->FileSize = SearchRecord.CurrentFileSize;
	FileRecord->CurrentPosition = 0;
	return XDVDFS_NO_ERROR;
}

// Open a file pointed by a search rec
DWORD	XDVDFS_OpenFileEx(
			PXDVDFS_SESSION	Session,
			PSEARCH_RECORD	SearchRecord,
			PFILE_RECORD	FileRecord)
{
	// Check structure validity
	if (SearchRecord->Magic != Session->Magic)
		return XDVDFS_EXPIRED_SESSION;

	// Do not accept a directory
	if (SearchRecord->CurrentFileAttributes & XDVDFS_ATTRIBUTE_DIRECTORY)
		return XDVDFS_FILE_NOT_FOUND;

	// Copy file info into the FILE_RECORD structure
	FileRecord->Magic = SearchRecord->Magic;
	FileRecord->PartialSector = 0;
	FileRecord->FileStartSector = SearchRecord->CurrentFileStartSector;
	FileRecord->FileSize = SearchRecord->CurrentFileSize;
	FileRecord->CurrentPosition = 0;
	return XDVDFS_NO_ERROR;
}

// Read a file
DWORD	XDVDFS_FileRead(
			PXDVDFS_SESSION	Session,
			PFILE_RECORD	FileRecord,
			PVOID			OutBuffer,
			DWORD			Size)
{
	DWORD   CurrentSector, Position, PartialRead, Readed, i;
	PBYTE	Buffer = (PBYTE)OutBuffer;

	Readed = 0;
	// Check structure validity
	if (FileRecord->Magic != Session->Magic)
		return Readed;

	// Limit read size
	if ((FileRecord->CurrentPosition + Size) > FileRecord->FileSize)
		Size = FileRecord->FileSize - FileRecord->CurrentPosition;

	// Dxbx addition : Stop if there's nothing to read
	if (!Size)
		return Readed;

	// Process partial sector read before
	Position = FileRecord->CurrentPosition % SECTOR_SIZE;
	if (Position)
	{
		CurrentSector = (FileRecord->CurrentPosition / SECTOR_SIZE) +
			FileRecord->FileStartSector;
		PartialRead = MIN(Size, SECTOR_SIZE - Position);
		if (FileRecord->PartialSector != CurrentSector)
		{
			if (!Session->Read.Sectors(
								Session->Read.Data,
								FileRecord->PartialData,
								CurrentSector,
								1))
				return 0;

			FileRecord->PartialSector = CurrentSector;
		}

		for(i = 0;i < PartialRead;i++)
			*Buffer++ = FileRecord->PartialData[Position++];

		Size -= PartialRead;
		Readed += PartialRead;
		FileRecord->CurrentPosition += PartialRead;
		if (!Size)
			return Readed;
	}

	// Process entire sector read
	PartialRead = Size / SECTOR_SIZE;
	if (PartialRead)
	{
		CurrentSector = (FileRecord->CurrentPosition / SECTOR_SIZE) + FileRecord->FileStartSector;
		i = PartialRead;
		while(i > 0)
		{
			Position = MIN(i, TRANSFER_SIZE);
			if (!Session->Read.Sectors(
					Session->Read.Data,
					Buffer,
					CurrentSector,
					Position))
				return Readed;

			Buffer += Position * SECTOR_SIZE;
			CurrentSector += Position;
			i -= Position;
		}

		PartialRead *= SECTOR_SIZE;
		Size -= PartialRead;
		Readed += PartialRead;
		FileRecord->CurrentPosition += PartialRead;
		if (!Size)
			return Readed;
	}

	// Process partial sector read after
	PartialRead = Size;
	CurrentSector = (FileRecord->CurrentPosition / SECTOR_SIZE)
						+ FileRecord->FileStartSector;
	if (FileRecord->PartialSector != CurrentSector)
	{
		if (!Session->Read.Sectors(
							Session->Read.Data,
							FileRecord->PartialData,
							CurrentSector,
							1))
			return Readed;

		FileRecord->PartialSector = CurrentSector;
	}

	for(i = 0;i < PartialRead;i++)
		*Buffer++ = FileRecord->PartialData[i];

	Readed += PartialRead;
	FileRecord->CurrentPosition += PartialRead;
	return Readed;
}

DWORD	XDVDFS_FileClose(
			PXDVDFS_SESSION	Session,
			PFILE_RECORD 	FileRecord)
{
	// Check structure validity
	if (FileRecord->Magic != Session->Magic)
		return XDVDFS_EXPIRED_SESSION;

	// Invalidate the structure
	FileRecord->Magic--;
	return XDVDFS_NO_ERROR;
}

DWORD	XDVDFS_FileSeek(
			PXDVDFS_SESSION	Session,
			PFILE_RECORD	FileRecord,
			int				Delta,
			DWORD			SeekMode)
{
	// Check structure validity
	if (FileRecord->Magic != Session->Magic)
		return XDVDFS_EXPIRED_SESSION;

	// Change file pointer
	switch(SeekMode)
	{
	case    SM_BEGIN:
		FileRecord->CurrentPosition = Delta;
		break;
	case    SM_CURRENT:
		FileRecord->CurrentPosition += Delta;
		break;
	case    SM_END:
		FileRecord->CurrentPosition = FileRecord->FileSize - Delta;
		break;
	default:
		return XDVDFS_INVALID_PARAMETER;
	}

	// Check the file pointer for limits
	if (FileRecord->CurrentPosition > FileRecord->FileSize)
	{
		if (Delta < 0)
			FileRecord->CurrentPosition = 0;
		else
			FileRecord->CurrentPosition = FileRecord->FileSize;
	}

	return XDVDFS_NO_ERROR;
}
