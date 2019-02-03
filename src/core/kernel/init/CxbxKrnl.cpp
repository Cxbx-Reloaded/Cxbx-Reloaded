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
// *
// *  All rights reserved
// *
// ******************************************************************

#define _XBOXKRNL_DEFEXTRN_
#define LOG_PREFIX CXBXR_MODULE::CXBXR
#define LOG_PREFIX_INIT CXBXR_MODULE::INIT

/* prevent name collisions */
namespace xboxkrnl
{
    #include <xboxkrnl/xboxkrnl.h>
};

#include "gui\ResCxbx.h"
#include "core\kernel\init\CxbxKrnl.h"
#include "common\xbdm\CxbxXbdm.h" // For Cxbx_LibXbdmThunkTable
#include "CxbxVersion.h"
#include "core\kernel\support\Emu.h"
#include "devices\x86\EmuX86.h"
#include "core\kernel\support\EmuFile.h"
#include "core\kernel\support\EmuFS.h"
#include "EmuEEPROM.h" // For CxbxRestoreEEPROM, EEPROM, XboxFactoryGameRegion
#include "core\kernel\exports\EmuKrnl.h"
#include "core\kernel\exports\EmuKrnlKi.h"
#include "EmuShared.h"
#include "core\kernel\support\EmuXTL.h"
#include "core\hle\Intercept.hpp"
#include "ReservedMemory.h" // For virtual_memory_placeholder
#include "core\kernel\memory-manager\VMManager.h"
#include "CxbxDebugger.h"

#include <clocale>
#include <process.h>
#include <time.h> // For time()
#include <sstream> // For std::ostringstream

#include "devices\EEPROMDevice.h" // For g_EEPROM
#include "devices\Xbox.h" // For InitXboxHardware()
#include "devices\LED.h" // For LED::Sequence
#include "devices\SMCDevice.h" // For SMC Access
#include "common\crypto\EmuSha.h" // For the SHA1 functions
#include "Timer.h" // For Timer_Init
#include "..\Common\Input\InputConfig.h" // For the InputDeviceManager

/*! thread local storage */
Xbe::TLS *CxbxKrnl_TLS = NULL;
/*! thread local storage data */
void *CxbxKrnl_TLSData = NULL;
/*! xbe header structure */
Xbe::Header *CxbxKrnl_XbeHeader = NULL;
/*! parent window handle */

/*! indicates a debug kernel */
bool g_bIsDebugKernel = false;

HWND CxbxKrnl_hEmuParent = NULL;
DebugMode CxbxKrnl_DebugMode = DebugMode::DM_NONE;
std::string CxbxKrnl_DebugFileName = "";
Xbe::Certificate *g_pCertificate = NULL;

/*! thread handles */
static std::vector<HANDLE> g_hThreads;

char szFilePath_CxbxReloaded_Exe[MAX_PATH] = { 0 };
char szFolder_CxbxReloadedData[MAX_PATH] = { 0 };
char szFilePath_EEPROM_bin[MAX_PATH] = { 0 };
char szFilePath_memory_bin[MAX_PATH] = { 0 };
char szFilePath_page_tables[MAX_PATH] = { 0 };
char szFilePath_Xbe[MAX_PATH*2] = { 0 }; // NOTE: LAUNCH_DATA_HEADER's szLaunchPath is MAX_PATH*2 = 520

std::string CxbxBasePath;
HANDLE CxbxBasePathHandle;
Xbe* CxbxKrnl_Xbe = NULL;
XbeType g_XbeType = xtRetail;
bool g_bIsChihiro = false;
bool g_bIsDebug = false;
bool g_bIsRetail = false;
DWORD_PTR g_CPUXbox = 0;
DWORD_PTR g_CPUOthers = 0;

// Indicates to disable/enable all interrupts when cli and sti instructions are executed
std::atomic_bool g_bEnableAllInterrupts = true;

// Set by the VMManager during initialization. Exported because it's needed in other parts of the emu
size_t g_SystemMaxMemory = 0;

HANDLE g_CurrentProcessHandle = 0; // Set in CxbxKrnlMain
bool g_bIsWine = false;

bool g_CxbxPrintUEM = false;
ULONG g_CxbxFatalErrorCode = FATAL_ERROR_NONE;

// Define function located in EmuXApi so we can call it from here
void SetupXboxDeviceTypes();

// ported from Dxbx's XbeExplorer
XbeType GetXbeType(Xbe::Header *pXbeHeader)
{
	// Detect if the XBE is for Chihiro (Untested!) :
	// This is based on https://github.com/radare/radare2/blob/master/libr/bin/p/bin_xbe.c#L45
	if ((pXbeHeader->dwEntryAddr & 0xf0000000) == 0x40000000)
		return xtChihiro;

	// Check for Debug XBE, using high bit of the kernel thunk address :
	// (DO NOT test like https://github.com/radare/radare2/blob/master/libr/bin/p/bin_xbe.c#L49 !)
	if ((pXbeHeader->dwKernelImageThunkAddr & 0x80000000) > 0)
		return xtDebug;

	// Otherwise, the XBE is a Retail build :
	return xtRetail;
}

void ApplyMediaPatches()
{
	// Patch the XBE Header to allow running from all media types
	g_pCertificate->dwAllowedMedia |= 0
		| XBEIMAGE_MEDIA_TYPE_HARD_DISK
		| XBEIMAGE_MEDIA_TYPE_DVD_X2
		| XBEIMAGE_MEDIA_TYPE_DVD_CD
		| XBEIMAGE_MEDIA_TYPE_CD
		| XBEIMAGE_MEDIA_TYPE_DVD_5_RO
		| XBEIMAGE_MEDIA_TYPE_DVD_9_RO
		| XBEIMAGE_MEDIA_TYPE_DVD_5_RW
		| XBEIMAGE_MEDIA_TYPE_DVD_9_RW
		;
	// Patch the XBE Header to allow running on all regions
	g_pCertificate->dwGameRegion = 0
		| XBEIMAGE_GAME_REGION_MANUFACTURING
		| XBEIMAGE_GAME_REGION_NA
		| XBEIMAGE_GAME_REGION_JAPAN
		| XBEIMAGE_GAME_REGION_RESTOFWORLD
		;
	// Patch the XBE Security Flag
	// This field is only present if the Xbe Size is >= than our Certificate Structure
	// This works as our structure is large enough to fit the newer certificate size, 
	// while dwSize is the actual size of the certificate in the Xbe.
	// Source: Various Hacked Kernels
	if (g_pCertificate->dwSize >= sizeof(Xbe::Certificate)) {
		g_pCertificate->dwSecurityFlags &= ~1;
	}
}

void SetupPerTitleKeys()
{
	// Generate per-title keys from the XBE Certificate
	UCHAR Digest[20] = {};

	// Set the LAN Key
	xboxkrnl::XcHMAC(xboxkrnl::XboxCertificateKey, xboxkrnl::XBOX_KEY_LENGTH, g_pCertificate->bzLanKey, xboxkrnl::XBOX_KEY_LENGTH, NULL, 0, Digest);
	memcpy(xboxkrnl::XboxLANKey, Digest, xboxkrnl::XBOX_KEY_LENGTH);

	// Signature Key
	xboxkrnl::XcHMAC(xboxkrnl::XboxCertificateKey, xboxkrnl::XBOX_KEY_LENGTH, g_pCertificate->bzSignatureKey, xboxkrnl::XBOX_KEY_LENGTH, NULL, 0, Digest);
	memcpy(xboxkrnl::XboxSignatureKey, Digest, xboxkrnl::XBOX_KEY_LENGTH);

	// Alternate Signature Keys
	for (int i = 0; i < xboxkrnl::ALTERNATE_SIGNATURE_COUNT; i++) {
		xboxkrnl::XcHMAC(xboxkrnl::XboxCertificateKey, xboxkrnl::XBOX_KEY_LENGTH, g_pCertificate->bzTitleAlternateSignatureKey[i], xboxkrnl::XBOX_KEY_LENGTH, NULL, 0, Digest);
		memcpy(xboxkrnl::XboxAlternateSignatureKeys[i], Digest, xboxkrnl::XBOX_KEY_LENGTH);
	}

}

void CxbxLaunchXbe(void(*Entry)())
{
	__try
	{
		Entry();
	}
	__except (EmuException(GetExceptionInformation()))
	{
		EmuLog(LOG_LEVEL::WARNING, "Problem with ExceptionFilter");
	}
}

// Entry point address XOR keys per Xbe type (Retail, Debug or Chihiro) :
const DWORD XOR_EP_KEY[3] = { XOR_EP_RETAIL, XOR_EP_DEBUG, XOR_EP_CHIHIRO };
// Kernel thunk address XOR keys per Xbe type (Retail, Debug or Chihiro) :
const DWORD XOR_KT_KEY[3] = { XOR_KT_RETAIL, XOR_KT_DEBUG, XOR_KT_CHIHIRO };

// Executable image header pointers (it's contents can be switched between
// Exe-compatibility and Xbe-identical mode, using RestoreExeImageHeader
// vs RestoreXbeImageHeader) :
const PIMAGE_DOS_HEADER ExeDosHeader = (PIMAGE_DOS_HEADER)XBE_IMAGE_BASE;
PIMAGE_NT_HEADERS ExeNtHeader = nullptr;
PIMAGE_OPTIONAL_HEADER ExeOptionalHeader = nullptr;

// Copy of original executable image headers, used both as backup and valid replacement structure :
PIMAGE_DOS_HEADER NewDosHeader = nullptr;
PIMAGE_NT_HEADERS NewNtHeader = nullptr;
PIMAGE_OPTIONAL_HEADER NewOptionalHeader = nullptr;

// Xbe backup values. RestoreXbeImageHeader place these into ExeHeader to restore loaded Xbe contents.
WORD Xbe_magic = 0;
LONG Xbe_lfanew = 0;
IMAGE_DATA_DIRECTORY Xbe_TLS = { };

// Remember the current XBE contents of the executable image
// header fields that RestoreExeImageHeader needs to restore.
void StoreXbeImageHeader()
{
	Xbe_magic = ExeDosHeader->e_magic; // Normally 0x4258 = 'XB'; (...'EH')
	Xbe_lfanew = ExeDosHeader->e_lfanew;
	Xbe_TLS = ExeOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];
}

// Restore memory to the exact contents as loaded from the current XBE.
// Avoid threadswitches and calling Windows API's while this in effect
// because those can fail. Hence, RestoreExeImageHeader quickly again!
void RestoreXbeImageHeader()
{
	ExeDosHeader->e_magic = Xbe_magic; // Sets XbeHeader.dwMagic
	ExeDosHeader->e_lfanew = Xbe_lfanew; // Sets part of XbeHeader.pbDigitalSignature
	ExeOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS] = Xbe_TLS;
}

// Restore memory to the exact contents loaded from the running EXE.
// This is required to keep thread-switching and Windows API's working.
void RestoreExeImageHeader()
{
	ExeDosHeader->e_magic = NewDosHeader->e_magic; // = 0x5A4D = 'MZ'; Overwrites XbeHeader.dwMagic
	ExeDosHeader->e_lfanew = NewDosHeader->e_lfanew; // Overwrites part of XbeHeader.pbDigitalSignature
	ExeOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS] = NewOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];
}

