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
// *
// *  All rights reserved
// *
// ******************************************************************

#include "common\xbe\XbePrinter.h"
#include "CxbxVersion.h" // For CxbxVersionStr

#include <locale> // For ctime
#include <sstream> // For std::stringstream
#include <iomanip> // For std::setfill, std::uppercase, std::hex
#include "common/util/strConverter.hpp" // for utf16le_to_ascii

extern std::string FormatTitleId(uint32_t title_id); // Exposed in Emu.cpp

// better time
static char *BetterTime(uint32_t x_timeDate)
{
    time_t x_time = x_timeDate;
    char *x_ctime = ctime(&x_time);

    int v=0;

    for(v=0;x_ctime[v] != '\n';v++);

    x_ctime[v] = '\0';

    return x_ctime;
}

std::string DumpInformation(Xbe* Xbe_object)
{
    if(Xbe_object->HasError()) {
        return "ERROR";
    }
    XbePrinter printer(Xbe_object);
    return printer.GenXbeInfo();
}

#define SSTREAM_SET_HEX(stream_name) stream_name << std::setfill('0') << std::uppercase << std::hex;

XbePrinter::XbePrinter(Xbe* Xbe_object)
{
    Xbe_to_print = Xbe_object;
    Xbe_header = &(Xbe_object->m_Header);
    Xbe_certificate = &(Xbe_object->m_Certificate);
}

std::string XbePrinter::GenXbeInfo()
{
    std::string info;
    info.append(GenDumpHeader());
    info.append(GenXbeHeaderInfo());
    info.append(GenXbeCertificateInfo());
	info.append(ValidateXbeSignature());
    info.append(GenSectionInfo());
    info.append(GenLibraryVersions());
    info.append(GenTLS());
    return info;
}

std::string XbePrinter::GenHexRow(
    uint8_t *signature, const uint8_t row, const uint8_t row_size
)
{
    const uint16_t offset = row * row_size;
    std::stringstream text;
    SSTREAM_SET_HEX(text);
    for(uint8_t x = 0; x < row_size; x++) {
        text << std::setw(2) << static_cast<unsigned>(signature[offset + x]);
    }
    return text.str();
}

std::string XbePrinter::AllowedMediaToString()
{
    const uint32_t dwAllowedMedia = Xbe_certificate->dwAllowedMedia;
    std::string text = "Media Types:";

    if(dwAllowedMedia & XBEIMAGE_MEDIA_TYPE_MEDIA_MASK) {
        if(dwAllowedMedia & XBEIMAGE_MEDIA_TYPE_HARD_DISK)
            text.append(" HARD_DISK");
        if(dwAllowedMedia & XBEIMAGE_MEDIA_TYPE_DVD_X2)
            text.append(" DVD_X2");
        if(dwAllowedMedia & XBEIMAGE_MEDIA_TYPE_DVD_CD)
            text.append(" DVD_CD");
        if(dwAllowedMedia & XBEIMAGE_MEDIA_TYPE_CD)
            text.append(" CD");
        if(dwAllowedMedia & XBEIMAGE_MEDIA_TYPE_DVD_5_RO)
            text.append(" DVD_5_RO");
        if(dwAllowedMedia & XBEIMAGE_MEDIA_TYPE_DVD_9_RO)
            text.append(" DVD_9_RO");
        if(dwAllowedMedia & XBEIMAGE_MEDIA_TYPE_DVD_5_RW)
            text.append(" DVD_5_RW");
        if(dwAllowedMedia & XBEIMAGE_MEDIA_TYPE_DVD_9_RW)
            text.append(" DVD_9_RW");
        if(dwAllowedMedia & XBEIMAGE_MEDIA_TYPE_DONGLE)
            text.append(" DONGLE");
        if(dwAllowedMedia & XBEIMAGE_MEDIA_TYPE_MEDIA_BOARD)
            text.append(" BOARD");
        if((dwAllowedMedia & XBEIMAGE_MEDIA_TYPE_MEDIA_MASK) >= (XBEIMAGE_MEDIA_TYPE_MEDIA_BOARD * 2))
            text.append(" UNKNOWN");
    }

    if(dwAllowedMedia & ~XBEIMAGE_MEDIA_TYPE_MEDIA_MASK) {
        text.append(" NONSECURE");
        if(dwAllowedMedia & XBEIMAGE_MEDIA_TYPE_NONSECURE_HARD_DISK)
            text.append(" HARD_DISK");
        if(dwAllowedMedia & XBEIMAGE_MEDIA_TYPE_NONSECURE_MODE)
            text.append(" MODE");
    }
    return text;
}

