// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuX.cpp
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
#define CXBXKRNL_INTERNAL
#define _XBOXKRNL_LOCAL_

#include "Cxbx.h"
#include "EmuX.h"

// ******************************************************************
// * ntdll wrapped in namespace to avoid collisions
// ******************************************************************
namespace xntdll
{
    #include "xntdll.h"
};

using namespace win32;

// ******************************************************************
// * static functions
// ******************************************************************
static void EmuXInstallWrappers(OOVPATable *OovpaTable, uint32 OovpaTableSize, void (*Entry)(), Xbe::Header *XbeHeader);

// ******************************************************************
// * func: EmuXInit
// ******************************************************************
CXBXKRNL_API void NTAPI EmuXInit(DebugMode DebugConsole, char *DebugFilename, Xbe::Header *XbeHeader, uint32 XbeHeaderSize, void (*Entry)())
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
            
            printf("CxbxKrnl [0x%.08X]: Debug console allocated.\n", GetCurrentThreadId());
        }
    }
    else if(DebugConsole == DM_FILE)
    {
        FreeConsole();

        freopen(DebugFilename, "wt", stdout);

        printf("EmuX (0x%.08X): Debug console allocated.\n", GetCurrentThreadId());
    }

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    {
        printf("EmuX: EmuXInit\n"
               "(\n"
               "   DebugConsole        : 0x%.08X\n"
               "   DebugFilename       : \"%s\"\n"
               "   XBEHeader           : 0x%.08X\n"
               "   XBEHeaderSize       : 0x%.08X\n"
               "   Entry               : 0x%.08X\n"
               ");\n",
               DebugConsole, DebugFilename, XbeHeader, XbeHeaderSize, Entry);
    }

    // ******************************************************************
    // * Locate functions and install wrapper vectors
    // ******************************************************************
    {
        EmuXInstallWrappers(XAPI_1_0_4361, XAPI_1_0_4361_SIZE, Entry, XbeHeader);
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
    // * Initialize FS system
    // ******************************************************************
    {
        EmuXInitFS();
    }

    // ******************************************************************
    // * Initialize FS:* structure
    // ******************************************************************
    {
        EmuXGenerateFS();
    }

    // ******************************************************************
    // * Initialize Direct3D8
    // ******************************************************************
    {
        xboxkrnl::EmuXInitD3D();
    }

    printf("EmuX (0x%.08X): Initial thread starting.\n", GetCurrentThreadId());

    // This must be enabled or the debugger may crash (sigh)
    //__asm _emit 0xF1

    EmuXSwapFS();   // XBox FS

    Entry();

    EmuXSwapFS();   // Win2k/XP FS

    printf("EmuX (0x%.08X): Initial thread ended.\n", GetCurrentThreadId());

    fflush(stdout);

    return;
}

// ******************************************************************
// * func: EmuXDummy
// ******************************************************************
CXBXKRNL_API void NTAPI EmuXDummy()
{
    EmuXSwapFS();   // Win2k/XP FS

    printf("EmuX (0x%.08X): EmuXDummy()\n", GetCurrentThreadId());

    EmuXSwapFS();   // XBox FS
}

// ******************************************************************
// * func: EmuXPanic
// ******************************************************************
CXBXKRNL_API void NTAPI EmuXPanic()
{
    EmuXSwapFS();   // Win2k/XP FS

    printf("EmuX (0x%.08X): EmuXPanic()\n", GetCurrentThreadId());

    MessageBox(NULL, "Kernel Panic! Process will now terminate.", "CxbxKrnl", MB_OK | MB_ICONEXCLAMATION);

    EmuXSwapFS();   // XBox FS
}

// ******************************************************************
// * func: EmuXInstallWrapper
// ******************************************************************
inline void EmuXInstallWrapper(void *FunctionAddr, void *WrapperAddr)
{
    uint08 *FuncBytes = (uint08*)FunctionAddr;

    *(uint08*)&FuncBytes[0] = 0xE9;
    *(uint32*)&FuncBytes[1] = (uint32)WrapperAddr - (uint32)FunctionAddr - 5;
}

// ******************************************************************
// * func: EmuXInstallWrappers
// ******************************************************************
void EmuXInstallWrappers(OOVPATable *OovpaTable, uint32 OovpaTableSize, void (*Entry)(), Xbe::Header *XbeHeader)
{
    // ******************************************************************
    // * debug trace
    // ******************************************************************
    {
        printf("EmuX: EmuXInstallWrappers\n"
               "(\n"
               "   Entry               : 0x%.08X\n"
               ");\n",
            Entry);
    }

    // ******************************************************************
    // * traverse the full OOVPA table
    // ******************************************************************
    for(uint32 a=0;a<OovpaTableSize/sizeof(OOVPATable);a++)
    {
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
        }
        // ******************************************************************
        // * Small
        // ******************************************************************
        else
        {
            SOOVPA<1> *Soovpa = (SOOVPA<1>*)Oovpa;

            upper -= Soovpa->Sovp[count-1].Offset;

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
                    printf("EmuXInstallWrappers: 0x%.08X -> %s\n", cur, XAPI_1_0_4361[a].szFuncName);
                    #endif

                    EmuXInstallWrapper((void*)cur, XAPI_1_0_4361[a].lpRedirect);

                    break;
                }
            }
        }
    }

}