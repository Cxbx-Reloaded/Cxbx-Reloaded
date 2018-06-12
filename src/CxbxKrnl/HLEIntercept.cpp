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

#define LOG_PREFIX "HLE " // Intentional extra space to align on 4 characters

#include <cmath>
#include <iomanip> // For std::setfill and std::setw
#include "CxbxKrnl.h"
#include "Emu.h"
#include "EmuFS.h"
#include "EmuXTL.h"
#include "EmuShared.h"
#include "CxbxDebugger.h"
#pragma comment(lib, "XbSymbolDatabase.lib")
#include "../../import/XbSymbolDatabase/XbSymbolDatabase.h"
#include "HLEIntercept.h"
#include "xxhash32.h"
#include <Shlwapi.h>
#include <subhook.h>

inline void EmuInstallPatch(std::string FunctionName, xbaddr FunctionAddr, void *Patch);

#include <shlobj.h>
#include <unordered_map>
#include <map>
#include <sstream>

std::map<std::string, xbaddr> g_SymbolAddresses;
std::unordered_map<std::string, subhook::Hook> g_FunctionHooks;
bool g_HLECacheUsed = false;

// D3D build version
uint32 g_BuildVersion = 0;

bool bLLE_APU = false; // Set this to true for experimental APU (sound) LLE
bool bLLE_GPU = false; // Set this to true for experimental GPU (graphics) LLE
bool bLLE_JIT = false; // Set this to true for experimental JIT

void* GetXboxFunctionPointer(std::string functionName)
{
    if (g_FunctionHooks.find(functionName) != g_FunctionHooks.end()) {
        return g_FunctionHooks[functionName].GetTrampoline();
    }

    // If we got here, the function wasn't patched, so we can just look it up the HLE cache
    // and return the correct offset
    auto symbol = g_SymbolAddresses.find(functionName);
    if (symbol != g_SymbolAddresses.end()) {
        return (void*)symbol->second;
    }

    // Finally, if none of the above were matched, return nullptr
    return nullptr;
}

// NOTE: GetDetectedSymbolName do not get to be in XbSymbolDatabase, get symbol string in Cxbx project only.
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

// NOTE: GetEmuPatchAddr do not get to be in XbSymbolDatabase, perform patch check in Cxbx project only.
void *GetEmuPatchAddr(std::string aFunctionName)
{
    std::string patchName = "XTL::EmuPatch_" + aFunctionName;
    void* addr = GetProcAddress(GetModuleHandle(NULL), patchName.c_str());
    return addr;
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

    // For XREF_D3DTSS_TEXCOORDINDEX, Kabuki Warriors hits this case
    CxbxPopupMessage("Verification of %s failed : XREF was 0x%.8X while lookup gave 0x%.8X", SymbolName, XRefAddr, Address);
    // For XREF_D3DTSS_TEXCOORDINDEX, Kabuki Warriors hits this case
    return false;
}*/

