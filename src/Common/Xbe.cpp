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
// *   Cxbx->Common->Xbe.cpp
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
#include "Xbe.h"
#include "CxbxVersion.h"
#include "CxbxUtil.h"

#include <cstdlib>
#include <locale>
#include <codecvt>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

#define PAGE_SIZE 0x1000

// construct via Xbe file
Xbe::Xbe(const char *x_szFilename)
{
    char szBuffer[MAX_PATH];

    ConstructorInit();

    printf("Xbe::Xbe: Opening Xbe file...");

    FILE *XbeFile = fopen(x_szFilename, "rb");

    // verify Xbe file was opened successfully
    if(XbeFile == 0)
    {
        SetFatalError("Could not open Xbe file.");
        return;
    }

    printf("OK\n");

    // remember the Xbe path
    {
        printf("Xbe::Xbe: Storing Xbe Path...");

        strcpy(m_szPath, x_szFilename);

		char * c = strrchr(m_szPath, '\\');
		if (c != NULL)
			*(++c) = '\0';
    }

    printf("OK\n");

    // read Xbe image header
    {
        printf("Xbe::Xbe: Reading Image Header...");

        if(fread(&m_Header, sizeof(m_Header), 1, XbeFile) != 1)
        {
            SetFatalError("Unexpected end of file while reading Xbe Image Header");
            goto cleanup;
        }

        if(m_Header.dwMagic != *(uint32 *)"XBEH")
        {
            SetFatalError("Invalid magic number in Xbe file");
            goto cleanup;
        }

        printf("OK\n");
    }

    // read Xbe image header extra bytes
    if(m_Header.dwSizeofHeaders > sizeof(m_Header))
    {
        printf("Xbe::Xbe: Reading Image Header Extra Bytes...");

        m_ExSize = RoundUp(m_Header.dwSizeofHeaders, PAGE_SIZE) - sizeof(m_Header);

        m_HeaderEx = new char[m_ExSize];

        if(fread(m_HeaderEx, m_ExSize, 1, XbeFile) != 1)
        {
            SetFatalError("Unexpected end of file while reading Xbe Image Header (Ex)");
            goto cleanup;
        }

        printf("OK\n");
    }

    // read Xbe certificate
    {
        printf("Xbe::Xbe: Reading Certificate...");

        fseek(XbeFile, m_Header.dwCertificateAddr - m_Header.dwBaseAddr, SEEK_SET);

        if(fread(&m_Certificate, sizeof(m_Certificate), 1, XbeFile) != 1)
        {
            SetFatalError("Unexpected end of file while reading Xbe Certificate");
            goto cleanup;
        }

        setlocale( LC_ALL, "English" );

        wcstombs(m_szAsciiTitle, m_Certificate.wszTitleName, 40);

        printf("OK\n");

        printf("Xbe::Xbe: Title identified as %s\n", m_szAsciiTitle);

		// Detect empty title :
		int len = strlen(m_szAsciiTitle);
		while (len > 0 && m_szAsciiTitle[len-1] <= ' ')
			len--;
		if (len <= 0) {
			// Try to fix empty title; first, try the executable name:
			char Dir[_MAX_DIR];
			char Filename[_MAX_FNAME];
			_splitpath(x_szFilename, nullptr, Dir, Filename, nullptr);
			if (_stricmp(Filename, "default") != 0) {
				strcpy(m_szAsciiTitle, Filename);
			}
			else {
				// If executable is named "default.xbe", try the parent folder name:
				len = strlen(Dir);
				if (len > 0) {
					Dir[len - 1] = '\0';
					_splitpath(Dir, nullptr, nullptr, m_szAsciiTitle, nullptr);
				}
			}

			printf("Xbe::Xbe: Replaced empty title with fallback : %s\n", m_szAsciiTitle);
		}
    }

    // read Xbe section headers
    {
        printf("Xbe::Xbe: Reading Section Headers...\n");

        fseek(XbeFile, m_Header.dwSectionHeadersAddr - m_Header.dwBaseAddr, SEEK_SET);

        m_SectionHeader = new SectionHeader[m_Header.dwSections];

        for(uint32 v=0;v<m_Header.dwSections;v++)
        {
            printf("Xbe::Xbe: Reading Section Header 0x%.04X...", v);

            if(fread(&m_SectionHeader[v], sizeof(*m_SectionHeader), 1, XbeFile) != 1)
            {
                sprintf(szBuffer, "Unexpected end of file while reading Xbe Section Header %d (%Xh)", v, v);
                SetFatalError(szBuffer);
                goto cleanup;
            }

            printf("OK\n");
        }
    }

    // read Xbe section names
    {
        printf("Xbe::Xbe: Reading Section Names...\n");

        m_szSectionName = new char[m_Header.dwSections][10];
        for(uint32 v=0;v<m_Header.dwSections;v++)
        {
            printf("Xbe::Xbe: Reading Section Name 0x%.04X...", v);

            uint08 *sn = GetAddr(m_SectionHeader[v].dwSectionNameAddr);

            memset(m_szSectionName[v], 0, 10);

            if(sn != 0)
            {
                for(int b=0;b<9;b++)
                {
                    m_szSectionName[v][b] = sn[b];

                    if(m_szSectionName[v][b] == '\0')
                        break;
                }
            }

            printf("OK (%s)\n", m_szSectionName[v]);
        }
    }

    // read Xbe library versions
    if(m_Header.dwLibraryVersionsAddr != 0)
    {
        printf("Xbe::Xbe: Reading Library Versions...\n");

        fseek(XbeFile, m_Header.dwLibraryVersionsAddr - m_Header.dwBaseAddr, SEEK_SET);

        m_LibraryVersion = new LibraryVersion[m_Header.dwLibraryVersions];

        for(uint32 v=0;v<m_Header.dwLibraryVersions;v++)
        {
            printf("Xbe::Xbe: Reading Library Version 0x%.04X...", v);

            if(fread(&m_LibraryVersion[v], sizeof(*m_LibraryVersion), 1, XbeFile) != 1)
            {
                sprintf(szBuffer, "Unexpected end of file while reading Xbe Library Version %d (%Xh)", v, v);
                SetFatalError(szBuffer);
                goto cleanup;
            }

            printf("OK\n");
        }

        // read Xbe kernel library version
        {
            printf("Xbe::Xbe: Reading Kernel Library Version...");

            if(m_Header.dwKernelLibraryVersionAddr == 0)
            {
                SetFatalError("Could not locate kernel library version");
                goto cleanup;
            }

            fseek(XbeFile, m_Header.dwKernelLibraryVersionAddr - m_Header.dwBaseAddr, SEEK_SET);

            m_KernelLibraryVersion = new LibraryVersion;

            if(fread(m_KernelLibraryVersion, sizeof(*m_LibraryVersion), 1, XbeFile) != 1)
            {
                SetFatalError("Unexpected end of file while reading Xbe Kernel Version");
                goto cleanup;
            }

            printf("OK\n");
        }

        // read Xbe Xapi library version
        {
            printf("Xbe::Xbe: Reading Xapi Library Version...");

            if(m_Header.dwXAPILibraryVersionAddr == 0)
            {
                SetFatalError("Could not locate Xapi Library Version");
                goto cleanup;
            }

            fseek(XbeFile, m_Header.dwXAPILibraryVersionAddr - m_Header.dwBaseAddr, SEEK_SET);

            m_XAPILibraryVersion = new LibraryVersion;

            if(fread(m_XAPILibraryVersion, sizeof(*m_LibraryVersion), 1, XbeFile) != 1)
            {
                SetFatalError("Unexpected end of file while reading Xbe Xapi Version");
                goto cleanup;
            }

            printf("OK\n");
        }
    }

    // read Xbe sections
    {
        printf("Xbe::Xbe: Reading Sections...\n");

        m_bzSection = new uint08*[m_Header.dwSections];

        memset(m_bzSection, 0, m_Header.dwSections);

        for(uint32 v=0;v<m_Header.dwSections;v++)
        {
            printf("Xbe::Xbe: Reading Section 0x%.04X...", v);

            uint32 RawSize = m_SectionHeader[v].dwSizeofRaw;
            uint32 RawAddr = m_SectionHeader[v].dwRawAddr;

            m_bzSection[v] = new uint08[RawSize];

            fseek(XbeFile, RawAddr, SEEK_SET);

            if(RawSize == 0)
            {
                printf("OK\n");
                continue;
            }

            if(fread(m_bzSection[v], RawSize, 1, XbeFile) != 1)
            {
                sprintf(szBuffer, "Unexpected end of file while reading Xbe Section %d (%Xh) (%s)", v, v, m_szSectionName[v]);
                SetFatalError(szBuffer);
                goto cleanup;
            }

            printf("OK\n");
        }
    }

    // read Xbe thread local storage
    if(m_Header.dwTLSAddr != 0)
    {
        printf("Xbe::Xbe: Reading Thread Local Storage...");

        void *Addr = GetAddr(m_Header.dwTLSAddr);

        if(Addr == 0)
        {
            SetFatalError("Could not locate Thread Local Storage");
            goto cleanup;
        }

        m_TLS = new TLS;

        memcpy(m_TLS, Addr, sizeof(*m_TLS));

        printf("OK\n");
    }

cleanup:

    if (HasError())
    {
        printf("FAILED!\n");
        printf("Xbe::Xbe: ERROR -> %s\n", GetError().c_str());
    }

    fclose(XbeFile);

    return;
}

