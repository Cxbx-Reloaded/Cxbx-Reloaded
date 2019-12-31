// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
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
// *  (c) 2016-2018 Luke Usher <luke.usher@outlook.com>
// *  (c) 2016-2018 Patrick van Logchem <pvanlogchem@gmail.com>
// *  (c) 2017-2018 RadWolfie
// *  (c) 2017-2018 jarupxx
// *  (c) 2018 x1nixmzeng
// *
// *  All rights reserved
// *
// ******************************************************************

#define LOG_PREFIX CXBXR_MODULE::HLE

#include <cmath>
#include <iomanip> // For std::setfill and std::setw

#include "core\kernel\init\CxbxKrnl.h"
#include "core\kernel\support\Emu.h"
#include "EmuShared.h"
#include "common\CxbxDebugger.h"
#include "Logging.h"
#pragma comment(lib, "XbSymbolDatabase.lib")
#include "..\..\import\XbSymbolDatabase\XbSymbolDatabase.h"
#include "Intercept.hpp"
#include "Patches.hpp"
#include "common\util\hasher.h"

#include <Shlwapi.h>
#include <shlobj.h>
#include <unordered_map>
#include <map>
#include <sstream>
#include <clocale>

static const char* section_info = "Info";
static struct {
	const char* SymbolDatabaseVersionHash = "SymbolDatabaseVersionHash";
} sect_info_keys;

static const char* section_certificate = "Certificate";
static struct {
	const char* Name = "Name";
	const char* TitleID = "TitleID";
	const char* TitleIDHex = "TitleIDHex";
	const char* Region = "Region";
} sect_certificate_keys;

static const char* section_libs = "Libs";
static struct {
	const char* BuildVersion = "BuildVersion";
} sect_libs_keys;

static const char* section_symbols = "Symbols";

std::map<std::string, xbaddr> g_SymbolAddresses;
bool g_SymbolCacheUsed = false;

bool bLLE_APU = false; // Set this to true for experimental APU (sound) LLE
bool bLLE_GPU = false; // Set this to true for experimental GPU (graphics) LLE
bool bLLE_USB = false; // Set this to true for experimental USB (input) LLE
bool bLLE_JIT = false; // Set this to true for experimental JIT

void* GetXboxFunctionPointer(std::string functionName)
{
	void* ptr = GetPatchedFunctionTrampoline(functionName);
	if (ptr != nullptr) {
		return ptr;
	}

    // If we got here, the function wasn't patched, so we can just look it up the symbol cache
    // and return the correct offset
    auto symbol = g_SymbolAddresses.find(functionName);
    if (symbol != g_SymbolAddresses.end()) {
        return (void*)symbol->second;
    }

    // Finally, if none of the above were matched, return nullptr
    return nullptr;
}