std::string XbePrinter::GameRatingToString()
{
    std::string text;
    // Info from: https://xboxdevwiki.net/EEPROM
    switch(Xbe_certificate->dwGameRatings) {
        case 0x0:
            text.append("(RP) Rating Pending");
            break;
        case 0x1:
            text.append("(AO) Adults Only");
            break;
        case 0x2:
            text.append("(M) Mature");
            break;
        case 0x3:
            text.append("(T) Teen");
            break;
        case 0x4:
            text.append("(E) Everyone");
            break;
        case 0x5:
            text.append("(K-A) Kids to Adults");
            break;
        case 0x6:
            text.append("(EC) Early Childhood");
            break;
        default:
            text.append("ERROR: no rating");
            break;
    }
    return text;
}

std::string XbePrinter::GenDumpHeader()
{
    std::string text;
    text.append("XBE information generated by Cxbx-Reloaded (Version ");
    text.append(CxbxVersionStr);
    text.append(")\n\n");
    text.append("Title identified as \"");
    text.append(Xbe_to_print->m_szAsciiTitle);
    text.append("\"\n\n");
    text.append("Dumping XBE file header...\n\n");
    text.append("Magic Number                     : XBEH\n");
    return text;
}

std::string XbePrinter::GenXbeHeaderInfo()
{
    std::string text;
    text.append(GenDigitalSignature());
    text.append(GenGeneralHeaderInfo1());
    text.append(GenInitFlags());
    text.append(GenGeneralHeaderInfo2());
    return text;
}

std::string XbePrinter::GenDigitalSignature()
{
    const uint8_t row_size = 16;
    std::string text;
    text.append("Digital Signature               : <Hex Dump>");
    for(int row = 0; row < 16; row++) {
        text.append("\n                                   ");
        text.append(GenHexRow(&(Xbe_header->pbDigitalSignature[0]), row, row_size));
    }
    text.append("\n                                   </Hex Dump>\n");
    return text;
}

std::string XbePrinter::GenGeneralHeaderInfo1()
{
    std::stringstream text;
    SSTREAM_SET_HEX(text);
    text << "Base Address                     : 0x" << std::setw(8) << Xbe_header->dwBaseAddr << "\n";
    text << "Size of Headers                  : 0x" << std::setw(8) << Xbe_header->dwSizeofHeaders << "\n";
    text << "Size of Image                    : 0x" << std::setw(8) << Xbe_header->dwSizeofImage << "\n";
    text << "Size of Image Header             : 0x" << std::setw(8) << Xbe_header->dwSizeofImageHeader << "\n";
    text << "TimeDate Stamp                   : 0x" << std::setw(8) << Xbe_header->dwTimeDate << " (" << BetterTime(Xbe_header->dwTimeDate) << ")\n";
    text << "Certificate Address              : 0x" << std::setw(8) << Xbe_header->dwCertificateAddr << "\n";
    text << "Number of Sections               : 0x" << std::setw(8) << Xbe_header->dwSections << "\n";
    text << "Section Headers Address          : 0x" << std::setw(8) << Xbe_header->dwSectionHeadersAddr << "\n";
    return text.str();
}

