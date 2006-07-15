// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEIntercept.cpp
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

#include "CxbxKrnl.h"
#include "Emu.h"
#include "EmuFS.h"
#include "EmuXTL.h"
#include "EmuShared.h"
#include "HLEDataBase.h"
#include "HLEIntercept.h"

static void *EmuLocateFunction(OOVPA *Oovpa, uint32 lower, uint32 upper);
static void  EmuInstallWrappers(OOVPATable *OovpaTable, uint32 OovpaTableSize, Xbe::Header *pXbeHeader);
static void  EmuXRefFailure();

#include <shlobj.h>
#include <vector>

static std::vector<void *> vCacheOut;

static bool bCacheInp = false;
static std::vector<void *> vCacheInp;
static std::vector<void*>::const_iterator vCacheInpIter;

void EmuHLEIntercept(Xbe::LibraryVersion *pLibraryVersion, Xbe::Header *pXbeHeader)
{
    Xbe::Certificate *pCertificate = (Xbe::Certificate*)pXbeHeader->dwCertificateAddr;

    char szCacheFileName[260];

    DbgPrintf("\n");
    DbgPrintf("*******************************************************************************\n");
    DbgPrintf("* Cxbx High Level Emulation database last modified %s\n", szHLELastCompileTime);
    DbgPrintf("*******************************************************************************\n");
    DbgPrintf("\n");

    //
    // initialize HLE cache file
    //

    {
        SHGetSpecialFolderPath(NULL, szCacheFileName, CSIDL_APPDATA, TRUE);

        strcat(szCacheFileName, "\\Cxbx\\");

        CreateDirectory(szCacheFileName, NULL);

        sint32 spot = -1;

        for(int v=0;v<260;v++)
        {
            if(szCacheFileName[v] == '\\') { spot = v; }
            else if(szCacheFileName[v] == '\0') { break; }
        }

        if(spot != -1) { szCacheFileName[spot] = '\0'; }

        //
        // create HLECache directory
        //

        strcpy(&szCacheFileName[spot], "\\HLECache");

        CreateDirectory(szCacheFileName, NULL);

        //
        // open title's cache file
        //

        sprintf(&szCacheFileName[spot+9], "\\%08x.dat", pCertificate->dwTitleId);

        FILE *pCacheFile = fopen(szCacheFileName, "rb");

        if(pCacheFile != NULL)
        {
            bool bVerified = false;

            //
            // verify last compiled timestamp
            //

            char szCacheLastCompileTime[64];

            memset(szCacheLastCompileTime, 0, 64);

            if(fread(szCacheLastCompileTime, 64, 1, pCacheFile) == 1)
            {
                if(strcmp(szCacheLastCompileTime, szHLELastCompileTime) == 0)
                {
                    bVerified = true;
                }
            }

            //
            // load function addresses
            //

            if(bVerified)
            {
                while(true)
                {
                    void *cur;

                    if(fread(&cur, 4, 1, pCacheFile) != 1)
                        break;

                    vCacheInp.push_back(cur);
                }

                bCacheInp = true;

                vCacheInpIter = vCacheInp.begin();

                DbgPrintf("HLE: Loaded HLE Cache for 0x%.08X\n", pCertificate->dwTitleId);
            }

            fclose(pCacheFile);
        }
    }

    //
    // initialize openxdk emulation (TODO)
    //

    if(pLibraryVersion == 0)
    {
        DbgPrintf("HLE: Detected OpenXDK application...\n");
    }

    //
    // initialize Microsoft XDK emulation
    //

    if(pLibraryVersion != 0)
    {
        DbgPrintf("HLE: Detected Microsoft XDK application...\n");

        uint32 dwLibraryVersions = pXbeHeader->dwLibraryVersions;
        uint32 dwHLEEntries = HLEDataBaseSize / sizeof(HLEData);

        uint32 LastUnResolvedXRefs = UnResolvedXRefs+1;
        uint32 OrigUnResolvedXRefs = UnResolvedXRefs;

        for(int p=0;UnResolvedXRefs < LastUnResolvedXRefs;p++)
        {
            DbgPrintf("HLE: Starting pass #%d...\n", p+1);

            LastUnResolvedXRefs = UnResolvedXRefs;

            bool bFoundD3D = false;
            for(uint32 v=0;v<dwLibraryVersions;v++)
            {
                uint16 MajorVersion = pLibraryVersion[v].wMajorVersion;
                uint16 MinorVersion = pLibraryVersion[v].wMinorVersion;
                uint16 BuildVersion = pLibraryVersion[v].wBuildVersion;
                uint16 OrigBuildVersion = BuildVersion;

                // aliases
                if(BuildVersion == 4928) { BuildVersion = 4627; }
                if(BuildVersion == 5659) { BuildVersion = 5558; }

                char szLibraryName[9] = {0};

                for(uint32 c=0;c<8;c++)
                {
                    szLibraryName[c] = pLibraryVersion[v].szName[c];
                }

                // TODO: HACK: These libraries are packed into one database
                if(strcmp(szLibraryName, "D3DX8") == 0)
                {
                    strcpy(szLibraryName, "D3D8");
                }

                if(strcmp(szLibraryName, "D3D8") == 0)
                {
                    if(bFoundD3D)
                    {
                        //DbgPrintf("Redundant\n");
                        continue;
                    }

                    bFoundD3D = true;
                }

                if(bXRefFirstPass)
                {
                    if(strcmp("XAPILIB", szLibraryName) == 0 && MajorVersion == 1 && MinorVersion == 0 && 
                        (BuildVersion == 3911 || BuildVersion == 4034 || BuildVersion == 4134 || BuildVersion == 4361
                      || BuildVersion == 4432 || BuildVersion == 4627 || BuildVersion == 5558 || BuildVersion == 5849))
                    {
                        uint32 lower = pXbeHeader->dwBaseAddr;
                        uint32 upper = pXbeHeader->dwBaseAddr + pXbeHeader->dwSizeofImage;

                        // locate XapiProcessHeap
                        {
                            void *pFunc = 0;
                            uint ProcessHeapOffs;
                            uint RtlCreateHeapOffs;

                            if(BuildVersion >= 5849)
                            {
                                pFunc = EmuLocateFunction((OOVPA*)&XapiInitProcess_1_0_5849, lower, upper);
                                ProcessHeapOffs = 0x51;
                                RtlCreateHeapOffs = 0x4A;
                            }
                            else if(BuildVersion >= 5558)
                            {
                                pFunc = EmuLocateFunction((OOVPA*)&XapiInitProcess_1_0_5558, lower, upper);

                                // 5659 has an updated function
                                if(pFunc == 0)
                                {
                                    pFunc = EmuLocateFunction((OOVPA*)&XapiInitProcess_1_0_5659, lower, upper);
                                }

                                ProcessHeapOffs = 0x51;
                                RtlCreateHeapOffs = 0x4A;
                            }
                            else if(BuildVersion >= 4361)
                            {
                                if(OrigBuildVersion == 4928)
                                {
					                pFunc = EmuLocateFunction((OOVPA*)&XapiInitProcess_1_0_4928, lower, upper);
                                    ProcessHeapOffs = 0x44;
                                    RtlCreateHeapOffs = 0x3B;
                                }
                                else
                                {
					                pFunc = EmuLocateFunction((OOVPA*)&XapiInitProcess_1_0_4361, lower, upper);
                                    ProcessHeapOffs = 0x3E;
                                    RtlCreateHeapOffs = 0x37;
                                }
                            }
                            else // 3911, 4034, 4134
                            {
                                pFunc = EmuLocateFunction((OOVPA*)&XapiInitProcess_1_0_3911, lower, upper);
                                ProcessHeapOffs = 0x3E;
                                RtlCreateHeapOffs = 0x37;
                            }

                            if(pFunc != 0)
                            {
                                XTL::EmuXapiProcessHeap = *(PVOID**)((uint32)pFunc + ProcessHeapOffs);

                                XTL::g_pRtlCreateHeap = *(XTL::pfRtlCreateHeap*)((uint32)pFunc + RtlCreateHeapOffs);
                                XTL::g_pRtlCreateHeap = (XTL::pfRtlCreateHeap)((uint32)pFunc + (uint32)XTL::g_pRtlCreateHeap + RtlCreateHeapOffs + 0x04);

                                DbgPrintf("HLE: 0x%.08X -> EmuXapiProcessHeap\n", XTL::EmuXapiProcessHeap);
                                DbgPrintf("HLE: 0x%.08X -> g_pRtlCreateHeap\n", XTL::g_pRtlCreateHeap);
                            }
                        }
                    }
                    else if(strcmp("D3D8", szLibraryName) == 0 && MajorVersion == 1 && MinorVersion == 0 &&
                        (BuildVersion == 3925 || BuildVersion == 4134 || BuildVersion == 4361 || BuildVersion == 4432 
                      || BuildVersion == 4627 || BuildVersion == 5558 || BuildVersion == 5849))
                    {
                        uint32 lower = pXbeHeader->dwBaseAddr;
                        uint32 upper = pXbeHeader->dwBaseAddr + pXbeHeader->dwSizeofImage;

                        void *pFunc = 0;
                        
                        if(BuildVersion == 3925)
                            pFunc = EmuLocateFunction((OOVPA*)&IDirect3DDevice8_SetRenderState_CullMode_1_0_3925, lower, upper);
                        else if(BuildVersion < 5558)
                            pFunc = EmuLocateFunction((OOVPA*)&IDirect3DDevice8_SetRenderState_CullMode_1_0_4134, lower, upper);
                        else
                            pFunc = EmuLocateFunction((OOVPA*)&IDirect3DDevice8_SetRenderState_CullMode_1_0_5558, lower, upper);

                        // locate D3DDeferredRenderState
                        if(pFunc != 0)
                        {
                            // offset for stencil cull enable render state in the deferred render state buffer
                            int patchOffset = 0;

                            if(BuildVersion == 3925)
                            {
                                XTL::EmuD3DDeferredRenderState = (DWORD*)(*(DWORD*)((uint32)pFunc + 0x25) - 0x19F + 72*4);  // TODO: Clean up (?)
                                patchOffset = 142*4 - 72*4; // TODO: Verify
                            }
                            else if(BuildVersion == 4134)
                            {
                                XTL::EmuD3DDeferredRenderState = (DWORD*)(*(DWORD*)((uint32)pFunc + 0x2B) - 0x248 + 82*4);  // TODO: Verify
                                patchOffset = 142*4 - 82*4;
                            }
                            else if(BuildVersion == 4361)
                            {
                                XTL::EmuD3DDeferredRenderState = (DWORD*)(*(DWORD*)((uint32)pFunc + 0x2B) - 0x200 + 82*4);
                                patchOffset = 142*4 - 82*4;
                            }
                            else if(BuildVersion == 4432)
                            {
                                XTL::EmuD3DDeferredRenderState = (DWORD*)(*(DWORD*)((uint32)pFunc + 0x2B) - 0x204 + 83*4);
                                patchOffset = 143*4 - 83*4;
                            }
                            else if(BuildVersion == 4627)
                            {
                                XTL::EmuD3DDeferredRenderState = (DWORD*)(*(DWORD*)((uint32)pFunc + 0x2B) - 0x24C + 92*4);
                                patchOffset = 162*4 - 92*4;
                            }
                            else if(BuildVersion == 5558 || BuildVersion == 5849)
                            {
                                // WARNING: Not thoroughly tested (just seemed very correct right away)
                                XTL::EmuD3DDeferredRenderState = (DWORD*)(*(DWORD*)((uint32)pFunc + 0x2B) - 0x24C + 92*4);
                                patchOffset = 162*4 - 92*4;
                            }

                            XRefDataBase[XREF_D3DDEVICE]                   = *(DWORD*)((DWORD)pFunc + 0x03);
                            XRefDataBase[XREF_D3DRS_STENCILCULLENABLE]     = (uint32)XTL::EmuD3DDeferredRenderState + patchOffset + 0*4;
                            XRefDataBase[XREF_D3DRS_ROPZCMPALWAYSREAD]     = (uint32)XTL::EmuD3DDeferredRenderState + patchOffset + 1*4;
                            XRefDataBase[XREF_D3DRS_ROPZREAD]              = (uint32)XTL::EmuD3DDeferredRenderState + patchOffset + 2*4;
                            XRefDataBase[XREF_D3DRS_DONOTCULLUNCOMPRESSED] = (uint32)XTL::EmuD3DDeferredRenderState + patchOffset + 3*4;

                            for(int v=0;v<44;v++)
                            {
                                XTL::EmuD3DDeferredRenderState[v] = X_D3DRS_UNK;
                            }

                            DbgPrintf("HLE: 0x%.08X -> EmuD3DDeferredRenderState\n", XTL::EmuD3DDeferredRenderState);
                        }
                        else
                        {
                            XTL::EmuD3DDeferredRenderState = 0;
                            EmuWarning("EmuD3DDeferredRenderState was not found!");
                        }

                        // locate D3DDeferredTextureState
                        {
                            pFunc = 0;

                            if(BuildVersion == 3925)
                                pFunc = EmuLocateFunction((OOVPA*)&IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_3925, lower, upper);
                            else if(BuildVersion == 4134)
                                pFunc = EmuLocateFunction((OOVPA*)&IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_4134, lower, upper);
                            else if(BuildVersion == 4361 || BuildVersion == 4432)
                                pFunc = EmuLocateFunction((OOVPA*)&IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_4361, lower, upper);
                            else if(BuildVersion == 4627 || BuildVersion == 5558 || BuildVersion == 5849)
                                pFunc = EmuLocateFunction((OOVPA*)&IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_4627, lower, upper);

                            if(pFunc != 0)
                            {
                                if(BuildVersion == 3925) // 0x18F180
                                    XTL::EmuD3DDeferredTextureState = (DWORD*)(*(DWORD*)((uint32)pFunc + 0x11) - 0x70); // TODO: Verify
                                else if(BuildVersion == 4134)
                                    XTL::EmuD3DDeferredTextureState = (DWORD*)(*(DWORD*)((uint32)pFunc + 0x18) - 0x70); // TODO: Verify
                                else
                                    XTL::EmuD3DDeferredTextureState = (DWORD*)(*(DWORD*)((uint32)pFunc + 0x19) - 0x70);

                                for(int s=0;s<4;s++)
                                {
                                    for(int v=0;v<32;v++)
                                        XTL::EmuD3DDeferredTextureState[v+s*32] = X_D3DTSS_UNK;
                                }

                                DbgPrintf("HLE: 0x%.08X -> EmuD3DDeferredTextureState\n", XTL::EmuD3DDeferredTextureState);
                            }
                            else
                            {
                                XTL::EmuD3DDeferredTextureState = 0;
                                CxbxKrnlCleanup("EmuD3DDeferredTextureState was not found!");
                            }
                        }
                    }
                }

                DbgPrintf("HLE: * Searching HLE database for %s %d.%d.%d...", pLibraryVersion[v].szName, MajorVersion, MinorVersion, BuildVersion);

                bool found=false;

                for(uint32 d=0;d<dwHLEEntries;d++)
                {
                    if(BuildVersion != HLEDataBase[d].BuildVersion || MinorVersion != HLEDataBase[d].MinorVersion || MajorVersion != HLEDataBase[d].MajorVersion || strcmp(szLibraryName, HLEDataBase[d].Library) != 0)
                        continue;

                    found = true;

                    DbgPrintf("Found\n");

                    EmuInstallWrappers(HLEDataBase[d].OovpaTable, HLEDataBase[d].OovpaTableSize, pXbeHeader);
                }

                if(!found) { DbgPrintf("Skipped\n"); }
            }

            bXRefFirstPass = false;
        }

        // display Xref summary
        DbgPrintf("HLE: Resolved %d cross reference(s)\n", OrigUnResolvedXRefs - UnResolvedXRefs);
    }

    vCacheInp.empty();

    //
    // update cache file
    //

    if(vCacheOut.size() > 0)
    {
        FILE *pCacheFile = fopen(szCacheFileName, "wb");

        if(pCacheFile != NULL)
        {
            DbgPrintf("HLE: Saving HLE Cache for 0x%.08X...\n", pCertificate->dwTitleId);

            //
            // write last compiled timestamp
            //

            char szCacheLastCompileTime[64];

            memset(szCacheLastCompileTime, 0, 64);

            strcpy(szCacheLastCompileTime, szHLELastCompileTime);

            fwrite(szCacheLastCompileTime, 64, 1, pCacheFile);

            //
            // write function addresses
            //

            std::vector<void*>::const_iterator cur;

            for(cur = vCacheOut.begin();cur != vCacheOut.end(); ++cur)
            {
                fwrite(&(*cur), 4, 1, pCacheFile);
            }
        }

        fclose(pCacheFile);
    }

    vCacheOut.empty();

    DbgPrintf("\n");

    return;
}

