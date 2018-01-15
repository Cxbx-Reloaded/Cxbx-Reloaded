// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Common->Xbe.h
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
#ifndef XBE_H
#define XBE_H

#include "Common/Error.h"

#include <cstdio>


//#include <windef.h> // For MAX_PATH
// The above leads to 55 compile errors, so until we've sorted out why that happens, declare MAX_PATH ourselves for now :
#define MAX_PATH 260
#define XPR_IMAGE_WH 128
#define XPR_IMAGE_DATA_SIZE (XPR_IMAGE_WH * XPR_IMAGE_WH) / 2
#define XPR_IMAGE_HDR_SIZE 2048

// Xbe (Xbox Executable) file object
class Xbe : public Error
{
    public:
        // construct via Xbe file
        Xbe(const char *x_szFilename);
		
        // deconstructor
       ~Xbe();

	   // find an image by name
	   void *FindSection(char *zsSectionName);

        // export to Xbe file
        void Export(const char *x_szXbeFilename);

        // dump Xbe information to text file
        void DumpInformation(FILE *x_file);

        // import logo bitmap from raw monochrome data
        void ImportLogoBitmap(const uint08 x_Gray[100*17]);

        // export logo bitmap to raw monochrome data
        void ExportLogoBitmap(uint08 x_Gray[100*17]);

		// purge illegal characters in Windows filenames or other OS's
		void PurgeBadChar(std::string& s, const std::string& illegalChars = "\\/:?\"<>|");

        // Convert game region field to string
        const char *GameRegionToString();

        // Xbe header
        #include "AlignPrefix1.h"
        struct Header
        {
            uint32 dwMagic;                         // 0x0000 - magic number [should be "XBEH"]
            uint08 pbDigitalSignature[256];         // 0x0004 - digital signature
            uint32 dwBaseAddr;                      // 0x0104 - base address
            uint32 dwSizeofHeaders;                 // 0x0108 - size of headers
            uint32 dwSizeofImage;                   // 0x010C - size of image
            uint32 dwSizeofImageHeader;             // 0x0110 - size of image header
            uint32 dwTimeDate;                      // 0x0114 - timedate stamp
            uint32 dwCertificateAddr;               // 0x0118 - certificate address
            uint32 dwSections;                      // 0x011C - number of sections
            uint32 dwSectionHeadersAddr;            // 0x0120 - section headers address

			typedef struct                         
            {
                uint32 bMountUtilityDrive   : 1;    // mount utility drive flag
                uint32 bFormatUtilityDrive  : 1;    // format utility drive flag
                uint32 bLimit64MB           : 1;    // limit development kit run time memory to 64mb flag
                uint32 bDontSetupHarddisk   : 1;    // don't setup hard disk flag
                uint32 Unused               : 4;    // unused (or unknown)
                uint32 Unused_b1            : 8;    // unused (or unknown)
                uint32 Unused_b2            : 8;    // unused (or unknown)
                uint32 Unused_b3            : 8;    // unused (or unknown)
			} InitFlags;

			union {                                 // 0x0124 - initialization flags
				InitFlags dwInitFlags;
				uint32 dwInitFlags_value;
			};

            uint32 dwEntryAddr;                     // 0x0128 - entry point address
            uint32 dwTLSAddr;                       // 0x012C - thread local storage directory address
            uint32 dwPeStackCommit;                 // 0x0130 - size of stack commit
            uint32 dwPeHeapReserve;                 // 0x0134 - size of heap reserve
            uint32 dwPeHeapCommit;                  // 0x0138 - size of heap commit
            uint32 dwPeBaseAddr;                    // 0x013C - original base address
            uint32 dwPeSizeofImage;                 // 0x0140 - size of original image
            uint32 dwPeChecksum;                    // 0x0144 - original checksum
            uint32 dwPeTimeDate;                    // 0x0148 - original timedate stamp
            uint32 dwDebugPathnameAddr;             // 0x014C - debug pathname address
            uint32 dwDebugFilenameAddr;             // 0x0150 - debug filename address
            uint32 dwDebugUnicodeFilenameAddr;      // 0x0154 - debug unicode filename address
            uint32 dwKernelImageThunkAddr;          // 0x0158 - kernel image thunk address
            uint32 dwNonKernelImportDirAddr;        // 0x015C - non kernel import directory address
            uint32 dwLibraryVersions;               // 0x0160 - number of library versions
            uint32 dwLibraryVersionsAddr;           // 0x0164 - library versions address
            uint32 dwKernelLibraryVersionAddr;      // 0x0168 - kernel library version address
            uint32 dwXAPILibraryVersionAddr;        // 0x016C - xapi library version address
            uint32 dwLogoBitmapAddr;                // 0x0170 - logo bitmap address
            uint32 dwSizeofLogoBitmap;              // 0x0174 - logo bitmap size
        }
        #include "AlignPosfix1.h"
        m_Header;

