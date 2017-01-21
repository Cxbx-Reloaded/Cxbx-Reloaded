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

#include <cmath>
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

uint32 fcount = 0;
uint32 funcExclude[2048] = {0};

uint32 g_BuildVersion;
uint32 g_OrigBuildVersion;

static std::vector<void *> vCacheOut;

static bool bCacheInp = false;
static std::vector<void *> vCacheInp;
static std::vector<void*>::const_iterator vCacheInpIter;


bool bLLE_APU = false; // Set this to true for experimental APU (sound) LLE
bool bLLE_GPU = false; // Set this to true for experimental GPU (graphics) LLE
bool bLLE_JIT = false; // Set this to true for experimental JIT

bool bXRefFirstPass; // For search speed optimization, set in EmuHLEIntercept, read in EmuLocateFunction
uint32 UnResolvedXRefs; // Tracks XRef location, used (read/write) in EmuHLEIntercept and EmuLocateFunction

void EmuHLEIntercept(Xbe::LibraryVersion *pLibraryVersion, Xbe::Header *pXbeHeader)
{
    Xbe::Certificate *pCertificate = (Xbe::Certificate*)pXbeHeader->dwCertificateAddr;

    char szCacheFileName[MAX_PATH];

    DbgPrintf("\n");
    DbgPrintf("*******************************************************************************\n");
    DbgPrintf("* Cxbx-Reloaded High Level Emulation database last modified %s\n", szHLELastCompileTime);
    DbgPrintf("*******************************************************************************\n");
    DbgPrintf("\n");

    //
    // initialize HLE cache file
    //

	{
		SHGetSpecialFolderPath(NULL, szCacheFileName, CSIDL_APPDATA, TRUE);

		strcat(szCacheFileName, "\\Cxbx-Reloaded\\");

		CreateDirectory(szCacheFileName, NULL);

		char *spot = strrchr(szCacheFileName, '\\');

		//
		// create HLECache directory
		//

		strcpy(spot, "\\HLECache");

		CreateDirectory(szCacheFileName, NULL);
		
		//
        // open title's cache file
        //
		sprintf(spot+9, "\\%08x.dat", pCertificate->dwTitleId);

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
    // initialize Microsoft XDK emulation
    //

    if(pLibraryVersion != 0)
    {
        DbgPrintf("HLE: Detected Microsoft XDK application...\n");

		UnResolvedXRefs = XREF_COUNT; // = sizeof(XRefDataBase) / sizeof(uint32)

        uint32 dwLibraryVersions = pXbeHeader->dwLibraryVersions;
        uint32 LastUnResolvedXRefs = UnResolvedXRefs+1;
        uint32 OrigUnResolvedXRefs = UnResolvedXRefs;

		bool bFoundD3D = false;

		bXRefFirstPass = true; // Set to false for search speed optimization

		memset((void*)XRefDataBase, XREF_UNKNOWN, sizeof(XRefDataBase));


		for(int p=0;UnResolvedXRefs < LastUnResolvedXRefs;p++)
        {
            DbgPrintf("HLE: Starting pass #%d...\n", p+1);

            LastUnResolvedXRefs = UnResolvedXRefs;

            for(uint32 v=0;v<dwLibraryVersions;v++)
            {
                uint16 BuildVersion = pLibraryVersion[v].wBuildVersion;
                uint16 OrigBuildVersion = BuildVersion;

                // Aliases - for testing purposes only
				if(BuildVersion == 4039) { BuildVersion = 4034; }
				if(BuildVersion == 4238) { BuildVersion = 4361; }	// I don't think this XDK was released.
				if(BuildVersion == 4242) { BuildVersion = 4361; }
				if(BuildVersion == 4400) { BuildVersion = 4361; }
				if(BuildVersion == 4531) { BuildVersion = 4432; }
                if(BuildVersion == 4721) { BuildVersion = 4627; }
				if(BuildVersion == 4831) { BuildVersion = 4627; }
                if(BuildVersion == 4928) { BuildVersion = 4627; }
                if(BuildVersion == 5344) { BuildVersion = 5233; }
                if(BuildVersion == 5455) { BuildVersion = 5558; }
                if(BuildVersion == 5659) { BuildVersion = 5558; }
                if(BuildVersion == 5028) { BuildVersion = 4627; }
				if(BuildVersion == 5120) { BuildVersion = 5233; }
                if(BuildVersion == 5933) { BuildVersion = 5849; }   // These XDK versions are pretty much the same
                /*
                if(BuildVersion == 3944) { BuildVersion = 3925; }
                if(BuildVersion == 4039) { BuildVersion = 4034; }
                if(BuildVersion == 4242) { BuildVersion = 4432; }
                if(BuildVersion == 4531) { BuildVersion = 4432; }
                if(BuildVersion == 4721) { BuildVersion = 4432; }
                if(BuildVersion == 4831) { BuildVersion = 4432; }
                if(BuildVersion == 4928) { BuildVersion = 4432; }
                if(BuildVersion == 5028) { BuildVersion = 4432; }
                if(BuildVersion == 5120) { BuildVersion = 4432; }
                if(BuildVersion == 5344) { BuildVersion = 4432; }
                if(BuildVersion == 5455) { BuildVersion = 4432; }
                if(BuildVersion == 5933) { BuildVersion = 4432; }
                */

                char szLibraryName[9] = {0};
				char szOrigLibraryName[9] = {0};

                for(uint32 c=0;c<8;c++)
                {
                    szLibraryName[c] = pLibraryVersion[v].szName[c];
					szOrigLibraryName[c] = pLibraryVersion[v].szName[c];
                }

				// TODO: HACK: D3DX8 is packed into D3D8 database
				if (strcmp(szLibraryName, Lib_D3DX8) == 0)
				{
					strcpy(szLibraryName, Lib_D3D8);
				}

				if(strcmp(szLibraryName, Lib_D3D8LTCG) == 0)
				{
					// Skip scanning for D3D8LTCG symbols when LLE GPU is selected
					if (bLLE_GPU)
						continue;

					// Test (do not release uncommented!)
					/*strcpy(szLibraryName, Lib_D3D8);*/
				}
				
				if (strcmp(szLibraryName, Lib_D3D8) == 0)
				{
					// Skip scanning for D3D8 symbols when LLE GPU is selected
					if (bLLE_GPU)
						continue;

					// Prevent scanning D3D8 again (since D3D8X is packed into it above)
					if (bFoundD3D)
					{
						//DbgPrintf("Redundant\n");
						continue;
					}

					bFoundD3D = true;

					// Some 3911 titles have different D3D8 builds
					if (BuildVersion <= 3948)
						BuildVersion = 3925;

					// Testing... don't release with this code in it!
					// TODO: 5233 and 5558
					//	if(BuildVersion == 4134)
					//		BuildVersion = 4627;
				}
				else if(strcmp(szLibraryName, Lib_DSOUND) == 0)
                {
					// Skip scanning for DSOUND symbols when LLE APU is selected
					if (bLLE_APU)
						continue;

					// Several 3911 titles has different DSound builds.
					if(BuildVersion < 4034)
                    {
                        BuildVersion = 3936;
                    }

					// Redirect other highly similar DSOUND library versions
					if(BuildVersion == 4361 || BuildVersion == 4400 || BuildVersion == 4432 || 
						BuildVersion == 4531 )
						BuildVersion = 4627;
                }
				else if(strcmp(szLibraryName, Lib_XAPILIB) == 0)
				{
					// Change a few XAPILIB versions to similar counterparts
					if(BuildVersion == 3944)
						BuildVersion = 3911;
					if(BuildVersion == 3950)
						BuildVersion = 3911;
					if(OrigBuildVersion == 4531)
						BuildVersion = 4627;
				}
				else if (strcmp(szLibraryName, Lib_XGRAPHC) == 0)
				{
					// Skip scanning for XGRAPHC (XG) symbols when LLE GPU is selected
					if (bLLE_GPU)
						continue;

					//	if(BuildVersion == 4432)
					//		BuildVersion = 4361;
					if (BuildVersion == 3944)
						BuildVersion = 3911;
					if (OrigBuildVersion == 4531)
						BuildVersion = 4361;
					// Quick test (JSRF)
					if (OrigBuildVersion == 4134)
						BuildVersion = 4361;
					// Quick test (Simpsons: RoadRage)
					//	if(BuildVersion == 4034)
					//		BuildVersion = 3911;
				}

				if(bXRefFirstPass)
                {
                    if(strcmp(Lib_XAPILIB, szLibraryName) == 0 && 
                        (BuildVersion == 3911 || BuildVersion == 4034 || BuildVersion == 4134 || BuildVersion == 4361
                      || BuildVersion == 4432 || BuildVersion == 4627 || BuildVersion == 5233 || BuildVersion == 5558
                      || BuildVersion == 5788 || BuildVersion == 5849))
                    {
                        uint32 lower = pXbeHeader->dwBaseAddr;
                        uint32 upper = pXbeHeader->dwBaseAddr + pXbeHeader->dwSizeofImage;
                    }
                    else if(strcmp(Lib_D3D8, szLibraryName) == 0 /*&& strcmp(Lib_D3D8LTCG, szOrigLibraryName)*/ && 
                        (BuildVersion == 3925 || BuildVersion == 4134 || BuildVersion == 4361 || BuildVersion == 4432
                      || BuildVersion == 4627 || BuildVersion == 5233 || BuildVersion == 5558 || BuildVersion == 5788
                      || BuildVersion == 5849))
                    {
						// Save D3D8 build version
						g_BuildVersion = BuildVersion;
						g_OrigBuildVersion = OrigBuildVersion;

                        uint32 lower = pXbeHeader->dwBaseAddr;
                        uint32 upper = pXbeHeader->dwBaseAddr + pXbeHeader->dwSizeofImage;

                        void *pFunc = nullptr;

                        if(BuildVersion == 3925)
                            pFunc = EmuLocateFunction((OOVPA*)&D3DDevice_SetRenderState_CullMode_3925, lower, upper);
                        else if(BuildVersion < 5233)
                            pFunc = EmuLocateFunction((OOVPA*)&D3DDevice_SetRenderState_CullMode_4034, lower, upper);
                        else
                            pFunc = EmuLocateFunction((OOVPA*)&D3DDevice_SetRenderState_CullMode_5233, lower, upper);

                        // locate D3DDeferredRenderState
                        if(pFunc != nullptr)
                        {
                            // offset for stencil cull enable render state in the deferred render state buffer
                            int patchOffset = 0;

                            if(BuildVersion == 3925)
							{
								XTL::EmuD3DDeferredRenderState = (DWORD*)(*(DWORD*)((uint32)pFunc + 0x25) - 0x1FC + 82*4);  // TODO: Clean up (?)
								patchOffset = 142*4 - 82*4; // TODO: Verify
								
								//XTL::EmuD3DDeferredRenderState = (DWORD*)(*(DWORD*)((uint32)pFunc + 0x25) - 0x19F + 72*4);  // TODO: Clean up (?)
								//patchOffset = 142*4 - 72*4; // TODO: Verify
                            }
                            else if(BuildVersion == 4034 || BuildVersion == 4134)
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
                            else if(BuildVersion == 4627 || BuildVersion == 5233 || BuildVersion == 5558 || BuildVersion == 5788
                                 || BuildVersion == 5849)
                            {
                                // WARNING: Not thoroughly tested (just seemed very correct right away)
                                XTL::EmuD3DDeferredRenderState = (DWORD*)(*(DWORD*)((uint32)pFunc + 0x2B) - 0x24C + 92*4);
                                patchOffset = 162*4 - 92*4;
                            }

                            XRefDataBase[XREF_D3DDEVICE]                   = *(DWORD*)((DWORD)pFunc + 0x03);
                            XRefDataBase[XREF_D3DRS_MULTISAMPLEMODE]       = (uint32)XTL::EmuD3DDeferredRenderState + patchOffset - 8*4;
                            XRefDataBase[XREF_D3DRS_MULTISAMPLERENDERTARGETMODE] = (uint32)XTL::EmuD3DDeferredRenderState + patchOffset - 7*4;
                            XRefDataBase[XREF_D3DRS_STENCILCULLENABLE]     = (uint32)XTL::EmuD3DDeferredRenderState + patchOffset + 0*4;
                            XRefDataBase[XREF_D3DRS_ROPZCMPALWAYSREAD]     = (uint32)XTL::EmuD3DDeferredRenderState + patchOffset + 1*4;
                            XRefDataBase[XREF_D3DRS_ROPZREAD]              = (uint32)XTL::EmuD3DDeferredRenderState + patchOffset + 2*4;
                            XRefDataBase[XREF_D3DRS_DONOTCULLUNCOMPRESSED] = (uint32)XTL::EmuD3DDeferredRenderState + patchOffset + 3*4;

                            for(int v=0;v<44;v++)
                            {
                                XTL::EmuD3DDeferredRenderState[v] = X_D3DRS_UNK;
                            }

                            DbgPrintf("HLE: 0x%.08X -> EmuD3DDeferredRenderState\n", XTL::EmuD3DDeferredRenderState);
							//DbgPrintf("HLE: 0x%.08X -> XREF_D3DRS_ROPZCMPALWAYSREAD\n", XRefDataBase[XREF_D3DRS_ROPZCMPALWAYSREAD] );
                        }
                        else
                        {
                            XTL::EmuD3DDeferredRenderState = nullptr;
                            CxbxKrnlCleanup("EmuD3DDeferredRenderState was not found!");
                        }

                        // locate D3DDeferredTextureState
                        {
                            pFunc = nullptr;

                            if(BuildVersion == 3925)
                                pFunc = EmuLocateFunction((OOVPA*)&D3DDevice_SetTextureState_TexCoordIndex_3925, lower, upper);
                            else if(BuildVersion == 4134)
                                pFunc = EmuLocateFunction((OOVPA*)&D3DDevice_SetTextureState_TexCoordIndex_4134, lower, upper);
                            else if(BuildVersion == 4361 || BuildVersion == 4432)
                                pFunc = EmuLocateFunction((OOVPA*)&D3DDevice_SetTextureState_TexCoordIndex_4361, lower, upper);
                            else if(BuildVersion == 4627 || BuildVersion == 5233 || BuildVersion == 5558 || BuildVersion == 5788
                                 || BuildVersion == 5849)
                                pFunc = EmuLocateFunction((OOVPA*)&D3DDevice_SetTextureState_TexCoordIndex_4627, lower, upper);

                            if(pFunc != nullptr)
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
                                XTL::EmuD3DDeferredTextureState = nullptr;
                                CxbxKrnlCleanup("EmuD3DDeferredTextureState was not found!");
                            }
                        }
                    }
					//else if(strcmp(Lib_D3D8LTCG, szLibraryName) == 0 &&
     //                   (BuildVersion == 5849))	// 5849 only so far...
     //               {
					//	// Save D3D8 build version
					//	g_BuildVersion = BuildVersion;
					//	g_OrigBuildVersion = OrigBuildVersion;

     //                   uint32 lower = pXbeHeader->dwBaseAddr;
     //                   uint32 upper = pXbeHeader->dwBaseAddr + pXbeHeader->dwSizeofImage;

     //                   void *pFunc = nullptr;

     //                   if(BuildVersion == 5849)
					//		pFunc = EmuLocateFunction((OOVPA*)&D3DDevice_SetRenderState_CullMode_LTCG_5849, lower, upper);

     //                   // locate D3DDeferredRenderState
     //                   if(pFunc != nullptr)
     //                   {
     //                       // offset for stencil cull enable render state in the deferred render state buffer
     //                       int patchOffset = 0;

     //                       if(BuildVersion == 5849)
     //                       {
     //                           // WARNING: Not thoroughly tested (just seemed very correct right away)
     //                           XTL::EmuD3DDeferredRenderState = (DWORD*)(*(DWORD*)((uint32)pFunc + 0x2B) - 0x24C + 92*4);
     //                           patchOffset = 162*4 - 92*4;
     //                       }

     //                       XRefDataBase[XREF_D3DDEVICE]                   = *(DWORD*)((DWORD)pFunc + 0x03);
     //                       XRefDataBase[XREF_D3DRS_MULTISAMPLEMODE]       = (uint32)XTL::EmuD3DDeferredRenderState + patchOffset - 8*4;
     //                       XRefDataBase[XREF_D3DRS_MULTISAMPLERENDERTARGETMODE] = (uint32)XTL::EmuD3DDeferredRenderState + patchOffset - 7*4;
     //                       XRefDataBase[XREF_D3DRS_STENCILCULLENABLE]     = (uint32)XTL::EmuD3DDeferredRenderState + patchOffset + 0*4;
     //                       XRefDataBase[XREF_D3DRS_ROPZCMPALWAYSREAD]     = (uint32)XTL::EmuD3DDeferredRenderState + patchOffset + 1*4;
     //                       XRefDataBase[XREF_D3DRS_ROPZREAD]              = (uint32)XTL::EmuD3DDeferredRenderState + patchOffset + 2*4;
     //                       XRefDataBase[XREF_D3DRS_DONOTCULLUNCOMPRESSED] = (uint32)XTL::EmuD3DDeferredRenderState + patchOffset + 3*4;

     //                       for(int v=0;v<44;v++)
     //                       {
     //                           XTL::EmuD3DDeferredRenderState[v] = X_D3DRS_UNK;
     //                       }

     //                       DbgPrintf("HLE: 0x%.08X -> EmuD3DDeferredRenderState\n", XTL::EmuD3DDeferredRenderState);
     //                   }
     //                   else
     //                   {
     //                       XTL::EmuD3DDeferredRenderState = nullptr;
     //                       CxbxKrnlCleanup("EmuD3DDeferredRenderState was not found!");
     //                   }

     //                   // locate D3DDeferredTextureState
     //                   {
     //                       pFunc = nullptr;

     //                       if(BuildVersion == 3925)
					//			pFunc = EmuLocateFunction((OOVPA*)&D3DDevice_SetTextureState_TexCoordIndex_LTCG_5849, lower, upper);

     //                       if(pFunc != nullptr)
     //                       {
     //                           if(BuildVersion == 3925) // 0x18F180
     //                               XTL::EmuD3DDeferredTextureState = (DWORD*)(*(DWORD*)((uint32)pFunc + 0x11) - 0x70); // TODO: Verify
     //                           else if(BuildVersion == 4134)
     //                               XTL::EmuD3DDeferredTextureState = (DWORD*)(*(DWORD*)((uint32)pFunc + 0x18) - 0x70); // TODO: Verify
     //                           else
     //                               XTL::EmuD3DDeferredTextureState = (DWORD*)(*(DWORD*)((uint32)pFunc + 0x19) - 0x70);

     //                           for(int s=0;s<4;s++)
     //                           {
     //                               for(int v=0;v<32;v++)
     //                                   XTL::EmuD3DDeferredTextureState[v+s*32] = X_D3DTSS_UNK;
     //                           }

     //                           DbgPrintf("HLE: 0x%.08X -> EmuD3DDeferredTextureState\n", XTL::EmuD3DDeferredTextureState);
     //                       }
     //                       else
     //                       {
     //                           XTL::EmuD3DDeferredTextureState = nullptr;
     //                           CxbxKrnlCleanup("EmuD3DDeferredTextureState was not found!");
     //                       }
     //                   }
     //               }
                }

                DbgPrintf("HLE: * Searching HLE database for %s version 1.0.%d... ", szLibraryName, BuildVersion);

                const HLEData *FoundHLEData = nullptr;
                for(uint32 d = 0; d < HLEDataBaseCount; d++) {
					if (BuildVersion == HLEDataBase[d].BuildVersion && strcmp(szLibraryName, HLEDataBase[d].Library) == 0) {
						FoundHLEData = &HLEDataBase[d];
						break;
					}
                }

				if (FoundHLEData) {
					if (g_bPrintfOn) printf("Found\n");
					EmuInstallWrappers(FoundHLEData->OovpaTable, FoundHLEData->OovpaTableSize, pXbeHeader);
				} else {
					if (g_bPrintfOn) printf("Skipped\n");
				}
			}

            bXRefFirstPass = false;
        }

        // display Xref summary
        DbgPrintf("HLE: Resolved %d cross reference(s)\n", OrigUnResolvedXRefs - UnResolvedXRefs);
    }

    vCacheInp.clear();

    //
    // update cache file
    //
/* Turn of the nasty HLE cacheing (When you are adding oovaps anyway), it's in dire need of a better file identify system
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
*/
    vCacheOut.clear();

    DbgPrintf("\n");

    return;
}