typedef const char* (CDECL *LPFN_WINEGETVERSION)(void);
LPFN_WINEGETVERSION wine_get_version;

// Forward declaration to avoid moving the definition of LoadXboxKeys
void LoadXboxKeys(std::string path);

// Returns the Win32 error in string format. Returns an empty string if there is no error.
std::string CxbxGetErrorCodeAsString(DWORD errorCode)
{
	std::string result;
	LPSTR lpMessageBuffer = nullptr;
	DWORD dwLength = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, // lpSource
		errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpMessageBuffer,
		0, // nSize
		NULL); // Arguments
	if (dwLength > 0) {
		result = std::string(lpMessageBuffer, dwLength);
	}

	LocalFree(lpMessageBuffer);
	return result;
}

// Returns the last Win32 error, in string format. Returns an empty string if there is no error.
std::string CxbxGetLastErrorString(char * lpszFunction)
{
	DWORD errorCode = ::GetLastError(); // Do this first, before any following code changes it
	std::string result = "No error";
	if (errorCode > 0) {
		std::ostringstream stringStream;
		stringStream << lpszFunction << " failed with error " << errorCode << ": " << CxbxGetErrorCodeAsString(errorCode);
		result = stringStream.str();
	}

	return result;
}

HANDLE CxbxRestoreContiguousMemory(char *szFilePath_memory_bin)
{
	// First, try to open an existing memory.bin file :
	HANDLE hFile = CreateFile(szFilePath_memory_bin,
		GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		/* lpSecurityAttributes */nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, // FILE_FLAG_WRITE_THROUGH
		/* hTemplateFile */nullptr);

	bool NeedsInitialization = (hFile == INVALID_HANDLE_VALUE);
	if (NeedsInitialization)
	{
		// If the memory.bin file doesn't exist yet, create it :
		hFile = CreateFile(szFilePath_memory_bin,
			GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			/* lpSecurityAttributes */nullptr,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, // FILE_FLAG_WRITE_THROUGH
			/* hTemplateFile */nullptr);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			CxbxKrnlCleanup("%s : Couldn't create memory.bin file!\n", __func__);
			return nullptr;
		}
	}

	// Make sure memory.bin is at least 128 MB in size
	SetFilePointer(hFile, CHIHIRO_MEMORY_SIZE, nullptr, FILE_BEGIN);
	SetEndOfFile(hFile);

	HANDLE hFileMapping = CreateFileMapping(
		hFile,
		/* lpFileMappingAttributes */nullptr,
		PAGE_EXECUTE_READWRITE,
		/* dwMaximumSizeHigh */0,
		/* dwMaximumSizeLow */CHIHIRO_MEMORY_SIZE,
		/**/nullptr);
	if (hFileMapping == NULL)
	{
		CxbxKrnlCleanup("%s : Couldn't create contiguous memory.bin file mapping!\n", __func__);
		return nullptr;
	}

	LARGE_INTEGER  len_li;
	GetFileSizeEx(hFile, &len_li);
	unsigned int FileSize = len_li.u.LowPart;
	if (FileSize != CHIHIRO_MEMORY_SIZE)
	{
		CxbxKrnlCleanup("%s : memory.bin file is not 128 MiB large!\n", __func__);
		return nullptr;
	}


#ifdef CXBX_LOADER
	// TODO : Use ReserveMemoryRange / UnreserveMemoryRange(Mem??) where appropriate
#endif

	// Map memory.bin contents into memory :
	void *memory = (void *)MapViewOfFileEx(
		hFileMapping,
		FILE_MAP_READ | FILE_MAP_WRITE | FILE_MAP_EXECUTE,
		/* dwFileOffsetHigh */0,
		/* dwFileOffsetLow */0,
		CHIHIRO_CONTIGUOUS_MEMORY_SIZE,
		(void *)CONTIGUOUS_MEMORY_BASE);
	if (memory != (void *)CONTIGUOUS_MEMORY_BASE)
	{
		if (memory)
			UnmapViewOfFile(memory);

		CxbxKrnlCleanup("%s: Couldn't map contiguous memory.bin to 0x80000000!", __func__);
		return nullptr;
	}

	printf("[0x%.4X] INIT: Mapped %d MiB of Xbox contiguous memory at 0x%.8X to 0x%.8X\n",
		GetCurrentThreadId(), CHIHIRO_CONTIGUOUS_MEMORY_SIZE / ONE_MB, CONTIGUOUS_MEMORY_BASE, CONTIGUOUS_MEMORY_BASE + CHIHIRO_CONTIGUOUS_MEMORY_SIZE - 1);

	if (NeedsInitialization)
	{
		memset(memory, 0, CHIHIRO_CONTIGUOUS_MEMORY_SIZE);
		printf("[0x%.4X] INIT: Initialized contiguous memory\n", GetCurrentThreadId());
	}
	else
		printf("[0x%.4X] INIT: Loaded contiguous memory.bin\n", GetCurrentThreadId());

	size_t tiledMemorySize = XBOX_WRITE_COMBINED_SIZE;
	if (g_bIsWine) {
		printf("Wine detected: Using 64MB Tiled Memory Size\n");
		// TODO: Figure out why Wine needs this and Windows doesn't.
		// Perhaps it's a Wine bug, or perhaps Wine reserves this memory for it's own usage?
		tiledMemorySize = XBOX_WRITE_COMBINED_SIZE / 2;
	}

	// Map memory.bin contents into tiled memory too :
	void *tiled_memory = (void *)MapViewOfFileEx(
		hFileMapping,
		FILE_MAP_READ | FILE_MAP_WRITE,
		/* dwFileOffsetHigh */0,
		/* dwFileOffsetLow */0,
		tiledMemorySize,
		(void *)XBOX_WRITE_COMBINED_BASE);

	if (tiled_memory != (void *)XBOX_WRITE_COMBINED_BASE)
	{
		if (tiled_memory)
			UnmapViewOfFile(tiled_memory);

		CxbxKrnlCleanup("%s: Couldn't map contiguous memory.bin into tiled memory at 0xF0000000!", __func__);
		return nullptr;
	}

	printf("[0x%.4X] INIT: Mapped contiguous memory to Xbox tiled memory at 0x%.8X to 0x%.8X\n",
		GetCurrentThreadId(), XBOX_WRITE_COMBINED_BASE, XBOX_WRITE_COMBINED_BASE + tiledMemorySize - 1);


	return hFileMapping;
}

HANDLE CxbxRestorePageTablesMemory(char* szFilePath_page_tables)
{
	// First, try to open an existing PageTables.bin file :
	HANDLE hFile = CreateFile(szFilePath_page_tables,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		/* lpSecurityAttributes */nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, // FILE_FLAG_WRITE_THROUGH
		/* hTemplateFile */nullptr);

	bool NeedsInitialization = (hFile == INVALID_HANDLE_VALUE);
	if (NeedsInitialization)
	{
		// If the PageTables.bin file doesn't exist yet, create it :
		hFile = CreateFile(szFilePath_page_tables,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		/* lpSecurityAttributes */nullptr,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, // FILE_FLAG_WRITE_THROUGH
		/* hTemplateFile */nullptr);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			CxbxKrnlCleanup("%s : Couldn't create PageTables.bin file!\n", __func__);
		}
	}

	// Make sure PageTables.bin is at least 4 MB in size
	SetFilePointer(hFile, PAGE_TABLES_SIZE, nullptr, FILE_BEGIN);
	SetEndOfFile(hFile);

	HANDLE hFileMapping = CreateFileMapping(
	hFile,
	/* lpFileMappingAttributes */nullptr,
	PAGE_READWRITE,
	/* dwMaximumSizeHigh */0,
	/* dwMaximumSizeLow */PAGE_TABLES_SIZE,
	/**/nullptr);
	if (hFileMapping == NULL)
	{
		CxbxKrnlCleanup("%s : Couldn't create PageTables.bin file mapping!\n", __func__);
	}

	LARGE_INTEGER  len_li;
	GetFileSizeEx(hFile, &len_li);
	unsigned int FileSize = len_li.u.LowPart;
	if (FileSize != PAGE_TABLES_SIZE)
	{
		CxbxKrnlCleanup("%s : PageTables.bin file is not 4 MiB large!\n", __func__);
	}

	// Map PageTables.bin contents into memory :
	void *memory = (void *)MapViewOfFileEx(
	hFileMapping,
	FILE_MAP_READ | FILE_MAP_WRITE,
	/* dwFileOffsetHigh */0,
	/* dwFileOffsetLow */0,
	4 * ONE_MB,
	(void *)PAGE_TABLES_BASE);
	if (memory != (void *)PAGE_TABLES_BASE)
	{
		if (memory)
			UnmapViewOfFile(memory);

		CxbxKrnlCleanup("%s: Couldn't map PageTables.bin to 0xC0000000!", __func__);
	}

	printf("[0x%.4X] INIT: Mapped %d MiB of Xbox page tables memory at 0x%.8X to 0x%.8X\n",
		GetCurrentThreadId(), 4, PAGE_TABLES_BASE, PAGE_TABLES_END);

	if (NeedsInitialization)
	{
		memset(memory, 0, 4 * ONE_MB);
		printf("[0x%.4X] INIT: Initialized page tables memory\n", GetCurrentThreadId());
	}
	else
		printf("[0x%.4X] INIT: Loaded PageTables.bin\n", GetCurrentThreadId());

	return hFileMapping;
}

#pragma optimize("", off)

void CxbxPopupMessageEx(CXBXR_MODULE cxbxr_module, LOG_LEVEL level, CxbxMsgDlgIcon icon, const char *message, ...)
{
	char Buffer[1024];
	va_list argp;
    UINT uType = MB_OK | MB_TOPMOST | MB_SETFOREGROUND;

    switch (icon) {
        case CxbxMsgDlgIcon_Warn: {
            uType |= MB_ICONWARNING;
            break;
        }
        case CxbxMsgDlgIcon_Error: {
            uType |= MB_ICONERROR;
            break;
        }
        case CxbxMsgDlgIcon_Info: {
            uType |= MB_ICONINFORMATION;
            break;
        }
        case CxbxMsgDlgIcon_Unknown:
        default: {
            uType |= MB_ICONQUESTION;
            break;
        }
    }

	va_start(argp, message);
	vsprintf(Buffer, message, argp);
	va_end(argp);

	EmuLogEx(cxbxr_module, level, "Popup : %s\n", Buffer);

	MessageBox(NULL, Buffer, TEXT("Cxbx-Reloaded"), uType);
}

