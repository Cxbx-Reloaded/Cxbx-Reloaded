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
#include <iomanip> // For std::setfill and std::setw
#include "CxbxKrnl.h"
#include "Emu.h"
#include "EmuFS.h"
#include "EmuXTL.h"
#include "EmuShared.h"
#include "HLEDataBase.h"
#include "HLEIntercept.h"
#include "xxhash32.h"
#include <Shlwapi.h>
#include <subhook.h>

static xbaddr EmuLocateFunction(OOVPA *Oovpa, xbaddr lower, xbaddr upper);
static void  EmuInstallPatches(OOVPATable *OovpaTable, uint32 OovpaTableSize, Xbe::Header *pXbeHeader);
static inline void EmuInstallPatch(std::string FunctionName, xbaddr FunctionAddr, void *Patch);

#include <shlobj.h>
#include <unordered_map>
#include <map>
#include <sstream>

std::map<std::string, xbaddr> g_SymbolAddresses;
std::unordered_map<std::string, subhook::Hook> g_FunctionHooks;
bool g_HLECacheUsed = false;

uint32 g_BuildVersion;

bool bLLE_APU = false; // Set this to true for experimental APU (sound) LLE
bool bLLE_GPU = false; // Set this to true for experimental GPU (graphics) LLE
bool bLLE_JIT = false; // Set this to true for experimental JIT

bool bXRefFirstPass; // For search speed optimization, set in EmuHLEIntercept, read in EmuLocateFunction
uint32 UnResolvedXRefs; // Tracks XRef location, used (read/write) in EmuHLEIntercept and EmuLocateFunction

void* GetXboxFunctionPointer(std::string functionName)
{
	if (g_FunctionHooks.find(functionName) != g_FunctionHooks.end()) {
		return g_FunctionHooks[functionName].GetTrampoline();
	}

	return nullptr;
}

std::string GetDetectedSymbolName(xbaddr address, int *symbolOffset)
{
	std::string result = "";
	int closestMatch = MAXINT;

	for (auto it = g_SymbolAddresses.begin(); it != g_SymbolAddresses.end(); ++it) {
		xbaddr symbolAddr = (*it).second;
		if (symbolAddr == NULL)
			continue;

		if (symbolAddr <= address)
		{
			int distance = address - symbolAddr;
			if (closestMatch > distance)
			{
				closestMatch = distance;
				result = (*it).first;
			}
		}
	}

	if (closestMatch < MAXINT)
	{
		*symbolOffset = closestMatch;
		return result;
	}

	*symbolOffset = 0;
	return "unknown";
}

void *GetEmuPatchAddr(std::string aFunctionName)
{
	std::string patchName = "XTL::EmuPatch_" + aFunctionName;
	void* addr = GetProcAddress(GetModuleHandle(NULL), patchName.c_str());
	return addr;
}