// install function interception wrapper
static inline void EmuInstallWrapper(void *FunctionAddr, void *WrapperAddr)
{
    uint08 *FuncBytes = (uint08*)FunctionAddr;

    *(uint08*)&FuncBytes[0] = 0xE9; // = opcode for JMP rel32 (Jump near, relative, displacement relative to next instruction)
    *(uint32*)&FuncBytes[1] = (uint32)WrapperAddr - (uint32)FunctionAddr - 5;
}

static inline void GetOovpaEntry(OOVPA *oovpa, int index, OUT uint32 &offset, OUT uint08 &value)
{
	if (oovpa->Type == Large) {
		offset = (uint32) ((LOOVPA<1>*)oovpa)->Lovp[index].Offset;
		value = ((LOOVPA<1>*)oovpa)->Lovp[index].Value;
	}
	else {
		offset = (uint32) ((SOOVPA<1>*)oovpa)->Sovp[index].Offset;
		value = ((SOOVPA<1>*)oovpa)->Sovp[index].Value;
	}
}

// locate the given function, searching within lower and upper bounds
static void *EmuLocateFunction(OOVPA *Oovpa, uint32 lower, uint32 upper)
{
	uint32 xref_count = Oovpa->XRefCount;
    // skip out if this is an unnecessary search
    if(!bXRefFirstPass && xref_count == XRefZero && Oovpa->XRefSaveIndex == XRefNoSaveIndex)
        return nullptr;

	// correct upper bound with highest Oovpa offset
	uint32 count = Oovpa->Count;
	{
		uint32 Offset;
		uint08 Value; // ignored

		GetOovpaEntry(Oovpa, count - 1, Offset, Value);
		upper -= Offset;
	}

	// search all of the image memory
	for (uint32 cur = lower; cur < upper; cur++)
	{
		uint32 v; // verification counter

		// check all cross references
		for (v = 0; v < xref_count; v++)
		{
			uint32 Offset;
			uint08 Value;

			// get XRef offset + value pair and currently registered (un)known address
			GetOovpaEntry(Oovpa, v, Offset, Value);
			uint32 XRefValue = XRefDataBase[Value];

			// unknown XRef cannot be checked yet
			if (XRefValue == XREF_UNKNOWN)
				break;

			uint32 RealValue = *(uint32*)(cur + Offset);
			// check if PC-relative or direct reference matches XRef
			if ((RealValue + cur + Offset + 4 != XRefValue) && (RealValue != XRefValue))
				break;
		}

		// did all xrefs match?
		if (v == xref_count)
		{
			// check all OV pairs, moving on if any do not match
			for (; v < count; v++)
			{
				uint32 Offset;
				uint08 Value;

				GetOovpaEntry(Oovpa, v, Offset, Value);
				uint08 RealValue = *(uint08*)(cur + Offset);
				if (RealValue != Value)
					break;
			}

			// success if we found all pairs
			if (v == count)
			{
				// do we need to save the found address?
				if (Oovpa->XRefSaveIndex != XRefNoSaveIndex)
				{
					// is the XRef not saved yet?
					if (XRefDataBase[Oovpa->XRefSaveIndex] == XREF_UNKNOWN)
					{
						// save and count the found address
						UnResolvedXRefs--;
						XRefDataBase[Oovpa->XRefSaveIndex] = cur;
					}
					else
					{
						// TODO : Check identical result?
						// already found, no bother patching again
						return (void*)XRefDataBase[Oovpa->XRefSaveIndex];
					}
				}

				// return found address
				return (void*)cur;
			}
		}
	}

	// found nothing
    return nullptr;
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

        void *pFunc = nullptr;

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

        if(pFunc != nullptr)
        {
            #ifdef _DEBUG_TRACE
            DbgPrintf("HLE: 0x%.08X -> %s\n", pFunc, OovpaTable[a].szFuncName);
            #endif

            if(OovpaTable[a].lpRedirect == nullptr)
            {
				// Insert breakpoint
				*(uint8_t*)pFunc = 0xCC;
                EmuInstallWrapper((void*)(((uint32_t)pFunc)+1), EmuXRefFailure);
            }
            else
            {
                EmuInstallWrapper(pFunc, OovpaTable[a].lpRedirect);
                funcExclude[fcount++] = (uint32)pFunc;
            }
        }
    }
}