// deconstructor
Xbe::~Xbe()
{
    if(m_bzSection != 0)
    {
        for(uint32 v=0;v<m_Header.dwSections;v++)
            delete[] m_bzSection[v];

        delete[] m_bzSection;
    }

    delete   m_XAPILibraryVersion;
    delete   m_KernelLibraryVersion;
    delete[] m_LibraryVersion;
    delete   m_TLS;
    delete[] m_szSectionName;
    delete[] m_SectionHeader;
    delete[] m_HeaderEx;
}

// export to Xbe file
void Xbe::Export(const char *x_szXbeFilename)
{
    if (HasError())
        return;

    char szBuffer[MAX_PATH];

    printf("Xbe::Export: Writing Xbe file...");

    FILE *XbeFile = fopen(x_szXbeFilename, "wb");

    // verify Xbe file was opened successfully
    if(XbeFile == 0)
    {
        SetError("Could not open .xbe file.");
        return;
    }

    printf("OK\n");

    // write Xbe image header
    {
        printf("Xbe::Export: Writing Image Header...");

        if(fwrite(&m_Header, sizeof(m_Header), 1, XbeFile) != 1)
        {
            SetError("Unexpected write error while writing Xbe Image Header");
            goto cleanup;
        }

        printf("OK\n");

        printf("Xbe::Export: Writing Image Header Extra Bytes...");

        if(fwrite(m_HeaderEx, m_Header.dwSizeofHeaders, 1, XbeFile) != 1)
        {
            SetError("Unexpected write error while writing Xbe Image Header (Ex)");
            goto cleanup;
        }

        printf("OK\n");
    }

    // write Xbe certificate
    {
        printf("Xbe::Export: Writing Certificate...");

        fseek(XbeFile, m_Header.dwCertificateAddr - m_Header.dwBaseAddr, SEEK_SET);

        if(fwrite(&m_Certificate, sizeof(m_Certificate), 1, XbeFile) != 1)
        {
            SetError("Unexpected write error while writing Xbe Certificate");
            goto cleanup;
        }

        printf("OK\n");
    }

    // write Xbe section headers
    {
        printf("Xbe::Export: Writing Section Headers...\n");

        fseek(XbeFile, m_Header.dwSectionHeadersAddr - m_Header.dwBaseAddr, SEEK_SET);

        for(uint32 v=0;v<m_Header.dwSections;v++)
        {
            printf("Xbe::Export: Writing Section Header 0x%.04X...", v);

            if(fwrite(&m_SectionHeader[v], sizeof(*m_SectionHeader), 1, XbeFile) != 1)
            {
                sprintf(szBuffer, "Unexpected write error while writing Xbe Section %d (%Xh)", v, v);
                SetError(szBuffer);
                goto cleanup;
            }

            printf("OK\n");
        }
    }

    // write Xbe sections
    {
        printf("Xbe::Export: Writing Sections...\n");

        for(uint32 v=0;v<m_Header.dwSections;v++)
        {
            printf("Xbe::Export: Writing Section 0x%.04X (%s)...", v, m_szSectionName[v]);

            uint32 RawSize = m_SectionHeader[v].dwSizeofRaw;
            uint32 RawAddr = m_SectionHeader[v].dwRawAddr;

            fseek(XbeFile, RawAddr, SEEK_SET);

            if(RawSize == 0)
            {
                printf("OK\n");
                continue;
            }

            if(fwrite(m_bzSection[v], RawSize, 1, XbeFile) != 1)
            {
                sprintf(szBuffer, "Unexpected write error while writing Xbe Section %d (%Xh) (%s)", v, v, m_szSectionName[v]);
                SetError(szBuffer);
                goto cleanup;
            }

            printf("OK\n");
        }
    }

    // zero pad
    {
        printf("Xbe::Export: Writing Zero Padding...");

        fpos_t pos;

        uint32 remaining = 0;

        fgetpos(XbeFile, &pos);

        remaining = (uint32)(PAGE_SIZE - ftell(XbeFile) % PAGE_SIZE);

        // write remaining bytes
        {
            char *szBuffer = new char[remaining];

            for(uint32 v=0;v<remaining;v++)
                szBuffer[v] = 0;

            fwrite(szBuffer, remaining, 1, XbeFile);

            delete[] szBuffer;
        }

        printf("OK\n");
    }

cleanup:

    // if we came across an error, delete the file we were creating
    if (HasError())
    {
        remove(x_szXbeFilename);
        printf("FAILED!\n");
        printf("Xbe::Export: ERROR -> %s\n", GetError().c_str());
    }

    fclose(XbeFile);
}

