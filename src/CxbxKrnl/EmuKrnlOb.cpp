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

#include "CxbxKrnl.h" // For CxbxKrnlCleanup
#include "Emu.h" // For EmuWarning()
#include "EmuKrnl.h" // For OBJECT_TO_OBJECT_HEADER()
#include "EmuFile.h" // For EmuNtSymbolicLinkObject, NtStatusToString(), etc.

#pragma warning(disable:4005) // Ignore redefined status values
#include <ntstatus.h>
#pragma warning(default:4005)

#define OB_FLAG_NAMED_OBJECT 1
#define OB_FLAG_PERMANENT_OBJECT 2

// ******************************************************************
// * 0x00EF - ObCreateObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(239) xboxkrnl::NTSTATUS NTAPI xboxkrnl::ObCreateObject
(
	IN POBJECT_TYPE ObjectType,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN ULONG ObjectBodySize,
	OUT PVOID *Object
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ObjectType)
		LOG_FUNC_ARG(ObjectAttributes)
		LOG_FUNC_ARG(ObjectBodySize)
		LOG_FUNC_ARG_OUT(Object)
		LOG_FUNC_END;

	NTSTATUS result = STATUS_SUCCESS;
	OBJECT_STRING TmpName = { 0 };
	int NameSize = 0;

	if (ObjectAttributes != NULL && ObjectAttributes->ObjectName != NULL) {
		if (ObjectAttributes->ObjectName->Length == 0) {
			result = STATUS_OBJECT_NAME_INVALID;
		}
		else {
			// TODO : Split name in parts?
			TmpName.Buffer = ObjectAttributes->ObjectName->Buffer;
			TmpName.Length = ObjectAttributes->ObjectName->Length;
			TmpName.MaximumLength = ObjectAttributes->ObjectName->Length;
			NameSize = sizeof(OBJECT_STRING) + TmpName.Length + sizeof('\0'); // trailing zero
		}
	}

	if (result == STATUS_SUCCESS) {
		// Allocate the object
		int ObjectSize = offsetof(OBJECT_HEADER, Body) + ObjectBodySize;
		// TODO : Is this ever something else than ExAllocatePoolWithTag ?
		POBJECT_HEADER ObjectHeader = (POBJECT_HEADER)ObjectType->AllocateProcedure(ObjectSize + NameSize, ObjectType->PoolTag);
		if (ObjectHeader == NULL) {
			// Detected out of memory
			*Object = NULL;
			result = STATUS_INSUFFICIENT_RESOURCES;
		}
		else {
			// Initialize default values of object (header) :
			ObjectHeader->PointerCount = 1;
			ObjectHeader->HandleCount = 0;
			ObjectHeader->Type = ObjectType;
			if (NameSize == 0)
				ObjectHeader->Flags = 0;
			else {
				// TODO : For other Ob* API's it must become possible to get from
				// and Object(Header) address to the Name. Right now, this requires
				// adding ObjectSize to ObjectHeader. This won't be available outside
				// this function, so we need a better solution for this. 
				// It might be possible to put the OBJECT_STRING struct BEFORE the
				// ObjectHeader (and the NameBuffer itself before that), which would
				// make it possible to simply offset everything off an Object.
				// It might also be possible to insert a linked list struct, so
				// ObReferenceObjectByName can iterate over all named objects.
				// (It's probably wise to use one list per pool, to reduce the number
				// of objects to walk through.)

				// Copy name after object (we've reserved NameSize bytes there) :
				OBJECT_STRING *Name = (OBJECT_STRING *)((char *)ObjectHeader + ObjectSize);
				char *NameBuffer = (char *)Name + sizeof(OBJECT_STRING);
				// Initialize name struct :
				Name->Buffer = NameBuffer;
				Name->Length = Name->MaximumLength = TmpName.Length;
				// Copy name into reserved buffer :
				memcpy(NameBuffer, TmpName.Buffer, TmpName.Length);
				// Mark object as named :
				ObjectHeader->Flags = OB_FLAG_NAMED_OBJECT;
				// TODO : Verify this all works, then use it somehow
			}

			*Object = &ObjectHeader->Body;
			result = STATUS_SUCCESS;
		}
	}

	RETURN(result);
}

