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
// *   Cxbx->Win32->CxbxKrnl->CxbxKrnl.cpp
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

#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

/* prevent name collisions */
namespace xboxkrnl
{
    #include <xboxkrnl/xboxkrnl.h>
};

#include "CxbxKrnl.h"
#include "CxbxVersion.h"
#include "Emu.h"
#include "EmuX86.h"
#include "EmuFile.h"
#include "EmuFS.h"
#include "EmuEEPROM.h" // For CxbxRestoreEEPROM, EEPROM, XboxFactoryGameRegion
#include "EmuKrnl.h"
#include "EmuShared.h"
#include "EmuNV2A.h" // For InitOpenGLContext
#include "HLEIntercept.h"
#include "ReservedMemory.h" // For virtual_memory_placeholder
#include "MemoryManager.h"

#include <shlobj.h>
#include <clocale>
#include <process.h>
#include <Shlwapi.h>
#include <time.h> // For time()

/* prevent name collisions */
namespace NtDll
{
    #include "EmuNtDll.h"
};

/*! thread local storage */
Xbe::TLS *CxbxKrnl_TLS = NULL;
/*! thread local storage data */
void *CxbxKrnl_TLSData = NULL;
/*! xbe header structure */
Xbe::Header *CxbxKrnl_XbeHeader = NULL;
/*! parent window handle */

HWND CxbxKrnl_hEmuParent = NULL;
DebugMode CxbxKrnl_DebugMode = DebugMode::DM_NONE;
char* CxbxKrnl_DebugFileName = NULL;

/*! thread handles */
static HANDLE g_hThreads[MAXIMUM_XBOX_THREADS] = { 0 };

char szFilePath_CxbxReloaded_Exe[MAX_PATH] = { 0 };
char szFolder_CxbxReloadedData[MAX_PATH] = { 0 };
char szFilePath_LaunchDataPage_bin[MAX_PATH] = { 0 };
char szFilePath_EEPROM_bin[MAX_PATH] = { 0 };
char szFilePath_memory_bin[MAX_PATH] = { 0 };

std::string CxbxBasePath;
HANDLE CxbxBasePathHandle;
Xbe* CxbxKrnl_Xbe = NULL;
XbeType g_XbeType = xtRetail;
bool g_bIsChihiro = false;
DWORD_PTR g_CPUXbox = 0;
DWORD_PTR g_CPUOthers = 0;

HANDLE g_CurrentProcessHandle = 0; // Set in CxbxKrnlInit

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
	Xbe::Certificate *pCertificate = (Xbe::Certificate*)CxbxKrnl_XbeHeader->dwCertificateAddr;

	// Patch the XBE Header to allow running from all media types
	pCertificate->dwAllowedMedia |= 0
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
	pCertificate->dwGameRegion = 0
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
	if (pCertificate->dwSize >= sizeof(Xbe::Certificate)) {
		pCertificate->dwSecurityFlags &= ~1;
	}
}