// constructor initialization
void Xbe::ConstructorInit()
{
    m_HeaderEx             = 0;
    m_SectionHeader        = 0;
    m_szSectionName        = 0;
    m_LibraryVersion       = 0;
    m_KernelLibraryVersion = 0;
    m_XAPILibraryVersion   = 0;
    m_TLS                  = 0;
    m_bzSection            = 0;
}

// better time
static char *BetterTime(uint32 x_timeDate)
{
    time_t x_time = x_timeDate;
    char *x_ctime = ctime(&x_time);

    int v=0;

    for(v=0;x_ctime[v] != '\n';v++);

    x_ctime[v] = '\0';

    return x_ctime;
}

bool Xbe::DumpInformationToFile(std::string out_filename)
{
    if(HasError()) {
        return false;
    }

    bool success = false;
    XbePrinter printer(this);
    std::ofstream out_file(out_filename);
    if(out_file.is_open()) {
        out_file << printer.GenXbeInfo();
        out_file.close();
        success = true;
    }
    return success;
}

void Xbe::DumpInformationToConsole()
{
    if(HasError())
        return;

    XbePrinter printer(this);
    std::cout << printer.GenXbeInfo();
}

// import logo bitmap from raw monochrome data
void Xbe::ImportLogoBitmap(const uint08 x_Gray[100*17])
{
    char  *LogoBuffer = new char[4*1024];
    uint32 LogoSize = 0;

    // encode logo bitmap
    {
        for(uint32 v=1;v<100*17;LogoSize++)
        {
            char color = x_Gray[v] >> 4;

            uint32 len = 1;

            while(++v<100*17-1 && len < 1024 && color == x_Gray[v] >> 4)
                len++;

            LogoRLE *cur = (LogoRLE *)&LogoBuffer[LogoSize];

            if(len <= 7)
            {
                cur->m_Eight.bType1 = 1;
                cur->m_Eight.Len    = len;
                cur->m_Eight.Data   = color;
            }
            else
            {
                cur->m_Sixteen.bType1 = 0;
                cur->m_Sixteen.bType2 = 1;
                cur->m_Sixteen.Len    = len;
                cur->m_Sixteen.Data   = color;
                LogoSize++;
            }
        }
    }

    // check if there is room to save this, if not then throw an error
    {
        uint08 *RLE = GetLogoBitmap(LogoSize);

        if(RLE == 0)
        {
            if (false == HasError())
                SetError("Logo bitmap could not be imported (not enough space in file?)");

            return;
        }

        memcpy(RLE, LogoBuffer, LogoSize);
    }

    return;
}