// install function interception wrapper
static inline void EmuInstallWrapper(void *FunctionAddr, void *WrapperAddr)
{
    uint08 *FuncBytes = (uint08*)FunctionAddr;

    *(uint08*)&FuncBytes[0] = 0xE9;
    *(uint32*)&FuncBytes[1] = (uint32)WrapperAddr - (uint32)FunctionAddr - 5;
}

// locate the given function, searching within lower and upper bounds
static void *EmuLocateFunction(OOVPA *Oovpa, uint32 lower, uint32 upper)
{
    uint32 count = Oovpa->Count;

    // Skip out if this is an unnecessary search
    if(!bXRefFirstPass && Oovpa->XRefCount == 0 && Oovpa->XRefSaveIndex == (uint08)-1)
        return 0;

    // large
    if(Oovpa->Large == 1)
    {
        LOOVPA<1> *Loovpa = (LOOVPA<1>*)Oovpa;

        upper -= Loovpa->Lovp[count-1].Offset;

        // search all of the image memory
        for(uint32 cur=lower;cur<upper;cur++)
        {
            uint32 v;

            // check all cross references
            for(v=0;v<Loovpa->XRefCount;v++)
            {
                uint32 Offset = Loovpa->Lovp[v].Offset;
                uint32 Value  = Loovpa->Lovp[v].Value;

                uint32 RealValue = *(uint32*)(cur + Offset);

                if(XRefDataBase[Value] == -1)
                    goto skipout_L;   // unsatisfied Xref is not acceptable

                if((RealValue + cur + Offset+4 != XRefDataBase[Value]) && (RealValue != XRefDataBase[Value]))
                    break;
            }

            // check all pairs, moving on if any do not match
            for(v=0;v<count;v++)
            {
                uint32 Offset = Loovpa->Lovp[v].Offset;
                uint32 Value  = Loovpa->Lovp[v].Value;

                uint08 RealValue = *(uint08*)(cur + Offset);

                if(RealValue != Value)
                    break;
            }

            // success if we found all pairs
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
                    {
                        return (void*)XRefDataBase[Loovpa->XRefSaveIndex];   // already Found, no bother patching again
                    }
                }

                return (void*)cur;
            }

            skipout_L:;
        }
    }
    // small
    else
    {
        SOOVPA<1> *Soovpa = (SOOVPA<1>*)Oovpa;

        upper -= Soovpa->Sovp[count-1].Offset;

        // search all of the image memory
        for(uint32 cur=lower;cur<upper;cur++)
        {
            uint32 v;

            // check all cross references
            for(v=0;v<Soovpa->XRefCount;v++)
            {
                uint32 Offset = Soovpa->Sovp[v].Offset;
                uint32 Value  = Soovpa->Sovp[v].Value;

                uint32 RealValue = *(uint32*)(cur + Offset);

                if(XRefDataBase[Value] == -1)
                    goto skipout_S;   // Unsatisfied XRef is not acceptable

                if( (RealValue + cur + Offset + 4 != XRefDataBase[Value]) && (RealValue != XRefDataBase[Value]))
                    break;
            }

            // check OV pairs if all xrefs matched
            if(v == Soovpa->XRefCount)
            {
                // check all pairs, moving on if any do not match
                for(;v<count;v++)
                {
                    uint32 Offset = Soovpa->Sovp[v].Offset;
                    uint32 Value  = Soovpa->Sovp[v].Value;

                    uint08 RealValue = *(uint08*)(cur + Offset);

                    if(RealValue != Value)
                        break;
                }
            }

            // success if we found all pairs
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
                    {
                        return (void*)XRefDataBase[Soovpa->XRefSaveIndex];   // already Found, no bother patching again
                    }
                }

                return (void*)cur;
            }

            skipout_S:;
        }
    }

    return 0;
}

