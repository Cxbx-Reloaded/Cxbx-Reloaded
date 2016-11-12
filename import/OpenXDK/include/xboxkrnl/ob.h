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

XBSYSAPI EXPORTNUM(246) BOOLEAN NTAPI ObReferenceObjectByHandle
( unsigned int a,
  unsigned int b,
  unsigned int c
);


XBSYSAPI VOID *ObReferenceObjectByName;
XBSYSAPI VOID *ObReferenceObjectByPointer;
XBSYSAPI EXPORTNUM(249) POBJECT_TYPE ObSymbolicLinkObjectType;
XBSYSAPI VOID *ObfDereferenceObject;
XBSYSAPI VOID *ObfReferenceObject;

#endif


