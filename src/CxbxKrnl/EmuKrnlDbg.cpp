// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuKrnlDbg.cpp
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
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

// prevent name collisions
namespace xboxkrnl
{
#include <xboxkrnl/xboxkrnl.h> // For DbgPrint, etc.
};

#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"

// prevent name collisions
namespace NtDll
{
#include "EmuNtDll.h"
};


#include "Emu.h" // For EmuWarning()

// ******************************************************************
// * 0x0005 - DbgBreakPoint()
// ******************************************************************
XBSYSAPI EXPORTNUM(5) xboxkrnl::VOID NTAPI xboxkrnl::DbgBreakPoint()
{
	LOG_FUNC();

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x0006 - DbgBreakPointWithStatus()
// ******************************************************************
XBSYSAPI EXPORTNUM(6) xboxkrnl::VOID NTAPI xboxkrnl::DbgBreakPointWithStatus
(
	IN ULONG Status 
)
{
	LOG_FUNC_ONE_ARG(Status);

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x0007 - DbgLoadImageSymbols()
// ******************************************************************
XBSYSAPI EXPORTNUM(7) xboxkrnl::NTSTATUS NTAPI xboxkrnl::DbgLoadImageSymbols
(
	IN PANSI_STRING Name,
	IN PVOID Base,
	IN ULONG_PTR ProcessId
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Name)
		LOG_FUNC_ARG(Base)
		LOG_FUNC_ARG(ProcessId)
		LOG_FUNC_END;

	NTSTATUS result = S_OK;

	LOG_UNIMPLEMENTED();

	RETURN(result);
}

// ******************************************************************
// * 0x0008 - DbgPrint()
// ******************************************************************
XBSYSAPI EXPORTNUM(8) xboxkrnl::ULONG _cdecl xboxkrnl::DbgPrint
(
	PCHAR  Format, ...
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Format)
		LOG_FUNC_ARG("...")
		LOG_FUNC_END;

	if (Format != NULL)
	{
		char szBuffer[MAX_PATH];

		va_list argp;
		va_start(argp, Format);

		vsprintf(szBuffer, Format, argp);
		va_end(argp);

		//LogPrintf("[EmuKrnl] DbgPrint: %s", szBuffer);

		EmuWarning(szBuffer);
		//DbgPrintf(szBuffer);
	}

	RETURN(STATUS_SUCCESS);
}

// ******************************************************************
// * 0x000A - DbgPrompt()
// ******************************************************************
// Source:ReactOS
XBSYSAPI EXPORTNUM(10) xboxkrnl::ULONG NTAPI xboxkrnl::DbgPrompt
(
	IN PCCH Prompt,
	OUT PCH Response,
	IN ULONG MaximumResponseLength
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Prompt)
		LOG_FUNC_ARG_OUT(Response)
		LOG_FUNC_ARG(MaximumResponseLength)
		LOG_FUNC_END;

	NTSTATUS result = S_OK;

	LOG_UNIMPLEMENTED();

	RETURN(result);
}

// ******************************************************************
// * 0x000B - DbgUnLoadImageSymbols()
// ******************************************************************
// Source:ReactOS
XBSYSAPI EXPORTNUM(11) xboxkrnl::VOID NTAPI xboxkrnl::DbgUnLoadImageSymbols
(
	IN PANSI_STRING Name,
	IN PVOID Base,
	IN ULONG_PTR ProcessId
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Name)
		LOG_FUNC_ARG(Base)
		LOG_FUNC_ARG(ProcessId)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();
}
