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
#define _XBOXKRNL_LOCAL_

#include "Cxbx.h"
#include "Emu.h"

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace xntdll
{
    #include "xntdll.h"
};

using namespace win32;

// ******************************************************************
// * globals
// ******************************************************************
static HANDLE g_hMapObject = NULL;

// ******************************************************************
// * statics
// ******************************************************************
static void EmuInstallWrappers(OOVPATable *OovpaTable, uint32 OovpaTableSize, void (*Entry)(), Xbe::Header *XbeHeader);

// ******************************************************************
// * shared memory
// ******************************************************************
struct EmuShared
{
    uint32 dwRandomData;
}
*g_EmuShared;

// ******************************************************************
// * func: DllMain
// ******************************************************************
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    // ******************************************************************
    // * Initialize shared memory
    // ******************************************************************
    if(fdwReason == DLL_PROCESS_ATTACH)
    {
        bool init = false;

        g_hMapObject = CreateFileMapping
        ( 
            INVALID_HANDLE_VALUE,   // Paging file
            NULL,                   // default security attributes
            PAGE_READWRITE,         // read/write access
            0,                      // size: high 32 bits
            sizeof(EmuShared),     // size: low 32 bits
            "EmuShared"            // name of map object
        );

        if(g_hMapObject == NULL)
            return FALSE;

        if(GetLastError() != ERROR_ALREADY_EXISTS)
            init = true;

        g_EmuShared = (EmuShared*)MapViewOfFile
        (
            g_hMapObject,   // object to map view of
            FILE_MAP_WRITE, // read/write access
            0,              // high offset:  map from
            0,              // low offset:   beginning
            0               // default: map entire file
        );

        if(g_EmuShared == NULL) 
            return FALSE; 

        if(init)
        {
            // initialization of shared data
            g_EmuShared->dwRandomData = 0;
        }
    }
    
    // ******************************************************************
    // * Release shared memory
    // ******************************************************************
    if(fdwReason == DLL_PROCESS_DETACH)
    {
        UnmapViewOfFile(g_EmuShared);

        CloseHandle(g_hMapObject);
    }

    return TRUE;
}

// ******************************************************************
// * func: EmuNoFunc
// ******************************************************************
CXBXKRNL_API void NTAPI EmuNoFunc()
{
    EmuSwapFS();   // Win2k/XP FS

    printf("Emu (0x%.08X): EmuNoFunc()\n", GetCurrentThreadId());

    EmuSwapFS();   // XBox FS
}

