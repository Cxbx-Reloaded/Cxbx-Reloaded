// ******************************************************************
// *
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : ob.h
// *
// * note : XBox Kernel *Object Manager* Declarations
// *
// ******************************************************************
#ifndef XBOXKRNL_OB_H
#define XBOXKRNL_OB_H

#include "types.h"

namespace xbox
{

#define OBJ_NAME_PATH_SEPARATOR ((char_xt)L'\\')

#define OB_NUMBER_HASH_BUCKETS          11
typedef struct _OBJECT_DIRECTORY {
	struct _OBJECT_HEADER_NAME_INFO *HashBuckets[OB_NUMBER_HASH_BUCKETS];
} OBJECT_DIRECTORY, *POBJECT_DIRECTORY;

typedef struct _OBJECT_SYMBOLIC_LINK {
	PVOID LinkTargetObject;
	OBJECT_STRING LinkTarget;
} OBJECT_SYMBOLIC_LINK, *POBJECT_SYMBOLIC_LINK;

typedef struct _OBJECT_HEADER_NAME_INFO {
	struct _OBJECT_HEADER_NAME_INFO *ChainLink;
	struct _OBJECT_DIRECTORY *Directory;
	OBJECT_STRING Name;
} OBJECT_HEADER_NAME_INFO, *POBJECT_HEADER_NAME_INFO;

inline constexpr dword_xt obj_case_insensitive = 0x40;

#define ObDosDevicesDirectory()         ((HANDLE)-3)
#define ObWin32NamedObjectsDirectory()  ((HANDLE)-4)

#define ObpIsFlagSet(flagset, flag) (((flagset) & (flag)) != 0)
#define ObpIsFlagClear(flagset, flag) (((flagset) & (flag)) == 0)
#define ObpEncodeFreeHandleLink(Link) (((ulong_ptr_xt)(Link)) | 1)
#define ObpDecodeFreeHandleLink(Link) (((ulong_ptr_xt)(Link)) & (~1))
#define ObpIsFreeHandleLink(Link) (((ulong_ptr_xt)(Link)) & 1)
#define ObpGetTableByteOffsetFromHandle(Handle) (HandleToUlong(Handle) & (OB_HANDLES_PER_TABLE * sizeof(PVOID) - 1))
#define ObpGetTableFromHandle(Handle) ObpObjectHandleTable.RootTable[HandleToUlong(Handle) >> (OB_HANDLES_PER_TABLE_SHIFT + 2)]
#define ObpGetHandleContentsPointer(Handle) ((PVOID*)((PUCHAR)ObpGetTableFromHandle(Handle) + ObpGetTableByteOffsetFromHandle(Handle)))
#define ObpMaskOffApplicationBits(Handle) ((HANDLE)(((ulong_ptr_xt)(Handle)) & ~(sizeof(ulong_xt) - 1)))

#define OB_FLAG_NAMED_OBJECT            0x01
#define OB_FLAG_PERMANENT_OBJECT        0x02
#define OB_FLAG_ATTACHED_OBJECT         0x04

#define OBJECT_TO_OBJECT_HEADER(Object) CONTAINING_RECORD(Object, OBJECT_HEADER, Body)
#define OBJECT_TO_OBJECT_HEADER_NAME_INFO(Object) ((POBJECT_HEADER_NAME_INFO)OBJECT_TO_OBJECT_HEADER(Object) - 1)
#define OBJECT_HEADER_NAME_INFO_TO_OBJECT_HEADER(ObjectHeaderNameInfo) ((POBJECT_HEADER)((POBJECT_HEADER_NAME_INFO)(ObjectHeaderNameInfo)+1))
#define OBJECT_HEADER_TO_OBJECT_HEADER_NAME_INFO(ObjectHeader) ((POBJECT_HEADER_NAME_INFO)(ObjectHeader)-1)
#define OBJECT_HEADER_NAME_INFO_TO_OBJECT(ObjectHeaderNameInfo) (&OBJECT_HEADER_NAME_INFO_TO_OBJECT_HEADER(ObjectHeaderNameInfo)->Body)

HANDLE ObpCreateObjectHandle(PVOID Object);
boolean_xt ObpCreatePermanentDirectoryObject(
	IN POBJECT_STRING DirectoryName OPTIONAL,
	OUT POBJECT_DIRECTORY *DirectoryObject
);

ntstatus_xt ObpReferenceObjectByName(
	IN HANDLE RootDirectoryHandle,
	IN POBJECT_STRING ObjectName,
	IN ulong_xt Attributes,
	IN POBJECT_TYPE ObjectType,
	IN OUT PVOID ParseContext OPTIONAL,
	OUT PVOID *ReturnedObject
);

#define XB_InitializeObjectAttributes(p, n, a, r, s){\
	(p)->RootDirectory = r;   \
	(p)->Attributes = a;      \
	(p)->ObjectName = n;      \
}

