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
#include "Emu.h"
#include "EmuX86.h"
#include "EmuFile.h"
#include "EmuFS.h"
#include "EmuShared.h"
#include "EmuNV2A.h" // For InitOpenGLContext
#include "HLEIntercept.h"
#include "Exe.h"

#include <shlobj.h>
#include <clocale>
#include <Shlwapi.h>

/* prevent name collisions */
namespace NtDll
{
    #include "EmuNtDll.h"
};

/*! thread local storage */
extern CXBXKRNL_API Xbe::TLS *CxbxKrnl_TLS = NULL;
/*! thread local storage data */
extern CXBXKRNL_API void *CxbxKrnl_TLSData = NULL;
/*! xbe header structure */
extern CXBXKRNL_API Xbe::Header *CxbxKrnl_XbeHeader = NULL;
/*! parent window handle */

CXBXKRNL_API HWND CxbxKrnl_hEmuParent = NULL;
CXBXKRNL_API DebugMode CxbxKrnl_DebugMode = DebugMode::DM_NONE;
CXBXKRNL_API char* CxbxKrnl_DebugFileName = NULL;

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

extern "C" CXBXKRNL_API bool CxbxKrnlVerifyVersion(const char *szVersion)
{
    if(strcmp(szVersion, _CXBX_VERSION) != 0)
        return false;

    return true;
}

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

