// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
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

#define LOG_PREFIX CXBXR_MODULE::OB

// prevent name collisions
namespace xboxkrnl
{
#include <xboxkrnl/xboxkrnl.h> // For ObDirectoryObjectType, etc.
};

#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"
#include "core\kernel\init\CxbxKrnl.h" // For CxbxKrnlCleanup
#include "EmuKrnl.h" // For OBJECT_TO_OBJECT_HEADER()
#include "core\kernel\support\EmuFile.h" // For EmuNtSymbolicLinkObject, NtStatusToString(), etc.
#include <cassert>

#pragma warning(disable:4005) // Ignore redefined status values
#include <ntstatus.h>
#pragma warning(default:4005)

#define FIELD_OFFSET(type,field)  ((ULONG)&(((type *)0)->field))

#define INITIALIZED_OBJECT_STRING(ObjectString, Value)                  \
	xboxkrnl::CHAR ObjectString##Buffer[] = Value;                      \
	xboxkrnl::OBJECT_STRING ObjectString = {							\
	sizeof(Value) - sizeof(CHAR),                                      \
	sizeof(Value),                                                      \
	ObjectString##Buffer                                                \
}

INITIALIZED_OBJECT_STRING(ObpDosDevicesString, "\\??");
INITIALIZED_OBJECT_STRING(ObpIoDevicesString, "\\Device");
INITIALIZED_OBJECT_STRING(ObpWin32NamedObjectsString, "\\Win32NamedObjects");

xboxkrnl::POBJECT_DIRECTORY ObpDosDevicesDirectoryObject;
xboxkrnl::POBJECT_DIRECTORY ObpWin32NamedObjectsDirectoryObject;
xboxkrnl::POBJECT_DIRECTORY ObpRootDirectoryObject;
xboxkrnl::POBJECT_DIRECTORY ObpIoDevicesDirectoryObject;

XBSYSAPI EXPORTNUM(245) xboxkrnl::OBJECT_HANDLE_TABLE xboxkrnl::ObpObjectHandleTable = {

};

xboxkrnl::PVOID ObpDosDevicesDriveLetterMap['Z' - 'A' + 1];

xboxkrnl::BOOLEAN xboxkrnl::ObpCreatePermanentDirectoryObject(
	IN xboxkrnl::POBJECT_STRING DirectoryName OPTIONAL,
	OUT xboxkrnl::POBJECT_DIRECTORY *DirectoryObject
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(DirectoryName)
		LOG_FUNC_ARG(DirectoryObject)
		LOG_FUNC_END;

	OBJECT_ATTRIBUTES ObjectAttributes;
	XB_InitializeObjectAttributes(&ObjectAttributes, DirectoryName, OBJ_PERMANENT, NULL, NULL);

	HANDLE Handle;
	NTSTATUS status = NtCreateDirectoryObject(&Handle, &ObjectAttributes);

	if (!NT_SUCCESS(status)) {
		RETURN(FALSE);
	}
	
	status = ObReferenceObjectByHandle(Handle, &ObDirectoryObjectType, (PVOID *)DirectoryObject);
	
	if (!NT_SUCCESS(status)) {
		RETURN(FALSE);
	}

	NtClose(Handle);
	
	RETURN(TRUE);
}