// ******************************************************************
// * ExportLogoBitmap
// ******************************************************************
// *
// * This algorithm was originally discovered by superfro. I couldnt
// * figure out what the hell the encoding format was before he gave
// * me the information:
// *
// * basically what is going on here is a single pass through the
// * bitmap data, with 2 possible encodings per rle chunk. data is
// * stored as 4 bit grayscale, so the logical approach is to expand
// * this to 8 bit using a simple 4 bit left shift (*16). However, it
// * has been suggested to me by superfro that you might calculate a
// * slightly darker image by multiplying by 15 and adding .5. It's
// * a toss up, but i've choosen a simple bit shift left.
// *
// ******************************************************************
void Xbe::ExportLogoBitmap(uint08 x_Gray[100*17])
{
    memset(x_Gray, 0, 100*17);

    uint32 dwLength = m_Header.dwSizeofLogoBitmap;

    uint08 *RLE = GetAddr(m_Header.dwLogoBitmapAddr);

    if(RLE == nullptr || HasError())
        return;

    uint32 o = 0;

    for(uint32 v=0;v<dwLength;v++)
    {
        uint32 len = 0, data = 0;

        LogoRLE *cur = (LogoRLE *)&RLE[v];

        if(cur->m_Eight.bType1)
        {
            len  = cur->m_Eight.Len;
            data = cur->m_Eight.Data;
        }
        else
        {
            if(cur->m_Sixteen.bType2)
            {
                len  = cur->m_Sixteen.Len;
                data = cur->m_Sixteen.Data;
                v   += 1;
            }
        }

        for(uint32 j=0;j<len;j++)
        {
            o++;

            if(o < 100*17)
                x_Gray[o] = (char)(data << 4); // could use (int)(data * 15.0 + .5);
        }
    }
}

