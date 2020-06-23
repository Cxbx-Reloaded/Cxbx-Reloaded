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
#ifndef XBE_H
#define XBE_H

#include "common\Error.h"
#include "common/xbox/Types.hpp"

#include <cstdio>


//#include <windef.h> // For MAX_PATH
// The above leads to 55 compile errors, so until we've sorted out why that happens, declare MAX_PATH ourselves for now :
#define MAX_PATH 260
#define XPR_IMAGE_WH 128
#define XPR_IMAGE_DATA_SIZE (XPR_IMAGE_WH * XPR_IMAGE_WH) / 2
#define XPR_IMAGE_HDR_SIZE 2048

namespace xboxkrnl
{
	typedef struct _XBE_SECTION	XBEIMAGE_SECTION, *PXBEIMAGE_SECTION;
}

// Xbe (Xbox Executable) file object
class Xbe : public Error
{
    public:
        // construct via Xbe file
        Xbe(const char *x_szFilename, bool bFromGUI);
		
        // deconstructor
       ~Xbe();

		// find an section by name
		void *FindSection(char *zsSectionName);

		// Find a section by its definition
		void* FindSection(xboxkrnl::PXBEIMAGE_SECTION section);

        // export to Xbe file
        void Export(const char *x_szXbeFilename);

        // verify the integrity of the xbe header
        bool CheckSignature();

        // verify the integrity of an xbe section
        bool CheckSectionIntegrity(uint32_t sectionIndex);

        // import logo bitmap from raw monochrome data
        void ImportLogoBitmap(const uint8_t x_Gray[100*17]);

        // export logo bitmap to raw monochrome data
        void ExportLogoBitmap(uint8_t x_Gray[100*17]);

		// purge illegal characters in Windows filenames or other OS's
		void PurgeBadChar(std::string& s, const std::string& illegalChars = "\\/:?\"<>|");

        // Convert game region field to string
        const char *GameRegionToString();

        XbeType GetXbeType();

        // Xbe header
        #include "AlignPrefix1.h"
        struct Header
        {
            uint32_t dwMagic;                         // 0x0000 - magic number [should be "XBEH"]
            uint8_t  pbDigitalSignature[256];         // 0x0004 - digital signature
            uint32_t dwBaseAddr;                      // 0x0104 - base address
            uint32_t dwSizeofHeaders;                 // 0x0108 - size of headers
            uint32_t dwSizeofImage;                   // 0x010C - size of image
            uint32_t dwSizeofImageHeader;             // 0x0110 - size of image header
            uint32_t dwTimeDate;                      // 0x0114 - timedate stamp
            uint32_t dwCertificateAddr;               // 0x0118 - certificate address
            uint32_t dwSections;                      // 0x011C - number of sections
            uint32_t dwSectionHeadersAddr;            // 0x0120 - section headers address

			typedef struct                         
            {
                uint32_t bMountUtilityDrive   : 1;    // mount utility drive flag
                uint32_t bFormatUtilityDrive  : 1;    // format utility drive flag
                uint32_t bLimit64MB           : 1;    // limit development kit run time memory to 64mb flag
                uint32_t bDontSetupHarddisk   : 1;    // don't setup hard disk flag
                uint32_t Unused               : 4;    // unused (or unknown)
                uint32_t Unused_b1            : 8;    // unused (or unknown)
                uint32_t Unused_b2            : 8;    // unused (or unknown)
                uint32_t Unused_b3            : 8;    // unused (or unknown)
			} InitFlags;

			union {                                 // 0x0124 - initialization flags
				InitFlags dwInitFlags;
                uint32_t dwInitFlags_value;
			};

