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
XBSYSAPI EXPORTNUM(240) POBJECT_TYPE ObDirectoryObjectType;
XBSYSAPI VOID *ObInsertObject;
XBSYSAPI VOID *ObMakeTemporaryObject;
XBSYSAPI VOID *ObOpenObjectByName;
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
XBSYSAPI EXPORTNUM(249) POBJECT_TYPE ObSymbolicLinkObjectType;

XBSYSAPI EXPORTNUM(250) VOID __fastcall ObfDereferenceObject
(
	IN PVOID Object
);

XBSYSAPI VOID *ObfReferenceObject;

#endif


