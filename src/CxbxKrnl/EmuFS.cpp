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
#define _XBOXKRNL_DEFEXTRN_

// prevent name collisions
namespace xboxkrnl
{
#include <xboxkrnl/xboxkrnl.h>
};

#include "EmuFS.h"
#include "EmuAlloc.h"
#include "CxbxKrnl.h"
#include "Exe.h"

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h>
#include <cstdio>

__declspec(naked) void EmuCmpEsiFs00()
{
	__asm
	{
		push eax
		mov eax, fs : [0x14]
		cmp esi, [eax]
		pop eax
		ret
	}
}

__declspec(naked) void EmuMEaxFs00()
{
	__asm
	{
		mov eax, fs : [0x14]
		mov eax, [eax]
			ret
	}
}

__declspec(naked) void EmuMEaxFs20()
{
	__asm
	{
		mov eax, fs : [0x14]
		mov eax, [eax + 20h]
			ret
	}
}

__declspec(naked) void EmuMEaxFs28()
{
	__asm
	{
		mov eax, fs : [0x14]
		mov eax, [eax + 28h]
			ret
	}
}

__declspec(naked) void EmuMEaxFs58()
{
	__asm
	{
		mov eax, fs : [0x14]
		mov eax, [eax + 58h]
			ret
	}
}

__declspec(naked) void EmuMEbxFs00()
{
	__asm
	{
		mov ebx, fs : [0x14]
		mov ebx, [ebx]
			ret
	}
}

__declspec(naked) void EmuMEcxFs00()
{
	__asm
	{
		mov ecx, fs : [0x14]
		mov ecx, [ecx]
			ret
	}
}

__declspec(naked) void EmuMEcxFs04()
{
	__asm
	{
		mov ecx, fs : [0x14]
		mov ecx, [ecx + 04h]
			ret
	}
}

__declspec(naked) void EmuMEdiFs00()
{
	__asm
	{
		mov edi, fs : [0x14]
		mov edi, [edi]
			ret
	}
}

__declspec(naked) void EmuMEdiFs04()
{
	__asm
	{
		mov edi, fs : [0x14]
		mov edi, [edi + 04h]
			ret
	}
}

__declspec(naked) void EmuMEsiFs00()
{
	__asm
	{
		mov esi, fs : [0x14]
		mov esi, [esi]
			ret
	}
}

__declspec(naked) void EmuMzxEaxBytePtrFs24()
{
	__asm
	{
		mov eax, fs : [0x14]
		movzx eax, byte ptr[eax + 24h]
			ret
	}
}

__declspec(naked) void EmuMFs00Eax()
{
	__asm
	{
		push ebx
		mov ebx, fs : [0x14]
			mov[ebx], eax
			pop ebx
			ret
	}
}

__declspec(naked) void EmuMFs00Ebx()
{
	__asm
	{
		push eax
		mov eax, fs : [0x14]
			mov[eax], ebx
			pop eax
			ret
	}
}

__declspec(naked) void EmuMFs00Ecx()
{
	__asm
	{
		push eax
		mov eax, fs : [0x14]
			mov[eax], ecx
			pop eax
			ret
	}
}

__declspec(naked) void EmuMFs00Esp()
{
	__asm
	{
		push eax
		mov eax, fs : [0x14]
			mov[eax], esp
			pop eax
			ret
	}
}

__declspec(naked) void EmuPushDwordPtrFs00()
{
	uint32 returnAddr;
	uint32 temp;
	__asm
	{
		pop returnAddr
		mov temp, eax
			mov eax, fs : [0x14]
			push[eax]
			mov eax, temp
			push returnAddr
			ret
	}
}

__declspec(naked) void EmuPopDwordPtrFs00()
{
	uint32 returnAddr;
	uint32 temp;

	__asm
	{
		pop returnAddr
		mov temp, eax
			mov eax, fs : [0x14]
			pop[eax]
			mov eax, temp
			push returnAddr
			ret
	}
}