xboxkrnl::NTSTATUS xboxkrnl::ObpReferenceObjectByName(
	IN HANDLE RootDirectoryHandle,
	IN POBJECT_STRING ObjectName,
	IN ULONG Attributes,
	IN POBJECT_TYPE ObjectType,
	IN OUT PVOID ParseContext OPTIONAL,
	OUT PVOID *ReturnedObject
)
{
	NTSTATUS status;
	*ReturnedObject = NULL;
	
	KIRQL OldIrql = KeRaiseIrqlToDpcLevel();
	
	OBJECT_STRING RemainingName;
	if (ObjectName != NULL) {
		RemainingName = *ObjectName;
	} else {
		RtlZeroMemory(&RemainingName, sizeof(OBJECT_STRING));
	}

	BOOLEAN ResolveSymbolicLink = TRUE;
	PVOID FoundObject;

	if (RootDirectoryHandle != NULL) {
		if (RootDirectoryHandle == ObDosDevicesDirectory()) {
			FoundObject = ObpDosDevicesDirectoryObject;
		} else if (RootDirectoryHandle == ObWin32NamedObjectsDirectory()) {
			FoundObject = ObpWin32NamedObjectsDirectoryObject;
		} else {
			FoundObject = (POBJECT_DIRECTORY)ObpGetObjectHandleContents(RootDirectoryHandle);

			if (FoundObject == NULL) {
				status = STATUS_INVALID_HANDLE;
				goto CleanupAndExit;
			}
		}

		if ((RemainingName.Length != 0) &&
			(RemainingName.Buffer[0] == OBJ_NAME_PATH_SEPARATOR)) {
			status = STATUS_OBJECT_NAME_INVALID;
			goto CleanupAndExit;
		}

		goto OpenRootDirectory;
	}

	if ((RemainingName.Length == 0) ||
		(RemainingName.Buffer[0] != OBJ_NAME_PATH_SEPARATOR)) {
		status = STATUS_OBJECT_NAME_INVALID;
		goto CleanupAndExit;
	}

	if (RemainingName.Length == sizeof(CHAR)) {
		RemainingName.Buffer++;
		RemainingName.Length = 0;
		RemainingName.MaximumLength = 0;
		goto OpenRootDirectory;
	}

	OBJECT_STRING ElementName;
	for (;;) {
		POBJECT_DIRECTORY Directory = (POBJECT_DIRECTORY)FoundObject;
		ObDissectName(RemainingName, &ElementName, &RemainingName);

		if (RemainingName.Length != 0) {
			if (RemainingName.Buffer[0] == OBJ_NAME_PATH_SEPARATOR) {
				status = STATUS_OBJECT_NAME_INVALID;
				goto CleanupAndExit;
			}
		} else {
			if (ObjectType == &ObSymbolicLinkObjectType) {
				ResolveSymbolicLink = FALSE;
			}
		}

		if (!ObpLookupElementNameInDirectory(Directory, &ElementName,
			ResolveSymbolicLink, &FoundObject)) {
			status = (RemainingName.Length != 0) ? STATUS_OBJECT_PATH_NOT_FOUND : STATUS_OBJECT_NAME_NOT_FOUND;
			goto CleanupAndExit;
		}

OpenRootDirectory:
		POBJECT_HEADER ObjectHeader = OBJECT_TO_OBJECT_HEADER(FoundObject);

		if (RemainingName.Length == 0) {
			if (ObjectHeader->Type->ParseProcedure != NULL) {
				goto InvokeParseProcedure;
			}

			if ((ObjectType != NULL) && (ObjectType != ObjectHeader->Type)) {
				status = STATUS_OBJECT_TYPE_MISMATCH;
				goto CleanupAndExit;
			}

			ObjectHeader->PointerCount++;
			*ReturnedObject = FoundObject;
			status = STATUS_SUCCESS;
			goto CleanupAndExit;
		}

		if (ObjectHeader->Type != &ObDirectoryObjectType) {
			if (ObjectHeader->Type->ParseProcedure == NULL) {
				status = STATUS_OBJECT_PATH_NOT_FOUND;
				goto CleanupAndExit;
			}

InvokeParseProcedure:
			ObjectHeader->PointerCount++;
			KfLowerIrql(OldIrql);

			if (ObjectHeader->Type != &IoFileObjectType && (RemainingName.Buffer > ObjectName->Buffer)) {
				RemainingName.Buffer--;
				RemainingName.Length += sizeof(CHAR);
				RemainingName.MaximumLength = RemainingName.Length;
			}

			PVOID ParsedObject = NULL;
			status = ObjectHeader->Type->ParseProcedure(FoundObject, ObjectType, Attributes, ObjectName, &RemainingName, ParseContext, &ParsedObject);
			ObfDereferenceObject(FoundObject);

			if (NT_SUCCESS(status)) {
				if ((ObjectType == NULL) || (ObjectType == OBJECT_TO_OBJECT_HEADER(ParsedObject)->Type)) {
					*ReturnedObject = ParsedObject;
					status = STATUS_SUCCESS;
				} else {
					ObfDereferenceObject(ParsedObject);
					status = STATUS_OBJECT_TYPE_MISMATCH;
				}
			}

			return status;
		}
	}
CleanupAndExit:
	KfLowerIrql(OldIrql);
	return status;
}

