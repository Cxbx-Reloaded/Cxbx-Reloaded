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

static xbaddr EmuLocateFunction(OOVPA *Oovpa, xbaddr lower, xbaddr upper);
static void  EmuInstallPatches(OOVPATable *OovpaTable, uint32 OovpaTableSize, Xbe::Header *pXbeHeader);

#include <shlobj.h>
#include <vector>

uint32 fcount = 0;

uint32 g_BuildVersion;
uint32 g_OrigBuildVersion;

static std::vector<xbaddr> vCacheOut;

static bool bCacheInp = false;
static std::vector<xbaddr> vCacheInp;
static std::vector<xbaddr>::const_iterator vCacheInpIter;


bool bLLE_APU = false; // Set this to true for experimental APU (sound) LLE
bool bLLE_GPU = false; // Set this to true for experimental GPU (graphics) LLE
bool bLLE_JIT = false; // Set this to true for experimental JIT

bool bXRefFirstPass; // For search speed optimization, set in EmuHLEIntercept, read in EmuLocateFunction
uint32 UnResolvedXRefs; // Tracks XRef location, used (read/write) in EmuHLEIntercept and EmuLocateFunction

void EmuHLEIntercept(Xbe::Header *pXbeHeader)
{
    Xbe::Certificate *pCertificate = (Xbe::Certificate*)pXbeHeader->dwCertificateAddr;
	Xbe::LibraryVersion *pLibraryVersion = (Xbe::LibraryVersion*)pXbeHeader->dwLibraryVersionsAddr;

    char szCacheFileName[MAX_PATH];

    printf("\n");
	printf("*******************************************************************************\n");
	printf("* Cxbx-Reloaded High Level Emulation database last modified %s\n", szHLELastCompileTime);
	printf("*******************************************************************************\n");
	printf("\n");

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
					xbaddr cur;

                    if(fread(&cur, 4, 1, pCacheFile) != 1)
                        break;

                    vCacheInp.push_back(cur);
                }

                bCacheInp = true;

                vCacheInpIter = vCacheInp.begin();

				printf("HLE: Loaded HLE Cache for 0x%.08X\n", pCertificate->dwTitleId);
            }

            fclose(pCacheFile);
        }
    }

	//
    // initialize Microsoft XDK emulation
    //

    if(pLibraryVersion != 0)
    {
		printf("HLE: Detected Microsoft XDK application...\n");

		UnResolvedXRefs = XREF_COUNT; // = sizeof(XRefDataBase) / sizeof(xbaddr)

        uint32 dwLibraryVersions = pXbeHeader->dwLibraryVersions;
        uint32 LastUnResolvedXRefs = UnResolvedXRefs+1;
        uint32 OrigUnResolvedXRefs = UnResolvedXRefs;

		bool bFoundD3D = false;

		bXRefFirstPass = true; // Set to false for search speed optimization

		// Mark all Xrefs initially as undetermined
		memset((void*)XRefDataBase, XREF_ADDR_UNDETERMINED, sizeof(XRefDataBase));

		// Request a few fundamental XRefs to be derived instead of checked
		XRefDataBase[XREF_D3DDEVICE] = XREF_ADDR_DERIVE;
		XRefDataBase[XREF_D3DRS_CULLMODE] = XREF_ADDR_DERIVE;
		XRefDataBase[XREF_D3DTSS_TEXCOORDINDEX] = XREF_ADDR_DERIVE;

		for(int p=0;UnResolvedXRefs < LastUnResolvedXRefs;p++)
        {
			printf("HLE: Starting pass #%d...\n", p+1);

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
                if(BuildVersion == 5455) { BuildVersion = 5558; }
                if(BuildVersion == 5659) { BuildVersion = 5558; }
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
				}

				if(bXRefFirstPass)
                {
                    if(strcmp(Lib_XAPILIB, szLibraryName) == 0 && 
                        (BuildVersion == 3911 || BuildVersion == 4034 || BuildVersion == 4134 || BuildVersion == 4361
                      || BuildVersion == 4432 || BuildVersion == 4627 || BuildVersion == 5028 || BuildVersion == 5233
                      || BuildVersion == 5344 || BuildVersion == 5558 || BuildVersion == 5788 || BuildVersion == 5849))
                    {
                        xbaddr lower = pXbeHeader->dwBaseAddr;
						xbaddr upper = pXbeHeader->dwBaseAddr + pXbeHeader->dwSizeofImage;
                    }
                    else if(strcmp(Lib_D3D8, szLibraryName) == 0 /*&& strcmp(Lib_D3D8LTCG, szOrigLibraryName)*/ && 
                        (BuildVersion == 3925 || BuildVersion == 4134 || BuildVersion == 4361 || BuildVersion == 4432
                      || BuildVersion == 4627 || BuildVersion == 5028 || BuildVersion == 5233 || BuildVersion == 5344
                      || BuildVersion == 5558 || BuildVersion == 5788 || BuildVersion == 5849))
                    {
						// Save D3D8 build version
						g_BuildVersion = BuildVersion;
						g_OrigBuildVersion = OrigBuildVersion;

						xbaddr lower = pXbeHeader->dwBaseAddr;
						xbaddr upper = pXbeHeader->dwBaseAddr + pXbeHeader->dwSizeofImage;
						xbaddr pFunc = (xbaddr)nullptr;

                        if(BuildVersion == 3925)
                            pFunc = EmuLocateFunction((OOVPA*)&D3DDevice_SetRenderState_CullMode_3925, lower, upper);
                        else if(BuildVersion < 5233)
                            pFunc = EmuLocateFunction((OOVPA*)&D3DDevice_SetRenderState_CullMode_4034, lower, upper);
                        else
                            pFunc = EmuLocateFunction((OOVPA*)&D3DDevice_SetRenderState_CullMode_5233, lower, upper);

                        // locate D3DDeferredRenderState
                        if(pFunc != (xbaddr)nullptr)
                        {
                            // offset for stencil cull enable render state in the deferred render state buffer
							xbaddr DerivedAddr_D3DRS_CULLMODE = NULL;
							int Decrement = 0; // TODO : Rename into something understandable
							int Increment = 0; // TODO : Rename into something understandable
                            int patchOffset = 0; // TODO : Rename into something understandable

							// Read address of D3DRS_CULLMODE from D3DDevice_SetRenderState_CullMode
							// TODO : Simplify this when XREF_D3DRS_CULLMODE derivation is deemed stable
							{
								if (BuildVersion == 3925)
								{
									DerivedAddr_D3DRS_CULLMODE = *(xbaddr*)(pFunc + 0x25);
									Decrement = 0x1FC;  // TODO: Clean up (?)
									Increment = 82 * 4;
									patchOffset = 140 * 4; // TODO: Verify

									//Decrement = 0x19F;  // TODO: Clean up (?)
									//Increment = 72 * 4;
									//patchOffset = 142*4; // TODO: Verify
								}
								else if (BuildVersion == 4034)
								{
									DerivedAddr_D3DRS_CULLMODE = *(xbaddr*)(pFunc + 0x2B);
									Decrement = 0x248;  // TODO: Verify
									Increment = 82 * 4;
									patchOffset = 142 * 4;
								}
								else if (BuildVersion == 4134 || BuildVersion == 4361)
								{
									DerivedAddr_D3DRS_CULLMODE = *(xbaddr*)(pFunc + 0x2B);
									Decrement = 0x200;
									Increment = 82 * 4;
									patchOffset = 142 * 4;
								}
								else if (BuildVersion == 4432)
								{
									DerivedAddr_D3DRS_CULLMODE = *(xbaddr*)(pFunc + 0x2B);
									Decrement = 0x204;
									Increment = 83 * 4;
									patchOffset = 143 * 4;
								}
								else if (BuildVersion == 4627 || BuildVersion == 5028 || BuildVersion == 5233 || BuildVersion == 5344
									|| BuildVersion == 5558 || BuildVersion == 5788 || BuildVersion == 5849)
								{
									// WARNING: Not thoroughly tested (just seemed very correct right away)
									DerivedAddr_D3DRS_CULLMODE = *(xbaddr*)(pFunc + 0x2B);
									Decrement = 0x24C;
									Increment = 92 * 4;
									patchOffset = 162 * 4;
								}

								// Temporary verification - is XREF_D3DDEVICE derived correctly?
								xbaddr DerivedAddr_D3DDevice = *(xbaddr*)((xbaddr)pFunc + 0x03);
								if (XRefDataBase[XREF_D3DDEVICE] != DerivedAddr_D3DDevice)
								{
									if (XRefDataBase[XREF_D3DDEVICE] != XREF_ADDR_DERIVE)
										EmuWarning("Second derived XREF_D3DDEVICE differs from first!");

									XRefDataBase[XREF_D3DDEVICE] = DerivedAddr_D3DDevice;
								}

								// Temporary verification - is XREF_D3DRS_CULLMODE derived correctly?
								if (XRefDataBase[XREF_D3DRS_CULLMODE] != DerivedAddr_D3DRS_CULLMODE)
								{
									if (XRefDataBase[XREF_D3DRS_CULLMODE] != XREF_ADDR_DERIVE)
										EmuWarning("Second derived XREF_D3DRS_CULLMODE differs from first!");

									XRefDataBase[XREF_D3DRS_CULLMODE] = DerivedAddr_D3DRS_CULLMODE;
								}
							}

							// Derive address of EmuD3DDeferredRenderState from D3DRS_CULLMODE
							XTL::EmuD3DDeferredRenderState = (DWORD*)(DerivedAddr_D3DRS_CULLMODE - Decrement + Increment);
							patchOffset -= Increment;

							// Derive address of a few other deferred render state slots (to help xref-based function location)
                            XRefDataBase[XREF_D3DRS_MULTISAMPLEMODE]       = (xbaddr)XTL::EmuD3DDeferredRenderState + patchOffset - 8*4;
                            XRefDataBase[XREF_D3DRS_MULTISAMPLERENDERTARGETMODE] = (xbaddr)XTL::EmuD3DDeferredRenderState + patchOffset - 7*4;
                            XRefDataBase[XREF_D3DRS_STENCILCULLENABLE]     = (xbaddr)XTL::EmuD3DDeferredRenderState + patchOffset + 0*4;
                            XRefDataBase[XREF_D3DRS_ROPZCMPALWAYSREAD]     = (xbaddr)XTL::EmuD3DDeferredRenderState + patchOffset + 1*4;
                            XRefDataBase[XREF_D3DRS_ROPZREAD]              = (xbaddr)XTL::EmuD3DDeferredRenderState + patchOffset + 2*4;
                            XRefDataBase[XREF_D3DRS_DONOTCULLUNCOMPRESSED] = (xbaddr)XTL::EmuD3DDeferredRenderState + patchOffset + 3*4;

                            for(int v=0;v<44;v++)
                            {
                                XTL::EmuD3DDeferredRenderState[v] = X_D3DRS_UNK;
                            }

							printf("HLE: 0x%.08X -> EmuD3DDeferredRenderState\n", XTL::EmuD3DDeferredRenderState);
							//DbgPrintf("HLE: 0x%.08X -> XREF_D3DRS_ROPZCMPALWAYSREAD\n", XRefDataBase[XREF_D3DRS_ROPZCMPALWAYSREAD] );
                        }
                        else
                        {
                            XTL::EmuD3DDeferredRenderState = nullptr;
                            CxbxKrnlCleanup("EmuD3DDeferredRenderState was not found!");
                        }

                        // locate D3DDeferredTextureState
                        {
                            pFunc = (xbaddr)nullptr;

                            if(BuildVersion == 3925)
                                pFunc = EmuLocateFunction((OOVPA*)&D3DDevice_SetTextureState_TexCoordIndex_3925, lower, upper);
							// TODO : What about 4034? Use it once it's offset to XREF_D3DTSS_TEXCOORDINDEX is verified
                            else if(BuildVersion == 4134)
                                pFunc = EmuLocateFunction((OOVPA*)&D3DDevice_SetTextureState_TexCoordIndex_4134, lower, upper);
                            else if(BuildVersion == 4361 || BuildVersion == 4432)
                                pFunc = EmuLocateFunction((OOVPA*)&D3DDevice_SetTextureState_TexCoordIndex_4361, lower, upper);
                            else if(BuildVersion == 4627 || BuildVersion == 5028 || BuildVersion == 5233 || BuildVersion == 5344
                                 || BuildVersion == 5558 || BuildVersion == 5788 || BuildVersion == 5849)
                                pFunc = EmuLocateFunction((OOVPA*)&D3DDevice_SetTextureState_TexCoordIndex_4627, lower, upper);

							if (pFunc != (xbaddr)nullptr)
							{
								xbaddr DerivedAddr_D3DTSS_TEXCOORDINDEX = NULL;
								int Decrement = 0x70; // TODO : Rename into something understandable

								// TODO : Remove this when XREF_D3DTSS_TEXCOORDINDEX derivation is deemed stable
								{
									if (BuildVersion == 3925) // 0x18F180
										DerivedAddr_D3DTSS_TEXCOORDINDEX = *(xbaddr*)(pFunc + 0x11);
									else if (BuildVersion == 4134)
										DerivedAddr_D3DTSS_TEXCOORDINDEX = *(xbaddr*)(pFunc + 0x18);
									else
										DerivedAddr_D3DTSS_TEXCOORDINDEX = *(xbaddr*)(pFunc + 0x19);
								

									// Temporary verification - is XREF_D3DTSS_TEXCOORDINDEX derived correctly?
									if (XRefDataBase[XREF_D3DTSS_TEXCOORDINDEX] != DerivedAddr_D3DTSS_TEXCOORDINDEX)
									{
										if (XRefDataBase[XREF_D3DTSS_TEXCOORDINDEX] != XREF_ADDR_DERIVE)
											EmuWarning("Second derived XREF_D3DTSS_TEXCOORDINDEX differs from first!");

										XRefDataBase[XREF_D3DTSS_TEXCOORDINDEX] = DerivedAddr_D3DTSS_TEXCOORDINDEX;
									}
								}

								XTL::EmuD3DDeferredTextureState = (DWORD*)(DerivedAddr_D3DTSS_TEXCOORDINDEX - Decrement);

								for(int s=0;s<4;s++)
                                {
                                    for(int v=0;v<32;v++)
                                        XTL::EmuD3DDeferredTextureState[v+s*32] = X_D3DTSS_UNK;
                                }

								printf("HLE: 0x%.08X -> EmuD3DDeferredTextureState\n", XTL::EmuD3DDeferredTextureState);
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

     //                   xbaddr lower = pXbeHeader->dwBaseAddr;
     //                   xbaddr upper = pXbeHeader->dwBaseAddr + pXbeHeader->dwSizeofImage;
     //                   xbaddr pFunc = (xbaddr)nullptr;

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
     //                           XTL::EmuD3DDeferredRenderState = (DWORD*)(*(DWORD*)(pFunc + 0x2B) - 0x24C + 92*4);
     //                           patchOffset = 162*4 - 92*4;
     //                       }

     //                       XRefDataBase[XREF_D3DDEVICE]                   = *(DWORD*)((DWORD)pFunc + 0x03);
     //                       XRefDataBase[XREF_D3DRS_MULTISAMPLEMODE]       = (xbaddr)XTL::EmuD3DDeferredRenderState + patchOffset - 8*4;
     //                       XRefDataBase[XREF_D3DRS_MULTISAMPLERENDERTARGETMODE] = (xbaddr)XTL::EmuD3DDeferredRenderState + patchOffset - 7*4;
     //                       XRefDataBase[XREF_D3DRS_STENCILCULLENABLE]     = (xbaddr)XTL::EmuD3DDeferredRenderState + patchOffset + 0*4;
     //                       XRefDataBase[XREF_D3DRS_ROPZCMPALWAYSREAD]     = (xbaddr)XTL::EmuD3DDeferredRenderState + patchOffset + 1*4;
     //                       XRefDataBase[XREF_D3DRS_ROPZREAD]              = (xbaddr)XTL::EmuD3DDeferredRenderState + patchOffset + 2*4;
     //                       XRefDataBase[XREF_D3DRS_DONOTCULLUNCOMPRESSED] = (xbaddr)XTL::EmuD3DDeferredRenderState + patchOffset + 3*4;

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
     //                       pFunc = (xbaddr)nullptr;

     //                       if(BuildVersion == 3925)
					//			pFunc = EmuLocateFunction((OOVPA*)&D3DDevice_SetTextureState_TexCoordIndex_LTCG_5849, lower, upper);

     //                       if(pFunc != (xbaddr)nullptr)
     //                       {
     //                           if(BuildVersion == 3925) // 0x18F180
     //                               XTL::EmuD3DDeferredTextureState = (DWORD*)(*(DWORD*)(pFunc + 0x11) - 0x70); // TODO: Verify
     //                           else if(BuildVersion == 4134)
     //                               XTL::EmuD3DDeferredTextureState = (DWORD*)(*(DWORD*)(pFunc + 0x18) - 0x70); // TODO: Verify
     //                           else
     //                               XTL::EmuD3DDeferredTextureState = (DWORD*)(*(DWORD*)(pFunc + 0x19) - 0x70);

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

				printf("HLE: * Searching HLE database for %s version 1.0.%d... ", szLibraryName, BuildVersion);

                const HLEData *FoundHLEData = nullptr;
                for(uint32 d = 0; d < HLEDataBaseCount; d++) {
					if (BuildVersion == HLEDataBase[d].BuildVersion && strcmp(szLibraryName, HLEDataBase[d].Library) == 0) {
						FoundHLEData = &HLEDataBase[d];
						break;
					}
                }

				if (FoundHLEData) {
					if (g_bPrintfOn) printf("Found\n");
					EmuInstallPatches(FoundHLEData->OovpaTable, FoundHLEData->OovpaTableSize, pXbeHeader);
				} else {
					if (g_bPrintfOn) printf("Skipped\n");
				}
			}

            bXRefFirstPass = false;
        }

        // display Xref summary
		printf("HLE: Resolved %d cross reference(s)\n", OrigUnResolvedXRefs - UnResolvedXRefs);
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

	printf("\n");

    return;
}

static inline void EmuInstallPatch(xbaddr FunctionAddr, void *Patch)
{
    uint08 *FuncBytes = (uint08*)FunctionAddr;

	*(uint08*)&FuncBytes[0] = OPCODE_JMP_E9; // = opcode for JMP rel32 (Jump near, relative, displacement relative to next instruction)
    *(uint32*)&FuncBytes[1] = (uint32)Patch - FunctionAddr - 5;
}

static inline void GetXRefEntry(OOVPA *oovpa, int index, OUT uint32 &xref, OUT uint08 &offset)
{
	// Note : These are stored swapped by the XREF_ENTRY macro, hence this difference from GetOovpaEntry :
	xref = (uint32)((LOOVPA<1>*)oovpa)->Lovp[index].Offset;
	offset = ((LOOVPA<1>*)oovpa)->Lovp[index].Value;
}

static inline void GetOovpaEntry(OOVPA *oovpa, int index, OUT uint32 &offset, OUT uint08 &value)
{
	offset = (uint32)((LOOVPA<1>*)oovpa)->Lovp[index].Offset;
	value = ((LOOVPA<1>*)oovpa)->Lovp[index].Value;
}

static boolean CompareOOVPAToAddress(OOVPA *Oovpa, xbaddr cur)
{
	uint32 v = 0; // verification counter

	// Check all XRefs, stop if any does not match
	for (; v < Oovpa->XRefCount; v++)
	{
		uint32 XRef;
		uint08 Offset;

		// get currently registered (un)known address
		GetXRefEntry(Oovpa, v, XRef, Offset);
		xbaddr XRefAddr = XRefDataBase[XRef];
		// Undetermined XRef cannot be checked yet
		// (EmuLocateFunction already checked this, but this check
		// is cheap enough to keep, and keep this function generic).
		if (XRefAddr == XREF_ADDR_UNDETERMINED)
			return false;

		// Don't verify an xref that has to be (but isn't yet) derived
		if (XRefAddr == XREF_ADDR_DERIVE)
			continue;

		xbaddr ActualAddr = *(xbaddr*)(cur + Offset);
		// check if PC-relative or direct reference matches XRef
		if ((ActualAddr + cur + Offset + 4 != XRefAddr) && (ActualAddr != XRefAddr))
			return false;
	}

	// Check all (Offset,Value)-pairs, stop if any does not match
	for (; v < Oovpa->Count; v++)
	{
		uint32 Offset;
		uint08 ExpectedValue;

		// get offset + value pair
		GetOovpaEntry(Oovpa, v, Offset, ExpectedValue);
		uint08 ActualValue = *(uint08*)(cur + Offset);
		if (ActualValue != ExpectedValue)
			return false;
	}

	// all offsets matched
	return true;
}

// locate the given function, searching within lower and upper bounds
static xbaddr EmuLocateFunction(OOVPA *Oovpa, xbaddr lower, xbaddr upper)
{
	// skip out if this is an unnecessary search
	if (!bXRefFirstPass && Oovpa->XRefCount == XRefZero && Oovpa->XRefSaveIndex == XRefNoSaveIndex)
		return (xbaddr)nullptr;

	uint32_t derive_indices = 0;
	// Check all XRefs are known (if not, don't do a useless scan) :
	for (uint32 v = 0; v < Oovpa->XRefCount; v++)
	{
		uint32 XRef;
		uint08 Offset;

		// get currently registered (un)known address
		GetXRefEntry(Oovpa, v, XRef, Offset);
		xbaddr XRefAddr = XRefDataBase[XRef];
		// Undetermined XRef cannot be checked yet
		if (XRefAddr == XREF_ADDR_UNDETERMINED)
			// Skip this scan over the address range
			return (xbaddr)nullptr;

		// Don't verify an xref that has to be (but isn't yet) derived
		if (XRefAddr == XREF_ADDR_DERIVE)
		{
			// Mark (up to index 32) which xref needs to be derived
			derive_indices |= (1 << v);
			continue;
		}
	}

	// correct upper bound with highest Oovpa offset
	uint32 count = Oovpa->Count;
	{
		uint32 Offset;
		uint08 Value; // ignored

		GetOovpaEntry(Oovpa, count - 1, Offset, Value);
		upper -= Offset;
	}

	// search all of the image memory
	for (xbaddr cur = lower; cur < upper; cur++)
		if (CompareOOVPAToAddress(Oovpa, cur))
		{
			// do we need to save the found address?
			if (Oovpa->XRefSaveIndex != XRefNoSaveIndex)
			{
				// is the XRef not saved yet?
				switch (XRefDataBase[Oovpa->XRefSaveIndex]) {
				case XREF_ADDR_NOT_FOUND:
				{
					EmuWarning("Found OOVPA after first finding nothing?");
					// fallthrough to XREF_ADDR_UNDETERMINED
				}
				case XREF_ADDR_UNDETERMINED:
				{
					// save and count the found address
					UnResolvedXRefs--;
					XRefDataBase[Oovpa->XRefSaveIndex] = cur;
					break;
				}
				case XREF_ADDR_DERIVE:
				{
					EmuWarning("Cannot derive a save index!");
					break;
				}
				default:
				{
					if (XRefDataBase[Oovpa->XRefSaveIndex] != cur)
						EmuWarning("Found OOVPA on other address than in XRefDataBase!");
					break;
				}
				}
			}

			while (derive_indices > 0)
			{
				uint32 XRef;
				uint08 Offset;
				DWORD derive_index;

				// Extract an index from the indices mask :
				_BitScanReverse(&derive_index, derive_indices); // MSVC intrinsic; GCC has __builtin_clz
				derive_indices ^= (1 << derive_index);

				// get currently registered (un)known address
				GetXRefEntry(Oovpa, derive_index, XRef, Offset);

				// Calculate the address where the XRef resides
				xbaddr XRefAddr = cur + Offset;
				// Read the address it points to
				XRefAddr = *((xbaddr*)XRefAddr);

				/* For now assume it's a direct reference;
				// TODO : Check if it's PC-relative reference?
				if (XRefAddr + cur + Offset + 4 < XBE_MAX_VA)
					XRefAddr = XRefAddr + cur + Offset + 4;
				*/

				// Does the address seem valid?
				if (XRefAddr < XBE_MAX_VA)
				{
					// save and count the derived address
					UnResolvedXRefs--;
					XRefDataBase[XRef] = XRefAddr;
					printf("Derived OOVPA!\n");
				}
			}

			return cur;
		}

	// found nothing
    return (xbaddr)nullptr;
}

// install function interception wrappers
static void EmuInstallPatches(OOVPATable *OovpaTable, uint32 OovpaTableSize, Xbe::Header *pXbeHeader)
{
    xbaddr lower = pXbeHeader->dwBaseAddr;

	// Find the highest address contained within an executable segment
	xbaddr upper = pXbeHeader->dwBaseAddr;
	Xbe::SectionHeader* headers = reinterpret_cast<Xbe::SectionHeader*>(pXbeHeader->dwSectionHeadersAddr);

	for (uint32_t i = 0; i < pXbeHeader->dwSections; i++) {
		xbaddr end_addr = headers[i].dwVirtualAddr + headers[i].dwVirtualSize;
		if (headers[i].dwFlags.bExecutable && end_addr > upper) {
			upper = end_addr;
		}	
	}


    // traverse the full OOVPA table
    for(size_t a=0;a<OovpaTableSize/sizeof(OOVPATable);a++)
    {
        OOVPA *Oovpa = OovpaTable[a].Oovpa;
		xbaddr pFunc = (xbaddr)nullptr;

        if(bCacheInp && (vCacheInpIter != vCacheInp.end()))
        {
            pFunc = *vCacheInpIter;
            ++vCacheInpIter;
        }
        else
        {
            pFunc = EmuLocateFunction(Oovpa, lower, upper);
			if (pFunc != (xbaddr)nullptr)
				vCacheOut.push_back(pFunc);
        }

        if(pFunc != (xbaddr)nullptr)
        {
            printf("HLE: 0x%.08X -> %s\n", pFunc, OovpaTable[a].szFuncName);

            if(OovpaTable[a].emuPatch == nullptr && (OovpaTable[a].Flags & Flag_DontScan == 0))
            {
                EmuInstallPatch(pFunc, OovpaTable[a].emuPatch);
            }
        }
    }
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
				HLEError(context, "Lovp[%d] : Offset (0x%x) must be larger then previous offset (0x%x)",
					p, curr_offset, prev_offset);
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
		void * entry_redirect = table[index].emuPatch;
		if (entry_redirect != nullptr) {
			if (table[index].Oovpa == nullptr) {
				HLEError(context, "Patch without an OOVPA at index %d",
					index);
			} else
				// check no patch occurs twice in this table
				for (uint32 t = index + 1; t < count; t++) {
					if (entry_redirect == table[t].emuPatch) {
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
