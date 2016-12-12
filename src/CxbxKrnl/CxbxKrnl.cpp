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
#include "EmuFile.h"
#include "EmuFS.h"
#include "EmuShared.h"
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

DWORD_PTR g_CPUXbox = 0;
DWORD_PTR g_CPUOthers = 0;

static uint32 funcAddr[]=
{
    0x001396D1, // -> 0x00139709 (Size : 56 bytes)
    0x00139709, // -> 0x001397B3 (Size : 170 bytes)
    0x001397B3, // -> 0x001397BB (Size : 8 bytes)
    0x001397BB, // -> 0x00139826 (Size : 107 bytes)
    0x00139826, // -> 0x0013989B (Size : 117 bytes)
    0x0013989B, // -> 0x00139AA5 (Size : 522 bytes)
    0x00139AA5, // -> 0x00139C01 (Size : 348 bytes)
    0x00139C01, // -> 0x00139C57 (Size : 86 bytes)
    0x00139C57, // -> 0x00139CD6 (Size : 127 bytes)
//    0x00139CD6, // -> 0x00139CF4 (Size : 30 bytes)
    0x00139CF4, // -> 0x00139D55 (Size : 97 bytes)
    0x00139D55, // -> 0x00139D75 (Size : 32 bytes)
    0x00139D75, // -> 0x00139D93 (Size : 30 bytes)
    0x00139D93, // -> 0x00139DD7 (Size : 68 bytes)
    0x00139DD7, // -> 0x00139E57 (Size : 128 bytes)
    0x00139E57, // -> 0x00139EA9 (Size : 82 bytes)
    0x00139EA9, // -> 0x00139EBB (Size : 18 bytes)
    0x00139EBB, // -> 0x00139EC9 (Size : 14 bytes)
    0x00139EC9, // -> 0x00139FB6 (Size : 237 bytes)
    0x00139FB6, // -> 0x0013A08C (Size : 214 bytes)
    0x0013A08C, // -> 0x0013A110 (Size : 132 bytes)
    0x0013A110, // -> 0x0013A20A (Size : 250 bytes)
    0x0013A20A, // -> 0x0013A231 (Size : 39 bytes)
    0x0013A231, // -> 0x0013A26F (Size : 62 bytes)
    0x0013A26F, // -> 0x0013A2C0 (Size : 81 bytes)
    0x0013A2C0, // -> 0x0013A2FB (Size : 59 bytes)
    0x0013A2FB, // -> 0x0013A486 (Size : 395 bytes)
    0x0013A486, // -> 0x0013A768 (Size : 738 bytes)
    0x0013A768, // -> 0x0013A787 (Size : 31 bytes)
    0x0013A787, // -> 0x0013A7AF (Size : 40 bytes)
    0x0013A7AF, // -> 0x0013A7DD (Size : 46 bytes)
    0x0013A7DD, // -> 0x0013A7F4 (Size : 23 bytes)
    0x0013A7F4, // -> 0x0013A859 (Size : 101 bytes)
    0x0013A859, // -> 0x0013A8BD (Size : 100 bytes)
    0x0013A8BD, // -> 0x0013A902 (Size : 69 bytes)
//    0x0013A902, // -> 0x0013A940 (Size : 62 bytes)
    0x0013A940, // -> 0x0013A9A5 (Size : 101 bytes)
    0x0013A9A5, // -> 0x0013A9FF (Size : 90 bytes)
    0x0013A9FF, // -> 0x0013AA3D (Size : 62 bytes)
    0x0013AA3D, // -> 0x0013AAE3 (Size : 166 bytes)
    0x0013AAE3, // -> 0x0013AB71 (Size : 142 bytes)
    0x0013AB71, // -> 0x0013AB93 (Size : 34 bytes)
    0x0013AB93, // -> 0x0013ABB8 (Size : 37 bytes)
    0x0013ABB8, // -> 0x0013ABC0 (Size : 8 bytes)
    0x0013ABC0, // -> 0x0013ABE9 (Size : 41 bytes)
    0x0013ABE9, // -> 0x0013AC35 (Size : 76 bytes)
    0x0013AC35, // -> 0x0013AC79 (Size : 68 bytes)
    0x0013AC79, // -> 0x0013AC9E (Size : 37 bytes)
    0x0013AC9E, // -> 0x0013ACA3 (Size : 5 bytes)
    0x0013ACA3, // -> 0x0013ACBA (Size : 23 bytes)
    0x0013ACBA, // -> 0x0013ACCD (Size : 19 bytes)
    0x0013ACCD, // -> 0x0013ADA7 (Size : 218 bytes)
    0x0013ADA7, // -> 0x0013ADEC (Size : 69 bytes)
    0x0013ADEC, // -> 0x0013AEFF (Size : 275 bytes)
    0x0013AEFF, // -> 0x0013AF6B (Size : 108 bytes)
    0x0013AF6B, // -> 0x0013AFF9 (Size : 142 bytes)
    0x0013AFF9, // -> 0x0013B065 (Size : 108 bytes)
    0x0013B065, // -> 0x0013B172 (Size : 269 bytes)
    0x0013B172, // -> 0x0013B1C3 (Size : 81 bytes)
    0x0013B1C3, // -> 0x0013B1F5 (Size : 50 bytes)
    0x0013B1F5, // -> 0x0013B257 (Size : 98 bytes)
    0x0013B257, // -> 0x0013B2C9 (Size : 114 bytes)
    0x0013B2C9, // -> 0x0013B304 (Size : 59 bytes)
    0x0013B304, // -> 0x0013B37F (Size : 123 bytes)
    0x0013B37F, // -> 0x0013B399 (Size : 26 bytes)
    0x0013B399, // -> 0x0013B3F6 (Size : 93 bytes)
    0x0013B3F6, // -> 0x0013B40A (Size : 20 bytes)
    0x0013B40A, // -> 0x0013B4FE (Size : 244 bytes)
    0x0013B4FE, // -> 0x0013B702 (Size : 516 bytes)
    0x0013B702, // -> 0x0013B7ED (Size : 235 bytes)
    0x0013B7ED, // -> 0x0013B84A (Size : 93 bytes)
    0x0013B84A, // -> 0x0013B8E9 (Size : 159 bytes)
    0x0013B8E9, // -> 0x0013B8FD (Size : 20 bytes)
    0x0013B8FD, // -> 0x0013B98F (Size : 146 bytes)
    0x0013B98F, // -> 0x0013B9D3 (Size : 68 bytes)
    0x0013B9D3, // -> 0x0013BB5E (Size : 395 bytes)
    0x0013BB5E, // -> 0x0013BBDC (Size : 126 bytes)
    0x0013BBDC, // -> 0x0013BC38 (Size : 92 bytes)
    0x0013BC38, // -> 0x0013BE9B (Size : 611 bytes)
};

