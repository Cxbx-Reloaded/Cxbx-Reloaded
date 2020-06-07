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

VOID InitializeFscCacheEvent();

// ******************************************************************
// * 0x0001 - AvGetSavedDataAddress()
// ******************************************************************
XBSYSAPI EXPORTNUM(1) PVOID NTAPI AvGetSavedDataAddress(void);

// ******************************************************************
// * 0x0002 - AvSendTVEncoderOption()
// ******************************************************************
XBSYSAPI EXPORTNUM(2) VOID NTAPI AvSendTVEncoderOption
(
    IN  PVOID   RegisterBase,
    IN  ULONG   Option,
    IN  ULONG   Param,
    OUT ULONG   *Result
);

// ******************************************************************
// * 0x0003 - AvSetDisplayMode()
// ******************************************************************
XBSYSAPI EXPORTNUM(3) ULONG NTAPI AvSetDisplayMode
(
    IN PVOID    RegisterBase,
    IN ULONG    Step,
    IN ULONG    Mode,
    IN ULONG    Format,
    IN ULONG    Pitch,
    IN ULONG    FrameBuffer
);

// ******************************************************************
// * 0x0004 - AvSetSavedDataAddress()
// ******************************************************************
XBSYSAPI EXPORTNUM(4) VOID NTAPI AvSetSavedDataAddress
(
    IN PVOID    Address
);

// ******************************************************************
// * 0x0023 - FscGetCacheSize()
// ******************************************************************
XBSYSAPI EXPORTNUM(35) ULONG NTAPI FscGetCacheSize();

// ******************************************************************
// * 0x0024 - FscInvalidateIdleBlocks()
// ******************************************************************
XBSYSAPI EXPORTNUM(36) VOID NTAPI FscInvalidateIdleBlocks();

// ******************************************************************
// * 0x0025 - FscSetCacheSize()
// ******************************************************************
XBSYSAPI EXPORTNUM(37) NTSTATUS NTAPI FscSetCacheSize
(
		ULONG NumberOfCachePages
);

// ******************************************************************
// * 0x0058 - KdDebuggerEnabled
// ******************************************************************
XBSYSAPI EXPORTNUM(88) BOOLEAN KdDebuggerEnabled;

// ******************************************************************
// * 0x0059 - KdDebuggerNotPresent
// ******************************************************************
XBSYSAPI EXPORTNUM(89) BOOLEAN KdDebuggerNotPresent;

// ******************************************************************
// * 0x00A0 - KfRaiseIrql()
// ******************************************************************
XBSYSAPI EXPORTNUM(160) UCHAR FASTCALL KfRaiseIrql
(
    IN UCHAR    NewIrql
);

// ******************************************************************
// * 0x00A1 - KfLowerIrql()
// ******************************************************************
XBSYSAPI EXPORTNUM(161) VOID FASTCALL KfLowerIrql
(
    IN UCHAR    NewIrql
);

// ******************************************************************
// * 0x00A2 - KiBugCheckData
// ******************************************************************
XBSYSAPI EXPORTNUM(162) ULONG_PTR KiBugCheckData[5];

void KiLockDispatcherDatabase 
(
	OUT KIRQL* OldIrql
);

// ******************************************************************
// * 0x00A3 - KiUnlockDispatcherDatabase()
// ******************************************************************
XBSYSAPI EXPORTNUM(163) VOID FASTCALL KiUnlockDispatcherDatabase
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
XBSYSAPI EXPORTNUM(252) DWORD NTAPI PhyGetLinkState
(
	IN ULONG	Mode
);

// ******************************************************************
// * 0x00FD - PhyInitialize()
// ******************************************************************
XBSYSAPI EXPORTNUM(253) NTSTATUS NTAPI PhyInitialize
(
	IN ULONG	forceReset,
	IN PVOID	Parameter2
);

// ******************************************************************
// * 0x0141 - XboxEEPROMKey
// ******************************************************************
XBSYSAPI EXPORTNUM(321) UCHAR XboxEEPROMKey[16];

// ******************************************************************
// * 0x0142 - XboxHardwareInfo
// ******************************************************************
XBSYSAPI EXPORTNUM(322) XBOX_HARDWARE_INFO XboxHardwareInfo;

