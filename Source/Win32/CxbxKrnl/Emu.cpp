// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->Emu.cpp
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

// prevent name collisions
namespace xboxkrnl
{
    #include <xboxkrnl/xboxkrnl.h>
};

#define COMPILE_MULTIMON_STUBS
#include "Emu.h"
#include "EmuFS.h"
#include "EmuAlloc.h"

// prevent name collisions
namespace NtDll
{
    #include "EmuNtDll.h"
};

#include "EmuXTL.h"
#include "EmuShared.h"
#include "HLEDataBase.h"
#include "HLEIntercept.h"

#include <shlobj.h>
#include <locale.h>

// Global Variable(s)
Xbe::TLS        *g_pTLS       = NULL;
void            *g_pTLSData   = NULL;
Xbe::Header     *g_pXbeHeader = NULL;
HANDLE           g_hCurDir    = NULL;
HANDLE           g_hTDrive    = NULL;
HANDLE           g_hUDrive    = NULL;
HANDLE           g_hZDrive    = NULL;
volatile bool    g_bEmuException = false;
volatile bool    g_bEmuSuspended = false;
volatile bool    g_bPrintfOn = true;

// global exception patching address
uint32 g_HaloHack[4] = {0};

// Static Function(s)
static int ExitException(LPEXCEPTION_POINTERS e);

// Static Variable(s)
static HANDLE g_hThreads[MAXIMUM_XBOX_THREADS] = {0};

// Dll entry point, exit point, ...
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    static HINSTANCE hInitInstance = NULL;

    if(fdwReason == DLL_PROCESS_ATTACH)
    {
        EmuShared::Init();
        hInitInstance = hinstDLL;
    }

    if(fdwReason == DLL_PROCESS_DETACH)
    {
        if(hInitInstance == hinstDLL)
            EmuShared::Cleanup();
    }

    return TRUE;
}

// pointless function
extern "C" CXBXKRNL_API void NTAPI EmuNoFunc()
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuMain (0x%X): EmuNoFunc()\n", GetCurrentThreadId());

    EmuSwapFS();   // XBox FS
}

// verify szVersion matches the current build version
extern "C" CXBXKRNL_API bool NTAPI EmuVerifyVersion(const char *szVersion)
{
    if(strcmp(szVersion, _CXBX_VERSION) != 0)
        return false;

    return true;
}

// cleanup and terminate the current thread
extern "C" CXBXKRNL_API void NTAPI EmuCleanThread()
{
    if(EmuIsXboxFS())
        EmuSwapFS();    // Win2k/XP FS

    EmuCleanupFS();

    TerminateThread(GetCurrentThread(), 0);
}

