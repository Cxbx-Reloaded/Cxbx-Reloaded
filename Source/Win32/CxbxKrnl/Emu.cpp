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

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace xboxkrnl
{
    #include <xboxkrnl/xboxkrnl.h>
};

#include "Emu.h"
#include "EmuFS.h"

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace XTL
{
    #include "EmuXTL.h"
};

#include <locale.h>

#include "EmuShared.h"
#include "HLEDataBase.h"

// ******************************************************************
// * global / static
// ******************************************************************
Xbe::TLS    *g_pTLS       = NULL;
void        *g_pTLSData   = NULL;
Xbe::Header *g_pXbeHeader = NULL;
HANDLE		 g_hCurDir    = NULL;
HANDLE       g_hTDrive    = NULL;
HANDLE       g_hUDrive    = NULL;
HANDLE       g_hZDrive    = NULL;

// ******************************************************************
// * static
// ******************************************************************
static void *EmuLocateFunction(OOVPA *Oovpa, uint32 lower, uint32 upper);
static void  EmuInstallWrappers(OOVPATable *OovpaTable, uint32 OovpaTableSize, void (*Entry)(), Xbe::Header *pXbeHeader);
static void  EmuXRefFailure();
static int   ExitException(LPEXCEPTION_POINTERS e);

// ******************************************************************
// * func: DllMain
// ******************************************************************
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if(fdwReason == DLL_PROCESS_ATTACH)
        EmuShared::Init();
    
    if(fdwReason == DLL_PROCESS_DETACH)
        EmuShared::Cleanup();

    return TRUE;
}

// ******************************************************************
// * func: EmuNoFunc
// ******************************************************************
extern "C" CXBXKRNL_API void NTAPI EmuNoFunc()
{
    EmuSwapFS();   // Win2k/XP FS

    printf("Emu (0x%X): EmuNoFunc()\n", GetCurrentThreadId());

    EmuSwapFS();   // XBox FS
}

// ******************************************************************
// * func: EmuVerifyVersion
// ******************************************************************
extern "C" CXBXKRNL_API bool NTAPI EmuVerifyVersion(const char *szVersion)
{
    if(strcmp(szVersion, _CXBX_VERSION) != 0)
        return false;

    return true;
}

// ******************************************************************
// * func: EmuCleanThread
// ******************************************************************
extern "C" CXBXKRNL_API void NTAPI EmuCleanThread()
{
    if(EmuIsXboxFS())
        EmuSwapFS();    // Win2k/XP FS

    EmuCleanupFS();

    TerminateThread(GetCurrentThread(), 0);
}

