// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuKrnlXc.cpp
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
#include <xboxkrnl/xboxkrnl.h> // For XcSHAInit, etc.
};

#include "Logging.h" // For LOG_FUNC()
#include "Emu.h" // For EmuWarning()

using namespace xboxkrnl;

// ******************************************************************
// * XcSHAInit
// ******************************************************************
XBSYSAPI EXPORTNUM(335) VOID NTAPI xboxkrnl::XcSHAInit
(
	UCHAR *pbSHAContext
)
{
	LOG_FUNC_ONE_ARG(pbSHAContext);

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * XcSHAUpdate
// ******************************************************************
XBSYSAPI EXPORTNUM(336) VOID NTAPI xboxkrnl::XcSHAUpdate
(
	UCHAR *pbSHAContext,
	UCHAR *pbInput,
	ULONG dwInputLength
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pbSHAContext)
		LOG_FUNC_ARG(pbInput)
		LOG_FUNC_ARG(dwInputLength)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * XcSHAFinal
// ******************************************************************
XBSYSAPI EXPORTNUM(337) VOID NTAPI xboxkrnl::XcSHAFinal
(
	UCHAR *pbSHAContext,
	UCHAR *pbDigest
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pbSHAContext)
		LOG_FUNC_ARG(pbDigest)
		LOG_FUNC_END;

	// for now, we dont care about the digest
	for (int v = 0; v<20; v++)
	{
		pbDigest[v] = 0;
	}
}

