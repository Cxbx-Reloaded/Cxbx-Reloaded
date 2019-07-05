// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
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
// *  (c) 2019 Jannik Vogel
// *  (c) 2018-2019 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************


#define LOG_PREFIX CXBXR_MODULE::XC

// prevent name collisions
namespace xboxkrnl
{
	#include <xboxkrnl/xboxkrnl.h> // For XcSHAInit, etc.
};

#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"
#include "common\crypto\EmuSha.h" // For A_SHAInit, etc.
#include "common\crypto\LibRc4.h" // For RC4 Functions
#include "common\crypto\EmuDes.h" // For DES Functions
#include "common\crypto\EmuRSA.h" // For RSA Functions

// prevent name collisions
namespace NtDll
{
	#include "core\kernel\support\EmuNtDll.h"
};


// The following are the default implementations of the crypto functions

xboxkrnl::VOID NTAPI JumpedSHAInit
(
	xboxkrnl::PUCHAR pbSHAContext
)
{
	// The sha1 context supplied to this function has an extra 24 bytes at the beginning which are unsed by our implementation,
	// so we skip them. The same is true for XcSHAUpdate and XcSHAFinal

	SHA1Init((SHA1_CTX*)(pbSHAContext + 24));
}

xboxkrnl::VOID NTAPI JumpedSHAUpdate
(
	xboxkrnl::PUCHAR pbSHAContext,
	xboxkrnl::PUCHAR pbInput,
	xboxkrnl::ULONG dwInputLength
)
{
	SHA1Update((SHA1_CTX*)(pbSHAContext + 24), pbInput, dwInputLength);
}

xboxkrnl::VOID NTAPI JumpedSHAFinal
(
	xboxkrnl::PUCHAR pbSHAContext,
	xboxkrnl::PUCHAR pbDigest
)
{
	SHA1Final(pbDigest, (SHA1_CTX*)(pbSHAContext + 24));
}

xboxkrnl::VOID NTAPI JumpedRC4Key
(
	xboxkrnl::PUCHAR pbKeyStruct,
	xboxkrnl::ULONG dwKeyLength,
	xboxkrnl::PUCHAR pbKey
)
{
	Rc4Initialise((Rc4Context*)pbKeyStruct, pbKey, dwKeyLength, 0);
}

xboxkrnl::VOID NTAPI JumpedRC4Crypt
(
	xboxkrnl::PUCHAR pbKeyStruct,
	xboxkrnl::ULONG dwInputLength,
	xboxkrnl::PUCHAR pbInput
)
{
	Rc4Xor((Rc4Context*)pbKeyStruct, pbInput, pbInput, dwInputLength);
}