// return a modifiable pointer inside this structure that corresponds to a virtual address
uint08 *Xbe::GetAddr(uint32 x_dwVirtualAddress)
{
    uint32 dwOffs = x_dwVirtualAddress - m_Header.dwBaseAddr;

    // offset into image header
    if(dwOffs < sizeof(m_Header))
        return &((uint08*)&m_Header)[dwOffs];

    // offset into image header extra bytes
    if(dwOffs < m_Header.dwSizeofHeaders)
        return (uint08*)&m_HeaderEx[dwOffs - sizeof(m_Header)];

    // offset into some random section
    {
        for(uint32 v=0;v<m_Header.dwSections;v++)
        {
            uint32 VirtAddr = m_SectionHeader[v].dwVirtualAddr;
            uint32 VirtSize = m_SectionHeader[v].dwVirtualSize;

            if( (x_dwVirtualAddress >= VirtAddr) && (x_dwVirtualAddress < (VirtAddr + VirtSize)) )
                return &m_bzSection[v][x_dwVirtualAddress - VirtAddr];
        }
    }

    return 0;
}

// return a modifiable pointer to logo bitmap data
uint08 *Xbe::GetLogoBitmap(uint32 x_dwSize)
{
    uint32 dwOffs = m_Header.dwLogoBitmapAddr - m_Header.dwBaseAddr;
    uint32 dwLength = m_Header.dwSizeofLogoBitmap;

    if(dwOffs == 0 || dwLength == 0)
        return 0;

    // if this bitmap will fit inside the already existing one, we don't need to resize, just return pointer
    if(dwLength >= x_dwSize)
    {
        // update size of headers, if necessary
        if(dwOffs < m_Header.dwSizeofHeaders)
        {
            m_Header.dwSizeofHeaders -= dwLength;
            m_Header.dwSizeofHeaders += x_dwSize;
            m_Header.dwSizeofLogoBitmap = x_dwSize;

            return GetAddr(m_Header.dwLogoBitmapAddr);
        }
    }

    return 0;
}


void *Xbe::FindSection(char *zsSectionName)
{
	for (uint32 v = 0; v < m_Header.dwSections; v++) {
		if (strcmp(m_szSectionName[v], zsSectionName) == 0) {
			if (m_SectionHeader[v].dwVirtualAddr > 0 && m_SectionHeader[v].dwVirtualSize > 0) {
				return m_bzSection[v];
			}
		}
	}

	return NULL;
}

void Xbe::PurgeBadChar(std::string& s, const std::string& illegalChars)
{
	for (auto it = s.begin(); it < s.end(); ++it)
	{
		bool found = illegalChars.find(*it) != std::string::npos;
		if (found) { *it = '_'; }
	}
}

const char *Xbe::GameRegionToString()
{
    const char *Region_text[] = {
        "Unknown", "NTSC", "JAP", "NTSC+JAP",
        "PAL", "PAL+NTSC", "PAL+JAP", "Region Free",
        "DEBUG", "NTSC (DEBUG)", "JAP (DEBUG)", "NTSC+JAP (DEBUG)",
        "PAL (DEBUG)", "PAL+NTSC (DEBUG)", "PAL+JAP (DEBUG)", "Region Free (DEBUG)"
    };
    const uint32 all_regions = XBEIMAGE_GAME_REGION_NA |
                               XBEIMAGE_GAME_REGION_JAPAN |
                               XBEIMAGE_GAME_REGION_RESTOFWORLD |
                               XBEIMAGE_GAME_REGION_MANUFACTURING;

    if(m_Certificate.dwGameRegion & ~all_regions) {
        return "REGION ERROR";
    }

    uint8 index = (m_Certificate.dwGameRegion & XBEIMAGE_GAME_REGION_MANUFACTURING) ? 0x8 : 0;
    index |= (m_Certificate.dwGameRegion & 0x7);
    return Region_text[index];
}