// ******************************************************************
// * 0x00F0 - ObDirectoryObjectType
// ******************************************************************
XBSYSAPI EXPORTNUM(240) xboxkrnl::OBJECT_TYPE xboxkrnl::ObDirectoryObjectType =
{
	xboxkrnl::ExAllocatePoolWithTag,
	xboxkrnl::ExFreePool,
	NULL,
	NULL,
	NULL,
	NULL, // TODO : &xboxkrnl::ObpDefaultObject,
	'eriD' // = first four characters of "Directory" in reverse
};

// ******************************************************************
// * 0x00F1 - ObInsertObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(241) xboxkrnl::NTSTATUS NTAPI xboxkrnl::ObInsertObject
(
	IN PVOID Object,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN ULONG ObjectPointerBias,
	OUT PHANDLE Handle
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Object)
		LOG_FUNC_ARG(ObjectAttributes)
		LOG_FUNC_ARG(ObjectPointerBias)
		LOG_FUNC_ARG_OUT(Handle)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x00F2 - ObMakeTemporaryObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(242) xboxkrnl::VOID NTAPI xboxkrnl::ObMakeTemporaryObject
(
	IN PVOID Object
)
{
	LOG_FUNC_ONE_ARG(Object);

	/* Get the header */
	POBJECT_HEADER ObjectHeader = OBJECT_TO_OBJECT_HEADER(Object);

	/* Acquire object lock */
	//ObpAcquireObjectLock(ObjectHeader);
	LOG_INCOMPLETE(); // TODO : Lock, etc.

	/* Remove the flag */
	ObjectHeader->Flags &= ~OB_FLAG_PERMANENT_OBJECT;

	/* Release the lock */
	// ObpReleaseObjectLock(ObjectHeader);

	/* Check if we should delete the object now */
	//ObpDeleteNameCheck(ObjectBody);
}

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

	NTSTATUS ret = STATUS_OBJECT_PATH_NOT_FOUND;

	// TODO : Call CxbxObjectAttributesToNT on ObjectAttributes?

	if (ObjectType == &xboxkrnl::ObSymbolicLinkObjectType)
	{
		EmuNtSymbolicLinkObject* symbolicLinkObject =
			FindNtSymbolicLinkObjectByName(PSTRING_to_string(ObjectAttributes->ObjectName));

		if (symbolicLinkObject != NULL)
		{
			// Return a new handle (which is an EmuHandle, actually) :
			*Handle = symbolicLinkObject->NewHandle();
			ret = STATUS_SUCCESS;
		}
	}
	else
	if (ObjectType == &xboxkrnl::ObDirectoryObjectType)
		LOG_UNIMPLEMENTED();
	else
		LOG_UNIMPLEMENTED();

	if (ret == STATUS_SUCCESS)
		DbgPrintf("EmuKrnl : ObOpenObjectByName Handle^ = 0x%.08X", *Handle);
	else
		EmuWarning("ObOpenObjectByName failed! (%s)", NtStatusToString(ret));

	RETURN(ret);
}