// initialize fs segment selector emulation
void EmuInitFS()
{
	/**
	* Build the vector of FS instructions we need to intercept.
	* The entries must be in order of size, to keep the chance of false positives to a minimum.
	*/
	std::vector<fs_instruction_t> fsInstructions;
	fsInstructions.push_back({ { 0x64, 0x0F, 0xB6, 0x05, 0x24, 0x00, 0x00, 0x00 }, &EmuMzxEaxBytePtrFs24 });// movzx eax, large byte ptr fs:24
	fsInstructions.push_back({ { 0x64, 0x3B, 0x35, 0x00, 0x00, 0x00, 0x00 }, &EmuCmpEsiFs00 });				// cmp esi, large fs:0
	fsInstructions.push_back({ { 0x64, 0x8B, 0x1D, 0x00, 0x00, 0x00, 0x00 }, &EmuMEbxFs00 });				// mov ebx, large fs:0
	fsInstructions.push_back({ { 0x64, 0x8B, 0x0D, 0x00, 0x00, 0x00, 0x00 }, &EmuMEcxFs00 });				// mov ecx, large fs:0
	fsInstructions.push_back({ { 0x64, 0x8B, 0x0D, 0x04, 0x00, 0x00, 0x00 }, &EmuMEcxFs04 });				// mov ecx, large fs:4
	fsInstructions.push_back({ { 0x64, 0x8B, 0x3D, 0x00, 0x00, 0x00, 0x00 }, &EmuMEdiFs00 });				// mov edi, large fs:0
	fsInstructions.push_back({ { 0x64, 0x8B, 0x3D, 0x04, 0x00, 0x00, 0x00 }, &EmuMEdiFs04 });				// mov edi, large fs:4
	fsInstructions.push_back({ { 0x64, 0x8B, 0x35, 0x00, 0x00, 0x00, 0x00 }, &EmuMEsiFs00 });				// mov esi, large fs:0
	fsInstructions.push_back({ { 0x64, 0x89, 0x1D, 0x00, 0x00, 0x00, 0x00 }, &EmuMFs00Ebx });				// mov large fs:0, ebx
	fsInstructions.push_back({ { 0x64, 0x89, 0x0D, 0x00, 0x00, 0x00, 0x00 }, &EmuMFs00Ecx });				// mov large fs:0, ecx
	fsInstructions.push_back({ { 0x64, 0x89, 0x25, 0x00, 0x00, 0x00, 0x00 }, &EmuMFs00Esp });				// mov large fs:0, esp
	fsInstructions.push_back({ { 0x64, 0x8F, 0x05, 0x00, 0x00, 0x00, 0x00 }, &EmuPopDwordPtrFs00 });		// pop large dword ptr fs:0
	fsInstructions.push_back({ { 0x64, 0xFF, 0x35, 0x00, 0x00, 0x00, 0x00 }, &EmuPushDwordPtrFs00 });		// push large dword ptr fs:0
	fsInstructions.push_back({ { 0x64, 0xA1, 0x00, 0x00, 0x00, 0x00 }, &EmuMEaxFs00 });						// mov eax, large fs:0
	fsInstructions.push_back({ { 0x64, 0xA1, 0x20, 0x00, 0x00, 0x00 }, &EmuMEaxFs20 });						// mov eax, large fs:20
	fsInstructions.push_back({ { 0x64, 0xA1, 0x28, 0x00, 0x00, 0x00 }, &EmuMEaxFs28 });						// mov eax, large fs:28
	fsInstructions.push_back({ { 0x64, 0xA1, 0x58, 0x00, 0x00, 0x00 }, &EmuMEaxFs58 });						// mov eax, large fs:58
	fsInstructions.push_back({ { 0x64, 0xA3, 0x00, 0x00, 0x00, 0x00 }, &EmuMFs00Eax });						// mov large fs:0, eax

	DbgPrintf("Patching FS Register Accesses\n");
	DWORD sizeOfImage = CxbxKrnl_XbeHeader->dwSizeofImage;
	long numberOfInstructions = fsInstructions.size();

	// Iterate through each CODE section
	for (uint32 sectionIndex = 0; sectionIndex < CxbxKrnl_Xbe->m_Header.dwSections; sectionIndex++) {
		if (!CxbxKrnl_Xbe->m_SectionHeader[sectionIndex].dwFlags.bExecutable) {
			continue;
		}

		DbgPrintf("Searching for FS Instruction in section %s\n", CxbxKrnl_Xbe->m_szSectionName[sectionIndex]);
		uint32_t startAddr = CxbxKrnl_Xbe->m_SectionHeader[sectionIndex].dwVirtualAddr;
		uint32_t endAddr = startAddr + CxbxKrnl_Xbe->m_SectionHeader[sectionIndex].dwSizeofRaw;
		for (uint32 addr = startAddr; addr < endAddr; addr++)
		{
			for (int i = 0; i < numberOfInstructions; i++)
			{
				// Loop through the data, checking if we get an exact match
				long sizeOfData = fsInstructions[i].data.size();

				if (addr + sizeOfData >= endAddr)
				{
					continue;
				}

				if (memcmp((void*)addr, &fsInstructions[i].data[0], sizeOfData) == 0)
				{
					DbgPrintf("Patching FS Instruction at 0x%08X\n", addr);

					// Write Call opcode
					*(uint08*)addr = 0xE8;
					*(uint32*)(addr + 1) = (uint32)fsInstructions[i].functionPtr - addr - 5;

					// Fill the remaining bytes with nop instructions
					int remaining_bytes = fsInstructions[i].data.size() - 5;
					memset((void*)(addr + 5), 0x90, remaining_bytes);
					addr += sizeOfData - 1;
					break;
				}
			}
		}
	}
	
	DbgPrintf("Done\n");
}