void PrintCurrentConfigurationLog()
{
	// Print environment information
	{
		// Get Windows Version
		DWORD dwVersion = 0;
		DWORD dwMajorVersion = 0;
		DWORD dwMinorVersion = 0;
		DWORD dwBuild = 0;

		// TODO: GetVersion is deprecated but we use it anyway (for now)
		// The correct solution is to use GetProductInfo but that function
		// requires more logic to parse the response, and I didn't feel
		// like building it just yet :P
		dwVersion = GetVersion();

		dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
		dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

		// Get the build number.
		if (dwVersion < 0x80000000) {
			dwBuild = (DWORD)(HIWORD(dwVersion));
		}

		printf("------------------------ENVIRONMENT DETAILS-------------------------\n");
		if (g_bIsWine) {
			printf("Wine %s\n", wine_get_version());
			printf("Presenting as Windows %d.%d (%d)\n", dwMajorVersion, dwMinorVersion, dwBuild);
		} else {
			printf("Windows %d.%d (%d)\n", dwMajorVersion, dwMinorVersion, dwBuild);
		}
	}

	// HACK: For API TRace..
	// bLLE_GPU = true;

	// Print current LLE configuration
	{
		printf("---------------------------- LLE CONFIG ----------------------------\n");
		printf("LLE for APU is %s\n", bLLE_APU ? "enabled" : "disabled");
		printf("LLE for GPU is %s\n", bLLE_GPU ? "enabled" : "disabled");
		printf("LLE for USB is %s\n", bLLE_USB ? "enabled" : "disabled");
		printf("LLE for JIT is %s\n", bLLE_JIT ? "enabled" : "disabled");
	}

	// Print current video configuration (DirectX/HLE)
	if (!bLLE_GPU) {
		Settings::s_video XBVideoConf;
		g_EmuShared->GetVideoSettings(&XBVideoConf);

		printf("--------------------------- VIDEO CONFIG ---------------------------\n");
		printf("Direct3D Device: %s\n", XBVideoConf.direct3DDevice == 0 ? "Direct3D HAL (Hardware Accelerated)" : "Direct3D REF (Software)");
		printf("Video Resolution: %s\n", XBVideoConf.szVideoResolution);
		printf("Force VSync is %s\n", XBVideoConf.bVSync ? "enabled" : "disabled");
		printf("Fullscreen is %s\n", XBVideoConf.bFullScreen ? "enabled" : "disabled");
		printf("Hardware YUV is %s\n", XBVideoConf.bHardwareYUV ? "enabled" : "disabled");
	}

	// Print current audio configuration
	{
		Settings::s_audio XBAudioConf;
		g_EmuShared->GetAudioSettings(&XBAudioConf);

		printf("--------------------------- AUDIO CONFIG ---------------------------\n");
		printf("Audio Adapter: %s\n", XBAudioConf.adapterGUID.Data1 == 0 ? "Primary Audio Device" : "Secondary Audio Device");
		printf("PCM is %s\n", XBAudioConf.codec_pcm ? "enabled" : "disabled");
		printf("XADPCM is %s\n", XBAudioConf.codec_xadpcm ? "enabled" : "disabled");
		printf("Unknown Codec is %s\n", XBAudioConf.codec_unknown ? "enabled" : "disabled");
	}

	// Print Enabled Hacks
	{
		printf("--------------------------- HACKS CONFIG ---------------------------\n");
		printf("Disable Pixel Shaders: %s\n", g_DisablePixelShaders == 1 ? "On" : "Off (Default)");
		printf("Uncap Framerate: %s\n", g_UncapFramerate == 1 ? "On" : "Off (Default)");
		printf("Run Xbox threads on all cores: %s\n", g_UseAllCores == 1 ? "On" : "Off (Default)");
		printf("Skip RDTSC Patching: %s\n", g_SkipRdtscPatching == 1 ? "On" : "Off (Default)");
		printf("Scale Xbox to host viewport (and back): %s\n", g_ScaleViewport == 1 ? "On" : "Off (Default)");
		printf("Render directly to Host BackBuffer: %s\n", g_DirectHostBackBufferAccess == 1 ? "On" : "Off (Default)");
	}

	printf("------------------------- END OF CONFIG LOG ------------------------\n");
	
}

#if 0
BOOLEAN ApcInterrupt
(
	IN struct _KINTERRUPT *Interrupt,
	IN PVOID ServiceContext
)
{

}

BOOLEAN DispatchInterrupt
(
	IN struct _KINTERRUPT *Interrupt,
	IN PVOID ServiceContext
)
{
	ExecuteDpcQueue();
}

void InitSoftwareInterrupts()
{
	// Init software interrupt 1 (for APC dispatching)
	xboxkrnl::KINTERRUPT SoftwareInterrupt_1;
	SoftwareInterrupt_1.BusInterruptLevel = 1;
	SoftwareInterrupt_1.ServiceRoutine = ApcInterrupt;
	xboxkrnl::KeConnectInterrupt(&SoftwareInterrupt_1);

	// Init software interrupt 2 (for DPC dispatching)
	xboxkrnl::KINTERRUPT SoftwareInterrupt_2;
	SoftwareInterrupt_2.BusInterruptLevel = 2;
	SoftwareInterrupt_2.ServiceRoutine = DispatchInterrupt;
	xboxkrnl::KeConnectInterrupt(&SoftwareInterrupt_2);
}
#endif

void TriggerPendingConnectedInterrupts()
{
	for (int i = 0; i < MAX_BUS_INTERRUPT_LEVEL; i++) {
		// If the interrupt is pending and connected, process it
		if (HalSystemInterrupts[i].IsPending() && EmuInterruptList[i] && EmuInterruptList[i]->Connected) {
			HalSystemInterrupts[i].Trigger(EmuInterruptList[i]);
		}
	}
}

static unsigned int WINAPI CxbxKrnlInterruptThread(PVOID param)
{
	CxbxSetThreadName("CxbxKrnl Interrupts");

	// Make sure Xbox1 code runs on one core :
	InitXboxThread(g_CPUXbox);

#if 0
	InitSoftwareInterrupts();
#endif

	while (true) {
		if (g_bEnableAllInterrupts) {
			TriggerPendingConnectedInterrupts();
		}
		Sleep(1);
	}

	return 0;
}

static void CxbxKrnlClockThread(void* pVoid)
{
	LARGE_INTEGER CurrentTicks;
	uint64_t Delta;
	uint64_t Microseconds;
	unsigned int IncrementScaling;
	static uint64_t LastTicks = 0;
	static uint64_t Error = 0;
	static uint64_t UnaccountedMicroseconds = 0;

	// This keeps track of how many us have elapsed between two cycles, so that the xbox clocks are updated
	// with the proper increment (instead of blindly adding a single increment at every step)

	if (LastTicks == 0) {
		QueryPerformanceCounter(&CurrentTicks);
		LastTicks = CurrentTicks.QuadPart;
		CurrentTicks.QuadPart = 0;
	}

	QueryPerformanceCounter(&CurrentTicks);
	Delta = CurrentTicks.QuadPart - LastTicks;
	LastTicks = CurrentTicks.QuadPart;

	Error += (Delta * SCALE_S_IN_US);
	Microseconds = Error / HostClockFrequency;
	Error -= (Microseconds * HostClockFrequency);

	UnaccountedMicroseconds += Microseconds;
	IncrementScaling = (unsigned int)(UnaccountedMicroseconds / 1000); // -> 1 ms = 1000us -> time between two xbox clock interrupts
	UnaccountedMicroseconds -= (IncrementScaling * 1000);

	xboxkrnl::KiClockIsr(IncrementScaling);
}

std::vector<xbaddr> g_RdtscPatches;

#define OPCODE_PATCH_RDTSC 0x90EF  // OUT DX, EAX; NOP

bool IsRdtscInstruction(xbaddr addr)
{
	// First the fastest check - does addr contain exact patch from PatchRdtsc?
	// Second check - is addr on the rdtsc patch list?
	return (*(uint16_t*)addr == OPCODE_PATCH_RDTSC)
		// Note : It's not needed to check for g_SkipRdtscPatching,
		// as when that's set, the g_RdtscPatches vector will be empty
		// anyway, failing this lookup :
		&& (std::find(g_RdtscPatches.begin(), g_RdtscPatches.end(), addr) != g_RdtscPatches.end());
}

void PatchRdtsc(xbaddr addr)
{
	// Patch away rdtsc with an opcode we can intercept
	// We use a privilaged instruction rather than int 3 for debugging
	// When using int 3, attached debuggers trap and rdtsc is used often enough
	// that it makes Cxbx-Reloaded unusable
	// A privilaged instruction (like OUT) does not suffer from this
	printf("INIT: Patching rdtsc opcode at 0x%.8X\n", (DWORD)addr);
	*(uint16_t*)addr = OPCODE_PATCH_RDTSC;
	g_RdtscPatches.push_back(addr);
}

const uint8_t rdtsc_pattern[] = {
	0x89,//{ 0x0F,0x31,0x89 },
	0xC3,//{ 0x0F,0x31,0xC3 },
	0x8B,//{ 0x0F,0x31,0x8B },   //one false positive in Sonic Rider .text 88 5C 0F 31
	0xB9,//{ 0x0F,0x31,0xB9 },
	0xC7,//{ 0x0F,0x31,0xC7 },
	0x8D,//{ 0x0F,0x31,0x8D },
	0x68,//{ 0x0F,0x31,0x68 },
	0x5A,//{ 0x0F,0x31,0x5A },
	0x29,//{ 0x0F,0x31,0x29 },
	0xF3,//{ 0x0F,0x31,0xF3 },
	0xE9,//{ 0x0F,0x31,0xE9 },
	0x2B,//{ 0x0F,0x31,0x2B },
	0x50,//{ 0x0F,0x31,0x50 },	// 0x50 only used in ExaSkeleton .text , but encounter false positive in RalliSport .text 83 E2 0F 31
	0x0F,//{ 0x0F,0x31,0x0F },
	0x3B,//{ 0x0F,0x31,0x3B },
	0xD9,//{ 0x0F,0x31,0xD9 },
	0x57,//{ 0x0F,0x31,0x57 },
	0xB9,//{ 0x0F,0x31,0xB9 },
	0x85,//{ 0x0F,0x31,0x85 },
	0x83,//{ 0x0F,0x31,0x83 },
	0x33,//{ 0x0F,0x31,0x33 },
	0xF7,//{ 0x0F,0x31,0xF7 },
	0x8A,//{ 0x0F,0x31,0x8A }, // 8A and 56 only apears in RalliSport 2 .text , need to watch whether any future false positive.
	0x56,//{ 0x0F,0x31,0x56 }
    0x6A,                      // 6A, 39, EB, F6, A1, 01 only appear in Unreal Championship, 01 is at WMVDEC section
    0x39,
    0xEB,
    0xF6,
    0xA1,
    0x01
};
const int sizeof_rdtsc_pattern = sizeof(rdtsc_pattern);