            uint32_t dwEntryAddr;                     // 0x0128 - entry point address
            uint32_t dwTLSAddr;                       // 0x012C - thread local storage directory address
            uint32_t dwPeStackCommit;                 // 0x0130 - size of stack commit
            uint32_t dwPeHeapReserve;                 // 0x0134 - size of heap reserve
            uint32_t dwPeHeapCommit;                  // 0x0138 - size of heap commit
            uint32_t dwPeBaseAddr;                    // 0x013C - original base address
            uint32_t dwPeSizeofImage;                 // 0x0140 - size of original image
            uint32_t dwPeChecksum;                    // 0x0144 - original checksum
            uint32_t dwPeTimeDate;                    // 0x0148 - original timedate stamp
            uint32_t dwDebugPathnameAddr;             // 0x014C - debug pathname address
            uint32_t dwDebugFilenameAddr;             // 0x0150 - debug filename address
            uint32_t dwDebugUnicodeFilenameAddr;      // 0x0154 - debug unicode filename address
            uint32_t dwKernelImageThunkAddr;          // 0x0158 - kernel image thunk address
            uint32_t dwNonKernelImportDirAddr;        // 0x015C - non kernel import directory address
            uint32_t dwLibraryVersions;               // 0x0160 - number of library versions
            uint32_t dwLibraryVersionsAddr;           // 0x0164 - library versions address
            uint32_t dwKernelLibraryVersionAddr;      // 0x0168 - kernel library version address
            uint32_t dwXAPILibraryVersionAddr;        // 0x016C - xapi library version address
            uint32_t dwLogoBitmapAddr;                // 0x0170 - logo bitmap address
            uint32_t dwSizeofLogoBitmap;              // 0x0174 - logo bitmap size
        }
        #include "AlignPosfix1.h"
        m_Header;

        // Xbe header extra byte (used to preserve unknown data)
        char *m_HeaderEx;
        uint32_t m_ExSize;

        // Xbe certificate
        #include "AlignPrefix1.h"
        struct Certificate
        {
            uint32_t  dwSize;                               // 0x0000 - size of certificate
            uint32_t  dwTimeDate;                           // 0x0004 - timedate stamp
            uint32_t  dwTitleId;                            // 0x0008 - title id
            wchar_t wszTitleName[40];                       // 0x000C - title name (unicode)
            uint32_t  dwAlternateTitleId[0x10];             // 0x005C - alternate title ids
            uint32_t  dwAllowedMedia;                       // 0x009C - allowed media types
            uint32_t  dwGameRegion;                         // 0x00A0 - game region
            uint32_t  dwGameRatings;                        // 0x00A4 - game ratings
            uint32_t  dwDiskNumber;                         // 0x00A8 - disk number
            uint32_t  dwVersion;                            // 0x00AC - version
            uint8_t  bzLanKey[16];                          // 0x00B0 - lan key
            uint8_t  bzSignatureKey[16];                    // 0x00C0 - signature key
            // NOT ALL XBEs have these fields!
            uint8_t  bzTitleAlternateSignatureKey[16][16];  // 0x00D0 - alternate signature keys
            uint32_t  dwOriginalCertificateSize;            // 0x01D0 - Original Certificate Size?
            uint32_t  dwOnlineService;                      // 0x01D4 - Online Service ID
            uint32_t  dwSecurityFlags;                      // 0x01D8 - Extra Security Flags
            uint8_t  bzCodeEncKey[16];                      // 0x01DC - Code Encryption Key?
        }
        #include "AlignPosfix1.h"
        m_Certificate;

        // Xbe section header
        #include "AlignPrefix1.h"
        struct SectionHeader
        {
            typedef struct 
            {
                uint32_t bWritable        : 1;    // writable flag
                uint32_t bPreload         : 1;    // preload flag
                uint32_t bExecutable      : 1;    // executable flag
                uint32_t bInsertedFile    : 1;    // inserted file flag
                uint32_t bHeadPageRO      : 1;    // head page read only flag
                uint32_t bTailPageRO      : 1;    // tail page read only flag
                uint32_t Unused_a1        : 1;    // unused (or unknown)
                uint32_t Unused_a2        : 1;    // unused (or unknown)
                uint32_t Unused_b1        : 8;    // unused (or unknown)
                uint32_t Unused_b2        : 8;    // unused (or unknown)
                uint32_t Unused_b3        : 8;    // unused (or unknown)
			} _Flags;

			union {
				_Flags dwFlags;
                uint32_t dwFlags_value;
			};