xboxkrnl::BOOLEAN xboxkrnl::ObInitSystem()
{
	ObpObjectHandleTable.HandleCount = 0;
	ObpObjectHandleTable.FirstFreeTableEntry = -1;
	ObpObjectHandleTable.NextHandleNeedingPool = 0;
	ObpObjectHandleTable.RootTable = NULL;

	RtlZeroMemory(ObpDosDevicesDriveLetterMap, sizeof(ObpDosDevicesDriveLetterMap));

	if (!ObpCreatePermanentDirectoryObject(NULL, &ObpRootDirectoryObject)) {
		return FALSE;
	}

	if (!ObpCreatePermanentDirectoryObject(&ObpDosDevicesString, &ObpDosDevicesDirectoryObject)) {
		return FALSE;
	}

	if (!ObpCreatePermanentDirectoryObject(&ObpIoDevicesString, &ObpIoDevicesDirectoryObject)) {
		return FALSE;
	}

	if (!ObpCreatePermanentDirectoryObject(&ObpWin32NamedObjectsString, &ObpWin32NamedObjectsDirectoryObject)) {
		return FALSE;
	}

	return TRUE;
}

xboxkrnl::BOOLEAN xboxkrnl::ObpExtendObjectHandleTable()
{
	PVOID* NewTable = (PVOID*)ExAllocatePoolWithTag(sizeof(PVOID) * OB_HANDLES_PER_TABLE, 'tHbO');
	if (NewTable == NULL) {
		return FALSE;
	}

	PVOID **NewRootTable;
	SIZE_T NewRootTableSize;
	if ((HandleToUlong(ObpObjectHandleTable.NextHandleNeedingPool) & (sizeof(PVOID) * OB_HANDLES_PER_SEGMENT - 1)) == 0) {
		if (ObpObjectHandleTable.NextHandleNeedingPool == NULL) {
			NewRootTable = ObpObjectHandleTable.BuiltinRootTable;
		} else {
			SIZE_T OldRootTableSize = HandleToUlong(ObpObjectHandleTable.NextHandleNeedingPool) / (sizeof(PVOID*) * OB_HANDLES_PER_TABLE);
			NewRootTableSize = OldRootTableSize + OB_TABLES_PER_SEGMENT;

			NewRootTable = (PVOID**)ExAllocatePoolWithTag(sizeof(PVOID*) * NewRootTableSize, 'rHbO');
			if (NewRootTable == NULL) {
				ExFreePool(NewTable);
				return FALSE;
			}

			RtlCopyMemory(NewRootTable, ObpObjectHandleTable.RootTable, sizeof(PVOID*) * OldRootTableSize);

			if (ObpObjectHandleTable.RootTable != ObpObjectHandleTable.BuiltinRootTable) {
				ExFreePool(ObpObjectHandleTable.RootTable);
			}
		}

		ObpObjectHandleTable.RootTable = NewRootTable;
	}

	ObpGetTableFromHandle(ObpObjectHandleTable.NextHandleNeedingPool) = NewTable;
	HANDLE Handle = ObpObjectHandleTable.NextHandleNeedingPool;
	PVOID *HandleContents = NewTable;
	LONG_PTR FreeHandleLink = ObpEncodeFreeHandleLink(Handle);
	ObpObjectHandleTable.FirstFreeTableEntry = FreeHandleLink;

	for (ULONG Index = 0; Index < OB_HANDLES_PER_TABLE - 1; Index++) {
		FreeHandleLink += sizeof(PVOID);
		*HandleContents++ = (PVOID)FreeHandleLink;
	}

	*HandleContents = (PVOID)-1;

	if (Handle == NULL) {
		HandleContents = NewTable;
		ObpObjectHandleTable.FirstFreeTableEntry = (LONG_PTR)*HandleContents;
		*HandleContents = NULL;
	}

	ObpObjectHandleTable.NextHandleNeedingPool = (HANDLE)(HandleToLong(Handle) + (sizeof(PVOID) * OB_HANDLES_PER_TABLE));

	return TRUE;
}