extern "C" CXBXKRNL_API bool CxbxKrnlVerifyVersion(const char *szVersion)
{
    if(strcmp(szVersion, _CXBX_VERSION) != 0)
        return false;

    return true;
}


void CxbxLaunchXbe(void(*Entry)())
{
	//
	// Xbe entry point
	//

	__try
	{
					   // _USE_XGMATH Disabled in mesh :[
					   // halo : dword_0_2E2D18
					   // halo : 1744F0 (bink)
					   //_asm int 3;

					   /*
					   for(int v=0;v<sizeof(funcAddr)/sizeof(uint32);v++)
					   {
					   bool bExclude = false;
					   for(int r=0;r<sizeof(funcExclude)/sizeof(uint32);r++)
					   {
					   if(funcAddr[v] == funcExclude[r])
					   {
					   bExclude = true;
					   break;
					   }
					   }

					   if(!bExclude)
					   {
					   *(uint08*)(funcAddr[v]) = 0xCC;
					   }
					   }
					   //*/

		Entry();

		
	}
	__except (EmuException(GetExceptionInformation()))
	{
		printf("Emu: WARNING!! Problem with ExceptionFilter\n");
	}

}

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
	

	// Read EXE Header Data
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
		if (ExeSectionHeaders[i].m_virtual_addr + ExeSectionHeaders[i].m_virtual_size < XBOX_MEMORY_SIZE) {
			char* NewSection = (char*)malloc(ExeSectionHeaders[i].m_virtual_size);
			memcpy(NewSection, (void*)(ExeSectionHeaders[i].m_virtual_addr + (uint32)ExeDosHeader), ExeSectionHeaders[i].m_virtual_size);
			NewSectionHeaders[i].m_virtual_addr = (uint32)NewSection - (uint32)ExeDosHeader;
			

			if (NewOptionalHeader->m_data_base == ExeSectionHeaders[i].m_virtual_addr) {
				NewOptionalHeader->m_data_base = NewSectionHeaders[i].m_virtual_addr;
			}
		}
	}



	DWORD OldProtection;
	VirtualProtect((void*)0x10000, XBOX_MEMORY_SIZE, PAGE_EXECUTE_READWRITE, &OldProtection);

	// TODO TLS


	// Clear out entire memory range
	ZeroMemory((void*)0x10000, XBOX_MEMORY_SIZE);

	g_EmuShared->SetXbePath(xbePath.c_str());
	CxbxKrnl_Xbe = new Xbe(xbePath.c_str());
	
	// Load Xbe Headers
	memcpy((void*)CxbxKrnl_Xbe->m_Header.dwBaseAddr, &CxbxKrnl_Xbe->m_Header, sizeof(Xbe::Header));	
	memcpy((void*)(CxbxKrnl_Xbe->m_Header.dwBaseAddr + sizeof(Xbe::Header)), CxbxKrnl_Xbe->m_HeaderEx, CxbxKrnl_Xbe->m_ExSize);
	
	// Load Sections
	for (uint32 i = 0; i < CxbxKrnl_Xbe->m_Header.dwSections; i++) {
		memcpy((void*)CxbxKrnl_Xbe->m_SectionHeader[i].dwVirtualAddr, CxbxKrnl_Xbe->m_bzSection[i], CxbxKrnl_Xbe->m_SectionHeader[i].dwSizeofRaw);
	}

	ConnectWindowsTimersToThunkTable();

	// Fixup Kernel Imports
	uint32 kt = CxbxKrnl_Xbe->m_Header.dwKernelImageThunkAddr;

	if ((kt ^ XOR_KT_DEBUG) > 0x01000000)
		kt ^= XOR_KT_RETAIL;
	else
		kt ^= XOR_KT_DEBUG;

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
	if ((EntryPoint ^ XOR_EP_DEBUG) > 0x01000000)
		EntryPoint ^= XOR_EP_RETAIL;
	else
		EntryPoint ^= XOR_EP_DEBUG;

	// Restore the area of the EXE required for WinAPI
	ExeDosHeader->m_magic = NewDosHeader->m_magic;
	ExeDosHeader->m_lfanew = NewDosHeader->m_lfanew;

	// Launch XBE
	CxbxKrnlInit(
		hWnd, XbeTlsData, XbeTls, CxbxKrnl_Xbe->m_LibraryVersion, DbgMode, 
		DebugFileName.c_str(), (Xbe::Header*)0x10000, CxbxKrnl_Xbe->m_Header.dwSizeofHeaders, (void(*)())EntryPoint
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

#ifdef _DEBUG
//	MessageBoxA(NULL, "Attach a Debugger", "DEBUG", 0);
//  Debug child processes using https://marketplace.visualstudio.com/items?itemName=GreggMiskelly.MicrosoftChildProcessDebuggingPowerTool
#endif

	// debug console allocation (if configured)
	if (DbgMode == DM_CONSOLE)
	{
		if (AllocConsole())
		{
			freopen("CONOUT$", "wt", stdout);
			freopen("CONIN$", "rt", stdin);

			SetConsoleTitle("Cxbx-Reloaded : Kernel Debug Console");

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);

			printf("EmuMain (0x%X): Cxbx-Reloaded Version %s\n", GetCurrentThreadId(), _CXBX_VERSION);
			printf("EmuMain (0x%X): Debug Console Allocated (DM_CONSOLE).\n", GetCurrentThreadId());
		}
	}
	else if (DbgMode == DM_FILE)
	{
		FreeConsole();

		freopen(szDebugFilename, "wt", stdout);

		printf("EmuMain (0x%X): Cxbx-Reloaded Version %s\n", GetCurrentThreadId(), _CXBX_VERSION);
		printf("EmuMain (0x%X): Debug Console Allocated (DM_FILE).\n", GetCurrentThreadId());
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
		printf("EmuMain (0x%X): Debug Trace Enabled.\n", GetCurrentThreadId());

		printf("EmuMain (0x%X): CxbxKrnlInit\n"
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
		printf("EmuMain (0x%X): Debug Trace Disabled.\n", GetCurrentThreadId());
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

#define XBOX_KERNEL_BASE 0x80010000
#define XBOX_NV2A_INIT_VECTOR 0xFF000008

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

	// Initialize devices :
	char szBuffer[MAX_PATH];
	SHGetSpecialFolderPath(NULL, szBuffer, CSIDL_APPDATA, TRUE);
	strcat(szBuffer, "\\Cxbx-Reloaded\\");

	std::string basePath(szBuffer);
	CxbxBasePath = basePath + "\\EmuDisk\\";

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
	CxbxRegisterDeviceNativePath(DeviceCdrom0, xbeDirectory);

	// Partition 0 contains configuration data, and is accessed as a native file, instead as a folder :
	CxbxRegisterDeviceNativePath(DeviceHarddisk0Partition0, CxbxBasePath + "Partition0", true); /*IsFile=*/
	// The first two partitions are for Data and Shell files, respectively :
	CxbxRegisterDeviceNativePath(DeviceHarddisk0Partition1, CxbxBasePath + "Partition1");
	CxbxRegisterDeviceNativePath(DeviceHarddisk0Partition2, CxbxBasePath + "Partition2");
	// The following partitions are for caching purposes - for now we allocate up to 7 (as xbmp needs that many) :
	CxbxRegisterDeviceNativePath(DeviceHarddisk0Partition3, CxbxBasePath + "Partition3");
	CxbxRegisterDeviceNativePath(DeviceHarddisk0Partition4, CxbxBasePath + "Partition4");
	CxbxRegisterDeviceNativePath(DeviceHarddisk0Partition5, CxbxBasePath + "Partition5");
	CxbxRegisterDeviceNativePath(DeviceHarddisk0Partition6, CxbxBasePath + "Partition6");
	CxbxRegisterDeviceNativePath(DeviceHarddisk0Partition7, CxbxBasePath + "Partition7");


	DbgPrintf("EmuMain : Creating default symbolic links.\n");

	// Create default symbolic links :
	{
		// Arrange that the Xbe path can reside outside the partitions, and put it to g_hCurDir :
		CxbxCreateSymbolicLink(DriveC, xbeDirectory);
		EmuNtSymbolicLinkObject* xbePathSymbolicLinkObject = FindNtSymbolicLinkObjectByVolumeLetter(CxbxDefaultXbeVolumeLetter);
		g_hCurDir = xbePathSymbolicLinkObject->RootDirectoryHandle;

		// Determine Xbox path to XBE and place it in XeImageFileName
		std::string fileName(xbePath);
		xboxkrnl::XeImageFileName.Buffer = (PCHAR)malloc(MAX_PATH);
		sprintf(xboxkrnl::XeImageFileName.Buffer, "%c:\\%s", CxbxDefaultXbeVolumeLetter, fileName.c_str());
		xboxkrnl::XeImageFileName.Length = (USHORT)strlen(xboxkrnl::XeImageFileName.Buffer);
		xboxkrnl::XeImageFileName.MaximumLength = MAX_PATH;

		DbgPrintf("EmuMain : XeImageFileName = %s\n", xboxkrnl::XeImageFileName.Buffer);

		CxbxCreateSymbolicLink(DriveD, DeviceCdrom0); // CdRom goes to D:
		CxbxCreateSymbolicLink(DriveE, DeviceHarddisk0Partition1); // Partition1 goes to E: (Data files, savegames, etc.)
		CxbxCreateSymbolicLink(DriveF, DeviceHarddisk0Partition2); // Partition2 goes to F: (Shell files, dashboard, etc.)
		CxbxCreateSymbolicLink(DriveT, DeviceHarddisk0Partition1 + "\\TDATA\\" + titleId + "\\"); // Partition1\Title data goes to T:
		CxbxCreateSymbolicLink(DriveU, DeviceHarddisk0Partition1 + "\\UDATA\\" + titleId + "\\"); // Partition1\User data goes to U:
		CxbxCreateSymbolicLink(DriveX, DeviceHarddisk0Partition3); // Partition3 goes to X:
		CxbxCreateSymbolicLink(DriveY, DeviceHarddisk0Partition4); // Partition4 goes to Y:

		// Mount the Utility drive (Z:) conditionally :
		if (CxbxKrnl_XbeHeader->dwInitFlags.bMountUtilityDrive)
			CxbxMountUtilityDrive(CxbxKrnl_XbeHeader->dwInitFlags.bFormatUtilityDrive);/*fFormatClean=*/
	}

	//
	// duplicate handle in order to retain Suspend/Resume thread rights from a remote thread
	//

	{
		HANDLE hDupHandle = NULL;

		DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &hDupHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);

		CxbxKrnlRegisterThread(hDupHandle);
	}

	//
	// Clear critical section list
	//
	//extern void InitializeSectionStructures(void); 
	InitializeSectionStructures();

	DbgPrintf("EmuMain (0x%X): Initializing Direct3D.\n", GetCurrentThreadId());

	XTL::EmuD3DInit(pXbeHeader, dwXbeHeaderSize);

	EmuHLEIntercept(pLibraryVersion, pXbeHeader);

	//
	// initialize FS segment selector
	//

	{
		EmuInitFS();

		EmuGenerateFS(pTLS, pTLSData);
	}

	
	DbgPrintf("EmuMain : Determining CPU affinity.\n");

	// Make sure the Xbox1 code runs on one core (as the box itself has only 1 CPU,
	// this will better aproximate the environment with regard to multi-threading) :
	{
		GetProcessAffinityMask(GetCurrentProcess(), &g_CPUXbox, &g_CPUOthers);
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

    DbgPrintf("EmuMain (0x%X): Initial thread starting.\n", GetCurrentThreadId());

	CxbxLaunchXbe(Entry);

    DbgPrintf("EmuMain (0x%X): Initial thread ended.\n", GetCurrentThreadId());

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

        sprintf(szBuffer1, "EmuMain (0x%X): Recieved Fatal Message:\n\n* ", GetCurrentThreadId());

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

    TerminateProcess(GetCurrentProcess(), 0);

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
    DbgPrintf("EmuMain (0x%X): CxbxKrnlPanic()\n", GetCurrentThreadId());

    CxbxKrnlCleanup("Kernel Panic!");
}

extern "C" CXBXKRNL_API void CxbxKrnlNoFunc()
{
    DbgPrintf("EmuMain (0x%X): CxbxKrnlNoFunc()\n", GetCurrentThreadId());
}
