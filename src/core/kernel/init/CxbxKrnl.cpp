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
#include "common\xbdm\CxbxXbdm.h" // For Cxbx_LibXbdmThunkTable
#include "core/kernel/support/PatchRdtsc.hpp"
#include "devices\x86\EmuX86.h" // For EmuX86_Init
#include "core\kernel\support\EmuFile.h"
#include "core\kernel\support\EmuFS.h" // EmuInitFS
#include "EmuEEPROM.h" // For CxbxRestoreEEPROM, EEPROM
#include "core\kernel\exports\EmuKrnl.h"
#include "core\kernel\exports\EmuKrnlKi.h"
#include "core\kernel\exports\EmuKrnlKe.h"
#include "core\kernel\exports\EmuKrnlPs.hpp"
#include "EmuShared.h"
#include "core\hle\D3D8\Direct3D9\Direct3D9.h" // For CxbxInitWindow, EmuD3DInit
#include "core\hle\DSOUND\DirectSound\DirectSound.hpp" // For CxbxInitAudio
#include "core\hle\JVS\JVS.h" // For JVS_Init
#include "core\hle\Intercept.hpp"
#include "core\kernel\memory-manager\VMManager.h"
#include "CxbxDebugger.h"
#include "common/util/cliConfig.hpp"
#include "xxhash.h"
#include "common/ReserveAddressRanges.h"
#include "common/xbox/Types.hpp"
#include "common/win32/WineEnv.h"

#include <clocale>
#include <process.h>
#include <sstream> // For std::ostringstream

#include "devices\EEPROMDevice.h" // For g_EEPROM
#include "devices\Xbox.h" // For InitXboxHardware()
#include "devices\LED.h" // For LED::Sequence
#include "common\crypto\EmuSha.h" // For the SHA1 functions
#include "common/Timer.h" // For Timer_Init
#include "common\input\InputManager.h" // For the InputDeviceManager
#include "core/kernel/support/NativeHandle.h"
#include "common/win32/Util.h" // for WinError2Str

#include "common/FilePaths.hpp"

// Intended only for CxbxrKrnl.cpp usage.
#include "KrnlPatches.hpp"

/*! thread local storage */
Xbe::TLS *CxbxKrnl_TLS = NULL;
/*! thread local storage data */
void *CxbxKrnl_TLSData = NULL;
/*! xbe header structure */
Xbe::Header *CxbxKrnl_XbeHeader = NULL;
/*! parent window handle */

/*! indicates a debug kernel */
bool g_bIsDebugKernel = false;

DebugMode CxbxrKrnl_DebugMode = DebugMode::DM_NONE;
std::string CxbxrKrnl_DebugFileName = "";
Xbe::Certificate *g_pCertificate = NULL;

// Hybrid host/xbox path to the xbe.
// A semicolon in the path indicates the Xbox mount point.
// Replacing the ';' with '\' gives a regular host path.
// NOTE: LAUNCH_DATA_HEADER's szLaunchPath is xbox::max_path*2 = 520
char szFilePath_Xbe[xbox::max_path*2] = { 0 };

Xbe* CxbxKrnl_Xbe = NULL;
bool g_bIsChihiro = false;
bool g_bIsDevKit = false;
bool g_bIsRetail = false;

// Set by the VMManager during initialization. Exported because it's needed in other parts of the emu
size_t g_SystemMaxMemory = 0;

HANDLE g_CurrentProcessHandle = 0; // Set in CxbxKrnlEmulate

bool g_CxbxPrintUEM = false;
ULONG g_CxbxFatalErrorCode = FATAL_ERROR_NONE;

// Define function located in EmuXApi so we can call it from here
void SetupXboxDeviceTypes();

extern xbox::void_xt NTAPI system_events(xbox::PVOID arg);

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

xbox::void_xt NTAPI CxbxLaunchXbe(xbox::PVOID Entry)
{
	EmuLogInit(LOG_LEVEL::DEBUG, "Calling XBE entry point...");
	static_cast<void(*)()>(Entry)();
	EmuLogInit(LOG_LEVEL::DEBUG, "XBE entry point returned");
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

FILE* CxbxrKrnlSetupVerboseLog(int BootFlags)
{
	std::string tempStr;
	// Get KernelDebugMode :
	CxbxrKrnl_DebugMode = DebugMode::DM_NONE;
	if (cli_config::GetValue(cli_config::debug_mode, &tempStr)) {
		CxbxrKrnl_DebugMode = (DebugMode)std::atoi(tempStr.c_str());
	}

	// Get KernelDebugFileName :
	CxbxrKrnl_DebugFileName = "";
	if (!cli_config::GetValue(cli_config::debug_file, &CxbxrKrnl_DebugFileName)) {
		CxbxrKrnl_DebugFileName = "";
	}

	// debug console allocation (if configured)
	if (CxbxrKrnl_DebugMode == DM_CONSOLE)
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
		if (CxbxrKrnl_DebugMode == DM_FILE) {
			// Peform clean write to kernel log for first boot. Unless multi-xbe boot occur then perform append to existing log.
			FILE* krnlLog = freopen(CxbxrKrnl_DebugFileName.c_str(), ((BootFlags == DebugMode::DM_NONE) ? "wt" : "at"), stdout);
			// Append separator for better readability after reboot.
			if (BootFlags != DebugMode::DM_NONE) {
				std::cout << "\n------REBOOT------REBOOT------REBOOT------REBOOT------REBOOT------\n" << std::endl;
			}
			return krnlLog;
		}
		else {
			char buffer[16];
			if (GetConsoleTitle(buffer, 16) != NULL)
				(void)freopen("nul", "w", stdout);
		}
	}
	return nullptr;
}