// initialize emulation
extern "C" CXBXKRNL_API void NTAPI EmuInit
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
    g_pTLS       = pTLS;
    g_pTLSData   = pTLSData;
    g_pXbeHeader = pXbeHeader;
    g_hEmuParent = IsWindow(hwndParent) ? hwndParent : NULL;

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

        printf("EmuMain (0x%X): EmuInit\n"
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

        g_hCurDir = CreateFile(szBuffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

        if(g_hCurDir == INVALID_HANDLE_VALUE)
            EmuCleanup("Could not map D:\\\n");

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

            g_hTDrive = CreateFile(szBuffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

            if(g_hTDrive == INVALID_HANDLE_VALUE)
                EmuCleanup("Could not map T:\\\n");

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

            g_hUDrive = CreateFile(szBuffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

            if(g_hUDrive == INVALID_HANDLE_VALUE)
                EmuCleanup("Could not map U:\\\n");

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

            g_hZDrive = CreateFile(szBuffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

            if(g_hUDrive == INVALID_HANDLE_VALUE)
                EmuCleanup("Could not map Z:\\\n");

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

        EmuRegisterThread(hDupHandle);
    }

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

    EmuCleanThread();

    return;
}

// print out a warning message to the kernel debug log file
#ifdef _DEBUG_WARNINGS
extern "C" CXBXKRNL_API void NTAPI EmuWarning(const char *szWarningMessage, ...)
{
    if(szWarningMessage == NULL)
        return;

    char szBuffer1[255];
    char szBuffer2[255];

    va_list argp;

    sprintf(szBuffer1, "EmuWarn (0x%X): ", GetCurrentThreadId());

    va_start(argp, szWarningMessage);

    vsprintf(szBuffer2, szWarningMessage, argp);

    va_end(argp);

    strcat(szBuffer1, szBuffer2);

    if(g_bPrintfOn)
    {
        printf("%s\n", szBuffer1);
    }

    fflush(stdout);

    return;
}
#endif

// cleanup emulation
extern "C" CXBXKRNL_API void NTAPI EmuCleanup(const char *szErrorMessage, ...)
{
    g_bEmuException = true;

    EmuResume();

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

        MessageBox(NULL, szBuffer1, "CxbxKrnl", MB_OK | MB_ICONSTOP);
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

    if(g_hEmuParent != NULL)
        SendMessage(g_hEmuParent, WM_PARENTNOTIFY, WM_DESTROY, 0);

    TerminateProcess(GetCurrentProcess(), 0);

    return;
}

// kernel function not implemented, panic
extern "C" CXBXKRNL_API void NTAPI EmuPanic()
{
    if(EmuIsXboxFS())
        EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuMain (0x%X): EmuPanic()\n", GetCurrentThreadId());

    EmuCleanup("Kernel Panic!");

    EmuSwapFS();   // XBox FS
}

// register a thread handle with the emulation thread management
extern "C" CXBXKRNL_API void NTAPI EmuRegisterThread(HANDLE hThread)
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
        EmuCleanup("There are too many active threads!");
}

// suspend all threads that have been created with PsCreateSystemThreadEx
extern "C" CXBXKRNL_API void NTAPI EmuSuspend()
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

        HWND hWnd = (g_hEmuParent != NULL) ? g_hEmuParent : g_hEmuWindow;

        GetWindowText(hWnd, szBuffer, 255 - 10);

        strcat(szBuffer, " (paused)");
        SetWindowText(hWnd, szBuffer);
    }

    g_bEmuSuspended = true;
}