void EmuHLEIntercept(Xbe::Header *pXbeHeader)
{
	Xbe::LibraryVersion *pLibraryVersion = (Xbe::LibraryVersion*)pXbeHeader->dwLibraryVersionsAddr;

    printf("\n");
	printf("*******************************************************************************\n");
	printf("* Cxbx-Reloaded High Level Emulation database last modified %s\n", szHLELastCompileTime);
	printf("*******************************************************************************\n");
	printf("\n");

	// Make sure the HLE Cache directory exists
	std::string cachePath = std::string(szFolder_CxbxReloadedData) + "\\HLECache\\";
	int result = SHCreateDirectoryEx(nullptr, cachePath.c_str(), nullptr);
	if ((result != ERROR_SUCCESS) && (result != ERROR_ALREADY_EXISTS)) {
		CxbxKrnlCleanup("Couldn't create Cxbx-Reloaded HLECache folder!");
	}

	// Hash the loaded XBE's header, use it as a filename
	uint32_t uiHash = XXHash32::hash((void*)&CxbxKrnl_Xbe->m_Header, sizeof(Xbe::Header), 0);
	std::stringstream sstream;
	sstream << cachePath << std::hex << uiHash << ".ini";
	std::string filename = sstream.str();

	if (PathFileExists(filename.c_str())) {
		printf("Found HLE Cache File: %08X.ini\n", uiHash);

		// Verify the version of the cache file against the HLE Database
		char buffer[SHRT_MAX] = { 0 };
		char* bufferPtr = buffer;

		GetPrivateProfileString("Info", "HLEDatabaseVersion", NULL, buffer, sizeof(buffer), filename.c_str());
		g_BuildVersion = GetPrivateProfileInt("Libs", "D3D8_BuildVersion", 0, filename.c_str());

		if (strcmp(buffer, szHLELastCompileTime) == 0) {
			printf("Using HLE Cache\n");
			GetPrivateProfileSection("Symbols", buffer, sizeof(buffer), filename.c_str());

			// Parse the .INI file into the map of symbol addresses
			while (strlen(bufferPtr) > 0) {
				std::string ini_entry(bufferPtr);

				auto separator = ini_entry.find('=');
				std::string key = ini_entry.substr(0, separator);
				std::string value = ini_entry.substr(separator + 1, std::string::npos);
				uint32_t addr = strtol(value.c_str(), 0, 16);

				g_SymbolAddresses[key] = addr;
				bufferPtr += strlen(bufferPtr) + 1;
			}

			// Iterate through the map of symbol addresses, calling GetEmuPatchAddr on all functions.	
			for (auto it = g_SymbolAddresses.begin(); it != g_SymbolAddresses.end(); ++it) {
				std::string functionName = (*it).first;
				xbaddr location = (*it).second;

				std::stringstream output;
				output << "HLECache: 0x" << std::setfill('0') << std::setw(8) << std::hex << location
					<< " -> " << functionName;
				void* pFunc = GetEmuPatchAddr(functionName);
				if (pFunc != nullptr)
				{
					// skip entries that weren't located at all
					if (location == NULL)
					{
						output << "\t(not patched)";
					}
					// Prevent patching illegal addresses
					else if (location < XBE_IMAGE_BASE)
					{
						output << "\t*ADDRESS TOO LOW!*";
					}
					else if (location > XBOX_MEMORY_SIZE)
					{
						output << "\t*ADDRESS TOO HIGH!*";
					}
					else
					{
						EmuInstallPatch(functionName, location, pFunc);
						output << "\t*PATCHED*";
					}
				}
				else
				{
					if (location != NULL)
						output << "\t(no patch)";
				}

				output << "\n";
				printf(output.str().c_str());
			}

			// Fix up Render state and Texture States
			if (g_SymbolAddresses.find("D3DDeferredRenderState") == g_SymbolAddresses.end()) {
				EmuWarning("EmuD3DDeferredRenderState was not found!");
			}
			
			if (g_SymbolAddresses.find("D3DDeferredTextureState") == g_SymbolAddresses.end()) {
				EmuWarning("EmuD3DDeferredTextureState was not found!");
			}

			if (g_SymbolAddresses.find("D3DDEVICE") == g_SymbolAddresses.end()) {
				EmuWarning("D3DDEVICE was not found!");
			}

			XTL::EmuD3DDeferredRenderState = (DWORD*)g_SymbolAddresses["D3DDeferredRenderState"];
			XTL::EmuD3DDeferredTextureState = (DWORD*)g_SymbolAddresses["D3DDeferredTextureState"];
			XRefDataBase[XREF_D3DDEVICE] = g_SymbolAddresses["D3DDEVICE"];

			// TODO: Move this into a function rather than duplicating from HLE scanning code
			if (XTL::EmuD3DDeferredRenderState != nullptr) {
				for (int v = 0; v<44; v++) {
					XTL::EmuD3DDeferredRenderState[v] = XTL::X_D3DRS_UNK;
				}

				for (int s = 0; s<4; s++) {
					for (int v = 0; v<32; v++)
						XTL::EmuD3DDeferredTextureState[v + s * 32] = X_D3DTSS_UNK;
				}
			}
		

			g_HLECacheUsed = true;
		}

		// If g_SymbolAddresses didn't get filled, the HLE cache is invalid
		if (g_SymbolAddresses.empty()) {
			printf("HLE Cache file is outdated and will be regenerated\n");
			g_HLECacheUsed = false;
		}
	}

	// If the HLE Cache was used, skip symbol searching/patching
	if (g_HLECacheUsed) {
		return;
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
				// TODO: Remove these and come up with a better way to handle XDKs we don't hve databases for
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
                
				std::string LibraryName(pLibraryVersion[v].szName, pLibraryVersion[v].szName + 8);

				if (strcmp(LibraryName.c_str(), Lib_D3D8LTCG) == 0)
				{
					// If LLE GPU is not enabled, show a warning that the title is not supported
					if (!bLLE_GPU) {
						CxbxKrnlCleanup("LTCG Title Detected: This game is not supported by HLE");
					}

					// Skip LTCG libraries as we cannot reliably detect them
					continue;
				}
				
				if (strcmp(LibraryName.c_str(), Lib_D3D8) == 0)
				{
					// Skip scanning for D3D8 symbols when LLE GPU is selected
					if (bLLE_GPU)
						continue;

					// Some 3911 titles have different D3D8 builds
					if (BuildVersion <= 3948)
						BuildVersion = 3925;
				}
				if (strcmp(LibraryName.c_str(), Lib_DSOUND) == 0)
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
				if (strcmp(LibraryName.c_str(), Lib_XAPILIB) == 0)
				{
					// Change a few XAPILIB versions to similar counterparts
					if(BuildVersion == 3944)
						BuildVersion = 3911;
					if(BuildVersion == 3950)
						BuildVersion = 3911;
					if(OrigBuildVersion == 4531)
						BuildVersion = 4627;
				}
				if (strcmp(LibraryName.c_str(), Lib_XGRAPHC) == 0)
				{
					// Skip scanning for XGRAPHC (XG) symbols when LLE GPU is selected
					if (bLLE_GPU)
						continue;

					if (BuildVersion == 3944)
						BuildVersion = 3911;
					if (OrigBuildVersion == 4531)
						BuildVersion = 4361;
				}

				if(bXRefFirstPass)
                {
                    if (strcmp(LibraryName.c_str(), Lib_XAPILIB) == 0 &&
                        (BuildVersion == 3911 || BuildVersion == 4034 || BuildVersion == 4134 || BuildVersion == 4361
                      || BuildVersion == 4432 || BuildVersion == 4627 || BuildVersion == 5028 || BuildVersion == 5233
                      || BuildVersion == 5344 || BuildVersion == 5558 || BuildVersion == 5788 || BuildVersion == 5849))
                    {
                        xbaddr lower = pXbeHeader->dwBaseAddr;
						xbaddr upper = pXbeHeader->dwBaseAddr + pXbeHeader->dwSizeofImage;
                    }
                    else if (strcmp(LibraryName.c_str(), Lib_D3D8) == 0 &&
                        (BuildVersion == 3925 || BuildVersion == 4134 || BuildVersion == 4361 || BuildVersion == 4432
                      || BuildVersion == 4627 || BuildVersion == 5028 || BuildVersion == 5233 || BuildVersion == 5344
                      || BuildVersion == 5558 || BuildVersion == 5788 || BuildVersion == 5849))
                    {
						// Save D3D8 build version
						g_BuildVersion = BuildVersion;

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
									patchOffset = 105 * 4; // TODO: Verify

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
										CxbxPopupMessage("Second derived XREF_D3DDEVICE differs from first!");

									XRefDataBase[XREF_D3DDEVICE] = DerivedAddr_D3DDevice;
								}

								g_SymbolAddresses["D3DDEVICE"] = DerivedAddr_D3DDevice;

								// Temporary verification - is XREF_D3DRS_CULLMODE derived correctly?
								if (XRefDataBase[XREF_D3DRS_CULLMODE] != DerivedAddr_D3DRS_CULLMODE)
								{
									if (XRefDataBase[XREF_D3DRS_CULLMODE] != XREF_ADDR_DERIVE)
										CxbxPopupMessage("Second derived XREF_D3DRS_CULLMODE differs from first!");

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
                                XTL::EmuD3DDeferredRenderState[v] = XTL::X_D3DRS_UNK;
                            }

							g_SymbolAddresses["D3DDeferredRenderState"] = (DWORD)XTL::EmuD3DDeferredRenderState;
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
											CxbxPopupMessage("Second derived XREF_D3DTSS_TEXCOORDINDEX differs from first!");

										XRefDataBase[XREF_D3DTSS_TEXCOORDINDEX] = DerivedAddr_D3DTSS_TEXCOORDINDEX;
									}
								}

								XTL::EmuD3DDeferredTextureState = (DWORD*)(DerivedAddr_D3DTSS_TEXCOORDINDEX - Decrement);

								for(int s=0;s<4;s++)
                                {
                                    for(int v=0;v<32;v++)
                                        XTL::EmuD3DDeferredTextureState[v+s*32] = X_D3DTSS_UNK;
                                }

								g_SymbolAddresses["D3DDeferredTextureState"] = (DWORD)XTL::EmuD3DDeferredTextureState;
								printf("HLE: 0x%.08X -> EmuD3DDeferredTextureState\n", XTL::EmuD3DDeferredTextureState);
                            }
                            else
                            {
                                XTL::EmuD3DDeferredTextureState = nullptr;
                                CxbxKrnlCleanup("EmuD3DDeferredTextureState was not found!");
                            }
                        }
                    }
                }

				printf("HLE: * Searching HLE database for %s version 1.0.%d... ", LibraryName.c_str(), BuildVersion);

                const HLEData *FoundHLEData = nullptr;
                for(uint32 d = 0; d < HLEDataBaseCount; d++) {
					if (BuildVersion == HLEDataBase[d].BuildVersion && strcmp(LibraryName.c_str(), HLEDataBase[d].Library) == 0) {
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

	printf("\n");

	// Write the HLE Database version string
	WritePrivateProfileString("Info", "HLEDatabaseVersion", szHLELastCompileTime, filename.c_str());

	// Write the Certificate Details to the cache file
	char tAsciiTitle[40] = "Unknown";
	setlocale(LC_ALL, "English");
	wcstombs(tAsciiTitle, g_pCertificate->wszTitleName, sizeof(tAsciiTitle));
	WritePrivateProfileString("Certificate", "Name", tAsciiTitle, filename.c_str());

	std::stringstream titleId;
	titleId << std::hex << g_pCertificate->dwTitleId;
	WritePrivateProfileString("Certificate", "TitleID", titleId.str().c_str(), filename.c_str());

	std::stringstream region;
	region << std::hex << g_pCertificate->dwGameRegion;
	WritePrivateProfileString("Certificate", "Region", region.str().c_str(), filename.c_str());

	// Write Library Details
	for (uint i = 0; i < pXbeHeader->dwLibraryVersions; i++)	{
		std::string LibraryName(pLibraryVersion[i].szName, pLibraryVersion[i].szName + 8);
		std::stringstream buildVersion;
		buildVersion << pLibraryVersion[i].wBuildVersion;

		WritePrivateProfileString("Libs", LibraryName.c_str(), buildVersion.str().c_str(), filename.c_str());
	}

	std::stringstream buildVersion;
	buildVersion << g_BuildVersion;
	WritePrivateProfileString("Libs", "D3D8_BuildVersion", buildVersion.str().c_str(), filename.c_str());

	// Write the found symbol addresses into the cache file
	for(auto it = g_SymbolAddresses.begin(); it != g_SymbolAddresses.end(); ++it) {
		std::stringstream cacheAddress;
		cacheAddress << std::hex << (*it).second;
		WritePrivateProfileString("Symbols", (*it).first.c_str(), cacheAddress.str().c_str(), filename.c_str());
	}

    return;
}

static inline void EmuInstallPatch(std::string FunctionName, xbaddr FunctionAddr, void *Patch)
{
	g_FunctionHooks[FunctionName].Install((void*)(FunctionAddr), Patch);
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
		// Never used : skip scans when so configured
		bool DontScan = (OovpaTable[a].Flags & Flag_DontScan) > 0;
		if (DontScan)
			continue;

		// Skip already found & handled symbols
		xbaddr pFunc = g_SymbolAddresses[OovpaTable[a].szFuncName];
		if (pFunc != (xbaddr)nullptr)
			continue;

		// Search for each function's location using the OOVPA
        OOVPA *Oovpa = OovpaTable[a].Oovpa;
		pFunc = (xbaddr)EmuLocateFunction(Oovpa, lower, upper);
		if (pFunc == (xbaddr)nullptr)
			continue;

		// Now that we found the address, store it (regardless if we patch it or not)
		g_SymbolAddresses[OovpaTable[a].szFuncName] = (uint32_t)pFunc;

		// Output some details
		std::stringstream output;
		output << "HLE: 0x" << std::setfill('0') << std::setw(8) << std::hex << pFunc
			<< " -> " << OovpaTable[a].szFuncName << " " << std::dec << OovpaTable[a].Version;

		bool IsXRef = (OovpaTable[a].Flags & Flag_XRef) > 0;
		if (IsXRef)
			output << "\t(XREF)";

		// Retrieve the associated patch, if any is available
		void* addr = GetEmuPatchAddr(std::string(OovpaTable[a].szFuncName));
		bool DontPatch = (OovpaTable[a].Flags & Flag_DontPatch) > 0;
		if (DontPatch)
		{
			// Mention if there's an unused patch
			if (addr != nullptr)
				output << "\t*PATCH UNUSED!*";
			else
				output << "\t*DISABLED*";
		}
		else
		{
			if (addr != nullptr)
			{
				EmuInstallPatch(OovpaTable[a].szFuncName, pFunc, addr);
				output << "\t*PATCHED*";
			}
			else
			{
				// Mention there's no patch available, if it was to be applied
				if (!IsXRef) // TODO : Remove this restriction once we patch xrefs regularly
					output << "\t*NO PATCH AVAILABLE!*";
			}
		}

		output << "\n";
		printf(output.str().c_str());
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
	} else {
		context->against_index = index;
	}

	if (context->against == nullptr) {
		if (table[index].Flags & Flag_DontPatch)
		{
			if (GetEmuPatchAddr((std::string)table[index].szFuncName))
			{
				HLEError(context, "OOVPA registration DISABLED while a patch exists!");
			}
		}
		else
		if (table[index].Flags & Flag_XRef)
		{
			if (GetEmuPatchAddr((std::string)table[index].szFuncName))
			{
				HLEError(context, "OOVPA registration XREF while a patch exists!");
			}
		}
	}

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

	// Don't check a database against itself :
	if (context->main_data == context->against_data)
		return;

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