// ******************************************************************
// * func: EmuInit
// ******************************************************************
CXBXKRNL_API void NTAPI EmuInit(Xbe::LibraryVersion *LibraryVersion, DebugMode DebugConsole, char *DebugFilename, Xbe::Header *XbeHeader, uint32 XbeHeaderSize, void (*Entry)())
{
    // ******************************************************************
    // * debug console allocation (if configured)
    // ******************************************************************
    if(DebugConsole == DM_CONSOLE)
    {
        if(AllocConsole())
        {
            freopen("CONOUT$", "wt", stdout);

            SetConsoleTitle("Cxbx : Kernel Debug Console");

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
            
            printf("CxbxKrnl (0x%.08X): Debug console allocated.\n", GetCurrentThreadId());
        }
    }
    else if(DebugConsole == DM_FILE)
    {
        FreeConsole();

        freopen(DebugFilename, "wt", stdout);

        printf("Emu (0x%.08X): Debug console allocated.\n", GetCurrentThreadId());
    }

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    {
        printf("Emu: EmuInit\n"
               "(\n"
               "   LibraryVersion      : 0x%.08X\n"
               "   DebugConsole        : 0x%.08X\n"
               "   DebugFilename       : \"%s\"\n"
               "   XBEHeader           : 0x%.08X\n"
               "   XBEHeaderSize       : 0x%.08X\n"
               "   Entry               : 0x%.08X\n"
               ");\n",
               LibraryVersion, DebugConsole, DebugFilename, XbeHeader, XbeHeaderSize, Entry);
    }

    // ******************************************************************
    // * Locate functions and install wrapper vectors
    // ******************************************************************
    {
        uint32 dwLibraryVersions = XbeHeader->dwLibraryVersions;
        uint32 dwHLEEntries      = HLEDataBaseSize/sizeof(HLEData);

        for(uint32 v=0;v<dwLibraryVersions;v++)
        {
            uint16 MajorVersion = LibraryVersion[v].wMajorVersion;
            uint16 MinorVersion = LibraryVersion[v].wMinorVersion;
            uint16 BuildVersion = LibraryVersion[v].wBuildVersion;

            char szLibraryName[9] = {0};

            for(uint32 c=0;c<8;c++)
                szLibraryName[c] = LibraryVersion[v].szName[c];

            printf("Emu: Locating HLE Information for %s %d.%d.%d...", szLibraryName, MajorVersion, MinorVersion, BuildVersion);

            bool found=false;

            for(uint32 d=0;d<dwHLEEntries;d++)
            {
                if
                (
                    BuildVersion != HLEDataBase[d].BuildVersion ||
                    MinorVersion != HLEDataBase[d].MinorVersion ||
                    MajorVersion != HLEDataBase[d].MajorVersion ||
                    strcmp(szLibraryName, HLEDataBase[d].Library) != 0
                )
                continue;

                found = true;

                printf("Found\n");

                EmuInstallWrappers(HLEDataBase[d].OovpaTable, HLEDataBase[d].OovpaTableSize, Entry, XbeHeader);
            }

            if(!found)
                printf("Skipped\n");
        }
    }

    // ******************************************************************
    // * Load the necessary pieces of XBEHeader
    // ******************************************************************
    {
        Xbe::Header *MemXbeHeader = (Xbe::Header*)0x00010000;

        uint32 old_protection = 0;

        VirtualProtect(MemXbeHeader, 0x1000, PAGE_READWRITE, &old_protection);

        // we sure hope we aren't corrupting anything necessary for an .exe to survive :]
        MemXbeHeader->dwSizeofHeaders        = XbeHeader->dwSizeofHeaders;
        MemXbeHeader->dwCertificateAddr      = XbeHeader->dwCertificateAddr;
        MemXbeHeader->dwPeHeapReserve        = XbeHeader->dwPeHeapReserve;
        MemXbeHeader->dwPeHeapCommit         = XbeHeader->dwPeHeapCommit;

        memcpy(&MemXbeHeader->dwInitFlags, &XbeHeader->dwInitFlags, sizeof(XbeHeader->dwInitFlags));

        memcpy((void*)XbeHeader->dwCertificateAddr, &((uint08*)XbeHeader)[XbeHeader->dwCertificateAddr - 0x00010000], sizeof(Xbe::Certificate));
    }

    // ******************************************************************
    // * Initialize all components
    // ******************************************************************
    {
        EmuInitFS();
        EmuGenerateFS();

        xboxkrnl::EmuInitD3D(XbeHeader, XbeHeaderSize);
    }

    printf("Emu (0x%.08X): Initial thread starting.\n", GetCurrentThreadId());

    // This must be enabled or the debugger may crash (sigh)
    // __asm _emit 0xF1

    EmuSwapFS();   // XBox FS

    Entry();

    EmuSwapFS();   // Win2k/XP FS

    printf("Emu (0x%.08X): Initial thread ended.\n", GetCurrentThreadId());

    fflush(stdout);

    while(true)
        Sleep(1000);

    return;
}

// ******************************************************************
// * func: EmuPanic
// ******************************************************************
CXBXKRNL_API void NTAPI EmuPanic()
{
    EmuSwapFS();   // Win2k/XP FS

    printf("Emu (0x%.08X): EmuPanic()\n", GetCurrentThreadId());

#ifdef _DEBUG_TRACE
    MessageBox(NULL, "Kernel Panic! Process will now terminate.\n\n"
                     "Check debug traces for hints on the cause of this crash.", "CxbxKrnl", MB_OK | MB_ICONEXCLAMATION);
#else
    MessageBox(NULL, "Kernel Panic! Process will now terminate.", "CxbxKrnl", MB_OK | MB_ICONEXCLAMATION);
#endif
    EmuSwapFS();   // XBox FS
}