// ******************************************************************
// * 0x00F4 - ObOpenObjectByPointer()
// ******************************************************************
XBSYSAPI EXPORTNUM(244) xboxkrnl::NTSTATUS NTAPI xboxkrnl::ObOpenObjectByPointer
(
	IN PVOID Object,
	IN POBJECT_TYPE ObjectType,
	OUT PHANDLE Handle
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Object)
		LOG_FUNC_ARG(ObjectType)
		LOG_FUNC_ARG_OUT(Handle)
		LOG_FUNC_END;

	HANDLE new_handle;
	NTSTATUS result = ObReferenceObjectByPointer(Object, ObjectType);

	if (!NT_SUCCESS(result))
		new_handle = (HANDLE)0;
	else {
		LOG_UNIMPLEMENTED();

		// TODO : Create a new_handle for this object
		// if that fails, do something like :
		// {
		//   // Detected out of memory
		//	 ObDereferenceObject(Object);
		//	 result = STATUS_INSUFFICIENT_RESOURCES;
		// }
	}

	// Set the new handle and return the correct status
	*Handle = new_handle;
	RETURN(result);
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

	// This is most certainly incorrect
	*ReturnedObject = Handle;

	LOG_UNIMPLEMENTED();
	// TODO : Implement and use a handle registration data structure

	RETURN(STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00F7 - ObReferenceObjectByName()
// ******************************************************************
XBSYSAPI EXPORTNUM(247) xboxkrnl::NTSTATUS NTAPI xboxkrnl::ObReferenceObjectByName
(
	IN POBJECT_STRING ObjectName,
	IN ULONG Attributes,
	IN POBJECT_TYPE ObjectType,
	IN OUT PVOID ParseContext OPTIONAL,
	OUT PVOID *Object
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ObjectName)
		LOG_FUNC_ARG(Attributes)
		LOG_FUNC_ARG(ObjectType)
		LOG_FUNC_ARG_OUT(ParseContext)
		LOG_FUNC_ARG_OUT(Object)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();
	// TODO : Implement a mechanism by which all named objects can be queried. See comments in ObCreateObject

	RETURN(S_OK);
}

// ******************************************************************
// * 0x00F8 - ObReferenceObjectByPointer()
// ******************************************************************
XBSYSAPI EXPORTNUM(248) xboxkrnl::NTSTATUS NTAPI xboxkrnl::ObReferenceObjectByPointer
(
	IN PVOID Object,
	IN POBJECT_TYPE ObjectType
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Object)
		LOG_FUNC_ARG(ObjectType)
		LOG_FUNC_END;

	POBJECT_HEADER ObjectHeader = OBJECT_TO_OBJECT_HEADER(Object);
	NTSTATUS result = STATUS_SUCCESS;
	if (ObjectType == ObjectHeader->Type)
		InterlockedIncrement(&ObjectHeader->PointerCount); // Same as ObfReferenceObject
	else
		result = STATUS_OBJECT_TYPE_MISMATCH;

	RETURN(result);
}

// ******************************************************************
// * 0x00F9 - ObSymbolicLinkObjectType
// ******************************************************************
XBSYSAPI EXPORTNUM(249) xboxkrnl::OBJECT_TYPE xboxkrnl::ObSymbolicLinkObjectType =
{
	xboxkrnl::ExAllocatePoolWithTag,
	xboxkrnl::ExFreePool,
	NULL,
	NULL, // TODO : xboxkrnl::ObpDeleteSymbolicLink,
	NULL,
	NULL, // TODO : &xboxkrnl::ObpDefaultObject,
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
	
	POBJECT_HEADER ObjectHeader = OBJECT_TO_OBJECT_HEADER(Object);
	
	if (InterlockedDecrement(&ObjectHeader->PointerCount) == 0)
	{
		if (ObjectHeader->Type->DeleteProcedure != NULL)
			ObjectHeader->Type->DeleteProcedure(Object);
		
		// TODO : How to handle named objects? See comments in ObCreateObject
		ObjectHeader->Type->FreeProcedure(ObjectHeader); // TODO : Is this ever something else than ExFreePool ?
	}
}

// ******************************************************************
// * 0x00FB - ObfReferenceObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(251) xboxkrnl::VOID FASTCALL xboxkrnl::ObfReferenceObject
(
	IN PVOID Object
)
{
	LOG_FUNC_ONE_ARG_OUT(Object);

	POBJECT_HEADER ObjectHeader = OBJECT_TO_OBJECT_HEADER(Object);

	InterlockedIncrement(&ObjectHeader->PointerCount);
}