static void CxbxrKrnlSyncGUI()
{
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
				CxbxrShutDown();
			}
			break;
		} while (true);
	}
}

static void CxbxrKrnlSetupMemorySystem(int BootFlags, unsigned emulate_system, unsigned reserved_systems, blocks_reserved_t blocks_reserved)
{
	// Release unnecessary memory ranges to allow console/host to use those memory ranges.
	FreeAddressRanges(emulate_system, reserved_systems, blocks_reserved);
	// Initialize the memory manager
	g_VMManager.Initialize(emulate_system, BootFlags, blocks_reserved);

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
}

static bool CxbxrKrnlXbeSystemSelector(int BootFlags,
                                       unsigned& reserved_systems,
                                       blocks_reserved_t blocks_reserved,
                                       HardwareModel &hardwareModel)
{
	unsigned int emulate_system = 0;
	// Get title path :
	std::string xbePath;
	cli_config::GetValue(cli_config::load, &xbePath);
	xbePath = std::filesystem::absolute(xbePath).string();

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

	// Once clean up process is done, proceed set to global variable string.
	strncpy(szFilePath_Xbe, xbePath.c_str(), xbox::max_path - 1);
	// Convert to host path by replacing special mount point character if present
	std::replace(xbePath.begin(), xbePath.end(), ';', '\\');
	// Load Xbe (this one will reside above WinMain's virtual_memory_placeholder)
	std::filesystem::path xbeDirectory = std::filesystem::path(xbePath).parent_path();

	CxbxKrnl_Xbe = new Xbe(xbePath.c_str()); // TODO : Instead of using the Xbe class, port Dxbx _ReadXbeBlock()

	if (CxbxKrnl_Xbe->HasFatalError()) {
		CxbxrAbort(CxbxKrnl_Xbe->GetError().c_str());
		return false;
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
		emulate_system = SYSTEM_CHIHIRO;
	}
	else if (cli_config::hasKey(cli_config::system_devkit)) {
		EmuLogInit(LOG_LEVEL::INFO, "Auto detect is disabled, running as devkit.");
		emulate_system = SYSTEM_DEVKIT;
	}
	else if (cli_config::hasKey(cli_config::system_retail)) {
		EmuLogInit(LOG_LEVEL::INFO, "Auto detect is disabled, running as retail.");
		emulate_system = SYSTEM_XBOX;
	}
	// Otherwise, use auto detect method.
	else {
		// Detect XBE type :
		XbeType xbeType = CxbxKrnl_Xbe->GetXbeType();
		EmuLogInit(LOG_LEVEL::INFO, "Auto detect: XbeType = %s", GetXbeTypeToStr(xbeType));

		// Convert XBE type into corresponding system to emulate.
		switch (xbeType) {
			case XbeType::xtChihiro:
				emulate_system = SYSTEM_CHIHIRO;
				break;
			case XbeType::xtDebug:
				emulate_system = SYSTEM_DEVKIT;
				break;
			case XbeType::xtRetail:
				emulate_system = SYSTEM_XBOX;
				break;
			DEFAULT_UNREACHABLE;
		}

		// If the XBE path contains a boot.id, it must be a Chihiro title
		// This is necessary as some Chihiro games use the Debug xor instead of the Chihiro ones
		// which means we cannot rely on that alone.
		if (std::filesystem::exists(xbeDirectory / "boot.id")) {
			emulate_system = SYSTEM_CHIHIRO;
		}
	}

	EmuLogInit(LOG_LEVEL::INFO, "Host's compatible system types: %2X", reserved_systems);
	// If the system to emulate isn't supported on host, enforce failure.
	if (!isSystemFlagSupport(reserved_systems, emulate_system)) {
		CxbxrAbort("Unable to emulate system type due to host is not able to reserve required memory ranges.");
		return false;
	}
	// Clear emulation system from reserved systems so all unneeded memory ranges can be freed.
	reserved_systems &= ~emulate_system;

	// If the Xbe is Chihiro, and we were not launched by SEGABOOT, we need to load SEGABOOT from the Chihiro Media Board rom instead!
	// All checks are absolutely necessary in order to support both "auto select" and forced chihiro mode.
	if (BootFlags == BOOT_NONE &&
		emulate_system == SYSTEM_CHIHIRO &&
		std::filesystem::exists(xbeDirectory / "boot.id")) {

		std::string chihiroMediaBoardRom = g_DataFilePath + "/EmuDisk/" + MediaBoardRomFile;
		if (!std::filesystem::exists(chihiroMediaBoardRom)) {
			CxbxrAbort("Chihiro Media Board ROM (fpr21042_m29w160et.bin) could not be found (should be placed in /EmuDisk/Chihiro/)");
		}

		// Open a handle to the mediaboard rom
		FILE* fpRom = fopen(chihiroMediaBoardRom.c_str(), "rb");
		if (fpRom == nullptr) {
			CxbxrAbort("Chihiro Media Board ROM (fpr21042_m29w160et.bin) could not be opened for read");
		}

		// Verify the size of media board rom
		fseek(fpRom, 0, SEEK_END);
		auto length = ftell(fpRom);
		if (length != 2 * ONE_MB) {
			CxbxrAbort("Chihiro Media Board ROM (fpr21042_m29w160et.bin) has an invalid size");

		}
		fseek(fpRom, 0, SEEK_SET);

		// Extract SEGABOOT_OLD.XBE and SEGABOOT.XBE from Media Rom
		// We only do this if SEGABOOT_OLD and SEGABOOT.XBE are *not* already present
		std::string chihiroSegaBootOld = g_DataFilePath + "/EmuDisk/" + MediaBoardSegaBoot0;
		std::string chihiroSegaBootNew = g_DataFilePath + "/EmuDisk/" + MediaBoardSegaBoot1;
		if (!std::filesystem::exists(chihiroSegaBootOld) || !std::filesystem::exists(chihiroSegaBootNew)) {
			FILE* fpSegaBootOld = fopen(chihiroSegaBootOld.c_str(), "wb");
			FILE* fpSegaBootNew = fopen(chihiroSegaBootNew.c_str(), "wb");
			if (fpSegaBootNew == nullptr || fpSegaBootOld == nullptr) {
				CxbxrAbort("Could not open SEGABOOT for writing");

			}

			// Extract SEGABOOT (Old)
			void* buffer = malloc(ONE_MB);
			if (buffer == nullptr) {
				CxbxrAbort("Could not allocate buffer for SEGABOOT");

			}

			fread(buffer, 1, ONE_MB, fpRom);
			fwrite(buffer, 1, ONE_MB, fpSegaBootOld);

			// Extract SEGABOOT (New)
			fread(buffer, 1, ONE_MB, fpRom);
			fwrite(buffer, 1, ONE_MB, fpSegaBootNew);

			free(buffer);

			fclose(fpSegaBootOld);
			fclose(fpSegaBootNew);
			fclose(fpRom);

		}

		g_EmuShared->SetTitleMountPath(xbeDirectory.string().c_str());

		// Launch Segaboot
		CxbxLaunchNewXbe(chihiroSegaBootNew);
		CxbxrShutDown(true);
	}

	// Once we have determine which system type to run as, enforce it in future reboots.
	if ((BootFlags & BOOT_QUICK_REBOOT) == 0) {
		const char* system_str = GetSystemTypeToStr(emulate_system);
		cli_config::SetSystemType(system_str);
	}

	// Register if we're running a Chihiro arcade or Xbox Devkit (otherwise it's a retail Xbox console) system
	g_bIsChihiro = (emulate_system == SYSTEM_CHIHIRO);
	g_bIsDevKit = (emulate_system == SYSTEM_DEVKIT);
	g_bIsRetail = (emulate_system == SYSTEM_XBOX);
	if (g_bIsChihiro) {
		hardwareModel = HardwareModel::Chihiro_Type1; // TODO: Make configurable to support Type-3 console.
	}
	else if (g_bIsDevKit) {
		hardwareModel = HardwareModel::DebugKit_r1_2; // Unlikely need to make configurable.
	}
	// Retail (default)
	else {
		// Should not be configurable. Otherwise, titles compiled with newer XDK will patch older xbox kernel.
		hardwareModel = HardwareModel::Revision1_6;
	}

	// If this is a Chihiro title, we need to patch the init flags to disable HDD setup
	// The Chihiro kernel does this, so we should too!
	if (g_bIsChihiro) {
		CxbxKrnl_Xbe->m_Header.dwInitFlags.bDontSetupHarddisk = true;
	}

	CxbxrKrnlSetupMemorySystem(BootFlags, emulate_system, reserved_systems, blocks_reserved);
	return true;
}

