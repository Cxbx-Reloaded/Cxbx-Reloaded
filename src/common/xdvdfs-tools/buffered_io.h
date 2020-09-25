// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef __BUFFERED_IO_H__
#define __BUFFERED_IO_H__

#ifdef __cplusplus
extern "C" {
#endif

namespace xbox
{

#define BOOL xbox::boolean_t
#define LPSTR LPCH

// Determines how large a sector is
#define SECTOR_SIZE 2048

// Determines how many sectors are buffered in each instance of CDIO_READ
#define DISK_BUFFER		64

typedef struct {
	xbox::dword_t	SectorList[DISK_BUFFER];		// Ring buffer for buffered disk i/o
	xbox::dword_t	LockList[DISK_BUFFER];			// Lock for each buffered sector
	xbox::byte_t	DiskBuffer[SECTOR_SIZE * DISK_BUFFER];	// Storage room for buffered sectors
	xbox::dword_t	WriteIndex;						// Write pointer

	// Pointer to arbitrary data passed at init
	// (usually a file or device handle)
	PVOID	Data;

	BOOL	(*Sectors)(						// Routine to get sectors
				PVOID	Data,				//  Pointer to arbitrary data
				PVOID	Buffer,				//  Buffer to fill
				xbox::dword_t	StartSector,		//  Start sector
				xbox::dword_t	ReadSize);			//  Number of sectors to read

} CDIO_READ, *PCDIO_READ;

// Get a sector from buffer and lock it
extern PBYTE GetSectorBuffered(
				PCDIO_READ This,
				xbox::dword_t SectorNumber);

// Release a locked buffer
extern void ReleaseBufferedSector(
				PCDIO_READ This,
				xbox::dword_t SectorNumber);

};

#ifdef __cplusplus
}
#endif

#endif // __BUFFERED_IO_H__