// ******************************************************************
// * func: EmuInstallWrapper
// ******************************************************************
inline void EmuInstallWrapper(void *FunctionAddr, void *WrapperAddr)
{
    uint08 *FuncBytes = (uint08*)FunctionAddr;

    *(uint08*)&FuncBytes[0] = 0xE9;
    *(uint32*)&FuncBytes[1] = (uint32)WrapperAddr - (uint32)FunctionAddr - 5;
}

// ******************************************************************
// * func: EmuInstallWrappers
// ******************************************************************
void EmuInstallWrappers(OOVPATable *OovpaTable, uint32 OovpaTableSize, void (*Entry)(), Xbe::Header *XbeHeader)
{
    // ******************************************************************
    // * traverse the full OOVPA table
    // ******************************************************************
    for(uint32 a=0;a<OovpaTableSize/sizeof(OOVPATable);a++)
    {
        #ifdef _DEBUG_TRACE
        printf("EmuInstallWrappers: Searching for %s...", OovpaTable[a].szFuncName);
        #endif

        OOVPA *Oovpa = OovpaTable[a].Oovpa;

        uint32 count = Oovpa->Count;
        uint32 lower = XbeHeader->dwBaseAddr;
        uint32 upper = XbeHeader->dwBaseAddr + XbeHeader->dwSizeofImage;

        // ******************************************************************
        // * Large
        // ******************************************************************
        if(Oovpa->Large == 1)
        {
            LOOVPA<1> *Loovpa = (LOOVPA<1>*)Oovpa;

            upper -= Loovpa->Lovp[count-1].Offset;

            bool found = false;

            // ******************************************************************
            // * Search all of the image memory
            // ******************************************************************
            for(uint32 cur=lower;cur<upper;cur++)
            {
                uint32  v=0;

                // ******************************************************************
                // * check all pairs, moving on if any do not match
                // ******************************************************************
                for(v=0;v<count;v++)
                {
                    uint32 Offset = Loovpa->Lovp[v].Offset;
                    uint32 Value  = Loovpa->Lovp[v].Value;

                    uint08 RealValue = *(uint08*)(cur + Offset);

                    if(RealValue != Value)
                        break;
                }

                // ******************************************************************
                // * success if we found all pairs
                // ******************************************************************
                if(v == count)
                {
                    #ifdef _DEBUG_TRACE
                    printf("Found! (0x%.08X)\n", cur);
                    #endif

                    EmuInstallWrapper((void*)cur, OovpaTable[a].lpRedirect);

                    found = true;

                    break;
                }
            }

            // ******************************************************************
            // * not found
            // ******************************************************************
            if(!found)
            {
                #ifdef _DEBUG_TRACE
                printf("None (OK)\n");
                #endif
            }
        }
        // ******************************************************************
        // * Small
        // ******************************************************************
        else
        {
            SOOVPA<1> *Soovpa = (SOOVPA<1>*)Oovpa;

            upper -= Soovpa->Sovp[count-1].Offset;

            bool found = false;

            // ******************************************************************
            // * Search all of the image memory
            // ******************************************************************
            for(uint32 cur=lower;cur<upper;cur++)
            {
                uint32  v=0;

                // ******************************************************************
                // * check all pairs, moving on if any do not match
                // ******************************************************************
                for(v=0;v<count;v++)
                {
                    uint32 Offset = Soovpa->Sovp[v].Offset;
                    uint32 Value  = Soovpa->Sovp[v].Value;

                    uint08 RealValue = *(uint08*)(cur + Offset);

                    if(RealValue != Value)
                        break;
                }

                // ******************************************************************
                // * success if we found all pairs
                // ******************************************************************
                if(v == count)
                {
                    #ifdef _DEBUG_TRACE
                    printf("Found! (0x%.08X)\n", cur);
                    #endif

                    EmuInstallWrapper((void*)cur, OovpaTable[a].lpRedirect);

                    found = true;

                    break;
                }
            }

            // ******************************************************************
            // * not found
            // ******************************************************************
            if(!found)
            {
                #ifdef _DEBUG_TRACE
                printf("None (OK)\n");
                #endif
            }
        }
    }

}