extern "C" CXBXKRNL_API void CxbxKrnlMain(int argc, char* argv[])
{
	std::string xbePath = argv[2];

	HWND hWnd = 0;
	if (argc > 2) {
		hWnd = (HWND)StrToInt(argv[3]);
	}
	
	DebugMode DbgMode = DebugMode::DM_NONE;
	if (argc > 3) {
		DbgMode = (DebugMode)StrToInt(argv[4]);
	}

	std::string DebugFileName;
	if (argc > 4) {
		DebugFileName = argv[5];
	}
	

	// Read host EXE Header Data
	Exe::DOSHeader* ExeDosHeader = (Exe::DOSHeader*)0x10000;
	Exe::Header* ExeNtHeader = (Exe::Header*)((uint32)ExeDosHeader + ExeDosHeader->m_lfanew);
	Exe::OptionalHeader* ExeOptionalHeader = (Exe::OptionalHeader*)((uint32)ExeNtHeader + sizeof(Exe::Header));
	DWORD ExeHeaderSize = ExeOptionalHeader->m_sizeof_headers;

	// Create a safe copy of the complete EXE header:
	Exe::DOSHeader* NewDosHeader = (Exe::DOSHeader*)malloc(ExeHeaderSize);
	Exe::Header* NewNtHeader = (Exe::Header*)((uint32)NewDosHeader + ExeDosHeader->m_lfanew);
	Exe::OptionalHeader* NewOptionalHeader = (Exe::OptionalHeader*)((uint32)NewNtHeader + sizeof(Exe::Header));
	memcpy(NewDosHeader, ExeDosHeader, ExeHeaderSize);

	// Make sure the new DOS header points to the new relative NtHeader location:
	NewDosHeader->m_lfanew = (uint32)NewNtHeader - 0x10000;

	// Create safe copies of all EXE sections that could get overwritten:
	Exe::SectionHeader* ExeSectionHeaders = (Exe::SectionHeader*)((uint32)ExeOptionalHeader + ExeNtHeader->m_sizeof_optional_header);
	Exe::SectionHeader* NewSectionHeaders = (Exe::SectionHeader*)((uint32)NewOptionalHeader + NewNtHeader->m_sizeof_optional_header);

	for (int i = 0; i < ExeNtHeader->m_sections; i++)
	{
		// Check if this section will be overwritten:
		if (ExeSectionHeaders[i].m_virtual_addr + ExeSectionHeaders[i].m_virtual_size < EMU_MAX_MEMORY_SIZE) {
			char* NewSection = (char*)malloc(ExeSectionHeaders[i].m_virtual_size);
			memcpy(NewSection, (void*)(ExeSectionHeaders[i].m_virtual_addr + (uint32)ExeDosHeader), ExeSectionHeaders[i].m_virtual_size);
			NewSectionHeaders[i].m_virtual_addr = (uint32)NewSection - (uint32)ExeDosHeader;
			

			if (NewOptionalHeader->m_data_base == ExeSectionHeaders[i].m_virtual_addr) {
				NewOptionalHeader->m_data_base = NewSectionHeaders[i].m_virtual_addr;
			}
		}
	}

	// Load Xbe (this will reside above WinMain's emulated_memory_placeholder) 
	g_EmuShared->SetXbePath(xbePath.c_str());
	CxbxKrnl_Xbe = new Xbe(xbePath.c_str());
	
	// Detect XBE type :
	g_XbeType = GetXbeType(&CxbxKrnl_Xbe->m_Header);

	// Register if we're running an Chihiro executable (otherwise it's an Xbox executable)
	g_bIsChihiro = (g_XbeType == xtChihiro);

	// Determine memory size accordingly :
	SIZE_T memorySize = (g_bIsChihiro ? CHIHRO_MEMORY_SIZE : XBOX_MEMORY_SIZE);

	// Mark the entire emulated memory range accessable
	DWORD OldProtection;
	VirtualProtect((void*)XBOX_BASE_ADDR, memorySize - XBOX_BASE_ADDR, PAGE_EXECUTE_READWRITE, &OldProtection);

	// Clear out entire memory range
	ZeroMemory((void*)XBOX_BASE_ADDR, memorySize - XBOX_BASE_ADDR);

	// Copy over loaded Xbe Header to specified base address
	memcpy((void*)CxbxKrnl_Xbe->m_Header.dwBaseAddr, &CxbxKrnl_Xbe->m_Header, sizeof(Xbe::Header));	
	memcpy((void*)(CxbxKrnl_Xbe->m_Header.dwBaseAddr + sizeof(Xbe::Header)), CxbxKrnl_Xbe->m_HeaderEx, CxbxKrnl_Xbe->m_ExSize);
	
	// Load Sections
	for (uint32 i = 0; i < CxbxKrnl_Xbe->m_Header.dwSections; i++) {
		memcpy((void*)CxbxKrnl_Xbe->m_SectionHeader[i].dwVirtualAddr, CxbxKrnl_Xbe->m_bzSection[i], CxbxKrnl_Xbe->m_SectionHeader[i].dwSizeofRaw);
	}

	ConnectWindowsTimersToThunkTable();

	// Fixup Kernel Imports

	uint32 kt = CxbxKrnl_Xbe->m_Header.dwKernelImageThunkAddr;
	kt ^= XOR_KT_KEY[g_XbeType];

	uint32_t* kt_tbl = (uint32_t*)kt;

	int i = 0;
	while (kt_tbl[i] != 0) {
		int t = kt_tbl[i] & 0x7FFFFFFF;
		kt_tbl[i] = CxbxKrnl_KernelThunkTable[t];
		i++;
	}

	// Load TLS
	Xbe::TLS* XbeTls = (Xbe::TLS*)CxbxKrnl_Xbe->m_Header.dwTLSAddr;
	void* XbeTlsData = nullptr;

	if (XbeTls != nullptr) {
		XbeTlsData = (void*)CxbxKrnl_Xbe->m_TLS->dwDataStartAddr;
	}

	// Decode Entry Point
	uint32_t EntryPoint = CxbxKrnl_Xbe->m_Header.dwEntryAddr;
	EntryPoint ^= XOR_EP_KEY[g_XbeType];

	// Restore the area of the EXE required for WinAPI
	ExeDosHeader->m_magic = NewDosHeader->m_magic;
	ExeDosHeader->m_lfanew = NewDosHeader->m_lfanew;

	// Launch XBE
	CxbxKrnlInit(
		hWnd, XbeTlsData, XbeTls, CxbxKrnl_Xbe->m_LibraryVersion, DbgMode, 
		DebugFileName.c_str(), (Xbe::Header*)CxbxKrnl_Xbe->m_Header.dwBaseAddr, CxbxKrnl_Xbe->m_Header.dwSizeofHeaders, (void(*)())EntryPoint
	);
}