            uint32_t dwVirtualAddr;               // virtual address
            uint32_t dwVirtualSize;               // virtual size
            uint32_t dwRawAddr;                   // file offset to raw data
            uint32_t dwSizeofRaw;                 // size of raw data
            uint32_t dwSectionNameAddr;           // section name addr
            uint32_t dwSectionRefCount;           // section reference count
            uint32_t dwHeadSharedRefCountAddr;    // head shared page reference count address
            uint32_t dwTailSharedRefCountAddr;    // tail shared page reference count address
            uint8_t  bzSectionDigest[20];         // section digest
        }
        #include "AlignPosfix1.h"
        *m_SectionHeader;

        // Xbe library versions
        #include "AlignPrefix1.h"
        struct LibraryVersion
        {
            char   szName[8];                     // library name
            uint16_t wMajorVersion;               // major version
            uint16_t wMinorVersion;               // minor version
            uint16_t wBuildVersion;               // build version

            typedef struct 
            {
                uint16_t QFEVersion       : 13;   // QFE Version
                uint16_t Approved         : 2;    // Approved? (0:no, 1:possibly, 2:yes)
                uint16_t bDebugBuild      : 1;    // Is this a debug build?
			} Flags;

			union {
				Flags wFlags;
                uint16_t wFlags_value;
			};
        }
        #include "AlignPosfix1.h"
		*m_LibraryVersion;

        // Xbe thread local storage
        #include "AlignPrefix1.h"
        struct TLS
        {
            uint32_t dwDataStartAddr;             // raw start address
            uint32_t dwDataEndAddr;               // raw end address
            uint32_t dwTLSIndexAddr;              // tls index address
            uint32_t dwTLSCallbackAddr;           // tls callback address
            uint32_t dwSizeofZeroFill;            // size of zero fill
            uint32_t dwCharacteristics;           // characteristics
        }
        #include "AlignPosfix1.h"
        *m_TLS;

		// Xbe signature header
		uint8_t* m_SignatureHeader;

        // Xbe section names, stored null terminated
        char (*m_szSectionName)[10];

        // Xbe sections
        uint8_t **m_bzSection;

        // Xbe original path
        char m_szPath[MAX_PATH];

        // Xbe ascii title, translated from certificate title
        char m_szAsciiTitle[40];

        // retrieve thread local storage data address
        uint8_t *GetTLSData() { if(m_TLS == 0) return 0; else return GetAddr(m_TLS->dwDataStartAddr); }

        // retrieve thread local storage index address
        uint32_t *GetTLSIndex() { if(m_TLS == 0) return 0; else return (uint32_t*)GetAddr(m_TLS->dwTLSIndexAddr); }

        // return a modifiable pointer inside this structure that corresponds to a virtual address
        uint8_t *GetAddr(uint32_t x_dwVirtualAddress);

        const wchar_t *GetUnicodeFilenameAddr();
    private:
        // constructor initialization
        void ConstructorInit();

        // return a modifiable pointer to logo bitmap data
        uint8_t *GetLogoBitmap(uint32_t x_dwSize);


        // used to encode/decode logo bitmap data
        union LogoRLE
        {
            struct Eight
            {
                uint32_t bType1  : 1;
                uint32_t Len     : 3;
                uint32_t Data    : 4;
            }
            m_Eight;

            struct Sixteen
            {
                uint32_t bType1  : 1;
                uint32_t bType2  : 1;
                uint32_t Len     : 10;
                uint32_t Data    : 4;
            }
            m_Sixteen;
        };

	public:
		// used to decode game logo bitmap data
		#include "AlignPrefix1.h"
		struct X_D3DResourceLoc
		{
            uint32_t Common;
            uint32_t Data;
            uint32_t Lock;
            uint32_t Format;
            uint32_t Size;
		}
		#include "AlignPosfix1.h"
		;

		#include "AlignPrefix1.h"
		// XPR structures

		// Purpose:
		//   The XPR file format allows multiple graphics resources to be pre-defined
		//   and bundled together into one file.  These resources can be copied into
		//   memory and then immediately used in-place as D3D objects such as textures
		//   and vertex buffers.  The structure below defines the XPR header and the
		//   unique identifier for this file type.
		struct XprHeader
		{
            uint32_t dwXprMagic; // 'XPR0' or 'XPR1'
            uint32_t dwXprTotalSize;
            uint32_t dwXprHeaderSize;
		}
		#include "AlignPosfix1.h"
		*m_xprHeader;

