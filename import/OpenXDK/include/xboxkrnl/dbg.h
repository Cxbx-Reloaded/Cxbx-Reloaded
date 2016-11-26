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

XBSYSAPI EXPORTNUM(5) VOID NTAPI DbgBreakPoint();
XBSYSAPI EXPORTNUM(6) VOID NTAPI DbgBreakPointWithStatus
(
	IN ULONG Status
);
XBSYSAPI EXPORTNUM(7) NTSTATUS NTAPI DbgLoadImageSymbols
(
	IN PANSI_STRING Name,
	IN PVOID Base,
	IN ULONG_PTR ProcessId
);
XBSYSAPI EXPORTNUM(8) ULONG _cdecl DbgPrint
(
	PCHAR  Format, ...
);
XBSYSAPI EXPORTNUM(10) ULONG NTAPI DbgPrompt
(
	IN PCCH Prompt,
	OUT PCH Response,
	IN ULONG MaximumResponseLength
);
XBSYSAPI EXPORTNUM(11) VOID NTAPI DbgUnLoadImageSymbols
(
	IN PANSI_STRING Name,
	IN PVOID Base,
	IN ULONG_PTR ProcessId
);

#endif