extern "C" CXBXKRNL_API void CxbxKrnlInit
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
		FreeConsole();

		char buffer[16];

		if (GetConsoleTitle(buffer, 16) != NULL)
			freopen("nul", "w", stdout);
	}

	//
	// debug trace
	//

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
	int CxbxLLE_Flags;
	g_EmuShared->GetFlagsLLE(&CxbxLLE_Flags);
	bLLE_APU = (CxbxLLE_Flags & LLE_APU) > 0;
	bLLE_GPU = (CxbxLLE_Flags & LLE_GPU) > 0;
	bLLE_JIT = (CxbxLLE_Flags & LLE_JIT) > 0;

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


	DbgPrintf("EmuMain : Creating default symbolic links.\n");

	// Create default symbolic links :
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
		if (fileName.rfind('\\') >= 0)
			fileName = fileName.substr(fileName.rfind('\\') + 1);

		if (xboxkrnl::XeImageFileName.Buffer != NULL)
			free(xboxkrnl::XeImageFileName.Buffer);

		xboxkrnl::XeImageFileName.MaximumLength = MAX_PATH;
		xboxkrnl::XeImageFileName.Buffer = (PCHAR)malloc(MAX_PATH);
		sprintf(xboxkrnl::XeImageFileName.Buffer, "%c:\\%s", CxbxDefaultXbeDriveLetter, fileName.c_str());
		xboxkrnl::XeImageFileName.Length = (USHORT)strlen(xboxkrnl::XeImageFileName.Buffer);

		DbgPrintf("EmuMain : XeImageFileName = %s\n", xboxkrnl::XeImageFileName.Buffer);

	}

	//
	// duplicate handle in order to retain Suspend/Resume thread rights from a remote thread
	//

	{
		HANDLE hDupHandle = NULL;

		DuplicateHandle(g_CurrentProcessHandle, GetCurrentThread(), g_CurrentProcessHandle, &hDupHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);

		CxbxKrnlRegisterThread(hDupHandle);
	}

	//
	// Clear critical section list
	//
	//extern void InitializeSectionStructures(void); 
	InitializeSectionStructures();


	DbgPrintf("EmuMain : Determining CPU affinity.\n");

	// Make sure the Xbox1 code runs on one core (as the box itself has only 1 CPU,
	// this will better aproximate the environment with regard to multi-threading) :
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

	//
	// initialize grapchics
	//
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
		XTL::EmuD3DInit(pXbeHeader, dwXbeHeaderSize);
	}

	EmuHLEIntercept(pLibraryVersion, pXbeHeader);

	//
	// initialize FS segment selector
	//

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

extern "C" CXBXKRNL_API void CxbxKrnlCleanup(const char *szErrorMessage, ...)
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

extern "C" CXBXKRNL_API void CxbxKrnlRegisterThread(HANDLE hThread)
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

extern "C" CXBXKRNL_API void CxbxKrnlSuspend()
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

extern "C" CXBXKRNL_API void CxbxKrnlResume()
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

extern "C" CXBXKRNL_API void CxbxKrnlTerminateThread()
{
    TerminateThread(GetCurrentThread(), 0);
}

extern "C" CXBXKRNL_API void CxbxKrnlPanic()
{
    DbgPrintf("EmuMain: CxbxKrnlPanic()\n");

    CxbxKrnlCleanup("Kernel Panic!");
}

extern "C" CXBXKRNL_API void CxbxKrnlNoFunc()
{
    DbgPrintf("EmuMain: CxbxKrnlNoFunc()\n");
}