// HACK: Attempt to patch out XBE header reads
static void CxbxrKrnlXbePatchXBEHSig() {
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

static size_t CxbxrKrnlGetRelativePath(std::filesystem::path& get_relative_path) {
	// Determine location for where possible auto mount D letter if ";" delimiter exist.
	// Also used to store in EmuShared's title mount path permanent storage on first emulation launch.
	// Unless it's launch within Cxbx-Reloaded's EmuDisk directly, then we don't store anything in title mount path storage.
	std::string relative_path(szFilePath_Xbe);
	size_t lastFind = relative_path.find(';');
	// First find if there is a semicolon when dashboard or title disc (such as demo disc) has it.
	// Then we must obey the current directory it asked for.
	if (lastFind != std::string::npos) {
		if (relative_path.find(';', lastFind + 1) != std::string::npos) {
			CxbxrAbortEx(LOG_PREFIX_INIT, "Cannot contain multiple of ; symbol.");
		}
		relative_path = relative_path.substr(0, lastFind);
	}
	else {
		relative_path = relative_path.substr(0, relative_path.find_last_of("\\/"));
	}
	CxbxResolveHostToFullPath(relative_path, "xbe's directory");

	get_relative_path = relative_path;
	return lastFind;
}

static void CxbxrKrnlSetupSymLinks(int CxbxCdrom0DeviceIndex, int lastFind, std::filesystem::path& relative_path)
{
	// Create default symbolic links :
	EmuLogInit(LOG_LEVEL::DEBUG, "Creating default symbolic links.");
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
		CxbxCreateSymbolicLink(DriveD, relative_path.string());
	}
}