// ******************************************************************
// * 0x0143 - XboxHDKey
// ******************************************************************
XBSYSAPI EXPORTNUM(323) UCHAR XboxHDKey[16];

typedef struct _XBOX_KRNL_VERSION {
    USHORT Major;
    USHORT Minor;
    USHORT Build;
    USHORT Qfe;
} XBOX_KRNL_VERSION;

// ******************************************************************
// * 0x0144 - XboxKrnlVersion
// ******************************************************************
XBSYSAPI EXPORTNUM(324) XBOX_KRNL_VERSION XboxKrnlVersion;

// ******************************************************************
// * 0x0145 - XboxSignatureKey
// ******************************************************************
XBSYSAPI EXPORTNUM(325) BYTE XboxSignatureKey[16];

// ******************************************************************
// * 0x0146 - XeImageFileName
// ******************************************************************
XBSYSAPI EXPORTNUM(326) OBJECT_STRING XeImageFileName;

// ******************************************************************
// * 0x0147 - XeLoadSection()
// ******************************************************************
XBSYSAPI EXPORTNUM(327) NTSTATUS NTAPI XeLoadSection
(
	IN PXBEIMAGE_SECTION Section
);

// ******************************************************************
// * 0x0148 - XeUnloadSection()
// ******************************************************************
XBSYSAPI EXPORTNUM(328) NTSTATUS NTAPI XeUnloadSection
(
	IN PXBEIMAGE_SECTION Section
);

// ******************************************************************
// * 0x014F - XcSHAInit()
// ******************************************************************
XBSYSAPI EXPORTNUM(335) VOID NTAPI XcSHAInit
(
	IN PUCHAR pbSHAContext
);

// ******************************************************************
// * 0x0150 - XcSHAUpdate()
// ******************************************************************
XBSYSAPI EXPORTNUM(336) VOID NTAPI XcSHAUpdate
(
	IN PUCHAR pbSHAContext,
	IN PUCHAR pbInput,
	IN ULONG dwInputLength
);

// ******************************************************************
// * 0x0151 - XcSHAFinal()
// ******************************************************************
XBSYSAPI EXPORTNUM(337) VOID NTAPI XcSHAFinal
(
	IN PUCHAR pbSHAContext,
	IN PUCHAR pbDigest
);

// ******************************************************************
// * 0x0152 - XcRC4Key()
// ******************************************************************
XBSYSAPI EXPORTNUM(338) VOID NTAPI XcRC4Key
(
	IN PUCHAR pbKeyStruct,
	IN ULONG dwKeyLength,
	IN PUCHAR pbKey
);

// ******************************************************************
// * 0x0153 - XcRC4Crypt()
// ******************************************************************
XBSYSAPI EXPORTNUM(339) VOID NTAPI XcRC4Crypt
(
	IN PUCHAR pbKeyStruct,
	IN ULONG dwInputLength,
	IN PUCHAR pbInput
);

// ******************************************************************
// * 0x0154 - XcHMAC()
// ******************************************************************
XBSYSAPI EXPORTNUM(340) VOID NTAPI XcHMAC
(
	IN PBYTE pbKeyMaterial,
	IN ULONG cbKeyMaterial,
	IN PBYTE pbData,
	IN ULONG cbData,
	IN PBYTE pbData2,
	IN ULONG cbData2,
	OUT PBYTE HmacData
);

// ******************************************************************
// * 0x0155 - XcPKEncPublic()
// ******************************************************************
XBSYSAPI EXPORTNUM(341) ULONG NTAPI XcPKEncPublic
(
	IN PUCHAR pbPubKey,
	IN PUCHAR pbInput,
	OUT PUCHAR pbOutput
);

// ******************************************************************
// * 0x0156 - XcPKDecPrivate()
// ******************************************************************
XBSYSAPI EXPORTNUM(342) ULONG NTAPI XcPKDecPrivate
(
	IN PUCHAR pbPrvKey,
	IN PUCHAR pbInput,
	OUT PUCHAR pbOutput
);

