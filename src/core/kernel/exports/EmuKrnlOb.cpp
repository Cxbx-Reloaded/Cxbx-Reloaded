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

// TODO: What needs proper implement from Ob functions' side: (use CXBX_KERNEL_REWORK_ENABLED defined to evaluate what needs fixing)
// ObpDeleteSymbolicLink - internal function which would need a call to ObDereferenceObject with LinkTargetObject pass down.
//   Plus it require proper IoCreateSymbolicLink implement as well, not emulated implement method.
// OBJECT_DIRECTORY interface (root, dos, device, and named object, you can find global varables about 30 lines below)
// etc (add more above this line)


#include <core\kernel\exports\xboxkrnl.h> // For ObDirectoryObjectType, etc.
#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"
#include "core\kernel\init\CxbxKrnl.h" // For CxbxrAbort
#include "EmuKrnl.h" // For OBJECT_TO_OBJECT_HEADER()
#include "core\kernel\support\EmuFile.h" // For EmuNtSymbolicLinkObject, NtStatusToString(), etc.
#include "core\kernel\support\NativeHandle.h"
#include <cassert>

#pragma warning(disable:4005) // Ignore redefined status values
#include <ntstatus.h>
#pragma warning(default:4005)

#define INITIALIZED_OBJECT_STRING(ObjectString, Value)                  \
    xbox::char_xt ObjectString##Buffer[] = Value;                       \
    xbox::OBJECT_STRING ObjectString = {                                \
    sizeof(Value) - sizeof(xbox::char_xt),                              \
    sizeof(Value),                                                      \
    ObjectString##Buffer                                                \
}

INITIALIZED_OBJECT_STRING(ObpDosDevicesString, "\\??");
INITIALIZED_OBJECT_STRING(ObpIoDevicesString, "\\Device");
INITIALIZED_OBJECT_STRING(ObpWin32NamedObjectsString, "\\Win32NamedObjects");

xbox::POBJECT_DIRECTORY ObpDosDevicesDirectoryObject;
xbox::POBJECT_DIRECTORY ObpWin32NamedObjectsDirectoryObject;
xbox::POBJECT_DIRECTORY ObpRootDirectoryObject;
xbox::POBJECT_DIRECTORY ObpIoDevicesDirectoryObject;
xbox::KEVENT xbox::ObpDefaultObject;

XBSYSAPI EXPORTNUM(245) xbox::OBJECT_HANDLE_TABLE xbox::ObpObjectHandleTable = {

};

xbox::PVOID ObpDosDevicesDriveLetterMap['Z' - 'A' + 1];

// This mutex is necessary to guard access to the global ob variables above
std::recursive_mutex g_ObMtx;

static xbox::KIRQL ObLock()
{
	xbox::KIRQL OldIrql = xbox::KeRaiseIrqlToDpcLevel();
	// Raising the irql to dpc level doesn't prevent thread switching, so acquire a lock
	g_ObMtx.lock();
	return OldIrql;
}

static void ObUnlock(xbox::KIRQL OldIrql)
{
	xbox::KfLowerIrql(OldIrql);
	g_ObMtx.unlock();
}

xbox::boolean_xt xbox::ObpCreatePermanentDirectoryObject(
	IN POBJECT_STRING DirectoryName OPTIONAL,
	OUT POBJECT_DIRECTORY *DirectoryObject
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(DirectoryName)
		LOG_FUNC_ARG(DirectoryObject)
		LOG_FUNC_END;

	OBJECT_ATTRIBUTES ObjectAttributes;
	X_InitializeObjectAttributes(&ObjectAttributes, DirectoryName, OBJ_PERMANENT, zeroptr);

	HANDLE Handle;
	NTSTATUS result = NtCreateDirectoryObject(&Handle, &ObjectAttributes);

	if (!X_NT_SUCCESS(result)) {
		RETURN(FALSE);
	}
	
	result = ObReferenceObjectByHandle(Handle, &ObDirectoryObjectType, (PVOID *)DirectoryObject);
	
	if (!X_NT_SUCCESS(result)) {
		RETURN(FALSE);
	}

	NtClose(Handle);

	RETURN(TRUE);
}