std::string XbePrinter::GenInitFlags()
{
    std::stringstream text;
    SSTREAM_SET_HEX(text);
    text << "Init Flags                       : 0x" << std::setw(8) << Xbe_header->dwInitFlags_value << " ";

    if(Xbe_header->dwInitFlags.bMountUtilityDrive) {
        text << "[Mount Utility Drive] ";
    }
    if(Xbe_header->dwInitFlags.bFormatUtilityDrive) {
        text << "[Format Utility Drive] ";
    }
    if(Xbe_header->dwInitFlags.bLimit64MB) {
        text << "[Limit Devkit Run Time Memory to 64MB] ";
    }
    if(!Xbe_header->dwInitFlags.bDontSetupHarddisk) {
        text << "[Setup Harddisk] ";
    }
    text << "\n";
    return text.str();
}

std::string XbePrinter::GenGeneralHeaderInfo2()
{
    const uint32_t retail_entry_point = Xbe_header->dwEntryAddr ^ XOR_EP_RETAIL;
    const uint32_t debug_entry_point = Xbe_header->dwEntryAddr ^ XOR_EP_DEBUG;
    const uint32_t retail_thunk_addr = Xbe_header->dwKernelImageThunkAddr ^ XOR_KT_RETAIL;
    const uint32_t debug_thunk_addr = Xbe_header->dwKernelImageThunkAddr ^ XOR_KT_DEBUG;
    const std::string AsciiFilename = utf16le_to_ascii(Xbe_to_print->GetUnicodeFilenameAddr());
    std::stringstream text;
    SSTREAM_SET_HEX(text);
    text << "Entry Point                      : 0x" << std::setw(8) << Xbe_header->dwEntryAddr << " (Retail: 0x" << std::setw(8) << retail_entry_point << ", Debug: 0x" <<  std::setw(8) << debug_entry_point << ")\n";
    text << "TLS Address                      : 0x" << std::setw(8) << Xbe_header->dwTLSAddr << "\n";
    text << "(PE) Stack Commit                : 0x" << std::setw(8) << Xbe_header->dwPeStackCommit << "\n";
    text << "(PE) Heap Reserve                : 0x" << std::setw(8) << Xbe_header->dwPeHeapReserve << "\n";
    text << "(PE) Heap Commit                 : 0x" << std::setw(8) << Xbe_header->dwPeHeapCommit << "\n";
    text << "(PE) Base Address                : 0x" << std::setw(8) << Xbe_header->dwPeBaseAddr << "\n";
    text << "(PE) Size of Image               : 0x" << std::setw(8) << Xbe_header->dwPeSizeofImage << "\n";
    text << "(PE) Checksum                    : 0x" << std::setw(8) << Xbe_header->dwPeChecksum << "\n";
    text << "(PE) TimeDate Stamp              : 0x" << std::setw(8) << Xbe_header->dwPeTimeDate << " (" << BetterTime(Xbe_header->dwPeTimeDate) << ")\n";
    text << "Debug Pathname Address           : 0x" << std::setw(8) << Xbe_header->dwDebugPathnameAddr << " (\"" << Xbe_to_print->GetAddr(Xbe_header->dwDebugPathnameAddr) << "\")\n";
    text << "Debug Filename Address           : 0x" << std::setw(8) << Xbe_header->dwDebugFilenameAddr << " (\"" << Xbe_to_print->GetAddr(Xbe_header->dwDebugFilenameAddr) << "\")\n";
    text << "Debug Unicode filename Address   : 0x" << std::setw(8) << Xbe_header->dwDebugUnicodeFilenameAddr << " (L\"" << AsciiFilename << "\")\n";
    text << "Kernel Image Thunk Address       : 0x" << std::setw(8) << Xbe_header->dwKernelImageThunkAddr << " (Retail: 0x" << std::setw(8) << retail_thunk_addr << ", Debug: 0x" <<  std::setw(8) << debug_thunk_addr << ")\n";
    text << "NonKernel Import Dir Address     : 0x" << std::setw(8) << Xbe_header->dwNonKernelImportDirAddr << "\n";
    text << "Library Versions                 : 0x" << std::setw(8) << Xbe_header->dwLibraryVersions << "\n";
    text << "Library Versions Address         : 0x" << std::setw(8) << Xbe_header->dwLibraryVersionsAddr << "\n";
    text << "Kernel Library Version Address   : 0x" << std::setw(8) << Xbe_header->dwKernelLibraryVersionAddr << "\n";
    text << "XAPI Library Version Address     : 0x" << std::setw(8) << Xbe_header->dwXAPILibraryVersionAddr << "\n";
    text << "Logo Bitmap Address              : 0x" << std::setw(8) << Xbe_header->dwLogoBitmapAddr << "\n";
    text << "Logo Bitmap Size                 : 0x" << std::setw(8) << Xbe_header->dwSizeofLogoBitmap << "\n\n";
    return text.str();
}