void SetupPerTitleKeys()
{
	// Generate per-title keys from the XBE Certificate
	Xbe::Certificate *pCertificate = (Xbe::Certificate*)CxbxKrnl_XbeHeader->dwCertificateAddr;
	UCHAR Digest[20] = {};

	// Set the LAN Key
	xboxkrnl::XcHMAC(xboxkrnl::XboxCertificateKey, xboxkrnl::XBOX_KEY_LENGTH, pCertificate->bzLanKey, xboxkrnl::XBOX_KEY_LENGTH, NULL, 0, Digest);
	memcpy(xboxkrnl::XboxLANKey, Digest, xboxkrnl::XBOX_KEY_LENGTH);

	// Signature Key
	xboxkrnl::XcHMAC(xboxkrnl::XboxCertificateKey, xboxkrnl::XBOX_KEY_LENGTH, pCertificate->bzSignatureKey, xboxkrnl::XBOX_KEY_LENGTH, NULL, 0, Digest);
	memcpy(xboxkrnl::XboxSignatureKey, Digest, xboxkrnl::XBOX_KEY_LENGTH);

	// Alternate Signature Keys
	for (int i = 0; i < xboxkrnl::ALTERNATE_SIGNATURE_COUNT; i++) {
		xboxkrnl::XcHMAC(xboxkrnl::XboxCertificateKey, xboxkrnl::XBOX_KEY_LENGTH, pCertificate->bzTitleAlternateSignatureKey[i], xboxkrnl::XBOX_KEY_LENGTH, NULL, 0, Digest);
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
		printf("Emu: WARNING!! Problem with ExceptionFilter\n");
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

void *CxbxRestoreContiguousMemory(char *szFilePath_memory_bin)
{
	// First, try to open an existing memory.bin file :
	HANDLE hFile = CreateFile(szFilePath_memory_bin,
		GENERIC_READ | GENERIC_WRITE,
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
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			/* lpSecurityAttributes */nullptr,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, // FILE_FLAG_WRITE_THROUGH
			/* hTemplateFile */nullptr);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			CxbxKrnlCleanup("CxbxRestoreContiguousMemory : Couldn't create memory.bin file!\n");
			return nullptr;
		}
	}

	// TODO : Make sure memory.bin is at least 64 MB in size - FileSeek(hFile, CONTIGUOUS_MEMORY_SIZE, soFromBeginning);

	HANDLE hFileMapping = CreateFileMapping(
		hFile,
		/* lpFileMappingAttributes */nullptr,
		PAGE_READWRITE,
		/* dwMaximumSizeHigh */0,
		/* dwMaximumSizeLow */CONTIGUOUS_MEMORY_SIZE,
		/**/nullptr);
	if (hFileMapping == NULL)
	{
		CxbxKrnlCleanup("CxbxRestoreContiguousMemory : Couldn't create contiguous memory.bin file mapping!\n");
		return nullptr;
	}

	// Map memory.bin contents into memory :
	void *memory = (void *)MapViewOfFileEx(
		hFileMapping,
		FILE_MAP_READ | FILE_MAP_WRITE,
		/* dwFileOffsetHigh */0,
		/* dwFileOffsetLow */0,
		CONTIGUOUS_MEMORY_SIZE,
		(void *)MM_SYSTEM_PHYSICAL_MAP);
	if (memory == NULL)
	{
		CxbxKrnlCleanup("CxbxRestoreContiguousMemory: Couldn't map contiguous memory.bin into memory!");
		return nullptr;
	}

	if (NeedsInitialization)
	{
		memset(memory, 0, CONTIGUOUS_MEMORY_SIZE);
		DbgPrintf("EmuMain: Initialized contiguous memory\n");
	}
	else
		DbgPrintf("EmuMain: Loaded contiguous memory.bin\n");

	// Map memory.bin contents into tiled memory too :
	void *tiled_memory = (void *)MapViewOfFileEx(
		hFileMapping,
		FILE_MAP_READ | FILE_MAP_WRITE,
		/* dwFileOffsetHigh */0,
		/* dwFileOffsetLow */0,
		CONTIGUOUS_MEMORY_SIZE,
		(void *)0xF0000000);
	if (tiled_memory == NULL)
	{
		CxbxKrnlCleanup("CxbxRestoreContiguousMemory: Couldn't map contiguous memory.bin into tiled memory!");
		return nullptr;
	}

	DbgPrintf("EmuMain: Mapped contiguous to tiled memory\n");

	return memory;
}

#pragma optimize("", off)

void CxbxPopupMessage(const char *message)
{
	DbgPrintf("Popup : %s\n", message);
	MessageBox(NULL, message, "Cxbx-Reloaded", MB_OK | MB_ICONEXCLAMATION);
}

