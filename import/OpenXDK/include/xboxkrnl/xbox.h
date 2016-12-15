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
// * AvGetSavedDataAddress
// ******************************************************************
XBSYSAPI EXPORTNUM(1) PVOID NTAPI AvGetSavedDataAddress(void);

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
XBSYSAPI EXPORTNUM(35) ULONG NTAPI FscGetCacheSize();

XBSYSAPI EXPORTNUM(36) VOID NTAPI FscInvalidateIdleBlocks();

// ******************************************************************
// * FscSetCacheSize
// ******************************************************************
XBSYSAPI EXPORTNUM(37) NTSTATUS NTAPI FscSetCacheSize
(
		ULONG NumberOfCachePages
);

XBSYSAPI EXPORTNUM(88) BOOLEAN KdDebuggerEnabled;
XBSYSAPI EXPORTNUM(89) BOOLEAN KdDebuggerNotPresent;

// ******************************************************************
// * KfRaiseIrql
// ******************************************************************
XBSYSAPI EXPORTNUM(160) UCHAR FASTCALL KfRaiseIrql
(
    IN UCHAR    NewIrql
);

// ******************************************************************
// * KfLowerIrql
// ******************************************************************
XBSYSAPI EXPORTNUM(161) VOID FASTCALL KfLowerIrql
(
    IN UCHAR    NewIrql
);

XBSYSAPI EXPORTNUM(162) ULONG_PTR KiBugCheckData[5];

XBSYSAPI EXPORTNUM(163) VOID FASTCALL KiUnlockDispatcherDatabase
(
	IN KIRQL OldIrql
);

// ******************************************************************
// * LaunchDataPage (actually a pointer)
// ******************************************************************
XBSYSAPI EXPORTNUM(164) PLAUNCH_DATA_PAGE LaunchDataPage;

XBSYSAPI EXPORTNUM(252) NTSTATUS NTAPI PhyGetLinkState
(
	IN ULONG	Mode
);

XBSYSAPI EXPORTNUM(253) NTSTATUS NTAPI PhyInitialize
(
	IN ULONG	Parameter1,
	IN ULONG	Parameter2
);

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