const wchar_t *Xbe::GetUnicodeFilenameAddr()
{
    return (const wchar_t *)GetAddr(m_Header.dwDebugUnicodeFilenameAddr);
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
    info.append(GenSectionInfo());
    info.append(GenLibraryVersions());
    info.append(GenTLS());
    return info;
}

std::string XbePrinter::GenHexRow(
    uint08 *signature, const uint08 row, const uint08 row_size
)
{
    const uint16 offset = row * row_size;
    std::stringstream text;
    SSTREAM_SET_HEX(text);
    for(uint08 x = 0; x < row_size; x++) {
        text << std::setw(2) << static_cast<unsigned>(signature[offset + x]);
    }
    return text.str();
}

// https://stackoverflow.com/questions/4786292/converting-unicode-strings-and-vice-versa
std::string XbePrinter::utf8_to_ascii(const wchar_t* utf8_string)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_to_ascii;
    const std::wstring utf8_filename(utf8_string);
    return utf8_to_ascii.to_bytes(utf8_filename);
}

std::string XbePrinter::AllowedMediaToString()
{
    const uint32 dwAllowedMedia = Xbe_certificate->dwAllowedMedia;
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
    // Info from: http://xboxdevwiki.net/EEPROM
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
    text.append("XBE information generated by Cxbx-Reloaded (Version " _CXBX_VERSION ")\n\n");
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
    const uint08 row_size = 16;
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
    const uint32 retail_entry_point = Xbe_header->dwEntryAddr ^ XOR_EP_RETAIL;
    const uint32 debug_entry_point = Xbe_header->dwEntryAddr ^ XOR_EP_DEBUG;
    const uint32 retail_thunk_addr = Xbe_header->dwKernelImageThunkAddr ^ XOR_KT_RETAIL;
    const uint32 debug_thunk_addr = Xbe_header->dwKernelImageThunkAddr ^ XOR_KT_DEBUG;
    const std::string AsciiFilename = utf8_to_ascii(Xbe_to_print->GetUnicodeFilenameAddr());
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

std::string XbePrinter::GenXbeCertificateInfo()
{
    std::string text;
    text.append(GenCertificateHeader());
    text.append(GenAlternateTitleIDs());
    text.append(GenMediaInfo());
    text.append(GenLANKey());
    text.append(GenSignatureKey());
    text.append(GenAlternateSignatureKeys());
    return text;
}

std::string XbePrinter::GenCertificateHeader()
{
    std::stringstream text;
    SSTREAM_SET_HEX(text);
    text << "Dumping XBE Certificate...\n\n";
    text << "Size of Certificate              : 0x" << std::setw(8) << Xbe_certificate->dwSize << "\n";
    text << "TimeDate Stamp                   : 0x" << std::setw(8) << Xbe_certificate->dwTimeDate << " (" << BetterTime(Xbe_certificate->dwTimeDate) << ")\n";
    text << "Title ID                         : 0x" << std::setw(8) << Xbe_certificate->dwTitleId << "\n";
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
    text << "Version                          : 0x" << std::setw(8) << Xbe_certificate->dwVersion << "\n";
    return text.str();
}

std::string XbePrinter::GenLANKey()
{
    const uint08 row = 0;
    const uint08 row_size = 16;
    std::string text;
    text.append("LAN Key                          : ");
    text.append(GenHexRow(&(Xbe_certificate->bzLanKey[0]), row, row_size));
    text.append("\n");
    return text;
}

std::string XbePrinter::GenSignatureKey()
{
    const uint08 row = 0;
    const uint08 row_size = 16;
    std::string text;
    text.append("Signature Key                    : ");
    text.append(GenHexRow(&(Xbe_certificate->bzSignatureKey[0]), row, row_size));
    text.append("\n");
    return text;
}

std::string XbePrinter::GenAlternateSignatureKeys()
{
    const uint08 row = 0;
    const uint08 row_size = 16;
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
    for(uint32 v=0; v < Xbe_header->dwSections; v++) {
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
    text.append("\n");
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
        for(uint32 v = 0; v < Xbe_header->dwLibraryVersions; v++) {
            char libname[9];
            for(uint32 c=0;c<8;c++) {
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
