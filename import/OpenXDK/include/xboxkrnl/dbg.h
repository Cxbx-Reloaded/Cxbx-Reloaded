// ******************************************************************
// *
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : dbg.h
// *
// * note : XBox Kernel *Debug* Declarations
// *
// ******************************************************************
#ifndef XBOXKRNL_DBG_H
#define XBOXKRNL_DBG_H

XBSYSAPI VOID *DbgBreakPoint;
XBSYSAPI VOID *DbgBreakPointWithStatus;
XBSYSAPI VOID *DbgLoadImageSymbols;
XBSYSAPI EXPORTNUM(8) ULONG _cdecl DbgPrint
(
	PCHAR  Format, ...
);
XBSYSAPI VOID *DbgPrompt;
XBSYSAPI VOID *DbgUnLoadImageSymbols;

#endif




