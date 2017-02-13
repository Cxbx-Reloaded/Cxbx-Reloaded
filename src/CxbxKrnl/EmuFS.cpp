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
#include "EmuAlloc.h" // For CxbxCalloc()
#include "CxbxKrnl.h"
#include "Exe.h"

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h>
#include <cstdio>

NT_TIB *GetNtTib()
{
	NT_TIB *NtTib;

	__asm
	{
		mov eax, fs : [TIB_LinearSelfAddress]
		mov NtTib, eax
	}

	return NtTib;
}

void EmuKeSetPcr(xboxkrnl::KPCR *Pcr)
{
	// Store the Xbox KPCR pointer in FS (See KeGetPcr())
	// TODO : Explain why this can't just be a global (or can it?)
	__asm {
		mov eax, Pcr
		mov fs : [TIB_ArbitraryDataSlot], eax
	}
}

__declspec(naked) void EmuCmpEsiFs00()
{
	// Note : eax must be preserved here, hence the push/pop
	__asm
	{
		push eax
		mov eax, fs : [TIB_ArbitraryDataSlot]
		cmp esi, [eax]
		pop eax
		ret
	}
}

__declspec(naked) void EmuMEaxFs00()
{
	__asm
	{
		mov eax, fs : [TIB_ArbitraryDataSlot]
		mov eax, [eax]
		ret
	}
}

__declspec(naked) void EmuMEaxFs20()
{
	__asm
	{
		mov eax, fs : [TIB_ArbitraryDataSlot]
		mov eax, [eax + 20h]
		ret
	}
}

__declspec(naked) void EmuMEaxFs28()
{
	__asm
	{
		mov eax, fs : [TIB_ArbitraryDataSlot]
		mov eax, [eax + 28h]
		ret
	}
}

__declspec(naked) void EmuMEaxFs58()
{
	__asm
	{
		mov eax, fs : [TIB_ArbitraryDataSlot]
		mov eax, [eax + 58h]
		ret
	}
}

__declspec(naked) void EmuMEbxFs00()
{
	__asm
	{
		mov ebx, fs : [TIB_ArbitraryDataSlot]
		mov ebx, [ebx]
		ret
	}
}

__declspec(naked) void EmuMEcxFs00()
{
	__asm
	{
		mov ecx, fs : [TIB_ArbitraryDataSlot]
		mov ecx, [ecx]
		ret
	}
}

__declspec(naked) void EmuMEcxFs04()
{
	__asm
	{
		mov ecx, fs : [TIB_ArbitraryDataSlot]
		mov ecx, [ecx + 04h]
		ret
	}
}

__declspec(naked) void EmuMEdiFs00()
{
	__asm
	{
		mov edi, fs : [TIB_ArbitraryDataSlot]
		mov edi, [edi]
		ret
	}
}

__declspec(naked) void EmuMEdiFs04()
{
	__asm
	{
		mov edi, fs : [TIB_ArbitraryDataSlot]
		mov edi, [edi + 04h]
		ret
	}
}

__declspec(naked) void EmuMEsiFs00()
{
	__asm
	{
		mov esi, fs : [TIB_ArbitraryDataSlot]
		mov esi, [esi]
		ret
	}
}

__declspec(naked) void EmuMzxEaxBytePtrFs24()
{
	__asm
	{
		mov eax, fs : [TIB_ArbitraryDataSlot]
		movzx eax, byte ptr[eax + 24h]
		ret
	}
}

__declspec(naked) void EmuMFs00Eax()
{
	// Note : ebx must be preserved here, hence the push/pop
	__asm
	{
		push ebx
		mov ebx, fs : [TIB_ArbitraryDataSlot]
		mov [ebx], eax
		pop ebx
		ret
	}
}

__declspec(naked) void EmuMFs00Ebx()
{
	// Note : eax must be preserved here, hence the push/pop
	__asm
	{
		push eax
		mov eax, fs : [TIB_ArbitraryDataSlot]
		mov [eax], ebx
		pop eax
		ret
	}
}

__declspec(naked) void EmuMFs00Ecx()
{
	// Note : eax must be preserved here, hence the push/pop
	__asm
	{
		push eax
		mov eax, fs : [TIB_ArbitraryDataSlot]
		mov [eax], ecx
		pop eax
		ret
	}
}