static void CxbxrKrnlSetupCdRom0Path(int BootFlags, int lastFind, std::filesystem::path relative_path, bool isEmuDisk)
{
	int CxbxCdrom0DeviceIndex = -1;
	// Check if title mounth path is already set. This may occur from early boot of Chihiro title.
	char title_mount_path[sizeof(szFilePath_Xbe)];
	g_EmuShared->GetTitleMountPath(title_mount_path);
	std::filesystem::path default_mount_path = title_mount_path;

	if (default_mount_path.native()[0] == 0 && BootFlags == BOOT_NONE) {
		// Remember our first initialize mount path for CdRom0 and Mbfs.
		if (!isEmuDisk) {
			g_EmuShared->SetTitleMountPath(relative_path.string().c_str());
			default_mount_path = relative_path.c_str();
		}
	}

	// TODO: Find a place to make permanent placement for DeviceCdrom0 that does not have disc loaded.
	if (default_mount_path.native()[0] != 0) {
		// NOTE: Don't need to perform CxbxResolveHostToFullPath again for default_mount_path.
		CxbxCdrom0DeviceIndex = CxbxRegisterDeviceHostPath(DeviceCdrom0, default_mount_path.string());
		// Since Chihiro also map Mbfs to the same path as Cdrom0, we'll map it the same way.
		if (g_bIsChihiro) {
			(void)CxbxRegisterDeviceHostPath(DriveMbfs, default_mount_path.string());
		}
	}

	CxbxrKrnlSetupSymLinks(CxbxCdrom0DeviceIndex, lastFind, relative_path);
}

static void CxbxrKrnlRegisterDevicePaths()
{
	// Partition 0 contains configuration data, and is accessed as a native file, instead as a folder :
	CxbxRegisterDeviceHostPath(DeviceHarddisk0Partition0, g_DiskBasePath + "Partition0", /*IsFile=*/true);
	// The first two partitions are for Data and Shell files, respectively :
	CxbxRegisterDeviceHostPath(DeviceHarddisk0Partition1, g_DiskBasePath + "Partition1");
	CxbxRegisterDeviceHostPath(DeviceHarddisk0Partition2, g_DiskBasePath + "Partition2");
	// The following partitions are for caching purposes - for now we allocate up to 7 (as xbmp needs that many) :
	CxbxRegisterDeviceHostPath(DeviceHarddisk0Partition3, g_DiskBasePath + "Partition3");
	CxbxRegisterDeviceHostPath(DeviceHarddisk0Partition4, g_DiskBasePath + "Partition4");
	CxbxRegisterDeviceHostPath(DeviceHarddisk0Partition5, g_DiskBasePath + "Partition5");
	CxbxRegisterDeviceHostPath(DeviceHarddisk0Partition6, g_DiskBasePath + "Partition6");
	CxbxRegisterDeviceHostPath(DeviceHarddisk0Partition7, g_DiskBasePath + "Partition7");
	CxbxRegisterDeviceHostPath(DevicePrefix + "\\Chihiro", g_DiskBasePath + "Chihiro");

	// Create the MU directories and the bin files
	CxbxRegisterDeviceHostPath(DeviceMU0, g_MuBasePath + "F", false, sizeof(FATX_SUPERBLOCK));
	CxbxRegisterDeviceHostPath(DeviceMU1, g_MuBasePath + "G", false, sizeof(FATX_SUPERBLOCK));
	CxbxRegisterDeviceHostPath(DeviceMU2, g_MuBasePath + "H", false, sizeof(FATX_SUPERBLOCK));
	CxbxRegisterDeviceHostPath(DeviceMU3, g_MuBasePath + "I", false, sizeof(FATX_SUPERBLOCK));
	CxbxRegisterDeviceHostPath(DeviceMU4, g_MuBasePath + "J", false, sizeof(FATX_SUPERBLOCK));
	CxbxRegisterDeviceHostPath(DeviceMU5, g_MuBasePath + "K", false, sizeof(FATX_SUPERBLOCK));
	CxbxRegisterDeviceHostPath(DeviceMU6, g_MuBasePath + "L", false, sizeof(FATX_SUPERBLOCK));
	CxbxRegisterDeviceHostPath(DeviceMU7, g_MuBasePath + "M", false, sizeof(FATX_SUPERBLOCK));
}