xboxkrnl::VOID NTAPI JumpedHMAC
(
	xboxkrnl::PBYTE pbKeyMaterial,
	xboxkrnl::ULONG cbKeyMaterial,
	xboxkrnl::PBYTE pbData,
	xboxkrnl::ULONG cbData,
	xboxkrnl::PBYTE pbData2,
	xboxkrnl::ULONG cbData2,
	xboxkrnl::PBYTE HmacData
)
{
	if (cbKeyMaterial > 64) {
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

	SHA1_CTX ShaContext;
	SHA1Init(&ShaContext);
	SHA1Update(&ShaContext, Pad1, 64);

	if (cbData != 0) {
		SHA1Update(&ShaContext, pbData, cbData);
	}

	if (cbData2 != 0) {
		SHA1Update(&ShaContext, pbData2, cbData2);
	}

	BYTE Temp[64 + A_SHA_DIGEST_LEN];
	SHA1Final(Temp + 64, &ShaContext);
	RtlCopyMemory(Temp, Pad2, 64);

	SHA1Init(&ShaContext);
	SHA1Update(&ShaContext, Temp, sizeof(Temp));
	SHA1Final(HmacData, &ShaContext);
}

xboxkrnl::ULONG NTAPI JumpedPKEncPublic
(
	xboxkrnl::PUCHAR pbPubKey,
	xboxkrnl::PUCHAR pbInput,
	xboxkrnl::PUCHAR pbOutput
)
{
	ULONG ret = 0;

	LOG_UNIMPLEMENTED();

	return ret;
}

xboxkrnl::ULONG NTAPI JumpedPKDecPrivate
(
	xboxkrnl::PUCHAR pbPrvKey,
	xboxkrnl::PUCHAR pbInput,
	xboxkrnl::PUCHAR pbOutput
)
{
	ULONG ret = 0;

	LOG_UNIMPLEMENTED();

	return ret;
}

xboxkrnl::ULONG NTAPI JumpedPKGetKeyLen
(
	xboxkrnl::PUCHAR pbPubKey
)
{
	ULONG ret = 0;

	LOG_UNIMPLEMENTED();

	return ret;
}

xboxkrnl::BOOLEAN NTAPI JumpedVerifyPKCS1Signature
(
	xboxkrnl::PUCHAR pbSig,
	xboxkrnl::PUCHAR pbPubKey,
	xboxkrnl::PUCHAR pbDigest
)
{
	BOOLEAN ret = TRUE;

	LOG_UNIMPLEMENTED();

	return ret;
}

xboxkrnl::ULONG NTAPI JumpedModExp
(
	xboxkrnl::LPDWORD pA,
	xboxkrnl::LPDWORD pB,
	xboxkrnl::LPDWORD pC,
	xboxkrnl::LPDWORD pD,
	xboxkrnl::ULONG dwN
)
{
	unsigned int len = dwN * 4;
	if (xbox_exp_mod((unsigned char*)pA, (const unsigned char*)pB, (const unsigned char*)pC, (const unsigned char*)pD, len, len, len, len)) {
		return 1;
	}

	return 0;
}

xboxkrnl::VOID NTAPI JumpedDESKeyParity
(
	xboxkrnl::PUCHAR pbKey,
	xboxkrnl::ULONG dwKeyLength
)
{
	mbedtls_des_key_set_parity(pbKey, dwKeyLength);
}

xboxkrnl::VOID NTAPI JumpedKeyTable
(
	xboxkrnl::ULONG dwCipher,
	xboxkrnl::PUCHAR pbKeyTable,
	xboxkrnl::PUCHAR pbKey
)
{
	if (dwCipher) {
		mbedtls_des3_set3key_enc((mbedtls_des3_context*)pbKeyTable, pbKey);
	}
	else {
		mbedtls_des_setkey_enc((mbedtls_des_context*)pbKeyTable, pbKey);
	}
}

xboxkrnl::VOID NTAPI JumpedBlockCrypt
(
	xboxkrnl::ULONG dwCipher,
	xboxkrnl::PUCHAR pbOutput,
	xboxkrnl::PUCHAR pbInput,
	xboxkrnl::PUCHAR pbKeyTable,
	xboxkrnl::ULONG dwOp
)
{
	if (dwCipher) {
		mbedtls_des3_crypt_ecb((mbedtls_des3_context*)pbKeyTable, pbInput, pbOutput, dwOp);
	}
	else {
		mbedtls_des_crypt_ecb((mbedtls_des_context*)pbKeyTable, pbInput, pbOutput, dwOp);
	}
}

xboxkrnl::VOID NTAPI JumpedBlockCryptCBC
(
	xboxkrnl::ULONG dwCipher,
	xboxkrnl::ULONG dwInputLength,
	xboxkrnl::PUCHAR pbOutput,
	xboxkrnl::PUCHAR pbInput,
	xboxkrnl::PUCHAR pbKeyTable,
	xboxkrnl::ULONG dwOp,
	xboxkrnl::PUCHAR pbFeedback
)
{
	int ret;

	if (dwCipher) {
		ret = mbedtls_des3_crypt_cbc((mbedtls_des3_context*)pbKeyTable, dwOp, dwInputLength, pbFeedback, pbInput, pbOutput);
	}
	else {
		ret = mbedtls_des_crypt_cbc((mbedtls_des_context*)pbKeyTable, dwOp, dwInputLength, pbFeedback, pbInput, pbOutput);
	}

	if (ret == MBEDTLS_ERR_DES_INVALID_INPUT_LENGTH) {
		EmuLog(LOG_LEVEL::DEBUG, "%s: dwInputLength was not a multiple of 8 (it was %lu)", __func__, dwInputLength);
	}
}

xboxkrnl::ULONG NTAPI JumpedCryptService
(
	xboxkrnl::ULONG dwOp,
	xboxkrnl::PVOID pArgs
)
{
	// This seems to be a dummy function. It just returns zero regardless of the input arguments, which are left unchanged.

	return 0;
}

/* This struct contains the original crypto functions exposed by the kernel */
const xboxkrnl::CRYPTO_VECTOR DefaultCryptoStruct =
{
	JumpedSHAInit,
	JumpedSHAUpdate,
	JumpedSHAFinal,
	JumpedRC4Key,
	JumpedRC4Crypt,
	JumpedHMAC,
	JumpedPKEncPublic,
	JumpedPKDecPrivate,
	JumpedPKGetKeyLen,
	JumpedVerifyPKCS1Signature,
	JumpedModExp,
	JumpedDESKeyParity,
	JumpedKeyTable,
	JumpedBlockCrypt,
	JumpedBlockCryptCBC,
	JumpedCryptService
};

/* This struct contains the updated crypto functions which can be changed by the title with XcUpdateCrypto */
xboxkrnl::CRYPTO_VECTOR UpdatedCryptoStruct = DefaultCryptoStruct;

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

	// This function changes the default crypto function implementations with those supplied by the title (if not NULL)

	if (pNewVector->pXcSHAInit)
	{
		UpdatedCryptoStruct.pXcSHAInit = pNewVector->pXcSHAInit;
	}
	if (pNewVector->pXcSHAUpdate)
	{
		UpdatedCryptoStruct.pXcSHAUpdate = pNewVector->pXcSHAUpdate;
	}
	if (pNewVector->pXcSHAFinal)
	{
		UpdatedCryptoStruct.pXcSHAFinal = pNewVector->pXcSHAFinal;
	}
	if (pNewVector->pXcRC4Key)
	{
		UpdatedCryptoStruct.pXcRC4Key = pNewVector->pXcRC4Key;
	}
	if (pNewVector->pXcRC4Crypt)
	{
		UpdatedCryptoStruct.pXcRC4Crypt = pNewVector->pXcRC4Crypt;
	}
	if (pNewVector->pXcHMAC)
	{
		UpdatedCryptoStruct.pXcHMAC = pNewVector->pXcHMAC;
	}
	if (pNewVector->pXcPKEncPublic)
	{
		UpdatedCryptoStruct.pXcPKEncPublic = pNewVector->pXcPKEncPublic;
	}
	if (pNewVector->pXcPKDecPrivate)
	{
		UpdatedCryptoStruct.pXcPKDecPrivate = pNewVector->pXcPKDecPrivate;
	}
	if (pNewVector->pXcPKGetKeyLen)
	{
		UpdatedCryptoStruct.pXcPKGetKeyLen = pNewVector->pXcPKGetKeyLen;
	}
	if (pNewVector->pXcVerifyPKCS1Signature)
	{
		UpdatedCryptoStruct.pXcVerifyPKCS1Signature = pNewVector->pXcVerifyPKCS1Signature;
	}
	if (pNewVector->pXcModExp)
	{
		UpdatedCryptoStruct.pXcModExp = pNewVector->pXcModExp;
	}
	if (pNewVector->pXcDESKeyParity)
	{
		UpdatedCryptoStruct.pXcDESKeyParity = pNewVector->pXcDESKeyParity;
	}
	if (pNewVector->pXcKeyTable)
	{
		UpdatedCryptoStruct.pXcKeyTable = pNewVector->pXcKeyTable;
	}
	if (pNewVector->pXcBlockCrypt)
	{
		UpdatedCryptoStruct.pXcBlockCrypt = pNewVector->pXcBlockCrypt;
	}
	if (pNewVector->pXcBlockCryptCBC)
	{
		UpdatedCryptoStruct.pXcBlockCryptCBC = pNewVector->pXcBlockCryptCBC;
	}
	if (pNewVector->pXcCryptService)
	{
		UpdatedCryptoStruct.pXcCryptService = pNewVector->pXcCryptService;
	}

	// Return to the title the original implementations if it supplied an out buffer

	if (pROMVector)
	{
		*pROMVector = DefaultCryptoStruct;
	}
}

