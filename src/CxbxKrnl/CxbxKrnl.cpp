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
#include "EmuFS.h"
#include "EmuShared.h"
#include "HLEIntercept.h"

#include <shlobj.h>
#include <clocale>

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
extern CXBXKRNL_API HWND CxbxKrnl_hEmuParent = NULL;

/*! thread handles */
static HANDLE g_hThreads[MAXIMUM_XBOX_THREADS] = { 0 };

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

extern "C" CXBXKRNL_API void CxbxKrnlInit
(
    HWND                    hwndParent,
    void                   *pTLSData,
    Xbe::TLS               *pTLS,
    Xbe::LibraryVersion    *pLibraryVersion,
    DebugMode               DbgMode,
    char                   *szDebugFilename,
    Xbe::Header            *pXbeHeader,
    uint32                  dwXbeHeaderSize,
    void                  (*Entry)())
{
    // update caches
    CxbxKrnl_TLS = pTLS;
    CxbxKrnl_TLSData = pTLSData;
    CxbxKrnl_XbeHeader = pXbeHeader;
    CxbxKrnl_hEmuParent = IsWindow(hwndParent) ? hwndParent : NULL;

    // for unicode conversions
    setlocale(LC_ALL, "English");

    // debug console allocation (if configured)
    if(DbgMode == DM_CONSOLE)
    {
        if(AllocConsole())
        {
            freopen("CONOUT$", "wt", stdout);
            freopen("CONIN$", "rt", stdin);

            SetConsoleTitle("Cxbx : Kernel Debug Console");

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);

            printf("EmuMain (0x%X): Cxbx Version %s\n", GetCurrentThreadId(), _CXBX_VERSION);
            printf("EmuMain (0x%X): Debug Console Allocated (DM_CONSOLE).\n", GetCurrentThreadId());
        }
    }
    else if(DbgMode == DM_FILE)
    {
        FreeConsole();

        freopen(szDebugFilename, "wt", stdout);

        printf("EmuMain (0x%X): Cxbx Version %s\n", GetCurrentThreadId(), _CXBX_VERSION);
        printf("EmuMain (0x%X): Debug Console Allocated (DM_FILE).\n", GetCurrentThreadId());
    }
    else
    {
        FreeConsole();

        char buffer[16];

        if(GetConsoleTitle(buffer, 16) != NULL)
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

    //
    // load the necessary pieces of XbeHeader
    //

    {
        Xbe::Header *MemXbeHeader = (Xbe::Header*)0x00010000;

        uint32 old_protection = 0;

        VirtualProtect(MemXbeHeader, 0x1000, PAGE_READWRITE, &old_protection);

        // we sure hope we aren't corrupting anything necessary for an .exe to survive :]
        MemXbeHeader->dwSizeofHeaders   = pXbeHeader->dwSizeofHeaders;
        MemXbeHeader->dwCertificateAddr = pXbeHeader->dwCertificateAddr;
        MemXbeHeader->dwPeHeapReserve   = pXbeHeader->dwPeHeapReserve;
        MemXbeHeader->dwPeHeapCommit    = pXbeHeader->dwPeHeapCommit;

        memcpy(&MemXbeHeader->dwInitFlags, &pXbeHeader->dwInitFlags, sizeof(pXbeHeader->dwInitFlags));

        memcpy((void*)pXbeHeader->dwCertificateAddr, &((uint08*)pXbeHeader)[pXbeHeader->dwCertificateAddr - 0x00010000], sizeof(Xbe::Certificate));
    }

    //
    // initialize current directory
    //

    {
        char szBuffer[260];

        g_EmuShared->GetXbePath(szBuffer);

        if(szBuffer && *szBuffer)
            SetCurrentDirectory(szBuffer);
        else
            GetCurrentDirectory(260, szBuffer);

        g_strCurDrive = _strdup(szBuffer);

        g_hCurDir = CreateFile(szBuffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

        if(g_hCurDir == INVALID_HANDLE_VALUE)
            CxbxKrnlCleanup("Could not map D:\\\n");

        DbgPrintf("EmuMain (0x%X): CurDir := %s\n", GetCurrentThreadId(), szBuffer);
    }

    //
    // initialize EmuDisk
    //

    {
        char szBuffer[260];

        SHGetSpecialFolderPath(NULL, szBuffer, CSIDL_APPDATA, TRUE);

        strcat(szBuffer, "\\Cxbx\\");

        CreateDirectory(szBuffer, NULL);

        sint32 spot = -1;

        for(int v=0;v<260;v++)
        {
            if(szBuffer[v] == '\\') { spot = v; }
            else if(szBuffer[v] == '\0') { break; }
        }

        if(spot != -1) { szBuffer[spot] = '\0'; }

        Xbe::Certificate *pCertificate = (Xbe::Certificate*)pXbeHeader->dwCertificateAddr;

        //
        // create EmuDisk directory
        //

        strcpy(&szBuffer[spot], "\\EmuDisk");

        CreateDirectory(szBuffer, NULL);

        //
        // create T:\ directory
        //

        {
            strcpy(&szBuffer[spot], "\\EmuDisk\\T");

            CreateDirectory(szBuffer, NULL);

            sprintf(&szBuffer[spot+10], "\\%08x", pCertificate->dwTitleId);

            CreateDirectory(szBuffer, NULL);

            g_strTDrive = _strdup(szBuffer);

            g_hTDrive = CreateFile(szBuffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

            if(g_hTDrive == INVALID_HANDLE_VALUE)
                CxbxKrnlCleanup("Could not map T:\\\n");

            DbgPrintf("EmuMain (0x%X): T Data := %s\n", GetCurrentThreadId(), szBuffer);
        }

        //
        // create U:\ directory
        //

        {
            strcpy(&szBuffer[spot], "\\EmuDisk\\U");

            CreateDirectory(szBuffer, NULL);

            sprintf(&szBuffer[spot+10], "\\%08x", pCertificate->dwTitleId);

            CreateDirectory(szBuffer, NULL);

            g_strUDrive = _strdup(szBuffer);

            g_hUDrive = CreateFile(szBuffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

            if(g_hUDrive == INVALID_HANDLE_VALUE)
                CxbxKrnlCleanup("Could not map U:\\\n");

            DbgPrintf("EmuMain (0x%X): U Data := %s\n", GetCurrentThreadId(), szBuffer);
        }

        //
        // create Z:\ directory
        //

        {
            strcpy(&szBuffer[spot], "\\EmuDisk\\Z");

            CreateDirectory(szBuffer, NULL);

            //* is it necessary to make this directory title unique?
            sprintf(&szBuffer[spot+10], "\\%08x", pCertificate->dwTitleId);

            CreateDirectory(szBuffer, NULL);
            //*/

            g_strZDrive = _strdup(szBuffer);

            g_hZDrive = CreateFile(szBuffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

            if(g_hUDrive == INVALID_HANDLE_VALUE)
                CxbxKrnlCleanup("Could not map Z:\\\n");

            DbgPrintf("EmuMain (0x%X): Z Data := %s\n", GetCurrentThreadId(), szBuffer);
        }
    }

    //
    // initialize FS segment selector
    //

    {
        EmuInitFS();

        EmuGenerateFS(pTLS, pTLSData);
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

    DbgPrintf("EmuMain (0x%X): Initial thread starting.\n", GetCurrentThreadId());

    //
    // Xbe entry point
    //

    __try
    {
        EmuSwapFS();   // XBox FS

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

        EmuSwapFS();   // Win2k/XP FS
    }
    __except(EmuException(GetExceptionInformation()))
    {
        printf("Emu: WARNING!! Problem with ExceptionFilter\n");
    }

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
    if(EmuIsXboxFS())
        EmuSwapFS();    // Win2k/XP FS

    EmuCleanupFS();

    TerminateThread(GetCurrentThread(), 0);
}

extern "C" CXBXKRNL_API void CxbxKrnlPanic()
{
    if(EmuIsXboxFS())
        EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuMain (0x%X): CxbxKrnlPanic()\n", GetCurrentThreadId());

    CxbxKrnlCleanup("Kernel Panic!");

    EmuSwapFS();   // XBox FS
}

extern "C" CXBXKRNL_API void CxbxKrnlNoFunc()
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuMain (0x%X): CxbxKrnlNoFunc()\n", GetCurrentThreadId());

    EmuSwapFS();   // XBox FS
}