xbox::ntstatus_xt xbox::ObpReferenceObjectByName(
	IN HANDLE RootDirectoryHandle,
	IN POBJECT_STRING ObjectName,
	IN ulong_xt Attributes,
	IN POBJECT_TYPE ObjectType,
	IN OUT PVOID ParseContext OPTIONAL,
	OUT PVOID *ReturnedObject
)
{
	NTSTATUS result;
	*ReturnedObject = NULL;
	
	KIRQL OldIrql = ObLock();
	
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
				result = X_STATUS_INVALID_HANDLE;
				goto CleanupAndExit;
			}
		}

		if ((RemainingName.Length != 0) &&
			(RemainingName.Buffer[0] == OBJ_NAME_PATH_SEPARATOR)) {
			result = X_STATUS_OBJECT_NAME_INVALID;
			goto CleanupAndExit;
		}

		goto OpenRootDirectory;
	}

	FoundObject = ObpRootDirectoryObject;

	if ((RemainingName.Length == 0) ||
		(RemainingName.Buffer[0] != OBJ_NAME_PATH_SEPARATOR)) {
		result = X_STATUS_OBJECT_NAME_INVALID;
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
		{
			POBJECT_DIRECTORY Directory = (POBJECT_DIRECTORY)FoundObject;
			ObDissectName(RemainingName, &ElementName, &RemainingName);

			if (RemainingName.Length != 0) {
				if (RemainingName.Buffer[0] == OBJ_NAME_PATH_SEPARATOR) {
					result = X_STATUS_OBJECT_NAME_INVALID;
					goto CleanupAndExit;
				}
			}
			else {
				if (ObjectType == &ObSymbolicLinkObjectType) {
					ResolveSymbolicLink = FALSE;
				}
			}

			if (!ObpLookupElementNameInDirectory(Directory, &ElementName,
				ResolveSymbolicLink, &FoundObject)) {
				result = (RemainingName.Length != 0) ? STATUS_OBJECT_PATH_NOT_FOUND : X_STATUS_OBJECT_NAME_NOT_FOUND;
				goto CleanupAndExit;
			}
		}

OpenRootDirectory:
		POBJECT_HEADER ObjectHeader = OBJECT_TO_OBJECT_HEADER(FoundObject);

		if (RemainingName.Length == 0) {
			if (ObjectHeader->Type->ParseProcedure != NULL) {
				goto InvokeParseProcedure;
			}

			if ((ObjectType != NULL) && (ObjectType != ObjectHeader->Type)) {
				result = STATUS_OBJECT_TYPE_MISMATCH;
				goto CleanupAndExit;
			}

			ObjectHeader->PointerCount++;
			*ReturnedObject = FoundObject;
			result = X_STATUS_SUCCESS;
			goto CleanupAndExit;
		}

		if (ObjectHeader->Type != &ObDirectoryObjectType) {
			if (ObjectHeader->Type->ParseProcedure == NULL) {
				result = STATUS_OBJECT_PATH_NOT_FOUND;
				goto CleanupAndExit;
			}

InvokeParseProcedure:
			ObjectHeader->PointerCount++;
			ObUnlock(OldIrql);

			if (ObjectHeader->Type != &IoFileObjectType && (RemainingName.Buffer > ObjectName->Buffer)) {
				RemainingName.Buffer--;
				RemainingName.Length += sizeof(CHAR);
				RemainingName.MaximumLength = RemainingName.Length;
			}

			PVOID ParsedObject = NULL;
			result = ObjectHeader->Type->ParseProcedure(FoundObject, ObjectType, Attributes, ObjectName, &RemainingName, ParseContext, &ParsedObject);
			ObfDereferenceObject(FoundObject);

			if (X_NT_SUCCESS(result)) {
				if ((ObjectType == NULL) || (ObjectType == OBJECT_TO_OBJECT_HEADER(ParsedObject)->Type)) {
					*ReturnedObject = ParsedObject;
					result = X_STATUS_SUCCESS;
				} else {
					ObfDereferenceObject(ParsedObject);
					result = STATUS_OBJECT_TYPE_MISMATCH;
				}
			}

			return result;
		}
	}
CleanupAndExit:
	ObUnlock(OldIrql);
	return result;
}