void PatchRdtscInstructions()
{
	uint8_t rdtsc[2] = { 0x0F, 0x31 };
	DWORD sizeOfImage = CxbxKrnl_XbeHeader->dwSizeofImage;

	// Iterate through each CODE section
	for (uint32 sectionIndex = 0; sectionIndex < CxbxKrnl_Xbe->m_Header.dwSections; sectionIndex++) {
		if (!CxbxKrnl_Xbe->m_SectionHeader[sectionIndex].dwFlags.bExecutable) {
			continue;
		}

		// Skip some segments known to never contain rdtsc (to avoid false positives)
		if (std::string(CxbxKrnl_Xbe->m_szSectionName[sectionIndex]) == "DSOUND"
			|| std::string(CxbxKrnl_Xbe->m_szSectionName[sectionIndex]) == "XGRPH"
			|| std::string(CxbxKrnl_Xbe->m_szSectionName[sectionIndex]) == ".data"
			|| std::string(CxbxKrnl_Xbe->m_szSectionName[sectionIndex]) == ".rdata"
			|| std::string(CxbxKrnl_Xbe->m_szSectionName[sectionIndex]) == "XMV"
			|| std::string(CxbxKrnl_Xbe->m_szSectionName[sectionIndex]) == "XONLINE"
			|| std::string(CxbxKrnl_Xbe->m_szSectionName[sectionIndex]) == "MDLPL") {
			continue;
		}

		printf("INIT: Searching for rdtsc in section %s\n", CxbxKrnl_Xbe->m_szSectionName[sectionIndex]);
		xbaddr startAddr = CxbxKrnl_Xbe->m_SectionHeader[sectionIndex].dwVirtualAddr;
		//rdtsc is two bytes instruction, it needs at least one opcode byte after it to finish a function, so the endAddr need to substract 3 bytes.
		xbaddr endAddr = startAddr + CxbxKrnl_Xbe->m_SectionHeader[sectionIndex].dwSizeofRaw-3;
		for (xbaddr addr = startAddr; addr <= endAddr; addr++) 
		{
			if (memcmp((void*)addr, rdtsc, 2) == 0) 
			{
				uint8_t next_byte = *(uint8_t*)(addr + 2);
				// If the following byte matches the known pattern.
				int i = 0;
				for (i = 0; i<sizeof_rdtsc_pattern; i++) 
				{
					if (next_byte == rdtsc_pattern[i]) 
					{
						if (next_byte == 0x8B)
						{
							if (*(uint8_t*)(addr - 2) == 0x88 && *(uint8_t*)(addr - 1) == 0x5C)
							{
								printf("Skipped false positive: rdtsc pattern  0x%.2X, @ 0x%.8X\n", next_byte, (DWORD)addr);
								continue;
							}

						}
						if (next_byte == 0x50)
						{
							if (*(uint8_t*)(addr - 2) == 0x83 && *(uint8_t*)(addr - 1) == 0xE2)
							{
								printf("Skipped false positive: rdtsc pattern  0x%.2X, @ 0x%.8X\n", next_byte, (DWORD)addr);
								continue;
							}

						}
						PatchRdtsc(addr);
						//the first for loop already increment addr per loop. we only increment one more time so the addr will point to the byte next to the found rdtsc instruction. this is important since there is at least one case that two rdtsc instructions are next to each other.
						addr += 1;
						//if a match found, break the pattern matching loop and keep looping addr for next rdtsc.
						break;
					}
				}
				if (i>= sizeof_rdtsc_pattern)
				{
					//no pattern matched, keep record for detections we treat as non-rdtsc for future debugging.
					printf("Skipped potential rdtsc: Unknown opcode pattern  0x%.2X, @ 0x%.8X\n", next_byte, (DWORD)addr);
				}
			}
		}
	}

	printf("INIT: Done patching rdtsc, total %d rdtsc instructions patched\n", g_RdtscPatches.size());
}

void MapThunkTable(uint32_t* kt, uint32* pThunkTable)
{
    const bool SendDebugReports = (pThunkTable == CxbxKrnl_KernelThunkTable) && CxbxDebugger::CanReport();

	uint32_t* kt_tbl = (uint32_t*)kt;
	int i = 0;
	while (kt_tbl[i] != 0) {
		int t = kt_tbl[i] & 0x7FFFFFFF;
		kt_tbl[i] = pThunkTable[t];
        if (SendDebugReports) {
            // TODO: Update CxbxKrnl_KernelThunkTable to include symbol names
            std::string importName = "KernelImport_" + std::to_string(t);
            CxbxDebugger::ReportKernelPatch(importName.c_str(), kt_tbl[i]);
        }
		i++;
	}
}

typedef struct {
	xbaddr ThunkAddr;
	xbaddr LibNameAddr;
} XbeImportEntry;

void ImportLibraries(XbeImportEntry *pImportDirectory)
{
	// assert(pImportDirectory);

	while (pImportDirectory->LibNameAddr && pImportDirectory->ThunkAddr) {
		std::wstring LibName = std::wstring((wchar_t*)pImportDirectory->LibNameAddr);

		if (LibName == L"xbdm.dll") {
			MapThunkTable((uint32_t *)pImportDirectory->ThunkAddr, Cxbx_LibXbdmThunkTable);
		}
		else {
			printf("LOAD : Skipping unrecognized import library : %s\n", LibName.c_str());
		}

		pImportDirectory++;
	}
}