static bool CxbxrKrnlPrepareXbeMap()
{

	// Our executable DOS image header must be loaded at 0x00010000
	// Assert(ExeDosHeader == XBE_IMAGE_BASE);

	// Determine EXE's header locations & size :
	ExeNtHeader = (PIMAGE_NT_HEADERS)((ULONG_PTR)ExeDosHeader + ExeDosHeader->e_lfanew); // = + 0x138
	ExeOptionalHeader = (PIMAGE_OPTIONAL_HEADER) & (ExeNtHeader->OptionalHeader);

	// verify base of code of our executable is 0x00001000
	if (ExeNtHeader->OptionalHeader.BaseOfCode != CXBX_BASE_OF_CODE)
	{
		PopupFatal(nullptr, "Cxbx-Reloaded executuable requires it's base of code to be 0x00001000");
		return false; // TODO : Halt(0);
	}

	// Create a safe copy of the complete EXE header:
	DWORD ExeHeaderSize = ExeOptionalHeader->SizeOfHeaders; // Should end up as 0x400
	NewDosHeader = (PIMAGE_DOS_HEADER)VirtualAlloc(nullptr, ExeHeaderSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	if (NewDosHeader == nullptr) {
		PopupFatal(nullptr, "Unable to VirtualAlloc with 0x%X size for NewDosHeader", ExeHeaderSize);
		return false;
	}

	memcpy(NewDosHeader, ExeDosHeader, ExeHeaderSize);

	// Determine NewOptionalHeader, required by RestoreExeImageHeader
	NewNtHeader = (PIMAGE_NT_HEADERS)((ULONG_PTR)NewDosHeader + ExeDosHeader->e_lfanew);
	NewOptionalHeader = (PIMAGE_OPTIONAL_HEADER) & (NewNtHeader->OptionalHeader);

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

	return true;
}

/*! initialize emulation */
[[noreturn]] static void CxbxrKrnlInit(
	void* pTLSData,
	Xbe::TLS* pTLS,
	Xbe::LibraryVersion* LibraryVersion,
	Xbe::Header* XbeHeader,
	uint32_t XbeHeaderSize,
	void (*Entry)(),
	int BootFlags,
	HardwareModel hardwareModel
);

void CxbxKrnlEmulate(unsigned int reserved_systems, blocks_reserved_t blocks_reserved)
{
	// This is beginning of emulation process start, therefore require to have exception manager initialized
	// and capture any crash from this point and beyond. Useful for capture live crash and generate crash report.
	g_ExceptionManager = new ExceptionManager();

	// Set current process handle in order for CxbxrShutDown to work properly.
	g_CurrentProcessHandle = GetCurrentProcess(); // OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());

	// First of all, check if the EmuShared version matches the emu version and abort otherwise
	char GitVersionEmuShared[GitVersionMaxLength];
	g_EmuShared->GetGitVersion(GitVersionEmuShared);
	if (std::strncmp(GitVersionEmuShared, GetGitVersionStr(), GetGitVersionLength()) != 0) {
		PopupError(nullptr, "Mismatch detected between EmuShared and cxbx.exe/cxbxr-emu.dll, aborting."
			"\n\nPlease extract all contents from zip file and do not mix with older/newer builds.");
		CxbxrShutDown();
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

	/* Initialize Cxbx-Reloaded File Paths */
	CxbxrInitFilePaths();

	// Get DCHandle :
	// We must save this handle now to keep the child window working in the case we need to display the UEM
	HWND hWnd = nullptr;
	if (cli_config::GetValue(cli_config::hwnd, &tempStr)) {
		hWnd = (HWND)std::atoi(tempStr.c_str());
	}
	CxbxKrnl_hEmuParent = IsWindow(hWnd) ? hWnd : nullptr;

	int BootFlags;
	g_EmuShared->GetBootFlags(&BootFlags);

	// Set up the logging variables for the kernel process during initialization.
	log_sync_config();

	// When a reboot occur, we need to keep persistent memory buffer open before emulation process shutdown.
	if ((BootFlags & BOOT_QUICK_REBOOT) != 0) {
		g_VMManager.GetPersistentMemory();
	}

	CxbxrKrnlSyncGUI();

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
		CxbxrShutDown();
	}

	/* Must be called after CxbxrInitFilePaths and previous kernel process shutdown. */
	CxbxrLockFilePath();

	FILE* krnlLog = CxbxrKrnlSetupVerboseLog(BootFlags);

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
	if (!CxbxrKrnlPrepareXbeMap()) {
		return; // TODO : Halt(0);
	}

	// Load Xbox Keys from the Cxbx-Reloaded AppData directory
	LoadXboxKeys();

	EEPROM = CxbxRestoreEEPROM(szFilePath_EEPROM_bin);
	if (EEPROM == nullptr)
	{
		PopupFatal(nullptr, "Couldn't init EEPROM!");
		return; // TODO : Halt(0); 
	}

	// TODO : Instead of loading an Xbe here, initialize the kernel so that it will launch the Xbe on itself.
	// using XeLoadImage from LaunchDataPage->Header.szLaunchPath

	// Now we can load the XBE :
	HardwareModel hardwareModel = HardwareModel::Revision1_6; // It is configured by CxbxrKrnlXbeSystemSelector function according to console type.
	if (!CxbxrKrnlXbeSystemSelector(BootFlags, reserved_systems, blocks_reserved, hardwareModel)) {
		return;
	}

	// We need to remember a few XbeHeader fields, so we can switch between a valid ExeHeader and XbeHeader :
	StoreXbeImageHeader();

	// Restore enough of the executable image headers to keep WinAPI's working :
	RestoreExeImageHeader();

	CxbxrKrnlXbePatchXBEHSig();

	// Launch the XBE :
	{
		// Load TLS
		Xbe::TLS* XbeTls = (Xbe::TLS*)CxbxKrnl_Xbe->m_Header.dwTLSAddr;
		void* XbeTlsData = (XbeTls != nullptr) ? (void*)CxbxKrnl_Xbe->m_TLS->dwDataStartAddr : nullptr;
		// Decode Entry Point
		xbox::addr_xt EntryPoint = CxbxKrnl_Xbe->m_Header.dwEntryAddr;
		EntryPoint ^= XOR_EP_KEY[to_underlying(CxbxKrnl_Xbe->GetXbeType())];
		// Launch XBE
		CxbxrKrnlInit(
			XbeTlsData, 
			XbeTls, 
			CxbxKrnl_Xbe->m_LibraryVersion,
			(Xbe::Header*)CxbxKrnl_Xbe->m_Header.dwBaseAddr,
			CxbxKrnl_Xbe->m_Header.dwSizeofHeaders,
			(void(*)())EntryPoint,
 			BootFlags,
			hardwareModel
		);
	}

	if (!krnlLog) {
		(void)fclose(krnlLog);
	}
}
#pragma optimize("", on)

// Dump Xbe information
// TODO: May need to relocate elsewhere, like in a log file than here.
static void CxbxrLogDumpXbeInfo(Xbe::LibraryVersion* libVersionInfo)
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
		if (CxbxKrnl_Xbe != nullptr) {
			EmuLogInit(LOG_LEVEL::INFO, "XBE Version : %d.%d", CxbxKrnl_Xbe->GetDiscVersion(), CxbxKrnl_Xbe->GetPatchVersion());
		}
		EmuLogInit(LOG_LEVEL::INFO, "XBE Version (Hex): %08X", g_pCertificate->dwVersion);
		EmuLogInit(LOG_LEVEL::INFO, "XBE TitleName : %.40ls", g_pCertificate->wsTitleName);
		EmuLogInit(LOG_LEVEL::INFO, "XBE Region : %s", CxbxKrnl_Xbe->GameRegionToString());
	}

	// Dump Xbe library build numbers
	if (libVersionInfo != NULL) {
		for (uint32_t v = 0; v < CxbxKrnl_XbeHeader->dwLibraryVersions; v++) {
			EmuLogInit(LOG_LEVEL::INFO, "XBE Library %u : %.8s (version %d)", v, libVersionInfo->szName, libVersionInfo->wBuildVersion);
			libVersionInfo++;
		}
	}
}

