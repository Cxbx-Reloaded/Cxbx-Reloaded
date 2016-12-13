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
	void* section
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(section)
		LOG_FUNC_END;

	if (((Xbe::SectionHeader*)section)->dwSectionRefCount > 0) {
		((Xbe::SectionHeader*)section)->dwSectionRefCount++;
		RETURN(STATUS_SUCCESS);
	}

	EmuWarning("XeLoadSection lied");

	RETURN(STATUS_SUCCESS);
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
	void* section
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(section)
		LOG_FUNC_END;

	if (((Xbe::SectionHeader*)section)->dwSectionRefCount == 0) {
		RETURN(STATUS_INVALID_PARAMETER);
	}

	EmuWarning("XeUnloadSection lied");

	RETURN(STATUS_SUCCESS);
}

// ******************************************************************
// * 0x0163 - XePublicKeyData
// ******************************************************************
// TODO : What should we initialize this to?
XBSYSAPI EXPORTNUM(355) xboxkrnl::DWORD xboxkrnl::XePublicKeyData = 0;

