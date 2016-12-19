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

XBSYSAPI VOID *ObCreateObject;

XBSYSAPI EXPORTNUM(240) OBJECT_TYPE ObDirectoryObjectType;

XBSYSAPI VOID *ObInsertObject;
XBSYSAPI VOID *ObMakeTemporaryObject;

XBSYSAPI EXPORTNUM(243) NTSTATUS NTAPI ObOpenObjectByName
(
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	IN POBJECT_TYPE ObjectType,
	IN OUT PVOID ParseContext OPTIONAL,
	OUT PHANDLE Handle
);

XBSYSAPI VOID *ObOpenObjectByPointer;
XBSYSAPI EXPORTNUM(245) DWORD ObpObjectHandleTable[1];

XBSYSAPI EXPORTNUM(246) NTSTATUS NTAPI ObReferenceObjectByHandle
(
    IN HANDLE Handle,
    IN POBJECT_TYPE ObjectType OPTIONAL,
    OUT PVOID *ReturnedObject
);


XBSYSAPI VOID *ObReferenceObjectByName;
XBSYSAPI VOID *ObReferenceObjectByPointer;
XBSYSAPI EXPORTNUM(249) OBJECT_TYPE ObSymbolicLinkObjectType;

XBSYSAPI EXPORTNUM(250) VOID FASTCALL ObfDereferenceObject
(
	IN PVOID Object
);

XBSYSAPI VOID *ObfReferenceObject;

#endif


