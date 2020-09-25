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

#define OBJ_NAME_PATH_SEPARATOR ((char_t)L'\\')

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

#define ObDosDevicesDirectory()         ((HANDLE)-3)
#define ObWin32NamedObjectsDirectory()  ((HANDLE)-4)

#define ObpIsFlagSet(flagset, flag) (((flagset) & (flag)) != 0)
#define ObpIsFlagClear(flagset, flag) (((flagset) & (flag)) == 0)
#define ObpEncodeFreeHandleLink(Link) (((ULONG_PTR)(Link)) | 1)
#define ObpDecodeFreeHandleLink(Link) (((ULONG_PTR)(Link)) & (~1))
#define ObpIsFreeHandleLink(Link) (((ULONG_PTR)(Link)) & 1)
#define ObpGetTableByteOffsetFromHandle(Handle) (HandleToUlong(Handle) & (OB_HANDLES_PER_TABLE * sizeof(PVOID) - 1))
#define ObpGetTableFromHandle(Handle) ObpObjectHandleTable.RootTable[HandleToUlong(Handle) >> (OB_HANDLES_PER_TABLE_SHIFT + 2)]
#define ObpGetHandleContentsPointer(Handle) ((PVOID*)((PUCHAR)ObpGetTableFromHandle(Handle) + ObpGetTableByteOffsetFromHandle(Handle)))
#define ObpMaskOffApplicationBits(Handle) ((HANDLE)(((ULONG_PTR)(Handle)) & ~(sizeof(ULONG) - 1)))

#define OB_FLAG_NAMED_OBJECT            0x01
#define OB_FLAG_PERMANENT_OBJECT        0x02
#define OB_FLAG_ATTACHED_OBJECT         0x04

#define OBJECT_TO_OBJECT_HEADER(Object) CONTAINING_RECORD(Object, OBJECT_HEADER, Body)
#define OBJECT_TO_OBJECT_HEADER_NAME_INFO(Object) ((POBJECT_HEADER_NAME_INFO)OBJECT_TO_OBJECT_HEADER(Object) - 1)
#define OBJECT_HEADER_NAME_INFO_TO_OBJECT_HEADER(ObjectHeaderNameInfo) ((POBJECT_HEADER)((POBJECT_HEADER_NAME_INFO)(ObjectHeaderNameInfo)+1))
#define OBJECT_HEADER_TO_OBJECT_HEADER_NAME_INFO(ObjectHeader) ((POBJECT_HEADER_NAME_INFO)(ObjectHeader)-1)
#define OBJECT_HEADER_NAME_INFO_TO_OBJECT(ObjectHeaderNameInfo) (&OBJECT_HEADER_NAME_INFO_TO_OBJECT_HEADER(ObjectHeaderNameInfo)->Body)

HANDLE ObpCreateObjectHandle(PVOID Object);
boolean_t ObpCreatePermanentDirectoryObject(
	IN POBJECT_STRING DirectoryName OPTIONAL,
	OUT POBJECT_DIRECTORY *DirectoryObject
);

NTSTATUS ObpReferenceObjectByName(
	IN HANDLE RootDirectoryHandle,
	IN POBJECT_STRING ObjectName,
	IN ULONG Attributes,
	IN POBJECT_TYPE ObjectType,
	IN OUT PVOID ParseContext OPTIONAL,
	OUT PVOID *ReturnedObject
);

#define XB_InitializeObjectAttributes(p, n, a, r, s){\
	(p)->RootDirectory = r;   \
	(p)->Attributes = a;      \
	(p)->ObjectName = n;      \
}

boolean_t ObInitSystem();
boolean_t ObpExtendObjectHandleTable();
void_t ObDissectName(OBJECT_STRING Path, POBJECT_STRING FirstName, POBJECT_STRING RemainingName);
PVOID ObpGetObjectHandleContents(HANDLE Handle);
PVOID ObpGetObjectHandleReference(HANDLE Handle);
ULONG FASTCALL ObpComputeHashIndex(IN POBJECT_STRING ElementName);

boolean_t ObpLookupElementNameInDirectory(
	IN POBJECT_DIRECTORY Directory,
	IN POBJECT_STRING ElementName,
	IN boolean_t ResolveSymbolicLink,
	OUT PVOID *ReturnedObject
);

// ******************************************************************
// * 0x00EF - ObCreateObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(239) NTSTATUS NTAPI ObCreateObject
(
	IN POBJECT_TYPE ObjectType,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN ULONG ObjectBodySize,
	OUT PVOID *Object
);

// ******************************************************************
// * 0x00F0 - ObDirectoryObjectType
// ******************************************************************
XBSYSAPI EXPORTNUM(240) OBJECT_TYPE ObDirectoryObjectType;

// ******************************************************************
// * 0x00F1 - ObInsertObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(241) NTSTATUS NTAPI ObInsertObject
(
	IN PVOID Object,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN ULONG ObjectPointerBias,
	OUT PHANDLE Handle
);

// ******************************************************************
// * 0x00F2 - ObMakeTemporaryObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(242) void_t NTAPI ObMakeTemporaryObject
(
	IN PVOID Object
);

// ******************************************************************
// * 0x00F3 - ObOpenObjectByName()
// ******************************************************************
XBSYSAPI EXPORTNUM(243) NTSTATUS NTAPI ObOpenObjectByName
(
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	IN POBJECT_TYPE ObjectType,
	IN OUT PVOID ParseContext OPTIONAL,
	OUT PHANDLE Handle
);

// ******************************************************************
// * 0x00F4 - ObOpenObjectByPointer()
// ******************************************************************
XBSYSAPI EXPORTNUM(244) NTSTATUS NTAPI ObOpenObjectByPointer
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
	LONG HandleCount;
	LONG_PTR FirstFreeTableEntry;
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
XBSYSAPI EXPORTNUM(246) NTSTATUS NTAPI ObReferenceObjectByHandle
(
    IN HANDLE Handle,
    IN POBJECT_TYPE ObjectType OPTIONAL,
    OUT PVOID *ReturnedObject
);

// ******************************************************************
// * 0x00F7 - ObReferenceObjectByName()
// ******************************************************************
XBSYSAPI EXPORTNUM(247) NTSTATUS NTAPI ObReferenceObjectByName
(
	IN POBJECT_STRING ObjectName,
	IN ULONG Attributes,
	IN POBJECT_TYPE ObjectType,
	IN OUT PVOID ParseContext OPTIONAL,
	OUT PVOID *Object
);

// ******************************************************************
// * 0x00F8 - ObReferenceObjectByPointer()
// ******************************************************************
XBSYSAPI EXPORTNUM(248) NTSTATUS NTAPI ObReferenceObjectByPointer
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
XBSYSAPI EXPORTNUM(250) void_t FASTCALL ObfDereferenceObject
(
	IN PVOID Object
);

// ******************************************************************
// * 0x00FB - ObfReferenceObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(251) void_t FASTCALL ObfReferenceObject
(
	IN PVOID Object
);


#endif