// ******************************************************************
// * 0x0157 - XcPKGetKeyLen()
// ******************************************************************
XBSYSAPI EXPORTNUM(343) ULONG NTAPI XcPKGetKeyLen
(
	OUT PUCHAR pbPubKey
);

// ******************************************************************
// * 0x0158 - XcVerifyPKCS1Signature()
// ******************************************************************
XBSYSAPI EXPORTNUM(344) BOOLEAN NTAPI XcVerifyPKCS1Signature
(
	IN PUCHAR pbSig,
	IN PUCHAR pbPubKey,
	IN PUCHAR pbDigest
);

// ******************************************************************
// * 0x0159 - XcModExp()
// ******************************************************************
XBSYSAPI EXPORTNUM(345) ULONG NTAPI XcModExp
(
	IN LPDWORD pA,
	IN LPDWORD pB,
	IN LPDWORD pC,
	IN LPDWORD pD,
	IN ULONG dwN
);

// ******************************************************************
// * 0x015A - XcDESKeyParity()
// ******************************************************************
XBSYSAPI EXPORTNUM(346) VOID NTAPI XcDESKeyParity
(
	IN PUCHAR pbKey,
	IN ULONG dwKeyLength
);

// ******************************************************************
// * 0x015B - XcKeyTable()
// ******************************************************************
XBSYSAPI EXPORTNUM(347) VOID NTAPI XcKeyTable
(
	IN ULONG dwCipher,
	OUT PUCHAR pbKeyTable,
	IN PUCHAR pbKey
);

// ******************************************************************
// * 0x015C - XcBlockCrypt()
// ******************************************************************
XBSYSAPI EXPORTNUM(348) VOID NTAPI XcBlockCrypt
(
	IN ULONG dwCipher,
	OUT PUCHAR pbOutput,
	IN PUCHAR pbInput,
	IN PUCHAR pbKeyTable,
	IN ULONG dwOp
);

// ******************************************************************
// * 0x015D - XcBlockCryptCBC()
// ******************************************************************
XBSYSAPI EXPORTNUM(349) VOID NTAPI XcBlockCryptCBC
(
	IN ULONG dwCipher,
	IN ULONG dwInputLength,
	OUT PUCHAR pbOutput,
	IN PUCHAR pbInput,
	IN PUCHAR pbKeyTable,
	IN ULONG dwOp,
	IN PUCHAR pbFeedback
);

// ******************************************************************
// * 0x015E - XcCryptService()
// ******************************************************************
XBSYSAPI EXPORTNUM(350) ULONG NTAPI XcCryptService
(
	IN ULONG dwOp,
	IN PVOID pArgs
);

