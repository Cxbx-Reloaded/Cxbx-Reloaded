// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com


#include <core\kernel\exports\xboxkrnl.h> //#include <stdtypes.h>
#include "buffered_io.h"

namespace xbox
{

PBYTE GetSectorBuffered(
		PCDIO_READ This,
		xbox::dword_t SectorNumber)
{
	int		i, index;
	PBYTE	Ptr;

	// Have we got this baby in buffer ?
	for(i = 0;i < DISK_BUFFER;i++)
	{
		if (This->SectorList[i] == SectorNumber)
		{
			This->LockList[i]++;
			return(&This->DiskBuffer[i * SECTOR_SIZE]);
		}
	}

	// Nope, load the sector and store it in buffer
	for(i = 0;i < DISK_BUFFER;i++)
	{
		if (This->LockList[This->WriteIndex] == 0)
		{
			// Take an entry into the ring buffer but do not
			// write the sector number yet (this is needed
			// for re-entrancy purposes
			Ptr = &This->DiskBuffer[This->WriteIndex * SECTOR_SIZE];

			This->LockList[This->WriteIndex]++;
			This->SectorList[This->WriteIndex] = 0;

			index = This->WriteIndex;
			This->WriteIndex++;
			This->WriteIndex %= DISK_BUFFER;

			if (This->Sectors(This->Data, Ptr, SectorNumber, 1))
			{
				// OK, write the sector number and exit
				This->SectorList[index] = SectorNumber;
				return(Ptr);
			}

			// If something went bad free the slot
			This->SectorList[index] = 0;
			This->LockList[index]--;
			return nullptr;
		}

		This->WriteIndex++;
		This->WriteIndex %= DISK_BUFFER;
	}

	// We land here if all entries were locked, and that's BAD !
	return nullptr;
}
//------------------------------------------------------------------------------
void ReleaseBufferedSector(
		PCDIO_READ This,
		xbox::dword_t SectorNumber)
{
	int     i;

	// Find the sector in the lock list and decrease its usage count
	for(i = 0;i < DISK_BUFFER;i++)
	{
		if ((This->SectorList[i] == SectorNumber)&&(This->LockList[i]))
		{
			This->LockList[i]--;
			return;
		}
	}
}

} // namespace