// NOTE: GetDetectedSymbolName do not get to be in XbSymbolDatabase, get symbol string in Cxbx project only.
std::string GetDetectedSymbolName(const xbaddr address, int * const symbolOffset)
{
    std::string result = "";
    int closestMatch = MAXINT;

    for (auto it = g_SymbolAddresses.begin(); it != g_SymbolAddresses.end(); ++it) {
        xbaddr symbolAddr = it->second;
        if (symbolAddr == xbnull)
            continue;

        if (symbolAddr <= address)
        {
            int distance = address - symbolAddr;
            if (closestMatch > distance)
            {
                closestMatch = distance;
                result = it->first;
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

// NOTE: VerifySymbolAddressAgainstXRef do not get to be in XbSymbolDatabase, perform verification in Cxbx project only.
/*
bool VerifySymbolAddressAgainstXRef(char *SymbolName, xbaddr Address, int XRef)
{
    // Temporary verification - is XREF_D3DTSS_TEXCOORDINDEX derived correctly?
    // TODO : Remove this when XREF_D3DTSS_TEXCOORDINDEX derivation is deemed stable
    xbaddr XRefAddr = XRefDataBase[XRef];
    if (XRefAddr == Address)
        return true;

    if (XRefAddr == XREF_ADDR_DERIVE) {
        printf("HLE: XRef #%d derived 0x%.08X -> %s\n", XRef, Address, SymbolName);
        XRefDataBase[XRef] = Address;
        return true;
    }

    CxbxPopupMessage(LOG_LEVEL::WARNING, CxbxMsgDlgIcon_Warn,
		"Verification of %s failed : XREF was 0x%.8X while lookup gave 0x%.8X", SymbolName, XRefAddr, Address);
    // test case : Kabuki Warriors (for XREF_D3DTSS_TEXCOORDINDEX)
    return false;
}*/

// x1nixmzeng: Hack to notify CxbxDebugger of the SymbolCache file, which is currently a hashed XBE header AND stripped title (see EmuHLEIntercept)
class CxbxDebuggerScopedMessage
{
    std::string& message;

    CxbxDebuggerScopedMessage() = delete;
    CxbxDebuggerScopedMessage(const CxbxDebuggerScopedMessage&) = delete;
public:

    CxbxDebuggerScopedMessage(std::string& message_string)
        : message(message_string)
    { }

    ~CxbxDebuggerScopedMessage()
    {
        if (CxbxDebugger::CanReport())
        {
            CxbxDebugger::ReportHLECacheFile(message.c_str());
        }
    }
};

void CDECL EmuOutputMessage(xb_output_message mFlag, 
                            const char* message)
{
    switch (mFlag) {
        case XB_OUTPUT_MESSAGE_INFO: {
            printf("%s\n", message);
            break;
        }
        case XB_OUTPUT_MESSAGE_WARN: {
            EmuLog(LOG_LEVEL::WARNING, "%s", message);
            break;
        }
        case XB_OUTPUT_MESSAGE_ERROR: {
            CxbxKrnlCleanup("%s", message);
            break;
        }
        case XB_OUTPUT_MESSAGE_DEBUG:
        default: {
#ifdef _DEBUG_TRACE
            printf("%s\n", message);
#endif

            break;
        }
    }
}

void CDECL EmuRegisterSymbol(const char* library_str,
                             uint32_t library_flag,
                             const char* symbol_str,
                             uint32_t func_addr,
                             uint32_t revision)
{
    // Ignore registered symbol in current database.
    uint32_t hasSymbol = g_SymbolAddresses[symbol_str];
    if (hasSymbol != 0)
        return;

    // Output some details
    std::stringstream output;
    output << "Symbol: 0x" << std::setfill('0') << std::setw(8) << std::hex << func_addr
        << " -> " << symbol_str << " " << std::dec << revision;

#if 0 // TODO: XbSymbolDatabase - Need to create a structure for patch and stuff.
    bool IsXRef = OovpaTable->Oovpa->XRefSaveIndex != XRefNoSaveIndex;
    if (IsXRef) {
        output << "\t(XREF)";

        // do we need to save the found address?
        OOVPA* Oovpa = OovpaTable->Oovpa;
        if (Oovpa->XRefSaveIndex != XRefNoSaveIndex) {
            // is the XRef not saved yet?
            switch (XRefDataBase[Oovpa->XRefSaveIndex]) {
                case XREF_ADDR_NOT_FOUND:
                {
                    EmuLog(LOG_LEVEL::WARNING, "Found OOVPA after first finding nothing?");
                    // fallthrough to XREF_ADDR_UNDETERMINED
                }
                case XREF_ADDR_UNDETERMINED:
                {
                    // save and count the found address
                    UnResolvedXRefs--;
                    XRefDataBase[Oovpa->XRefSaveIndex] = pFunc;
                    break;
                }
                case XREF_ADDR_DERIVE:
                {
                    EmuLog(LOG_LEVEL::WARNING, "Cannot derive a save index!");
                    break;
                }
                default:
                {
                    if (XRefDataBase[OovpaTable->Oovpa->XRefSaveIndex] != pFunc) {
                        EmuLog(LOG_LEVEL::WARNING, "Found OOVPA on other address than in XRefDataBase!");
                        EmuLog(LOG_LEVEL::WARNING, "%s: %4d - pFunc: %08X, stored: %08X", OovpaTable->szFuncName, Oovpa->XRefSaveIndex, pFunc, XRefDataBase[Oovpa->XRefSaveIndex]);
                    }
                    break;
                }
            }
        }
    }

    // Retrieve the associated patch, if any is available
    void* addr = GetEmuPatchAddr(std::string(OovpaTable->szFuncName));

    if (addr != nullptr) {
        EmuInstallPatch(OovpaTable->szFuncName, pFunc, addr);
        output << "\t*PATCHED*";
    } else {
        const char* checkDisableStr = nullptr;
        size_t getFuncStrLength = strlen(OovpaTable->szFuncName);

        if (getFuncStrLength > 10) {
            checkDisableStr = &OovpaTable->szFuncName[getFuncStrLength - 10];
        }

        if (checkDisableStr != nullptr && strcmp(checkDisableStr, "_UNPATCHED") == 0) {
            output << "\t*UNPATCHED*";

            // Mention there's no patch available, if it was to be applied
        } else if (!IsXRef) {
            output << "\t*NO PATCH AVAILABLE!*";
        }
    }
#endif

	output << "\n";

	g_SymbolAddresses[symbol_str] = func_addr;
    printf(output.str().c_str());
}

// Update shared structure with GUI process
void EmuUpdateLLEStatus(uint32_t XbLibScan)
{
    unsigned int FlagsLLE;
    g_EmuShared->GetFlagsLLE(&FlagsLLE);

    if ((FlagsLLE & LLE_GPU) == false
        && !((XbLibScan & XbSymbolLib_D3D8) > 0
            || (XbLibScan & XbSymbolLib_D3D8LTCG) > 0)) {
        bLLE_GPU = true;
        FlagsLLE ^= LLE_GPU;
        EmuOutputMessage(XB_OUTPUT_MESSAGE_INFO, "Fallback to LLE GPU.");
    }

    if ((FlagsLLE & LLE_APU) == false
        && (XbLibScan & XbSymbolLib_DSOUND) == 0) {
        bLLE_APU = true;
        FlagsLLE ^= LLE_APU;
        EmuOutputMessage(XB_OUTPUT_MESSAGE_INFO, "Fallback to LLE APU.");
    }
#if 0 // Reenable this when LLE USB actually works
	if ((FlagsLLE & LLE_USB) == false
		&& (XbLibScan & XbSymbolLib_XAPILIB) == 0) {
		bLLE_USB = true;
		FlagsLLE ^= LLE_USB;
		EmuOutputMessage(XB_OUTPUT_MESSAGE_INFO, "Fallback to LLE USB.");
	}
#endif
    ipc_send_gui_update(IPC_UPDATE_GUI::LLE_FLAGS, FlagsLLE);
    //return FlagsLLE;
}

// NOTE: EmuHLEIntercept do not get to be in XbSymbolDatabase, do the intecept in Cxbx project only.
void EmuHLEIntercept(Xbe::Header *pXbeHeader)
{
	Xbe::LibraryVersion *pLibraryVersion = (Xbe::LibraryVersion*)pXbeHeader->dwLibraryVersionsAddr;

	uint16_t xdkVersion = 0;
	uint32_t XbLibScan = 0;

	// NOTE: We need to check if title has library header to optimize verification process.
	if (pLibraryVersion != nullptr) {
		uint32_t dwLibraryVersions = pXbeHeader->dwLibraryVersions;
		const char* SectionName = nullptr;
		Xbe::SectionHeader* pSectionHeaders = (Xbe::SectionHeader*)pXbeHeader->dwSectionHeadersAddr;
		uint32_t XbLibFlag;

		// Get the highest revision build and prefix library to scan.
		for (uint32_t v = 0; v < dwLibraryVersions; v++) {
			uint16_t BuildVersion, QFEVersion;
			BuildVersion = pLibraryVersion[v].wBuildVersion;
			QFEVersion = pLibraryVersion[v].wFlags.QFEVersion;

			if (xdkVersion < BuildVersion) {
				xdkVersion = BuildVersion;
			}
			XbLibFlag = XbSymbolDatabase_LibraryToFlag(std::string(pLibraryVersion[v].szName, pLibraryVersion[v].szName + 8).c_str());
			XbLibScan |= XbLibFlag;

			// Keep certain library versions for plugin usage.
			if ((XbLibFlag & (XbSymbolLib_D3D8 | XbSymbolLib_D3D8LTCG)) > 0) {
				if (g_LibVersion_D3D8 < BuildVersion) {
					g_LibVersion_D3D8 = BuildVersion;
				}
			}
			else if ((XbLibFlag & XbSymbolLib_DSOUND) > 0) {
				g_LibVersion_DSOUND = BuildVersion;
			}
		}

		// Since XDK 4039 title does not have library version for DSOUND, let's check section header if it exists or not.
		for (unsigned int v = 0; v < pXbeHeader->dwSections; v++) {
			SectionName = (const char*)pSectionHeaders[v].dwSectionNameAddr;
			if (strncmp(SectionName, Lib_DSOUND, 8) == 0) {
				XbLibScan |= XbSymbolLib_DSOUND;

				// If DSOUND version is not set, we need to force set it.
				if (g_LibVersion_DSOUND == 0) {
					g_LibVersion_DSOUND = xdkVersion;
				}
				break;
			}
		}
	}

	EmuUpdateLLEStatus(XbLibScan);

	std::cout << "\n"
	    "*******************************************************************************\n"
	    "* Cxbx-Reloaded High Level Emulation database\n"
	    "*******************************************************************************\n"
	    << std::endl;

	// Make sure the Symbol Cache directory exists
	std::string cachePath = std::string(szFolder_CxbxReloadedData) + "\\SymbolCache\\";
	if (!std::filesystem::exists(cachePath) && !std::filesystem::create_directory(cachePath)) {
		CxbxKrnlCleanup("Couldn't create Cxbx-Reloaded SymbolCache folder!");
	}

	// Hash the loaded XBE's header, use it as a filename
	uint64_t uiHash = ComputeHash((void*)&CxbxKrnl_Xbe->m_Header, sizeof(Xbe::Header));
	std::stringstream sstream;
	char tAsciiTitle[40] = "Unknown";
	std::setlocale(LC_ALL, "English");
	std::wcstombs(tAsciiTitle, g_pCertificate->wszTitleName, sizeof(tAsciiTitle));
	std::string szTitleName(tAsciiTitle);
	CxbxKrnl_Xbe->PurgeBadChar(szTitleName);
	sstream << cachePath << szTitleName << "-" << std::hex << uiHash << ".ini";
	std::string filename = sstream.str();

	// This will fire when we exit this function scope; either after detecting a previous cache file, or when one is created
	CxbxDebuggerScopedMessage symbolCacheFilename(filename);

	CSimpleIniA symbolCacheData;

	if (std::filesystem::exists(filename.c_str())) {
		std::printf("Found Symbol Cache File: %08llX.ini\n", uiHash);

		symbolCacheData.LoadFile(filename.c_str());

		xdkVersion = (uint16_t)symbolCacheData.GetLongValue(section_libs, sect_libs_keys.BuildVersion, /*Default=*/0);

		// Verify the version of the cache file against the Symbol Database version hash
		const uint32_t SymbolDatabaseVersionHash = symbolCacheData.GetLongValue(section_info, sect_info_keys.SymbolDatabaseVersionHash, /*Default=*/0);

		if (SymbolDatabaseVersionHash == XbSymbolDatabase_LibraryVersion()) {
			g_SymbolCacheUsed = true;
			CSimpleIniA::TNamesDepend symbol_names;

			if (g_SymbolCacheUsed) {
				std::printf("Using Symbol Cache\n");

				// Parse the .INI file into the map of symbol addresses
				symbolCacheData.GetAllKeys(section_symbols, symbol_names);
				for (auto it = symbol_names.begin(); it != symbol_names.end(); ++it) {
					g_SymbolAddresses[it->pItem] = symbolCacheData.GetLongValue(section_symbols, it->pItem, /*Default=*/0);
				}

				// Iterate through the map of symbol addresses, calling GetEmuPatchAddr on all functions.
				for (auto it = g_SymbolAddresses.begin(); it != g_SymbolAddresses.end(); ++it) {
					std::string functionName = it->first;
					xbaddr location = it->second;

					std::stringstream output;
					output << "SymbolCache: 0x" << std::setfill('0') << std::setw(8) << std::hex << location
					    << " -> " << functionName << "\n";
					std::printf(output.str().c_str());
				}

				// Fix up Render state and Texture States
				if (g_SymbolAddresses.find("D3DDeferredRenderState") == g_SymbolAddresses.end()
				    || g_SymbolAddresses["D3DDeferredRenderState"] == 0) {
					EmuLog(LOG_LEVEL::WARNING, "EmuD3DDeferredRenderState was not found!");
				}

				if (g_SymbolAddresses.find("D3DDeferredTextureState") == g_SymbolAddresses.end()
				    || g_SymbolAddresses["D3DDeferredTextureState"] == 0) {
					EmuLog(LOG_LEVEL::WARNING, "EmuD3DDeferredTextureState was not found!");
				}

				if (g_SymbolAddresses.find("D3DDEVICE") == g_SymbolAddresses.end()
				    || g_SymbolAddresses["D3DDEVICE"] == 0) {
					EmuLog(LOG_LEVEL::WARNING, "D3DDEVICE was not found!");
				}
			}
		}

		// If g_SymbolAddresses didn't get filled, then symbol cache is invalid
		if (g_SymbolAddresses.empty()) {
			std::printf("Symbol Cache file is outdated and will be regenerated\n");
			g_SymbolCacheUsed = false;
		}
	}

	// If the Symbol Cache was used, go straight to patching, no need to re-scan
	if (g_SymbolCacheUsed) {
		EmuInstallPatches();
		return;
	}

	//
	// initialize Microsoft XDK emulation
	//
	if(pLibraryVersion != nullptr) {

		std::printf("Symbol: Detected Microsoft XDK application...\n");

#if 0 // NOTE: This code is currently disabled due to not optimized and require more work to do.

        XbSymbolRegisterLibrary(XbLibScan);

        while (true) {

            size_t SymbolSize = g_SymbolAddresses.size();

            Xbe::SectionHeader* pSectionHeaders = reinterpret_cast<Xbe::SectionHeader*>(pXbeHeader->dwSectionHeadersAddr);
            Xbe::SectionHeader* pSectionScan = nullptr;

            for (uint32_t v = 0; v < pXbeHeader->dwSections; v++) {

                pSectionScan = pSectionHeaders + v;

                XbSymbolScanSection((uint32_t)pXbeHeader, 64 * ONE_MB, (const char*)pSectionScan->dwSectionNameAddr, pSectionScan->dwVirtualAddr, pSectionScan->dwSizeofRaw, xdkVersion, EmuRegisterSymbol);
            }

            // If symbols are not adding to array, break the loop.
            if (SymbolSize == g_SymbolAddresses.size()) {
                break;
            }
        }
#endif

		XbSymbolDatabase_SetOutputMessage(EmuOutputMessage);

		XbSymbolScan(pXbeHeader, EmuRegisterSymbol, false);
	}

	std::printf("\n");

	// Perform a reset just in case a cached file data still exist.
	symbolCacheData.Reset();

	// Store Symbol Database version
	symbolCacheData.SetLongValue(section_info, sect_info_keys.SymbolDatabaseVersionHash, XbSymbolDatabase_LibraryVersion(), nullptr, /*UseHex =*/false);

	// Store Certificate Details
	symbolCacheData.SetValue(section_certificate, sect_certificate_keys.Name, tAsciiTitle);
	symbolCacheData.SetValue(section_certificate, sect_certificate_keys.TitleID, FormatTitleId(g_pCertificate->dwTitleId).c_str());
	symbolCacheData.SetLongValue(section_certificate, sect_certificate_keys.TitleIDHex, g_pCertificate->dwTitleId, nullptr, /*UseHex =*/true);
	symbolCacheData.SetLongValue(section_certificate, sect_certificate_keys.Region, g_pCertificate->dwGameRegion, nullptr, /*UseHex =*/true);

	// Store Library Details
	for (unsigned int i = 0; i < pXbeHeader->dwLibraryVersions; i++) {
		std::string LibraryName(pLibraryVersion[i].szName, pLibraryVersion[i].szName + 8);
		symbolCacheData.SetLongValue(section_libs, LibraryName.c_str(), pLibraryVersion[i].wBuildVersion, nullptr, /*UseHex =*/false);
	}

	symbolCacheData.SetLongValue(section_libs, sect_libs_keys.BuildVersion, xdkVersion, nullptr, /*UseHex =*/false);

	// Store detected symbol addresses
	for(auto it = g_SymbolAddresses.begin(); it != g_SymbolAddresses.end(); ++it) {
		symbolCacheData.SetLongValue(section_symbols, it->first.c_str(), it->second, nullptr, /*UseHex =*/true);
	}

	// Save data to unique symbol cache file
	symbolCacheData.SaveFile(filename.c_str());

	EmuInstallPatches();
}


#if 0 // TODO: Need to move this into XbSymbolDatabase for depth verification usage.
#ifdef _DEBUG_TRACE

struct HLEVerifyContext {
    const HLEData *main_data;
    OOVPA *oovpa, *against;
    const HLEData *against_data;
    uint32_t main_index, against_index;
};

std::string HLEErrorString(const HLEData *data, uint16_t buildVersion, uint32_t index)
{
    std::string result =
        "OOVPATable " + (std::string)(data->LibSec.library) + "_" + std::to_string(buildVersion)
        + "[" + std::to_string(index) + "] "
        + (std::string)(data->OovpaTable[index].szFuncName);

    return result;
}

void HLEError(HLEVerifyContext *context, uint16_t buildVersion, char *format, ...)
{
    std::string output = "HLE Error ";
    if (context->main_data != nullptr) {
        output += "in " + HLEErrorString(context->main_data, buildVersion, context->main_index);
    }

    if (context->against != nullptr && context->against_data != nullptr) {
        output += ", comparing against " + HLEErrorString(context->against_data, buildVersion, context->against_index);
    }

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

void VerifyHLEOOVPA(HLEVerifyContext *context, uint16_t buildVersion, OOVPA *oovpa)
{
    if (context->against == nullptr) {
        // TODO : verify XRefSaveIndex and XRef's (how?)

        // verify offsets are in increasing order
        uint32_t prev_offset;
        uint8_t dummy_value;
        GetOovpaEntry(oovpa, oovpa->XRefCount, prev_offset, dummy_value);
        for (int p = oovpa->XRefCount + 1; p < oovpa->Count; p++) {
            uint32_t curr_offset;
            GetOovpaEntry(oovpa, p, curr_offset, dummy_value);
            if (!(curr_offset > prev_offset)) {
                HLEError(context, buildVersion, "Lovp[%d] : Offset (0x%x) must be larger then previous offset (0x%x)",
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
    if (context->against == oovpa) {
        return;
    }

    // compare {Offset, Value}-pairs between two OOVPA's
    OOVPA *left = context->against, *right = oovpa;
    int l = 0, r = 0;
    uint32_t left_offset, right_offset;
    uint8_t left_value, right_value;
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
        } else if (left_offset > right_offset) {
            unique_offset_right++;
            left_next = false;
        } else if (left_value == right_value) {
            equal_offset_value++;
        } else {
            equal_offset_different_value++;
        }

        // increment r before use (in left_next)
        if (right_next) {
            r++;
        }

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
    if (equal_offset_different_value == 0) {
        // enough matching OV-pairs?
        if (equal_offset_value > 4) {
            // no unique OV-pairs on either side?
            if (unique_offset_left + unique_offset_right == 0) {
                HLEError(context, buildVersion, "OOVPA's are identical",
                         unique_offset_left,
                         unique_offset_right);
            } else {
                // not too many new OV-pairs on the left side?
                if (unique_offset_left < 6) {
                    // not too many new OV-parirs on the right side?
                    if (unique_offset_right < 6) {
                        HLEError(context, buildVersion, "OOVPA's are expanded (left +%d, right +%d)",
                                 unique_offset_left,
                                 unique_offset_right);
                    }
                }
            }
        }
    }
}

void VerifyHLEDataEntry(HLEVerifyContext *context, const OOVPATable *table, uint32_t index)
{
    if (context->against == nullptr) {
        context->main_index = index;
    } else {
        context->against_index = index;
    }

    if (context->against == nullptr) {
        const char* checkDisableStr = nullptr;
        size_t getFuncStrLength = strlen(table[index].szFuncName);

        if (getFuncStrLength > 10) {
            checkDisableStr = &table[index].szFuncName[getFuncStrLength - 10];
        }

        if (checkDisableStr != nullptr && strcmp(checkDisableStr, "_UNPATCHED") == 0) {
            if (GetEmuPatchAddr((std::string)table[index].szFuncName)) {
                HLEError(context, table[index].Version, "OOVPA registration UNPATCHED while a patch exists!");
            }
        } else if (table[index].Oovpa->XRefSaveIndex != XRefNoSaveIndex) {
            if (GetEmuPatchAddr((std::string)table[index].szFuncName)) {
                HLEError(context, table[index].Version, "OOVPA registration XREF while a patch exists!");
            }
        }
    }

    // verify the OOVPA of this entry
    if (table[index].Oovpa != nullptr) {
        VerifyHLEOOVPA(context, table[index].Version, table[index].Oovpa);
    }
}

void VerifyHLEData(HLEVerifyContext *context, const HLEData *data)
{
    if (context->against == nullptr) {
        context->main_data = data;
    } else {
        context->against_data = data;
    }

    // Don't check a database against itself :
    if (context->main_data == context->against_data) {
        return;
    }

    // verify each entry in this HLEData
    for (uint32_t e = 0; e < data->OovpaTableCount; e++) {
        VerifyHLEDataEntry(context, data->OovpaTable, e);
    }
}

void VerifyHLEDataBaseAgainst(HLEVerifyContext *context)
{
    // verify all HLEData's
    for (uint32_t d = 0; d < HLEDataBaseCount; d++) {
        VerifyHLEData(context, &HLEDataBase[d]);
    }
}

void VerifyHLEDataBase()
{
    HLEVerifyContext context = { 0 };
    VerifyHLEDataBaseAgainst(&context);
}
#endif // _DEBUG_TRACE
#endif