/* Function pointers which point to all the kernel crypto functions. Used by PCRYPTO_VECTOR. */
typedef VOID(NTAPI *pfXcSHAInit)(PUCHAR pbSHAContext);
typedef VOID(NTAPI *pfXcSHAUpdate)(PUCHAR pbSHAContext, PUCHAR pbInput, ULONG dwInputLength);
typedef VOID(NTAPI *pfXcSHAFinal)(PUCHAR pbSHAContext, PUCHAR pbDigest);
typedef VOID(NTAPI *pfXcRC4Key)(PUCHAR pbKeyStruct, ULONG dwKeyLength, PUCHAR pbKey);
typedef VOID(NTAPI *pfXcRC4Crypt)(PUCHAR pbKeyStruct, ULONG dwInputLength, PUCHAR pbInput);
typedef VOID(NTAPI *pfXcHMAC)(PBYTE pbKeyMaterial, ULONG cbKeyMaterial, PBYTE pbData, ULONG cbData, PBYTE pbData2, ULONG cbData2, PBYTE HmacData);
typedef ULONG(NTAPI *pfXcPKEncPublic)(PUCHAR pbPubKey, PUCHAR pbInput, PUCHAR pbOutput);
typedef ULONG(NTAPI *pfXcPKDecPrivate)(PUCHAR pbPrvKey, PUCHAR pbInput, PUCHAR pbOutput);
typedef ULONG(NTAPI *pfXcPKGetKeyLen)(PUCHAR pbPubKey);
typedef BOOLEAN(NTAPI *pfXcVerifyPKCS1Signature)(PUCHAR pbSig, PUCHAR pbPubKey, PUCHAR pbDigest);
typedef ULONG(NTAPI *pfXcModExp)(LPDWORD pA, LPDWORD pB, LPDWORD pC, LPDWORD pD, ULONG dwN);
typedef VOID(NTAPI *pfXcDESKeyParity)(PUCHAR pbKey, ULONG dwKeyLength);
typedef VOID(NTAPI *pfXcKeyTable)(ULONG dwCipher, PUCHAR pbKeyTable, PUCHAR pbKey);
typedef VOID(NTAPI *pfXcBlockCrypt)(ULONG dwCipher, PUCHAR pbOutput, PUCHAR pbInput, PUCHAR pbKeyTable, ULONG dwOp);
typedef VOID(NTAPI *pfXcBlockCryptCBC)(ULONG dwCipher, ULONG dwInputLength, PUCHAR pbOutput, PUCHAR pbInput, PUCHAR pbKeyTable, ULONG dwOp, PUCHAR pbFeedback);
typedef ULONG(NTAPI *pfXcCryptService)(ULONG dwOp, PVOID pArgs);

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
XBSYSAPI EXPORTNUM(351) VOID NTAPI XcUpdateCrypto
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
XBSYSAPI EXPORTNUM(355) UCHAR XePublicKeyData[284];
XBSYSAPI EXPORTNUM(355) UCHAR XePublicKeyDataRetail[284];
XBSYSAPI EXPORTNUM(355) UCHAR XePublicKeyDataChihiroGame[284];
XBSYSAPI EXPORTNUM(355) UCHAR XePublicKeyDataChihiroBoot[284];

// ******************************************************************
// * 0x0165 - IdexChannelObject
// ******************************************************************
XBSYSAPI EXPORTNUM(357) IDE_CHANNEL_OBJECT IdexChannelObject;

// ******************************************************************
// * 0x0169 - RtlSnprintf()
// ******************************************************************
XBSYSAPI EXPORTNUM(361) INT CDECL RtlSnprintf
(
	IN PCHAR string,
	IN SIZE_T count,
	IN LPCCH format,
	...
);

// ******************************************************************
// * 0x016A - RtlSprintf()
// ******************************************************************
XBSYSAPI EXPORTNUM(362) INT CDECL RtlSprintf
(
	IN PCHAR string,
	IN LPCCH format,
	...
);

// ******************************************************************
// * 0x016B - RtlVsnprintf()
// ******************************************************************
XBSYSAPI EXPORTNUM(363) INT CDECL RtlVsnprintf
(
	IN PCHAR string,
	IN SIZE_T count,
	IN LPCCH format,
	...
);

// ******************************************************************
// * 0x016C - RtlVsprintf()
// ******************************************************************
XBSYSAPI EXPORTNUM(364) INT CDECL RtlVsprintf
(
	IN PCHAR string,
	IN LPCCH format,
	...
);

XBSYSAPI EXPORTNUM(367) NTSTATUS NTAPI UnknownAPI367
(
	// UNKNOWN ARGUMENTS
);

XBSYSAPI EXPORTNUM(368) NTSTATUS NTAPI UnknownAPI368
(
	// UNKNOWN ARGUMENTS
);

XBSYSAPI EXPORTNUM(369) NTSTATUS NTAPI UnknownAPI369
(
	// UNKNOWN ARGUMENTS
);

XBSYSAPI EXPORTNUM(370) NTSTATUS NTAPI XProfpControl // PROFILING
(
	ULONG Action,
	ULONG Param
);

XBSYSAPI EXPORTNUM(371) NTSTATUS NTAPI XProfpGetData // PROFILING 
(
	// NO ARGUMENTS
);

XBSYSAPI EXPORTNUM(372) NTSTATUS NTAPI IrtClientInitFast // PROFILING
(
	// UNKNOWN ARGUMENTS
);

XBSYSAPI EXPORTNUM(373) NTSTATUS NTAPI IrtSweep // PROFILING
(
	// UNKNOWN ARGUMENTS
);

#endif
