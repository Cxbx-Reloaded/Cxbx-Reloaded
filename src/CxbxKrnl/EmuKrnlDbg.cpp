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
#include "Emu.h" // For EmuWarning()

using namespace xboxkrnl;

// ******************************************************************
// * 0x0008 DbgPrint
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

	return STATUS_SUCCESS;
}
