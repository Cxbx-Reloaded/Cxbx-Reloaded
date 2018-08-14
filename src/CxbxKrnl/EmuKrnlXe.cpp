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

	void* sectionData = CxbxKrnl_Xbe->FindSection((char*)std::string(Section->SectionName, 9).c_str(), Section->FileAddress);
	if (sectionData != nullptr) {
		// If the reference count was zero, load the section
		if (Section->SectionReferenceCount == 0) {
			// Clear the memory the section requires
			memset(Section->VirtualAddress, 0, Section->VirtualSize);
			// Copy the section data
			memcpy(Section->VirtualAddress, sectionData, Section->FileSize);

			// REMARK: Some titles have sections less than PAGE_SIZE, which will cause an overlap with the next section
			// since both will have the same aligned starting address.
			// Test case: Dead or Alive 3, section XGRPH has a size of 764 bytes
			// XGRPH										DSOUND
			// 1F18A0 + 2FC -> aligned_start = 1F1000		1F1BA0 -> aligned_start = 1F1000 <- collision

			VAddr BaseAddress = (VAddr)Section->VirtualAddress;
			size_t SectionSize = (VAddr)Section->VirtualSize;

			ret = g_VMManager.XbAllocateVirtualMemory(&BaseAddress, 0, &SectionSize, XBOX_MEM_COMMIT, XBOX_PAGE_EXECUTE_READWRITE);

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
				EndingAddress = ROUND_DOWN_4K(EndingAddress);
			}

			if ((*Section->HeadReferenceCount) != 0)
			{
				BaseAddress = ROUND_UP_4K(BaseAddress);
			}

			if (EndingAddress > BaseAddress)
			{
				size_t RegionSize = EndingAddress - BaseAddress;
				g_VMManager.XbFreeVirtualMemory(&BaseAddress, &RegionSize, XBOX_MEM_DECOMMIT);
			}
		}

		ret = STATUS_SUCCESS;
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x0163 - XePublicKeyData
// ******************************************************************
// We are allowed to use the real RSA public key since it cannot be used to sign data, only verify it -> it's not secret/private
XBSYSAPI EXPORTNUM(355) xboxkrnl::UCHAR xboxkrnl::XePublicKeyData[284] = {
	0x52,0x53,0x41,0x31, 0x08,0x01,0x00,0x00, 0x00,0x08,0x00,0x00, 0xff,0x00,0x00,0x00,
	0x01,0x00,0x01,0x00,
	// Public Modulus "m"
	0xd3,0xd7,0x4e,0xe5, 0x66,0x3d,0xd7,0xe6, 0xc2,0xd4,0xa3,0xa1, 0xf2,0x17,0x36,0xd4,
	0x2e,0x52,0xf6,0xd2, 0x02,0x10,0xf5,0x64, 0x9c,0x34,0x7b,0xff, 0xef,0x7f,0xc2,0xee,
	0xbd,0x05,0x8b,0xde, 0x79,0xb4,0x77,0x8e, 0x5b,0x8c,0x14,0x99, 0xe3,0xae,0xc6,0x73,
	0x72,0x73,0xb5,0xfb, 0x01,0x5b,0x58,0x46, 0x6d,0xfc,0x8a,0xd6, 0x95,0xda,0xed,0x1b,
	0x2e,0x2f,0xa2,0x29, 0xe1,0x3f,0xf1,0xb9, 0x5b,0x64,0x51,0x2e, 0xa2,0xc0,0xf7,0xba,
	0xb3,0x3e,0x8a,0x75, 0xff,0x06,0x92,0x5c, 0x07,0x26,0x75,0x79, 0x10,0x5d,0x47,0xbe,
	0xd1,0x6a,0x52,0x90, 0x0b,0xae,0x6a,0x0b, 0x33,0x44,0x93,0x5e, 0xf9,0x9d,0xfb,0x15,
	0xd9,0xa4,0x1c,0xcf, 0x6f,0xe4,0x71,0x94, 0xbe,0x13,0x00,0xa8, 0x52,0xca,0x07,0xbd,
	0x27,0x98,0x01,0xa1, 0x9e,0x4f,0xa3,0xed, 0x9f,0xa0,0xaa,0x73, 0xc4,0x71,0xf3,0xe9,
	0x4e,0x72,0x42,0x9c, 0xf0,0x39,0xce,0xbe, 0x03,0x76,0xfa,0x2b, 0x89,0x14,0x9a,0x81,
	0x16,0xc1,0x80,0x8c, 0x3e,0x6b,0xaa,0x05, 0xec,0x67,0x5a,0xcf, 0xa5,0x70,0xbd,0x60,
	0x0c,0xe8,0x37,0x9d, 0xeb,0xf4,0x52,0xea, 0x4e,0x60,0x9f,0xe4, 0x69,0xcf,0x52,0xdb,
	0x68,0xf5,0x11,0xcb, 0x57,0x8f,0x9d,0xa1, 0x38,0x0a,0x0c,0x47, 0x1b,0xb4,0x6c,0x5a,
	0x53,0x6e,0x26,0x98, 0xf1,0x88,0xae,0x7c, 0x96,0xbc,0xf6,0xbf, 0xb0,0x47,0x9a,0x8d,
	0xe4,0xb3,0xe2,0x98, 0x85,0x61,0xb1,0xca, 0x5f,0xf7,0x98,0x51, 0x2d,0x83,0x81,0x76,
	0x0c,0x88,0xba,0xd4, 0xc2,0xd5,0x3c,0x14, 0xc7,0x72,0xda,0x7e, 0xbd,0x1b,0x4b,0xa4,
	// Padding? Unused, by the way
	0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00
};

