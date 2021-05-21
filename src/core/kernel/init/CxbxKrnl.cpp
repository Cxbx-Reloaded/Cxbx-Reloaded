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

#define LOG_PREFIX CXBXR_MODULE::CXBXR
#define LOG_PREFIX_INIT CXBXR_MODULE::INIT


#include <core\kernel\exports\xboxkrnl.h>
#include "gui/resource/ResCxbx.h"
#include "core\kernel\init\CxbxKrnl.h"
#include "common\xbdm\CxbxXbdm.h" // For Cxbx_LibXbdmThunkTable
#include "CxbxVersion.h"
#include "core\kernel\support\Emu.h"
#include "devices\x86\EmuX86.h"
#include "core\kernel\support\EmuFile.h"
#include "core\kernel\support\EmuFS.h" // EmuInitFS
#include "EmuEEPROM.h" // For CxbxRestoreEEPROM, EEPROM, XboxFactoryGameRegion
#include "core\kernel\exports\EmuKrnl.h"
#include "core\kernel\exports\EmuKrnlKi.h"
#include "EmuShared.h"
#include "core\hle\D3D8\Direct3D9\Direct3D9.h" // For CxbxInitWindow, EmuD3DInit
#include "core\hle\DSOUND\DirectSound\DirectSound.hpp" // For CxbxInitAudio
#include "core\hle\Intercept.hpp"
#include "ReservedMemory.h" // For virtual_memory_placeholder
#include "core\kernel\memory-manager\VMManager.h"
#include "CxbxDebugger.h"
#include "common/util/cliConfig.hpp"
#include "xxhash.h"
#include "common/ReserveAddressRanges.h"
#include "common/xbox/Types.hpp"
#include "common/win32/WineEnv.h"
#include "core/common/video/RenderBase.hpp"

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
#include "common\input\InputManager.h" // For the InputDeviceManager

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
char szFilePath_Xbe[xbox::max_path*2] = { 0 }; // NOTE: LAUNCH_DATA_HEADER's szLaunchPath is xbox::max_path*2 = 520

std::string CxbxBasePath;
HANDLE CxbxBasePathHandle;
Xbe* CxbxKrnl_Xbe = NULL;
bool g_bIsChihiro = false;
bool g_bIsDebug = false;
bool g_bIsRetail = false;

// Indicates to disable/enable all interrupts when cli and sti instructions are executed
std::atomic_bool g_bEnableAllInterrupts = true;

// Set by the VMManager during initialization. Exported because it's needed in other parts of the emu
size_t g_SystemMaxMemory = 0;

HANDLE g_CurrentProcessHandle = 0; // Set in CxbxKrnlMain

bool g_CxbxPrintUEM = false;
ULONG g_CxbxFatalErrorCode = FATAL_ERROR_NONE;

// Define function located in EmuXApi so we can call it from here
void SetupXboxDeviceTypes();

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
	xbox::XcHMAC(xbox::XboxCertificateKey, xbox::XBOX_KEY_LENGTH, g_pCertificate->bzLanKey, xbox::XBOX_KEY_LENGTH, NULL, 0, Digest);
	memcpy(xbox::XboxLANKey, Digest, xbox::XBOX_KEY_LENGTH);

	// Signature Key
	xbox::XcHMAC(xbox::XboxCertificateKey, xbox::XBOX_KEY_LENGTH, g_pCertificate->bzSignatureKey, xbox::XBOX_KEY_LENGTH, NULL, 0, Digest);
	memcpy(xbox::XboxSignatureKey, Digest, xbox::XBOX_KEY_LENGTH);

	// Alternate Signature Keys
	for (int i = 0; i < xbox::ALTERNATE_SIGNATURE_COUNT; i++) {
		xbox::XcHMAC(xbox::XboxCertificateKey, xbox::XBOX_KEY_LENGTH, g_pCertificate->bzTitleAlternateSignatureKey[i], xbox::XBOX_KEY_LENGTH, NULL, 0, Digest);
		memcpy(xbox::XboxAlternateSignatureKeys[i], Digest, xbox::XBOX_KEY_LENGTH);
	}

}