// alert for the situation where an Xref function body is hit
static void EmuXRefFailure()
{
    CxbxKrnlCleanup("XRef-only function body reached. Fatal Error.");
}

#ifdef _DEBUG_TRACE

struct HLEVerifyContext {
	const HLEData *main_data;
	OOVPA *oovpa, *against;
	const HLEData *against_data;
	uint32 main_index, against_index;
};

std::string HLEErrorString(const HLEData *data, uint32 index)
{
	std::string result = 
		"OOVPATable " + (std::string)(data->Library) + "_" + std::to_string(data->BuildVersion)
		+ "[" + std::to_string(index) + "] " 
		+ (std::string)(data->OovpaTable[index].szFuncName);

	return result;
}

void HLEError(HLEVerifyContext *context, char *format, ...)
{
	std::string output = "HLE Error ";
	if (context->main_data != nullptr)
		output += "in " + HLEErrorString(context->main_data, context->main_index);

	if (context->against != nullptr && context->against_data != nullptr)
		output += ", comparing against " + HLEErrorString(context->against_data, context->against_index);

	// format specific error message
	char buffer[200];
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);

	output += " : " + (std::string)buffer + (std::string)"\n";
	printf(output.c_str());
}

void VerifyHLEDataBaseAgainst(HLEVerifyContext *context); // forward