void PrintCurrentConfigurationLog() {
	// Print current LLE configuration
	{
		printf("---------------------------- LLE CONFIG ----------------------------\n");
		printf("[0x%X] EmuMain: LLE for APU is %s\n", GetCurrentThreadId(), bLLE_APU ? "enabled" : "disabled");
		printf("[0x%X] EmuMain: LLE for GPU is %s\n", GetCurrentThreadId(), bLLE_GPU ? "enabled" : "disabled");
		printf("[0x%X] EmuMain: LLE for JIT is %s\n", GetCurrentThreadId(), bLLE_JIT ? "enabled" : "disabled");
	}

	// Print current INPUT configuration
	{
		printf("--------------------------- INPUT CONFIG ---------------------------\n");
		printf("[0x%X] EmuMain: Using %s\n", GetCurrentThreadId(), g_XInputEnabled ? "XInput" : "DirectInput");
	}

	// Print current video configuration
	{
		XBVideo XBVideoConf;
		g_EmuShared->GetXBVideo(&XBVideoConf);

		printf("--------------------------- VIDEO CONFIG ---------------------------\n");
		printf("[0x%X] EmuMain: Direct3D Device: %s\n", GetCurrentThreadId(), XBVideoConf.GetDirect3DDevice() == 0 ? "Direct3D HAL (Hardware Accelerated)" : "Direct3D REF (Software)");
		printf("[0x%X] EmuMain: Video Resolution: %s\n", GetCurrentThreadId(), XBVideoConf.GetVideoResolution());
		printf("[0x%X] EmuMain: Force VSync is %s\n", GetCurrentThreadId(), XBVideoConf.GetVSync() ? "enabled" : "disabled");
		printf("[0x%X] EmuMain: Fullscreen is %s\n", GetCurrentThreadId(), XBVideoConf.GetFullscreen() ? "enabled" : "disabled");
		printf("[0x%X] EmuMain: Hardware YUV is %s\n", GetCurrentThreadId(), XBVideoConf.GetHardwareYUV() ? "enabled" : "disabled");
	}

	// Print current audio configuration
	{
		XBAudio XBAudioConf;
		g_EmuShared->GetXBAudio(&XBAudioConf);

		printf("--------------------------- AUDIO CONFIG ---------------------------\n");
		printf("[0x%X] EmuMain: Audio Adapter: %s\n", GetCurrentThreadId(), XBAudioConf.GetAudioAdapter().Data1 == 0 ? "Primary Audio Device" : "Secondary Audio Device");
		printf("[0x%X] EmuMain: Legacy Audio Hack is %s\n", GetCurrentThreadId(), XBAudioConf.GetLegacyAudioHack() ? "enabled" : "disabled");
		printf("[0x%X] EmuMain: PCM is %s\n", GetCurrentThreadId(), XBAudioConf.GetPCM() ? "enabled" : "disabled");
		printf("[0x%X] EmuMain: XADPCM is %s\n", GetCurrentThreadId(), XBAudioConf.GetXADPCM() ? "enabled" : "disabled");
		printf("[0x%X] EmuMain: Unknown Codec is %s\n", GetCurrentThreadId(), XBAudioConf.GetUnknownCodec() ? "enabled" : "disabled");
	}

	printf("------------------------- END OF CONFIG LOG ------------------------\n");

}

static unsigned int WINAPI CxbxKrnlInterruptThread(PVOID param)
{
	EmuGenerateFS(CxbxKrnl_TLS, CxbxKrnl_TLSData);

	_controlfp(_PC_53, _MCW_PC); // Set Precision control to 53 bits (verified setting)
	_controlfp(_RC_NEAR, _MCW_RC); // Set Rounding control to near (unsure about this)

	while (true) {
		for (int i = 0; i < MAX_BUS_INTERRUPT_LEVEL; i++) {
			// If the interrupt is pending and connected, process it
			if (HalSystemInterrupts[i].IsPending() && EmuInterruptList[i]->Connected) {
				HalSystemInterrupts[i].Trigger(EmuInterruptList[i]);
			}
		}

		SwitchToThread();
	}

	return 0;
}