void CxbxLaunchXbe(void(*Entry)())
{
	Entry();
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

#pragma optimize("", off)

void PrintCurrentConfigurationLog()
{
	if (isWineEnv()) {
		EmuLogInit(LOG_LEVEL::INFO, "Running under Wine Version %s", getWineVersion());
	}

	// HACK: For API TRace..
	// bLLE_GPU = true;

	// Print current LLE configuration
	{
		EmuLogInit(LOG_LEVEL::INFO, "---------------------------- LLE CONFIG ----------------------------");
		EmuLogInit(LOG_LEVEL::INFO, "LLE for APU is %s", bLLE_APU ? "enabled" : "disabled");
		EmuLogInit(LOG_LEVEL::INFO, "LLE for GPU is %s", bLLE_GPU ? "enabled" : "disabled");
		EmuLogInit(LOG_LEVEL::INFO, "LLE for USB is %s", bLLE_USB ? "enabled" : "disabled");
		EmuLogInit(LOG_LEVEL::INFO, "LLE for JIT is %s", bLLE_JIT ? "enabled" : "disabled");
	}

	// Print current video configuration (DirectX/HLE)
	if (!bLLE_GPU) {
		Settings::s_video XBVideoConf;
		g_EmuShared->GetVideoSettings(&XBVideoConf);

		EmuLogInit(LOG_LEVEL::INFO, "--------------------------- VIDEO CONFIG ---------------------------");
		EmuLogInit(LOG_LEVEL::INFO, "Direct3D Device: %s", XBVideoConf.direct3DDevice == 0 ? "Direct3D HAL (Hardware Accelerated)" : "Direct3D REF (Software)");
		EmuLogInit(LOG_LEVEL::INFO, "Video Resolution: %s", XBVideoConf.szVideoResolution);
		EmuLogInit(LOG_LEVEL::INFO, "Force VSync is %s", XBVideoConf.bVSync ? "enabled" : "disabled");
		EmuLogInit(LOG_LEVEL::INFO, "Fullscreen is %s", XBVideoConf.bFullScreen ? "enabled" : "disabled");
		EmuLogInit(LOG_LEVEL::INFO, "Maintain Aspect is %s", XBVideoConf.bMaintainAspect ? "enabled" : "disabled");
	}

	// Print current audio configuration
	{
		Settings::s_audio XBAudioConf;
		g_EmuShared->GetAudioSettings(&XBAudioConf);

		EmuLogInit(LOG_LEVEL::INFO, "--------------------------- AUDIO CONFIG ---------------------------");
		EmuLogInit(LOG_LEVEL::INFO, "Audio Adapter: %s", XBAudioConf.adapterGUID.Data1 == 0 ? "Primary Audio Device" : "Secondary Audio Device");
		EmuLogInit(LOG_LEVEL::INFO, "PCM is %s", XBAudioConf.codec_pcm ? "enabled" : "disabled");
		EmuLogInit(LOG_LEVEL::INFO, "XADPCM is %s", XBAudioConf.codec_xadpcm ? "enabled" : "disabled");
		EmuLogInit(LOG_LEVEL::INFO, "Unknown Codec is %s", XBAudioConf.codec_unknown ? "enabled" : "disabled");
	}

	// Print current network configuration
	{
		Settings::s_network XBNetworkConf;
		g_EmuShared->GetNetworkSettings(&XBNetworkConf);

		EmuLogInit(LOG_LEVEL::INFO, "--------------------------- NETWORK CONFIG -------------------------");
		EmuLogInit(LOG_LEVEL::INFO, "Network Adapter Name: %s", strlen(XBNetworkConf.adapter_name) == 0 ? "Not Configured" : XBNetworkConf.adapter_name);
	}

	// Print Enabled Hacks
	{
		EmuLogInit(LOG_LEVEL::INFO, "--------------------------- HACKS CONFIG ---------------------------");
		EmuLogInit(LOG_LEVEL::INFO, "Disable Pixel Shaders: %s", g_DisablePixelShaders == 1 ? "On" : "Off (Default)");
		EmuLogInit(LOG_LEVEL::INFO, "Run Xbox threads on all cores: %s", g_UseAllCores == 1 ? "On" : "Off (Default)");
		EmuLogInit(LOG_LEVEL::INFO, "Skip RDTSC Patching: %s", g_SkipRdtscPatching == 1 ? "On" : "Off (Default)");
	}

	EmuLogInit(LOG_LEVEL::INFO, "------------------------- END OF CONFIG LOG ------------------------");
	
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
	xbox::KINTERRUPT SoftwareInterrupt_1;
	SoftwareInterrupt_1.BusInterruptLevel = 1;
	SoftwareInterrupt_1.ServiceRoutine = ApcInterrupt;
	xbox::KeConnectInterrupt(&SoftwareInterrupt_1);

	// Init software interrupt 2 (for DPC dispatching)
	xbox::KINTERRUPT SoftwareInterrupt_2;
	SoftwareInterrupt_2.BusInterruptLevel = 2;
	SoftwareInterrupt_2.ServiceRoutine = DispatchInterrupt;
	xbox::KeConnectInterrupt(&SoftwareInterrupt_2);
}
#endif

void TriggerPendingConnectedInterrupts()
{
	for (int i = 0; i < MAX_BUS_INTERRUPT_LEVEL; i++) {
		// If the interrupt is pending and connected, process it
		if (HalSystemInterrupts[i].IsPending() && EmuInterruptList[i] && EmuInterruptList[i]->Connected) {
			HalSystemInterrupts[i].Trigger(EmuInterruptList[i]);
		}
		SwitchToThread();
	}
}

static unsigned int WINAPI CxbxKrnlInterruptThread(PVOID param)
{
	CxbxSetThreadName("CxbxKrnl Interrupts");

	// Make sure Xbox1 code runs on one core :
	InitXboxThread();
	g_AffinityPolicy->SetAffinityXbox();

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

	xbox::KiClockIsr(IncrementScaling);
}

std::vector<xbox::addr_xt> g_RdtscPatches;

#define OPCODE_PATCH_RDTSC 0x90EF  // OUT DX, EAX; NOP

bool IsRdtscInstruction(xbox::addr_xt addr)
{
	// First the fastest check - does addr contain exact patch from PatchRdtsc?
	// Second check - is addr on the rdtsc patch list?
	return (*(uint16_t*)addr == OPCODE_PATCH_RDTSC)
		// Note : It's not needed to check for g_SkipRdtscPatching,
		// as when that's set, the g_RdtscPatches vector will be empty
		// anyway, failing this lookup :
		&& (std::find(g_RdtscPatches.begin(), g_RdtscPatches.end(), addr) != g_RdtscPatches.end());
}

void PatchRdtsc(xbox::addr_xt addr)
{
	// Patch away rdtsc with an opcode we can intercept
	// We use a privilaged instruction rather than int 3 for debugging
	// When using int 3, attached debuggers trap and rdtsc is used often enough
	// that it makes Cxbx-Reloaded unusable
	// A privilaged instruction (like OUT) does not suffer from this
	EmuLogInit(LOG_LEVEL::DEBUG, "Patching rdtsc opcode at 0x%.8X", (DWORD)addr);
	*(uint16_t*)addr = OPCODE_PATCH_RDTSC;
	g_RdtscPatches.push_back(addr);
}

const uint8_t rdtsc_pattern[] = {
	0x89,//{ 0x0F,0x31,0x89 },   // two false positives in Stranger's Wrath
	0xC3,//{ 0x0F,0x31,0xC3 },
	0x8B,//{ 0x0F,0x31,0x8B },   // one false positive in Sonic Rider .text 88 5C 0F 31, two false positives in Stranger's Wrath
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
	0xF7,//{ 0x0F,0x31,0xF7 }, // one false positive in Stranger's Wrath
	0x8A,//{ 0x0F,0x31,0x8A }, // 8A and 56 only apears in RalliSport 2 .text , need to watch whether any future false positive.
	0x56,//{ 0x0F,0x31,0x56 }
    0x6A,                      // 6A, 39, EB, F6, A1, 01 only appear in Unreal Championship, 01 is at WMVDEC section
    0x39,
    0xEB,
    0xF6,
    0xA1,
    0x01,                      // one false positive in Group S Challenge [1.05] .text E8 0F 31 01 00
    0xA3
};
const int sizeof_rdtsc_pattern = sizeof(rdtsc_pattern);

void PatchRdtscInstructions()
{
	uint8_t rdtsc[2] = { 0x0F, 0x31 };
	DWORD sizeOfImage = CxbxKrnl_XbeHeader->dwSizeofImage;

	// Iterate through each CODE section
	for (uint32_t sectionIndex = 0; sectionIndex < CxbxKrnl_Xbe->m_Header.dwSections; sectionIndex++) {
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

		EmuLogInit(LOG_LEVEL::INFO, "Searching for rdtsc in section %s", CxbxKrnl_Xbe->m_szSectionName[sectionIndex]);
		xbox::addr_xt startAddr = CxbxKrnl_Xbe->m_SectionHeader[sectionIndex].dwVirtualAddr;
		//rdtsc is two bytes instruction, it needs at least one opcode byte after it to finish a function, so the endAddr need to substract 3 bytes.
		xbox::addr_xt endAddr = startAddr + CxbxKrnl_Xbe->m_SectionHeader[sectionIndex].dwSizeofRaw-3;
		for (xbox::addr_xt addr = startAddr; addr <= endAddr; addr++) 
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
								EmuLogInit(LOG_LEVEL::INFO, "Skipped false positive: rdtsc pattern  0x%.2X, @ 0x%.8X", next_byte, (DWORD)addr);
								continue;
							}

							if (*(uint8_t *)(addr - 2) == 0x24 && *(uint8_t *)(addr - 1) == 0x0C)
							{
								EmuLogInit(LOG_LEVEL::INFO, "Skipped false positive: rdtsc pattern  0x%.2X, @ 0x%.8X", next_byte, (DWORD)addr);
								continue;
							}
						}
						if (next_byte == 0x89)
						{
							if (*(uint8_t *)(addr + 4) == 0x8B && *(uint8_t *)(addr - 5) == 0x04)
							{
								EmuLogInit(LOG_LEVEL::INFO, "Skipped false positive: rdtsc pattern  0x%.2X, @ 0x%.8X", next_byte, (DWORD)addr);
								continue;
							}

						}
						if (next_byte == 0x50)
						{
							if (*(uint8_t*)(addr - 2) == 0x83 && *(uint8_t*)(addr - 1) == 0xE2)
							{
								EmuLogInit(LOG_LEVEL::INFO, "Skipped false positive: rdtsc pattern  0x%.2X, @ 0x%.8X", next_byte, (DWORD)addr);
								continue;
							}

						}
						if (next_byte == 0x01)
						{
							if (*(uint8_t*)(addr - 1) == 0xE8 && *(uint8_t*)(addr + 3) == 0x00)
							{
								EmuLogInit(LOG_LEVEL::INFO, "Skipped false positive: rdtsc pattern  0x%.2X, @ 0x%.8X", next_byte, (DWORD)addr);
								continue;
							}

						}
						if (next_byte == 0xF7)
						{
							if (*(uint8_t *)(addr - 1) == 0xE8 && *(uint8_t *)(addr + 3) == 0xFF)
							{
								EmuLogInit(LOG_LEVEL::INFO, "Skipped false positive: rdtsc pattern  0x%.2X, @ 0x%.8X", next_byte, (DWORD)addr);
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
					EmuLogInit(LOG_LEVEL::INFO, "Skipped potential rdtsc: Unknown opcode pattern  0x%.2X, @ 0x%.8X", next_byte, (DWORD)addr);
				}
			}
		}
	}

	EmuLogInit(LOG_LEVEL::INFO, "Done patching rdtsc, total %d rdtsc instructions patched", g_RdtscPatches.size());
}