// The following functions simply jump to the corresponding registered crypto functions

// ******************************************************************
// * 0x014F - XcSHAInit()
// ******************************************************************
XBSYSAPI EXPORTNUM(335) xboxkrnl::VOID NTAPI xboxkrnl::XcSHAInit
(
	IN PUCHAR pbSHAContext
)
{
	LOG_FUNC_ONE_ARG_TYPE(PBYTE, pbSHAContext);

	UpdatedCryptoStruct.pXcSHAInit(pbSHAContext);
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

	UpdatedCryptoStruct.pXcSHAUpdate(pbSHAContext, pbInput, dwInputLength);
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

	UpdatedCryptoStruct.pXcSHAFinal(pbSHAContext, pbDigest);
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

	UpdatedCryptoStruct.pXcRC4Key(pbKeyStruct, dwKeyLength, pbKey);
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

	UpdatedCryptoStruct.pXcRC4Crypt(pbKeyStruct, dwInputLength, pbInput);
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

	UpdatedCryptoStruct.pXcHMAC(pbKeyMaterial, cbKeyMaterial, pbData, cbData, pbData2, cbData2, HmacData);
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

	ULONG ret = UpdatedCryptoStruct.pXcPKEncPublic(pbPubKey, pbInput, pbOutput);

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

	ULONG ret = UpdatedCryptoStruct.pXcPKDecPrivate(pbPrvKey, pbInput, pbOutput);

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

	ULONG ret = UpdatedCryptoStruct.pXcPKGetKeyLen(pbPubKey);

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

	BOOLEAN ret = UpdatedCryptoStruct.pXcVerifyPKCS1Signature(pbSig, pbPubKey, pbDigest);

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

	ULONG ret = UpdatedCryptoStruct.pXcModExp(pA, pB, pC, pD, dwN);

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

	UpdatedCryptoStruct.pXcDESKeyParity(pbKey, dwKeyLength);
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

	UpdatedCryptoStruct.pXcKeyTable(dwCipher, pbKeyTable, pbKey);
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

	UpdatedCryptoStruct.pXcBlockCrypt(dwCipher, pbOutput, pbInput, pbKeyTable, dwOp);
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

	UpdatedCryptoStruct.pXcBlockCryptCBC(dwCipher, dwInputLength, pbOutput, pbInput, pbKeyTable, dwOp, pbFeedback);
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

	ULONG ret = UpdatedCryptoStruct.pXcCryptService(dwOp, pArgs);

	RETURN(ret);
}
