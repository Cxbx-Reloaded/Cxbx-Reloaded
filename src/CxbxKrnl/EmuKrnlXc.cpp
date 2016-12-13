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
#include "EmuKrnlLogging.h"
#include "EmuSha.h" // For A_SHAInit, etc.

// prevent name collisions
namespace NtDll
{
#include "EmuNtDll.h"
};

// ******************************************************************
// * 0x014F - XcSHAInit()
// ******************************************************************
XBSYSAPI EXPORTNUM(335) xboxkrnl::VOID NTAPI xboxkrnl::XcSHAInit
(
	UCHAR *pbSHAContext
)
{
	LOG_FUNC_ONE_ARG_OUT(pbSHAContext);

	A_SHAInit((SHA_CTX*)pbSHAContext);
}

// ******************************************************************
// * 0x0150 - XcSHAUpdate()
// ******************************************************************
XBSYSAPI EXPORTNUM(336) xboxkrnl::VOID NTAPI xboxkrnl::XcSHAUpdate
(
	UCHAR *pbSHAContext,
	UCHAR *pbInput,
	ULONG dwInputLength
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(pbSHAContext)
		LOG_FUNC_ARG_OUT(pbInput)
		LOG_FUNC_ARG(dwInputLength)
		LOG_FUNC_END;

	A_SHAUpdate((SHA_CTX*)pbSHAContext, pbInput, dwInputLength);
}

// ******************************************************************
// * 0x0151 - XcSHAFinal()
// ******************************************************************
XBSYSAPI EXPORTNUM(337) xboxkrnl::VOID NTAPI xboxkrnl::XcSHAFinal
(
	UCHAR *pbSHAContext,
	UCHAR *pbDigest
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(pbSHAContext)
		LOG_FUNC_ARG_OUT(pbDigest)
		LOG_FUNC_END;

	A_SHAFinal((SHA_CTX*)pbSHAContext, pbDigest);
}

// ******************************************************************
// * 0x0154 - XcHMAC()
// ******************************************************************
XBSYSAPI EXPORTNUM(340) xboxkrnl::VOID NTAPI xboxkrnl::XcHMAC(
	IN PBYTE pbKeyMaterial,
	IN ULONG cbKeyMaterial,
	IN PBYTE pbData,
	IN ULONG cbData,
	IN PBYTE pbData2,
	IN ULONG cbData2,
	OUT PBYTE HmacData
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(pbKeyMaterial)
		LOG_FUNC_ARG(cbKeyMaterial)
		LOG_FUNC_ARG_OUT(pbData)
		LOG_FUNC_ARG(cbData)
		LOG_FUNC_ARG_OUT(pbData2)
		LOG_FUNC_ARG(cbData2)
		LOG_FUNC_ARG_OUT(HmacData)
		LOG_FUNC_END;

	if (cbKeyMaterial > 64)	{
		cbKeyMaterial = 64;
	}

	BYTE Pad1[64];
	RtlZeroMemory(Pad1, 64);
	RtlCopyMemory(Pad1, pbKeyMaterial, cbKeyMaterial);

	BYTE Pad2[64];
	RtlZeroMemory(Pad2, 64);
	RtlCopyMemory(Pad2, pbKeyMaterial, cbKeyMaterial);

	for (ULONG dwBlock = 0; dwBlock< 64 / sizeof(DWORD); dwBlock++) {
		((DWORD*)Pad1)[dwBlock] ^= ((DWORD)0x36363636);
		((DWORD*)Pad2)[dwBlock] ^= ((DWORD)0x5C5C5C5C);
	}

	SHA_CTX ShaContext;
	A_SHAInit(&ShaContext);
	A_SHAUpdate(&ShaContext, Pad1, 64);

	if (cbData != 0) {
		A_SHAUpdate(&ShaContext, pbData, cbData);
	}

	if (cbData2 != 0) {
		A_SHAUpdate(&ShaContext, pbData2, cbData2);
	}

	BYTE Temp[64 + A_SHA_DIGEST_LEN];
	A_SHAFinal(&ShaContext, Temp + 64);
	RtlCopyMemory(Temp, Pad2, 64);

	A_SHAInit(&ShaContext);
	A_SHAUpdate(&ShaContext, Temp, sizeof(Temp));
	A_SHAFinal(&ShaContext, HmacData);
}
