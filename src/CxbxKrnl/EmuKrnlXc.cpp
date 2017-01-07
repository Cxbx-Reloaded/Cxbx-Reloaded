// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
#include "LibRc4.h" // For RC4 Functions

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
	IN PUCHAR pbSHAContext
)
{
	LOG_FUNC_ONE_ARG_TYPE(PBYTE, pbSHAContext);

	A_SHAInit((SHA_CTX*)pbSHAContext);
}

// ******************************************************************
// * 0x0150 - XcSHAUpdate()
// ******************************************************************
XBSYSAPI EXPORTNUM(336) xboxkrnl::VOID NTAPI xboxkrnl::XcSHAUpdate
(
	PUCHAR pbSHAContext,
	PUCHAR pbInput,
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
	IN PUCHAR pbSHAContext,
	IN PUCHAR pbDigest
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_TYPE(PBYTE, pbSHAContext)
		LOG_FUNC_ARG_TYPE(PBYTE, pbDigest)
		LOG_FUNC_END;

	A_SHAFinal((SHA_CTX*)pbSHAContext, pbDigest);
}

// ******************************************************************
// * 0x0152 - XcRC4Key()
// ******************************************************************
XBSYSAPI EXPORTNUM(338) xboxkrnl::VOID NTAPI xboxkrnl::XcRC4Key
(
	IN PUCHAR pbKeyStruct,
	IN ULONG dwKeyLength,
	IN PUCHAR pbKey
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_TYPE(PBYTE, pbKeyStruct)
		LOG_FUNC_ARG(dwKeyLength)
		LOG_FUNC_ARG_TYPE(PBYTE, pbKey)
		LOG_FUNC_END;

	Rc4Initialise((Rc4Context*)pbKeyStruct, pbKey, dwKeyLength, 0);
}

// ******************************************************************
// * 0x0153 - XcRC4Crypt
// ******************************************************************
XBSYSAPI EXPORTNUM(339) xboxkrnl::VOID NTAPI xboxkrnl::XcRC4Crypt
(
	IN PUCHAR pbKeyStruct,
	IN ULONG dwInputLength,
	IN PUCHAR pbInput
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_TYPE(PBYTE, pbKeyStruct)
		LOG_FUNC_ARG(dwInputLength)
		LOG_FUNC_ARG_TYPE(PBYTE, pbInput)
		LOG_FUNC_END;

	Rc4Xor((Rc4Context*)pbKeyStruct, pbInput, pbInput, dwInputLength);
}

