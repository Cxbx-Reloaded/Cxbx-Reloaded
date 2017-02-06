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
XBSYSAPI EXPORTNUM(242) VOID NTAPI ObMakeTemporaryObject
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

// ******************************************************************
// * 0x00F5 - ObpObjectHandleTable
// ******************************************************************
XBSYSAPI EXPORTNUM(245) DWORD ObpObjectHandleTable[1];

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
XBSYSAPI EXPORTNUM(250) VOID FASTCALL ObfDereferenceObject
(
	IN PVOID Object
);

// ******************************************************************
// * 0x00FB - ObfReferenceObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(251) VOID FASTCALL ObfReferenceObject
(
	IN PVOID Object
);


#endif