void CxbxKrnlMain(int argc, char* argv[])
{
	// Skip '/load' switch
	// Get XBE Name :
	std::string xbePath = argv[2];

	// Get DCHandle :
	HWND hWnd = 0;
	if (argc > 2) {
		hWnd = (HWND)StrToInt(argv[3]);
	}

	// Get KernelDebugMode :
	DebugMode DbgMode = DebugMode::DM_NONE;
	if (argc > 3) {
		DbgMode = (DebugMode)StrToInt(argv[4]);
	}

	// Get KernelDebugFileName :
	std::string DebugFileName;
	if (argc > 4) {
		DebugFileName = argv[5];
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
			CxbxPopupMessage("Cxbx-Reloaded executuable requires it's base of code to be 0x00001000");
			return; // TODO : Halt(0); 
		}

		// verify virtual_memory_placeholder is located at 0x00011000
		if ((UINT_PTR)(&(virtual_memory_placeholder[0])) != (XBE_IMAGE_BASE + CXBX_BASE_OF_CODE))
		{
			CxbxPopupMessage("virtual_memory_placeholder is not loaded to base address 0x00011000 (which is a requirement for Xbox emulation)");
			return; // TODO : Halt(0); 
		}

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
		VirtualProtect((void*)XBE_IMAGE_BASE, XBE_MAX_VA - XBE_IMAGE_BASE, PAGE_EXECUTE_READWRITE, &OldProtection);

		// Clear out the virtual memory range
		memset((void*)XBE_IMAGE_BASE, 0, XBE_MAX_VA - XBE_IMAGE_BASE);

		// Restore enough of the executable image headers to keep WinAPI's working :
		RestoreExeImageHeader();
	}

	CxbxRestoreContiguousMemory(szFilePath_memory_bin);

	CxbxRestorePersistentMemoryRegions();

	EEPROM = CxbxRestoreEEPROM(szFilePath_EEPROM_bin);
	if (EEPROM == nullptr)
	{
		CxbxPopupMessage("Couldn't init EEPROM!");
		return; // TODO : Halt(0); 
	}

	// TODO : Instead of loading an Xbe here, initialize the kernel so that it will launch the Xbe on itself.
	// using XeLoadImage from LaunchDataPage->Header.szLaunchPath

	// Now we can load and run the XBE :
	// MapAndRunXBE(XbePath, DCHandle);
	{
		// Load Xbe (this one will reside above WinMain's virtual_memory_placeholder) 
		g_EmuShared->SetXbePath(xbePath.c_str());
		CxbxKrnl_Xbe = new Xbe(xbePath.c_str()); // TODO : Instead of using the Xbe class, port Dxbx _ReadXbeBlock()

		// Detect XBE type :
		g_XbeType = GetXbeType(&CxbxKrnl_Xbe->m_Header);

		// Register if we're running an Chihiro executable (otherwise it's an Xbox executable)
		g_bIsChihiro = (g_XbeType == xtChihiro);

		// Determine memory size accordingly :
		SIZE_T memorySize = (g_bIsChihiro ? CHIHIRO_MEMORY_SIZE : XBOX_MEMORY_SIZE);

		// Copy over loaded Xbe Header to specified base address
		memcpy((void*)CxbxKrnl_Xbe->m_Header.dwBaseAddr, &CxbxKrnl_Xbe->m_Header, sizeof(Xbe::Header));
		// Copy over the certificate
		memcpy((void*)(CxbxKrnl_Xbe->m_Header.dwBaseAddr + sizeof(Xbe::Header)), CxbxKrnl_Xbe->m_HeaderEx, CxbxKrnl_Xbe->m_ExSize);
		// Copy over the library versions
		memcpy((void*)CxbxKrnl_Xbe->m_Header.dwLibraryVersionsAddr, CxbxKrnl_Xbe->m_LibraryVersion, CxbxKrnl_Xbe->m_Header.dwLibraryVersions * sizeof(DWORD));
		// TODO : Actually, instead of copying from CxbxKrnl_Xbe, we should load the entire Xbe directly into memory, like Dxbx does - see _ReadXbeBlock()

		// Verify no section would load outside virtual_memory_placeholder (which would overwrite Cxbx code)
		for (uint32 i = 0; i < CxbxKrnl_Xbe->m_Header.dwSections; i++) {
			xbaddr section_end = CxbxKrnl_Xbe->m_SectionHeader[i].dwVirtualAddr + CxbxKrnl_Xbe->m_SectionHeader[i].dwSizeofRaw;
			if (section_end >= XBE_MAX_VA)
			{
				CxbxPopupMessage("Couldn't load XBE section - please report this!");
				return; // TODO : Halt(0); 
			}
		}

		// Load all sections to their requested Virtual Address :
		for (uint32 i = 0; i < CxbxKrnl_Xbe->m_Header.dwSections; i++) {
			memcpy((void*)CxbxKrnl_Xbe->m_SectionHeader[i].dwVirtualAddr, CxbxKrnl_Xbe->m_bzSection[i], CxbxKrnl_Xbe->m_SectionHeader[i].dwSizeofRaw);
		}

		// We need to remember a few XbeHeader fields, so we can switch between a valid ExeHeader and XbeHeader :
		StoreXbeImageHeader();

		// Restore enough of the executable image headers to keep WinAPI's working :
		RestoreExeImageHeader();
	}
		
	// Before readout, make sure our kernel thunk table references the Windows host timer addresses :
	ConnectWindowsTimersToThunkTable();

	// Decode kernel thunk table address :
	uint32_t kt = CxbxKrnl_Xbe->m_Header.dwKernelImageThunkAddr;
	kt ^= XOR_KT_KEY[g_XbeType];

	// Process the Kernel thunk table to map Kernel function calls to their actual address :
	{
		uint32_t* kt_tbl = (uint32_t*)kt;
		int i = 0;
		while (kt_tbl[i] != 0) {
			int t = kt_tbl[i] & 0x7FFFFFFF;
			kt_tbl[i] = CxbxKrnl_KernelThunkTable[t];
			i++;
		}
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
			hWnd, 
			XbeTlsData, 
			XbeTls, 
			CxbxKrnl_Xbe->m_LibraryVersion, 
			DbgMode,
			DebugFileName.c_str(),
			(Xbe::Header*)CxbxKrnl_Xbe->m_Header.dwBaseAddr,
			CxbxKrnl_Xbe->m_Header.dwSizeofHeaders,
			(void(*)())EntryPoint
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
		} else {
			EmuWarning("Keys.bin has an incorrect filesize. Should be %d bytes", xboxkrnl::XBOX_KEY_LENGTH * 2);
		}

		fclose(fp);
		return;
	}

	// If we didn't already exit the function, keys.bin could not be loaded
	EmuWarning("Failed to load Keys.bin. Cxbx-Reloaded will be unable to read Save Data from a real Xbox");
}