// ******************************************************************
// * func: EmuInit
// ******************************************************************
extern "C" CXBXKRNL_API void NTAPI EmuInit
(
    void                   *pTLSData, 
    Xbe::TLS               *pTLS,
    Xbe::LibraryVersion    *pLibraryVersion,
    DebugMode               DbgMode,
    char                   *szDebugFilename,
    Xbe::Header            *pXbeHeader,
    uint32                  dwXbeHeaderSize,
    void                  (*Entry)())
{
    g_pTLS       = pTLS;
    g_pTLSData   = pTLSData;
	g_pXbeHeader = pXbeHeader;

	// For Unicode Conversions
	setlocale(LC_ALL, "English");

    // ******************************************************************
    // * debug console allocation (if configured)
    // ******************************************************************
    if(DbgMode == DM_CONSOLE)
    {
        if(AllocConsole())
        {
            freopen("CONOUT$", "wt", stdout);

            SetConsoleTitle("Cxbx : Kernel Debug Console");

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
            
            printf("Emu (0x%X): Debug console allocated (DM_CONSOLE).\n", GetCurrentThreadId());
        }
    }
    else if(DbgMode == DM_FILE)
    {
        FreeConsole();

        freopen(szDebugFilename, "wt", stdout);

        printf("Emu (0x%X): Debug console allocated (DM_FILE).\n", GetCurrentThreadId());
    }
    else
    {
        FreeConsole();

        char buffer[16];

        if(GetConsoleTitle(buffer, 16) != NULL)
            freopen("nul", "w", stdout);
    }

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    {
        #ifdef _DEBUG_TRACE
        printf("Emu (0x%X): Debug Trace Enabled.\n", GetCurrentThreadId());

        printf("Emu (0x%X): EmuInit\n"
               "(\n"
               "   pTLSData            : 0x%.08X\n"
               "   pTLS                : 0x%.08X\n"
               "   pLibraryVersion     : 0x%.08X\n"
               "   DebugConsole        : 0x%.08X\n"
               "   DebugFilename       : \"%s\"\n"
               "   pXBEHeader          : 0x%.08X\n"
               "   pXBEHeaderSize      : 0x%.08X\n"
               "   Entry               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pTLSData, pTLS, pLibraryVersion, DbgMode, szDebugFilename, pXbeHeader, dwXbeHeaderSize, Entry);

        #else
        printf("Emu (0x%X): Debug Trace Disabled.\n", GetCurrentThreadId());
        #endif
    }

    // ******************************************************************
    // * Load the necessary pieces of XBEHeader
    // ******************************************************************
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

    // ******************************************************************
	// * Initialize current directory
    // ******************************************************************
	{
		char szBuffer[260];

        g_EmuShared->GetXbePath(szBuffer);

        SetCurrentDirectory(szBuffer);

		g_hCurDir = CreateFile(szBuffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

        if(g_hCurDir == INVALID_HANDLE_VALUE)
			EmuCleanup("Could not map D:\\\n");
	}

    // ******************************************************************
	// * Initialize T:\ and U:\ directories
    // ******************************************************************
    {
		char szBuffer[260];

        #ifdef _DEBUG
        GetModuleFileName(GetModuleHandle("CxbxKrnl.dll"), szBuffer, 260);
        #else
        GetModuleFileName(GetModuleHandle("Cxbx.dll"), szBuffer, 260);
        #endif

        sint32 spot=-1;
        for(int v=0;v<260;v++)
        {
            if(szBuffer[v] == '\\')
                spot = v;
            else if(szBuffer[v] == '\0')
                break;
        }

        if(spot != -1)
            szBuffer[spot] = '\0';

        Xbe::Certificate *pCertificate = (Xbe::Certificate*)pXbeHeader->dwCertificateAddr;

        // Create TData Directory
        {
            strcpy(&szBuffer[spot], "\\TDATA");

            CreateDirectory(szBuffer, NULL);

            sprintf(&szBuffer[spot+6], "\\%08x", pCertificate->dwTitleId);

            CreateDirectory(szBuffer, NULL);

            g_hTDrive = CreateFile(szBuffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

            if(g_hTDrive == INVALID_HANDLE_VALUE)
                EmuCleanup("Could not map T:\\\n");
        }

        // Create UData Directory
        {
            strcpy(&szBuffer[spot], "\\UDATA");

            CreateDirectory(szBuffer, NULL);

            sprintf(&szBuffer[spot+6], "\\%08x", pCertificate->dwTitleId);

            CreateDirectory(szBuffer, NULL);

            g_hUDrive = CreateFile(szBuffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

            if(g_hUDrive == INVALID_HANDLE_VALUE)
                EmuCleanup("Could not map U:\\\n");
        }

        // Create ZData Directory
        {
            strcpy(&szBuffer[spot], "\\CxbxCache");

            CreateDirectory(szBuffer, NULL);

            //* is it necessary to make this directory title unique?
            sprintf(&szBuffer[spot+10], "\\%08x", pCertificate->dwTitleId);

            CreateDirectory(szBuffer, NULL);
            //*/

            g_hZDrive = CreateFile(szBuffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

            if(g_hUDrive == INVALID_HANDLE_VALUE)
                EmuCleanup("Could not map Z:\\\n");
        }
    }

    // ******************************************************************
    // * Initialize OpenXDK emulation
    // ******************************************************************
    if(pLibraryVersion == 0)
        printf("Emu (0x%X): Detected OpenXDK application...\n", GetCurrentThreadId());

    // ******************************************************************
    // * Initialize Microsoft XDK emulation
    // ******************************************************************
    if(pLibraryVersion != 0)
    {
        printf("Emu (0x%X): Detected Microsoft XDK application...\n", GetCurrentThreadId());

        uint32 dwLibraryVersions = pXbeHeader->dwLibraryVersions;
        uint32 dwHLEEntries      = HLEDataBaseSize/sizeof(HLEData);

        uint32 LastUnResolvedXRefs = UnResolvedXRefs+1;
        uint32 OrigUnResolvedXRefs = UnResolvedXRefs;

        for(int p=0;UnResolvedXRefs < LastUnResolvedXRefs;p++)
        {
            printf("Emu (0x%X): Beginning HLE Pass %d...\n", GetCurrentThreadId(), p);

            LastUnResolvedXRefs = UnResolvedXRefs;

            bool bFoundD3D = false;
            for(uint32 v=0;v<dwLibraryVersions;v++)
            {
                uint16 MajorVersion = pLibraryVersion[v].wMajorVersion;
                uint16 MinorVersion = pLibraryVersion[v].wMinorVersion;
                uint16 BuildVersion = pLibraryVersion[v].wBuildVersion;

                char szLibraryName[9] = {0};

                for(uint32 c=0;c<8;c++)
                    szLibraryName[c] = pLibraryVersion[v].szName[c];

                printf("Emu (0x%X): Locating HLE Information for %s %d.%d.%d...", GetCurrentThreadId(), szLibraryName, MajorVersion, MinorVersion, BuildVersion);

                // TODO: HACK: These libraries are packed into one database
                if(strcmp(szLibraryName, "D3DX8") == 0)
                    strcpy(szLibraryName, "D3D8");

                if(strcmp(szLibraryName, "D3D8") == 0)
                {
                    if(bFoundD3D)
                    {
                        printf("Redundant\n");
                        continue;
                    }

                    bFoundD3D = true;
                }

                bool found=false;

                for(uint32 d=0;d<dwHLEEntries;d++)
                {
                    if(BuildVersion != HLEDataBase[d].BuildVersion || MinorVersion != HLEDataBase[d].MinorVersion || MajorVersion != HLEDataBase[d].MajorVersion || strcmp(szLibraryName, HLEDataBase[d].Library) != 0)
                        continue;

                    found = true;

                    printf("Found\n");

                    EmuInstallWrappers(HLEDataBase[d].OovpaTable, HLEDataBase[d].OovpaTableSize, Entry, pXbeHeader);
                }

                if(!found)
                    printf("Skipped\n");

                if(bXRefFirstPass)
                {
                    if(strcmp("XAPILIB", szLibraryName) == 0 && MajorVersion == 1 && MinorVersion == 0 && (BuildVersion == 3911 || BuildVersion == 4034 || BuildVersion == 4134 || BuildVersion == 4361 || BuildVersion == 4627))
                    {
                        uint32 lower = pXbeHeader->dwBaseAddr;
                        uint32 upper = pXbeHeader->dwBaseAddr + pXbeHeader->dwSizeofImage;

				        // ******************************************************************
				        // * Locate XapiProcessHeap
				        // ******************************************************************
                        {
                            void *pFunc = 0;

                            if(BuildVersion >= 4361)
					            pFunc = EmuLocateFunction((OOVPA*)&XapiInitProcess_1_0_4361, lower, upper);
                            else // 3911, 4034, 4134
                                pFunc = EmuLocateFunction((OOVPA*)&XapiInitProcess_1_0_3911, lower, upper);

					        if(pFunc != 0)
					        {
						        XTL::EmuXapiProcessHeap = *(PVOID**)((uint32)pFunc + 0x3E);

						        XTL::g_pRtlCreateHeap = *(XTL::pfRtlCreateHeap*)((uint32)pFunc + 0x37);
						        XTL::g_pRtlCreateHeap = (XTL::pfRtlCreateHeap)((uint32)pFunc + (uint32)XTL::g_pRtlCreateHeap + 0x37 + 0x04);

						        printf("Emu (0x%X): 0x%.08X -> EmuXapiProcessHeap\n", GetCurrentThreadId(), XTL::EmuXapiProcessHeap);
						        printf("Emu (0x%X): 0x%.08X -> RtlCreateHeap\n", GetCurrentThreadId(), XTL::g_pRtlCreateHeap);
					        }
				        }
                    }
			        else if(strcmp("D3D8", szLibraryName) == 0 && MajorVersion == 1 && MinorVersion == 0 && (BuildVersion == 4134 || BuildVersion == 4361 || BuildVersion == 4627))
			        {
                        uint32 lower = pXbeHeader->dwBaseAddr;
                        uint32 upper = pXbeHeader->dwBaseAddr + pXbeHeader->dwSizeofImage;

				        void *pFunc = EmuLocateFunction((OOVPA*)&IDirect3DDevice8_SetRenderState_CullMode_1_0_4134, lower, upper);

                        // ******************************************************************
				        // * Locate D3DDeferredRenderState
				        // ******************************************************************
                        if(pFunc != 0 && (BuildVersion == 4134 || BuildVersion == 4361 || BuildVersion == 4627))
                        {
                            if(BuildVersion == 4134)
                                XTL::EmuD3DDeferredRenderState = (DWORD*)(*(DWORD*)((uint32)pFunc + 0x2B) - 0x248 + 82*4);  // TODO: Verify
                            else if(BuildVersion == 4361)
						        XTL::EmuD3DDeferredRenderState = (DWORD*)(*(DWORD*)((uint32)pFunc + 0x2B) - 0x200 + 82*4);
                            else if(BuildVersion == 4627)
						        XTL::EmuD3DDeferredRenderState = (DWORD*)(*(DWORD*)((uint32)pFunc + 0x2B) - 0x24C + 92*4);

                            for(int v=0;v<146;v++)
                                XTL::EmuD3DDeferredRenderState[v] = X_D3DRS_UNK;

                            printf("Emu (0x%X): 0x%.08X -> EmuD3DDeferredRenderState\n", GetCurrentThreadId(), XTL::EmuD3DDeferredRenderState);
                        }
                        else
                        {
                            XTL::EmuD3DDeferredRenderState = 0;
                            EmuWarning("EmuD3DDeferredRenderState was not found!");
                        }

                        // ******************************************************************
				        // * Locate D3DDeferredTextureState
				        // ******************************************************************
                        {
                            if(BuildVersion == 4134)
                                pFunc = EmuLocateFunction((OOVPA*)&IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_4134, lower, upper);
                            else if(BuildVersion == 4361)
                                pFunc = EmuLocateFunction((OOVPA*)&IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_4361, lower, upper);
                            else if(BuildVersion == 4627)
                                pFunc = EmuLocateFunction((OOVPA*)&IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_4627, lower, upper);

                            if(pFunc != 0)
                            {
                                if(BuildVersion == 4134)
					                XTL::EmuD3DDeferredTextureState = (DWORD*)(*(DWORD*)((uint32)pFunc + 0x18) - 0x70);
                                else
					                XTL::EmuD3DDeferredTextureState = (DWORD*)(*(DWORD*)((uint32)pFunc + 0x19) - 0x70);

                                for(int v=0;v<32*4;v++)
                                    XTL::EmuD3DDeferredTextureState[v] = X_D3DTSS_UNK;

                                printf("Emu (0x%X): 0x%.08X -> EmuD3DDeferredTextureState\n", GetCurrentThreadId(), XTL::EmuD3DDeferredTextureState);
                            }
                            else
                            {
                                XTL::EmuD3DDeferredTextureState = 0;
                                EmuWarning("EmuD3DDeferredTextureState was not found!");
                            }
                        }
			        }
                }
            }

            bXRefFirstPass = false;
        }

        // ******************************************************************
        // * Display XRef Summary
        // ******************************************************************
        printf("Emu (0x%X): Resolved %d cross reference(s)\n", GetCurrentThreadId(), OrigUnResolvedXRefs - UnResolvedXRefs);
    }

	// ******************************************************************
    // * Initialize FS Emulation
    // ******************************************************************
    {
        EmuInitFS();

        EmuGenerateFS(pTLS, pTLSData);
    }

    printf("Emu (0x%X): Initializing Direct3D.\n", GetCurrentThreadId());

    XTL::EmuD3DInit(pXbeHeader, dwXbeHeaderSize);

    printf("Emu (0x%X): Initial thread starting.\n", GetCurrentThreadId());

    // ******************************************************************
    // * Entry Point
    // ******************************************************************
    __try
    {
        EmuSwapFS();   // XBox FS

        // _USE_XGMATH Disabled in mesh :[
        // halo : dword_0_2E2D18
        //_asm int 3

        Entry();

        EmuSwapFS();   // Win2k/XP FS
    }
    __except(EmuException(GetExceptionInformation()))
    {
        printf("Emu: WARNING!! Problem with ExceptionFilter\n");
    }

    printf("Emu (0x%X): Initial thread ended.\n", GetCurrentThreadId());

    fflush(stdout);

    EmuCleanThread();

    return;
}

// ******************************************************************
// * func: EmuWarning
// ******************************************************************
#ifdef _DEBUG_WARNINGS
extern "C" CXBXKRNL_API void NTAPI EmuWarning(const char *szWarningMessage, ...)
{
    if(szWarningMessage == NULL)
        return;

    char szBuffer1[255];
    char szBuffer2[255];

    va_list argp;

    sprintf(szBuffer1, "Emu (0x%X): *WARNING* -> ", GetCurrentThreadId());

    va_start(argp, szWarningMessage);

    vsprintf(szBuffer2, szWarningMessage, argp);

    va_end(argp);

    strcat(szBuffer1, szBuffer2);

    printf("%s\n", szBuffer1);

    fflush(stdout);

    return;
}
#endif

// ******************************************************************
// * func: EmuCleanup
// ******************************************************************
extern "C" CXBXKRNL_API void NTAPI EmuCleanup(const char *szErrorMessage, ...)
{
    // ******************************************************************
    // * Print out ErrorMessage (if exists)
    // ******************************************************************
    if(szErrorMessage != NULL)
    {
        char szBuffer1[255];
        char szBuffer2[255];

        va_list argp;

        sprintf(szBuffer1, "Emu (0x%X): Recieved Fatal Message -> \n\n", GetCurrentThreadId());

        va_start(argp, szErrorMessage);

        vsprintf(szBuffer2, szErrorMessage, argp);

        va_end(argp);

        strcat(szBuffer1, szBuffer2);

        printf("%s\n", szBuffer1);

        MessageBox(NULL, szBuffer1, "CxbxKrnl", MB_OK | MB_ICONEXCLAMATION);
    }

    printf("CxbxKrnl: Terminating Process\n");
    fflush(stdout);

    // ******************************************************************
    // * Cleanup debug output
    // ******************************************************************
    {
        FreeConsole();

        char buffer[16];

        if(GetConsoleTitle(buffer, 16) != NULL)
            freopen("nul", "w", stdout);
    }

    TerminateProcess(GetCurrentProcess(), 0);

    return;
}

// ******************************************************************
// * func: EmuPanic
// ******************************************************************
extern "C" CXBXKRNL_API void NTAPI EmuPanic()
{
    if(EmuIsXboxFS())
        EmuSwapFS();   // Win2k/XP FS

    printf("Emu (0x%X): EmuPanic()\n", GetCurrentThreadId());

    EmuCleanup("Kernel Panic!");

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
// * func: EmuLocateFunction
// ******************************************************************
void *EmuLocateFunction(OOVPA *Oovpa, uint32 lower, uint32 upper)
{
    uint32 count = Oovpa->Count;

    // Skip out if this is an unnecessary search
    if(!bXRefFirstPass && Oovpa->XRefCount == 0 && Oovpa->XRefSaveIndex == (uint08)-1)
        return 0;

    // ******************************************************************
    // * Large
    // ******************************************************************
    if(Oovpa->Large == 1)
    {
        LOOVPA<1> *Loovpa = (LOOVPA<1>*)Oovpa;

        upper -= Loovpa->Lovp[count-1].Offset;

        // ******************************************************************
        // * Search all of the image memory
        // ******************************************************************
        for(uint32 cur=lower;cur<upper;cur++)
        {
            uint32 v;

            // ******************************************************************
            // * check all cross references
            // ******************************************************************
            for(v=0;v<Loovpa->XRefCount;v++)
            {
                uint32 Offset = Loovpa->Lovp[v].Offset;
                uint32 Value  = Loovpa->Lovp[v].Value;

                uint32 RealValue = *(uint32*)(cur + Offset);

                if(XRefDataBase[Value] == -1)
                    goto skipout_L;   // Unsatisfied XRef is not acceptable

                if(RealValue + cur + Offset+4 != XRefDataBase[Value])
                    break;
            }

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
                if(Loovpa->XRefSaveIndex != (uint08)-1)
                {
                    if(XRefDataBase[Loovpa->XRefSaveIndex] == -1)
                    {
                        UnResolvedXRefs--;
                        XRefDataBase[Loovpa->XRefSaveIndex] = cur;

                        return (void*)cur;
                    }
                    else
                        return 0;   // Already Found, no bother patching again
                }

                return (void*)cur;
            }

            skipout_L:;
        }
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
            uint32 v;

//            if( (cur == 0x0006A6C6) && (Soovpa->Sovp[v].Value == XREF_SETCURRENTPOSITION2) )
//                _asm int 3
            // ******************************************************************
            // * check all cross references
            // ******************************************************************
            for(v=0;v<Soovpa->XRefCount;v++)
            {
                uint32 Offset = Soovpa->Sovp[v].Offset;
                uint32 Value  = Soovpa->Sovp[v].Value;

                uint32 RealValue = *(uint32*)(cur + Offset);

                if(XRefDataBase[Value] == -1)
                    goto skipout_S;   // Unsatisfied XRef is not acceptable

                if( (RealValue + cur + Offset + 4 != XRefDataBase[Value]))
                    break;
            }

            // check OV pairs if all xrefs matched
            if(v == Soovpa->XRefCount)
            {
                // ******************************************************************
                // * check all pairs, moving on if any do not match
                // ******************************************************************
                for(;v<count;v++)
                {
                    uint32 Offset = Soovpa->Sovp[v].Offset;
                    uint32 Value  = Soovpa->Sovp[v].Value;

                    uint08 RealValue = *(uint08*)(cur + Offset);

                    if(RealValue != Value)
                        break;
                }
            }

            // ******************************************************************
            // * success if we found all pairs
            // ******************************************************************
            if(v == count)
            {
                if(Soovpa->XRefSaveIndex != (uint08)-1)
                {
                    if(XRefDataBase[Soovpa->XRefSaveIndex] == -1)
                    {
                        UnResolvedXRefs--;
                        XRefDataBase[Soovpa->XRefSaveIndex] = cur;

                        return (void*)cur;
                    }
                    else
                        return 0;   // Already Found, no bother patching again
                }

                return (void*)cur;
            }

            skipout_S:;
        }
    }

    return 0;
}

// ******************************************************************
// * func: EmuInstallWrappers
// ******************************************************************
void EmuInstallWrappers(OOVPATable *OovpaTable, uint32 OovpaTableSize, void (*Entry)(), Xbe::Header *pXbeHeader)
{
    uint32 lower = pXbeHeader->dwBaseAddr;
    uint32 upper = pXbeHeader->dwBaseAddr + pXbeHeader->dwSizeofImage;

    // ******************************************************************
    // * traverse the full OOVPA table
    // ******************************************************************
    for(uint32 a=0;a<OovpaTableSize/sizeof(OOVPATable);a++)
    {
        OOVPA *Oovpa = OovpaTable[a].Oovpa;

        void *pFunc = EmuLocateFunction(Oovpa, lower, upper);

        if(pFunc != 0)
        {
            #ifdef _DEBUG_TRACE
            printf("Emu (0x%X): 0x%.08X -> %s\n", GetCurrentThreadId(), pFunc, OovpaTable[a].szFuncName);
            #endif

            if(OovpaTable[a].lpRedirect == 0)
                EmuInstallWrapper(pFunc, EmuXRefFailure);
            else
                EmuInstallWrapper(pFunc, OovpaTable[a].lpRedirect);
        }
    }
}

// ******************************************************************
// * func: EmuXRefFailure
// ******************************************************************
void EmuXRefFailure()
{
    EmuSwapFS();    // Win2k/XP FS
    
    EmuCleanup("XRef-only function body reached. Fatal Error.");
}

// ******************************************************************
// * func: EmuException
// ******************************************************************
int EmuException(LPEXCEPTION_POINTERS e)
{
    if(EmuIsXboxFS())
        EmuSwapFS();

    // ******************************************************************
	// * Debugging Information
	// ******************************************************************
	{
		printf("Emu (0x%X): * * * * * EXCEPTION * * * * *\n", GetCurrentThreadId());
		printf("Emu (0x%X): Recieved Exception [0x%.08X]@0x%.08X\n", GetCurrentThreadId(), e->ExceptionRecord->ExceptionCode, e->ContextRecord->Eip);
		printf("Emu (0x%X): * * * * * EXCEPTION * * * * *\n", GetCurrentThreadId());
	}

    fflush(stdout);

	// ******************************************************************
	// * Notify User
	// ******************************************************************
	{
		char buffer[256];

		sprintf(buffer, "Recieved Exception [0x%.08X]@0x%.08X\n\nPress 'OK' to terminate emulation.\nPress 'Cancel' to debug.", e->ExceptionRecord->ExceptionCode, e->ContextRecord->Eip);

        if(MessageBox(XTL::g_hEmuWindow, buffer, "Cxbx", MB_ICONSTOP | MB_OKCANCEL) == IDOK)
			ExitProcess(1);
	}

    return EXCEPTION_CONTINUE_SEARCH;
}


// ******************************************************************
// * func: ExitException
// ******************************************************************
int ExitException(LPEXCEPTION_POINTERS e)
{
    if(EmuIsXboxFS())
        EmuSwapFS();

    static int count = 0;

    // ******************************************************************
	// * Debugging Information
	// ******************************************************************
	{
		printf("Emu (0x%X): * * * * * EXCEPTION * * * * *\n", GetCurrentThreadId());
		printf("Emu (0x%X): Recieved Exception [0x%.08X]@0x%.08X\n", GetCurrentThreadId(), e->ExceptionRecord->ExceptionCode, e->ContextRecord->Eip);
		printf("Emu (0x%X): * * * * * EXCEPTION * * * * *\n", GetCurrentThreadId());
	}

    fflush(stdout);

    MessageBox(XTL::g_hEmuWindow, "Warning: Could not safely terminate process!", "Cxbx", MB_OK);

    count++;

    if(count > 1)
    {
        MessageBox(XTL::g_hEmuWindow, "Warning: Multiple Problems!", "Cxbx", MB_OK);
        return EXCEPTION_CONTINUE_SEARCH;
    }

    ExitProcess(1);

    return EXCEPTION_CONTINUE_SEARCH;
}