std::string XbePrinter::ValidateXbeSignature()
{
	std::string text("\nInvalid xbe signature. Homebrew, tampered or pirated xbe?\n");
	if (Xbe_to_print->CheckSignature()) {
		text = "\nValid xbe signature. Xbe is legit\n";
	}
	return text;
}

std::string XbePrinter::GenXbeCertificateInfo()
{
    std::string text;
    text.append(GenCertificateHeader());
    text.append(GenAlternateTitleIDs());
    text.append(GenMediaInfo());
    text.append(GenLANKey());
    text.append(GenSignatureKey());
    text.append(GenAlternateSignatureKeys());
    text.append(GenExtraInfo());
    return text;
}

std::string XbePrinter::GenCertificateHeader()
{
    std::stringstream text;
    SSTREAM_SET_HEX(text);
    text << "Dumping XBE Certificate...\n\n";
    text << "Size of Certificate              : 0x" << std::setw(8) << Xbe_certificate->dwSize << "\n";
    text << "TimeDate Stamp                   : 0x" << std::setw(8) << Xbe_certificate->dwTimeDate << " (" << BetterTime(Xbe_certificate->dwTimeDate) << ")\n";
    text << "Title ID                         : " << FormatTitleId(Xbe_certificate->dwTitleId) << "\n";
    text << "Title ID (Hex)                   : 0x" << std::setw(8) << Xbe_certificate->dwTitleId << "\n";
    text << "Title                            : L\"" << Xbe_to_print->m_szAsciiTitle << "\"\n";
    return text.str();
}

std::string XbePrinter::GenAlternateTitleIDs()
{
    std::stringstream text;
    SSTREAM_SET_HEX(text);
    text << "Alternate Titles IDs             : ";

    for(int v = 0; v < 0x10; v++) {
        if(v != 0) {
            text << "                                   ";
        }
        text << "0x" << std::setw(8) << Xbe_certificate->dwAlternateTitleId[v];
        if(v != 0x0F) {
            text << "\n";
        }
    }
    text << "\n";
    return text.str();
}

std::string XbePrinter::GenMediaInfo()
{
    std::stringstream text;
    SSTREAM_SET_HEX(text);
    text << "Allowed Media                    : 0x" << std::setw(8) << Xbe_certificate->dwAllowedMedia << " (" << AllowedMediaToString() << ")\n";
    text << "Game Region                      : 0x" << std::setw(8) << Xbe_certificate->dwGameRegion << " (" << Xbe_to_print->GameRegionToString() << ")\n";
    text << "Game Ratings                     : 0x" << std::setw(8) << Xbe_certificate->dwGameRatings << " (" << GameRatingToString() << ")\n";
    text << "Disk Number                      : 0x" << std::setw(8) << Xbe_certificate->dwDiskNumber << "\n";
    text << "Version                          : 1." << std::dec << std::setw(2) << Xbe_certificate->dwVersion << "\n";
    return text.str();
}

std::string XbePrinter::GenLANKey()
{
    const uint8_t row = 0;
    const uint8_t row_size = 16;
    std::string text;
    text.append("LAN Key                          : ");
    text.append(GenHexRow(&(Xbe_certificate->bzLanKey[0]), row, row_size));
    text.append("\n");
    return text;
}

std::string XbePrinter::GenSignatureKey()
{
    const uint8_t row = 0;
    const uint8_t row_size = 16;
    std::string text;
    text.append("Signature Key                    : ");
    text.append(GenHexRow(&(Xbe_certificate->bzSignatureKey[0]), row, row_size));
    text.append("\n");
    return text;
}