// generate fs segment selector
void EmuGenerateFS(Xbe::TLS *pTLS, void *pTLSData)
{
	// Make sure the TLS Start and End addresses are within Xbox Memory
	if (pTLS->dwDataStartAddr > XBOX_MEMORY_SIZE || pTLS->dwDataEndAddr > XBOX_MEMORY_SIZE) {
		return;
	}

	NT_TIB         *OrgNtTib;
	xboxkrnl::KPCR *NewPcr;

	uint08 *pNewTLS = NULL;

	uint16 NewFS = -1, OrgFS = -1;

	// copy global TLS to the current thread
	{
		uint32 dwCopySize = pTLS->dwDataEndAddr - pTLS->dwDataStartAddr;
		uint32 dwZeroSize = pTLS->dwSizeofZeroFill;

		pNewTLS = (uint08*)CxbxMalloc(dwCopySize + dwZeroSize + 0x100 /* + HACK: extra safety padding 0x100*/);

		memset(pNewTLS, 0, dwCopySize + dwZeroSize + 0x100);
		memcpy(pNewTLS, pTLSData, dwCopySize);
	}

	// dump raw TLS data
	{
#ifdef _DEBUG_TRACE
		if (pNewTLS == 0)
		{
			DbgPrintf("EmuFS (0x%X): TLS Non-Existant (OK)\n", GetCurrentThreadId());
		}
		else
		{
			DbgPrintf("EmuFS (0x%X): TLS Data Dump...\n", GetCurrentThreadId());
			DbgPrintf("EmuFS (0x%X): 0x%.08X: ", GetCurrentThreadId(), pNewTLS);

			uint32 stop = pTLS->dwDataEndAddr - pTLS->dwDataStartAddr + pTLS->dwSizeofZeroFill;

			for (uint32 v = 0;v<stop;v++)
			{
				uint08 *bByte = (uint08*)pNewTLS + v;

				DbgPrintf("%.01X", (uint32)*bByte);

				if ((v + 1) % 0x10 == 0 && v + 1<stop)
					DbgPrintf("\nEmuFS (0x%X): 0x%.08X: ", GetCurrentThreadId(), ((uint32)pNewTLS + v));
			}

			DbgPrintf("\n");
		}
#endif
	}

	__asm
	{
		// Obtain "OrgFS"
		mov ax, fs
		mov OrgFS, ax

			// Obtain "OrgNtTib"
			mov eax, fs:[0x18]
			mov OrgNtTib, eax
	}

	// allocate KPCR structure
	{
		uint32 dwSize = sizeof(xboxkrnl::KPCR);

		NewPcr = (xboxkrnl::KPCR*)CxbxMalloc(dwSize);

		memset(NewPcr, 0, sizeof(*NewPcr));
	}

	// generate TIB
	xboxkrnl::ETHREAD *EThread = (xboxkrnl::ETHREAD*)CxbxMalloc(sizeof(xboxkrnl::ETHREAD));

	EThread->Tcb.TlsData = (void*)pNewTLS;
	EThread->UniqueThread = GetCurrentThreadId();

	memcpy(&NewPcr->NtTib, OrgNtTib, sizeof(NT_TIB));

	NewPcr->NtTib.Self = &NewPcr->NtTib;

	NewPcr->PrcbData.CurrentThread = (xboxkrnl::KTHREAD*)EThread;

	NewPcr->Prcb = &NewPcr->PrcbData;

	//  Set the stack base
	NewPcr->NtTib.StackBase = pNewTLS;

	// prepare TLS
	{
		// TLS Index Address := 0
		*(uint32*)pTLS->dwTLSIndexAddr = 0;

		// dword @ pTLSData := pTLSData
		if (pNewTLS != 0)
			*(void**)pNewTLS = pNewTLS;
	}

	// Store the new KPCR pointer in FS
	__asm {
		mov eax, NewPcr
			mov fs : [0x14], eax
	}

	DbgPrintf("EmuFS (0x%X): OrgFS=%d NewFS=%d pTLS=0x%.08X\n", GetCurrentThreadId(), OrgFS, NewFS, pTLS);
}