// x1nixmzeng: Hack to notify CxbxDebugger of the HLECache file, which is currently a hashed XBE header AND stripped title (see EmuHLEIntercept)
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
            EmuWarning("%s", message);
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
    output << "HLE: 0x" << std::setfill('0') << std::setw(8) << std::hex << func_addr
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
                    EmuWarning("Found OOVPA after first finding nothing?");
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
                    EmuWarning("Cannot derive a save index!");
                    break;
                }
                default:
                {
                    if (XRefDataBase[OovpaTable->Oovpa->XRefSaveIndex] != pFunc) {
                        EmuWarning("Found OOVPA on other address than in XRefDataBase!");
                        EmuWarning("%s: %4d - pFunc: %08X, stored: %08X", OovpaTable->szFuncName, Oovpa->XRefSaveIndex, pFunc, XRefDataBase[Oovpa->XRefSaveIndex]);
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

    // NOTE: Alternate fix, however it will not register symbols just like the original method did.
    //       We need to create an array for symbol, patch, library type, etc structure.
    //       Then we can replace checks below into permanent solution.
    if (bLLE_APU && ((library_flag & XbSymbolLib_XACTENG) || (library_flag & XbSymbolLib_DSOUND) > 0)) {
        // Do nothing if emulating LLE APU
    } else if (bLLE_GPU && ((library_flag & XbSymbolLib_XGRAPHC) || (library_flag & XbSymbolLib_D3D8) || (library_flag & XbSymbolLib_D3D8LTCG) > 0)) {
        // Do nothing if emulating LLE GPU
    } else {
        // Or else check if patch exist then patch it.

        // Now that we found the address, store it (regardless if we patch it or not)
        g_SymbolAddresses[symbol_str] = func_addr;

        // Retrieve the associated patch, if any is available
        void* addr = GetEmuPatchAddr(symbol_str);

        if (addr != nullptr) {
            EmuInstallPatch(symbol_str, func_addr, addr);
            output << "\t*PATCHED*";
        }
    }
    output << "\n";
    printf(output.str().c_str());
}

// TODO: Move this into a function rather than duplicating from HLE scanning code
void EmuD3D_Init_DeferredStates()
{
    if (g_SymbolAddresses.find("D3DDeferredRenderState") != g_SymbolAddresses.end()) {
        XTL::EmuD3DDeferredRenderState = (DWORD*)g_SymbolAddresses["D3DDeferredRenderState"];
    }
    if (g_SymbolAddresses.find("D3DDeferredTextureState") != g_SymbolAddresses.end()) {
        XTL::EmuD3DDeferredTextureState = (DWORD*)g_SymbolAddresses["D3DDeferredTextureState"];
    }

    if (XTL::EmuD3DDeferredRenderState != nullptr) {
        for (int v = 0; v < 44; v++) {
            XTL::EmuD3DDeferredRenderState[v] = XTL::X_D3DRS_UNK;
        }
    }
    if (XTL::EmuD3DDeferredTextureState != nullptr) {
        for (int s = 0; s < 4; s++) {
            for (int v = 0; v < 32; v++)
                XTL::EmuD3DDeferredTextureState[v + s * 32] = X_D3DTSS_UNK;
        }
    }
}

// Update shared structure with GUI process
void EmuUpdateLLEStatus(uint32_t XbLibScan)
{
    int FlagsLLE;
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
    g_EmuShared->SetFlagsLLE(&FlagsLLE);
}

// NOTE: EmuHLEIntercept do not get to be in XbSymbolDatabase, do the intecept in Cxbx project only.
void EmuHLEIntercept(Xbe::Header *pXbeHeader)
{
    // NOTE: Increase this revision number any time we changed something inside Cxbx-Reloaded.
    int revisionCache = 4;

    Xbe::LibraryVersion *pLibraryVersion = (Xbe::LibraryVersion*)pXbeHeader->dwLibraryVersionsAddr;

    uint16 xdkVersion = 0;
    uint32_t XbLibScan = 0;

    // NOTE: We need to check if title has library header to optimize verification process.
    if (pLibraryVersion != nullptr) {
        uint32 dwLibraryVersions = pXbeHeader->dwLibraryVersions;
        const char* SectionName = nullptr;
        Xbe::SectionHeader* pSectionHeaders = (Xbe::SectionHeader*)pXbeHeader->dwSectionHeadersAddr;

        // Get the highest revision build and prefix library to scan.
        for (uint32 v = 0; v < dwLibraryVersions; v++) {
            uint16 BuildVersion = pLibraryVersion[v].wBuildVersion;
            uint16 QFEVersion = pLibraryVersion[v].wFlags.QFEVersion;

            if (xdkVersion < BuildVersion) {
                xdkVersion = BuildVersion;
            }
            XbLibScan |= XbSymbolLibrayToFlag(std::string(pLibraryVersion[v].szName, pLibraryVersion[v].szName + 8).c_str());
        }

        // Since XDK 4039 title does not have library version for DSOUND, let's check section header if it exists or not.
        for (unsigned int v = 0; v < pXbeHeader->dwSections; v++) {
            SectionName = (const char*)pSectionHeaders[v].dwSectionNameAddr;
            if (strncmp(SectionName, Lib_DSOUND, 8) == 0) {
                XbLibScan |= XbSymbolLib_DSOUND;
                break;
            }
        }
    }

    EmuUpdateLLEStatus(XbLibScan);
    int gFlagsLLE;
    g_EmuShared->GetFlagsLLE(&gFlagsLLE);

    printf("\n");
    printf("*******************************************************************************\n");
    printf("* Cxbx-Reloaded High Level Emulation database\n");
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
    char tAsciiTitle[40] = "Unknown";
    setlocale(LC_ALL, "English");
    wcstombs(tAsciiTitle, g_pCertificate->wszTitleName, sizeof(tAsciiTitle));
    std::string szTitleName(tAsciiTitle);
    CxbxKrnl_Xbe->PurgeBadChar(szTitleName);
    sstream << cachePath << szTitleName << "-" << std::hex << uiHash << ".ini";
    std::string filename = sstream.str();

    // This will fire when we exit this function scope; either after detecting a previous cache file, or when one is created
    CxbxDebuggerScopedMessage hleCacheFilename(filename);

    if (PathFileExists(filename.c_str())) {
        printf("Found HLE Cache File: %08X.ini\n", uiHash);

        g_BuildVersion = GetPrivateProfileInt("Libs", "D3D8_BuildVersion", 0, filename.c_str());

        // Verify the version of the cache file against the HLE Database
        const uint32 HLECacheHash = GetPrivateProfileInt("Info", "HLECacheHash", 0, filename.c_str());

        if (HLECacheHash == XbSymbolLibraryVersion()) {
            char buffer[SHRT_MAX] = { 0 };
            char* bufferPtr = buffer;
            g_HLECacheUsed = true;

            const uint32 cacheRevision = GetPrivateProfileInt("Info", "revision", 0, filename.c_str());
            const uint32 cacheFlagsLLE = GetPrivateProfileInt("Info", "FlagsLLE", 0, filename.c_str());

            if (cacheFlagsLLE != gFlagsLLE) {
                g_HLECacheUsed = false;
            }
            else if (cacheRevision != revisionCache) {
                g_HLECacheUsed = false;
            }

            if (g_HLECacheUsed) {
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
                    if (pFunc != nullptr) {
                        // skip entries that weren't located at all
                        if (location == NULL) {
                            output << "\t(not patched)";
                        }
                        // Prevent patching illegal addresses
                        else if (location < XBE_IMAGE_BASE) {
                            output << "\t*ADDRESS TOO LOW!*";
                        } else if (location > g_SystemMaxMemory) {
                            output << "\t*ADDRESS TOO HIGH!*";
                        } else {
                            EmuInstallPatch(functionName, location, pFunc);
                            output << "\t*PATCHED*";
                        }
                    } else {
                        if (location != NULL)
                            output << "\t(no patch)";
                    }

                    output << "\n";
                    printf(output.str().c_str());
                }

                // Fix up Render state and Texture States
                if (g_SymbolAddresses.find("D3DDeferredRenderState") == g_SymbolAddresses.end()
                    || g_SymbolAddresses["D3DDeferredRenderState"] == 0) {
                    EmuWarning("EmuD3DDeferredRenderState was not found!");
                }

                if (g_SymbolAddresses.find("D3DDeferredTextureState") == g_SymbolAddresses.end()
                    || g_SymbolAddresses["D3DDeferredTextureState"] == 0) {
                    EmuWarning("EmuD3DDeferredTextureState was not found!");
                }

                if (g_SymbolAddresses.find("D3DDEVICE") == g_SymbolAddresses.end()
                    || g_SymbolAddresses["D3DDEVICE"] == 0) {
                    EmuWarning("D3DDEVICE was not found!");
                }

                EmuD3D_Init_DeferredStates();
            }
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
    if(pLibraryVersion != nullptr) {

        printf("HLE: Detected Microsoft XDK application...\n");

        // TODO: Is this enough for alias? We need to verify it.
        if ((XbLibScan & XbSymbolLib_D3D8) > 0 || (XbLibScan & XbSymbolLib_D3D8LTCG) > 0) {
            g_BuildVersion = xdkVersion;
        }
#if 0 // NOTE: This is a note for what we should do for above.
        if (BuildVersion >= 5558 && BuildVersion <= 5659 && QFEVersion > 1) {
            EmuWarning("D3D8 version 1.0.%d.%d Title Detected: This game uses an alias version 1.0.5788", BuildVersion, QFEVersion);
            BuildVersion = 5788;
        }
#endif

#if 0 // NOTE: This code is currently disabled due to not optimized and require more work to do.

        XbSymbolRegisterLibrary(XbLibScan);

        while (true) {

            size_t SymbolSize = g_SymbolAddresses.size();

            Xbe::SectionHeader* pSectionHeaders = reinterpret_cast<Xbe::SectionHeader*>(pXbeHeader->dwSectionHeadersAddr);
            Xbe::SectionHeader* pSectionScan = nullptr;

            for (uint32 v = 0; v < pXbeHeader->dwSections; v++) {

                pSectionScan = pSectionHeaders + v;

                XbSymbolScanSection((uint32_t)pXbeHeader, 64 * ONE_MB, (const char*)pSectionScan->dwSectionNameAddr, pSectionScan->dwVirtualAddr, pSectionScan->dwSizeofRaw, xdkVersion, EmuRegisterSymbol);
            }

            // If symbols are not adding to array, break the loop.
            if (SymbolSize == g_SymbolAddresses.size()) {
                break;
            }
        }
#endif

        XbSymbolSetOutputMessage(EmuOutputMessage);

        XbSymbolScan(pXbeHeader, EmuRegisterSymbol);

        EmuD3D_Init_DeferredStates();
    }

    printf("\n");

    // Write the HLE Database version string
    {
        std::string HLECacheHashString = std::to_string(XbSymbolLibraryVersion());
        WritePrivateProfileString("Info", "HLECacheHash", HLECacheHashString.c_str(), filename.c_str());
    }


    std::stringstream revision;
    revision << std::dec << revisionCache;
    WritePrivateProfileString("Info", "revision", revision.str().c_str(), filename.c_str());

    std::stringstream flagsLLE;
    flagsLLE << std::dec << gFlagsLLE;
    WritePrivateProfileString("Info", "FlagsLLE", flagsLLE.str().c_str(), filename.c_str());

    // Write the Certificate Details to the cache file
    WritePrivateProfileString("Certificate", "Name", tAsciiTitle, filename.c_str());

    std::stringstream titleId;
    titleId << std::hex << g_pCertificate->dwTitleId;
    WritePrivateProfileString("Certificate", "TitleID", titleId.str().c_str(), filename.c_str());

    std::stringstream region;
    region << std::hex << g_pCertificate->dwGameRegion;
    WritePrivateProfileString("Certificate", "Region", region.str().c_str(), filename.c_str());

    // Write Library Details
    for (uint i = 0; i < pXbeHeader->dwLibraryVersions; i++) {
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
}

// NOTE: EmuInstallPatch do not get to be in XbSymbolDatabase, do the patches in Cxbx project only.
inline void EmuInstallPatch(std::string FunctionName, xbaddr FunctionAddr, void *Patch)
{
    g_FunctionHooks[FunctionName].Install((void*)(FunctionAddr), Patch);
}

#if 0 // TODO: Need to move this into XbSymbolDatabase for depth verification usage.
#ifdef _DEBUG_TRACE

struct HLEVerifyContext {
    const HLEData *main_data;
    OOVPA *oovpa, *against;
    const HLEData *against_data;
    uint32 main_index, against_index;
};

std::string HLEErrorString(const HLEData *data, uint16_t buildVersion, uint32 index)
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
        uint32 prev_offset;
        uint08 dummy_value;
        GetOovpaEntry(oovpa, oovpa->XRefCount, prev_offset, dummy_value);
        for (int p = oovpa->XRefCount + 1; p < oovpa->Count; p++) {
            uint32 curr_offset;
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

void VerifyHLEDataEntry(HLEVerifyContext *context, const OOVPATable *table, uint32 index)
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
    for (uint32 e = 0; e < data->OovpaTableCount; e++) {
        VerifyHLEDataEntry(context, data->OovpaTable, e);
    }
}

void VerifyHLEDataBaseAgainst(HLEVerifyContext *context)
{
    // verify all HLEData's
    for (uint32 d = 0; d < HLEDataBaseCount; d++) {
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