void CxbxKrnlMain(int argc, char* argv[])
{
	// Skip '/load' switch
	// Get XBE Name :
	std::string xbePath = argv[2];

	// Get DCHandle :
	HWND hWnd = 0;
	if (argc > 3) {
		hWnd = (HWND)std::atoi(argv[3]);
	}

	// Get KernelDebugMode :
	DebugMode DbgMode = DebugMode::DM_NONE;
	if (argc > 4) {
		DbgMode = (DebugMode)std::atoi(argv[4]);
	}

	// Get KernelDebugFileName :
	std::string DebugFileName = "";
	if (argc > 5) {
		DebugFileName = argv[5];
	}

	int BootFlags;
	g_EmuShared->GetBootFlags(&BootFlags);

	// debug console allocation (if configured)
	if (DbgMode == DM_CONSOLE)
	{
		if (AllocConsole())
		{
			HANDLE StdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
			// Maximise the console scroll buffer height :
			CONSOLE_SCREEN_BUFFER_INFO coninfo;
			GetConsoleScreenBufferInfo(StdHandle, &coninfo);
			coninfo.dwSize.Y = SHRT_MAX - 1; // = 32767-1 = 32766 = maximum value that works
			SetConsoleScreenBufferSize(StdHandle, coninfo.dwSize);
			freopen("CONOUT$", "wt", stdout);
			freopen("CONIN$", "rt", stdin);
			SetConsoleTitle("Cxbx-Reloaded : Kernel Debug Console");
			SetConsoleTextAttribute(StdHandle, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
		}
	}
	else
	{
		FreeConsole();
		if (DbgMode == DM_FILE) {
			// Peform clean write to kernel log for first boot. Unless multi-xbe boot occur then perform append to existing log.
			freopen(DebugFileName.c_str(), ((BootFlags == DebugMode::DM_NONE) ? "wt" : "at"), stdout);
			// Append separator for better readability after reboot.
			if (BootFlags != DebugMode::DM_NONE) {
				std::cout << "\n------REBOOT------REBOOT------REBOOT------REBOOT------REBOOT------\n" << std::endl;
			}
		}
		else {
			char buffer[16];
			if (GetConsoleTitle(buffer, 16) != NULL)
				freopen("nul", "w", stdout);
		}
	}

	// We must save this handle now to keep the child window working in the case we need to display the UEM
	CxbxKrnl_hEmuParent = IsWindow(hWnd) ? hWnd : NULL;

	g_CurrentProcessHandle = GetCurrentProcess(); // OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());

	// Set up the logging variables for the kernel process during initialization.
	log_sync_config();

	if (CxbxKrnl_hEmuParent != NULL) {
		ipc_send_gui_update(IPC_UPDATE_GUI::KRNL_IS_READY, static_cast<UINT>(GetCurrentProcessId()));

		// Force wait until GUI process is ready
		do {
			int waitCounter = 10;
			bool isReady = false;

			while (waitCounter > 0) {
				g_EmuShared->GetIsReady(&isReady);
				if (isReady) {
					break;
				}
				waitCounter--;
				Sleep(100);
			}
			if (!isReady) {
				EmuLog(LOG_LEVEL::WARNING, "GUI process is not ready!");
				int mbRet = MessageBox(NULL, "GUI process is not ready, do you wish to retry?", TEXT("Cxbx-Reloaded"),
										MB_ICONWARNING | MB_RETRYCANCEL | MB_TOPMOST | MB_SETFOREGROUND);
				if (mbRet == IDRETRY) {
					continue;
				}
				CxbxKrnlShutDown();
			}
			break;
		} while (true);
	}

	g_EmuShared->SetIsReady(false);

	UINT prevKrnlProcID = 0;
	DWORD dwExitCode = EXIT_SUCCESS;
	g_EmuShared->GetKrnlProcID(&prevKrnlProcID);

	// Force wait until previous kernel process is closed.
	if (prevKrnlProcID != 0) {
		HANDLE hProcess = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION, FALSE, prevKrnlProcID);
		// If we do receive valid handle, let's do the next step.
		if (hProcess != NULL) {

			WaitForSingleObject(hProcess, INFINITE);

			GetExitCodeProcess(hProcess, &dwExitCode);
			CloseHandle(hProcess);
		}
	}

	if (dwExitCode != EXIT_SUCCESS) {// StopEmulation
		CxbxKrnlShutDown();
	}

	bool isLogEnabled;
	g_EmuShared->GetIsKrnlLogEnabled(&isLogEnabled);
	g_bPrintfOn = isLogEnabled;

	g_EmuShared->ResetKrnl();

	// Save current kernel proccess id for next reboot if will occur in the future.
	g_EmuShared->SetKrnlProcID(GetCurrentProcessId());

	// Write a header to the log
	{
		printf("[0x%.4X] INIT: Cxbx-Reloaded Version %s\n", GetCurrentThreadId(), _CXBX_VERSION);

		time_t startTime = time(nullptr);
		struct tm* tm_info = localtime(&startTime);
		char timeString[26];
		strftime(timeString, 26, "%F %T", tm_info);
		printf("[0x%.4X] INIT: Log started at %s\n", GetCurrentThreadId(), timeString);

#ifdef _DEBUG_TRACE
		printf("[0x%.4X] INIT: Debug Trace Enabled.\n", GetCurrentThreadId());
#else
		printf("[0x%.4X] INIT: Debug Trace Disabled.\n", GetCurrentThreadId());
#endif
	}

	// Log once, since multi-xbe boot is appending to file instead of overwrite.
	if (BootFlags == BOOT_NONE) {
		log_generate_active_filter_output(CXBXR_MODULE::INIT);
	}

	// Detect Wine
	g_bIsWine = false;
	HMODULE hNtDll = GetModuleHandle("ntdll.dll");

	if (hNtDll != nullptr) {
		wine_get_version = (LPFN_WINEGETVERSION)GetProcAddress(hNtDll, "wine_get_version");
		if (wine_get_version) {
			g_bIsWine = true;
		}
	}

	// Now we got the arguments, start by initializing the Xbox memory map :
	// PrepareXBoxMemoryMap()
	{
		// Our executable DOS image header must be loaded at 0x00010000
		// Assert(ExeDosHeader == XBE_IMAGE_BASE);

		// Determine EXE's header locations & size :
		ExeNtHeader = (PIMAGE_NT_HEADERS)((ULONG_PTR)ExeDosHeader + ExeDosHeader->e_lfanew); // = + 0x138
		ExeOptionalHeader = (PIMAGE_OPTIONAL_HEADER)&(ExeNtHeader->OptionalHeader);

		// verify base of code of our executable is 0x00001000
		if (ExeNtHeader->OptionalHeader.BaseOfCode != CXBX_BASE_OF_CODE)
		{
			CxbxPopupMessage(LOG_LEVEL::FATAL, CxbxMsgDlgIcon_Error, "Cxbx-Reloaded executuable requires it's base of code to be 0x00001000");
			return; // TODO : Halt(0); 
		}

#ifndef CXBX_LOADER
		// verify virtual_memory_placeholder is located at 0x00011000
		if ((UINT_PTR)(&(virtual_memory_placeholder[0])) != (XBE_IMAGE_BASE + CXBX_BASE_OF_CODE))
		{
			CxbxPopupMessage(LOG_LEVEL::FATAL, CxbxMsgDlgIcon_Error, "virtual_memory_placeholder is not loaded to base address 0x00011000 (which is a requirement for Xbox emulation)");
			return; // TODO : Halt(0); 
		}
#endif

#ifdef CXBX_LOADER
		if (!VerifyAddressRanges(SYSTEM_XBOX)) {
			CxbxPopupMessage("Cxbx-Reloaded hasn't got access to all required address ranges");
			return; // TODO : Halt(0); 
		}
#endif

		// Create a safe copy of the complete EXE header:
		DWORD ExeHeaderSize = ExeOptionalHeader->SizeOfHeaders; // Should end up as 0x400
		NewDosHeader = (PIMAGE_DOS_HEADER)VirtualAlloc(nullptr, ExeHeaderSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		memcpy(NewDosHeader, ExeDosHeader, ExeHeaderSize);

		// Determine NewOptionalHeader, required by RestoreExeImageHeader
		NewNtHeader = (PIMAGE_NT_HEADERS)((ULONG_PTR)NewDosHeader + ExeDosHeader->e_lfanew);
		NewOptionalHeader = (PIMAGE_OPTIONAL_HEADER)&(NewNtHeader->OptionalHeader);

		// Make sure the new DOS header points to the new relative NtHeader location:
		NewDosHeader->e_lfanew = (ULONG_PTR)NewNtHeader - XBE_IMAGE_BASE;

		// Note : NewOptionalHeader->ImageBase can stay at ExeOptionalHeader->ImageBase = 0x00010000

		// Note : Since virtual_memory_placeholder prevents overlap between reserved xbox memory
		// and Cxbx.exe sections, section headers don't have to be patched up.

		// Mark the virtual memory range completely accessible
		DWORD OldProtection;
		if (0 == VirtualProtect((void*)XBE_IMAGE_BASE, XBE_MAX_VA - XBE_IMAGE_BASE, PAGE_EXECUTE_READWRITE, &OldProtection)) {
			DWORD err = GetLastError();

			// Translate ErrorCode to String.
			LPTSTR Error = 0;
			if (::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				err,
				0,
				(LPTSTR)&Error,
				0,
				NULL) == 0) {
				// Failed in translating.
			}

			// Free the buffer.
			if (Error) {
				::LocalFree(Error);
				Error = 0;
			}
		}

		// Clear out the virtual memory range
		memset((void*)XBE_IMAGE_BASE, 0, XBE_MAX_VA - XBE_IMAGE_BASE);

		// Restore enough of the executable image headers to keep WinAPI's working :
		RestoreExeImageHeader();
	}

	HANDLE hMemoryBin = CxbxRestoreContiguousMemory(szFilePath_memory_bin);
	HANDLE hPageTables = CxbxRestorePageTablesMemory(szFilePath_page_tables);

	// Load Per-Xbe Keys from the Cxbx-Reloaded AppData directory
	LoadXboxKeys(szFolder_CxbxReloadedData);

	EEPROM = CxbxRestoreEEPROM(szFilePath_EEPROM_bin);
	if (EEPROM == nullptr)
	{
		CxbxPopupMessage(LOG_LEVEL::FATAL, CxbxMsgDlgIcon_Error, "Couldn't init EEPROM!");
		return; // TODO : Halt(0); 
	}

	// TODO : Instead of loading an Xbe here, initialize the kernel so that it will launch the Xbe on itself.
	// using XeLoadImage from LaunchDataPage->Header.szLaunchPath

	// Now we can load and run the XBE :
	// MapAndRunXBE(XbePath, DCHandle);
	{
		// NOTE: This is a safety to clean the file path for any malicious file path attempt.
		// Might want to move this into a utility function.
		size_t n, i;
		// Remove useless slashes before and after semicolon.
		std::string semicolon_search[] = { "\\;", ";\\", "/;", ";/" };
		std::string semicolon_str = ";";
		for (n = 0, i = 0; i < semicolon_search->size(); i++, n = 0) {
			while ((n = xbePath.find(semicolon_search[i], n)) != std::string::npos) {
				xbePath.replace(n, semicolon_search[i].size(), semicolon_str);
				n += semicolon_str.size();
			}
		}
		// Remove extra slashes.
		std::string slash_search[] = { "\\\\", "//" };
		std::string slash_str = "/";
		for (n = 0, i = 0; i < slash_search->size(); i++, n = 0) {
			while ((n = xbePath.find(slash_search[i], n)) != std::string::npos) {
				xbePath.replace(n, slash_search[i].size(), slash_str);
				n += slash_str.size();
			}
		}

		// Once clean up process is done, proceed set to global variable string.
		strncpy(szFilePath_Xbe, xbePath.c_str(), MAX_PATH - 1);
		std::replace(xbePath.begin(), xbePath.end(), ';', '/');
		// Load Xbe (this one will reside above WinMain's virtual_memory_placeholder)
		CxbxKrnl_Xbe = new Xbe(xbePath.c_str(), false); // TODO : Instead of using the Xbe class, port Dxbx _ReadXbeBlock()

		if (CxbxKrnl_Xbe->HasFatalError()) {
			CxbxKrnlCleanup(CxbxKrnl_Xbe->GetError().c_str());
			return;
		}

		// Check the signature of the xbe
		if (CxbxKrnl_Xbe->CheckXbeSignature()) {
			printf("[0x%X] INIT: Valid xbe signature. Xbe is legit\n", GetCurrentThreadId());
		}
		else {
			printf("[0x%X] INIT: Invalid xbe signature. Homebrew, tampered or pirated xbe?\n", GetCurrentThreadId());
		}

		// Check the integrity of the xbe sections
		for (uint32 sectionIndex = 0; sectionIndex < CxbxKrnl_Xbe->m_Header.dwSections; sectionIndex++) {
			uint32 RawSize = CxbxKrnl_Xbe->m_SectionHeader[sectionIndex].dwSizeofRaw;
			if (RawSize == 0) {
				continue;
			}
			unsigned char SHADigest[A_SHA_DIGEST_LEN];
			CalcSHA1Hash(SHADigest, CxbxKrnl_Xbe->m_bzSection[sectionIndex], RawSize);

			if (memcmp(SHADigest, (CxbxKrnl_Xbe->m_SectionHeader)[sectionIndex].bzSectionDigest, A_SHA_DIGEST_LEN) != 0) {
				printf("[0x%X] INIT: SHA hash of section %s doesn't match, possible section corruption\n", GetCurrentThreadId(), CxbxKrnl_Xbe->m_szSectionName[sectionIndex]);
			}
			else {
				printf("[0x%X] INIT: SHA hash check of section %s successful\n", GetCurrentThreadId(), CxbxKrnl_Xbe->m_szSectionName[sectionIndex]);
			}
		}

		// Detect XBE type :
		g_XbeType = GetXbeType(&CxbxKrnl_Xbe->m_Header);

		// Register if we're running an Chihiro executable or a debug xbe, otherwise it's an Xbox retail executable
		g_bIsChihiro = (g_XbeType == xtChihiro);
		g_bIsDebug = (g_XbeType == xtDebug);
		g_bIsRetail = (g_XbeType == xtRetail);

		// Disabled: The media board rom fails to run because it REQUIRES LLE USB, which is not yet enabled.
		// Chihiro games can be ran directly for now. 
		// This just means that you cannot access the Chihiro test menus and related stuff, games should still be okay
#if 0   
		// If the Xbe is Chihiro, and we were not launched by SEGABOOT, we need to load SEGABOOT from the Chihiro Media Board rom instead!
		// TODO: We also need to store the path of the loaded game, and mount it as the mediaboard filesystem
		// TODO: How to we detect who launched us, to prevent a reboot-loop
		if (g_bIsChihiro) {
			std::string chihiroMediaBoardRom = std::string(szFolder_CxbxReloadedData) + std::string("/EmuDisk/") + MediaBoardRomFile;
			if (!std::experimental::filesystem::exists(chihiroMediaBoardRom)) {
				CxbxKrnlCleanup("Chihiro Media Board ROM (fpr21042_m29w160et.bin) could not be found");
			}

			delete CxbxKrnl_Xbe;
			CxbxKrnl_Xbe = new Xbe(chihiroMediaBoardRom.c_str(), false);
		}
#endif
		// Initialize the virtual manager
		g_VMManager.Initialize(hMemoryBin, hPageTables, BootFlags);

		// Commit the memory used by the xbe header
		size_t HeaderSize = CxbxKrnl_Xbe->m_Header.dwSizeofHeaders;
		VAddr XbeBase = XBE_IMAGE_BASE;
		g_VMManager.XbAllocateVirtualMemory(&XbeBase, 0, &HeaderSize, XBOX_MEM_COMMIT, XBOX_PAGE_READWRITE);


		// Copy over loaded Xbe Headers to specified base address
		memcpy((void*)CxbxKrnl_Xbe->m_Header.dwBaseAddr, &CxbxKrnl_Xbe->m_Header, sizeof(Xbe::Header));
		memcpy((void*)(CxbxKrnl_Xbe->m_Header.dwBaseAddr + sizeof(Xbe::Header)), CxbxKrnl_Xbe->m_HeaderEx, CxbxKrnl_Xbe->m_ExSize);

		// Load all sections marked as preload using the in-memory copy of the xbe header
		xboxkrnl::PXBEIMAGE_SECTION sectionHeaders = (xboxkrnl::PXBEIMAGE_SECTION)CxbxKrnl_Xbe->m_Header.dwSectionHeadersAddr;
		for (uint32 i = 0; i < CxbxKrnl_Xbe->m_Header.dwSections; i++) {
			if ((sectionHeaders[i].Flags & XBEIMAGE_SECTION_PRELOAD) != 0) {
				NTSTATUS result = xboxkrnl::XeLoadSection(&sectionHeaders[i]);
				if (FAILED(result)) {
					CxbxKrnlCleanupEx(LOG_PREFIX_INIT, "Failed to preload XBE section: %s", CxbxKrnl_Xbe->m_szSectionName[i]);
				}
			}
		}

		// We need to remember a few XbeHeader fields, so we can switch between a valid ExeHeader and XbeHeader :
		StoreXbeImageHeader();

		// Restore enough of the executable image headers to keep WinAPI's working :
		RestoreExeImageHeader();
	}

	// Decode kernel thunk table address :
	uint32_t kt = CxbxKrnl_Xbe->m_Header.dwKernelImageThunkAddr;
	kt ^= XOR_KT_KEY[g_XbeType];

	// Process the Kernel thunk table to map Kernel function calls to their actual address :
	MapThunkTable((uint32_t*)kt, CxbxKrnl_KernelThunkTable);

	// Does this xbe import any other libraries?
	if (CxbxKrnl_Xbe->m_Header.dwNonKernelImportDirAddr) {
		ImportLibraries((XbeImportEntry*)CxbxKrnl_Xbe->m_Header.dwNonKernelImportDirAddr);
	}

	// Launch the XBE :
	{
		// Load TLS
		Xbe::TLS* XbeTls = (Xbe::TLS*)CxbxKrnl_Xbe->m_Header.dwTLSAddr;
		void* XbeTlsData = (XbeTls != nullptr) ? (void*)CxbxKrnl_Xbe->m_TLS->dwDataStartAddr : nullptr;
		// Decode Entry Point
		xbaddr EntryPoint = CxbxKrnl_Xbe->m_Header.dwEntryAddr;
		EntryPoint ^= XOR_EP_KEY[g_XbeType];
		// Launch XBE
		CxbxKrnlInit(
			XbeTlsData, 
			XbeTls, 
			CxbxKrnl_Xbe->m_LibraryVersion, 
			DbgMode,
			DebugFileName.c_str(),
			(Xbe::Header*)CxbxKrnl_Xbe->m_Header.dwBaseAddr,
			CxbxKrnl_Xbe->m_Header.dwSizeofHeaders,
			(void(*)())EntryPoint,
 			BootFlags
		);
	}
}
#pragma optimize("", on)

