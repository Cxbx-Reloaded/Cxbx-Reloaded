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
// *  Cxbx is free software; you can redistribute it
// *  and/or modify it under the terms of the GNU General Public
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
#include "Emu.h"
#include "EmuX86.h"
#include "EmuFile.h"
#include "EmuFS.h"
#include "EmuShared.h"
#include "EmuNV2A.h" // For InitOpenGLContext
#include "HLEIntercept.h"
#include "ReservedMemory.h" // For emulated_memory_placeholder

#include <shlobj.h>
#include <clocale>
#include <Shlwapi.h>

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
	pCertificate->dwAllowedMedia = XBEIMAGE_MEDIA_TYPE_HARD_DISK | XBEIMAGE_MEDIA_TYPE_DVD_CD | XBEIMAGE_MEDIA_TYPE_MEDIA_BOARD;

	// Patch the XBE Header to allow running on all regions
	pCertificate->dwGameRegion = XBEIMAGE_GAME_REGION_MANUFACTURING | XBEIMAGE_GAME_REGION_NA | XBEIMAGE_GAME_REGION_JAPAN | XBEIMAGE_GAME_REGION_RESTOFWORLD;

	// Generic Media Check Patches (From DVD2XBOX)
	typedef struct
	{
		std::string name;
		std::vector<uint8_t> data;
		std::vector<uint8_t> patch;
	} hexpatch_t;
	
	std::vector<hexpatch_t> mediaPatches;
	mediaPatches.push_back({ "Media Patch 1", 
		{ 0x74, 0x4B, 0xE8, 0xCA, 0xFD, 0xFF, 0xFF, 0x85, 0xC0, 0x7D, 0x06, 0x33, 0xC0, 0x50, 0x50, 0xEB, 0x44, 0xF6, 0x05},
		{ 0x74, 0x4B, 0xE8, 0xCA, 0xFD, 0xFF, 0xFF, 0x85, 0xC0, 0xEB, 0x06, 0x33, 0xC0, 0x50, 0x50, 0xEB, 0x44, 0xF6, 0x05 }
	});

	mediaPatches.push_back({ "Media Patch 2", 
		{ 0xE8, 0xCA, 0xFD, 0xFF, 0xFF, 0x85, 0xC0, 0x7D }, 
		{ 0xE8, 0xCA, 0xFD, 0xFF, 0xFF, 0x85, 0xC0, 0xEB } 
	});

	mediaPatches.push_back({ "FATX Patch",
		{ 0x46, 0x41, 0x54, 0x58, 0x00, 0x00, 0x00, 0x00, 0x44, 0x3A, 0x5C },
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } 
	});

	for (auto it = mediaPatches.begin(); it < mediaPatches.end(); ++it)	{
		hexpatch_t patch = *it;
		
		// Iterate through the XBE until we find the patch
		for (xbaddr addr = CxbxKrnl_XbeHeader->dwBaseAddr; addr < CxbxKrnl_XbeHeader->dwSizeofImage + CxbxKrnl_XbeHeader->dwBaseAddr; addr++) {
			if (memcmp((void*)addr, &patch.data[0], patch.data.size()) == 0) {
				printf("Applying %s\n", patch.name.c_str());
				memcpy((void*)addr, &patch.patch[0], patch.patch.size());
				addr += patch.data.size() - 1;
			}
		}
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
		// Determine EXE's header locations & size :
		ExeNtHeader = (PIMAGE_NT_HEADERS)((ULONG_PTR)ExeDosHeader + ExeDosHeader->e_lfanew); // = + 0x138
		ExeOptionalHeader = (PIMAGE_OPTIONAL_HEADER)&(ExeNtHeader->OptionalHeader);

		// verify emulated_memory_placeholder is located at 0x00011000
		if ((UINT_PTR)(&(emulated_memory_placeholder[0])) != (XBE_IMAGE_BASE + ExeNtHeader->OptionalHeader.BaseOfCode))
		{
			MessageBox(NULL, "emulated_memory_placeholder is not loaded to base address 0x00011000 (which is a requirement for Xbox emulation)", "Cxbx-Reloaded", MB_OK);
			return; // TODO : Halt(0); 
		}

		// Create a safe copy of the complete EXE header:
		DWORD ExeHeaderSize = ExeOptionalHeader->SizeOfHeaders; // Should end up as 0x400
		NewDosHeader = (PIMAGE_DOS_HEADER)VirtualAlloc(nullptr, ExeHeaderSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		memcpy(NewDosHeader, ExeDosHeader, ExeHeaderSize);

		// Determine NewNtHeader and NewOptionalHeader, both required by RestoreExeImageHeader
		NewNtHeader = (PIMAGE_NT_HEADERS)((ULONG_PTR)NewDosHeader + ExeDosHeader->e_lfanew);
		NewOptionalHeader = (PIMAGE_OPTIONAL_HEADER)&(NewNtHeader->OptionalHeader);

		// Make sure the new DOS header points to the new relative NtHeader location:
		NewDosHeader->e_lfanew = (ULONG_PTR)NewNtHeader - XBE_IMAGE_BASE;

		// Note : NewOptionalHeader->ImageBase can stay at ExeOptionalHeader->ImageBase = 0x00010000

		// Note : Since emulated_memory_placeholder prevents overlap between reserved xbox memory
		// and Cxbx.exe sections, the new section headers don't have to be patched up.

		// Mark the entire emulated memory range accessible
		DWORD OldProtection;
		VirtualProtect((void*)XBE_IMAGE_BASE, EMU_MAX_MEMORY_SIZE, PAGE_EXECUTE_READWRITE, &OldProtection);

		// Clear out the entire memory range
		memset((void*)ExeDosHeader, 0, EMU_MAX_MEMORY_SIZE);

		// Restore enough of the executable image headers to keep WinAPI's working :
		RestoreExeImageHeader();
	}

	// TODO : Setup LaunchData and Initialize kernel
	// (which will launch the Xbe on itself), instead here.

	// Now we can load and run the XBE :
	// MapAndRunXBE(XbePath, DCHandle);
	{
		// Load Xbe (this will reside above WinMain's emulated_memory_placeholder) 
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
//	MessageBoxA(NULL, "Attach a Debugger", "DEBUG", 0);
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
			printf("[0x%X] EmuMain: Cxbx-Reloaded Version %s\n", GetCurrentThreadId(), _CXBX_VERSION);
			printf("[0x%X] EmuMain: Debug Console Allocated (DM_CONSOLE).\n", GetCurrentThreadId());
		}
	}
	else if (DbgMode == DM_FILE)
	{
		FreeConsole();
		freopen(szDebugFilename, "wt", stdout);
		printf("[0x%X] EmuMain: Cxbx-Reloaded Version %s\n", GetCurrentThreadId(), _CXBX_VERSION);
		printf("[0x%X] EmuMain: Debug Console Allocated (DM_FILE).\n", GetCurrentThreadId());
	}
	else
	{
		char buffer[16];
		FreeConsole();
		if (GetConsoleTitle(buffer, 16) != NULL)
			freopen("nul", "w", stdout);
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
		typedef struct DUMMY_KERNEL
		{
			IMAGE_DOS_HEADER DosHeader;
			DWORD Signature;
			IMAGE_FILE_HEADER FileHeader;
			IMAGE_SECTION_HEADER SectionHeader;
		} *PDUMMY_KERNEL;

		PDUMMY_KERNEL DummyKernel = (PDUMMY_KERNEL)VirtualAlloc(
			(PVOID)XBOX_KERNEL_BASE, sizeof(DUMMY_KERNEL),
			MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE
		);

		if (DummyKernel == NULL)
			CxbxKrnlCleanup("InitializeDummyKernel: Could not allocate dummy kernel.");
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
		if (bLLE_APU)
			DbgPrintf("EmuMain : LLE enabled for APU.\n");

		bLLE_GPU = (CxbxLLE_Flags & LLE_GPU) > 0;
		if (bLLE_GPU)
			DbgPrintf("EmuMain : LLE enabled for GPU.\n");

		bLLE_JIT = (CxbxLLE_Flags & LLE_JIT) > 0;
		if (bLLE_JIT)
			DbgPrintf("EmuMain : LLE enabled for JIT.\n");
	}

	// Initialize devices :
	char szBuffer[MAX_PATH];
	SHGetSpecialFolderPath(NULL, szBuffer, CSIDL_APPDATA, TRUE);
	strcat(szBuffer, "\\Cxbx-Reloaded\\");
	std::string basePath(szBuffer);
	CxbxBasePath = basePath + "EmuDisk\\";
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
		xboxkrnl::XeImageFileName.Buffer = (PCHAR)malloc(MAX_PATH);
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
		GetProcessAffinityMask(g_CurrentProcessHandle, &g_CPUXbox, &g_CPUOthers);
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

		// Make sure Xbox1 code runs on one core :
		SetThreadAffinityMask(GetCurrentThread(), g_CPUXbox);
	}

	// initialize grapchics
	DbgPrintf("EmuMain: Initializing render window.\n");
	XTL::CxbxInitWindow(pXbeHeader, dwXbeHeaderSize);
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

	EmuHLEIntercept(pXbeHeader);
	// Apply Media Patches to bypass Anti-Piracy checks
	// Required until we perfect emulation of X2 DVD Authentication
	// See: https://multimedia.cx/eggs/xbox-sphinx-protocol/
	ApplyMediaPatches();

	// initialize FS segment selector
	{
		EmuInitFS();
		EmuGenerateFS(pTLS, pTLSData);
	}

	EmuX86_Init();
    DbgPrintf("EmuMain: Initial thread starting.\n");
	CxbxLaunchXbe(Entry);
    DbgPrintf("EmuMain: Initial thread ended.\n");
    fflush(stdout);
    CxbxKrnlTerminateThread();
    return;
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

        sprintf(szBuffer1, "[0x%X] EmuMain: Recieved Fatal Message:\n\n* ", GetCurrentThreadId());

        va_start(argp, szErrorMessage);

        vsprintf(szBuffer2, szErrorMessage, argp);

        va_end(argp);

        strcat(szBuffer1, szBuffer2);
        strcat(szBuffer1, "\n");

        printf("%s\n", szBuffer1);

        MessageBox(NULL, szBuffer1, "CxbxKrnl", MB_OK | MB_ICONEXCLAMATION);
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