// Process Hacks
static void CxbxrKrnlInitHacks()
{
	int HackEnabled = 0;
	g_EmuShared->GetDisablePixelShaders(&HackEnabled);
	g_DisablePixelShaders = !!HackEnabled;
	g_EmuShared->GetUseAllCores(&HackEnabled);
	g_UseAllCores = !!HackEnabled;
	g_EmuShared->GetSkipRdtscPatching(&HackEnabled);
	g_SkipRdtscPatching = !!HackEnabled;
}

[[noreturn]] static void CxbxrKrnlInit
(
	void                   *pTLSData,
	Xbe::TLS               *pTLS,
	Xbe::LibraryVersion    *pLibraryVersion,
	Xbe::Header            *pXbeHeader,
	uint32_t                dwXbeHeaderSize,
	void(*Entry)(),
	int BootFlags,
	HardwareModel hardwareModel)
{
	unsigned Host2XbStackBaseReserved = 0;
	__asm mov Host2XbStackBaseReserved, esp;
	unsigned Host2XbStackSizeReserved = EmuGenerateStackSize(Host2XbStackBaseReserved, 0);
	__asm sub esp, Host2XbStackSizeReserved;
    // Set windows timer period to 1ms
    // Windows will automatically restore this value back to original on program exit
    // But with this, we can replace some busy loops with sleeps.
    timeBeginPeriod(1);

    xbox::InitializeFscCacheEvent();

	// update caches
	CxbxKrnl_TLS = pTLS;
	CxbxKrnl_TLSData = pTLSData;
	CxbxKrnl_XbeHeader = pXbeHeader;

	// A patch to dwCertificateAddr is a requirement due to Windows TLS is overwriting dwGameRegion data address.
	// By using unalternated certificate data, it should no longer cause any problem with titles running and Cxbx's log as well.
	CxbxKrnl_XbeHeader->dwCertificateAddr = (uint32_t)&CxbxKrnl_Xbe->m_Certificate;
	g_pCertificate = &CxbxKrnl_Xbe->m_Certificate;

	// Initialize timer subsystem
	timer_init();
	// for unicode conversions
	setlocale(LC_ALL, "English");
	// Initialize DPC global
	InitDpcData();
#ifdef _DEBUG
//	PopupCustom(LOG_LEVEL::INFO, "Attach a Debugger");
//  Debug child processes using https://marketplace.visualstudio.com/items?itemName=GreggMiskelly.MicrosoftChildProcessDebuggingPowerTool
#endif

	// debug trace
	{
#ifdef _DEBUG_TRACE
		EmuLogInit(LOG_LEVEL::INFO, "Debug Trace Enabled.");
		EmuLogInit(LOG_LEVEL::INFO, "CxbxrKrnlInit\n"
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
			CxbxKrnl_hEmuParent, pTLSData, pTLS, pLibraryVersion, CxbxrKrnl_DebugMode, CxbxrKrnl_DebugFileName.c_str(), pXbeHeader, dwXbeHeaderSize, Entry);
#else
		EmuLogInit(LOG_LEVEL::INFO, "Debug Trace Disabled.");
#endif
	}

#ifdef _DEBUG_TRACE
	// VerifyHLEDataBase();
#endif

	CxbxrKrnlSetupDummyHeader();

	// Read which components need to be LLE'ed per user request
	{
		unsigned int CxbxLLE_Flags;
		g_EmuShared->GetFlagsLLE(&CxbxLLE_Flags);
		bLLE_APU = (CxbxLLE_Flags & LLE_APU) > 0;
		bLLE_GPU = (CxbxLLE_Flags & LLE_GPU) > 0;
		//bLLE_USB = (CxbxLLE_Flags & LLE_USB) > 0; // Reenable this when LLE USB actually works
		bLLE_JIT = (CxbxLLE_Flags & LLE_JIT) > 0;
	}

	CxbxrKrnlInitHacks();

#ifdef _DEBUG_PRINT_CURRENT_CONF
	PrintCurrentConfigurationLog();
#endif

	// Determine xbe path
	std::filesystem::path xbePath;
	{
		std::string szBuffer(szFilePath_Xbe);
		std::replace(szBuffer.begin(), szBuffer.end(), ';', '/');
		xbePath = szBuffer;
	}
	CxbxResolveHostToFullPath(xbePath, "xbe's file");

	std::filesystem::path relative_path;
	size_t lastFind = CxbxrKrnlGetRelativePath(relative_path);

	g_DiskBasePathHandle = CreateFile(g_DiskBasePath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

	CxbxrKrnlRegisterDevicePaths();

	bool isEmuDisk = CxbxrIsPathInsideEmuDisk(relative_path);
	CxbxrKrnlSetupCdRom0Path(BootFlags, lastFind, relative_path, isEmuDisk);

	std::mbstate_t ps = std::mbstate_t();
	const char *src = g_MuBasePath.c_str();
	std::wstring wMuBasePath(g_MuBasePath.size(), L'0');
	std::mbsrtowcs(wMuBasePath.data(), &src, wMuBasePath.size(), &ps);
	g_io_mu_metadata = new io_mu_metadata(wMuBasePath);

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

	CxbxrLogDumpXbeInfo(pLibraryVersion);

	// Make sure the Xbox1 code runs on one core (as the box itself has only 1 CPU,
	// this will better aproximate the environment with regard to multi-threading) :
	EmuLogInit(LOG_LEVEL::DEBUG, "Determining CPU affinity.");
	g_AffinityPolicy = AffinityPolicy::InitPolicy();

	// Create a kpcr for this thread. This is necessary because ObInitSystem needs to access the irql. This must also be done before
	// CxbxInitWindow because that function creates the xbox EmuUpdateTickCount thread
	EmuGenerateFS<true>(xbox::zeroptr, Host2XbStackBaseReserved, Host2XbStackSizeReserved);
	if (!xbox::ObInitSystem()) {
		CxbxrAbortEx(LOG_PREFIX_INIT, "Unable to intialize ObInitSystem.");
	}
	xbox::PsInitSystem();
	xbox::KiInitSystem();
	xbox::RtlInitSystem();

	// initialize graphics
	EmuLogInit(LOG_LEVEL::DEBUG, "Initializing render window.");
	CxbxInitWindow();

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

	InitXboxHardware(hardwareModel);

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

	// Verify that the emulator region matches the game region, if not, show a warning
	// that it may not work.
	if (!(g_pCertificate->dwGameRegion & EEPROM->EncryptedSettings.GameRegion))
	{
		auto expected = CxbxKrnl_Xbe->GameRegionToString();
		auto actual = CxbxKrnl_Xbe->GameRegionToString(EEPROM->EncryptedSettings.GameRegion);

		std::stringstream ss;
		ss << "The loaded title is designed for region: " << expected << "\n";
		ss << "However Cxbx-Reloaded is configured as: " << actual << "\n\n";
		ss << "This means that you may encounter emulation issues\n\n";
		ss << "You can fix this by changing your emulated Xbox region in EEPROM Settings\n\n";
		ss << "Please do not submit bug reports that result from incorrect region flags\n\n";
		ss << "Would you like to attempt emulation anyway?";

		PopupReturn ret = PopupWarningEx(nullptr, PopupButtons::YesNo, PopupReturn::No, ss.str().c_str());
		if (ret != PopupReturn::Yes)
		{
			CxbxrShutDown();
		}
	}
	

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

	// If this title is Chihiro, Setup JVS
	if (g_bIsChihiro) {
		JVS_Init();
	}

	EmuX86_Init();
	// Start the event thread
	xbox::HANDLE hThread;
	xbox::PsCreateSystemThread(&hThread, xbox::zeroptr, system_events, xbox::zeroptr, FALSE);
	// Launch the xbe
	xbox::PsCreateSystemThread(&hThread, xbox::zeroptr, CxbxLaunchXbe, Entry, FALSE);

	xbox::KeRaiseIrqlToDpcLevel();
	while (true) {
		xbox::KeWaitForDpc();
		ExecuteDpcQueue();
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

void CxbxrKrnlSuspendThreads()
{
	xbox::PLIST_ENTRY ThreadListEntry = KiUniqueProcess.ThreadListHead.Flink;
	std::vector<HANDLE> threads;
	threads.reserve(KiUniqueProcess.StackCount);

	// Don't use EmuKeGetPcr because that asserts kpcr
	xbox::KPCR* Pcr = reinterpret_cast<xbox::PKPCR>(__readfsdword(TIB_ArbitraryDataSlot));

	// If there's nothing in list entry, skip this step.
	if (!ThreadListEntry) {
		return;
	}

	while (ThreadListEntry != &KiUniqueProcess.ThreadListHead) {
		xbox::HANDLE UniqueThread = CONTAINING_RECORD(ThreadListEntry, xbox::ETHREAD, Tcb.ThreadListEntry)->UniqueThread;
		if (UniqueThread) {
			// Current thread is an xbox thread
			if (Pcr) {
				const auto& nHandle = GetNativeHandle<true>(UniqueThread);
				if (nHandle) {
					// We do not want to suspend current thread, so we let it skip this one.
					if (*nHandle != NtCurrentThread()) {
						threads.push_back(*nHandle);
					}
				}
			}
			// Otherwise, convert all UniqueThread to host thead handles.
			else {
				const auto& nHandle = GetNativeHandle<false>(UniqueThread);
				if (nHandle) {
					threads.push_back(*nHandle);
				}
			}
		}
		ThreadListEntry = ThreadListEntry->Flink;
	}

	for (const auto& thread : threads) {
		DWORD PrevCount = SuspendThread(thread);
		if (PrevCount == -1) {
			EmuLog(LOG_LEVEL::ERROR2, "Unable to suspend thread 0x%X for: %s", thread, WinError2Str().c_str());
		}
	}
}

void CxbxKrnlPrintUEM(ULONG ErrorCode)
{
	xbox::dword_xt Type;
	xbox::XBOX_EEPROM Eeprom;
	ULONG ResultSize;

	xbox::ntstatus_xt status = xbox::ExQueryNonVolatileSetting(xbox::XC_MAX_ALL, &Type, &Eeprom, sizeof(Eeprom), (xbox::PSIZE_T)&ResultSize);

	if (status == X_STATUS_SUCCESS)
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
		CxbxrAbort("Could not display the fatal error screen");
	}

	if (g_bIsChihiro)
	{
		// The Chihiro doesn't display the UEM
		CxbxrAbort("The running Chihiro xbe has encountered a fatal error and needs to close");
	}

	g_CxbxFatalErrorCode = ErrorCode;
	g_CxbxPrintUEM = true; // print the UEM

	// Force repaint screen in order for UEM to be visible every time.
	InvalidateRect(g_hEmuWindow, nullptr, TRUE);
	UpdateWindow(g_hEmuWindow);

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

void CxbxKrnlPanic()
{
    CxbxrAbort("Kernel Panic!");
}
