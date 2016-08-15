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
#include "HLEIntercept.h"

#include <shlobj.h>
#include <vector>

uint32 g_BuildVersion;
uint32 g_OrigBuildVersion;

void EmuHLEIntercept(Xbe::LibraryVersion *pLibraryVersion, Xbe::Header *pXbeHeader)
{
    Xbe::Certificate *pCertificate = (Xbe::Certificate*)pXbeHeader->dwCertificateAddr;

	//
    // initialize openxdk emulation (TODO)
    //

    if(pLibraryVersion == 0)
    {
        DbgPrintf("HLE: Detected OpenXDK application...\n");
    }

    //
    // initialize Microsoft XDK emulation
    //

    if(pLibraryVersion != 0)
    {
        DbgPrintf("HLE: Detected Microsoft XDK application...\n");

        uint32 dwLibraryVersions = pXbeHeader->dwLibraryVersions;
    
    
		for (uint32 v = 0;v < dwLibraryVersions;v++)
		{
			uint16 MajorVersion = pLibraryVersion[v].wMajorVersion;
			uint16 MinorVersion = pLibraryVersion[v].wMinorVersion;
			uint16 BuildVersion = pLibraryVersion[v].wBuildVersion;
			uint16 OrigBuildVersion = BuildVersion;

			char szLibraryName[9] = { 0 };
			char szOrigLibraryName[9] = { 0 };

			for (uint32 c = 0;c < 8;c++)
			{
				szLibraryName[c] = pLibraryVersion[v].szName[c];
				szOrigLibraryName[c] = pLibraryVersion[v].szName[c];
			}

			if (strcmp("D3D8", szLibraryName) == 0)
			{
				// Save D3D8 build version
				g_BuildVersion = BuildVersion;
				g_OrigBuildVersion = OrigBuildVersion;
			}
		}

    }

    DbgPrintf("\n");

    return;
}

// install function interception wrapper
static inline void EmuInstallWrapper(void *FunctionAddr, void *WrapperAddr)
{
    uint08 *FuncBytes = (uint08*)FunctionAddr;

    *(uint08*)&FuncBytes[0] = 0xE9;
    *(uint32*)&FuncBytes[1] = (uint32)WrapperAddr - (uint32)FunctionAddr - 5;
}

// alert for the situation where an Xref function body is hit
static void EmuXRefFailure()
{
    _asm int 3;
    EmuSwapFS();    // Win2k/XP FS

    CxbxKrnlCleanup("XRef-only function body reached. Fatal Error.");
}