std::string XbePrinter::GenAlternateSignatureKeys()
{
    const uint8_t row = 0;
    const uint8_t row_size = 16;
    std::string text;

    text.append("Title Alternate Signature Keys : <Hex Dump>");
    for(int row = 0; row < 16; row++)
    {
        text.append("\n                                   ");
        text.append(GenHexRow(&(Xbe_certificate->bzTitleAlternateSignatureKey[0][0]), row, row_size));
    }
    text.append("\n                                   </Hex Dump>\n");
    return text;
}

std::string XbePrinter::GenExtraInfo()
{
    std::stringstream text;
    
    if (Xbe_certificate->dwSize >= 0x1EC)
    {
        SSTREAM_SET_HEX(text);
        text << "Original Certificate Size        : 0x" << std::setw(8) << Xbe_certificate->dwOriginalCertificateSize << "\n";
        text << "Online Service ID                : 0x" << std::setw(8) << Xbe_certificate->dwOnlineService << "\n";
        text << "Extra Security Flags             : 0x" << std::setw(8) << Xbe_certificate->dwSecurityFlags << "\n";
        text << "Code Encryption Key              : ";
        text << GenHexRow(&(Xbe_certificate->bzCodeEncKey[0]), 0, 16) << "\n";
    }
    return text.str();
}

std::string XbePrinter::GenSectionInfo()
{
    std::string text;
    text.append("\nDumping XBE Section Headers...\n\n");
    text.append(GenSectionHeaders());
    return text;
}

std::string XbePrinter::GenSectionHeaders()
{
    std::stringstream text;
    SSTREAM_SET_HEX(text);
    for(uint32_t v=0; v < Xbe_header->dwSections; v++) {
        text << "Section Name                     : 0x" << std::setw(8) << Xbe_to_print->m_SectionHeader[v].dwSectionNameAddr << " (\"" << Xbe_to_print->m_szSectionName[v] << "\")\n";
        text << GenSectionFlags(Xbe_to_print->m_SectionHeader[v]);
        text << "Virtual Address                  : 0x" << std::setw(8) << Xbe_to_print->m_SectionHeader[v].dwVirtualAddr << "\n";
        text << "Virtual Size                     : 0x" << std::setw(8) << Xbe_to_print->m_SectionHeader[v].dwVirtualSize << "\n";
        text << "Raw Address                      : 0x" << std::setw(8) << Xbe_to_print->m_SectionHeader[v].dwRawAddr << "\n";
        text << "Size of Raw                      : 0x" << std::setw(8) << Xbe_to_print->m_SectionHeader[v].dwSizeofRaw << "\n";
        text << "Section Name Address             : 0x" << std::setw(8) << Xbe_to_print->m_SectionHeader[v].dwSectionNameAddr << "\n";
        text << "Section Reference Count          : 0x" << std::setw(8) << Xbe_to_print->m_SectionHeader[v].dwSectionRefCount << "\n";
        text << "Head Shared Reference Count Addr : 0x" << std::setw(8) << Xbe_to_print->m_SectionHeader[v].dwHeadSharedRefCountAddr << "\n";
        text << "Tail Shared Reference Count Addr : 0x" << std::setw(8) << Xbe_to_print->m_SectionHeader[v].dwTailSharedRefCountAddr << "\n";
        text << GenSectionDigest(Xbe_to_print->m_SectionHeader[v]) << "\n";
        if (Xbe_to_print->CheckSectionIntegrity(v)) {
            text << "SHA hash check of section " << Xbe_to_print->m_szSectionName[v] << " successful" << "\n\n";
        }
        else {
            text << "SHA hash of section " << Xbe_to_print->m_szSectionName[v] << " doesn't match, section is corrupted" << "\n\n";
        }
    }
    return text.str();
}

