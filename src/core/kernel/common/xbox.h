// ******************************************************************
// *
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : xbox.h
// *
// * note : XBox Kernel Declarations
// *
// ******************************************************************
#ifndef XBOX_H
#define XBOX_H

#include "types.h"

namespace xbox
{

#define AV_PACK_NONE                      0x00000000
#define AV_PACK_STANDARD                  0x00000001
#define AV_PACK_RFU                       0x00000002
#define AV_PACK_SCART                     0x00000003
#define AV_PACK_HDTV                      0x00000004
#define AV_PACK_VGA                       0x00000005
#define AV_PACK_SVIDEO                    0x00000006
#define AV_PACK_MAX                       0x00000007
#define AV_PACK_MASK                      0x000000FF

#define AV_STANDARD_NTSC_M                0x00000100
#define AV_STANDARD_NTSC_J                0x00000200
#define AV_STANDARD_PAL_I                 0x00000300
#define AV_STANDARD_PAL_M                 0x00000400
#define AV_STANDARD_MAX                   0x00000500
#define AV_STANDARD_MASK                  0x00000F00
#define AV_STANDARD_SHIFT                 8

#define AV_FLAGS_HDTV_480i                0x00000000 // Bogus 'flag'
#define AV_FLAGS_HDTV_720p                0x00020000
#define AV_FLAGS_HDTV_1080i               0x00040000
#define AV_FLAGS_HDTV_480p                0x00080000
#define AV_HDTV_MODE_MASK                 0x000E0000 // Exclude AV_FLAGS_WIDESCREEN !

#define AV_FLAGS_NORMAL                   0x00000000
#define AV_FLAGS_WIDESCREEN               0x00010000
#define AV_FLAGS_LETTERBOX                0x00100000
#define AV_ASPECT_RATIO_MASK              0x00110000 // = AV_FLAGS_WIDESCREEN | AV_FLAGS_LETTERBOX

#define AV_FLAGS_INTERLACED               0x00200000

#define AV_FLAGS_60Hz                     0x00400000
#define AV_FLAGS_50Hz                     0x00800000
#define AV_REFRESH_MASK                   0x00C00000 // = AV_FLAGS_60Hz | AV_FLAGS_50Hz

#define AV_FLAGS_FIELD                    0x01000000
#define AV_FLAGS_10x11PAR                 0x02000000

#define AV_USER_FLAGS_MASK                (AV_HDTV_MODE_MASK | AV_ASPECT_RATIO_MASK | AV_REFRESH_MASK) // TODO : Should AV_REFRESH_MASK be AV_FLAGS_60Hz instead?
#define AV_USER_FLAGS_SHIFT               16

// Options to AvSendTVEncoderOption() :
#define AV_OPTION_MACROVISION_MODE        1

#define AV_OPTION_ENABLE_CC               2
#define AV_OPTION_DISABLE_CC              3
#define AV_OPTION_SEND_CC_DATA            4
#define AV_QUERY_CC_STATUS                5
#define AV_QUERY_AV_CAPABILITIES          6
#define AV_OPTION_BLANK_SCREEN            9
#define AV_OPTION_MACROVISION_COMMIT      10
#define AV_OPTION_FLICKER_FILTER          11
#define AV_OPTION_ZERO_MODE               12
#define AV_OPTION_QUERY_MODE              13
#define AV_OPTION_ENABLE_LUMA_FILTER      14
#define AV_OPTION_GUESS_FIELD             15
#define AV_QUERY_ENCODER_TYPE             16
#define AV_QUERY_MODE_TABLE_VERSION       17
#define AV_OPTION_CGMS                    18
#define AV_OPTION_WIDESCREEN              19

void_xt InitializeFscCacheEvent();

// ******************************************************************
// * 0x0001 - AvGetSavedDataAddress()
// ******************************************************************
XBSYSAPI EXPORTNUM(1) PVOID NTAPI AvGetSavedDataAddress(void);

// ******************************************************************
// * 0x0002 - AvSendTVEncoderOption()
// ******************************************************************
XBSYSAPI EXPORTNUM(2) void_xt NTAPI AvSendTVEncoderOption
(
    IN  PVOID   RegisterBase,
    IN  ulong_xt   Option,
    IN  ulong_xt   Param,
    OUT ulong_xt   *Result
);

// ******************************************************************
// * 0x0003 - AvSetDisplayMode()
// ******************************************************************
XBSYSAPI EXPORTNUM(3) ulong_xt NTAPI AvSetDisplayMode
(
    IN PVOID    RegisterBase,
    IN ulong_xt    Step,
    IN ulong_xt    Mode,
    IN ulong_xt    Format,
    IN ulong_xt    Pitch,
    IN ulong_xt    FrameBuffer
);

// ******************************************************************
// * 0x0004 - AvSetSavedDataAddress()
// ******************************************************************
XBSYSAPI EXPORTNUM(4) void_xt NTAPI AvSetSavedDataAddress
(
    IN PVOID    Address
);

// ******************************************************************
// * 0x0023 - FscGetCacheSize()
// ******************************************************************
XBSYSAPI EXPORTNUM(35) ulong_xt NTAPI FscGetCacheSize();

// ******************************************************************
// * 0x0024 - FscInvalidateIdleBlocks()
// ******************************************************************
XBSYSAPI EXPORTNUM(36) void_xt NTAPI FscInvalidateIdleBlocks();

// ******************************************************************
// * 0x0025 - FscSetCacheSize()
// ******************************************************************
XBSYSAPI EXPORTNUM(37) ntstatus_xt NTAPI FscSetCacheSize
(
		ulong_xt NumberOfCachePages
);

// ******************************************************************
// * 0x0058 - KdDebuggerEnabled
// ******************************************************************
XBSYSAPI EXPORTNUM(88) boolean_xt KdDebuggerEnabled;

// ******************************************************************
// * 0x0059 - KdDebuggerNotPresent
// ******************************************************************
XBSYSAPI EXPORTNUM(89) boolean_xt KdDebuggerNotPresent;

// ******************************************************************
// * 0x00A0 - KfRaiseIrql()
// ******************************************************************
XBSYSAPI EXPORTNUM(160) uchar_xt FASTCALL KfRaiseIrql
(
    IN uchar_xt    NewIrql
);

// ******************************************************************
// * 0x00A1 - KfLowerIrql()
// ******************************************************************
XBSYSAPI EXPORTNUM(161) void_xt FASTCALL KfLowerIrql
(
    IN uchar_xt    NewIrql
);

// ******************************************************************
// * 0x00A2 - KiBugCheckData
// ******************************************************************
XBSYSAPI EXPORTNUM(162) ulong_ptr_xt KiBugCheckData[5];

void KiLockDispatcherDatabase 
(
	OUT KIRQL* OldIrql
);

// ******************************************************************
// * 0x00A3 - KiUnlockDispatcherDatabase()
// ******************************************************************
XBSYSAPI EXPORTNUM(163) void_xt FASTCALL KiUnlockDispatcherDatabase
(
	IN KIRQL OldIrql
);

// ******************************************************************
// * 0x00A4 - LaunchDataPage (actually a pointer)
// ******************************************************************
XBSYSAPI EXPORTNUM(164) PLAUNCH_DATA_PAGE LaunchDataPage;

// ******************************************************************
// * 0x00FC - PhyGetLinkState()
// ******************************************************************
XBSYSAPI EXPORTNUM(252) dword_xt NTAPI PhyGetLinkState
(
	IN ulong_xt	Mode
);

// ******************************************************************
// * 0x00FD - PhyInitialize()
// ******************************************************************
XBSYSAPI EXPORTNUM(253) ntstatus_xt NTAPI PhyInitialize
(
	IN ulong_xt	forceReset,
	IN PVOID	Parameter2
);

// ******************************************************************
// * 0x0141 - XboxEEPROMKey
// ******************************************************************
XBSYSAPI EXPORTNUM(321) uchar_xt XboxEEPROMKey[16];

// ******************************************************************
// * 0x0142 - XboxHardwareInfo
// ******************************************************************
XBSYSAPI EXPORTNUM(322) XBOX_HARDWARE_INFO XboxHardwareInfo;

// ******************************************************************
// * 0x0143 - XboxHDKey
// ******************************************************************
XBSYSAPI EXPORTNUM(323) uchar_xt XboxHDKey[16];

typedef struct _XBOX_KRNL_VERSION {
    ushort_xt Major;
    ushort_xt Minor;
    ushort_xt Build;
    ushort_xt Qfe;
} XBOX_KRNL_VERSION;

// ******************************************************************
// * 0x0144 - XboxKrnlVersion
// ******************************************************************
XBSYSAPI EXPORTNUM(324) XBOX_KRNL_VERSION XboxKrnlVersion;

// ******************************************************************
// * 0x0145 - XboxSignatureKey
// ******************************************************************
XBSYSAPI EXPORTNUM(325) byte_xt XboxSignatureKey[16];

// ******************************************************************
// * 0x0146 - XeImageFileName
// ******************************************************************
XBSYSAPI EXPORTNUM(326) OBJECT_STRING XeImageFileName;

// ******************************************************************
// * 0x0147 - XeLoadSection()
// ******************************************************************
XBSYSAPI EXPORTNUM(327) ntstatus_xt NTAPI XeLoadSection
(
	IN PXBEIMAGE_SECTION Section
);

// ******************************************************************
// * 0x0148 - XeUnloadSection()
// ******************************************************************
XBSYSAPI EXPORTNUM(328) ntstatus_xt NTAPI XeUnloadSection
(
	IN PXBEIMAGE_SECTION Section
);

// ******************************************************************
// * 0x014F - XcSHAInit()
// ******************************************************************
XBSYSAPI EXPORTNUM(335) void_xt NTAPI XcSHAInit
(
	IN PUCHAR pbSHAContext
);

// ******************************************************************
// * 0x0150 - XcSHAUpdate()
// ******************************************************************
XBSYSAPI EXPORTNUM(336) void_xt NTAPI XcSHAUpdate
(
	IN PUCHAR pbSHAContext,
	IN PUCHAR pbInput,
	IN ulong_xt dwInputLength
);

// ******************************************************************
// * 0x0151 - XcSHAFinal()
// ******************************************************************
XBSYSAPI EXPORTNUM(337) void_xt NTAPI XcSHAFinal
(
	IN PUCHAR pbSHAContext,
	IN PUCHAR pbDigest
);

// ******************************************************************
// * 0x0152 - XcRC4Key()
// ******************************************************************
XBSYSAPI EXPORTNUM(338) void_xt NTAPI XcRC4Key
(
	IN PUCHAR pbKeyStruct,
	IN ulong_xt dwKeyLength,
	IN PUCHAR pbKey
);

// ******************************************************************
// * 0x0153 - XcRC4Crypt()
// ******************************************************************
XBSYSAPI EXPORTNUM(339) void_xt NTAPI XcRC4Crypt
(
	IN PUCHAR pbKeyStruct,
	IN ulong_xt dwInputLength,
	IN PUCHAR pbInput
);

// ******************************************************************
// * 0x0154 - XcHMAC()
// ******************************************************************
XBSYSAPI EXPORTNUM(340) void_xt NTAPI XcHMAC
(
	IN PBYTE pbKeyMaterial,
	IN ulong_xt cbKeyMaterial,
	IN PBYTE pbData,
	IN ulong_xt cbData,
	IN PBYTE pbData2,
	IN ulong_xt cbData2,
	OUT PBYTE HmacData
);

// ******************************************************************
// * 0x0155 - XcPKEncPublic()
// ******************************************************************
XBSYSAPI EXPORTNUM(341) ulong_xt NTAPI XcPKEncPublic
(
	IN PUCHAR pbPubKey,
	IN PUCHAR pbInput,
	OUT PUCHAR pbOutput
);

// ******************************************************************
// * 0x0156 - XcPKDecPrivate()
// ******************************************************************
XBSYSAPI EXPORTNUM(342) ulong_xt NTAPI XcPKDecPrivate
(
	IN PUCHAR pbPrvKey,
	IN PUCHAR pbInput,
	OUT PUCHAR pbOutput
);

// ******************************************************************
// * 0x0157 - XcPKGetKeyLen()
// ******************************************************************
XBSYSAPI EXPORTNUM(343) ulong_xt NTAPI XcPKGetKeyLen
(
	OUT PUCHAR pbPubKey
);

// ******************************************************************
// * 0x0158 - XcVerifyPKCS1Signature()
// ******************************************************************
XBSYSAPI EXPORTNUM(344) boolean_xt NTAPI XcVerifyPKCS1Signature
(
	IN PUCHAR pbSig,
	IN PUCHAR pbPubKey,
	IN PUCHAR pbDigest
);

// ******************************************************************
// * 0x0159 - XcModExp()
// ******************************************************************
XBSYSAPI EXPORTNUM(345) ulong_xt NTAPI XcModExp
(
	IN LPDWORD pA,
	IN LPDWORD pB,
	IN LPDWORD pC,
	IN LPDWORD pD,
	IN ulong_xt dwN
);

// ******************************************************************
// * 0x015A - XcDESKeyParity()
// ******************************************************************
XBSYSAPI EXPORTNUM(346) void_xt NTAPI XcDESKeyParity
(
	IN PUCHAR pbKey,
	IN ulong_xt dwKeyLength
);

// ******************************************************************
// * 0x015B - XcKeyTable()
// ******************************************************************
XBSYSAPI EXPORTNUM(347) void_xt NTAPI XcKeyTable
(
	IN ulong_xt dwCipher,
	OUT PUCHAR pbKeyTable,
	IN PUCHAR pbKey
);

// ******************************************************************
// * 0x015C - XcBlockCrypt()
// ******************************************************************
XBSYSAPI EXPORTNUM(348) void_xt NTAPI XcBlockCrypt
(
	IN ulong_xt dwCipher,
	OUT PUCHAR pbOutput,
	IN PUCHAR pbInput,
	IN PUCHAR pbKeyTable,
	IN ulong_xt dwOp
);

// ******************************************************************
// * 0x015D - XcBlockCryptCBC()
// ******************************************************************
XBSYSAPI EXPORTNUM(349) void_xt NTAPI XcBlockCryptCBC
(
	IN ulong_xt dwCipher,
	IN ulong_xt dwInputLength,
	OUT PUCHAR pbOutput,
	IN PUCHAR pbInput,
	IN PUCHAR pbKeyTable,
	IN ulong_xt dwOp,
	IN PUCHAR pbFeedback
);

// ******************************************************************
// * 0x015E - XcCryptService()
// ******************************************************************
XBSYSAPI EXPORTNUM(350) ulong_xt NTAPI XcCryptService
(
	IN ulong_xt dwOp,
	IN PVOID pArgs
);

/* Function pointers which point to all the kernel crypto functions. Used by PCRYPTO_VECTOR. */
typedef void_xt(NTAPI *pfXcSHAInit)(PUCHAR pbSHAContext);
typedef void_xt(NTAPI *pfXcSHAUpdate)(PUCHAR pbSHAContext, PUCHAR pbInput, ulong_xt dwInputLength);
typedef void_xt(NTAPI *pfXcSHAFinal)(PUCHAR pbSHAContext, PUCHAR pbDigest);
typedef void_xt(NTAPI *pfXcRC4Key)(PUCHAR pbKeyStruct, ulong_xt dwKeyLength, PUCHAR pbKey);
typedef void_xt(NTAPI *pfXcRC4Crypt)(PUCHAR pbKeyStruct, ulong_xt dwInputLength, PUCHAR pbInput);
typedef void_xt(NTAPI *pfXcHMAC)(PBYTE pbKeyMaterial, ulong_xt cbKeyMaterial, PBYTE pbData, ulong_xt cbData, PBYTE pbData2, ulong_xt cbData2, PBYTE HmacData);
typedef ulong_xt(NTAPI *pfXcPKEncPublic)(PUCHAR pbPubKey, PUCHAR pbInput, PUCHAR pbOutput);
typedef ulong_xt(NTAPI *pfXcPKDecPrivate)(PUCHAR pbPrvKey, PUCHAR pbInput, PUCHAR pbOutput);
typedef ulong_xt(NTAPI *pfXcPKGetKeyLen)(PUCHAR pbPubKey);
typedef boolean_xt(NTAPI *pfXcVerifyPKCS1Signature)(PUCHAR pbSig, PUCHAR pbPubKey, PUCHAR pbDigest);
typedef ulong_xt(NTAPI *pfXcModExp)(LPDWORD pA, LPDWORD pB, LPDWORD pC, LPDWORD pD, ulong_xt dwN);
typedef void_xt(NTAPI *pfXcDESKeyParity)(PUCHAR pbKey, ulong_xt dwKeyLength);
typedef void_xt(NTAPI *pfXcKeyTable)(ulong_xt dwCipher, PUCHAR pbKeyTable, PUCHAR pbKey);
typedef void_xt(NTAPI *pfXcBlockCrypt)(ulong_xt dwCipher, PUCHAR pbOutput, PUCHAR pbInput, PUCHAR pbKeyTable, ulong_xt dwOp);
typedef void_xt(NTAPI *pfXcBlockCryptCBC)(ulong_xt dwCipher, ulong_xt dwInputLength, PUCHAR pbOutput, PUCHAR pbInput, PUCHAR pbKeyTable, ulong_xt dwOp, PUCHAR pbFeedback);
typedef ulong_xt(NTAPI *pfXcCryptService)(ulong_xt dwOp, PVOID pArgs);

/* Struct which contains all the pointers to the crypto functions */
typedef struct {
	pfXcSHAInit pXcSHAInit;
	pfXcSHAUpdate pXcSHAUpdate;
	pfXcSHAFinal pXcSHAFinal;
	pfXcRC4Key pXcRC4Key;
	pfXcRC4Crypt pXcRC4Crypt;
	pfXcHMAC pXcHMAC;
	pfXcPKEncPublic pXcPKEncPublic;
	pfXcPKDecPrivate pXcPKDecPrivate;
	pfXcPKGetKeyLen pXcPKGetKeyLen;
	pfXcVerifyPKCS1Signature pXcVerifyPKCS1Signature;
	pfXcModExp pXcModExp;
	pfXcDESKeyParity pXcDESKeyParity;
	pfXcKeyTable pXcKeyTable;
	pfXcBlockCrypt pXcBlockCrypt;
	pfXcBlockCryptCBC pXcBlockCryptCBC;
	pfXcCryptService pXcCryptService;
} CRYPTO_VECTOR, *PCRYPTO_VECTOR;

// ******************************************************************
// * 0x015F - XcUpdateCrypto()
// ******************************************************************
XBSYSAPI EXPORTNUM(351) void_xt NTAPI XcUpdateCrypto
(
	IN PCRYPTO_VECTOR pNewVector,
	OUT PCRYPTO_VECTOR pROMVector OPTIONAL
);

// Not exported by the kernel, but required to generate other keys
XBSYSAPI XBOX_KEY_DATA XboxCertificateKey;

// ******************************************************************
// * 0x0161 - XboxLANKey
// ******************************************************************
XBSYSAPI EXPORTNUM(353) XBOX_KEY_DATA XboxLANKey;

// ******************************************************************
// * 0x0162 - XboxAlternateSignatureKeys
// ******************************************************************
XBSYSAPI EXPORTNUM(354) XBOX_KEY_DATA XboxAlternateSignatureKeys[ALTERNATE_SIGNATURE_COUNT];

// ******************************************************************
// * 0x0163 - XePublicKeyData
// ******************************************************************
XBSYSAPI EXPORTNUM(355) uchar_xt XePublicKeyData[284];
XBSYSAPI EXPORTNUM(355) uchar_xt XePublicKeyDataRetail[284];
XBSYSAPI EXPORTNUM(355) uchar_xt XePublicKeyDataChihiroGame[284];
XBSYSAPI EXPORTNUM(355) uchar_xt XePublicKeyDataChihiroBoot[284];

// ******************************************************************
// * 0x0165 - IdexChannelObject
// ******************************************************************
XBSYSAPI EXPORTNUM(357) IDE_CHANNEL_OBJECT IdexChannelObject;

// ******************************************************************
// * 0x0169 - RtlSnprintf()
// ******************************************************************
XBSYSAPI EXPORTNUM(361) int_xt CDECL RtlSnprintf
(
	IN PCHAR string,
	IN size_xt count,
	IN LPCCH format,
	...
);

// ******************************************************************
// * 0x016A - RtlSprintf()
// ******************************************************************
XBSYSAPI EXPORTNUM(362) int_xt CDECL RtlSprintf
(
	IN PCHAR string,
	IN LPCCH format,
	...
);

// ******************************************************************
// * 0x016B - RtlVsnprintf()
// ******************************************************************
XBSYSAPI EXPORTNUM(363) int_xt CDECL RtlVsnprintf
(
	IN PCHAR string,
	IN size_xt count,
	IN LPCCH format,
	...
);

// ******************************************************************
// * 0x016C - RtlVsprintf()
// ******************************************************************
XBSYSAPI EXPORTNUM(364) int_xt CDECL RtlVsprintf
(
	IN PCHAR string,
	IN LPCCH format,
	...
);

XBSYSAPI EXPORTNUM(367) ntstatus_xt NTAPI UnknownAPI367
(
	// UNKNOWN ARGUMENTS
);

XBSYSAPI EXPORTNUM(368) ntstatus_xt NTAPI UnknownAPI368
(
	// UNKNOWN ARGUMENTS
);

XBSYSAPI EXPORTNUM(369) ntstatus_xt NTAPI UnknownAPI369
(
	// UNKNOWN ARGUMENTS
);

XBSYSAPI EXPORTNUM(370) ntstatus_xt NTAPI XProfpControl // PROFILING
(
	ulong_xt Action,
	ulong_xt Param
);

XBSYSAPI EXPORTNUM(371) ntstatus_xt NTAPI XProfpGetData // PROFILING 
(
	// NO ARGUMENTS
);

XBSYSAPI EXPORTNUM(372) ntstatus_xt NTAPI IrtClientInitFast // PROFILING
(
	// UNKNOWN ARGUMENTS
);

XBSYSAPI EXPORTNUM(373) ntstatus_xt NTAPI IrtSweep // PROFILING
(
	// UNKNOWN ARGUMENTS
);

}

#endif