void MapThunkTable(uint32_t* kt, uint32_t* pThunkTable)
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
	xbox::addr_xt ThunkAddr;
	xbox::addr_xt LibNameAddr;
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
			// TODO: replace wprintf to EmuLogInit, how?
			wprintf(L"LOAD : Skipping unrecognized import library : %s\n", LibName.c_str());
		}

		pImportDirectory++;
	}
}

bool CreateSettings()
{
	g_Settings = new Settings();
	if (g_Settings == nullptr) {
		PopupError(nullptr, szSettings_alloc_error);
		return false;
	}

	if (!g_Settings->Init()) {
		return false;
	}

	log_get_settings();
	return true;
}

bool HandleFirstLaunch()
{
	bool bFirstLaunch;
	g_EmuShared->GetIsFirstLaunch(&bFirstLaunch);

	/* check if process is launch with elevated access then prompt for continue on or not. */
	if (!bFirstLaunch) {
		if (!CreateSettings()) {
			return false;
		}

		// Wine will always run programs as administrator by default, it can be safely disregard.
		// Since Wine doesn't use root permission. Unless user is running Wine as root.
		bool bElevated = CxbxIsElevated();
		if (bElevated && !isWineEnv() && !g_Settings->m_core.allowAdminPrivilege) {
			PopupReturn ret = PopupWarningEx(nullptr, PopupButtons::YesNo, PopupReturn::No,
				"Cxbx-Reloaded has detected that it has been launched with Administrator rights.\n"
				"\nThis is dangerous, as a maliciously modified Xbox titles could take control of your system.\n"
				"\nAre you sure you want to continue?");
			if (ret != PopupReturn::Yes) {
				return false;
			}
		}

		g_EmuShared->SetIsFirstLaunch(true);
	}

	return true;
}