static xbox::void_xt NTAPI ObpDeleteSymbolicLink(
	IN PVOID Object
)
{
	xbox::POBJECT_SYMBOLIC_LINK SymLinkObject = reinterpret_cast<xbox::POBJECT_SYMBOLIC_LINK>(Object);
	xbox::ObfDereferenceObject(SymLinkObject->LinkTargetObject);
}

xbox::boolean_xt xbox::ObInitSystem()
{
	ObpObjectHandleTable.HandleCount = 0;
	ObpObjectHandleTable.FirstFreeTableEntry = -1;
	ObpObjectHandleTable.NextHandleNeedingPool = 0;
	ObpObjectHandleTable.RootTable = NULL;

	std::memset(ObpDosDevicesDriveLetterMap, 0, sizeof(ObpDosDevicesDriveLetterMap));

	ObpDefaultObject.Header.Absolute = FALSE;
	ObpDefaultObject.Header.Inserted = FALSE;
	KeInitializeEvent(&ObpDefaultObject, SynchronizationEvent, TRUE);

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

xbox::boolean_xt xbox::ObpExtendObjectHandleTable()
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

xbox::HANDLE xbox::ObpCreateObjectHandle(xbox::PVOID Object)
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

xbox::void_xt xbox::ObDissectName(OBJECT_STRING Path, POBJECT_STRING FirstName, POBJECT_STRING RemainingName)
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

static inline xbox::HANDLE ObpGetHandleByObjectThenDereferenceInline(const xbox::PVOID Object, xbox::ntstatus_xt& result)
{
	xbox::HANDLE newHandle = NULL;

	if (X_NT_SUCCESS(result)) {
		xbox::KIRQL oldIrql = ObLock();

		newHandle = xbox::ObpCreateObjectHandle(Object);

		ObUnlock(oldIrql);

		if (newHandle == NULL) {
			xbox::ObfDereferenceObject(Object);
			result = X_STATUS_INSUFFICIENT_RESOURCES;
		}
	}

	return newHandle;
}

xbox::ntstatus_xt xbox::ObpResolveLinkTarget(
	POBJECT_STRING LinkTarget,
	PVOID* LinkTargetObject
)
{
	/* Verify RemainingName has string and contain path separator */
	OBJECT_STRING RemainingName = *LinkTarget;
	if (!RemainingName.Length || RemainingName.Buffer[0] != OBJ_NAME_PATH_SEPARATOR) {
		return X_STATUS_INVALID_PARAMETER;
	}

	KIRQL OldIrql = ObLock();

	POBJECT_DIRECTORY directory = ObpRootDirectoryObject;

	/* Perform search for object last known allocated */
	while (1) {
		OBJECT_STRING ElementName;
		ObDissectName(RemainingName, &ElementName, &RemainingName);

		/* Verify RemainingName does not have multiple backslashes */
		if (RemainingName.Length && RemainingName.Buffer[0] == OBJ_NAME_PATH_SEPARATOR) {
			break;
		}

		/* Search ElementName in the directory and verify it does not exist yet.*/
		PVOID FoundObject;
		if (!ObpLookupElementNameInDirectory(directory, &ElementName, TRUE, &FoundObject)) {
			break;
		}

		POBJECT_HEADER ObjectHeader = OBJECT_TO_OBJECT_HEADER(FoundObject);

		/* Verify we have no further search necessary with last found object. */
		if (!RemainingName.Length) {
			ObjectHeader->PointerCount++;
			*LinkTargetObject = FoundObject;
			ObUnlock(OldIrql);
			return X_STATUS_SUCCESS;
		}

		/* Keep searching until we find that is not a directory type of object. */
		if (ObjectHeader->Type != &ObDirectoryObjectType) {

			/* Check to see if object does not have parse procedure then continue the search again. */
			if (!ObjectHeader->Type->ParseProcedure) {
				break;
			}

			ObjectHeader->PointerCount++;
			ObUnlock(OldIrql);

			/* Perform parse procedure callback to find final object to be used. */
			ntstatus_xt result = ObjectHeader->Type->ParseProcedure(FoundObject, zeroptr, OBJ_CASE_INSENSITIVE, LinkTarget, &RemainingName, zeroptr, LinkTargetObject);
			if (result == X_STATUS_OBJECT_TYPE_MISMATCH) {
				/* If we have enter this stage, it is likely needed I/O manager in order to build parse context to resolve a file object. */
				HANDLE FileHandle;
				OBJECT_ATTRIBUTES ObjectAttributes;
				IO_STATUS_BLOCK IoStatusBlock;
				X_InitializeObjectAttributes(&ObjectAttributes, LinkTarget, OBJ_CASE_INSENSITIVE, zeroptr);
				result = NtOpenFile(&FileHandle, 0, &ObjectAttributes, &IoStatusBlock, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_DIRECTORY_FILE);
				/* Successfully found and keep target object alive.*/
				if (X_NT_SUCCESS(result)) {
					result = ObReferenceObjectByHandle(FileHandle, &IoFileObjectType, LinkTargetObject);
					NtClose(FileHandle);
				}
			}

			/* Return any result whether we found it or not. */
			ObfDereferenceObject(FoundObject);
			return result;
		}

		directory = (POBJECT_DIRECTORY)FoundObject;
	}

	ObUnlock(OldIrql);
	return X_STATUS_INVALID_PARAMETER;
}

// ******************************************************************
// * 0x00EF - ObCreateObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(239) xbox::ntstatus_xt NTAPI xbox::ObCreateObject
(
	IN POBJECT_TYPE ObjectType,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN ulong_xt ObjectBodySize,
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
		POBJECT_HEADER ObjectHeader = (POBJECT_HEADER)ObjectType->AllocateProcedure(offsetof(OBJECT_HEADER, Body) + ObjectBodySize, ObjectType->PoolTag);

		if (ObjectHeader == nullptr) {
			RETURN(X_STATUS_INSUFFICIENT_RESOURCES);
		}

		ObjectHeader->PointerCount = 1;
		ObjectHeader->HandleCount = 0;
		ObjectHeader->Type = ObjectType;
		ObjectHeader->Flags = 0;

		*Object = &ObjectHeader->Body;

		LOG_FUNC_BEGIN_ARG_RESULT
			LOG_FUNC_ARG_RESULT(Object)
		LOG_FUNC_END_ARG_RESULT;
		
		RETURN(X_STATUS_SUCCESS);
	}

	OBJECT_STRING RemainingName = *ObjectAttributes->ObjectName;
	OBJECT_STRING ElementName;
	ElementName.Buffer = NULL;
	ElementName.Length = 0;

	while (RemainingName.Length != 0) {
		ObDissectName(RemainingName, &ElementName, &RemainingName);
		if ((RemainingName.Length != 0) && (RemainingName.Buffer[0] == OBJ_NAME_PATH_SEPARATOR)) {
			RETURN(X_STATUS_OBJECT_NAME_INVALID);
		}
	}

	if (ElementName.Length == 0) {
		RETURN(X_STATUS_OBJECT_NAME_INVALID);
	}

	ObjectBodySize = ALIGN_UP(ObjectBodySize, ULONG);

	POBJECT_HEADER_NAME_INFO ObjectNameInfo = (POBJECT_HEADER_NAME_INFO)ObjectType->AllocateProcedure(
		sizeof(OBJECT_HEADER_NAME_INFO) + offsetof(OBJECT_HEADER, Body) +
		ObjectBodySize + ElementName.Length, ObjectType->PoolTag);

	if (ObjectNameInfo == NULL) {
		RETURN(X_STATUS_INSUFFICIENT_RESOURCES);
	}

	POBJECT_HEADER ObjectHeader = (POBJECT_HEADER)(ObjectNameInfo + 1);
	ObjectNameInfo->ChainLink = NULL;
	ObjectNameInfo->Directory = NULL;
	ObjectNameInfo->Name.Buffer = (PSTR)((PUCHAR)&ObjectHeader->Body + ObjectBodySize);
	ObjectNameInfo->Name.Length = ElementName.Length;
	ObjectNameInfo->Name.MaximumLength = ElementName.Length;

	RtlCopyMemory(ObjectNameInfo->Name.Buffer, ElementName.Buffer, ElementName.Length);

	ObjectHeader->PointerCount = 1;
	ObjectHeader->HandleCount = 0;
	ObjectHeader->Type = ObjectType;
	ObjectHeader->Flags = OB_FLAG_NAMED_OBJECT;

	*Object = &ObjectHeader->Body;

	LOG_FUNC_BEGIN_ARG_RESULT
		LOG_FUNC_ARG_RESULT(Object)
	LOG_FUNC_END_ARG_RESULT;

	RETURN(X_STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00F0 - ObDirectoryObjectType
// ******************************************************************
XBSYSAPI EXPORTNUM(240) xbox::OBJECT_TYPE xbox::ObDirectoryObjectType =
{
	xbox::ExAllocatePoolWithTag,
	xbox::ExFreePool,
	NULL,
	NULL,
	NULL,
	&xbox::ObpDefaultObject,
	'eriD' // = first four characters of "Directory" in reverse
};

xbox::PVOID xbox::ObpGetObjectHandleContents(HANDLE Handle)
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

xbox::ulong_xt FASTCALL xbox::ObpComputeHashIndex(
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

xbox::PVOID xbox::ObpGetObjectHandleReference(HANDLE Handle)
{
	PVOID *HandleContents;
	Handle = ObpMaskOffApplicationBits(Handle);
	KIRQL OldIrql = ObLock();

	if (HandleToUlong(Handle) < HandleToUlong(ObpObjectHandleTable.NextHandleNeedingPool)) {
		HandleContents = ObpGetHandleContentsPointer(Handle);
		PVOID Object = *HandleContents;

		if (Object != NULL && !ObpIsFreeHandleLink(Object)) {
			OBJECT_TO_OBJECT_HEADER(Object)->PointerCount++;
			ObUnlock(OldIrql);
			return Object;
		}
	}

	ObUnlock(OldIrql);
	return NULL;
}

xbox::boolean_xt xbox::ObpLookupElementNameInDirectory(
	IN POBJECT_DIRECTORY Directory,
	IN POBJECT_STRING ElementName,
	IN boolean_xt ResolveSymbolicLink,
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

xbox::PVOID xbox::ObpDestroyObjectHandle(
	IN HANDLE Handle
)
{
	Handle = ObpMaskOffApplicationBits(Handle);

	if (HandleToUlong(Handle) < HandleToUlong(ObpObjectHandleTable.NextHandleNeedingPool)) {
		PVOID *HandleContents = ObpGetHandleContentsPointer(Handle);
		PVOID Object = *HandleContents;

		if (Object != NULL && !ObpIsFreeHandleLink(Object)) {
			*HandleContents = (PVOID)ObpObjectHandleTable.FirstFreeTableEntry;
			ObpObjectHandleTable.FirstFreeTableEntry = ObpEncodeFreeHandleLink(Handle);
			ObpObjectHandleTable.HandleCount--;

			return Object;
		}
	}

	return NULL;
}

xbox::void_xt xbox::ObpDetachNamedObject(
	IN PVOID Object,
	IN KIRQL OldIrql
)
{
	POBJECT_HEADER_NAME_INFO ObjectHeaderNameInfo = OBJECT_TO_OBJECT_HEADER_NAME_INFO(Object);
	POBJECT_DIRECTORY Directory = ObjectHeaderNameInfo->Directory;

	if ((Directory == ObpDosDevicesDirectoryObject) &&
		(ObjectHeaderNameInfo->Name.Length == sizeof(char_xt) * 2) &&
		(ObjectHeaderNameInfo->Name.Buffer[1] == (char_xt)':')) {

		char_xt DriveLetter = ObjectHeaderNameInfo->Name.Buffer[0];
		if (DriveLetter >= 'a' && DriveLetter <= 'z') {
			ObpDosDevicesDriveLetterMap[DriveLetter - 'a'] = NULL;
		}
		else if (DriveLetter >= 'A' && DriveLetter <= 'Z') {
			ObpDosDevicesDriveLetterMap[DriveLetter - 'A'] = NULL;
		}
	}

	ulong_xt HashIndex = ObpComputeHashIndex(&ObjectHeaderNameInfo->Name);
	POBJECT_HEADER_NAME_INFO LastObjectHeaderNameInfo = NULL;
	POBJECT_HEADER_NAME_INFO CurrentObjectHeaderNameInfo = Directory->HashBuckets[HashIndex];

	while (CurrentObjectHeaderNameInfo != ObjectHeaderNameInfo) {
		LastObjectHeaderNameInfo = CurrentObjectHeaderNameInfo;
		CurrentObjectHeaderNameInfo = CurrentObjectHeaderNameInfo->ChainLink;
	}

	if (LastObjectHeaderNameInfo == NULL) {
		Directory->HashBuckets[HashIndex] = CurrentObjectHeaderNameInfo->ChainLink;
	}
	else {
		LastObjectHeaderNameInfo->ChainLink = CurrentObjectHeaderNameInfo->ChainLink;
	}

	ObjectHeaderNameInfo->ChainLink = NULL;
	ObjectHeaderNameInfo->Directory = NULL;

	ObUnlock(OldIrql);

	ObfDereferenceObject(Directory);
	ObfDereferenceObject(Object);
}

xbox::ntstatus_xt xbox::ObpClose(
	IN HANDLE Handle
)
{
	KIRQL OldIrql = ObLock();

	PVOID Object = ObpDestroyObjectHandle(Handle);
	if (Object != NULL) {
		POBJECT_HEADER ObjectHeader = OBJECT_TO_OBJECT_HEADER(Object);
		ulong_xt HandleCount = ObjectHeader->HandleCount;
		ObjectHeader->HandleCount--;

		if (ObjectHeader->Type->CloseProcedure != NULL) {
			ObUnlock(OldIrql);
			ObjectHeader->Type->CloseProcedure(Object, HandleCount);
			OldIrql = ObLock();
		}

		if ((ObjectHeader->HandleCount == 0) &&
			ObpIsFlagSet(ObjectHeader->Flags, OB_FLAG_ATTACHED_OBJECT) &&
			ObpIsFlagClear(ObjectHeader->Flags, OB_FLAG_PERMANENT_OBJECT)) {
			ObpDetachNamedObject(Object, OldIrql);
		}
		else {
			ObUnlock(OldIrql);
		}

		ObfDereferenceObject(Object);
		return X_STATUS_SUCCESS;
	}
	else {
		ObUnlock(OldIrql);
		return X_STATUS_INVALID_HANDLE;
	}
}

// ******************************************************************
// * 0x00F1 - ObInsertObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(241) xbox::ntstatus_xt NTAPI xbox::ObInsertObject
(
	IN PVOID Object,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN ulong_xt ObjectPointerBias,
	OUT PHANDLE ReturnedHandle
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Object)
		LOG_FUNC_ARG(ObjectAttributes)
		LOG_FUNC_ARG(ObjectPointerBias)
		LOG_FUNC_ARG_OUT(ReturnedHandle)
		LOG_FUNC_END;

	NTSTATUS result;
	POBJECT_DIRECTORY Directory;

	KIRQL OldIrql = ObLock();

	HANDLE Handle = NULL;
	PVOID InsertObject = Object;
	if (ObjectAttributes != NULL && ObjectAttributes->ObjectName != NULL) {
		OBJECT_STRING RemainingName = *ObjectAttributes->ObjectName;
		HANDLE RootDirectoryHandle = ObjectAttributes->RootDirectory;

		if (RootDirectoryHandle != NULL) {
			if (RemainingName.Buffer[0] == OBJ_NAME_PATH_SEPARATOR) {
				result = STATUS_OBJECT_NAME_INVALID;
				goto CleanupAndExit;
			}

			if (RootDirectoryHandle == ObDosDevicesDirectory()) {
				Directory = ObpDosDevicesDirectoryObject;
			} else if (RootDirectoryHandle == ObWin32NamedObjectsDirectory()) {
				Directory = ObpWin32NamedObjectsDirectoryObject;
			} else {
				Directory = (POBJECT_DIRECTORY)ObpGetObjectHandleContents(RootDirectoryHandle);

				if (Directory == NULL) {
					result = STATUS_INVALID_HANDLE;
					goto CleanupAndExit;
				}

				if (OBJECT_TO_OBJECT_HEADER(Directory)->Type != &ObDirectoryObjectType) {
					result = STATUS_OBJECT_TYPE_MISMATCH;
					goto CleanupAndExit;
				}
			}
		} else {
			if (RemainingName.Buffer[0] != OBJ_NAME_PATH_SEPARATOR) {
				result = STATUS_OBJECT_NAME_INVALID;
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
							result = STATUS_OBJECT_TYPE_MISMATCH;
							goto CleanupAndExit;
						}
					} else {
						result = X_STATUS_OBJECT_NAME_COLLISION;
						goto CleanupAndExit;
					}
				}

				if (OBJECT_TO_OBJECT_HEADER(FoundObject)->Type !=
					&ObDirectoryObjectType) {
					result = STATUS_OBJECT_PATH_NOT_FOUND;
					goto CleanupAndExit;
				}

				Directory = (POBJECT_DIRECTORY)FoundObject;
			} else {
				if (RemainingName.Length != 0) {
					result = STATUS_OBJECT_PATH_NOT_FOUND;
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
		result = X_STATUS_INSUFFICIENT_RESOURCES;
		goto CleanupAndExit;
	}

	{
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
				}
				else if (DriveLetter >= 'A' && DriveLetter <= 'Z') {
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
	}

	result = (Object == InsertObject) ? X_STATUS_SUCCESS : STATUS_OBJECT_NAME_EXISTS;

CleanupAndExit:
	ObUnlock(OldIrql);
	ObfDereferenceObject(Object);
	*ReturnedHandle = Handle;

	LOG_FUNC_BEGIN_ARG_RESULT
		LOG_FUNC_ARG_RESULT(ReturnedHandle)
	LOG_FUNC_END_ARG_RESULT;

	RETURN(result);
}

// ******************************************************************
// * 0x00F2 - ObMakeTemporaryObject()
// ******************************************************************
// Source: ReactOS, using modified ObpSetPermanentObject function
// Plus reverse engineered reveal permanent flag is force cleared.
XBSYSAPI EXPORTNUM(242) xbox::void_xt NTAPI xbox::ObMakeTemporaryObject
(
	IN PVOID Object
)
{
	LOG_FUNC_ONE_ARG(Object);

	/* Acquire lock */
	KIRQL OldIrql = ObLock();

	/* Get the header */
	POBJECT_HEADER ObjectHeader = OBJECT_TO_OBJECT_HEADER(Object);

	/* Force remove the flag */
	ObjectHeader->Flags &= ~OB_FLAG_PERMANENT_OBJECT;

	/* Check see if object is still attached */
	if (ObjectHeader->HandleCount == 0 && ObpIsFlagSet(ObjectHeader->Flags, OB_FLAG_ATTACHED_OBJECT)) {
		ObpDetachNamedObject(Object, OldIrql);
	}
	else {
		/* Release the lock */
		ObUnlock(OldIrql);
	}
}

// ******************************************************************
// * 0x00F3 - ObOpenObjectByName()
// ******************************************************************
XBSYSAPI EXPORTNUM(243) xbox::ntstatus_xt NTAPI xbox::ObOpenObjectByName
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

	PVOID Object;
	ntstatus_xt result = ObpReferenceObjectByName(ObjectAttributes->RootDirectory, ObjectAttributes->ObjectName,
		ObjectAttributes->Attributes, ObjectType, ParseContext, &Object);

	*Handle = ObpGetHandleByObjectThenDereferenceInline(Object, result);

	LOG_FUNC_BEGIN_ARG_RESULT
		LOG_FUNC_ARG_RESULT(Handle)
	LOG_FUNC_END_ARG_RESULT;

	RETURN(result);
}

// ******************************************************************
// * 0x00F4 - ObOpenObjectByPointer()
// ******************************************************************
XBSYSAPI EXPORTNUM(244) xbox::ntstatus_xt NTAPI xbox::ObOpenObjectByPointer
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

	ntstatus_xt result = ObReferenceObjectByPointer(Object, ObjectType);
	*Handle = ObpGetHandleByObjectThenDereferenceInline(Object, result);

	LOG_FUNC_BEGIN_ARG_RESULT
		LOG_FUNC_ARG_RESULT(Handle)
	LOG_FUNC_END_ARG_RESULT;

	RETURN(result);
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
XBSYSAPI EXPORTNUM(246) xbox::ntstatus_xt NTAPI xbox::ObReferenceObjectByHandle
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

	NTSTATUS result;
	PVOID Object;
	POBJECT_HEADER ObjectHeader;

	// Check if Handle contain special handle for current thread.
	if (Handle == NtCurrentThread()) {
		/* Check if this is the right object type */
		if ((ObjectType == &PsThreadObjectType) || !ObjectType) {
			/* Get the current process */
			Object = PspGetCurrentThread();

			/* Reference ourselves */
			ObjectHeader = OBJECT_TO_OBJECT_HEADER(Object);
			InterlockedIncrement((::PLONG)(&ObjectHeader->PointerCount));

			/* Return the pointer */
			*ReturnedObject = Object;
			goto ObjectObtained;
		}
		else {
			/* The caller used this special handle value with a non-process type */
			result = STATUS_OBJECT_TYPE_MISMATCH;
		}
	}
	else {
		/* Get the object */
		Object = ObpGetObjectHandleReference(Handle);

		// Check if object is null pointer
		if (Object) {
			/* Validate the type*/
			ObjectHeader = OBJECT_TO_OBJECT_HEADER(Object);

			// Verify if object type do match with found object or any if null object type.
			if ((ObjectType == ObjectHeader->Type) || !ObjectType) {
				*ReturnedObject = Object;
				goto ObjectObtained;
			}
			else {
				/* Invalid object type */
				ObfDereferenceObject(Object);
				result = STATUS_OBJECT_TYPE_MISMATCH;
			}
		}
		else {
			result = STATUS_INVALID_HANDLE;
		}
	}

	/* Assume failure */
	*ReturnedObject = NULL;

	/* Return the failure status */
	RETURN(result);

	ObjectObtained:
	LOG_FUNC_BEGIN_ARG_RESULT
		LOG_FUNC_ARG_RESULT(ReturnedObject)
	LOG_FUNC_END_ARG_RESULT;

	/* Return the status */
	RETURN(X_STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00F7 - ObReferenceObjectByName()
// ******************************************************************
XBSYSAPI EXPORTNUM(247) xbox::ntstatus_xt NTAPI xbox::ObReferenceObjectByName
(
	IN POBJECT_STRING ObjectName,
	IN ulong_xt Attributes,
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

	ntstatus_xt result = ObpReferenceObjectByName(NULL, ObjectName, Attributes, ObjectType, ParseContext, Object);

	LOG_FUNC_BEGIN_ARG_RESULT
		LOG_FUNC_ARG_RESULT(Object)
	LOG_FUNC_END_ARG_RESULT;

	RETURN(result);
}

// ******************************************************************
// * 0x00F8 - ObReferenceObjectByPointer()
// ******************************************************************
XBSYSAPI EXPORTNUM(248) xbox::ntstatus_xt NTAPI xbox::ObReferenceObjectByPointer
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
		InterlockedIncrement((::PLONG)(&ObjectHeader->PointerCount));
		RETURN(X_STATUS_SUCCESS);
	} 
	
	RETURN(STATUS_OBJECT_TYPE_MISMATCH);
}

// ******************************************************************
// * 0x00F9 - ObSymbolicLinkObjectType
// ******************************************************************
XBSYSAPI EXPORTNUM(249) xbox::OBJECT_TYPE xbox::ObSymbolicLinkObjectType =
{
	xbox::ExAllocatePoolWithTag,
	xbox::ExFreePool,
	NULL,
	ObpDeleteSymbolicLink,
	NULL,
	&xbox::ObpDefaultObject,
	'bmyS' // = first four characters of "SymbolicLink" in reverse
};

// ******************************************************************
// * 0x00FA - ObfDereferenceObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(250) xbox::void_xt FASTCALL xbox::ObfDereferenceObject
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
	
	if (InterlockedDecrement((::PLONG)(&ObjectHeader->PointerCount)) == 0) {
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
		RemoveXboxObject(Object);
	}
}

// ******************************************************************
// * 0x00FB - ObfReferenceObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(251) xbox::void_xt FASTCALL xbox::ObfReferenceObject
(
	IN PVOID Object
)
{
	LOG_FUNC_ONE_ARG_OUT(Object);
	InterlockedIncrement((::PLONG)(&OBJECT_TO_OBJECT_HEADER(Object)->PointerCount));
}
