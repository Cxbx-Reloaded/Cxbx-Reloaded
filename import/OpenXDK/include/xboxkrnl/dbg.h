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

#include "xboxkrnl/xboxkrnl_types.h"

namespace xbox
{

XBSYSAPI EXPORTNUM(5) void_t NTAPI DbgBreakPoint();
XBSYSAPI EXPORTNUM(6) void_t NTAPI DbgBreakPointWithStatus
(
	IN ulong_t Status
);
XBSYSAPI EXPORTNUM(7) NTSTATUS NTAPI DbgLoadImageSymbols
(
	IN PANSI_STRING Name,
	IN PVOID Base,
	IN ULONG_PTR ProcessId
);
XBSYSAPI EXPORTNUM(8) ulong_t _cdecl DbgPrint
(
	PCHAR  Format, ...
);
XBSYSAPI EXPORTNUM(10) ulong_t NTAPI DbgPrompt
(
	IN PCCH Prompt,
	OUT PCH Response,
	IN ulong_t MaximumResponseLength
);
XBSYSAPI EXPORTNUM(11) void_t NTAPI DbgUnLoadImageSymbols
(
	IN PANSI_STRING Name,
	IN PVOID Base,
	IN ULONG_PTR ProcessId
);

}

#endif




