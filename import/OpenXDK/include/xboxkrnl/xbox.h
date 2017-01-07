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

#define AV_FLAGS_60Hz                     0x00400000
#define AV_FLAGS_50Hz                     0x00800000
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
#define AV_QUERY_AV_CAPABILITIES		  6

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
XBSYSAPI EXPORTNUM(252) NTSTATUS NTAPI PhyGetLinkState
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

typedef PVOID PCRYPTO_VECTOR; // TODO : Expand

// ******************************************************************
// * 0x015F - XcUpdateCrypto()
// ******************************************************************
XBSYSAPI EXPORTNUM(351) VOID NTAPI XcUpdateCrypto
(
	IN PCRYPTO_VECTOR pNewVector,
	OUT PCRYPTO_VECTOR pROMVector OPTIONAL
);

// ******************************************************************
// * 0x0161 - XboxLANKey
// ******************************************************************
XBSYSAPI EXPORTNUM(353) XBOX_KEY_DATA XboxLANKey;

// ******************************************************************
// * 0x0162 - XboxAlternateSignatureKeys
// ******************************************************************
XBSYSAPI EXPORTNUM(354) XBOX_KEY_DATA XboxAlternateSignatureKeys;

// ******************************************************************
// * 0x0163 - XePublicKeyData
// ******************************************************************
XBSYSAPI EXPORTNUM(355) DWORD XePublicKeyData;

// ******************************************************************
// * 0x0165 - IdexChannelObject
// ******************************************************************
XBSYSAPI EXPORTNUM(357) BYTE IdexChannelObject[0x100];

XBSYSAPI EXPORTNUM(361) VOID *xsnprintf;     // prefixed with "x" to avoid xlibc collisions
XBSYSAPI EXPORTNUM(362) VOID *xsprintf;      // ""
XBSYSAPI EXPORTNUM(363) VOID *xvsnprintf;    // ""
XBSYSAPI EXPORTNUM(364) VOID *xvsprintf;     // ""

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
	// UNKNOWN ARGUMENTS
);

XBSYSAPI EXPORTNUM(371) NTSTATUS NTAPI XProfpGetData // PROFILING 
(
	// UNKNOWN ARGUMENTS
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