void VerifyHLEOOVPA(HLEVerifyContext *context, OOVPA *oovpa)
{
	if (context->against == nullptr) {
		// TODO : verify XRefSaveIndex and XRef's (how?)

		// verify offsets are in increasing order
		uint32 prev_offset;
		uint08 dummy_value;
		GetOovpaEntry(oovpa, oovpa->XRefCount, prev_offset, dummy_value);
		for (int p = oovpa->XRefCount + 1; p < oovpa->Count; p++) {
			uint32 curr_offset;
			GetOovpaEntry(oovpa, p, curr_offset, dummy_value);
			if (!(curr_offset > prev_offset)) {
				HLEError(context, "%s[%d] : Offset (0x%x) must be larger then previous offset (0x%x)",
					(oovpa->Type = Large) ? "Lovp" : "Sovp", p, curr_offset, prev_offset);
			}
		}

		// find duplicate OOVPA's across all other data-table-oovpa's
		context->oovpa = oovpa;
		context->against = oovpa;
		VerifyHLEDataBaseAgainst(context);
		context->against = nullptr; // reset scanning state
		return;
	}

	// prevent checking an oovpa against itself
	if (context->against == oovpa)
		return;

	// compare {Offset, Value}-pairs between two OOVPA's
	OOVPA *left = context->against, *right = oovpa;
	int l = 0, r = 0;
	uint32 left_offset, right_offset;
	uint08 left_value, right_value;
	GetOovpaEntry(left, l, left_offset, left_value);
	GetOovpaEntry(right, r, right_offset, right_value);
	int unique_offset_left = 0;
	int unique_offset_right = 0;
	int equal_offset_value = 0;
	int equal_offset_different_value = 0;
	while (true) {
		bool left_next = true;
		bool right_next = true;

		if (left_offset < right_offset) {
			unique_offset_left++;
			right_next = false;
		}
		else if (left_offset > right_offset) {
			unique_offset_right++;
			left_next = false;
		}
		else if (left_value == right_value) {
			equal_offset_value++;
		}
		else {
			equal_offset_different_value++;
		}

		// increment r before use (in left_next)
		if (right_next)
			r++;

		if (left_next) {
			l++;
			if (l >= left->Count) {
				unique_offset_right += right->Count - r;
				break;
			}

			GetOovpaEntry(left, l, left_offset, left_value);
		}

		if (right_next) {
			if (r >= right->Count) {
				unique_offset_left += left->Count - l;
				break;
			}

			GetOovpaEntry(right, r, right_offset, right_value);
		}
	}

	// no mismatching values on identical offsets?
	if (equal_offset_different_value == 0)
		// enough matching OV-pairs?
		if (equal_offset_value > 4)
		{
			// no unique OV-pairs on either side?
			if (unique_offset_left + unique_offset_right == 0)
				HLEError(context, "OOVPA's are identical",
					unique_offset_left,
					unique_offset_right);
			else
				// not too many new OV-pairs on the left side?
				if (unique_offset_left < 6)
					// not too many new OV-parirs on the right side?
					if (unique_offset_right < 6)
						HLEError(context, "OOVPA's are expanded (left +%d, right +%d)",
							unique_offset_left,
							unique_offset_right);
		}
}