std::string XbePrinter::GenSectionFlags(Xbe::SectionHeader section_header)
{
    std::stringstream text;
    SSTREAM_SET_HEX(text);
    text << "Flags                            : 0x" << std::setw(8) << section_header.dwFlags_value << " ";

    if(section_header.dwFlags.bWritable) {
       text << "(Writable) ";
    }
    if(section_header.dwFlags.bPreload) {
       text << "(Preload) ";
    }
    if(section_header.dwFlags.bExecutable) {
       text << "(Executable) ";
    }
    if(section_header.dwFlags.bInsertedFile) {
       text << "(Inserted File) ";
    }
    if(section_header.dwFlags.bHeadPageRO) {
       text << "(Head Page RO) ";
    }
    if(section_header.dwFlags.bTailPageRO) {
        text << "(Tail Page RO) ";
    }
    text << "\n";
    return text.str();
}

std::string XbePrinter::GenSectionDigest(Xbe::SectionHeader section_header)
{
    std::string text;
    text.append("Section Digest                   : ");
    text.append(GenHexRow(&section_header.bzSectionDigest[0], 0, 20));
    return text;
}

std::string XbePrinter::GenLibraryVersions()
{
    std::stringstream text;
    text << "Dumping XBE Library Versions...\n\n";

    if(Xbe_to_print->m_LibraryVersion == 0 || Xbe_header->dwLibraryVersions == 0) {
        text << "(This XBE contains no Library Versions)\n\n";
    }
    else {
        for(uint32_t v = 0; v < Xbe_header->dwLibraryVersions; v++) {
            char libname[9];
            for(uint32_t c=0;c<8;c++) {
                libname[c] = Xbe_to_print->m_LibraryVersion[v].szName[c];
            }
            libname[8] = '\0';

            text << "Library Name                     : " << libname << "\n";
            text << "Version                          : " 
                 << Xbe_to_print->m_LibraryVersion[v].wMajorVersion << "." 
                 << Xbe_to_print->m_LibraryVersion[v].wMinorVersion << "." 
                 << Xbe_to_print->m_LibraryVersion[v].wBuildVersion << "\n";
            text << GenLibraryFlags(Xbe_to_print->m_LibraryVersion[v]);
            text << "\n";
        }
    }
    return text.str();
}

std::string XbePrinter::GenLibraryFlags(Xbe::LibraryVersion libver)
{
    std::stringstream text;
    SSTREAM_SET_HEX(text);
    text << "Flags                            : 0x" << std::setw(4) << libver.wFlags_value << " ";
    text << "QFEVersion : 0x" << std::setw(4) << libver.wFlags.QFEVersion << ", ";

    if(libver.wFlags.bDebugBuild) {
        text << "Debug, ";
    }
    else {
        text << "Retail, ";
    }

    switch(libver.wFlags.Approved) {
        case 0:
            text << "Unapproved";
            break;
        case 1:
            text << "Possibly Approved";
            break;
        case 2:
            text << "Approved";
            break;
    }
    text << "\n";
    return text.str();
}

std::string XbePrinter::GenTLS()
{
    std::stringstream text;
    SSTREAM_SET_HEX(text);
    Xbe::TLS *local_TLS = Xbe_to_print->m_TLS;

    text << "Dumping XBE TLS...\n\n";
    // print thread local storage
    if(local_TLS != 0) {
        text << "Data Start Address               : 0x" << std::setw(8) << local_TLS->dwDataStartAddr << "\n";
        text << "Data End Address                 : 0x" << std::setw(8) << local_TLS->dwDataEndAddr << "\n";
        text << "TLS Index Address                : 0x" << std::setw(8) << local_TLS->dwTLSIndexAddr << "\n";
        text << "TLS Callback Address             : 0x" << std::setw(8) << local_TLS->dwTLSCallbackAddr << "\n";
        text << "Size of Zero Fill                : 0x" << std::setw(8) << local_TLS->dwSizeofZeroFill << "\n";
        text << "Characteristics                  : 0x" << std::setw(8) << local_TLS->dwCharacteristics << "\n";
    }
    else {
        text << "(This XBE contains no TLS)\n";
    }
    return text.str();
}