        // Xbe header extra byte (used to preserve unknown data)
        char *m_HeaderEx;
		uint32 m_ExSize;

        // Xbe certificate
        #include "AlignPrefix1.h"
        struct Certificate
        {
            uint32  dwSize;                               // 0x0000 - size of certificate
            uint32  dwTimeDate;                           // 0x0004 - timedate stamp
            uint32  dwTitleId;                            // 0x0008 - title id
            wchar_t wszTitleName[40];                     // 0x000C - title name (unicode)
            uint32  dwAlternateTitleId[0x10];             // 0x005C - alternate title ids
            uint32  dwAllowedMedia;                       // 0x009C - allowed media types
            uint32  dwGameRegion;                         // 0x00A0 - game region
            uint32  dwGameRatings;                        // 0x00A4 - game ratings
            uint32  dwDiskNumber;                         // 0x00A8 - disk number
            uint32  dwVersion;                            // 0x00AC - version
            uint08  bzLanKey[16];                         // 0x00B0 - lan key
            uint08  bzSignatureKey[16];                   // 0x00C0 - signature key
            // NOT ALL XBEs have these fields!
            uint08  bzTitleAlternateSignatureKey[16][16]; // 0x00D0 - alternate signature keys
            uint32  dwOriginalCertificateSize;			  // 0x01D0 - Original Certificate Size?
            uint32  dwOnlineService;					  // 0x01D4 - Online Service ID
            uint32  dwSecurityFlags;					  // 0x01D8 - Extra Security Flags
        }
        #include "AlignPosfix1.h"
        m_Certificate;

        // Xbe section header
        #include "AlignPrefix1.h"
        struct SectionHeader
        {
            typedef struct 
            {
                uint32 bWritable        : 1;    // writable flag
                uint32 bPreload         : 1;    // preload flag
                uint32 bExecutable      : 1;    // executable flag
                uint32 bInsertedFile    : 1;    // inserted file flag
                uint32 bHeadPageRO      : 1;    // head page read only flag
                uint32 bTailPageRO      : 1;    // tail page read only flag
                uint32 Unused_a1        : 1;    // unused (or unknown)
                uint32 Unused_a2        : 1;    // unused (or unknown)
                uint32 Unused_b1        : 8;    // unused (or unknown)
                uint32 Unused_b2        : 8;    // unused (or unknown)
                uint32 Unused_b3        : 8;    // unused (or unknown)
			} _Flags;

			union {
				_Flags dwFlags;
				uint32 dwFlags_value;
			};

            uint32 dwVirtualAddr;               // virtual address
            uint32 dwVirtualSize;               // virtual size
            uint32 dwRawAddr;                   // file offset to raw data
            uint32 dwSizeofRaw;                 // size of raw data
            uint32 dwSectionNameAddr;           // section name addr
            uint32 dwSectionRefCount;           // section reference count
            uint32 dwHeadSharedRefCountAddr;    // head shared page reference count address
            uint32 dwTailSharedRefCountAddr;    // tail shared page reference count address
            uint08 bzSectionDigest[20];         // section digest
        }
        #include "AlignPosfix1.h"
        *m_SectionHeader;

        // Xbe library versions
        #include "AlignPrefix1.h"
        struct LibraryVersion
        {
            char   szName[8];                   // library name
            uint16 wMajorVersion;               // major version
            uint16 wMinorVersion;               // minor version
            uint16 wBuildVersion;               // build version

            typedef struct 
            {
                uint16 QFEVersion       : 13;   // QFE Version
                uint16 Approved         : 2;    // Approved? (0:no, 1:possibly, 2:yes)
                uint16 bDebugBuild      : 1;    // Is this a debug build?
			} Flags;

			union {
				Flags wFlags;
				uint16 wFlags_value;
			};
        }
        #include "AlignPosfix1.h"
        *m_LibraryVersion, *m_KernelLibraryVersion, *m_XAPILibraryVersion;

        // Xbe thread local storage
        #include "AlignPrefix1.h"
        struct TLS
        {
            uint32 dwDataStartAddr;             // raw start address
            uint32 dwDataEndAddr;               // raw end address
            uint32 dwTLSIndexAddr;              // tls index address
            uint32 dwTLSCallbackAddr;           // tls callback address
            uint32 dwSizeofZeroFill;            // size of zero fill
            uint32 dwCharacteristics;           // characteristics
        }
        #include "AlignPosfix1.h"
        *m_TLS;

        // Xbe section names, stored null terminated
        char (*m_szSectionName)[10];

        // Xbe sections
        uint08 **m_bzSection;

        // Xbe original path
        char m_szPath[MAX_PATH];

        // Xbe ascii title, translated from certificate title
        char m_szAsciiTitle[40];