void VerifyHLEDataEntry(HLEVerifyContext *context, const OOVPATable *table, uint32 index, uint32 count)
{
	if (context->against == nullptr) {
		context->main_index = index;
		// does this entry specify a redirection (patch)?
		void * entry_redirect = table[index].lpRedirect;
		if (entry_redirect != nullptr) {
			if (table[index].Oovpa == nullptr) {
				HLEError(context, "Patch without an OOVPA at index %d",
					index);
			} else
				// check no patch occurs twice in this table
				for (uint32 t = index + 1; t < count; t++) {
					if (entry_redirect == table[t].lpRedirect) {
						if (table[index].Oovpa == table[t].Oovpa) {
							HLEError(context, "Patch registered again (with same OOVPA) at index %d",
								t);
						} else {
							HLEError(context, "Patch also used for another OOVPA at index %d",
								t);
						}
					}
				}
		}
	}
	else
		context->against_index = index;

	// verify the OOVPA of this entry
	if (table[index].Oovpa != nullptr)
		VerifyHLEOOVPA(context, table[index].Oovpa);
}

void VerifyHLEData(HLEVerifyContext *context, const HLEData *data)
{
	if (context->against == nullptr) {
		context->main_data = data;
	} else {
		context->against_data = data;
	}

	// verify each entry in this HLEData
	uint32 count = data->OovpaTableSize / sizeof(OOVPATable);
	for (uint32 e = 0; e < count; e++)
		VerifyHLEDataEntry(context, data->OovpaTable, e, count);
}

void VerifyHLEDataBaseAgainst(HLEVerifyContext *context)
{
	// verify all HLEData's
	for (uint32 d = 0; d < HLEDataBaseCount; d++)
		VerifyHLEData(context, &HLEDataBase[d]);
}

void VerifyHLEDataBase()
{
	HLEVerifyContext context = { 0 };
	VerifyHLEDataBaseAgainst(&context);
}
#endif // _DEBUG_TRACE