boolean_xt ObInitSystem();
boolean_xt ObpExtendObjectHandleTable();
void_xt ObDissectName(OBJECT_STRING Path, POBJECT_STRING FirstName, POBJECT_STRING RemainingName);
PVOID ObpGetObjectHandleContents(HANDLE Handle);
PVOID ObpGetObjectHandleReference(HANDLE Handle);
ulong_xt FASTCALL ObpComputeHashIndex(IN POBJECT_STRING ElementName);

boolean_xt ObpLookupElementNameInDirectory(
	IN POBJECT_DIRECTORY Directory,
	IN POBJECT_STRING ElementName,
	IN boolean_xt ResolveSymbolicLink,
	OUT PVOID *ReturnedObject
);

// ******************************************************************
// * 0x00EF - ObCreateObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(239) ntstatus_xt NTAPI ObCreateObject
(
	IN POBJECT_TYPE ObjectType,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN ulong_xt ObjectBodySize,
	OUT PVOID *Object
);

// ******************************************************************
// * 0x00F0 - ObDirectoryObjectType
// ******************************************************************
XBSYSAPI EXPORTNUM(240) OBJECT_TYPE ObDirectoryObjectType;

// ******************************************************************
// * 0x00F1 - ObInsertObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(241) ntstatus_xt NTAPI ObInsertObject
(
	IN PVOID Object,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN ulong_xt ObjectPointerBias,
	OUT PHANDLE Handle
);

// ******************************************************************
// * 0x00F2 - ObMakeTemporaryObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(242) void_xt NTAPI ObMakeTemporaryObject
(
	IN PVOID Object
);

// ******************************************************************
// * 0x00F3 - ObOpenObjectByName()
// ******************************************************************
XBSYSAPI EXPORTNUM(243) ntstatus_xt NTAPI ObOpenObjectByName
(
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	IN POBJECT_TYPE ObjectType,
	IN OUT PVOID ParseContext OPTIONAL,
	OUT PHANDLE Handle
);

// ******************************************************************
// * 0x00F4 - ObOpenObjectByPointer()
// ******************************************************************
XBSYSAPI EXPORTNUM(244) ntstatus_xt NTAPI ObOpenObjectByPointer
(
	IN PVOID Object,
	IN POBJECT_TYPE ObjectType,
	OUT PHANDLE Handle
);

#define OB_HANDLES_PER_TABLE_SHIFT      6
#define OB_HANDLES_PER_TABLE            (1 << OB_HANDLES_PER_TABLE_SHIFT)
#define OB_TABLES_PER_SEGMENT           8
#define OB_HANDLES_PER_SEGMENT          (OB_TABLES_PER_SEGMENT * OB_HANDLES_PER_TABLE)

typedef struct _OBJECT_HANDLE_TABLE {
	long_xt HandleCount;
	long_ptr_xt FirstFreeTableEntry;
	HANDLE NextHandleNeedingPool;
	PVOID **RootTable;
	PVOID *BuiltinRootTable[OB_TABLES_PER_SEGMENT];
} OBJECT_HANDLE_TABLE, *POBJECT_HANDLE_TABLE;

// ******************************************************************
// * 0x00F5 - ObpObjectHandleTable
// ******************************************************************
XBSYSAPI EXPORTNUM(245) OBJECT_HANDLE_TABLE ObpObjectHandleTable;

// ******************************************************************
// * 0x00F6 - ObReferenceObjectByHandle()
// ******************************************************************
XBSYSAPI EXPORTNUM(246) ntstatus_xt NTAPI ObReferenceObjectByHandle
(
    IN HANDLE Handle,
    IN POBJECT_TYPE ObjectType OPTIONAL,
    OUT PVOID *ReturnedObject
);

// ******************************************************************
// * 0x00F7 - ObReferenceObjectByName()
// ******************************************************************
XBSYSAPI EXPORTNUM(247) ntstatus_xt NTAPI ObReferenceObjectByName
(
	IN POBJECT_STRING ObjectName,
	IN ulong_xt Attributes,
	IN POBJECT_TYPE ObjectType,
	IN OUT PVOID ParseContext OPTIONAL,
	OUT PVOID *Object
);

// ******************************************************************
// * 0x00F8 - ObReferenceObjectByPointer()
// ******************************************************************
XBSYSAPI EXPORTNUM(248) ntstatus_xt NTAPI ObReferenceObjectByPointer
(
	IN PVOID Object,
	IN POBJECT_TYPE ObjectType
);

// ******************************************************************
// * 0x00F9 - ObSymbolicLinkObjectType
// ******************************************************************
XBSYSAPI EXPORTNUM(249) OBJECT_TYPE ObSymbolicLinkObjectType;

// ******************************************************************
// * 0x00FA - ObfDereferenceObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(250) void_xt FASTCALL ObfDereferenceObject
(
	IN PVOID Object
);

// ******************************************************************
// * 0x00FB - ObfReferenceObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(251) void_xt FASTCALL ObfReferenceObject
(
	IN PVOID Object
);

}

#endif


