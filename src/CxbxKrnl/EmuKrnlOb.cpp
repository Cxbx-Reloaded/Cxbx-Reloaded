// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuKrnlOb.cpp
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
#include <xboxkrnl/xboxkrnl.h> // For ObDirectoryObjectType, etc.
};

#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"

// prevent name collisions
namespace NtDll
{
#include "EmuNtDll.h"
};

#include "Emu.h" // For EmuWarning()

// ******************************************************************
// * 0x00F0 - ObDirectoryObjectType
// ******************************************************************
XBSYSAPI EXPORTNUM(240) xboxkrnl::OBJECT_TYPE xboxkrnl::ObDirectoryObjectType =
{
	/*
	ExAllocatePoolWithTag,
	ExFreePool,
	NULL,
	NULL,
	NULL,
	*/
	NULL, // &ObpDefaultObject,
	'eriD' // = first four characters of "Directory" in reverse
};

// ******************************************************************
// * 0x00F3 - ObOpenObjectByName()
// ******************************************************************
XBSYSAPI EXPORTNUM(243) xboxkrnl::NTSTATUS NTAPI xboxkrnl::ObOpenObjectByName
(
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	IN POBJECT_TYPE ObjectType,
	IN OUT PVOID ParseContext OPTIONAL,
	OUT PHANDLE Handle
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ObjectAttributes)
		LOG_FUNC_ARG(ObjectType)
		LOG_FUNC_ARG(ParseContext)
		LOG_FUNC_ARG_OUT(Handle)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00F5 - ObpObjectHandleTable
// ******************************************************************
// TODO : Determine size. What should we initialize this to?
XBSYSAPI EXPORTNUM(245) xboxkrnl::DWORD xboxkrnl::ObpObjectHandleTable[1] = {};

// ******************************************************************
// * 0x00F6 - ObReferenceObjectByHandle()
// ******************************************************************
// Turns a handle into a kernel object pointer.  The ObjectType parameter
// specifies what type of object it is.  This function also increments the
// object's reference count.
//
// Differences from NT: There are no DesiredAccess, AccessMode, or
//     HandleInformation parameters.
XBSYSAPI EXPORTNUM(246) xboxkrnl::NTSTATUS NTAPI xboxkrnl::ObReferenceObjectByHandle
(
	IN HANDLE Handle,
	IN POBJECT_TYPE ObjectType OPTIONAL,
	OUT PVOID *ReturnedObject
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Handle)
		LOG_FUNC_ARG_OUT(ObjectType)
		LOG_FUNC_ARG_OUT(ReturnedObject)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	// This is probably incorrect
	*ReturnedObject = Handle;
	RETURN(STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00F9 - ObSymbolicLinkObjectType
// ******************************************************************
XBSYSAPI EXPORTNUM(249) xboxkrnl::OBJECT_TYPE xboxkrnl::ObSymbolicLinkObjectType =
{
	/*
	ExAllocatePoolWithTag,
	ExFreePool,
	NULL,
	ObpDeleteSymbolicLink,
	NULL,
	*/
	NULL, // &ObpDefaultObject,
	'bmyS' // = first four characters of "SymbolicLink" in reverse
};

// ******************************************************************
// * 0x00FA - ObfDereferenceObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(250) xboxkrnl::VOID FASTCALL xboxkrnl::ObfDereferenceObject
(
	IN PVOID Object
)
{
	LOG_FUNC_ONE_ARG_OUT(Object);

	LOG_UNIMPLEMENTED();
}