// ******************************************************************
// * 0x0154 - XcHMAC()
// ******************************************************************
XBSYSAPI EXPORTNUM(340) xboxkrnl::VOID NTAPI xboxkrnl::XcHMAC
(
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
		LOG_FUNC_ARG(pbKeyMaterial)
		LOG_FUNC_ARG(cbKeyMaterial)
		LOG_FUNC_ARG(pbData)
		LOG_FUNC_ARG(cbData)
		LOG_FUNC_ARG(pbData2)
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

// ******************************************************************
// * 0x0155 - XcPKEncPublic()
// ******************************************************************
XBSYSAPI EXPORTNUM(341) xboxkrnl::ULONG NTAPI xboxkrnl::XcPKEncPublic
(
	IN PUCHAR pbPubKey,
	IN PUCHAR pbInput,
	OUT PUCHAR pbOutput
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_TYPE(PBYTE, pbPubKey)
		LOG_FUNC_ARG_TYPE(PBYTE, pbInput)
		LOG_FUNC_ARG_OUT(pbOutput)
		LOG_FUNC_END;

	ULONG ret = 0;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x0156 - XcPKDecPrivate()
// ******************************************************************
XBSYSAPI EXPORTNUM(342) xboxkrnl::ULONG NTAPI xboxkrnl::XcPKDecPrivate
(
	IN PUCHAR pbPrvKey,
	IN PUCHAR pbInput,
	OUT PUCHAR pbOutput
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_TYPE(PBYTE, pbPrvKey)
		LOG_FUNC_ARG_TYPE(PBYTE, pbInput)
		LOG_FUNC_ARG_OUT(pbOutput)
		LOG_FUNC_END;

	ULONG ret = 0;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x0157 - XcPKGetKeyLen()
// ******************************************************************
XBSYSAPI EXPORTNUM(343) xboxkrnl::ULONG NTAPI xboxkrnl::XcPKGetKeyLen
(
	OUT PUCHAR pbPubKey
)
{
	LOG_FUNC_ONE_ARG_TYPE(PBYTE, pbPubKey);

	ULONG ret = 0;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x0158 - XcVerifyPKCS1Signature()
// ******************************************************************
XBSYSAPI EXPORTNUM(344) xboxkrnl::BOOLEAN NTAPI xboxkrnl::XcVerifyPKCS1Signature
(
	IN PUCHAR pbSig,
	IN PUCHAR pbPubKey,
	IN PUCHAR pbDigest
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_TYPE(PBYTE, pbSig)
		LOG_FUNC_ARG_TYPE(PBYTE, pbPubKey)
		LOG_FUNC_ARG_TYPE(PBYTE, pbDigest)
		LOG_FUNC_END;

	BOOLEAN ret = TRUE;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x0159 - XcModExp()
// ******************************************************************
XBSYSAPI EXPORTNUM(345) xboxkrnl::ULONG NTAPI xboxkrnl::XcModExp
(
	IN LPDWORD pA,
	IN LPDWORD pB,
	IN LPDWORD pC,
	IN LPDWORD pD,
	IN ULONG dwN
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pA)
		LOG_FUNC_ARG(pB)
		LOG_FUNC_ARG(pC)
		LOG_FUNC_ARG(pD)
		LOG_FUNC_ARG(dwN)
		LOG_FUNC_END;

	ULONG ret = 0;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x015A - XcDESKeyParity()
// ******************************************************************
XBSYSAPI EXPORTNUM(346) xboxkrnl::VOID NTAPI xboxkrnl::XcDESKeyParity
(
	IN PUCHAR pbKey,
	IN ULONG dwKeyLength
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_TYPE(PBYTE, pbKey)
		LOG_FUNC_ARG(dwKeyLength)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x015B - XcKeyTable()
// ******************************************************************
XBSYSAPI EXPORTNUM(347) xboxkrnl::VOID NTAPI xboxkrnl::XcKeyTable
(
	IN ULONG dwCipher,
	OUT PUCHAR pbKeyTable,
	IN PUCHAR pbKey
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(dwCipher)
		LOG_FUNC_ARG_OUT(pbKeyTable)
		LOG_FUNC_ARG_TYPE(PBYTE, pbKey)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x015C - XcBlockCrypt()
// ******************************************************************
XBSYSAPI EXPORTNUM(348) xboxkrnl::VOID NTAPI xboxkrnl::XcBlockCrypt
(
	IN ULONG dwCipher,
	OUT PUCHAR pbOutput,
	IN PUCHAR pbInput,
	IN PUCHAR pbKeyTable,
	IN ULONG dwOp
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(dwCipher)
		LOG_FUNC_ARG_OUT(pbOutput)
		LOG_FUNC_ARG_TYPE(PBYTE, pbInput)
		LOG_FUNC_ARG_TYPE(PBYTE, pbKeyTable)
		LOG_FUNC_ARG(dwOp)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x015D - XcBlockCryptCBC()
// ******************************************************************
XBSYSAPI EXPORTNUM(349) xboxkrnl::VOID NTAPI xboxkrnl::XcBlockCryptCBC
(
	IN ULONG dwCipher,
	IN ULONG dwInputLength,
	OUT PUCHAR pbOutput,
	IN PUCHAR pbInput,
	IN PUCHAR pbKeyTable,
	IN ULONG dwOp,
	IN PUCHAR pbFeedback
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(dwCipher)
		LOG_FUNC_ARG(dwInputLength)
		LOG_FUNC_ARG_OUT(pbOutput)
		LOG_FUNC_ARG_TYPE(PBYTE, pbInput)
		LOG_FUNC_ARG_TYPE(PBYTE, pbKeyTable)
		LOG_FUNC_ARG(dwOp)
		LOG_FUNC_ARG_TYPE(PBYTE, pbFeedback)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x015E - XcCryptService()
// ******************************************************************
XBSYSAPI EXPORTNUM(350) xboxkrnl::ULONG NTAPI xboxkrnl::XcCryptService
(
	IN ULONG dwOp,
	IN PVOID pArgs
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(dwOp)
		LOG_FUNC_ARG(pArgs)
		LOG_FUNC_END;

	ULONG ret = 0;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

typedef PVOID PCRYPTO_VECTOR;

// ******************************************************************
// * 0x015F - XcUpdateCrypto()
// ******************************************************************
XBSYSAPI EXPORTNUM(351) xboxkrnl::VOID NTAPI xboxkrnl::XcUpdateCrypto
(
	IN PCRYPTO_VECTOR pNewVector,
	OUT PCRYPTO_VECTOR pROMVector OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pNewVector)
		LOG_FUNC_ARG_OUT(pROMVector)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();
}
