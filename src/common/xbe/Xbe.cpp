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
// *  (c) 2017 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************

// prevent name collisions
namespace xboxkrnl
{
	#include <xboxkrnl/xboxkrnl.h>
};

#include "common\xbe\Xbe.h"
#include "common\util\CxbxUtil.h" // For RoundUp
#include <filesystem> // filesystem related functions available on C++ 17
#include <locale> // For ctime
#include <array>
#include "devices\LED.h" // For LED::Sequence
#include "core\kernel\init\CxbxKrnl.h" // For CxbxKrnlPrintUEM
#include "common\crypto\EmuSha.h" // For the SHA functions
#include "common\crypto\EmuRsa.h" // For the RSA functions
#include "core\hle\XAPI\Xapi.h" // For LDT_FROM_DASHBOARD
#include "core\hle\D3D8\Direct3D9\Direct3D9.h" // For CxbxInitWindow
#include "common/AddressRanges.h"
#include "common/xbox/Types.hpp"

namespace fs = std::filesystem;



// construct via Xbe file
Xbe::Xbe(const char *x_szFilename, bool bFromGUI)
{
    char szBuffer[MAX_PATH];

    ConstructorInit();

    printf("Xbe::Xbe: Opening Xbe file...");

    FILE *XbeFile = fopen(x_szFilename, "rb");

    // verify Xbe file was opened successfully
    if(XbeFile == 0)
    {
		using namespace fs; // limit its scope inside here

		std::string XbeName = path(x_szFilename).filename().string(); // recover the xbe name

		// NOTE: the check for the existence of the child window is necessary because the user could have previously loaded the dashboard,
		// removed/changed the path and attempt to load it again from the recent list, which will crash CxbxInitWindow below
		// Note that GetHwnd(), CxbxKrnl_hEmuParent and HalReturnToFirmware are all not suitable here for various reasons 
		if (XbeName.compare(std::string("xboxdash.xbe")) == 0 && !bFromGUI)
		{
			// The dashboard could not be found on partition2. This is a fatal error on the Xbox so we display the UEM. The
			// error code is different if we have a launch data page

			CxbxInitWindow(false);

			ULONG FatalErrorCode = FATAL_ERROR_XBE_DASH_GENERIC;

			if (xboxkrnl::LaunchDataPage && xboxkrnl::LaunchDataPage->Header.dwLaunchDataType == LDT_FROM_DASHBOARD)
			{
				xboxkrnl::PDASH_LAUNCH_DATA pLaunchDashboard = (xboxkrnl::PDASH_LAUNCH_DATA)&(xboxkrnl::LaunchDataPage->LaunchData[0]);
				FatalErrorCode += pLaunchDashboard->dwReason;
			}
			SetLEDSequence(0xE1); // green, red, red, red
			CxbxKrnlPrintUEM(FatalErrorCode); // won't return

			// TODO: FATAL_ERROR_XBE_DASH_X2_PASS (requires DVD drive authentication emulation...)
		}
		else
		{
			// Report which xbe could not be found
			SetFatalError(std::string("Could not open the Xbe file ") + XbeName);
			return;
		}
    }

    printf("OK\n");

    // remember the Xbe path
    {
        printf("Xbe::Xbe: Storing Xbe Path...");

        strcpy(m_szPath, x_szFilename);

		char * c = strrchr(m_szPath, '\\');
		if (c != nullptr)
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

        if(m_Header.dwMagic != *(uint32_t *)"XBEH")
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

        for(uint32_t v=0;v<m_Header.dwSections;v++)
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
        for(uint32_t v=0;v<m_Header.dwSections;v++)
        {
            printf("Xbe::Xbe: Reading Section Name 0x%.04X...", v);

            uint8_t *sn = GetAddr(m_SectionHeader[v].dwSectionNameAddr);

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
	if (m_Header.dwLibraryVersionsAddr != 0)
	{
		printf("Xbe::Xbe: Reading Library Versions...\n");

		fseek(XbeFile, m_Header.dwLibraryVersionsAddr - m_Header.dwBaseAddr, SEEK_SET);

		m_LibraryVersion = new LibraryVersion[m_Header.dwLibraryVersions];

		for (uint32_t v = 0; v < m_Header.dwLibraryVersions; v++)
		{
			printf("Xbe::Xbe: Reading Library Version 0x%.04X...", v);

			if (fread(&m_LibraryVersion[v], sizeof(*m_LibraryVersion), 1, XbeFile) != 1)
			{
				sprintf(szBuffer, "Unexpected end of file while reading Xbe Library Version %d (%Xh)", v, v);
				SetFatalError(szBuffer);
				goto cleanup;
			}

			printf("OK\n");
		}
	}

    // read Xbe sections
    {
        printf("Xbe::Xbe: Reading Sections...\n");

        m_bzSection = new uint8_t*[m_Header.dwSections];

        memset(m_bzSection, 0, m_Header.dwSections);

        for(uint32_t v=0;v<m_Header.dwSections;v++)
        {
            printf("Xbe::Xbe: Reading Section 0x%.04X...", v);

            uint32_t RawSize = m_SectionHeader[v].dwSizeofRaw;
            uint32_t RawAddr = m_SectionHeader[v].dwRawAddr;

            m_bzSection[v] = new uint8_t[RawSize];

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

	// read the header to calculate the rsa/sha1 signature against
	{
		printf("Xbe::Xbe: Reading Signature Header...");

		fseek(XbeFile, sizeof(m_Header.dwMagic) + sizeof(m_Header.pbDigitalSignature), SEEK_SET);
		m_SignatureHeader = new uint8_t[m_Header.dwSizeofHeaders - (sizeof(m_Header.dwMagic) + sizeof(m_Header.pbDigitalSignature))];
		fread(m_SignatureHeader, m_Header.dwSizeofHeaders - (sizeof(m_Header.dwMagic) + sizeof(m_Header.pbDigitalSignature)), 1, XbeFile);

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
        for(uint32_t v=0;v<m_Header.dwSections;v++)
            delete[] m_bzSection[v];

        delete[] m_bzSection;
    }

    delete[] m_LibraryVersion;
    delete   m_TLS;
    delete[] m_szSectionName;
    delete[] m_SectionHeader;
    delete[] m_HeaderEx;
	delete[] m_SignatureHeader;
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

        for(uint32_t v=0;v<m_Header.dwSections;v++)
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

        for(uint32_t v=0;v<m_Header.dwSections;v++)
        {
            printf("Xbe::Export: Writing Section 0x%.04X (%s)...", v, m_szSectionName[v]);

            uint32_t RawSize = m_SectionHeader[v].dwSizeofRaw;
            uint32_t RawAddr = m_SectionHeader[v].dwRawAddr;

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

        uint32_t remaining = 0;

        fgetpos(XbeFile, &pos);

        remaining = (uint32_t)(PAGE_SIZE - ftell(XbeFile) % PAGE_SIZE);

        // write remaining bytes
        {
            char *szBuffer = new char[remaining];

            for(uint32_t v=0;v<remaining;v++)
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
    m_TLS                  = 0;
    m_bzSection            = 0;
	m_SignatureHeader      = 0;
}

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

// import logo bitmap from raw monochrome data
void Xbe::ImportLogoBitmap(const uint8_t x_Gray[100*17])
{
    char  *LogoBuffer = new char[4*1024];
    uint32_t LogoSize = 0;

    // encode logo bitmap
    {
        for(uint32_t v=1;v<100*17;LogoSize++)
        {
            char color = x_Gray[v] >> 4;

            uint32_t len = 1;

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
        uint8_t *RLE = GetLogoBitmap(LogoSize);

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
void Xbe::ExportLogoBitmap(uint8_t x_Gray[100*17])
{
    memset(x_Gray, 0, 100*17);

    uint32_t dwLength = m_Header.dwSizeofLogoBitmap;

    uint8_t *RLE = GetAddr(m_Header.dwLogoBitmapAddr);

    if(RLE == nullptr || HasError())
        return;

    uint32_t o = 0;

    for(uint32_t v=0;v<dwLength;v++)
    {
        uint32_t len = 0, data = 0;

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

        for(uint32_t j=0;j<len;j++)
        {
            o++;

            if(o < 100*17)
                x_Gray[o] = (char)(data << 4); // could use (int)(data * 15.0 + .5);
        }
    }
}

// return a modifiable pointer inside this structure that corresponds to a virtual address
uint8_t *Xbe::GetAddr(uint32_t x_dwVirtualAddress)
{
    uint32_t dwOffs = x_dwVirtualAddress - m_Header.dwBaseAddr;

    // offset into image header
    if(dwOffs < sizeof(m_Header))
        return &((uint8_t*)&m_Header)[dwOffs];

    // offset into image header extra bytes
    if(dwOffs < m_Header.dwSizeofHeaders)
        return (uint8_t*)&m_HeaderEx[dwOffs - sizeof(m_Header)];

    // offset into some random section
    {
        for(uint32_t v=0;v<m_Header.dwSections;v++)
        {
            uint32_t VirtAddr = m_SectionHeader[v].dwVirtualAddr;
            uint32_t VirtSize = m_SectionHeader[v].dwVirtualSize;

            if( (x_dwVirtualAddress >= VirtAddr) && (x_dwVirtualAddress < (VirtAddr + VirtSize)) )
                return &m_bzSection[v][x_dwVirtualAddress - VirtAddr];
        }
    }

    return 0;
}

// return a modifiable pointer to logo bitmap data
uint8_t *Xbe::GetLogoBitmap(uint32_t x_dwSize)
{
    uint32_t dwOffs = m_Header.dwLogoBitmapAddr - m_Header.dwBaseAddr;
    uint32_t dwLength = m_Header.dwSizeofLogoBitmap;

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
	for (uint32_t v = 0; v < m_Header.dwSections; v++) {
		if (strcmp(m_szSectionName[v], zsSectionName) == 0) {
			if (m_SectionHeader[v].dwVirtualAddr > 0 && m_SectionHeader[v].dwVirtualSize > 0) {
				return m_bzSection[v];
			}
		}
	}

	return nullptr;
}

void* Xbe::FindSection(xboxkrnl::PXBEIMAGE_SECTION section)
{
	for (uint32_t v = 0; v < m_Header.dwSections; v++) {
		if (m_SectionHeader[v].dwVirtualAddr > 0 && m_SectionHeader[v].dwVirtualSize > 0) {
			if (m_SectionHeader[v].dwRawAddr == section->FileAddress && m_SectionHeader[v].dwVirtualAddr == (uint32_t)(section->VirtualAddress)
				&& m_SectionHeader[v].dwVirtualSize == section->VirtualSize) {
				return m_bzSection[v];
			}
		}

	}

	return nullptr;
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
    const uint32_t all_regions = XBEIMAGE_GAME_REGION_NA |
                               XBEIMAGE_GAME_REGION_JAPAN |
                               XBEIMAGE_GAME_REGION_RESTOFWORLD |
                               XBEIMAGE_GAME_REGION_MANUFACTURING;

    if(m_Certificate.dwGameRegion & ~all_regions) {
        return "REGION ERROR";
    }

    uint8_t index = (m_Certificate.dwGameRegion & XBEIMAGE_GAME_REGION_MANUFACTURING) ? 0x8 : 0;
    index |= (m_Certificate.dwGameRegion & 0x7);
    return Region_text[index];
}

const wchar_t *Xbe::GetUnicodeFilenameAddr()
{
    return (const wchar_t *)GetAddr(m_Header.dwDebugUnicodeFilenameAddr);
}

bool Xbe::CheckSignature()
{
	init_tom_lib();

	DWORD HeaderDigestSize = m_Header.dwSizeofHeaders - (sizeof(m_Header.dwMagic) + sizeof(m_Header.pbDigitalSignature));
	UCHAR SHADigest[A_SHA_DIGEST_LEN];
	unsigned char crypt_buffer[256];
	CalcSHA1Hash(SHADigest, m_SignatureHeader, HeaderDigestSize);

	// Hash against all currently known public keys, if these pass, we can guarantee the Xbe is unmodified
	std::array<RSA_PUBLIC_KEY, 3> keys = { 0 };
	memcpy(keys[0].Default, (void*)xboxkrnl::XePublicKeyDataRetail, 284);
	memcpy(keys[1].Default, (void*)xboxkrnl::XePublicKeyDataChihiroGame, 284);
	memcpy(keys[2].Default, (void*)xboxkrnl::XePublicKeyDataChihiroBoot, 284);
	// TODO: memcpy(keys[3].Default, (void*)xboxkrnl::XePublicKeyDataDebug, 284);

	for (unsigned int i = 0; i < keys.size(); i++) {
		if (xbox_rsa_public(m_Header.pbDigitalSignature, crypt_buffer, keys[i])) {
			if (verify_hash(SHADigest, crypt_buffer, keys[i])) {
				// Load the successful key into XboxKrnl::XePublicKeyData for application use
				memcpy(xboxkrnl::XePublicKeyData, keys[i].Default, 284);
				return true; // success
			}
		}
	}

	// Default to the Retail key if no key matched, just to make sure we don't init in an invalid state
	memcpy(xboxkrnl::XePublicKeyData, xboxkrnl::XePublicKeyDataRetail, 284);
	return false;  // signature check failed
}

bool Xbe::CheckSectionIntegrity(uint32_t sectionIndex)
{
    uint32_t RawSize = m_SectionHeader[sectionIndex].dwSizeofRaw;
    if (RawSize == 0) {
        return true;
    }

    unsigned char SHADigest[A_SHA_DIGEST_LEN];
    CalcSHA1Hash(SHADigest, m_bzSection[sectionIndex], RawSize);

    if (std::memcmp(SHADigest, m_SectionHeader[sectionIndex].bzSectionDigest, A_SHA_DIGEST_LEN) != 0) {
        return false;
    }
    else {
        return true;
    }
}

// ported from Dxbx's XbeExplorer
XbeType Xbe::GetXbeType()
{
	// Detect if the XBE is for Chihiro (Untested!) :
	// This is based on https://github.com/radare/radare2/blob/master/libr/bin/p/bin_xbe.c#L45
	if ((m_Header.dwEntryAddr & XBOX_WRITE_COMBINED_BASE) == SEGABOOT_EP_XOR)
		return XbeType::xtChihiro;

	// Check for Debug XBE, using high bit of the kernel thunk address :
	// (DO NOT test like https://github.com/radare/radare2/blob/master/libr/bin/p/bin_xbe.c#L49 !)
	if ((m_Header.dwKernelImageThunkAddr & KSEG0_BASE) > 0)
		return XbeType::xtDebug;

	// Otherwise, the XBE is a Retail build :
	return XbeType::xtRetail;
}