void CxbxKrnlInit
(
	HWND                    hwndParent,
	void                   *pTLSData,
	Xbe::TLS               *pTLS,
	Xbe::LibraryVersion    *pLibraryVersion,
	DebugMode               DbgMode,
	const char             *szDebugFilename,
	Xbe::Header            *pXbeHeader,
	uint32                  dwXbeHeaderSize,
	void(*Entry)())
{
	// update caches
	CxbxKrnl_TLS = pTLS;
	CxbxKrnl_TLSData = pTLSData;
	CxbxKrnl_XbeHeader = pXbeHeader;
	CxbxKrnl_hEmuParent = IsWindow(hwndParent) ? hwndParent : NULL;
	CxbxKrnl_DebugMode = DbgMode;
	CxbxKrnl_DebugFileName = (char*)szDebugFilename;
	// for unicode conversions
	setlocale(LC_ALL, "English");
	g_CurrentProcessHandle = GetCurrentProcess();
	CxbxInitPerformanceCounters();
#ifdef _DEBUG
//	CxbxPopupMessage("Attach a Debugger");
//  Debug child processes using https://marketplace.visualstudio.com/items?itemName=GreggMiskelly.MicrosoftChildProcessDebuggingPowerTool
#endif

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
		if (DbgMode == DM_FILE)
			freopen(szDebugFilename, "wt", stdout);
		else
		{
			char buffer[16];
			if (GetConsoleTitle(buffer, 16) != NULL)
				freopen("nul", "w", stdout);
		}
	}

	// Write a header to the log
	{
		printf("[0x%X] EmuMain: Cxbx-Reloaded Version %s\n", GetCurrentThreadId(), _CXBX_VERSION);

		time_t startTime = time(nullptr);
		struct tm* tm_info = localtime(&startTime);
		char timeString[26];
		strftime(timeString, 26, "%F %T", tm_info);
		printf("[0x%X] EmuMain: Log started at %s\n", GetCurrentThreadId(), timeString);
	}

	// debug trace
	{
#ifdef _DEBUG_TRACE
		printf("[0x%X] EmuMain: Debug Trace Enabled.\n", GetCurrentThreadId());
		printf("[0x%X] EmuMain: CxbxKrnlInit\n"
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
			GetCurrentThreadId(), hwndParent, pTLSData, pTLS, pLibraryVersion, DbgMode, szDebugFilename, pXbeHeader, dwXbeHeaderSize, Entry);
#else
		printf("[0x%X] EmuMain: Debug Trace Disabled.\n", GetCurrentThreadId());
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
	}

	// Read which components need to be LLE'ed :
	{
		int CxbxLLE_Flags;
		g_EmuShared->GetFlagsLLE(&CxbxLLE_Flags);
		bLLE_APU = (CxbxLLE_Flags & LLE_APU) > 0;
		bLLE_GPU = (CxbxLLE_Flags & LLE_GPU) > 0;
		bLLE_JIT = (CxbxLLE_Flags & LLE_JIT) > 0;
	}

	// Process XInput Enabled flag
	{
		int XInputEnabled;
		g_EmuShared->GetXInputEnabled(&XInputEnabled);
		g_XInputEnabled = XInputEnabled;
	}