xboxkrnl::HANDLE xboxkrnl::ObpCreateObjectHandle(xboxkrnl::PVOID Object)
{
	LOG_FUNC_ONE_ARG(Object);

	HANDLE Handle;
	PVOID *HandleContents;

	if (ObpObjectHandleTable.FirstFreeTableEntry == -1) {
		if (!ObpExtendObjectHandleTable()) {
			return NULL;
		}
	}

	Handle = (HANDLE)ObpDecodeFreeHandleLink(ObpObjectHandleTable.FirstFreeTableEntry);
	HandleContents = ObpGetHandleContentsPointer(Handle);
	ObpObjectHandleTable.FirstFreeTableEntry = (LONG_PTR)*HandleContents;
	ObpObjectHandleTable.HandleCount++;
	OBJECT_TO_OBJECT_HEADER(Object)->HandleCount++;
	*HandleContents = Object;
	return Handle;
}

xboxkrnl::VOID xboxkrnl::ObDissectName(OBJECT_STRING Path, POBJECT_STRING FirstName, POBJECT_STRING RemainingName)
{
	ULONG i = 0;
	
	FirstName->Length = 0;
	FirstName->MaximumLength = 0;
	FirstName->Buffer = NULL;

	RemainingName->Length = 0;
	RemainingName->MaximumLength = 0;
	RemainingName->Buffer = NULL;

	ULONG PathLength = Path.Length / sizeof(CHAR);

	if (PathLength == 0) {
		return;
	}

	if (Path.Buffer[0] == '\\') {
		i = 1;
	}

	ULONG FirstNameStart;
	for (FirstNameStart = i; (i < PathLength) && (Path.Buffer[i] != '\\'); i += 1) {
		;
	}

	FirstName->Length = (USHORT)((i - FirstNameStart) * sizeof(CHAR));
	FirstName->MaximumLength = FirstName->Length;
	FirstName->Buffer = &Path.Buffer[FirstNameStart];

	if (i < PathLength) {
		RemainingName->Length = (USHORT)((PathLength - (i + 1)) * sizeof(CHAR));
		RemainingName->MaximumLength = RemainingName->Length;
		RemainingName->Buffer = &Path.Buffer[i + 1];
	}

	return;
}

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

	if (ObjectAttributes == NULL || ObjectAttributes->ObjectName == NULL) {
		POBJECT_HEADER ObjectHeader = (POBJECT_HEADER)ObjectType->AllocateProcedure(FIELD_OFFSET(OBJECT_HEADER, Body) + ObjectBodySize, ObjectType->PoolTag);

		if (ObjectHeader == nullptr) {
			RETURN(STATUS_INSUFFICIENT_RESOURCES);
		}

		ObjectHeader->PointerCount = 1;
		ObjectHeader->HandleCount = 0;
		ObjectHeader->Type = ObjectType;
		ObjectHeader->Flags = 0;

		*Object = &ObjectHeader->Body;
		
		RETURN(STATUS_SUCCESS);
	}

	OBJECT_STRING RemainingName = *ObjectAttributes->ObjectName;
	OBJECT_STRING ElementName;
	ElementName.Buffer = NULL;
	ElementName.Length = 0;

	while (RemainingName.Length != 0) {
		ObDissectName(RemainingName, &ElementName, &RemainingName);
		if ((RemainingName.Length != 0) && (RemainingName.Buffer[0] == OBJ_NAME_PATH_SEPARATOR)) {
			return STATUS_OBJECT_NAME_INVALID;
		}
	}

	if (ElementName.Length == 0) {
		return STATUS_OBJECT_NAME_INVALID;
	}

	ObjectBodySize = ALIGN_UP(ObjectBodySize, ULONG);

	POBJECT_HEADER_NAME_INFO ObjectNameInfo = (POBJECT_HEADER_NAME_INFO)ObjectType->AllocateProcedure(
		sizeof(OBJECT_HEADER_NAME_INFO) + FIELD_OFFSET(OBJECT_HEADER, Body) +
		ObjectBodySize + ElementName.Length, ObjectType->PoolTag);

	if (ObjectNameInfo == NULL) {
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	POBJECT_HEADER ObjectHeader = (POBJECT_HEADER)(ObjectNameInfo + 1);
	ObjectNameInfo->ChainLink = NULL;
	ObjectNameInfo->Directory = NULL;
	ObjectNameInfo->Name.Buffer = (PSTR)((PUCHAR)&ObjectHeader->Body +	ObjectBodySize);
	ObjectNameInfo->Name.Length = ElementName.Length;
	ObjectNameInfo->Name.MaximumLength = ElementName.Length;

	RtlCopyMemory(ObjectNameInfo->Name.Buffer, ElementName.Buffer, ElementName.Length);

	ObjectHeader->PointerCount = 1;
	ObjectHeader->HandleCount = 0;
	ObjectHeader->Type = ObjectType;
	ObjectHeader->Flags = OB_FLAG_NAMED_OBJECT;

	*Object = &ObjectHeader->Body;

	RETURN(STATUS_SUCCESS);
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

xboxkrnl::PVOID xboxkrnl::ObpGetObjectHandleContents(HANDLE Handle)
{
	PVOID *HandleContents;
	PVOID Object;
	Handle = ObpMaskOffApplicationBits(Handle);
	
	if (HandleToUlong(Handle) < HandleToUlong(ObpObjectHandleTable.NextHandleNeedingPool)) {
		HandleContents = ObpGetHandleContentsPointer(Handle);
		Object = *HandleContents;

		if (Object != NULL && !ObpIsFreeHandleLink(Object)) {
			return Object;
		}
	}

	return NULL;
}

xboxkrnl::ULONG FASTCALL xboxkrnl::ObpComputeHashIndex(
	IN POBJECT_STRING ElementName
)
{

	ULONG HashIndex = 0;
	PUCHAR Buffer = (PUCHAR)ElementName->Buffer;
	PUCHAR BufferEnd = Buffer + ElementName->Length;
	
	// Calculate hash of string data
	UCHAR Char;
	while (Buffer < BufferEnd) {
		Char = *Buffer++;
		// Skip special characters
		if (Char >= 0x80) {
			continue;
		}

		// Force all characters to be lowercase
		Char |= 0x20;

		HashIndex += (HashIndex << 1) + (HashIndex >> 1) + Char;
	}

	return HashIndex % OB_NUMBER_HASH_BUCKETS;
}

xboxkrnl::PVOID xboxkrnl::ObpGetObjectHandleReference(HANDLE Handle)
{
	PVOID *HandleContents;
	Handle = ObpMaskOffApplicationBits(Handle);
	KIRQL OldIrql = KeRaiseIrqlToDpcLevel();

	if (HandleToUlong(Handle) < HandleToUlong(ObpObjectHandleTable.NextHandleNeedingPool)) {
		HandleContents = ObpGetHandleContentsPointer(Handle);
		PVOID Object = *HandleContents;

		if (Object != NULL && !ObpIsFreeHandleLink(Object)) {
			OBJECT_TO_OBJECT_HEADER(Object)->PointerCount++;
			KfLowerIrql(OldIrql);
			return Object;
		}
	}

	KfLowerIrql(OldIrql);
	return NULL;
}

xboxkrnl::BOOLEAN xboxkrnl::ObpLookupElementNameInDirectory(
	IN POBJECT_DIRECTORY Directory,
	IN POBJECT_STRING ElementName,
	IN BOOLEAN ResolveSymbolicLink,
	OUT PVOID *ReturnedObject
)
{
	PVOID Object = NULL;

	if ((Directory == ObpDosDevicesDirectoryObject) && ResolveSymbolicLink && (ElementName->Length == sizeof(CHAR) * 2) && (ElementName->Buffer[1] == (CHAR)':')) {
		CHAR DriveLetter = ElementName->Buffer[0];

		if (DriveLetter >= 'a' && DriveLetter <= 'z') {
			Object = ObpDosDevicesDriveLetterMap[DriveLetter - 'a'];
		} else if (DriveLetter >= 'A' && DriveLetter <= 'Z') {
			Object = ObpDosDevicesDriveLetterMap[DriveLetter - 'A'];
		}

		if (Object != NULL) {
			*ReturnedObject = Object;
			return TRUE;
		}
	}
	
	ULONG HashIndex = ObpComputeHashIndex(ElementName);
	POBJECT_HEADER_NAME_INFO ObjectHeaderNameInfo = Directory->HashBuckets[HashIndex];
	
	while (ObjectHeaderNameInfo != NULL) {
		if (RtlEqualString(&ObjectHeaderNameInfo->Name, ElementName, TRUE)) {
			Object = OBJECT_HEADER_NAME_INFO_TO_OBJECT(ObjectHeaderNameInfo);
			if (ResolveSymbolicLink && (OBJECT_TO_OBJECT_HEADER(Object)->Type == &ObSymbolicLinkObjectType)) {
				Object = ((POBJECT_SYMBOLIC_LINK)Object)->LinkTargetObject;
			}

			*ReturnedObject = Object;
			return TRUE;
		}

		ObjectHeaderNameInfo = ObjectHeaderNameInfo->ChainLink;
	}

	*ReturnedObject = NULL;
	return FALSE;
}

// ******************************************************************
// * 0x00F1 - ObInsertObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(241) xboxkrnl::NTSTATUS NTAPI xboxkrnl::ObInsertObject
(
	IN PVOID Object,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN ULONG ObjectPointerBias,
	OUT PHANDLE ReturnedHandle
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Object)
		LOG_FUNC_ARG(ObjectAttributes)
		LOG_FUNC_ARG(ObjectPointerBias)
		LOG_FUNC_ARG_OUT(ReturnedHandle)
		LOG_FUNC_END;

	NTSTATUS status;
	POBJECT_DIRECTORY Directory;

	KIRQL OldIrql = KeRaiseIrqlToDpcLevel();

	HANDLE Handle = NULL;
	PVOID InsertObject = Object;
	if (ObjectAttributes != NULL && ObjectAttributes->ObjectName != NULL) {
		OBJECT_STRING RemainingName = *ObjectAttributes->ObjectName;
		HANDLE RootDirectoryHandle = ObjectAttributes->RootDirectory;

		if (RootDirectoryHandle != NULL) {
			if (RemainingName.Buffer[0] == OBJ_NAME_PATH_SEPARATOR) {
				status = STATUS_OBJECT_NAME_INVALID;
				goto CleanupAndExit;
			}

			if (RootDirectoryHandle == ObDosDevicesDirectory()) {
				Directory = ObpDosDevicesDirectoryObject;
			} else if (RootDirectoryHandle == ObWin32NamedObjectsDirectory()) {
				Directory = ObpWin32NamedObjectsDirectoryObject;
			} else {
				Directory = (POBJECT_DIRECTORY)ObpGetObjectHandleContents(RootDirectoryHandle);

				if (Directory == NULL) {
					status = STATUS_INVALID_HANDLE;
					goto CleanupAndExit;
				}

				if (OBJECT_TO_OBJECT_HEADER(Directory)->Type != &ObDirectoryObjectType) {
					status = STATUS_OBJECT_TYPE_MISMATCH;
					goto CleanupAndExit;
				}
			}
		} else {
			if (RemainingName.Buffer[0] != OBJ_NAME_PATH_SEPARATOR) {
				status = STATUS_OBJECT_NAME_INVALID;
				goto CleanupAndExit;
			}

			Directory = ObpRootDirectoryObject;
		}

		for (;;) {
			OBJECT_STRING ElementName;
			ObDissectName(RemainingName, &ElementName, &RemainingName);
			PVOID FoundObject;
			if (ObpLookupElementNameInDirectory(Directory, &ElementName, TRUE, &FoundObject)) {
				if (RemainingName.Length == 0) {
					if (ObpIsFlagSet(ObjectAttributes->Attributes, OBJ_OPENIF)) {
						if (OBJECT_TO_OBJECT_HEADER(FoundObject)->Type == OBJECT_TO_OBJECT_HEADER(Object)->Type) {
							InsertObject = FoundObject;
							Directory = NULL;
							break;
						} else {
							status = STATUS_OBJECT_TYPE_MISMATCH;
							goto CleanupAndExit;
						}
					} else {
						status = STATUS_OBJECT_NAME_COLLISION;
						goto CleanupAndExit;
					}
				}

				if (OBJECT_TO_OBJECT_HEADER(FoundObject)->Type !=
					&ObDirectoryObjectType) {
					status = STATUS_OBJECT_PATH_NOT_FOUND;
					goto CleanupAndExit;
				}

				Directory = (POBJECT_DIRECTORY)FoundObject;
			} else {
				if (RemainingName.Length != 0) {
					status = STATUS_OBJECT_PATH_NOT_FOUND;
					goto CleanupAndExit;
				}

				break;
			}
		}
	} else {
		Directory = NULL;
	}

	Handle = ObpCreateObjectHandle(InsertObject);

	if (Handle == NULL) {
		status = STATUS_INSUFFICIENT_RESOURCES;
		goto CleanupAndExit;
	}

	POBJECT_HEADER ObjectHeader = OBJECT_TO_OBJECT_HEADER(InsertObject);
	ObjectHeader->PointerCount += ObjectPointerBias + 1;

	if (Directory != NULL) {
		POBJECT_HEADER_NAME_INFO ObjectHeaderNameInfo = OBJECT_TO_OBJECT_HEADER_NAME_INFO(Object);
		ULONG HashIndex = ObpComputeHashIndex(&ObjectHeaderNameInfo->Name);
		ObjectHeader->Flags |= OB_FLAG_ATTACHED_OBJECT;
		ObjectHeaderNameInfo->Directory = Directory;
		ObjectHeaderNameInfo->ChainLink = Directory->HashBuckets[HashIndex];
		Directory->HashBuckets[HashIndex] = ObjectHeaderNameInfo;

		if ((Directory == ObpDosDevicesDirectoryObject) && (ObjectHeaderNameInfo->Name.Length == sizeof(CHAR) * 2) && (ObjectHeaderNameInfo->Name.Buffer[1] == (CHAR)':')) {
			PVOID DosDevicesObject = Object;

			if (OBJECT_TO_OBJECT_HEADER(DosDevicesObject)->Type ==
				&ObSymbolicLinkObjectType) {
				DosDevicesObject = ((POBJECT_SYMBOLIC_LINK)DosDevicesObject)->LinkTargetObject;
			}

			CHAR DriveLetter = ObjectHeaderNameInfo->Name.Buffer[0];
			if (DriveLetter >= 'a' && DriveLetter <= 'z') {
				ObpDosDevicesDriveLetterMap[DriveLetter - 'a'] = DosDevicesObject;
			} else if (DriveLetter >= 'A' && DriveLetter <= 'Z') {
				ObpDosDevicesDriveLetterMap[DriveLetter - 'A'] = DosDevicesObject;
			}
		}

		OBJECT_TO_OBJECT_HEADER(Directory)->PointerCount++;
		ObjectHeader->PointerCount++;
	}

	if ((ObjectAttributes != NULL) &&
		ObpIsFlagSet(ObjectAttributes->Attributes, OBJ_PERMANENT)) {
		ObjectHeader->Flags |= OB_FLAG_PERMANENT_OBJECT;
	}

	status = (Object == InsertObject) ? STATUS_SUCCESS : STATUS_OBJECT_NAME_EXISTS;

CleanupAndExit:
	KfLowerIrql(OldIrql);
	ObfDereferenceObject(Object);
	*ReturnedHandle = Handle;

	RETURN(status);
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

	LOG_UNIMPLEMENTED();
	assert(false);
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

	LOG_UNIMPLEMENTED();
	assert(false);

	RETURN(STATUS_SUCCESS);
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

	LOG_UNIMPLEMENTED();
	assert(false);

	RETURN(STATUS_SUCCESS);
}

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
		LOG_FUNC_ARG(ObjectType)
		LOG_FUNC_ARG_OUT(ReturnedObject)
		LOG_FUNC_END;

	NTSTATUS status;
	PVOID Object;
	POBJECT_HEADER ObjectHeader;

	if (Handle == NtCurrentThread()) {
		if ((ObjectType == &PsThreadObjectType) || (ObjectType == NULL)) {
			Object = PsGetCurrentThread();
			ObjectHeader = OBJECT_TO_OBJECT_HEADER(Object);
			InterlockedIncrement(&ObjectHeader->PointerCount);
			*ReturnedObject = Object;
			return STATUS_SUCCESS;
		} else {
			status = STATUS_OBJECT_TYPE_MISMATCH;
		}
	} else {
		Object = ObpGetObjectHandleReference(Handle);

		if (Object != NULL) {
			ObjectHeader = OBJECT_TO_OBJECT_HEADER(Object);

			if ((ObjectType == ObjectHeader->Type) || (ObjectType == NULL)) {
				*ReturnedObject = Object;
				return STATUS_SUCCESS;
			} else {
				ObfDereferenceObject(Object);
				status = STATUS_OBJECT_TYPE_MISMATCH;
			}
		} else {
			// HACK: Since we forward to NtDll::NtCreateEvent, this *might* be a Windows handle instead of our own
			// In this case, we must return the input handle
			// Test Case: Xbox Live Dashboard, Network Test (or any other Xbox Live connection)
			//DWORD flags = 0;
			//if (GetHandleInformation(Handle, &flags)) {
			//	// This was a Windows Handle, so return it.
			//	*ReturnedObject = Handle;
			//	return STATUS_SUCCESS;
			//}

			status = STATUS_INVALID_HANDLE;
		}
	}

	*ReturnedObject = NULL;

	RETURN(status);
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
		LOG_FUNC_ARG(Attributes) // TODO : Use, how?
		LOG_FUNC_ARG(ObjectType)
		LOG_FUNC_ARG_OUT(ParseContext) // TODO : Use and populate, how?
		LOG_FUNC_ARG_OUT(Object)
		LOG_FUNC_END;

	NTSTATUS result = ObpReferenceObjectByName(NULL, ObjectName, Attributes, ObjectType, ParseContext, Object);
	RETURN(result);
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

	if (ObjectType == ObjectHeader->Type) {
		InterlockedIncrement(&ObjectHeader->PointerCount);
		RETURN(STATUS_SUCCESS);
	} 
	
	RETURN(STATUS_OBJECT_TYPE_MISMATCH);
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

	// HACK: Since we forward to NtDll::NtCreateEvent, this *might* be a Windows handle instead of our own
	// In this case, we must do nothing, otherwise we'll crash...
	// Test Case: Xbox Live Dashboard, Network Test (or any other Xbox Live connection)
	DWORD flags = 0;
	if (GetHandleInformation((HANDLE)Object, &flags)) {
		return;
	}

	POBJECT_HEADER ObjectHeader = OBJECT_TO_OBJECT_HEADER(Object);
	
	if (InterlockedDecrement(&ObjectHeader->PointerCount) == 0) {
		if (ObjectHeader->Type->DeleteProcedure != NULL) {
			ObjectHeader->Type->DeleteProcedure(Object);
		}

		PVOID ObjectBase;
		if (ObpIsFlagSet(ObjectHeader->Flags, OB_FLAG_NAMED_OBJECT)) {
			ObjectBase = OBJECT_HEADER_TO_OBJECT_HEADER_NAME_INFO(ObjectHeader);
		} else {
			ObjectBase = ObjectHeader;
		}

		ObjectHeader->Type->FreeProcedure(ObjectBase);
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
	InterlockedIncrement(&OBJECT_TO_OBJECT_HEADER(Object)->PointerCount);
}