        // retrieve thread local storage data address
        uint08 *GetTLSData() { if(m_TLS == 0) return 0; else return GetAddr(m_TLS->dwDataStartAddr); }

        // retrieve thread local storage index address
        uint32 *GetTLSIndex() { if(m_TLS == 0) return 0; else return (uint32*)GetAddr(m_TLS->dwTLSIndexAddr); }

    private:
        // constructor initialization
        void ConstructorInit();

        // return a modifiable pointer inside this structure that corresponds to a virtual address
        uint08 *GetAddr(uint32 x_dwVirtualAddress);

        // return a modifiable pointer to logo bitmap data
        uint08 *GetLogoBitmap(uint32 x_dwSize);

        std::string AllowedMediaToString();

        // used to encode/decode logo bitmap data
        union LogoRLE
        {
            struct Eight
            {
                uint32 bType1  : 1;
                uint32 Len     : 3;
                uint32 Data    : 4;
            }
            m_Eight;

            struct Sixteen
            {
                uint32 bType1  : 1;
                uint32 bType2  : 1;
                uint32 Len     : 10;
                uint32 Data    : 4;
            }
            m_Sixteen;
        };

	public:
		// used to decode game logo bitmap data
		#include "AlignPrefix1.h"
		struct X_D3DResourceLoc
		{
			uint32 Common;
			uint32 Data;
			uint32 Lock;
			uint32 Format;
			uint32 Size;
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
			uint32 dwXprMagic; // 'XPR0' or 'XPR1'
			uint32 dwXprTotalSize;
			uint32 dwXprHeaderSize;
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
			uint32 dwEndOfHeader; // $FFFFFFFF
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
const uint32 XOR_EP_DEBUG                            = 0x94859D4B; // Entry Point (Debug)
const uint32 XOR_EP_RETAIL                           = 0xA8FC57AB; // Entry Point (Retail)
const uint32 XOR_KT_DEBUG                            = 0xEFB1F152; // Kernel Thunk (Debug)
const uint32 XOR_KT_RETAIL                           = 0x5B6D40B6; // Kernel Thunk (Retail)

// Sega Chihiro XOR keys
const uint32 XOR_EP_CHIHIRO							 = 0x40B5C16E;
const uint32 XOR_KT_CHIHIRO							 = 0x2290059D;

// game region flags for Xbe certificate
const uint32 XBEIMAGE_GAME_REGION_NA                 = 0x00000001;
const uint32 XBEIMAGE_GAME_REGION_JAPAN              = 0x00000002;
const uint32 XBEIMAGE_GAME_REGION_RESTOFWORLD        = 0x00000004;
const uint32 XBEIMAGE_GAME_REGION_MANUFACTURING      = 0x80000000;

// media type flags for Xbe certificate
const uint32 XBEIMAGE_MEDIA_TYPE_HARD_DISK           = 0x00000001;
const uint32 XBEIMAGE_MEDIA_TYPE_DVD_X2              = 0x00000002;
const uint32 XBEIMAGE_MEDIA_TYPE_DVD_CD              = 0x00000004;
const uint32 XBEIMAGE_MEDIA_TYPE_CD                  = 0x00000008;
const uint32 XBEIMAGE_MEDIA_TYPE_DVD_5_RO            = 0x00000010;
const uint32 XBEIMAGE_MEDIA_TYPE_DVD_9_RO            = 0x00000020;
const uint32 XBEIMAGE_MEDIA_TYPE_DVD_5_RW            = 0x00000040;
const uint32 XBEIMAGE_MEDIA_TYPE_DVD_9_RW            = 0x00000080;
const uint32 XBEIMAGE_MEDIA_TYPE_DONGLE              = 0x00000100;
const uint32 XBEIMAGE_MEDIA_TYPE_MEDIA_BOARD         = 0x00000200;
const uint32 XBEIMAGE_MEDIA_TYPE_NONSECURE_HARD_DISK = 0x40000000;
const uint32 XBEIMAGE_MEDIA_TYPE_NONSECURE_MODE      = 0x80000000;
const uint32 XBEIMAGE_MEDIA_TYPE_MEDIA_MASK          = 0x00FFFFFF;

// section type flags for Xbe
const uint32 XBEIMAGE_SECTION_WRITEABLE				 = 0x00000001;
const uint32 XBEIMAGE_SECTION_PRELOAD				 = 0x00000002;
const uint32 XBEIMAGE_SECTION_EXECUTABLE			 = 0x00000004;
const uint32 XBEIMAGE_SECTION_INSERTFILE			 = 0x00000008;
const uint32 XBEIMAGE_SECTION_HEAD_PAGE_READONLY	 = 0x00000010;
const uint32 XBEIMAGE_SECTION_TAIL_PAGE_READONLY	 = 0x00000020;
#endif
