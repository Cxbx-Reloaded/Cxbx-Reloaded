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
#include "CxbxKrnl\EmuD3D8Types.h"

#include <memory.h>
#include <clocale>
#include <cstdlib>
#include <cstring>
#include <ctime>

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

        m_szSectionName = new char[m_Header.dwSections][9];
        for(uint32 v=0;v<m_Header.dwSections;v++)
        {
            printf("Xbe::Xbe: Reading Section Name 0x%.04X...", v);

            uint08 *sn = GetAddr(m_SectionHeader[v].dwSectionNameAddr);

            memset(m_szSectionName[v], 0, 9);

            if(sn != 0)
            {
                for(int b=0;b<8;b++)
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
	printf("-------\n");
	ExportLogoBitmap(); // FIXME move this line elsewhere
	printf("-------\n");

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

// dump Xbe information to text file
void Xbe::DumpInformation(FILE *x_file)
{
    if(HasError())
        return;

    fprintf(x_file, "XBE information generated by Cxbx-Reloaded (Version " _CXBX_VERSION ")\n");
    fprintf(x_file, "\n");
    fprintf(x_file, "Title identified as \"%s\"\n", m_szAsciiTitle);
    fprintf(x_file, "\n");
    fprintf(x_file, "Dumping XBE file header...\n");
    fprintf(x_file, "\n");
    fprintf(x_file, "Magic Number                     : XBEH\n");

    // print digital signature
    {
        fprintf(x_file, "Digitial Signature               : <Hex Dump>");
        for(int y=0;y<16;y++)
        {
            fprintf(x_file, "\n                                   ");
            for(int x=0;x<16;x++)
                fprintf(x_file, "%.02X", m_Header.pbDigitalSignature[y*16+x]);
        }
        fprintf(x_file, "\n                                   </Hex Dump>\n");
    }

    fprintf(x_file, "Base Address                     : 0x%.08X\n", m_Header.dwBaseAddr);
    fprintf(x_file, "Size of Headers                  : 0x%.08X\n", m_Header.dwSizeofHeaders);
    fprintf(x_file, "Size of Image                    : 0x%.08X\n", m_Header.dwSizeofImage);
    fprintf(x_file, "Size of Image Header             : 0x%.08X\n", m_Header.dwSizeofImageHeader);
    fprintf(x_file, "TimeDate Stamp                   : 0x%.08X (%s)\n", m_Header.dwTimeDate, BetterTime(m_Header.dwTimeDate));
    fprintf(x_file, "Certificate Address              : 0x%.08X\n", m_Header.dwCertificateAddr);
    fprintf(x_file, "Number of Sections               : 0x%.08X\n", m_Header.dwSections);
    fprintf(x_file, "Section Headers Address          : 0x%.08X\n", m_Header.dwSectionHeadersAddr);

    // print init flags
    {
        fprintf(x_file, "Init Flags                       : 0x%.08X ", m_Header.dwInitFlags.bMountUtilityDrive);

        if(m_Header.dwInitFlags.bMountUtilityDrive)
            fprintf(x_file, "[Mount Utility Drive] ");

        if(m_Header.dwInitFlags.bFormatUtilityDrive)
            fprintf(x_file, "[Format Utility Drive] ");

        if(m_Header.dwInitFlags.bLimit64MB)
            fprintf(x_file, "[Limit Devkit Run Time Memory to 64MB] ");

        if(!m_Header.dwInitFlags.bDontSetupHarddisk)
            fprintf(x_file, "[Setup Harddisk] ");

        fprintf(x_file, "\n");
    }

    char AsciiFilename[40];

    setlocale( LC_ALL, "English" );

    const wchar_t *wszFilename = (const wchar_t *)GetAddr(m_Header.dwDebugUnicodeFilenameAddr);

    if(wszFilename != NULL)
        wcstombs(AsciiFilename, wszFilename, 40);
    else
        AsciiFilename[0] = '\0';

    fprintf(x_file, "Entry Point                      : 0x%.08X (Retail: 0x%.08X, Debug: 0x%.08X)\n", m_Header.dwEntryAddr, m_Header.dwEntryAddr ^ XOR_EP_RETAIL, m_Header.dwEntryAddr ^ XOR_EP_DEBUG);
    fprintf(x_file, "TLS Address                      : 0x%.08X\n", m_Header.dwTLSAddr);
    fprintf(x_file, "(PE) Stack Commit                : 0x%.08X\n", m_Header.dwPeStackCommit);
    fprintf(x_file, "(PE) Heap Reserve                : 0x%.08X\n", m_Header.dwPeHeapReserve);
    fprintf(x_file, "(PE) Heap Commit                 : 0x%.08X\n", m_Header.dwPeHeapCommit);
    fprintf(x_file, "(PE) Base Address                : 0x%.08X\n", m_Header.dwPeBaseAddr);
    fprintf(x_file, "(PE) Size of Image               : 0x%.08X\n", m_Header.dwPeSizeofImage);
    fprintf(x_file, "(PE) Checksum                    : 0x%.08X\n", m_Header.dwPeChecksum);
    fprintf(x_file, "(PE) TimeDate Stamp              : 0x%.08X (%s)\n", m_Header.dwPeTimeDate, BetterTime(m_Header.dwPeTimeDate));
    fprintf(x_file, "Debug Pathname Address           : 0x%.08X (\"%s\")\n", m_Header.dwDebugPathnameAddr, GetAddr(m_Header.dwDebugPathnameAddr));
    fprintf(x_file, "Debug Filename Address           : 0x%.08X (\"%s\")\n", m_Header.dwDebugFilenameAddr, GetAddr(m_Header.dwDebugFilenameAddr));
    fprintf(x_file, "Debug Unicode filename Address   : 0x%.08X (L\"%s\")\n", m_Header.dwDebugUnicodeFilenameAddr, AsciiFilename);
    fprintf(x_file, "Kernel Image Thunk Address       : 0x%.08X (Retail: 0x%.08X, Debug: 0x%.08X)\n", m_Header.dwKernelImageThunkAddr, m_Header.dwKernelImageThunkAddr ^ XOR_KT_RETAIL, m_Header.dwKernelImageThunkAddr ^ XOR_KT_DEBUG);
    fprintf(x_file, "NonKernel Import Dir Address     : 0x%.08X\n", m_Header.dwNonKernelImportDirAddr);
    fprintf(x_file, "Library Versions                 : 0x%.08X\n", m_Header.dwLibraryVersions);
    fprintf(x_file, "Library Versions Address         : 0x%.08X\n", m_Header.dwLibraryVersionsAddr);
    fprintf(x_file, "Kernel Library Version Address   : 0x%.08X\n", m_Header.dwKernelLibraryVersionAddr);
    fprintf(x_file, "XAPI Library Version Address     : 0x%.08X\n", m_Header.dwXAPILibraryVersionAddr);
    fprintf(x_file, "Logo Bitmap Address              : 0x%.08X\n", m_Header.dwLogoBitmapAddr);
    fprintf(x_file, "Logo Bitmap Size                 : 0x%.08X\n", m_Header.dwSizeofLogoBitmap);
    fprintf(x_file, "\n");
    fprintf(x_file, "Dumping XBE Certificate...\n");
    fprintf(x_file, "\n");
    fprintf(x_file, "Size of Certificate              : 0x%.08X\n", m_Certificate.dwSize);
    fprintf(x_file, "TimeDate Stamp                   : 0x%.08X (%s)\n", m_Certificate.dwTimeDate, BetterTime(m_Certificate.dwTimeDate));
    fprintf(x_file, "Title ID                         : 0x%.08X\n", m_Certificate.dwTitleId);
    fprintf(x_file, "Title                            : L\"%s\"\n", m_szAsciiTitle);

    // print alternate title IDs
    {
        fprintf(x_file, "Alternate Titles IDs             : ");

        for(int v=0;v<0x10;v++)
        {
            if(v != 0)
                fprintf(x_file, "                                   ");
            fprintf(x_file, "0x%.08X", m_Certificate.dwAlternateTitleId[v]);
            if(v != 0x0F)
                fprintf(x_file, "\n");
        }

        fprintf(x_file, "\n");
    }

    fprintf(x_file, "Allowed Media                    : 0x%.08X\n", m_Certificate.dwAllowedMedia);
    fprintf(x_file, "Game Region                      : 0x%.08X\n", m_Certificate.dwGameRegion);
    fprintf(x_file, "Game Ratings                     : 0x%.08X\n", m_Certificate.dwGameRatings);
    fprintf(x_file, "Disk Number                      : 0x%.08X\n", m_Certificate.dwDiskNumber);
    fprintf(x_file, "Version                          : 0x%.08X\n", m_Certificate.dwVersion);

    // print LAN key
    {
        fprintf(x_file, "LAN Key                          : ");
        for(int x=0;x<16;x++)
            fprintf(x_file, "%.02X", m_Certificate.bzLanKey[x]);
        fprintf(x_file, "\n");
    }

    // print signature key
    {
        fprintf(x_file, "Signature Key                    : ");
        for(int x=0;x<16;x++)
            fprintf(x_file, "%.02X", m_Certificate.bzSignatureKey[x]);
        fprintf(x_file, "\n");
    }

    // print alternate signature keys
    {
        fprintf(x_file, "Title Alternate Signature Keys : <Hex Dump>");
        for(int y=0;y<16;y++)
        {
            fprintf(x_file, "\n                                   ");
            for(int x=0;x<16;x++)
                fprintf(x_file, "%.02X", m_Certificate.bzTitleAlternateSignatureKey[y][x]);
        }
        fprintf(x_file, "\n                                   </Hex Dump>\n");
    }

    fprintf(x_file, "\n");
    fprintf(x_file, "Dumping XBE Section Headers...\n");
    fprintf(x_file, "\n");

    // print section headers
    {
        for(uint32 v=0;v<m_Header.dwSections;v++)
        {
            fprintf(x_file, "Section Name                     : 0x%.08X (\"%s\")\n", m_SectionHeader[v].dwSectionNameAddr, m_szSectionName[v]);

            // print flags
            {
                fprintf(x_file, "Flags                            : 0x%.08X ", m_SectionHeader[v].dwFlags.bWritable);

                if(m_SectionHeader[v].dwFlags.bWritable)
                    fprintf(x_file, "(Writable) ");

                if(m_SectionHeader[v].dwFlags.bPreload)
                    fprintf(x_file, "(Preload) ");

                if(m_SectionHeader[v].dwFlags.bExecutable)
                    fprintf(x_file, "(Executable) ");

                if(m_SectionHeader[v].dwFlags.bInsertedFile)
                    fprintf(x_file, "(Inserted File) ");

                if(m_SectionHeader[v].dwFlags.bHeadPageRO)
                    fprintf(x_file, "(Head Page RO) ");

                if(m_SectionHeader[v].dwFlags.bTailPageRO)
                    fprintf(x_file, "(Tail Page RO) ");

                fprintf(x_file, "\n");
            }

            fprintf(x_file, "Virtual Address                  : 0x%.08X\n", m_SectionHeader[v].dwVirtualAddr);
            fprintf(x_file, "Virtual Size                     : 0x%.08X\n", m_SectionHeader[v].dwVirtualSize);
            fprintf(x_file, "Raw Address                      : 0x%.08X\n", m_SectionHeader[v].dwRawAddr);
            fprintf(x_file, "Size of Raw                      : 0x%.08X\n", m_SectionHeader[v].dwSizeofRaw);
            fprintf(x_file, "Section Name Address             : 0x%.08X\n", m_SectionHeader[v].dwSectionNameAddr);
            fprintf(x_file, "Section Reference Count          : 0x%.08X\n", m_SectionHeader[v].dwSectionRefCount);
            fprintf(x_file, "Head Shared Reference Count Addr : 0x%.08X\n", m_SectionHeader[v].dwHeadSharedRefCountAddr);
            fprintf(x_file, "Tail Shared Reference Count Addr : 0x%.08X\n", m_SectionHeader[v].dwTailSharedRefCountAddr);

            // print section digest
            {
                fprintf(x_file, "Section Digest                   : ");
                for(int s=0;s<20;s++)
                    fprintf(x_file, "%.02X", m_SectionHeader[v].bzSectionDigest[s]);
                fprintf(x_file, "\n");
            }

            fprintf(x_file, "\n");
        }
    }

    fprintf(x_file, "Dumping XBE Library Versions...\n");
    fprintf(x_file, "\n");

    // print library versions
    {
        if(m_LibraryVersion == 0 || m_Header.dwLibraryVersions == 0)
        {
            fprintf(x_file, "(This XBE contains no Library Versions)\n");
            fprintf(x_file, "\n");
        }
        else
        {
            for(uint32 v=0;v<m_Header.dwLibraryVersions;v++)
            {
                char tmp[9];

                for(uint32 c=0;c<8;c++)
                    tmp[c] = m_LibraryVersion[v].szName[c];

                tmp[8] = '\0';

                fprintf(x_file, "Library Name                     : %s\n", tmp);
                fprintf(x_file, "Version                          : %d.%d.%d\n", m_LibraryVersion[v].wMajorVersion, m_LibraryVersion[v].wMinorVersion, m_LibraryVersion[v].wBuildVersion);

                // print flags
                {
                    fprintf(x_file, "Flags                            : ");

                    fprintf(x_file, "QFEVersion : 0x%.04X, ", m_LibraryVersion[v].dwFlags.QFEVersion);

                    if(m_LibraryVersion[v].dwFlags.bDebugBuild)
                        fprintf(x_file, "Debug, ");
                    else
                        fprintf(x_file, "Retail, ");

                    switch(m_LibraryVersion[v].dwFlags.Approved)
                    {
                        case 0:
                            fprintf(x_file, "Unapproved");
                            break;
                        case 1:
                            fprintf(x_file, "Possibly Approved");
                            break;
                        case 2:
                            fprintf(x_file, "Approved");
                            break;
                    }

                    fprintf(x_file, "\n");
                }

                fprintf(x_file, "\n");
            }
        }
    }

    fprintf(x_file, "Dumping XBE TLS...\n");
    fprintf(x_file, "\n");

    // print thread local storage
    if(m_TLS != 0)
    {
        fprintf(x_file, "Data Start Address               : 0x%.08X\n", m_TLS->dwDataStartAddr);
        fprintf(x_file, "Data End Address                 : 0x%.08X\n", m_TLS->dwDataEndAddr);
        fprintf(x_file, "TLS Index Address                : 0x%.08X\n", m_TLS->dwTLSIndexAddr);
        fprintf(x_file, "TLS Callback Address             : 0x%.08X\n", m_TLS->dwTLSCallbackAddr);
        fprintf(x_file, "Size of Zero Fill                : 0x%.08X\n", m_TLS->dwSizeofZeroFill);
        fprintf(x_file, "Characteristics                  : 0x%.08X\n", m_TLS->dwCharacteristics);
    }
    else
    {
        fprintf(x_file, "(This XBE contains no TLS)\n");
    }
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

bool Xbe::ExportLogoBitmap()
{
	bool result = false;
	uint32 dwOffs = 0;
	uint32 dwLength = 0;
	int index = 0;
	for (uint32 v = 0; v < m_Header.dwSections; v++)
	{
		if (strcmp(m_szSectionName[v], "$$XTIMAG")==0) {
			dwOffs = m_SectionHeader[v].dwVirtualAddr;
			dwLength = m_SectionHeader[v].dwVirtualSize;
			index = v;
		}
	}
	if (dwOffs == 0 || dwLength == 0)
		return 0;

	printf("Game Logo Addr: 0x%08X\n", dwOffs);
	printf("Game Logo Size: 0x%08X\n", dwLength);

	m_xprImage = (XprImage*) m_bzSection[index];

	printf("GameLogoHeader Magic: %s\n", &m_xprImage->xprImageHeader.xprHeader.dwXprMagic);
	printf("GameLogoHeader Total Size: 0x%08X\n", m_xprImage->xprImageHeader.xprHeader.dwXprTotalSize);
	printf("GameLogoHeader Header Size: 0x%08X\n", m_xprImage->xprImageHeader.xprHeader.dwXprHeaderSize);
	
	int width = 1 << ((m_xprImage->xprImageHeader.d3dTexture.Format & X_D3DFORMAT_USIZE_MASK) >> X_D3DFORMAT_USIZE_SHIFT);
	int height = 1 <<((m_xprImage->xprImageHeader.d3dTexture.Format & X_D3DFORMAT_VSIZE_MASK) >> X_D3DFORMAT_VSIZE_SHIFT);
	printf("Game Logo Width: %X\n", width);
	printf("Game Logo Height: %X\n", height);

	int pitch = width * sizeof(uint32);
	void* bitmap = (void*)malloc(sizeof(uint32) * pitch * height);

	if ((m_xprImage->xprImageHeader.xprHeader.dwXprTotalSize - m_xprImage->xprImageHeader.xprHeader.dwXprHeaderSize) / width / height == 2) {
		printf("Game Logo is 16bit\n");
		result = ReadD3D16bitTextureFormatIntoBitmap(
			(m_xprImage->xprImageHeader.d3dTexture.Format & X_D3DFORMAT_FORMAT_MASK) >> X_D3DFORMAT_FORMAT_SHIFT,
			m_xprImage->pBits[0],
			m_xprImage->xprImageHeader.xprHeader.dwXprTotalSize - m_xprImage->xprImageHeader.xprHeader.dwXprHeaderSize,
			bitmap);
	}
	else {
		printf("Game Logo is 32bit\n");
		result = ReadD3DTextureFormatIntoBitmap(
			(m_xprImage->xprImageHeader.d3dTexture.Format & X_D3DFORMAT_FORMAT_MASK) >> X_D3DFORMAT_FORMAT_SHIFT,
			m_xprImage->pBits[0],
			m_xprImage->xprImageHeader.xprHeader.dwXprTotalSize - m_xprImage->xprImageHeader.xprHeader.dwXprHeaderSize,
			bitmap);
	}
	
	
	return result;
}

// FIXME: this function should be moved elsewhere
bool Xbe::ReadD3DTextureFormatIntoBitmap(uint32 format, unsigned char &data, uint32 dataSize, void* bitmap)
{
	switch (format)
	{
	case X_D3DFMT_DXT1:
	case X_D3DFMT_DXT3:
	case X_D3DFMT_DXT5:
		return ReadS3TCFormatIntoBitmap(format, data, dataSize, bitmap);
		break;
	case X_D3DFMT_A8R8G8B8:
	case X_D3DFMT_X8R8G8B8:
		return ReadSwizzledFormatIntoBitmap(format, data, dataSize, bitmap);
		break;
	default:
		return false;
		break;
	}
}

// FIXME: this function should be moved elsewhere
bool Xbe::ReadD3D16bitTextureFormatIntoBitmap(uint32 format, unsigned char &data, uint32 dataSize, void* bitmap)
{
	switch (format)
	{
	case X_D3DFMT_R5G6B5:
		return ReadSwizzled16bitFormatIntoBitmap(format, data, dataSize, bitmap);
		break;
	default:
		return false;
		break;
	}
}

// FIXME: this function should be moved elsewhere
bool Xbe::ReadS3TCFormatIntoBitmap(uint32 format, unsigned char &data, uint32 dataSize, void* bitmap)
{
	// FIXME - STUB
}

// FIXME: this function should be moved elsewhere
bool Xbe::ReadSwizzledFormatIntoBitmap(uint32 format, unsigned char &data, uint32 dataSize, void* bitmap)
{
	// FIXME - STUB
}

// FIXME: this function should be moved elsewhere
bool Xbe::ReadSwizzled16bitFormatIntoBitmap(uint32 format, unsigned char &data, uint32 dataSize, void* bitmap)
{
	// FIXME - STUB
}