__declspec(naked) void EmuMFs00Esp()
{
	// Note : eax must be preserved here, hence the push/pop
	__asm
	{
		push eax
		mov eax, fs : [TIB_ArbitraryDataSlot]
		mov [eax], esp
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
		mov eax, fs : [TIB_ArbitraryDataSlot]
		push [eax]
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
		mov eax, fs : [TIB_ArbitraryDataSlot]
		pop [eax]
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
		xbaddr startAddr = CxbxKrnl_Xbe->m_SectionHeader[sectionIndex].dwVirtualAddr;
		xbaddr endAddr = startAddr + CxbxKrnl_Xbe->m_SectionHeader[sectionIndex].dwSizeofRaw;
		for (xbaddr addr = startAddr; addr < endAddr; addr++)
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
					*(uint08*)addr = OPCODE_CALL_E8;
					*(uint32*)(addr + 1) = (uint32)fsInstructions[i].functionPtr - addr - 5;

					// Fill the remaining bytes with nop instructions
					int remaining_bytes = fsInstructions[i].data.size() - 5;
					memset((void*)(addr + 5), OPCODE_NOP_90, remaining_bytes);
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
	if (pTLS->dwDataStartAddr >= EMU_MAX_MEMORY_SIZE || pTLS->dwDataEndAddr >= EMU_MAX_MEMORY_SIZE) {
		return;
	}

	xboxkrnl::KPCR *NewPcr;
	uint08 *pNewTLS = nullptr;

	// copy global TLS to the current thread
	{
		uint32 dwCopySize = pTLS->dwDataEndAddr - pTLS->dwDataStartAddr;
		uint32 dwZeroSize = pTLS->dwSizeofZeroFill;

		pNewTLS = (uint08*)CxbxCalloc(1, dwCopySize + dwZeroSize + 0x100 /* + HACK: extra safety padding 0x100*/);

		memcpy(pNewTLS, pTLSData, dwCopySize);

#ifdef _DEBUG_TRACE
		// dump raw TLS data
		if (pNewTLS == nullptr)
			DbgPrintf("EmuFS: TLS Non-Existant (OK)\n");
		else
		{
			DbgPrintf("EmuFS: TLS Data Dump...\n");
			if (g_bPrintfOn)
			{
				for (uint32 v = 0; v < dwCopySize; v++) // Note : Don't dump dwZeroSize
				{
					uint08 *bByte = (uint08*)pNewTLS + v;

					if (v % 0x10 == 0)
						DbgPrintf("EmuFS: 0x%.08X: ", (xbaddr)bByte);

					// Note : Use printf instead of DbgPrintf here, which prefixes with GetCurrentThreadId() :
					printf("%.01X", (uint32)(*bByte));
				}

				printf("\n");
			}
		}
#endif
	}

	// prepare TLS
	{
		*(xbaddr*)pTLS->dwTLSIndexAddr = (xbaddr)nullptr;

		// dword @ pTLSData := pTLSData
		if (pNewTLS != nullptr)
			*(void**)pNewTLS = pNewTLS;
	}

	// Allocate the xbox KPCR structure
	NewPcr = (xboxkrnl::KPCR*)CxbxCalloc(1, sizeof(xboxkrnl::KPCR));

	// Copy the Nt TIB over to the emulated TIB :
	{
		memcpy(&NewPcr->NtTib, GetNtTib(), sizeof(NT_TIB));
		// Fixup the TIB self pointer :
		NewPcr->NtTib.Self = &NewPcr->NtTib;
		// Set the stack base - TODO : Verify this, doesn't look right?
		NewPcr->NtTib.StackBase = pNewTLS;
	}
	
	// Set flat address of this PCR
	NewPcr->SelfPcr = NewPcr;
	// Set pointer to Prcb
	NewPcr->Prcb = &NewPcr->PrcbData;
	// TODO : Should Irql be set? And if so, to what - PASSIVE_LEVEL?
	// NewPcr->Irql = PASSIVE_LEVEL;

	// Initialize a fake PrcbData.CurrentThread 
	{
		xboxkrnl::ETHREAD *EThread = (xboxkrnl::ETHREAD*)CxbxCalloc(1, sizeof(xboxkrnl::ETHREAD)); // Clear, to prevent side-effects on random contents

		EThread->Tcb.TlsData = (void*)pNewTLS;
		EThread->UniqueThread = GetCurrentThreadId();
		// Set PrcbData.CurrentThread
		NewPcr->PrcbData.CurrentThread = (xboxkrnl::KTHREAD*)EThread;
	}

	// Make the KPCR struct available to KeGetPcr()
	EmuKeSetPcr(NewPcr);

	DbgPrintf("EmuFS: Installed KPCR in TIB_ArbitraryDataSlot (with pTLS = 0x%.08X)\n", pTLS);
}
