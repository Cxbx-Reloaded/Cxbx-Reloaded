// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuKrnlXe.cpp
// *
// *  This file is part of the Cxbx project.
// *
// *  Cxbx and Cxbe are free software; you can redistribute them
// *  and/or modify them under the terms of the GNU General Public
// *  License as published by the Free Software Foundation; either
// *  version 2 of the license, or (at your option) any later version.
// *
// *  This program is distributed in the hope that it will be useful,
// *  but WITHOUT ANY WARRANTY; without even the implied warranty of
// *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// *  GNU General Public License for more details.
// *
// *  You should have recieved a copy of the GNU General Public License
// *  along with this program; see the file COPYING.
// *  If not, write to the Free Software Foundation, Inc.,
// *  59 Temple Place - Suite 330, Bostom, MA 02111-1307, USA.
// *
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *  (c) 2016 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#define _XBOXKRNL_DEFEXTRN_

#define LOG_PREFIX "KRNL"

// prevent name collisions
namespace xboxkrnl
{
#include <xboxkrnl/xboxkrnl.h> // For XeImageFileName, etc.
};

#include "CxbxKrnl.h" // For CxbxKrnl_Xbe
#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"
#include "Emu.h" // For EmuWarning()
#include "VMManager.h"

// ******************************************************************
// * 0x0146 - XeImageFileName
// ******************************************************************
XBSYSAPI EXPORTNUM(326) xboxkrnl::OBJECT_STRING xboxkrnl::XeImageFileName =
// XeImageFileName.Buffer points to path of XBE
//
// Format is like this: \Device\Harddisk0\Partition1\bla.xbe
// Size of XeImageFileName.Buffer is stored in XeImageFileName.Length
{

};

// ******************************************************************
// * 0x0147 - XeLoadSection()
// ******************************************************************
// XeLoadSection:
// Adds one to the reference count of the specified section and loads if the
// count is now above zero.
//
// New to the XBOX.
XBSYSAPI EXPORTNUM(327) xboxkrnl::NTSTATUS NTAPI xboxkrnl::XeLoadSection
(
	IN PXBEIMAGE_SECTION Section
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Section)
		LOG_FUNC_END;

	NTSTATUS ret = STATUS_SUCCESS;

	void* sectionData = CxbxKrnl_Xbe->FindSection((char*)std::string(Section->SectionName, 9).c_str());
	if (sectionData != nullptr) {
		// If the reference count was zero, load the section
		if (Section->SectionReferenceCount == 0) {
			// Clear the memory the section requires
			memset(Section->VirtualAddress, 0, Section->VirtualSize);
			// Copy the section data
			memcpy(Section->VirtualAddress, sectionData, Section->FileSize);

			// ergo720: I can't just +/- PAGE_SIZE the VirtualAddress and the VirtualSize of a section because some titles have
			// sections less than PAGE_SIZE, which will cause again an overlap with the next section since both will have the
			// same aligned starting address. 
			// Test case: Dead or Alive 3, section XGRPH has a size of 764 bytes
			// XGRPH										DSOUND
			// 1F18A0 + 2FC -> aligned_start = 1F1000		1F1BA0 -> aligned_start = 1F1000 <- collision

			// The following just increases the amount of physical/virtual memory consumed but doesn't actually allocate anything
			// inside the manager. This is done so that, because we now have fewer physical allocations, we also get more free contiguous
			// space that can be used by MmAllocateContiguousMemoryEx, which is problematic to map in the case of fragmentation. Also
			// note that the manager physical allocation routines check the free memory left before attempting a new allocation and bail out
			// immediately if not enough is available, this prevents exceeding the max memory on the Xbox

			VAddr BaseAddress = (VAddr)Section->VirtualAddress;
			VAddr EndingAddress = (VAddr)Section->VirtualAddress + Section->VirtualSize;

			if ((*Section->TailReferenceCount) != 0)
			{
				EndingAddress &= ~PAGE_MASK;
			}

			if ((*Section->HeadReferenceCount) != 0)
			{
				BaseAddress = (BaseAddress + PAGE_SIZE) & ~PAGE_MASK;
			}

			if (EndingAddress > BaseAddress)
			{
				size_t RegionSize = EndingAddress - BaseAddress;
				ret = XbAllocateVirtualMemoryStub(&BaseAddress, 0, &RegionSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			}

			// Increment the head/tail page reference counters
			(*Section->HeadReferenceCount)++;
			(*Section->TailReferenceCount)++;
		}

		// Increment the reference count
		Section->SectionReferenceCount++;
	}
	
	RETURN(ret);
}

// ******************************************************************
// * 0x0148 - XeUnloadSection()
// ******************************************************************
// Subtracts one from the reference count of the specified section and unloads
// if the count is now zero.
//
// New to the XBOX.
XBSYSAPI EXPORTNUM(328) xboxkrnl::NTSTATUS NTAPI xboxkrnl::XeUnloadSection
(
	IN PXBEIMAGE_SECTION Section
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Section)
		LOG_FUNC_END;

	NTSTATUS ret = STATUS_INVALID_PARAMETER;

	// If the section was loaded, process it
	if (Section->SectionReferenceCount > 0) {
		// Decrement the reference count
		Section->SectionReferenceCount -= 1;

		// Free the section and the physical memory in use if necessary
		if (Section->SectionReferenceCount == 0) {
			memset(Section->VirtualAddress, 0, Section->VirtualSize);

			// REMARK: the following can be tested with Broken Sword - The Sleeping Dragon, RalliSport Challenge, ...

			VAddr BaseAddress = (VAddr)Section->VirtualAddress;
			VAddr EndingAddress = (VAddr)Section->VirtualAddress + Section->VirtualSize;

			// Decrement the head/tail page reference counters
			(*Section->HeadReferenceCount)--;
			(*Section->TailReferenceCount)--;

			if ((*Section->TailReferenceCount) != 0)
			{
				EndingAddress &= ~PAGE_MASK;
			}

			if ((*Section->HeadReferenceCount) != 0)
			{
				BaseAddress = (BaseAddress + PAGE_SIZE) & ~PAGE_MASK;
			}

			if (EndingAddress > BaseAddress)
			{
				size_t RegionSize = EndingAddress - BaseAddress;
				XbFreeVirtualMemoryStub(&BaseAddress, &RegionSize, MEM_DECOMMIT);
			}
		}

		ret = STATUS_SUCCESS;
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x0163 - XePublicKeyData
// ******************************************************************
// TODO : What should we initialize this to?
XBSYSAPI EXPORTNUM(355) xboxkrnl::DWORD xboxkrnl::XePublicKeyData = 0;