#ifdef _DEBUG_PRINT_CURRENT_CONF
	PrintCurrentConfigurationLog();
#endif
	
	// Initialize devices :
	char szBuffer[MAX_PATH];
	SHGetSpecialFolderPath(NULL, szBuffer, CSIDL_APPDATA, TRUE);
	strcat(szBuffer, "\\Cxbx-Reloaded\\");
	std::string basePath(szBuffer);
	CxbxBasePath = basePath + "EmuDisk\\";

	// Load Per-Xbe Keys from the Cxbx-Reloaded AppData directory
	LoadXboxKeys(szBuffer);

	// Determine XBE Path
	memset(szBuffer, 0, MAX_PATH);
	g_EmuShared->GetXbePath(szBuffer);
	std::string xbePath(szBuffer);
	PathRemoveFileSpec(szBuffer);
	std::string xbeDirectory(szBuffer);
	CxbxBasePathHandle = CreateFile(CxbxBasePath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	memset(szBuffer, 0, MAX_PATH);
	sprintf(szBuffer, "%08X", ((Xbe::Certificate*)pXbeHeader->dwCertificateAddr)->dwTitleId);
	std::string titleId(szBuffer);
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
	DbgPrintf("EmuMain : Creating default symbolic links.\n");
	{
		// TODO: DriveD should always point to the Xbe Path
		// This is the only symbolic link the Xbox Kernel sets, the rest are set by the application, usually via XAPI.
		// If the Xbe is located outside of the emulated HDD, mounting it as DeviceCdrom0 is correct
		// If the Xbe is located inside the emulated HDD, the full path should be used, eg: "\\Harddisk0\\partition2\\xboxdash.xbe"
		CxbxCreateSymbolicLink(DriveD, DeviceCdrom0);
		// Arrange that the Xbe path can reside outside the partitions, and put it to g_hCurDir :
		EmuNtSymbolicLinkObject* xbePathSymbolicLinkObject = FindNtSymbolicLinkObjectByDriveLetter(CxbxDefaultXbeDriveLetter);
		g_hCurDir = xbePathSymbolicLinkObject->RootDirectoryHandle;
		// Determine Xbox path to XBE and place it in XeImageFileName
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
		xboxkrnl::XeImageFileName.Buffer = (PCHAR)g_MemoryManager.Allocate(MAX_PATH);
		sprintf(xboxkrnl::XeImageFileName.Buffer, "%c:\\%s", CxbxDefaultXbeDriveLetter, fileName.c_str());
		xboxkrnl::XeImageFileName.Length = (USHORT)strlen(xboxkrnl::XeImageFileName.Buffer);
		DbgPrintf("EmuMain : XeImageFileName = %s\n", xboxkrnl::XeImageFileName.Buffer);
	}

	// duplicate handle in order to retain Suspend/Resume thread rights from a remote thread
	{
		HANDLE hDupHandle = NULL;

		DuplicateHandle(g_CurrentProcessHandle, GetCurrentThread(), g_CurrentProcessHandle, &hDupHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);
		CxbxKrnlRegisterThread(hDupHandle);
	}

	// Clear critical section list
	//extern void InitializeSectionStructures(void); 
	InitializeSectionStructures();

	// Make sure the Xbox1 code runs on one core (as the box itself has only 1 CPU,
	// this will better aproximate the environment with regard to multi-threading) :
	DbgPrintf("EmuMain : Determining CPU affinity.\n");
	{
		if (!GetProcessAffinityMask(g_CurrentProcessHandle, &g_CPUXbox, &g_CPUOthers))
			CxbxKrnlCleanup("EmuMain: GetProcessAffinityMask failed.");

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

	// initialize grapchics
	DbgPrintf("EmuMain: Initializing render window.\n");
	XTL::CxbxInitWindow(pXbeHeader, dwXbeHeaderSize);

    XTL::CxbxInitAudio();

	EmuHLEIntercept(pXbeHeader);

	if (bLLE_GPU)
	{
		DbgPrintf("EmuMain: Initializing OpenGL.\n");
		InitOpenGLContext();
	}
	else
	{
		DbgPrintf("EmuMain: Initializing Direct3D.\n");
		XTL::EmuD3DInit();
	}

	// Apply Media Patches to bypass Anti-Piracy checks
	// Required until we perfect emulation of X2 DVD Authentication
	// See: https://multimedia.cx/eggs/xbox-sphinx-protocol/
	ApplyMediaPatches();

	// Setup per-title encryption keys
	SetupPerTitleKeys();

	EmuInitFS();

	InitXboxThread(g_CPUXbox);

	EmuX86_Init();
	// Create the interrupt processing thread
	DWORD dwThreadId;
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, NULL, CxbxKrnlInterruptThread, NULL, NULL, (uint*)&dwThreadId);
	// Make sure Xbox1 code runs on one core :
	SetThreadAffinityMask(hThread, g_CPUXbox);
    DbgPrintf("EmuMain: Initial thread starting.\n");
	CxbxLaunchXbe(Entry);
    DbgPrintf("EmuMain: Initial thread ended.\n");
    fflush(stdout);
	//	EmuShared::Cleanup();   FIXME: commenting this line is a bad workaround for issue #617 (https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/617)
    CxbxKrnlTerminateThread();
    return;
}