// install function interception wrappers
static void EmuInstallWrappers(OOVPATable *OovpaTable, uint32 OovpaTableSize, Xbe::Header *pXbeHeader)
{
    uint32 lower = pXbeHeader->dwBaseAddr;
    uint32 upper = pXbeHeader->dwBaseAddr + pXbeHeader->dwSizeofImage;

    // traverse the full OOVPA table
    for(uint32 a=0;a<OovpaTableSize/sizeof(OOVPATable);a++)
    {
        OOVPA *Oovpa = OovpaTable[a].Oovpa;

        void *pFunc = NULL;

        if(bCacheInp && (vCacheInpIter != vCacheInp.end()))
        {
            pFunc = (*vCacheInpIter);

            ++vCacheInpIter;
        }
        else
        {
            pFunc = EmuLocateFunction(Oovpa, lower, upper);
            vCacheOut.push_back(pFunc);
        }

        if(pFunc != 0)
        {
            #ifdef _DEBUG_TRACE
            DbgPrintf("HLE: 0x%.08X -> %s\n", pFunc, OovpaTable[a].szFuncName);
            #endif

            if(OovpaTable[a].lpRedirect == 0)
            {
                EmuInstallWrapper(pFunc, EmuXRefFailure);
            }
            else
            {
                EmuInstallWrapper(pFunc, OovpaTable[a].lpRedirect);
            }
        }
    }
}

// alert for the situation where an Xref function body is hit
static void EmuXRefFailure()
{
    EmuSwapFS();    // Win2k/XP FS

    CxbxKrnlCleanup("XRef-only function body reached. Fatal Error.");
}

