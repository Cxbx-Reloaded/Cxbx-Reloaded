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

#include "EmuKrnl.h" // For InitializeListHead(), etc.
#include "EmuFS.h"
#include "EmuAlloc.h" // For CxbxCalloc()
#include "CxbxKrnl.h"
#include "MemoryManager.h"

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
	// 
	// Note : Cxbx currently doesn't do preemptive thread switching,
	// which implies that thread-state management is done by Windows.
	//
	// Xbox executable code expects thread-specific state data to
	// be available via the FS segment register. To emulate this,
	// Cxbx uses the user data-slot feature of Windows threads.
	// 
	// Cxbx puts a pointer to a thread-specific copy of an entire
	// Kernel Processor Control Region (KPCR) into this data-slot.
	//
	// In the Xbox there's only be KPCR (as it's a per-processor-
	// structure, and the Xbox has only one processor).
	//
	// Since Cxbx doesn't control thread-swiches (yet), each thread
	// must have a thread-specific copy of the KPCR, to contain all
	// thread-specific data that can be reached via this structure
	// (like the NT_TIB structure and ETHREAD CurrentThread pointer).
	// 
	// For this to work, Cxbx patches all executable code accessing
	// the FS segment register, so that the KPCR is accessed via
	// the user data-slot of each Windows thread Cxbx uses for an
	// Xbox thread.
	//
	__asm {
		mov eax, Pcr
		mov fs : [TIB_ArbitraryDataSlot], eax
	}
}

__declspec(naked) void EmuFS_CmpEsiFs00()
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

__declspec(naked) void EmuFS_MovEaxFs00()
{
	__asm
	{
		mov eax, fs : [TIB_ArbitraryDataSlot]
		mov eax, [eax]
		ret
	}
}

__declspec(naked) void EmuFS_MovEaxFs20()
{
	__asm
	{
		mov eax, fs : [TIB_ArbitraryDataSlot]
		mov eax, [eax + 20h]
		ret
	}
}

__declspec(naked) void EmuFS_MovEaxFs28()
{
	__asm
	{
		mov eax, fs : [TIB_ArbitraryDataSlot]
		mov eax, [eax + 28h]
		ret
	}
}

__declspec(naked) void EmuFS_MovEaxFs58()
{
	__asm
	{
		mov eax, fs : [TIB_ArbitraryDataSlot]
		mov eax, [eax + 58h]
		ret
	}
}

__declspec(naked) void EmuFS_MovEbxFs00()
{
	__asm
	{
		mov ebx, fs : [TIB_ArbitraryDataSlot]
		mov ebx, [ebx]
		ret
	}
}

__declspec(naked) void EmuFS_MovEcxFs00()
{
	__asm
	{
		mov ecx, fs : [TIB_ArbitraryDataSlot]
		mov ecx, [ecx]
		ret
	}
}

__declspec(naked) void EmuFS_MovEcxFs04()
{
	__asm
	{
		mov ecx, fs : [TIB_ArbitraryDataSlot]
		mov ecx, [ecx + 04h]
		ret
	}
}

__declspec(naked) void EmuFS_MovEdiFs00()
{
	__asm
	{
		mov edi, fs : [TIB_ArbitraryDataSlot]
		mov edi, [edi]
		ret
	}
}

__declspec(naked) void EmuFS_MovEdiFs04()
{
	__asm
	{
		mov edi, fs : [TIB_ArbitraryDataSlot]
		mov edi, [edi + 04h]
		ret
	}
}

__declspec(naked) void EmuFS_MovEsiFs00()
{
	__asm
	{
		mov esi, fs : [TIB_ArbitraryDataSlot]
		mov esi, [esi]
		ret
	}
}

__declspec(naked) void EmuFS_MovzxEaxBytePtrFs24()
{
	__asm
	{
		mov eax, fs : [TIB_ArbitraryDataSlot]
		movzx eax, byte ptr[eax + 24h]
		ret
	}
}

__declspec(naked) void EmuFS_MovFs00Eax()
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

__declspec(naked) void EmuFS_MovFs00Ebx()
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

__declspec(naked) void EmuFS_MovFs00Ecx()
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

__declspec(naked) void EmuFS_MovFs00Esp()
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