void CxbxInitFilePaths()
{
	char szAppData[MAX_PATH];
	SHGetSpecialFolderPath(NULL, szAppData, CSIDL_APPDATA, TRUE);
	snprintf(szFolder_CxbxReloadedData, MAX_PATH, "%s\\Cxbx-Reloaded", szAppData);

	// Make sure our data folder exists :
	int result = SHCreateDirectoryEx(nullptr, szFolder_CxbxReloadedData, nullptr);
	if ((result != ERROR_SUCCESS) && (result != ERROR_ALREADY_EXISTS))
		CxbxKrnlCleanup("CxbxInitFilePaths : Couldn't create Cxbx-Reloaded AppData folder!");

	snprintf(szFilePath_LaunchDataPage_bin, MAX_PATH, "%s\\CxbxLaunchDataPage.bin", szFolder_CxbxReloadedData);
	snprintf(szFilePath_EEPROM_bin, MAX_PATH, "%s\\EEPROM.bin", szFolder_CxbxReloadedData);
	snprintf(szFilePath_memory_bin, MAX_PATH, "%s\\memory.bin", szFolder_CxbxReloadedData);

	GetModuleFileName(GetModuleHandle(NULL), szFilePath_CxbxReloaded_Exe, MAX_PATH);
}

// TODO : Is DefaultLaunchDataPage really necessary? (No-one assigns it yet to LaunchDataPage)
xboxkrnl::LAUNCH_DATA_PAGE DefaultLaunchDataPage =
{
	{   // header
		2,  // 2: dashboard, 0: title
		0,
		"D:\\default.xbe",
		0
	}
};

void CxbxRestoreLaunchDataPage()
{
	// If CxbxLaunchDataPage.bin exist, load it into "persistent" memory
	FILE* fp = fopen(szFilePath_LaunchDataPage_bin, "rb");
	if (fp)
	{
		// Make sure LaunchDataPage is writeable  :
		if (xboxkrnl::LaunchDataPage == &DefaultLaunchDataPage)
			xboxkrnl::LaunchDataPage = NULL;

		if (xboxkrnl::LaunchDataPage == NULL)
			xboxkrnl::LaunchDataPage = (xboxkrnl::LAUNCH_DATA_PAGE *)xboxkrnl::MmAllocateContiguousMemory(sizeof(xboxkrnl::LAUNCH_DATA_PAGE));

		// Read in the contents.
		fseek(fp, 0, SEEK_SET);
		fread(xboxkrnl::LaunchDataPage, sizeof(xboxkrnl::LAUNCH_DATA_PAGE), 1, fp);
		fclose(fp);
		// Delete the file once we're done.
		remove(szFilePath_LaunchDataPage_bin);

		DbgPrintf("EmuMain: Restored LaunchDataPage\n");
	}
}

