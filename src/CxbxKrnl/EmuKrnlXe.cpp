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
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

// prevent name collisions
namespace xboxkrnl
{
#include <xboxkrnl/xboxkrnl.h> // For XeImageFileName, etc.
};

#include "CxbxKrnl.h" // For CxbxKrnl_Xbe
#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"
#include "Emu.h" // For EmuWarning()

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

	NTSTATUS ret = STATUS_INVALID_HANDLE;

	void* sectionData = CxbxKrnl_Xbe->FindSection((char*)std::string(Section->SectionName, 9).c_str());
	if (sectionData != nullptr) {
		// If the reference count was zero, load the section
		if (Section->SectionReferenceCount == 0) {
			// Clear the memory the section requires
			memset(Section->VirtualAddress, 0, Section->VirtualSize);
			// Copy the section data
			memcpy(Section->VirtualAddress, sectionData, Section->FileSize);
		}

		// Increment the reference count
		Section->SectionReferenceCount++;

		ret = STATUS_SUCCESS;
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

		// Free the section if necessary
		if (Section->SectionReferenceCount == 0) {
			memset(Section->VirtualAddress, 0, Section->VirtualSize);
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