// Loads a keys.bin file as generated by dump-xbox
// See https://github.com/JayFoxRox/xqemu-tools/blob/master/dump-xbox.c
void LoadXboxKeys(std::string path)
{
	std::string keys_path = path + "\\keys.bin";

	// Attempt to open Keys.bin
	FILE* fp = fopen(keys_path.c_str(), "rb");

	if (fp != nullptr) {
		// Determine size of Keys.bin
		xboxkrnl::XBOX_KEY_DATA keys[2];
		fseek(fp, 0, SEEK_END);
		long size = ftell(fp);
		rewind(fp);

		// If the size of Keys.bin is correct (two keys), read it
		if (size == xboxkrnl::XBOX_KEY_LENGTH * 2) {
			fread(keys, xboxkrnl::XBOX_KEY_LENGTH, 2, fp);

			memcpy(xboxkrnl::XboxEEPROMKey, &keys[0], xboxkrnl::XBOX_KEY_LENGTH);
			memcpy(xboxkrnl::XboxCertificateKey, &keys[1], xboxkrnl::XBOX_KEY_LENGTH);
		}
		else {
			EmuLog(LOG_LEVEL::WARNING, "Keys.bin has an incorrect filesize. Should be %d bytes", xboxkrnl::XBOX_KEY_LENGTH * 2);
		}

		fclose(fp);
		return;
	}

	// If we didn't already exit the function, keys.bin could not be loaded
	EmuLog(LOG_LEVEL::WARNING, "Failed to load Keys.bin. Cxbx-Reloaded will be unable to read Save Data from a real Xbox");
}