void CxbxRestorePersistentMemoryRegions()
{
	CxbxRestoreLaunchDataPage();
	// TODO : Restore all other persistent memory regions here too.
}

void CxbxKrnlCleanup(const char *szErrorMessage, ...)
{
    g_bEmuException = true;

    CxbxKrnlResume();

    // print out error message (if exists)
    if(szErrorMessage != NULL)
    {
        char szBuffer1[255];
        char szBuffer2[255];

        va_list argp;

        sprintf(szBuffer1, "[0x%X] EmuMain: Received Fatal Message:\n\n* ", GetCurrentThreadId());

        va_start(argp, szErrorMessage);
        vsprintf(szBuffer2, szErrorMessage, argp);
        va_end(argp);

        strcat(szBuffer1, szBuffer2);
        strcat(szBuffer1, "\n");

		CxbxPopupMessage(szBuffer1); // Will also DbgPrintf
    }

    printf("CxbxKrnl: Terminating Process\n");
    fflush(stdout);

    // cleanup debug output
    {
        FreeConsole();

        char buffer[16];

        if(GetConsoleTitle(buffer, 16) != NULL)
            freopen("nul", "w", stdout);
    }

    if(CxbxKrnl_hEmuParent != NULL)
        SendMessage(CxbxKrnl_hEmuParent, WM_PARENTNOTIFY, WM_DESTROY, 0);

	EmuShared::Cleanup();
    TerminateProcess(g_CurrentProcessHandle, 0);

    return;
}

void CxbxKrnlRegisterThread(HANDLE hThread)
{
    int v=0;

    for(v=0;v<MAXIMUM_XBOX_THREADS;v++)
    {
        if(g_hThreads[v] == 0)
        {
            g_hThreads[v] = hThread;
            break;
        }
    }

    if(v == MAXIMUM_XBOX_THREADS)
    {
        CxbxKrnlCleanup("There are too many active threads!");
    }
}

void CxbxKrnlSuspend()
{
    if(g_bEmuSuspended || g_bEmuException)
        return;

    for(int v=0;v<MAXIMUM_XBOX_THREADS;v++)
    {
        if(g_hThreads[v] != NULL)
        {
            DWORD dwExitCode;

            if(GetExitCodeThread(g_hThreads[v], &dwExitCode) && dwExitCode == STILL_ACTIVE)
            {
                // suspend thread if it is active
                SuspendThread(g_hThreads[v]);
            }
            else
            {
                // remove thread from thread list if it is dead
                g_hThreads[v] = 0;
            }
        }
    }

    // append 'paused' to rendering window caption text
    {
        char szBuffer[256];

        HWND hWnd = (CxbxKrnl_hEmuParent != NULL) ? CxbxKrnl_hEmuParent : g_hEmuWindow;

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

        HWND hWnd = (CxbxKrnl_hEmuParent != NULL) ? CxbxKrnl_hEmuParent : g_hEmuWindow;

        GetWindowText(hWnd, szBuffer, 255);

        szBuffer[strlen(szBuffer)-9] = '\0';

        SetWindowText(hWnd, szBuffer);
    }

    for(int v=0;v<MAXIMUM_XBOX_THREADS;v++)
    {
        if(g_hThreads[v] != NULL)
        {
            DWORD dwExitCode;

            if(GetExitCodeThread(g_hThreads[v], &dwExitCode) && dwExitCode == STILL_ACTIVE)
            {
                // resume thread if it is active
                ResumeThread(g_hThreads[v]);
            }
            else
            {
                // remove thread from thread list if it is dead
                g_hThreads[v] = 0;
            }
        }
    }

    g_bEmuSuspended = false;
}

void CxbxKrnlTerminateThread()
{
    TerminateThread(GetCurrentThread(), 0);
}

void CxbxKrnlPanic()
{
    DbgPrintf("EmuMain: CxbxKrnlPanic()\n");

    CxbxKrnlCleanup("Kernel Panic!");
}