// resume all threads that have been created with PsCreateSystemThreadEx
extern "C" CXBXKRNL_API void NTAPI EmuResume()
{
    if(!g_bEmuSuspended)
        return;

    // remove 'paused' from rendering window caption text
    {
        char szBuffer[256];

        HWND hWnd = (g_hEmuParent != NULL) ? g_hEmuParent : g_hEmuWindow;

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

// exception handler
extern int EmuException(LPEXCEPTION_POINTERS e)
{
    if(EmuIsXboxFS())
        EmuSwapFS();

    g_bEmuException = true;

    // check for Halo hack
    {
        if(e->ExceptionRecord->ExceptionCode == 0xC0000005)
        {
            // Halo Access Adjust 1
            if(e->ContextRecord->Eip == 0x0003394C)
            {
                if(e->ContextRecord->Ecx == 0x803BD800)
                {
                    // Halo BINK skip
                    {
                        // nop sled over bink calls
                        /*
                        memset((void*)0x2CBA4, 0x90, 0x2CBAF - 0x2CBA4);
                        memset((void*)0x2CBBD, 0x90, 0x2CBD5 - 0x2CBBD);
                        //*/
                        memset((void*)0x2CAE0, 0x90, 0x2CE1E - 0x2CAE0);
                    }

                    uint32 fix = g_HaloHack[1] + (e->ContextRecord->Eax - 0x803A6000);

                    e->ContextRecord->Eax = e->ContextRecord->Ecx = fix;

                    *(uint32*)e->ContextRecord->Esp = fix;

                    ((XTL::X_D3DResource*)fix)->Data = g_HaloHack[1] + (((XTL::X_D3DResource*)fix)->Data - 0x803A6000);

                    // go through and fix any other pointers in the ESI allocation chunk
                    {
                        DWORD dwESI = e->ContextRecord->Esi;
                        DWORD dwSize = EmuCheckAllocationSize((PVOID)dwESI, false);

                        // dword aligned
                        dwSize -= 4 - dwSize%4;

                        for(DWORD v=0;v<dwSize;v+=4)
                        {
                            DWORD dwCur = *(DWORD*)(dwESI+v);

                            if(dwCur >= 0x803A6000 && dwCur < 0x819A6000)
                                *(DWORD*)(dwESI+v) = g_HaloHack[1] + (dwCur - 0x803A6000);
                        }
                    }

                    // fix this global pointer
                    {
                        DWORD dwValue = *(DWORD*)0x39CE24;

                        *(DWORD*)0x39CE24 = g_HaloHack[1] + (dwValue - 0x803A6000);
                    }

                    DbgPrintf("EmuMain (0x%X): Halo Access Adjust 1 was applied!\n", GetCurrentThreadId());

                    g_bEmuException = false;

                    return EXCEPTION_CONTINUE_EXECUTION;
                }
            }
            // Halo Access Adjust 2
            else if(e->ContextRecord->Eip == 0x00058D8C)
            {
                if(e->ContextRecord->Eax == 0x819A5818)
                {
                    uint32 fix = g_HaloHack[1] + (e->ContextRecord->Eax - 0x803A6000);

                    *(DWORD*)0x0039BE58 = e->ContextRecord->Eax = fix;

                    // go through and fix any other pointers in the 0x2DF1C8 allocation chunk
                    {
                        DWORD dwPtr = *(DWORD*)0x2DF1C8;
                        DWORD dwSize = EmuCheckAllocationSize((PVOID)dwPtr, false);

                        // dword aligned
                        dwSize -= 4 - dwSize%4;

                        for(DWORD v=0;v<dwSize;v+=4)
                        {
                            DWORD dwCur = *(DWORD*)(dwPtr+v);

                            if(dwCur >= 0x803A6000 && dwCur < 0x819A6000)
                                *(DWORD*)(dwPtr+v) = g_HaloHack[1] + (dwCur - 0x803A6000);
                        }
                    }

                    DbgPrintf("EmuMain (0x%X): Halo Access Adjust 2 was applied!\n", GetCurrentThreadId());

                    g_bEmuException = false;

                    return EXCEPTION_CONTINUE_EXECUTION;
                }
            }
        }
    }

    // print debug information
    {
        if(e->ExceptionRecord->ExceptionCode == 0x80000003)
            printf("EmuMain (0x%X): Recieved Breakpoint Exception (int 3)\n", GetCurrentThreadId());
        else
            printf("EmuMain (0x%X): Recieved Exception (Code := 0x%.08X)\n", GetCurrentThreadId(), e->ExceptionRecord->ExceptionCode);

        printf("\n"
            " EIP := 0x%.08X EFL := 0x%.08X\n"
            " EAX := 0x%.08X EBX := 0x%.08X ECX := 0x%.08X EDX := 0x%.08X\n"
            " ESI := 0x%.08X EDI := 0x%.08X ESP := 0x%.08X EBP := 0x%.08X\n"
            "\n",
            e->ContextRecord->Eip, e->ContextRecord->EFlags,
            e->ContextRecord->Eax, e->ContextRecord->Ebx, e->ContextRecord->Ecx, e->ContextRecord->Edx,
            e->ContextRecord->Esi, e->ContextRecord->Edi, e->ContextRecord->Esp, e->ContextRecord->Ebp);
    }

    fflush(stdout);

    // notify user
    {
        char buffer[256];

        if(e->ExceptionRecord->ExceptionCode == 0x80000003)
        {
            sprintf(buffer, 
                "Recieved Breakpoint Exception (int 3) @ EIP := 0x%.08X\n"
                "\n"
                "  Press Abort to terminate emulation.\n"
                "  Press Retry to debug.\n"
                "  Press Ignore to continue emulation.",
                e->ContextRecord->Eip, e->ContextRecord->EFlags);

            e->ContextRecord->Eip += 1;

            int ret = MessageBox(g_hEmuWindow, buffer, "Cxbx", MB_ICONSTOP | MB_ABORTRETRYIGNORE);

            if(ret == IDABORT)
            {
                printf("EmuMain (0x%X): Aborting Emulation\n", GetCurrentThreadId());
                fflush(stdout);

                if(g_hEmuParent != NULL)
                    SendMessage(g_hEmuParent, WM_PARENTNOTIFY, WM_DESTROY, 0);

                ExitProcess(1);
            }
            else if(ret == IDIGNORE)
            {
                printf("EmuMain (0x%X): Ignored Breakpoint Exception\n", GetCurrentThreadId());

                g_bEmuException = false;

                return EXCEPTION_CONTINUE_EXECUTION;
            }
        }
        else
        {
            sprintf(buffer, 
                "Recieved Exception Code 0x%.08X @ EIP := 0x%.08X\n"
                "\n"
                "  Press \"OK\" to terminate emulation.\n"
                "  Press \"Cancel\" to debug.",
                e->ExceptionRecord->ExceptionCode, e->ContextRecord->Eip, e->ContextRecord->EFlags);

            if(MessageBox(g_hEmuWindow, buffer, "Cxbx", MB_ICONSTOP | MB_OKCANCEL) == IDOK)
            {
                printf("EmuMain (0x%X): Aborting Emulation\n", GetCurrentThreadId());
                fflush(stdout);

                if(g_hEmuParent != NULL)
                    SendMessage(g_hEmuParent, WM_PARENTNOTIFY, WM_DESTROY, 0);

                ExitProcess(1);
            }
        }
    }

    g_bEmuException = false;

    return EXCEPTION_CONTINUE_SEARCH;
}

// check how many bytes were allocated for a structure
extern int EmuCheckAllocationSize(LPVOID pBase, bool largeBound)
{
    MEMORY_BASIC_INFORMATION MemoryBasicInfo;

    DWORD dwRet;
#ifdef _DEBUG_ALLOC
    dwRet = CxbxVirtualQueryDebug(pBase, &MemoryBasicInfo, sizeof(MemoryBasicInfo));
    if (dwRet == -1)
#endif
    dwRet = VirtualQuery(pBase, &MemoryBasicInfo, sizeof(MemoryBasicInfo));

    if(dwRet == 0)
        return 0;

    if(MemoryBasicInfo.State != MEM_COMMIT)
        return 0;

    // this is a hack in order to determine when pointers come from a large write-combined database
    if(largeBound && MemoryBasicInfo.RegionSize > 5*1024*1024)
        return -1;

    return MemoryBasicInfo.RegionSize - ((DWORD)pBase - (DWORD)MemoryBasicInfo.BaseAddress);
}

// exception handle for that tough final exit :)
int ExitException(LPEXCEPTION_POINTERS e)
{
    if(EmuIsXboxFS())
        EmuSwapFS();

    static int count = 0;

    // debug information
    printf("EmuMain (0x%X): * * * * * EXCEPTION * * * * *\n", GetCurrentThreadId());
    printf("EmuMain (0x%X): Recieved Exception [0x%.08X]@0x%.08X\n", GetCurrentThreadId(), e->ExceptionRecord->ExceptionCode, e->ContextRecord->Eip);
    printf("EmuMain (0x%X): * * * * * EXCEPTION * * * * *\n", GetCurrentThreadId());

    fflush(stdout);

    MessageBox(g_hEmuWindow, "Warning: Could not safely terminate process!", "Cxbx", MB_OK);

    count++;

    if(count > 1)
    {
        MessageBox(g_hEmuWindow, "Warning: Multiple Problems!", "Cxbx", MB_OK);
        return EXCEPTION_CONTINUE_SEARCH;
    }

    if(g_hEmuParent != NULL)
        SendMessage(g_hEmuParent, WM_PARENTNOTIFY, WM_DESTROY, 0);

    ExitProcess(1);

    return EXCEPTION_CONTINUE_SEARCH;
}