__declspec(naked) void EmuFS_PushDwordPtrFs00()
{
	uint32 returnAddr;
	uint32 temp;

	__asm
	{
		pop returnAddr
		mov temp, eax
		mov eax, fs : [TIB_ArbitraryDataSlot]
		push dword ptr [eax]
		mov eax, temp
		push returnAddr
		ret
	}
}

__declspec(naked) void EmuFS_PopDwordPtrFs00()
{
	uint32 returnAddr;
	uint32 temp;

	__asm
	{
		pop returnAddr
		mov temp, eax
		mov eax, fs : [TIB_ArbitraryDataSlot]
		pop dword ptr [eax]
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
	fsInstructions.push_back({ { 0x64, 0x0F, 0xB6, 0x05, 0x24, 0x00, 0x00, 0x00 }, &EmuFS_MovzxEaxBytePtrFs24 });// movzx eax, large byte ptr fs:24
	fsInstructions.push_back({ { 0x64, 0x3B, 0x35, 0x00, 0x00, 0x00, 0x00 }, &EmuFS_CmpEsiFs00 });				// cmp esi, large fs:0
	fsInstructions.push_back({ { 0x64, 0x8B, 0x1D, 0x00, 0x00, 0x00, 0x00 }, &EmuFS_MovEbxFs00 });				// mov ebx, large fs:0
	fsInstructions.push_back({ { 0x64, 0x8B, 0x0D, 0x00, 0x00, 0x00, 0x00 }, &EmuFS_MovEcxFs00 });				// mov ecx, large fs:0
	fsInstructions.push_back({ { 0x64, 0x8B, 0x0D, 0x04, 0x00, 0x00, 0x00 }, &EmuFS_MovEcxFs04 });				// mov ecx, large fs:4
	fsInstructions.push_back({ { 0x64, 0x8B, 0x3D, 0x00, 0x00, 0x00, 0x00 }, &EmuFS_MovEdiFs00 });				// mov edi, large fs:0
	fsInstructions.push_back({ { 0x64, 0x8B, 0x3D, 0x04, 0x00, 0x00, 0x00 }, &EmuFS_MovEdiFs04 });				// mov edi, large fs:4
	fsInstructions.push_back({ { 0x64, 0x8B, 0x35, 0x00, 0x00, 0x00, 0x00 }, &EmuFS_MovEsiFs00 });				// mov esi, large fs:0
	fsInstructions.push_back({ { 0x64, 0x89, 0x1D, 0x00, 0x00, 0x00, 0x00 }, &EmuFS_MovFs00Ebx });				// mov large fs:0, ebx
	fsInstructions.push_back({ { 0x64, 0x89, 0x0D, 0x00, 0x00, 0x00, 0x00 }, &EmuFS_MovFs00Ecx });				// mov large fs:0, ecx
	fsInstructions.push_back({ { 0x64, 0x89, 0x25, 0x00, 0x00, 0x00, 0x00 }, &EmuFS_MovFs00Esp });				// mov large fs:0, esp
	fsInstructions.push_back({ { 0x64, 0x8F, 0x05, 0x00, 0x00, 0x00, 0x00 }, &EmuFS_PopDwordPtrFs00 });		    // pop large dword ptr fs:0
	fsInstructions.push_back({ { 0x64, 0xFF, 0x35, 0x00, 0x00, 0x00, 0x00 }, &EmuFS_PushDwordPtrFs00 });		// push large dword ptr fs:0
	fsInstructions.push_back({ { 0x64, 0xA1, 0x00, 0x00, 0x00, 0x00 }, &EmuFS_MovEaxFs00 });					// mov eax, large fs:0
	fsInstructions.push_back({ { 0x64, 0xA1, 0x20, 0x00, 0x00, 0x00 }, &EmuFS_MovEaxFs20 });					// mov eax, large fs:20
	fsInstructions.push_back({ { 0x64, 0xA1, 0x28, 0x00, 0x00, 0x00 }, &EmuFS_MovEaxFs28 });					// mov eax, large fs:28
	fsInstructions.push_back({ { 0x64, 0xA1, 0x58, 0x00, 0x00, 0x00 }, &EmuFS_MovEaxFs58 });					// mov eax, large fs:58
	fsInstructions.push_back({ { 0x64, 0xA3, 0x00, 0x00, 0x00, 0x00 }, &EmuFS_MovFs00Eax });					// mov large fs:0, eax

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
	void *pNewTLS = nullptr;

	// Be aware that TLS might be absent (for example in homebrew "Wolf3d-xbox")
	if (pTLS != nullptr) {
		// copy global TLS to the current thread
		{
			uint32 dwCopySize = 0;
			uint32 dwZeroSize = pTLS->dwSizeofZeroFill;

			if (pTLSData != NULL) {
				// Make sure the TLS Start and End addresses are within Xbox virtual memory
				if (pTLS->dwDataStartAddr >= XBE_MAX_VA || pTLS->dwDataEndAddr >= XBE_MAX_VA) {
					// ignore
				}
				else {
					dwCopySize = pTLS->dwDataEndAddr - pTLS->dwDataStartAddr;
				}
			}

			pNewTLS = g_MemoryManager.AllocateZeroed(1, dwCopySize + dwZeroSize + 0x100 /* + HACK: extra safety padding 0x100*/);

			if (dwCopySize > 0) {
				memcpy(pNewTLS, pTLSData, dwCopySize);
			}

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
	}

	// Allocate the xbox KPCR structure
	xboxkrnl::KPCR *NewPcr = (xboxkrnl::KPCR*)g_MemoryManager.AllocateZeroed(1, sizeof(xboxkrnl::KPCR));
	xboxkrnl::NT_TIB *XbTib = &(NewPcr->NtTib);
	xboxkrnl::PKPRCB Prcb = &(NewPcr->PrcbData);
	// Note : As explained above (at EmuKeSetPcr), Cxbx cannot allocate one NT_TIB and KPRCB
	// structure per thread, since Cxbx currently doesn't do thread-switching.
	// Thus, the only way to give each thread it's own PrcbData.CurrentThread, is to put the
	// KPCR pointer in the TIB_ArbitraryDataSlot, which is read by the above EmuFS_* patches.
	//
	// Once we simulate thread switching ourselves, we can update PrcbData.CurrentThread
	// and simplify this initialization, by using only one KPCR for the single Xbox processor.
	// 
	// One way to do our own (preemprive) thread-switching would be to use this technique :
	// http://www.eran.io/implementing-a-preemptive-kernel-within-a-single-windows-thread/
	// See https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/146 for more info.

	// Copy the Nt TIB over to the emulated TIB :
	{
		memcpy(XbTib, GetNtTib(), sizeof(NT_TIB));
		// Fixup the TIB self pointer :
		NewPcr->NtTib.Self = XbTib;
		// Set the stack base - TODO : Verify this, doesn't look right?
		NewPcr->NtTib.StackBase = pNewTLS;
	}

	// Set flat address of this PCR
	NewPcr->SelfPcr = NewPcr;
	// Set pointer to Prcb
	NewPcr->Prcb = Prcb;

	// Initialize the prcb :
	{
		// TODO : Once we do our own thread-switching (as mentioned above),
		// we can also start using Prcb->DpcListHead instead of DpcQueue :
		InitializeListHead(&(Prcb->DpcListHead));
		Prcb->DpcRoutineActive = 0;

		// TODO : Should Irql be set? And if so, to what - PASSIVE_LEVEL, or perhaps better : APC_LEVEL?
		// NewPcr->Irql = PASSIVE_LEVEL; // See KeLowerIrql;
	}

	// Initialize a fake PrcbData.CurrentThread 
	{
		xboxkrnl::ETHREAD *EThread = (xboxkrnl::ETHREAD*)g_MemoryManager.AllocateZeroed(1, sizeof(xboxkrnl::ETHREAD)); // Clear, to prevent side-effects on random contents

		EThread->Tcb.TlsData = pNewTLS;
		EThread->UniqueThread = GetCurrentThreadId();
		// Set PrcbData.CurrentThread
		Prcb->CurrentThread = (xboxkrnl::KTHREAD*)EThread;
	}

	// Make the KPCR struct available to KeGetPcr()
	EmuKeSetPcr(NewPcr);

	DbgPrintf("EmuFS: Installed KPCR in TIB_ArbitraryDataSlot (with pTLS = 0x%.08X)\n", pTLS);
}
