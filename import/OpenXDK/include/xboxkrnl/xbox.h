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

// ******************************************************************
// * AvGetSavedDataAddress
// ******************************************************************
XBSYSAPI EXPORTNUM(1) PVOID NTAPI AvGetSavedDataAddress();

// ******************************************************************
// * AvSendTVEncoderOption
// ******************************************************************
XBSYSAPI EXPORTNUM(2) VOID NTAPI AvSendTVEncoderOption
(
    IN  PVOID   RegisterBase,
    IN  ULONG   Option,
    IN  ULONG   Param,
    OUT ULONG   *Result
);

// ******************************************************************
// * AvSetDisplayMode
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
// * AvSetSavedDataAddress
// ******************************************************************
XBSYSAPI EXPORTNUM(4) VOID NTAPI AvSetSavedDataAddress
(
    IN PVOID    Address
);

// ******************************************************************
// * FscGetCacheSize
// ******************************************************************
XBSYSAPI EXPORTNUM(35) DWORD NTAPI FscGetCacheSize();

XBSYSAPI EXPORTNUM(36) VOID NTAPI FscInvalidateIdleBlocks();

// ******************************************************************
// * FscSetCacheSize
// ******************************************************************
XBSYSAPI EXPORTNUM(37) LONG NTAPI FscSetCacheSize(ULONG uCachePages);

XBSYSAPI EXPORTNUM(88) BOOLEAN KdDebuggerEnabled;
XBSYSAPI EXPORTNUM(89) BOOLEAN KdDebuggerNotPresent;

// ******************************************************************
// * KfRaiseIrql
// ******************************************************************
XBSYSAPI EXPORTNUM(160) UCHAR _fastcall KfRaiseIrql
(
    IN UCHAR    NewIrql
);

// ******************************************************************
// * KfLowerIrql
// ******************************************************************
XBSYSAPI EXPORTNUM(161) VOID _fastcall KfLowerIrql
(
    IN UCHAR    NewIrql
);

XBSYSAPI EXPORTNUM(162) ULONG_PTR KiBugCheckData[5];
XBSYSAPI VOID *KiUnlockDispatcherDatabase;

// ******************************************************************
// * LaunchDataPage (actually a pointer)
// ******************************************************************
XBSYSAPI EXPORTNUM(164) PLAUNCH_DATA_PAGE LaunchDataPage;

XBSYSAPI VOID *PhyGetLinkState;
XBSYSAPI VOID *PhyInitialize;

// ******************************************************************
// * XboxEEPROMKey
// ******************************************************************
XBSYSAPI EXPORTNUM(321) UCHAR XboxEEPROMKey[16];

// ******************************************************************
// * XboxHardwareInfo
// ******************************************************************
XBSYSAPI EXPORTNUM(322) XBOX_HARDWARE_INFO XboxHardwareInfo;

// ******************************************************************
// * XboxHDKey
// ******************************************************************
XBSYSAPI EXPORTNUM(323) UCHAR XboxHDKey[16];

typedef struct _XBOX_KRNL_VERSION {
    USHORT Major;
    USHORT Minor;
    USHORT Build;
    USHORT Qfe;
} XBOX_KRNL_VERSION;

XBSYSAPI EXPORTNUM(324) XBOX_KRNL_VERSION XboxKrnlVersion;

// ******************************************************************
// * XboxSignatureKey
// ******************************************************************
XBSYSAPI EXPORTNUM(325) BYTE XboxSignatureKey[16];

XBSYSAPI EXPORTNUM(326) OBJECT_STRING XeImageFileName;
XBSYSAPI EXPORTNUM(327) NTSTATUS NTAPI XeLoadSection(void* section);
XBSYSAPI EXPORTNUM(328) NTSTATUS NTAPI XeUnloadSection(void* section);

// ******************************************************************
// * XcSHAInit
// ******************************************************************
XBSYSAPI EXPORTNUM(335) VOID NTAPI XcSHAInit(UCHAR *pbSHAContext);

// ******************************************************************
// * XcSHAUpdate
// ******************************************************************
XBSYSAPI EXPORTNUM(336) VOID NTAPI XcSHAUpdate(UCHAR *pbSHAContext, UCHAR *pbInput, ULONG dwInputLength);

// ******************************************************************
// * XcSHAFinal
// ******************************************************************
XBSYSAPI EXPORTNUM(337) VOID NTAPI XcSHAFinal(UCHAR *pbSHAContext, UCHAR *pbDigest);

XBSYSAPI EXPORTNUM(338) VOID *XcRC4Key;
XBSYSAPI EXPORTNUM(339) VOID *XcRC4Crypt;

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

XBSYSAPI EXPORTNUM(341) VOID *XcPKEncPublic;
XBSYSAPI EXPORTNUM(342) VOID *XcPKDecPrivate;
XBSYSAPI EXPORTNUM(343) VOID *XcPKGetKeyLen;
XBSYSAPI EXPORTNUM(344) VOID *XcVerifyPKCS1Signature;
XBSYSAPI EXPORTNUM(345) VOID *XcModExp;
XBSYSAPI EXPORTNUM(346) VOID *XcDESKeyParity;
XBSYSAPI EXPORTNUM(347) VOID *XcKeyTable;
XBSYSAPI EXPORTNUM(348) VOID *XcBlockCrypt;
XBSYSAPI EXPORTNUM(349) VOID *XcBlockCryptCBC;
XBSYSAPI EXPORTNUM(350) VOID *XcCryptService;
XBSYSAPI EXPORTNUM(351) VOID *XcUpdateCrypto;
XBSYSAPI EXPORTNUM(353) XBOX_KEY_DATA XboxLANKey;
XBSYSAPI EXPORTNUM(354) XBOX_KEY_DATA XboxAlternateSignatureKeys;
XBSYSAPI EXPORTNUM(355) DWORD XePublicKeyData;

XBSYSAPI EXPORTNUM(357) BYTE IdexChannelObject[0x100];

XBSYSAPI EXPORTNUM(361) VOID *xsnprintf;     // prefixed with "x" to avoid xlibc collisions
XBSYSAPI EXPORTNUM(362) VOID *xsprintf;      // ""
XBSYSAPI EXPORTNUM(363) VOID *xvsnprintf;    // ""
XBSYSAPI EXPORTNUM(364) VOID *xvsprintf;     // ""

#endif


