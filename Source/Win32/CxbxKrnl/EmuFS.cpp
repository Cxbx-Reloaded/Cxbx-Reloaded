// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuFS.cpp
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
#define _XBOXKRNL_LOCAL_

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace xboxkrnl
{
    #include <xboxkrnl/xboxkrnl.h>
};

#include "EmuFS.h"
#include "EmuLDT.h"

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h>
#include <stdio.h>

// ******************************************************************
// * data: EmuAutoSleepRate
// ******************************************************************
uint32 EmuAutoSleepRate = -1;

// ******************************************************************
// * func: EmuInitFS
// ******************************************************************
void EmuInitFS()
{
    EmuInitLDT();
}

// ******************************************************************
// * func: EmuGenerateFS
// ******************************************************************
void EmuGenerateFS(Xbe::TLS *pTLS, void *pTLSData)
{
    NT_TIB         *OrgNtTib;
    xboxkrnl::KPCR *NewPcr;

    uint08 *pNewTLS = NULL;

    uint16 NewFS = -1, OrgFS = -1;

    // ******************************************************************
    // * Copy Global TLS to Local
    // ******************************************************************
    {
		uint32 dwCopySize = pTLS->dwDataEndAddr - pTLS->dwDataStartAddr;
        uint32 dwZeroSize = pTLS->dwSizeofZeroFill;

        pNewTLS = new uint08[dwCopySize + dwZeroSize];

        memcpy(pNewTLS, pTLSData, dwCopySize);

		ZeroMemory(pNewTLS + dwCopySize, dwZeroSize);
    }

    // ******************************************************************
    // * Dump Raw TLS data
    // ******************************************************************
    {
        #ifdef _DEBUG_TRACE
		if(pNewTLS == 0)
		{
			printf("EmuFS (0x%X): TLS Non-Existant (OK)\n", GetCurrentThreadId());
		}
		else
		{
			printf("EmuFS (0x%X): TLS Data Dump... \n  0x%.08X: ", GetCurrentThreadId(), pNewTLS);

			uint32 stop = pTLS->dwDataEndAddr - pTLS->dwDataStartAddr + pTLS->dwSizeofZeroFill;

			for(uint32 v=0;v<stop;v++)
			{
				uint08 *bByte = (uint08*)pNewTLS + v;

				printf("%.01X", (uint32)*bByte);

				if((v+1) % 0x10 == 0)
					printf("\n  0x%.08X: ", ((uint32)pNewTLS + v));
			}

			printf("\n");
		}
        #endif
    }

    // ******************************************************************
    // * Obtain "OrgFS"
    // ******************************************************************
    __asm
    {
        // Obtain "OrgFS"
        mov ax, fs
        mov OrgFS, ax

        // Obtain "OrgNtTib"
        mov eax, fs:[0x18]
        mov OrgNtTib, eax
    }

    // ******************************************************************
    // * Allocate LDT entry
    // ******************************************************************
    {
        uint32 dwSize = sizeof(xboxkrnl::KPCR);

        NewPcr = (xboxkrnl::KPCR*)new char[dwSize];

        memset(NewPcr, 0, sizeof(*NewPcr));

        NewFS = EmuAllocateLDT((uint32)NewPcr, (uint32)NewPcr + dwSize);
    }

    // ******************************************************************
    // * Update "OrgFS" with NewFS and (bIsBoxFs = false)
    // ******************************************************************
    {
        __asm
        {
            mov ax, NewFS
            mov bh, 0

            mov fs:[0x14], ax
            mov fs:[0x16], bh
        }
    }

    // ******************************************************************
    // * Generate TIB
    // ******************************************************************
    {
        xboxkrnl::ETHREAD *EThread = new xboxkrnl::ETHREAD();

        EThread->Tcb.TlsData  = (void*)pNewTLS;
        EThread->UniqueThread = GetCurrentThreadId();

        memcpy(&NewPcr->NtTib, OrgNtTib, sizeof(NT_TIB));

        NewPcr->NtTib.Self = &NewPcr->NtTib;

        NewPcr->PrcbData.CurrentThread = (xboxkrnl::KTHREAD*)EThread;

        NewPcr->Prcb = &NewPcr->PrcbData;
    }

    // ******************************************************************
    // * Prepare TLS
    // ******************************************************************
    {
        // TLS Index Address := 0
        *(uint32*)pTLS->dwTLSIndexAddr = 0;

        // dword @ pTLSData := pTLSData
        if(pNewTLS != 0)
            *(void**)pNewTLS = pNewTLS;
    }

    // ******************************************************************
    // * Swap into the "NewFS"
    // ******************************************************************
    EmuSwapFS();

    // ******************************************************************
    // * Update "NewFS" with OrgFS and (bIsBoxFs = true)
    // ******************************************************************
    {
        __asm
        {
            mov ax, OrgFS
            mov bh, 1

            mov fs:[0x14], ax
            mov fs:[0x16], bh
        }
    }

    // ******************************************************************
    // * Save "TLSPtr" inside NewFS.StackBase
    // ******************************************************************
    {
        __asm
        {
            mov eax, pNewTLS
            mov fs:[0x04], eax
        }
    }

    // ******************************************************************
    // * Swap back into the "OrgFS"
    // ******************************************************************
    EmuSwapFS();

    // ******************************************************************
    // * Debug output
    // ******************************************************************
	#ifdef _DEBUG_TRACE
    printf("EmuFS (0x%X): OrgFS=%d NewFS=%d pTLS=0x%.08X\n", GetCurrentThreadId(), OrgFS, NewFS, pTLS);
	#endif
}

// ******************************************************************
// * func: EmuCleanupFS
// ******************************************************************
void EmuCleanupFS()
{
    uint16 wSwapFS = 0;

    __asm
    {
        mov ax, fs:[0x14]   // FS.ArbitraryUserPointer
        mov wSwapFS, ax
    }

    if(wSwapFS == 0)
        return;

    if(!EmuIsXboxFS())
        EmuSwapFS();    // Xbox FS

    uint08 *pTLSData = NULL;

    __asm
    {
        mov eax, fs:[0x04]
        mov pTLSData, eax
    }

    EmuSwapFS(); // Win2k/XP FS

    if(pTLSData != 0)
        delete[] pTLSData;

    EmuDeallocateLDT(wSwapFS);
}