		#include "AlignPrefix1.h"
		// Layout of SaveImage.xbx saved game image file
		//
		// File is XPR0 format. Since the XPR will always contain only a single
		// 256x256 DXT1 image, we know exactly what the header portion will look like
		struct XprImageHeader
		{
			XprHeader xprHeader; // Standard XPR struct
			X_D3DResourceLoc d3dTexture; // Standard D3D texture struct
            uint32_t dwEndOfHeader; // $FFFFFFFF
		}
		#include "AlignPosfix1.h"
		*m_xprImageHeader;


		#include "AlignPrefix1.h"
		struct XprImage
		{
			XprImageHeader xprImageHeader;
			char strPad[XPR_IMAGE_HDR_SIZE - sizeof(XprImageHeader)];
			unsigned char pBits;
		}
		#include "AlignPosfix1.h"
		*m_xprImage;
};

// debug/retail XOR keys
const uint32_t XOR_EP_DEBUG                            = 0x94859D4B; // Entry Point (Debug)
const uint32_t XOR_EP_RETAIL                           = 0xA8FC57AB; // Entry Point (Retail)
const uint32_t XOR_KT_DEBUG                            = 0xEFB1F152; // Kernel Thunk (Debug)
const uint32_t XOR_KT_RETAIL                           = 0x5B6D40B6; // Kernel Thunk (Retail)

// Sega Chihiro XOR keys
const uint32_t XOR_EP_CHIHIRO							 = 0x40B5C16E;
const uint32_t XOR_KT_CHIHIRO							 = 0x2290059D;

// game region flags for Xbe certificate
const uint32_t XBEIMAGE_GAME_REGION_NA                 = 0x00000001;
const uint32_t XBEIMAGE_GAME_REGION_JAPAN              = 0x00000002;
const uint32_t XBEIMAGE_GAME_REGION_RESTOFWORLD        = 0x00000004;
const uint32_t XBEIMAGE_GAME_REGION_MANUFACTURING      = 0x80000000;

// media type flags for Xbe certificate
const uint32_t XBEIMAGE_MEDIA_TYPE_HARD_DISK           = 0x00000001;
const uint32_t XBEIMAGE_MEDIA_TYPE_DVD_X2              = 0x00000002;
const uint32_t XBEIMAGE_MEDIA_TYPE_DVD_CD              = 0x00000004;
const uint32_t XBEIMAGE_MEDIA_TYPE_CD                  = 0x00000008;
const uint32_t XBEIMAGE_MEDIA_TYPE_DVD_5_RO            = 0x00000010;
const uint32_t XBEIMAGE_MEDIA_TYPE_DVD_9_RO            = 0x00000020;
const uint32_t XBEIMAGE_MEDIA_TYPE_DVD_5_RW            = 0x00000040;
const uint32_t XBEIMAGE_MEDIA_TYPE_DVD_9_RW            = 0x00000080;
const uint32_t XBEIMAGE_MEDIA_TYPE_DONGLE              = 0x00000100;
const uint32_t XBEIMAGE_MEDIA_TYPE_MEDIA_BOARD         = 0x00000200;
const uint32_t XBEIMAGE_MEDIA_TYPE_NONSECURE_HARD_DISK = 0x40000000;
const uint32_t XBEIMAGE_MEDIA_TYPE_NONSECURE_MODE      = 0x80000000;
const uint32_t XBEIMAGE_MEDIA_TYPE_MEDIA_MASK          = 0x00FFFFFF;

// section type flags for Xbe
const uint32_t XBEIMAGE_SECTION_WRITEABLE				 = 0x00000001;
const uint32_t XBEIMAGE_SECTION_PRELOAD				 = 0x00000002;
const uint32_t XBEIMAGE_SECTION_EXECUTABLE			 = 0x00000004;
const uint32_t XBEIMAGE_SECTION_INSERTFILE			 = 0x00000008;
const uint32_t XBEIMAGE_SECTION_HEAD_PAGE_READONLY	 = 0x00000010;
const uint32_t XBEIMAGE_SECTION_TAIL_PAGE_READONLY	 = 0x00000020;
#endif