__declspec(noreturn) void CxbxKrnlInit
(
	void                   *pTLSData,
	Xbe::TLS               *pTLS,
	Xbe::LibraryVersion    *pLibraryVersion,
	DebugMode               DbgMode,
	const char             *szDebugFilename,
	Xbe::Header            *pXbeHeader,
	uint32                  dwXbeHeaderSize,
	void(*Entry)(),
	int BootFlags)
{
	// update caches
	CxbxKrnl_TLS = pTLS;
	CxbxKrnl_TLSData = pTLSData;
	CxbxKrnl_XbeHeader = pXbeHeader;
	CxbxKrnl_DebugMode = DbgMode;
	CxbxKrnl_DebugFileName = (char*)szDebugFilename;

	// A patch to dwCertificateAddr is a requirement due to Windows TLS is overwriting dwGameRegion data address.
	// By using unalternated certificate data, it should no longer cause any problem with titles running and Cxbx's log as well.
	CxbxKrnl_XbeHeader->dwCertificateAddr = (uint32)&CxbxKrnl_Xbe->m_Certificate;
	g_pCertificate = &CxbxKrnl_Xbe->m_Certificate;

	// Initialize timer subsystem
	Timer_Init();
	// for unicode conversions
	setlocale(LC_ALL, "English");
	// Initialize time-related variables for the kernel and the timers
	CxbxInitPerformanceCounters();
#ifdef _DEBUG
//	CxbxPopupMessage(LOG_LEVEL::INFO, "Attach a Debugger");
//  Debug child processes using https://marketplace.visualstudio.com/items?itemName=GreggMiskelly.MicrosoftChildProcessDebuggingPowerTool
#endif

	// debug trace
	{
#ifdef _DEBUG_TRACE
		printf("[0x%X] INIT: Debug Trace Enabled.\n", GetCurrentThreadId());
		printf("[0x%X] INIT: CxbxKrnlInit\n"
			"(\n"
			"   hwndParent          : 0x%.08X\n"
			"   pTLSData            : 0x%.08X\n"
			"   pTLS                : 0x%.08X\n"
			"   pLibraryVersion     : 0x%.08X\n"
			"   DebugConsole        : 0x%.08X\n"
			"   DebugFilename       : \"%s\"\n"
			"   pXBEHeader          : 0x%.08X\n"
			"   pXBEHeaderSize      : 0x%.08X\n"
			"   Entry               : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), CxbxKrnl_hEmuParent, pTLSData, pTLS, pLibraryVersion, DbgMode, szDebugFilename, pXbeHeader, dwXbeHeaderSize, Entry);
#else
		printf("[0x%X] INIT: Debug Trace Disabled.\n", GetCurrentThreadId());
#endif
	}

#ifdef _DEBUG_TRACE
	// VerifyHLEDataBase();
#endif
	// TODO : The following seems to cause a crash when booting the game "Forza Motorsport",
	// according to https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/101#issuecomment-277230140
	{
		// Create a fake kernel header for XapiRestrictCodeSelectorLimit
		// Thanks advancingdragon / DirtBox
		PDUMMY_KERNEL DummyKernel = (PDUMMY_KERNEL)XBOX_KERNEL_BASE;
		memset(DummyKernel, 0, sizeof(DUMMY_KERNEL));

		// XapiRestrictCodeSelectorLimit only checks these fields.
		DummyKernel->DosHeader.e_lfanew = sizeof(IMAGE_DOS_HEADER); // RVA of NtHeaders
		DummyKernel->FileHeader.SizeOfOptionalHeader = 0;
		DummyKernel->FileHeader.NumberOfSections = 1;
		// as long as this doesn't start with "INIT"
		strncpy_s((PSTR)DummyKernel->SectionHeader.Name, 8, "DONGS", 8);
		printf("[0x%.4X] INIT: Initialized dummy kernel image header.\n", GetCurrentThreadId());
	}

	// Read which components need to be LLE'ed per user request
	{
		uint CxbxLLE_Flags;
		g_EmuShared->GetFlagsLLE(&CxbxLLE_Flags);
		bLLE_APU = (CxbxLLE_Flags & LLE_APU) > 0;
		bLLE_GPU = (CxbxLLE_Flags & LLE_GPU) > 0;
		//bLLE_USB = (CxbxLLE_Flags & LLE_USB) > 0; // Reenable this when LLE USB actually works
		bLLE_JIT = (CxbxLLE_Flags & LLE_JIT) > 0;
	}

	// Process Hacks
	{
		int HackEnabled = 0;
		g_EmuShared->GetDisablePixelShaders(&HackEnabled);
		g_DisablePixelShaders = !!HackEnabled;
		g_EmuShared->GetUncapFramerate(&HackEnabled);
		g_UncapFramerate = !!HackEnabled;
		g_EmuShared->GetUseAllCores(&HackEnabled);
		g_UseAllCores = !!HackEnabled;
		g_EmuShared->GetSkipRdtscPatching(&HackEnabled);
		g_SkipRdtscPatching = !!HackEnabled;
		g_EmuShared->GetScaleViewport(&HackEnabled);
		g_ScaleViewport = !!HackEnabled;
		g_EmuShared->GetDirectHostBackBufferAccess(&HackEnabled);
		g_DirectHostBackBufferAccess = !!HackEnabled;
	}

#ifdef _DEBUG_PRINT_CURRENT_CONF
	PrintCurrentConfigurationLog();
#endif
	
	// Initialize devices :
	char szBuffer[sizeof(szFilePath_Xbe)];
	g_EmuShared->GetStorageLocation(szBuffer);

	CxbxBasePath = std::string(szBuffer) + "\\EmuDisk\\";

	// Determine XBE Path
	strncpy(szBuffer, szFilePath_Xbe, sizeof(szBuffer)-1);
	szBuffer[sizeof(szBuffer) - 1] = '\0'; // Safely null terminate at the end.

	std::string xbePath(szBuffer);
	std::replace(xbePath.begin(), xbePath.end(), ';', '/');
	std::string xbeDirectory(szBuffer);
	size_t lastFind = xbeDirectory.find(';');
	// First find if there is a semicolon when dashboard or title disc (such as demo disc) has it.
	// Then we must obey the current directory it asked for.
	if (lastFind != std::string::npos) {
		if (xbeDirectory.find(';', lastFind + 1) != std::string::npos) {
			CxbxKrnlCleanupEx(LOG_PREFIX_INIT, "Cannot contain multiple of ; symbol.");
		}
		xbeDirectory = xbeDirectory.substr(0, lastFind);
	}
	else {
		xbeDirectory = xbeDirectory.substr(0, xbeDirectory.find_last_of("\\/"));
	}
	CxbxBasePathHandle = CreateFile(CxbxBasePath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	memset(szBuffer, 0, sizeof(szBuffer));
	// Games may assume they are running from CdRom :
	CxbxDefaultXbeDriveIndex = CxbxRegisterDeviceHostPath(DeviceCdrom0, xbeDirectory);
	// Partition 0 contains configuration data, and is accessed as a native file, instead as a folder :
	CxbxRegisterDeviceHostPath(DeviceHarddisk0Partition0, CxbxBasePath + "Partition0", /*IsFile=*/true);
	// The first two partitions are for Data and Shell files, respectively :
	CxbxRegisterDeviceHostPath(DeviceHarddisk0Partition1, CxbxBasePath + "Partition1");
	CxbxRegisterDeviceHostPath(DeviceHarddisk0Partition2, CxbxBasePath + "Partition2");
	// The following partitions are for caching purposes - for now we allocate up to 7 (as xbmp needs that many) :
	CxbxRegisterDeviceHostPath(DeviceHarddisk0Partition3, CxbxBasePath + "Partition3");
	CxbxRegisterDeviceHostPath(DeviceHarddisk0Partition4, CxbxBasePath + "Partition4");
	CxbxRegisterDeviceHostPath(DeviceHarddisk0Partition5, CxbxBasePath + "Partition5");
	CxbxRegisterDeviceHostPath(DeviceHarddisk0Partition6, CxbxBasePath + "Partition6");
	CxbxRegisterDeviceHostPath(DeviceHarddisk0Partition7, CxbxBasePath + "Partition7");

	// Create default symbolic links :
	DBG_PRINTF_EX(LOG_PREFIX_INIT, "Creating default symbolic links.\n");
	{
		// TODO: DriveD should always point to the Xbe Path
		// This is the only symbolic link the Xbox Kernel sets, the rest are set by the application, usually via XAPI.
		// If the Xbe is located outside of the emulated HDD, mounting it as DeviceCdrom0 is correct
		// If the Xbe is located inside the emulated HDD, the full path should be used, eg: "\\Harddisk0\\partition2\\xboxdash.xbe"
		CxbxCreateSymbolicLink(DriveD, DeviceCdrom0);
		// Arrange that the Xbe path can reside outside the partitions, and put it to g_hCurDir :
		EmuNtSymbolicLinkObject* xbePathSymbolicLinkObject = FindNtSymbolicLinkObjectByDriveLetter(CxbxDefaultXbeDriveLetter);
		g_hCurDir = xbePathSymbolicLinkObject->RootDirectoryHandle;
	}

	// Determine Xbox path to XBE and place it in XeImageFileName
	{
		std::string fileName(xbePath);
		// Strip out the path, leaving only the XBE file name
		// NOTE: we assume that the XBE is always on the root of the D: drive
		// This is a safe assumption as the Xbox kernel ALWAYS mounts D: as the Xbe Path
		if (fileName.rfind('\\') != std::string::npos)
			fileName = fileName.substr(fileName.rfind('\\') + 1);

		if (xboxkrnl::XeImageFileName.Buffer != NULL)
			free(xboxkrnl::XeImageFileName.Buffer);

		// Assign the running Xbe path, so it can be accessed via the kernel thunk 'XeImageFileName' :
		xboxkrnl::XeImageFileName.MaximumLength = MAX_PATH;
		xboxkrnl::XeImageFileName.Buffer = (PCHAR)g_VMManager.Allocate(MAX_PATH);
		sprintf(xboxkrnl::XeImageFileName.Buffer, "%c:\\%s", CxbxDefaultXbeDriveLetter, fileName.c_str());
		xboxkrnl::XeImageFileName.Length = (USHORT)strlen(xboxkrnl::XeImageFileName.Buffer);
		printf("[0x%.4X] INIT: XeImageFileName = %s\n", GetCurrentThreadId(), xboxkrnl::XeImageFileName.Buffer);
	}

	// Dump Xbe information
	{
		if (CxbxKrnl_Xbe != nullptr) {
			printf("[0x%.4X] INIT: Title : %s\n", GetCurrentThreadId(), CxbxKrnl_Xbe->m_szAsciiTitle);
		}

		// Dump Xbe certificate
		if (g_pCertificate != NULL) {
			std::stringstream titleIdHex;
			titleIdHex << std::hex << g_pCertificate->dwTitleId;

			printf("[0x%.4X] INIT: XBE TitleID : %s\n", GetCurrentThreadId(), FormatTitleId(g_pCertificate->dwTitleId).c_str());
			printf("[0x%.4X] INIT: XBE TitleID (Hex) : 0x%s\n", GetCurrentThreadId(), titleIdHex.str().c_str());
			printf("[0x%.4X] INIT: XBE Version : 1.%02d\n", GetCurrentThreadId(), g_pCertificate->dwVersion);
			printf("[0x%.4X] INIT: XBE TitleName : %ls\n", GetCurrentThreadId(), g_pCertificate->wszTitleName);
			printf("[0x%.4X] INIT: XBE Region : %s\n", GetCurrentThreadId(), CxbxKrnl_Xbe->GameRegionToString());
		}

		// Dump Xbe library build numbers
		Xbe::LibraryVersion* libVersionInfo = pLibraryVersion;// (LibraryVersion *)(CxbxKrnl_XbeHeader->dwLibraryVersionsAddr);
		if (libVersionInfo != NULL) {
			for (uint32 v = 0; v < CxbxKrnl_XbeHeader->dwLibraryVersions; v++) {
				printf("[0x%.4X] INIT: XBE Library %u : %.8s (version %d)\n", GetCurrentThreadId(), v, libVersionInfo->szName, libVersionInfo->wBuildVersion);
				libVersionInfo++;
			}
		}
	}

	CxbxKrnlRegisterThread(GetCurrentThread());

	// Make sure the Xbox1 code runs on one core (as the box itself has only 1 CPU,
	// this will better aproximate the environment with regard to multi-threading) :
	DBG_PRINTF_EX(LOG_PREFIX_INIT, "Determining CPU affinity.\n");
	{
		if (!GetProcessAffinityMask(g_CurrentProcessHandle, &g_CPUXbox, &g_CPUOthers))
			CxbxKrnlCleanupEx(LOG_PREFIX_INIT, "GetProcessAffinityMask failed.");

		// For the other threads, remove one bit from the processor mask:
		g_CPUOthers = ((g_CPUXbox - 1) & g_CPUXbox);

		// Test if there are any other cores available :
		if (g_CPUOthers > 0) {
			// If so, make sure the Xbox threads run on the core NOT running Xbox code :
			g_CPUXbox = g_CPUXbox & (~g_CPUOthers);
		} else {
			// Else the other threads must run on the same core as the Xbox code :
			g_CPUOthers = g_CPUXbox;
		}
	}

	// initialize graphics
	DBG_PRINTF_EX(LOG_PREFIX_INIT, "Initializing render window.\n");
	XTL::CxbxInitWindow(true);

	// Now process the boot flags to see if there are any special conditions to handle
	if (BootFlags & BOOT_EJECT_PENDING) {} // TODO
	if (BootFlags & BOOT_FATAL_ERROR)
	{
		// If we are here it means we have been rebooted to display the fatal error screen. The error code is set
		// to 0x15 and the led flashes with the sequence green, red, red, red

		SetLEDSequence(0xE1);
		CxbxKrnlPrintUEM(FATAL_ERROR_REBOOT_ROUTINE); // won't return
	}
	if (BootFlags & BOOT_SKIP_ANIMATION) {} // TODO
	if (BootFlags & BOOT_RUN_DASHBOARD) {} // TODO

    XTL::CxbxInitAudio();

	EmuHLEIntercept(pXbeHeader);

	if (!bLLE_USB) {
		SetupXboxDeviceTypes();
	}

	InitXboxHardware(HardwareModel::Revision1_5); // TODO : Make configurable

	// Read Xbox video mode from the SMC, store it in HalBootSMCVideoMode
	xboxkrnl::HalReadSMBusValue(SMBUS_ADDRESS_SYSTEM_MICRO_CONTROLLER, SMC_COMMAND_AV_PACK, FALSE, &xboxkrnl::HalBootSMCVideoMode);

	if (bLLE_USB) {
#if 0 // Reenable this when LLE USB actually works
		int ret;
		g_InputDeviceManager = new InputDeviceManager;
		ret = g_InputDeviceManager->EnumSdl2Devices();
		g_InputDeviceManager->StartInputThread();
		if (ret > 0) {
			// Temporary: the device type and bindings should be read from emushared, for now always assume one xbox controller
			g_InputDeviceManager->ConnectDeviceToXbox(1, MS_CONTROLLER_DUKE);
		}
#endif
	}

	// Now the hardware devices exist, couple the EEPROM buffer to it's device
	g_EEPROM->SetEEPROM((uint8_t*)EEPROM);

	if (!bLLE_GPU)
	{
		DBG_PRINTF_EX(LOG_PREFIX_INIT, "Initializing Direct3D.\n");
		XTL::EmuD3DInit();
	}
	
	if (CxbxDebugger::CanReport())
	{
		CxbxDebugger::ReportDebuggerInit(CxbxKrnl_Xbe->m_szAsciiTitle);
	}

	// Apply Media Patches to bypass Anti-Piracy checks
	// Required until we perfect emulation of X2 DVD Authentication
	// See: https://multimedia.cx/eggs/xbox-sphinx-protocol/
	ApplyMediaPatches();

	// Chihiro games require more patches
	// The chihiro BIOS does this to bypass XAPI cache init
	if (g_bIsChihiro) {
		CxbxKrnl_XbeHeader->dwInitFlags.bDontSetupHarddisk = true;
	}

	if(!g_SkipRdtscPatching)
	{ 
		PatchRdtscInstructions();
	}

	// Setup per-title encryption keys
	SetupPerTitleKeys();

	EmuInitFS();

	InitXboxThread(g_CPUXbox);
	xboxkrnl::ObInitSystem();
	xboxkrnl::KiInitSystem();

	EmuX86_Init();
	// Create the interrupt processing thread
	DWORD dwThreadId;
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, NULL, CxbxKrnlInterruptThread, NULL, NULL, (uint*)&dwThreadId);
	// Start the kernel clock thread
	TimerObject* KernelClockThr = Timer_Create(CxbxKrnlClockThread, nullptr, "Kernel clock thread", &g_CPUOthers);
	Timer_Start(KernelClockThr, SCALE_MS_IN_NS);

	DBG_PRINTF_EX(LOG_PREFIX_INIT, "Calling XBE entry point...\n");
	CxbxLaunchXbe(Entry);

	// FIXME: Wait for Cxbx to exit or error fatally
	Sleep(INFINITE);

	DBG_PRINTF_EX(LOG_PREFIX_INIT, "XBE entry point returned\n");
	fflush(stdout);

	//	EmuShared::Cleanup();   FIXME: commenting this line is a bad workaround for issue #617 (https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/617)
    CxbxKrnlTerminateThread();
}

void CxbxInitFilePaths()
{
	g_EmuShared->GetStorageLocation(szFolder_CxbxReloadedData);

	// Make sure our data folder exists :
	bool result = std::experimental::filesystem::exists(szFolder_CxbxReloadedData);
	if (!result && !std::experimental::filesystem::create_directory(szFolder_CxbxReloadedData)) {
		CxbxKrnlCleanup("%s : Couldn't create Cxbx-Reloaded's data folder!", __func__);
	}

	// Make sure the EmuDisk folder exists
	std::string emuDisk = std::string(szFolder_CxbxReloadedData) + std::string("\\EmuDisk");
	result = std::experimental::filesystem::exists(emuDisk);
	if (!result && !std::experimental::filesystem::create_directory(emuDisk)) {
		CxbxKrnlCleanup("%s : Couldn't create Cxbx-Reloaded EmuDisk folder!", __func__);
	}

	snprintf(szFilePath_EEPROM_bin, MAX_PATH, "%s\\EEPROM.bin", szFolder_CxbxReloadedData);
	snprintf(szFilePath_memory_bin, MAX_PATH, "%s\\memory.bin", szFolder_CxbxReloadedData);
	snprintf(szFilePath_page_tables, MAX_PATH, "%s\\PageTables.bin", szFolder_CxbxReloadedData);

	GetModuleFileName(GetModuleHandle(NULL), szFilePath_CxbxReloaded_Exe, MAX_PATH);
}

// REMARK: the following is useless, but PatrickvL has asked to keep it for documentation purposes
/*xboxkrnl::LAUNCH_DATA_PAGE DefaultLaunchDataPage =
{
	{   // header
		2,  // 2: dashboard, 0: title
		0,
		"D:\\default.xbe",
		0
	}
};*/

__declspec(noreturn) void CxbxKrnlCleanupEx(CXBXR_MODULE cxbxr_module, const char *szErrorMessage, ...)
{
    g_bEmuException = true;

    CxbxKrnlResume();

    // print out error message (if exists)
    if(szErrorMessage != NULL)
    {
        char szBuffer2[1024];
        va_list argp;

        va_start(argp, szErrorMessage);
        vsprintf(szBuffer2, szErrorMessage, argp);
        va_end(argp);

		CxbxPopupMessageEx(cxbxr_module, LOG_LEVEL::FATAL, CxbxMsgDlgIcon_Error, "Received Fatal Message:\n\n* %s\n", szBuffer2); // Will also DBG_PRINTF
    }

    printf("[0x%.4X] MAIN: Terminating Process\n", GetCurrentThreadId());
    fflush(stdout);

    // cleanup debug output
    {
        FreeConsole();

        char buffer[16];

        if(GetConsoleTitle(buffer, 16) != NULL)
            freopen("nul", "w", stdout);
    }

	CxbxKrnlShutDown();
}

void CxbxKrnlRegisterThread(HANDLE hThread)
{
	// we must duplicate this handle in order to retain Suspend/Resume thread rights from a remote thread
	{
		HANDLE hDupHandle = NULL;

		if (DuplicateHandle(g_CurrentProcessHandle, hThread, g_CurrentProcessHandle, &hDupHandle, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
			hThread = hDupHandle; // Thread handle was duplicated, continue registration with the duplicate
		}
		else {
			auto message = CxbxGetLastErrorString("DuplicateHandle");
			EmuLog(LOG_LEVEL::WARNING, message.c_str());
		}
	}

	g_hThreads.push_back(hThread);
}

void CxbxKrnlSuspend()
{
    if(g_bEmuSuspended || g_bEmuException)
        return;

    for (auto it = g_hThreads.begin(); it != g_hThreads.end(); ++it)
    {
        DWORD dwExitCode;

        if(GetExitCodeThread(*it, &dwExitCode) && dwExitCode == STILL_ACTIVE) {
            // suspend thread if it is active
            SuspendThread(*it);
        } else {
            // remove thread from thread list if it is dead
			g_hThreads.erase(it);
        }
    }

    // append 'paused' to rendering window caption text
    {
        char szBuffer[256];

        HWND hWnd = GET_FRONT_WINDOW_HANDLE;

        GetWindowText(hWnd, szBuffer, 255 - 10);

        strcat(szBuffer, " (paused)");
        SetWindowText(hWnd, szBuffer);
    }

    g_bEmuSuspended = true;
}

void CxbxKrnlResume()
{
    if(!g_bEmuSuspended)
        return;

    // remove 'paused' from rendering window caption text
    {
        char szBuffer[256];

        HWND hWnd = GET_FRONT_WINDOW_HANDLE;

        GetWindowText(hWnd, szBuffer, 255);

        szBuffer[strlen(szBuffer)-9] = '\0';

        SetWindowText(hWnd, szBuffer);
    }

	for (auto it = g_hThreads.begin(); it != g_hThreads.end(); ++it)
	{
		DWORD dwExitCode;

		if (GetExitCodeThread(*it, &dwExitCode) && dwExitCode == STILL_ACTIVE) {
			// resume thread if it is active
			ResumeThread(*it);
		}
		else {
			// remove thread from thread list if it is dead
			g_hThreads.erase(it);
		}
	}

    g_bEmuSuspended = false;
}

void CxbxKrnlShutDown()
{
	// Clear all kernel boot flags. These (together with the shared memory) persist until Cxbx-Reloaded is closed otherwise.
	int BootFlags = 0;
	g_EmuShared->SetBootFlags(&BootFlags);

	// NOTE: This causes a hang when exiting while NV2A is processing
	// This is okay for now: It won't leak memory or resources since TerminateProcess will free everything
	// delete g_NV2A; // TODO : g_pXbox

	if (CxbxKrnl_hEmuParent != NULL)
		SendMessage(CxbxKrnl_hEmuParent, WM_PARENTNOTIFY, WM_DESTROY, 0);

	EmuShared::Cleanup();
	TerminateProcess(g_CurrentProcessHandle, 0);
}

void CxbxKrnlPrintUEM(ULONG ErrorCode)
{
	ULONG Type;
	xboxkrnl::XBOX_EEPROM Eeprom;
	ULONG ResultSize;

	xboxkrnl::NTSTATUS status = xboxkrnl::ExQueryNonVolatileSetting(xboxkrnl::XC_MAX_ALL, &Type, &Eeprom, sizeof(Eeprom), &ResultSize);

	if (status == STATUS_SUCCESS)
	{
		xboxkrnl::XBOX_UEM_INFO* UEMInfo = (xboxkrnl::XBOX_UEM_INFO*)&(Eeprom.UEMInfo[0]);

		if (UEMInfo->ErrorCode == FATAL_ERROR_NONE)
		{
			// ergo720: the Xbox sets the error code and displays the UEM only for non-manufacturing xbe's (it power cycles
			// otherwise). Considering that this flag can be easily tampered with in the xbe and the typical end user of cxbx
			// can't fix the cause of the fatal error, I decided to always display it anyway.

			UEMInfo->ErrorCode = (UCHAR)ErrorCode;
			UEMInfo->History |= (1 << (ErrorCode - 5));
		}
		else {
			UEMInfo->ErrorCode = FATAL_ERROR_NONE;
		}
		xboxkrnl::ExSaveNonVolatileSetting(xboxkrnl::XC_MAX_ALL, Type, &Eeprom, sizeof(Eeprom));
	}
	else {
		CxbxKrnlCleanup("Could not display the fatal error screen");
	}

	if (g_bIsChihiro)
	{
		// The Chihiro doesn't display the UEM
		CxbxKrnlCleanup("The running Chihiro xbe has encountered a fatal error and needs to close");
	}

	g_CxbxFatalErrorCode = ErrorCode;
	g_CxbxPrintUEM = true; // print the UEM

	CxbxPrintUEMInfo(ErrorCode);

	// Sleep forever to prevent continuing the initialization
	Sleep(INFINITE);
}

void CxbxPrintUEMInfo(ULONG ErrorCode)
{
	// See here for a description of the error codes and their meanings:
	// https://www.reddit.com/r/originalxbox/wiki/error_codes

	std::map<int, std::string> UEMErrorTable;

	UEMErrorTable.emplace(FATAL_ERROR_CORE_DIGITAL, "General motherboard issue");
	UEMErrorTable.emplace(FATAL_ERROR_BAD_EEPROM, "General EEPROM issue");
	UEMErrorTable.emplace(FATAL_ERROR_BAD_RAM, "RAM failure");
	UEMErrorTable.emplace(FATAL_ERROR_HDD_NOT_LOCKED, "HDD is not locked");
	UEMErrorTable.emplace(FATAL_ERROR_HDD_CANNOT_UNLOCK, "Unable to unlock HDD (bad password?)");
	UEMErrorTable.emplace(FATAL_ERROR_HDD_TIMEOUT, "HDD failed to respond");
	UEMErrorTable.emplace(FATAL_ERROR_HDD_NOT_FOUND, "Missing HDD");
	UEMErrorTable.emplace(FATAL_ERROR_HDD_BAD_CONFIG, "Invalid / missing HDD parameter(s)");
	UEMErrorTable.emplace(FATAL_ERROR_DVD_TIMEOUT, "DVD drive failed to respond");
	UEMErrorTable.emplace(FATAL_ERROR_DVD_NOT_FOUND, "Missing DVD drive");
	UEMErrorTable.emplace(FATAL_ERROR_DVD_BAD_CONFIG, "Invalid / missing DVD drive parameter(s)");
	UEMErrorTable.emplace(FATAL_ERROR_XBE_DASH_GENERIC, "Generic MS dashboard issue (dashboard not installed?)");
	UEMErrorTable.emplace(FATAL_ERROR_XBE_DASH_ERROR, "General MS dashboard issue");
	UEMErrorTable.emplace(FATAL_ERROR_XBE_DASH_SETTINGS, "MS dashboard issue: cannot reset console clock");
	UEMErrorTable.emplace(FATAL_ERROR_XBE_DASH_X2_PASS, "General MS dashboard issue, DVD drive authentication was successfull");
	UEMErrorTable.emplace(FATAL_ERROR_REBOOT_ROUTINE, "The console was instructed to reboot to this error screen");

	auto it = UEMErrorTable.find(ErrorCode);
	if (it != UEMErrorTable.end())
	{
		std::string ErrorMessage = "Fatal error. " + it->second + ". This error screen will persist indefinitely. Stop the emulation to close it";
		CxbxPopupMessage(LOG_LEVEL::FATAL, CxbxMsgDlgIcon_Error, ErrorMessage.c_str());
	}
	else
	{
		CxbxPopupMessage(LOG_LEVEL::FATAL, CxbxMsgDlgIcon_Error, "Unknown fatal error. This error screen will persist indefinitely. Stop the emulation to close it");
	}
}

__declspec(noreturn) void CxbxKrnlTerminateThread()
{
    TerminateThread(GetCurrentThread(), 0);
}

void CxbxKrnlPanic()
{
    CxbxKrnlCleanup("Kernel Panic!");
}

void CxbxConvertArgToString(std::string &dest, const char* krnlExe, const char* xbeFile, HWND hwndParent, DebugMode krnlDebug, const char* krnlDebugFile) {

    std::stringstream szArgsStream;

    // The format is: "krnlExe" /load "xbeFile" hwndParent krnlDebug "krnlDebugFile"
    szArgsStream <<
        "\"" << krnlExe << "\""
        " /load \"" << xbeFile << "\""
        " " << std::dec << (int)hwndParent <<
        " " << std::dec << (int)krnlDebug <<
        " \"" << krnlDebugFile << "\"";

    dest = szArgsStream.str();
}

static clock_t						g_DeltaTime = 0;			 // Used for benchmarking/fps count
static unsigned int					g_Frames = 0;

// ******************************************************************
// * update the current milliseconds per frame
// ******************************************************************
static void UpdateCurrentMSpFAndFPS() {
	if (g_EmuShared) {
		static float currentFPSVal = 30;

		currentFPSVal = (float)(g_Frames*0.5 + currentFPSVal * 0.5);
		g_EmuShared->SetCurrentFPS(&currentFPSVal);
	}
}

void UpdateFPSCounter()
{
	static clock_t lastDrawFunctionCallTime = 0;
	clock_t currentDrawFunctionCallTime = clock();

	g_DeltaTime += currentDrawFunctionCallTime - lastDrawFunctionCallTime;
	lastDrawFunctionCallTime = currentDrawFunctionCallTime;
	g_Frames++;

	if (g_DeltaTime >= CLOCKS_PER_SEC) {
		UpdateCurrentMSpFAndFPS();
		g_Frames = 0;
		g_DeltaTime -= CLOCKS_PER_SEC;
	}
}