void CxbxKrnlEmulate(unsigned int reserved_systems, blocks_reserved_t blocks_reserved)
{
	// First of all, check if the EmuShared version matches the emu version and abort otherwise
	char GitVersionEmuShared[GitVersionMaxLength];
	g_EmuShared->GetGitVersion(GitVersionEmuShared);
	if (std::strncmp(GitVersionEmuShared, GetGitVersionStr(), GetGitVersionLength()) != 0) {
		PopupError(nullptr, "Mismatch detected between EmuShared and cxbx.exe/cxbxr-emu.dll, aborting."
			"\n\nPlease extract all contents from zip file and do not mix with older/newer builds.");
		CxbxKrnlShutDown();
		return;
	}

	std::string tempStr;

	// NOTE: This is designated for standalone kernel mode launch without GUI
	if (g_Settings != nullptr) {

		// Reset to default
		g_EmuShared->Reset();

		g_Settings->Verify();
		g_Settings->SyncToEmulator();

		// We don't need to keep Settings open plus allow emulator to use unused memory.
		delete g_Settings;
		g_Settings = nullptr;

		// Perform identical to what GUI will do to certain EmuShared's variable before launch.
		g_EmuShared->SetIsEmulating(true);

		// NOTE: This setting the ready status is optional. Internal kernel process is checking if GUI is running.
		// Except if enforce check, then we need to re-set ready status every time for non-GUI.
		//g_EmuShared->SetIsReady(true);
	}

	/* Initialize ClipCursor flag from EmuShared */
	g_EmuShared->GetClipCursorFlag(&g_bClipCursor);

	/* Initialize popup message management from kernel side. */
	log_init_popup_msg();

	/* Initialize Cxbx File Paths */
	CxbxInitFilePaths();

	// Skip '/load' switch
	// Get XBE Name :
	std::string xbePath;
	cli_config::GetValue(cli_config::load, &xbePath);
	xbePath = std::filesystem::absolute(std::filesystem::path(xbePath)).string();

	// Get DCHandle :
	// We must save this handle now to keep the child window working in the case we need to display the UEM
	HWND hWnd = nullptr;
	if (cli_config::GetValue(cli_config::hwnd, &tempStr)) {
		hWnd = (HWND)std::atoi(tempStr.c_str());
	}
	CxbxKrnl_hEmuParent = IsWindow(hWnd) ? hWnd : nullptr;

	// Get KernelDebugMode :
	DebugMode DbgMode = DebugMode::DM_NONE;
	if (cli_config::GetValue(cli_config::debug_mode, &tempStr)) {
		DbgMode = (DebugMode)std::atoi(tempStr.c_str());
	}

	// Get KernelDebugFileName :
	std::string DebugFileName = "";
	if (cli_config::GetValue(cli_config::debug_file, &tempStr)) {
		DebugFileName = tempStr;
	}

	int BootFlags;
	g_EmuShared->GetBootFlags(&BootFlags);

	g_CurrentProcessHandle = GetCurrentProcess(); // OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());

	// Set up the logging variables for the kernel process during initialization.
	log_sync_config();

	// When a reboot occur, we need to keep persistent memory buffer open before emulation process shutdown.
	if ((BootFlags & BOOT_QUICK_REBOOT) != 0) {
		g_VMManager.GetPersistentMemory();
	}

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
				PopupReturn mbRet = PopupWarningEx(nullptr, PopupButtons::RetryCancel, PopupReturn::Cancel,
					"GUI process is not ready, do you wish to retry?");
				if (mbRet == PopupReturn::Retry) {
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

	// Save current kernel proccess id for next reboot if will occur in the future.
	// And to tell previous kernel process we had take over. This allow reboot's shared memory buffer to survive.
	g_EmuShared->SetKrnlProcID(GetCurrentProcessId());

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

	if (dwExitCode != EXIT_SUCCESS) {// Stop emulation
		CxbxKrnlShutDown();
	}

	/* Must be called after CxbxInitFilePaths and previous kernel process shutdown. */
	if (!CxbxLockFilePath()) {
		return;
	}

	FILE* krnlLog = nullptr;
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
			(void)freopen("CONOUT$", "wt", stdout);
			(void)freopen("CONIN$", "rt", stdin);
			SetConsoleTitle("Cxbx-Reloaded : Kernel Debug Console");
			SetConsoleTextAttribute(StdHandle, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
		}
	}
	else
	{
		FreeConsole();
		if (DbgMode == DM_FILE) {
			// Peform clean write to kernel log for first boot. Unless multi-xbe boot occur then perform append to existing log.
			krnlLog = freopen(DebugFileName.c_str(), ((BootFlags == DebugMode::DM_NONE) ? "wt" : "at"), stdout);
			// Append separator for better readability after reboot.
			if (BootFlags != DebugMode::DM_NONE) {
				std::cout << "\n------REBOOT------REBOOT------REBOOT------REBOOT------REBOOT------\n" << std::endl;
			}
		}
		else {
			char buffer[16];
			if (GetConsoleTitle(buffer, 16) != NULL)
				(void)freopen("nul", "w", stdout);
		}
	}

	bool isLogEnabled;
	g_EmuShared->GetIsKrnlLogEnabled(&isLogEnabled);
	g_bPrintfOn = isLogEnabled;

	g_EmuShared->ResetKrnl();

	// Write a header to the log
	{
		EmuLogInit(LOG_LEVEL::INFO, "Cxbx-Reloaded Version %s", CxbxVersionStr);

		time_t startTime = time(nullptr);
		struct tm* tm_info = localtime(&startTime);
		char timeString[26];
		strftime(timeString, 26, "%F %T", tm_info);
		EmuLogInit(LOG_LEVEL::INFO, "Log started at %s", timeString);

#ifdef _DEBUG_TRACE
		EmuLogInit(LOG_LEVEL::INFO, "Debug Trace Enabled.");
#else
		EmuLogInit(LOG_LEVEL::INFO, "Debug Trace Disabled.");
#endif
	}

	// Log once, since multi-xbe boot is appending to file instead of overwrite.
	if (BootFlags == BOOT_NONE) {
		log_generate_active_filter_output(CXBXR_MODULE::INIT);
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
			PopupFatal(nullptr, "Cxbx-Reloaded executuable requires it's base of code to be 0x00001000");
			return; // TODO : Halt(0); 
		}

#ifndef CXBXR_EMU
		// verify virtual_memory_placeholder is located at 0x00011000
		if ((UINT_PTR)(&(virtual_memory_placeholder[0])) != (XBE_IMAGE_BASE + CXBX_BASE_OF_CODE))
		{
			PopupFatal(nullptr, "virtual_memory_placeholder is not loaded to base address 0x00011000 (which is a requirement for Xbox emulation)");
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

	// Load Per-Xbe Keys from the Cxbx-Reloaded AppData directory
	LoadXboxKeys(szFolder_CxbxReloadedData);

	EEPROM = CxbxRestoreEEPROM(szFilePath_EEPROM_bin);
	if (EEPROM == nullptr)
	{
		PopupFatal(nullptr, "Couldn't init EEPROM!");
		return; // TODO : Halt(0); 
	}

	// TODO : Instead of loading an Xbe here, initialize the kernel so that it will launch the Xbe on itself.
	// using XeLoadImage from LaunchDataPage->Header.szLaunchPath

	// Now we can load and run the XBE :
	// MapAndRunXBE(XbePath, DCHandle);
	XbeType xbeType = XbeType::xtRetail;
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
		strncpy(szFilePath_Xbe, xbePath.c_str(), xbox::max_path - 1);
		std::replace(xbePath.begin(), xbePath.end(), ';', '/');
		// Load Xbe (this one will reside above WinMain's virtual_memory_placeholder)
		CxbxKrnl_Xbe = new Xbe(xbePath.c_str(), false); // TODO : Instead of using the Xbe class, port Dxbx _ReadXbeBlock()

		if (CxbxKrnl_Xbe->HasFatalError()) {
			CxbxKrnlCleanup(CxbxKrnl_Xbe->GetError().c_str());
			return;
		}

		// Check the signature of the xbe
		if (CxbxKrnl_Xbe->CheckSignature()) {
			EmuLogInit(LOG_LEVEL::INFO, "Valid xbe signature. Xbe is legit");
		}
		else {
			EmuLogInit(LOG_LEVEL::WARNING, "Invalid xbe signature. Homebrew, tampered or pirated xbe?");
		}

		// Check the integrity of the xbe sections
		for (uint32_t sectionIndex = 0; sectionIndex < CxbxKrnl_Xbe->m_Header.dwSections; sectionIndex++) {
			if (CxbxKrnl_Xbe->CheckSectionIntegrity(sectionIndex)) {
				EmuLogInit(LOG_LEVEL::INFO, "SHA hash check of section %s successful", CxbxKrnl_Xbe->m_szSectionName[sectionIndex]);
			}
			else {
				EmuLogInit(LOG_LEVEL::WARNING, "SHA hash of section %s doesn't match, section is corrupted", CxbxKrnl_Xbe->m_szSectionName[sectionIndex]);
			}
		}

		// If CLI has given console type, then enforce it.
		if (cli_config::hasKey(cli_config::system_chihiro)) {
			EmuLogInit(LOG_LEVEL::INFO, "Auto detect is disabled, running as chihiro.");
			xbeType = XbeType::xtChihiro;
		}
		else if (cli_config::hasKey(cli_config::system_devkit)) {
			EmuLogInit(LOG_LEVEL::INFO, "Auto detect is disabled, running as devkit.");
			xbeType = XbeType::xtDebug;
		}
		else if (cli_config::hasKey(cli_config::system_retail)) {
			EmuLogInit(LOG_LEVEL::INFO, "Auto detect is disabled, running as retail.");
			xbeType = XbeType::xtRetail;
		}
		// Otherwise, use auto detect method.
		else {
			// Detect XBE type :
			xbeType = CxbxKrnl_Xbe->GetXbeType();
			EmuLogInit(LOG_LEVEL::INFO, "Auto detect: XbeType = %s", GetXbeTypeToStr(xbeType));
		}

		EmuLogInit(LOG_LEVEL::INFO, "Host's compatible system types: %2X", reserved_systems);
		unsigned int emulate_system = 0;
		// Set reserved_systems which system we will about to emulate.
		if (isSystemFlagSupport(reserved_systems, SYSTEM_CHIHIRO) && xbeType == XbeType::xtChihiro) {
			emulate_system = SYSTEM_CHIHIRO;
		}
		else if (isSystemFlagSupport(reserved_systems, SYSTEM_DEVKIT) && xbeType == XbeType::xtDebug) {
			emulate_system = SYSTEM_DEVKIT;
		}
		else if (isSystemFlagSupport(reserved_systems, SYSTEM_XBOX) && xbeType == XbeType::xtRetail) {
			emulate_system = SYSTEM_XBOX;
		}
		// If none of system type requested to emulate isn't supported on host's end. Then enforce failure.
		else {
			CxbxKrnlCleanup("Unable to emulate system type due to host is not able to reserve required memory ranges.");
			return;
		}
		// Clear emulation system from reserved systems to be free.
		reserved_systems &= ~emulate_system;

		// Once we have determine which system type to run as, enforce it in future reboots.
		if ((BootFlags & BOOT_QUICK_REBOOT) == 0) {
			const char* system_str = GetSystemTypeToStr(emulate_system);
			cli_config::SetSystemType(system_str);
		}

		// Register if we're running an Chihiro executable or a debug xbe, otherwise it's an Xbox retail executable
		g_bIsChihiro = (xbeType == XbeType::xtChihiro);
		g_bIsDebug = (xbeType == XbeType::xtDebug);
		g_bIsRetail = (xbeType == XbeType::xtRetail);

		// Disabled: The media board rom fails to run because it REQUIRES LLE USB, which is not yet enabled.
		// Chihiro games can be ran directly for now. 
		// This just means that you cannot access the Chihiro test menus and related stuff, games should still be okay
#if 0   
		// If the Xbe is Chihiro, and we were not launched by SEGABOOT, we need to load SEGABOOT from the Chihiro Media Board rom instead!
		// TODO: We also need to store the path of the loaded game, and mount it as the mediaboard filesystem
		// TODO: How to we detect who launched us, to prevent a reboot-loop
		if (g_bIsChihiro) {
			std::string chihiroMediaBoardRom = std::string(szFolder_CxbxReloadedData) + std::string("/EmuDisk/") + MediaBoardRomFile;
			if (!std::filesystem::exists(chihiroMediaBoardRom)) {
				CxbxKrnlCleanup("Chihiro Media Board ROM (fpr21042_m29w160et.bin) could not be found");
			}

			delete CxbxKrnl_Xbe;
			CxbxKrnl_Xbe = new Xbe(chihiroMediaBoardRom.c_str(), false);
		}
#endif

#ifndef CXBXR_EMU
		// Only for GUI executable with emulation code.
		blocks_reserved_t blocks_reserved_gui = { 0 };
		// Reserve console system's memory ranges before start initialize.
		if (!ReserveAddressRanges(emulate_system, blocks_reserved_gui)) {
			CxbxKrnlCleanup("Failed to reserve required memory ranges!", GetLastError());
		}
		// Initialize the memory manager
		g_VMManager.Initialize(emulate_system, BootFlags, blocks_reserved_gui);
#else
		// Release unnecessary memory ranges to allow console/host to use those memory ranges.
		FreeAddressRanges(emulate_system, reserved_systems, blocks_reserved);
		// Initialize the memory manager
		g_VMManager.Initialize(emulate_system, BootFlags, blocks_reserved);
#endif

		// Commit the memory used by the xbe header
		size_t HeaderSize = CxbxKrnl_Xbe->m_Header.dwSizeofHeaders;
		VAddr XbeBase = XBE_IMAGE_BASE;
		g_VMManager.XbAllocateVirtualMemory(&XbeBase, 0, &HeaderSize, XBOX_MEM_COMMIT, XBOX_PAGE_READWRITE);


		// Copy over loaded Xbe Headers to specified base address
		memcpy((void*)CxbxKrnl_Xbe->m_Header.dwBaseAddr, &CxbxKrnl_Xbe->m_Header, sizeof(Xbe::Header));
		memcpy((void*)(CxbxKrnl_Xbe->m_Header.dwBaseAddr + sizeof(Xbe::Header)), CxbxKrnl_Xbe->m_HeaderEx, CxbxKrnl_Xbe->m_ExSize);

		// Load all sections marked as preload using the in-memory copy of the xbe header
		xbox::PXBEIMAGE_SECTION sectionHeaders = (xbox::PXBEIMAGE_SECTION)CxbxKrnl_Xbe->m_Header.dwSectionHeadersAddr;
		for (uint32_t i = 0; i < CxbxKrnl_Xbe->m_Header.dwSections; i++) {
			if ((sectionHeaders[i].Flags & XBEIMAGE_SECTION_PRELOAD) != 0) {
				NTSTATUS result = xbox::XeLoadSection(&sectionHeaders[i]);
				if (FAILED(result)) {
					EmuLogInit(LOG_LEVEL::WARNING, "Failed to preload XBE section: %s", CxbxKrnl_Xbe->m_szSectionName[i]);
				}
			}
		}

		// We need to remember a few XbeHeader fields, so we can switch between a valid ExeHeader and XbeHeader :
		StoreXbeImageHeader();

		// Restore enough of the executable image headers to keep WinAPI's working :
		RestoreExeImageHeader();

		// HACK: Attempt to patch out XBE header reads
		// This works by searching for the XBEH signature and replacing it with what appears in host address space instead
		// Test case: Half Life 2
		// Iterate through each CODE section
		for (uint32_t sectionIndex = 0; sectionIndex < CxbxKrnl_Xbe->m_Header.dwSections; sectionIndex++) {
			if (!CxbxKrnl_Xbe->m_SectionHeader[sectionIndex].dwFlags.bExecutable) {
				continue;
			}

			EmuLogInit(LOG_LEVEL::INFO, "Searching for XBEH in section %s", CxbxKrnl_Xbe->m_szSectionName[sectionIndex]);
			xbox::addr_xt startAddr = CxbxKrnl_Xbe->m_SectionHeader[sectionIndex].dwVirtualAddr;
			xbox::addr_xt endAddr = startAddr + CxbxKrnl_Xbe->m_SectionHeader[sectionIndex].dwSizeofRaw;
			for (xbox::addr_xt addr = startAddr; addr < endAddr; addr++) {
				if (*(uint32_t*)addr == 0x48454258) {
					EmuLogInit(LOG_LEVEL::INFO, "Patching XBEH at 0x%08X", addr);
					*((uint32_t*)addr) = *(uint32_t*)XBE_IMAGE_BASE;
				}
			}
		}
	}

	g_ExceptionManager = new ExceptionManager(); // If in need to add VEHs, move this line earlier. (just in case)

	// Launch the XBE :
	{
		// Load TLS
		Xbe::TLS* XbeTls = (Xbe::TLS*)CxbxKrnl_Xbe->m_Header.dwTLSAddr;
		void* XbeTlsData = (XbeTls != nullptr) ? (void*)CxbxKrnl_Xbe->m_TLS->dwDataStartAddr : nullptr;
		// Decode Entry Point
		xbox::addr_xt EntryPoint = CxbxKrnl_Xbe->m_Header.dwEntryAddr;
		EntryPoint ^= XOR_EP_KEY[to_underlying(CxbxKrnl_Xbe->GetXbeType())];
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

	if (!krnlLog) {
		(void)fclose(krnlLog);
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
		xbox::XBOX_KEY_DATA keys[2];
		fseek(fp, 0, SEEK_END);
		long size = ftell(fp);
		rewind(fp);

		// If the size of Keys.bin is correct (two keys), read it
		if (size == xbox::XBOX_KEY_LENGTH * 2) {
			fread(keys, xbox::XBOX_KEY_LENGTH, 2, fp);

			memcpy(xbox::XboxEEPROMKey, &keys[0], xbox::XBOX_KEY_LENGTH);
			memcpy(xbox::XboxCertificateKey, &keys[1], xbox::XBOX_KEY_LENGTH);
		}
		else {
			EmuLog(LOG_LEVEL::WARNING, "Keys.bin has an incorrect filesize. Should be %d bytes", xbox::XBOX_KEY_LENGTH * 2);
		}

		fclose(fp);
		return;
	}

	// If we didn't already exit the function, keys.bin could not be loaded
	EmuLog(LOG_LEVEL::WARNING, "Failed to load Keys.bin. Cxbx-Reloaded will be unable to read Save Data from a real Xbox");
}

//TODO: Possible move CxbxResolveHostToFullPath inline function someplace else if become useful elsewhere.
// Let filesystem library clean it up for us, including resolve host's symbolic link path.
// Since internal kernel do translate to full path than preserved host symoblic link path.
static inline void CxbxResolveHostToFullPath(std::filesystem::path& file_path, std::string_view finish_error_sentence) {
	std::error_code error;
	std::filesystem::path sanityPath = std::filesystem::canonical(file_path, error);
	if (error.value() != 0) {
		CxbxKrnlCleanupEx(LOG_PREFIX_INIT, "Could not resolve to %s: %s", finish_error_sentence.data(), file_path.string().c_str());
	}
	file_path = sanityPath;
}
// TODO: Eventually, we should remove this function to start using std::filesystem::path method for all host paths.
static inline void CxbxResolveHostToFullPath(std::string& file_path, std::string_view finish_error_sentence) {
	std::filesystem::path sanityPath(file_path);
	CxbxResolveHostToFullPath(sanityPath, finish_error_sentence);
	file_path = sanityPath.string();
}

__declspec(noreturn) void CxbxKrnlInit
(
	void                   *pTLSData,
	Xbe::TLS               *pTLS,
	Xbe::LibraryVersion    *pLibraryVersion,
	DebugMode               DbgMode,
	const char             *szDebugFilename,
	Xbe::Header            *pXbeHeader,
	uint32_t                dwXbeHeaderSize,
	void(*Entry)(),
	int BootFlags)
{
    // Set windows timer period to 1ms
    // Windows will automatically restore this value back to original on program exit
    // But with this, we can replace some busy loops with sleeps.
    timeBeginPeriod(1);

    xbox::InitializeFscCacheEvent();

	// update caches
	CxbxKrnl_TLS = pTLS;
	CxbxKrnl_TLSData = pTLSData;
	CxbxKrnl_XbeHeader = pXbeHeader;
	CxbxKrnl_DebugMode = DbgMode;
	CxbxKrnl_DebugFileName = (char*)szDebugFilename;

	// A patch to dwCertificateAddr is a requirement due to Windows TLS is overwriting dwGameRegion data address.
	// By using unalternated certificate data, it should no longer cause any problem with titles running and Cxbx's log as well.
	CxbxKrnl_XbeHeader->dwCertificateAddr = (uint32_t)&CxbxKrnl_Xbe->m_Certificate;
	g_pCertificate = &CxbxKrnl_Xbe->m_Certificate;

	// Initialize timer subsystem
	Timer_Init();
	// for unicode conversions
	setlocale(LC_ALL, "English");
	// Initialize time-related variables for the kernel and the timers
	CxbxInitPerformanceCounters();
#ifdef _DEBUG
//	PopupCustom(LOG_LEVEL::INFO, "Attach a Debugger");
//  Debug child processes using https://marketplace.visualstudio.com/items?itemName=GreggMiskelly.MicrosoftChildProcessDebuggingPowerTool
#endif

	// debug trace
	{
#ifdef _DEBUG_TRACE
		EmuLogInit(LOG_LEVEL::INFO, "Debug Trace Enabled.");
		EmuLogInit(LOG_LEVEL::INFO, "CxbxKrnlInit\n"
			"(\n"
			"   hwndParent          : 0x%.08p\n"
			"   pTLSData            : 0x%.08p\n"
			"   pTLS                : 0x%.08p\n"
			"   pLibraryVersion     : 0x%.08p\n"
			"   DebugConsole        : 0x%.08X\n"
			"   DebugFilename       : \"%s\"\n"
			"   pXBEHeader          : 0x%.08p\n"
			"   dwXBEHeaderSize     : 0x%.08X\n"
			"   Entry               : 0x%.08p\n"
			");",
			CxbxKrnl_hEmuParent, pTLSData, pTLS, pLibraryVersion, DbgMode, szDebugFilename, pXbeHeader, dwXbeHeaderSize, Entry);
#else
		EmuLogInit(LOG_LEVEL::INFO, "Debug Trace Disabled.");
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
		EmuLogInit(LOG_LEVEL::INFO, "Initialized dummy kernel image header.");
	}

	// Read which components need to be LLE'ed per user request
	{
		unsigned int CxbxLLE_Flags;
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
		g_EmuShared->GetUseAllCores(&HackEnabled);
		g_UseAllCores = !!HackEnabled;
		g_EmuShared->GetSkipRdtscPatching(&HackEnabled);
		g_SkipRdtscPatching = !!HackEnabled;
	}

#ifdef _DEBUG_PRINT_CURRENT_CONF
	PrintCurrentConfigurationLog();
#endif
	
	// Initialize devices :
	{
		char szBuffer[sizeof(szFilePath_Xbe)];
		g_EmuShared->GetStorageLocation(szBuffer);

		CxbxBasePath = std::string(szBuffer) + "\\EmuDisk";
		CxbxResolveHostToFullPath(CxbxBasePath, "Cxbx-Reloaded's EmuDisk directory");
		// Since canonical always remove the extra slash, we need to manually add it back.
		// TODO: Once CxbxBasePath is filesystem::path, replace CxbxBasePath's + operators to / for include path separator internally.
		CxbxBasePath = std::filesystem::path(CxbxBasePath).append("").string();
	}

	// Determine xbe path
	std::filesystem::path xbePath;
	{
		std::string szBuffer(szFilePath_Xbe);
		std::replace(szBuffer.begin(), szBuffer.end(), ';', '/');
		xbePath = szBuffer;
	}
	CxbxResolveHostToFullPath(xbePath, "xbe's file");

	// Determine location for where possible auto mount D letter if ";" delimiter exist.
	// Also used to store in EmuShared's title mount path permanent storage on first emulation launch.
	// Unless it's launch within Cxbx-Reloaded's EmuDisk directly, then we don't store anything in title mount path storage.
	std::string relative_path(szFilePath_Xbe);
	size_t lastFind = relative_path.find(';');
	// First find if there is a semicolon when dashboard or title disc (such as demo disc) has it.
	// Then we must obey the current directory it asked for.
	if (lastFind != std::string::npos) {
		if (relative_path.find(';', lastFind + 1) != std::string::npos) {
			CxbxKrnlCleanupEx(LOG_PREFIX_INIT, "Cannot contain multiple of ; symbol.");
		}
		relative_path = relative_path.substr(0, lastFind);
	}
	else {
		relative_path = relative_path.substr(0, relative_path.find_last_of("\\/"));
	}
	CxbxResolveHostToFullPath(relative_path, "xbe's directory");

	CxbxBasePathHandle = CreateFile(CxbxBasePath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	int CxbxCdrom0DeviceIndex = -1;
	bool isEmuDisk = _strnicmp(relative_path.c_str(), CxbxBasePath.c_str(), CxbxBasePath.size() - 1) == 0;
	// Check if title mounth path is already set. This may occur from early boot of Chihiro title.
	char title_mount_path[sizeof(szFilePath_Xbe)];
	const char* p_default_mount_path = title_mount_path;
	g_EmuShared->GetTitleMountPath(title_mount_path);

	if (p_default_mount_path[0] == '\0' && BootFlags == BOOT_NONE) {
		// Remember our first initialize mount path for CdRom0 and Mbfs.
		if (!isEmuDisk) {
			g_EmuShared->SetTitleMountPath(relative_path.c_str());
			p_default_mount_path = relative_path.c_str();
		}
	}

	// TODO: Find a place to make permanent placement for DeviceCdrom0 that does not have disc loaded.
	if (p_default_mount_path[0] != '\0') {
		CxbxCdrom0DeviceIndex = CxbxRegisterDeviceHostPath(DeviceCdrom0, p_default_mount_path);
		// Since Chihiro also map Mbfs to the same path as Cdrom0, we'll map it the same way.
		if (g_bIsChihiro) {
			(void)CxbxRegisterDeviceHostPath(DriveMbfs, p_default_mount_path);
		}
	}

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
	EmuLogInit(LOG_LEVEL::DEBUG, "Creating default symbolic links.");
	{
		// TODO: DriveD should auto mount based on the launchdata page's ; delimiter in the xbe path.
		// This is the only symbolic link the Xbox Kernel sets, the rest are set by the application, usually via XAPI.
		// If the Xbe is located outside of the emulated HDD, mounting it as DeviceCdrom0 is correct
		// If the Xbe is located inside the emulated HDD, the full path should be used, eg: "\\Harddisk0\\partition2\\xboxdash.xbe"
#ifdef CXBX_KERNEL_REWORK_ENABLED
		if (lastFind != std::string::npos) {
#else
		// HACK: It is a hack to override XDK's default mount to CdRom0 which may not exist when launch to dashboard directly.
		// Otherwise, titles may launch to dashboard, more specifically xbox live title, and back.
		if (CxbxCdrom0DeviceIndex == -1 || lastFind != std::string::npos) {
#endif
			CxbxCreateSymbolicLink(DriveD, relative_path);
		}
	}

	// Determine Xbox path to XBE and place it in XeImageFileName
	{
		std::string fileName;
		if (xbox::LaunchDataPage == xbox::zeroptr) {
			// First launch and possible launch to dashboard
			if (isEmuDisk) {
				XboxDevice* xbeLoc = CxbxDeviceByHostPath(xbePath.string());
				fileName = xbeLoc->XboxDevicePath;
			}
			// Otherwise it might be from CdRom0 device.
			else {
				fileName = DeviceCdrom0;
			}

			// Strip out the path, leaving only the XBE file name to append.
			if (xbePath.has_filename()) {
				fileName += "\\" + xbePath.filename().string();
			}
		}
		else {
			// One way to say launch to dashboard. We already load the xbe and check it's xbe type.
			if (xbox::LaunchDataPage->Header.szLaunchPath[0] == '\0') {
				fileName = DeviceHarddisk0Partition2;

				// Strip out the path, leaving only the XBE file name to append.
				if (xbePath.has_filename()) {
					fileName += "\\" + xbePath.filename().string();
				}
			}
			// Otherwise, preserve the launch path and replace delimiter.
			else {
				fileName = xbox::LaunchDataPage->Header.szLaunchPath;
				std::replace(fileName.begin(), fileName.end(), ';', '\\');
			}
		}

		if (xbox::XeImageFileName.Buffer != xbox::zeroptr) {
			xbox::ExFreePool(xbox::XeImageFileName.Buffer);
		}

		// Assign the running Xbe path, so it can be accessed via the kernel thunk 'XeImageFileName' :
		xbox::XeImageFileName.Length = static_cast<xbox::ushort_xt>(fileName.size());
		xbox::XeImageFileName.MaximumLength = xbox::XeImageFileName.Length + 1;
		xbox::XeImageFileName.Buffer = (PCHAR)xbox::ExAllocatePoolWithTag(xbox::XeImageFileName.MaximumLength, 'nFeX');
		strncpy_s(xbox::XeImageFileName.Buffer, xbox::XeImageFileName.MaximumLength,fileName.c_str(), fileName.size());
		EmuLogInit(LOG_LEVEL::INFO, "XeImageFileName = %s", xbox::XeImageFileName.Buffer);
	}

	// Dump Xbe information
	{
		if (CxbxKrnl_Xbe != nullptr) {
			EmuLogInit(LOG_LEVEL::INFO, "Title : %s", CxbxKrnl_Xbe->m_szAsciiTitle);
		}

		// Dump Xbe certificate
		if (g_pCertificate != NULL) {
			std::stringstream titleIdHex;
			titleIdHex << std::hex << g_pCertificate->dwTitleId;

			EmuLogInit(LOG_LEVEL::INFO, "XBE TitleID : %s", FormatTitleId(g_pCertificate->dwTitleId).c_str());
			EmuLogInit(LOG_LEVEL::INFO, "XBE TitleID (Hex) : 0x%s", titleIdHex.str().c_str());
			EmuLogInit(LOG_LEVEL::INFO, "XBE Version : 1.%02d", g_pCertificate->dwVersion);
			EmuLogInit(LOG_LEVEL::INFO, "XBE TitleName : %.40ls", g_pCertificate->wsTitleName);
			EmuLogInit(LOG_LEVEL::INFO, "XBE Region : %s", CxbxKrnl_Xbe->GameRegionToString());
		}

		// Dump Xbe library build numbers
		Xbe::LibraryVersion* libVersionInfo = pLibraryVersion;// (LibraryVersion *)(CxbxKrnl_XbeHeader->dwLibraryVersionsAddr);
		if (libVersionInfo != NULL) {
			for (uint32_t v = 0; v < CxbxKrnl_XbeHeader->dwLibraryVersions; v++) {
				EmuLogInit(LOG_LEVEL::INFO, "XBE Library %u : %.8s (version %d)", v, libVersionInfo->szName, libVersionInfo->wBuildVersion);
				libVersionInfo++;
			}
		}
	}

	CxbxKrnlRegisterThread(GetCurrentThread());

	// Make sure the Xbox1 code runs on one core (as the box itself has only 1 CPU,
	// this will better aproximate the environment with regard to multi-threading) :
	EmuLogInit(LOG_LEVEL::DEBUG, "Determining CPU affinity.");
	g_AffinityPolicy = AffinityPolicy::InitPolicy();

	// initialize graphics
	EmuLogInit(LOG_LEVEL::DEBUG, "Initializing render window.");
	CxbxInitWindow(true);

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

	CxbxInitAudio();

	// EmuHLEIntercept must be call before MapThunkTable, otherwise scanning for symbols will not work properly.
	EmuHLEIntercept(pXbeHeader);

	// Decode kernel thunk table address :
	uint32_t kt = CxbxKrnl_Xbe->m_Header.dwKernelImageThunkAddr;
	kt ^= XOR_KT_KEY[to_underlying(CxbxKrnl_Xbe->GetXbeType())];

	// Process the Kernel thunk table to map Kernel function calls to their actual address :
	MapThunkTable((uint32_t *)kt, CxbxKrnl_KernelThunkTable);

	// Does this xbe import any other libraries?
	if (CxbxKrnl_Xbe->m_Header.dwNonKernelImportDirAddr) {
		ImportLibraries((XbeImportEntry *)CxbxKrnl_Xbe->m_Header.dwNonKernelImportDirAddr);
	}

	if (!bLLE_USB) {
		SetupXboxDeviceTypes();
	}

	InitXboxHardware(HardwareModel::Revision1_5); // TODO : Make configurable

	// Read Xbox video mode from the SMC, store it in HalBootSMCVideoMode
	xbox::HalReadSMBusValue(SMBUS_ADDRESS_SYSTEM_MICRO_CONTROLLER, SMC_COMMAND_AV_PACK, FALSE, (xbox::PULONG)&xbox::HalBootSMCVideoMode);

	g_InputDeviceManager.Initialize(false, g_hEmuWindow);

	// Now the hardware devices exist, couple the EEPROM buffer to it's device
	g_EEPROM->SetEEPROM((uint8_t*)EEPROM);

	if (!bLLE_GPU)
	{
		EmuLogInit(LOG_LEVEL::DEBUG, "Initializing Direct3D.");
		EmuD3DInit();
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

	InitXboxThread();
	g_AffinityPolicy->SetAffinityXbox();
	if (!xbox::ObInitSystem()) {
		// TODO: Replace EmuLogEx to CxbxKrnlCleanupEx when ObInitSystem's calls are properly implement.
		EmuLogEx(LOG_PREFIX_INIT, LOG_LEVEL::WARNING, "Unable to intialize xbox::ObInitSystem.");
	}
	xbox::KiInitSystem();

	EmuX86_Init();
	// Create the interrupt processing thread
	DWORD dwThreadId;
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, NULL, CxbxKrnlInterruptThread, NULL, NULL, (unsigned int*)&dwThreadId);
	// Start the kernel clock thread
	TimerObject* KernelClockThr = Timer_Create(CxbxKrnlClockThread, nullptr, "Kernel clock thread", false);
	Timer_Start(KernelClockThr, SCALE_MS_IN_NS);

	EmuLogInit(LOG_LEVEL::DEBUG, "Calling XBE entry point...");
	CxbxLaunchXbe(Entry);

	// FIXME: Wait for Cxbx to exit or error fatally
	Sleep(INFINITE);

	EmuLogInit(LOG_LEVEL::DEBUG, "XBE entry point returned");
	fflush(stdout);

	CxbxUnlockFilePath();

	//	EmuShared::Cleanup();   FIXME: commenting this line is a bad workaround for issue #617 (https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/617)
    CxbxKrnlTerminateThread();
}

void CxbxInitFilePaths()
{
	if (g_Settings) {
		std::string dataLoc = g_Settings->GetDataLocation();
		std::strncpy(szFolder_CxbxReloadedData, dataLoc.c_str(), dataLoc.length() + 1);
	}
	else {
		g_EmuShared->GetStorageLocation(szFolder_CxbxReloadedData);
	}

	// Make sure our data folder exists :
	bool result = std::filesystem::exists(szFolder_CxbxReloadedData);
	if (!result && !std::filesystem::create_directory(szFolder_CxbxReloadedData)) {
		CxbxKrnlCleanup("%s : Couldn't create Cxbx-Reloaded's data folder!", __func__);
	}

	// Make sure the EmuDisk folder exists
	std::string emuDisk = std::string(szFolder_CxbxReloadedData) + std::string("\\EmuDisk");
	result = std::filesystem::exists(emuDisk);
	if (!result && !std::filesystem::create_directory(emuDisk)) {
		CxbxKrnlCleanup("%s : Couldn't create Cxbx-Reloaded EmuDisk folder!", __func__);
	}

	snprintf(szFilePath_EEPROM_bin, MAX_PATH, "%s\\EEPROM.bin", szFolder_CxbxReloadedData);

	GetModuleFileName(GetModuleHandle(nullptr), szFilePath_CxbxReloaded_Exe, MAX_PATH);
}

HANDLE hMapDataHash = nullptr;

bool CxbxLockFilePath()
{
    std::stringstream filePathHash("Local\\");
    uint64_t hashValue = XXH3_64bits(szFolder_CxbxReloadedData, strlen(szFolder_CxbxReloadedData) + 1);
    if (!hashValue) {
        CxbxKrnlCleanup("%s : Couldn't generate Cxbx-Reloaded's data folder hash!", __func__);
    }

    filePathHash << std::hex << hashValue;

    hMapDataHash = CreateFileMapping
    (
        INVALID_HANDLE_VALUE,       // Paging file
        nullptr,                    // default security attributes
        PAGE_READONLY,              // readonly access
        0,                          // size: high 32 bits
        /*Dummy size*/4,            // size: low 32 bits
        filePathHash.str().c_str()  // name of map object
    );

    if (hMapDataHash == nullptr) {
        return false;
    }

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        PopupError(nullptr, "Data path directory is currently in use.\nUse a different data path directory or stop emulation from another process.");
        CloseHandle(hMapDataHash);
        return false;
    }

    return true;
}

void CxbxUnlockFilePath()
{
    // Close opened file path lockdown shared memory.
    if (hMapDataHash) {
        CloseHandle(hMapDataHash);
        hMapDataHash = nullptr;
    }
}

// REMARK: the following is useless, but PatrickvL has asked to keep it for documentation purposes
/*xbox::LAUNCH_DATA_PAGE DefaultLaunchDataPage =
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

		(void)PopupCustomEx(nullptr, cxbxr_module, LOG_LEVEL::FATAL, PopupIcon::Error, PopupButtons::Ok, PopupReturn::Ok, "Received Fatal Message:\n\n* %s\n", szBuffer2); // Will also EmuLogEx
    }

	EmuLogInit(LOG_LEVEL::INFO, "MAIN: Terminating Process");
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

    g_bEmuSuspended = true;
}

void CxbxKrnlResume()
{
    if(!g_bEmuSuspended)
        return;

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

void CxbxKrnlShutDown(bool is_reboot)
{
	if (!is_reboot) {
		// Clear all kernel boot flags. These (together with the shared memory) persist until Cxbx-Reloaded is closed otherwise.
		int BootFlags = 0;
		g_EmuShared->SetBootFlags(&BootFlags);
	}

	// NOTE: This causes a hang when exiting while NV2A is processing
	// This is okay for now: It won't leak memory or resources since TerminateProcess will free everything
	// delete g_NV2A; // TODO : g_pXbox

	// Shutdown the input device manager
	g_InputDeviceManager.Shutdown();

	// Shutdown the memory manager
	g_VMManager.Shutdown();

	// Shutdown the render manager
	if (g_renderbase != nullptr) {
		g_renderbase->Shutdown();
		g_renderbase = nullptr;
	}

	CxbxUnlockFilePath();

	if (CxbxKrnl_hEmuParent != NULL && !is_reboot) {
		SendMessage(CxbxKrnl_hEmuParent, WM_PARENTNOTIFY, WM_DESTROY, 0);
	}

	EmuShared::Cleanup();

	if (g_ExceptionManager) {
		delete g_ExceptionManager;
		g_ExceptionManager = nullptr;
	}

	TerminateProcess(g_CurrentProcessHandle, 0);
}

void CxbxKrnlPrintUEM(ULONG ErrorCode)
{
	xbox::dword_xt Type;
	xbox::XBOX_EEPROM Eeprom;
	ULONG ResultSize;

	xbox::ntstatus_xt status = xbox::ExQueryNonVolatileSetting(xbox::XC_MAX_ALL, &Type, &Eeprom, sizeof(Eeprom), (xbox::PSIZE_T)&ResultSize);

	if (status == xbox::status_success)
	{
		xbox::XBOX_UEM_INFO* UEMInfo = (xbox::XBOX_UEM_INFO*)&(Eeprom.UEMInfo[0]);

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
		xbox::ExSaveNonVolatileSetting(xbox::XC_MAX_ALL, Type, &Eeprom, sizeof(Eeprom));
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
		std::string ErrorMessage = "Fatal error. " + it->second + ". This error screen will persist indefinitely. Stop the emulation to close it.";
		PopupFatal(nullptr, ErrorMessage.c_str());
	}
	else
	{
		PopupFatal(nullptr, "Unknown fatal error. This error screen will persist indefinitely. Stop the emulation to close it.");
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
