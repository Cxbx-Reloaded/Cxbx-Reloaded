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
// *   Cxbx->Win32->CxbxKrnl->EmuX86.cpp
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
// *  (c) 2016 Luke Usher <luke.usher@outlook.com>
// *  All rights reserved
// *
// ******************************************************************
#define _XBOXKRNL_DEFEXTRN_

#define LOG_PREFIX "X86 " // Intentional extra space to align on 4 characters
#include <unicorn\unicorn.h>

#include "CxbxKrnl.h"
#include "Emu.h" // For EmuWarning
#include "EmuX86.h"
#include "HLEIntercept.h" // for bLLE_GPU

#include <assert.h>
#include "devices\Xbox.h" // For g_PCIBus

extern uint32_t GetAPUTime();

//
// Read & write handlers handlers for I/O
//

static int field_pin = 0;

uint32_t EmuX86_IORead(xbaddr addr, int size)
{
	switch (addr) {
	case 0x8008: { // TODO : Move 0x8008 TIMER to a device
		if (size == sizeof(uint32_t)) {
			// HACK: This is very wrong.
			// This timer should count at a specific frequency (3579.545 ticks per ms)
			// But this is enough to keep NXDK from hanging for now.
			LARGE_INTEGER performanceCount;
			QueryPerformanceCounter(&performanceCount);
			return static_cast<uint32_t>(performanceCount.QuadPart);
		}
		break;
	}
	case 0x80C0: { // TODO : Move 0x80C0 TV encoder to a device
		if (size == sizeof(uint8_t)) {
			// field pin from tv encoder?
			field_pin = (field_pin + 1) & 1;
			return field_pin << 5;
		}
		break;
	}
	}

	// Pass the IO Read to the PCI Bus, this will handle devices with BARs set to IO addresses
	uint32_t value = 0;
	if (g_PCIBus->IORead(addr, &value, size)) {
		return value;
	}

	EmuWarning("EmuX86_IORead(0x%08X, %d) [Unhandled]", addr, size);
	return 0;
}

void EmuX86_IOWrite(xbaddr addr, uint32_t value, int size)
{
	// Pass the IO Write to the PCI Bus, this will handle devices with BARs set to IO addresses
	if (g_PCIBus->IOWrite(addr, value, size)) {
		return;
	}

	EmuWarning("EmuX86_IOWrite(0x%08X, 0x%04X, %d) [Unhandled]", addr, value, size);
}

//
// Read & write handlers for pass-through access to (host committed, virtual) xbox memory
//
// Only allowed to be called outside our EmuException exception handler,
// to prevent recursive exceptions when accessing unallocated memory.
//

uint32_t EmuX86_Mem_Read(xbaddr addr, int size)
{
	switch (size) {
	case sizeof(uint32_t) :
		return *(uint32_t*)addr;
	case sizeof(uint16_t) :
		return *(uint16_t*)addr;
	case sizeof(uint8_t) :
		return *(uint8_t*)addr;
	default:
		// UNREACHABLE(size);
		return 0;
	}
}

void EmuX86_Mem_Write(xbaddr addr, uint32_t value, int size)
{
	switch (size) {
	case sizeof(uint32_t) :
		*(uint32_t*)addr = (uint32_t)value;
		break;
	case sizeof(uint16_t) :
		*(uint16_t*)addr = (uint16_t)value;
		break;
	case sizeof(uint8_t) :
		*(uint8_t*)addr = (uint8_t)value;
		break;
	default:
		// UNREACHABLE(size);
		return;
	}
}

uint32_t EmuFlash_Read32(xbaddr addr) // TODO : Move to EmuFlash.cpp
{
	uint32_t r;

	switch (addr) {
	case 0x78: // ROM_VERSION
		r = 0x90; // Luke's hardware revision 1.6 Xbox returns this (also since XboxKrnlVersion is set to 5838)
		break;
	default:
		EmuWarning("Read32 FLASH_ROM (0x%.8X) [Unknown address]", addr);
		return -1;
	}

	DbgPrintf("X86 : Read32 FLASH_ROM (0x%.8X) = 0x%.8X [HANDLED]\n", addr, r);
	return r;
}

//
// Read & write handlers for memory-mapped hardware devices
//

uint32_t EmuX86_Read(xbaddr addr, int size)
{
	if ((addr & (size - 1)) != 0) {
		EmuWarning("EmuX86_Read(0x%08X, %d) [Unaligned unimplemented]", addr, size);
		// LOG_UNIMPLEMENTED();
		return 0;
	}

	uint32_t value;

	if (addr >= XBOX_FLASH_ROM_BASE) { // 0xFFF00000 - 0xFFFFFFF
		value = EmuFlash_Read32(addr - XBOX_FLASH_ROM_BASE); // TODO : Make flash access size-aware
	} else if(addr == 0xFE80200C) {
		// TODO: Remove this once we have an LLE APU Device
		return GetAPUTime();
	} else {
		// Pass the Read to the PCI Bus, this will handle devices with BARs set to MMIO addresses
		if (g_PCIBus->MMIORead(addr, &value, size)) {
			return value;
		}

		if (g_bEmuException) {
			EmuWarning("EmuX86_Read(0x%08X, %d) [Unknown address]", addr, size);
			value = 0;
		} else {
			// Outside EmuException, pass the memory-access through to normal memory :
			value = EmuX86_Mem_Read(addr, size);
		}

		DbgPrintf("X86 : Read(0x%08X, %d) = 0x%08X\n", addr, size, value);
	}

	return value;
}

void EmuX86_Write(xbaddr addr, uint32_t value, int size)
{
	if ((addr & (size - 1)) != 0) {
		EmuWarning("EmuX86_Write(0x%08X, 0x%08X, %d) [Unaligned unimplemented]", addr, value, size);
		// LOG_UNIMPLEMENTED();
		return;
	}

	if (addr >= XBOX_FLASH_ROM_BASE) { // 0xFFF00000 - 0xFFFFFFF
		EmuWarning("EmuX86_Write(0x%08X, 0x%08X) [FLASH_ROM]", addr, value);
		return;
	}

	// Pass the Write to the PCI Bus, this will handle devices with BARs set to MMIO addresses
	if (g_PCIBus->MMIOWrite(addr, value, size)) {
		return;
	}

	if (g_bEmuException) {
		EmuWarning("EmuX86_Write(0x%08X, 0x%08X) [Unknown address]", addr, value);
		return;
	}

	// Outside EmuException, pass the memory-access through to normal memory :
	DbgPrintf("X86 : Write(0x%.8X, 0x%.8X, %d)\n", addr, value, size);
	EmuX86_Mem_Write(addr, value, size);
}

// Unicorn MMIO->EmuX86 Wrappers
static uint64_t read_cb(struct uc_struct* uc, void *opaque, uint64_t addr, unsigned size) 
{
	return EmuX86_Read(addr + 0xFD000000, size);
}

static void write_cb(struct uc_struct* uc, void *opaque, uint64_t addr, uint64_t data, unsigned size)
{
	EmuX86_Write(addr + 0xFD000000, (uint32_t)data, size);
}

// Unicorn IO->EmuX86 Wrappers
static uint32_t hook_in_cb(uc_engine *uc, uint32_t port, int size, void *user_data)
{
	return EmuX86_IORead(port, size);
}

static void hook_out_cb(uc_engine *uc, uint32_t port, int size, uint32_t value, void *user_data)
{
	EmuX86_IOWrite(port, value, size);
}

uc_engine* EmuX86_Init();
bool EmuX86_DecodeException(LPEXCEPTION_POINTERS e)
{
	// Only decode instructions which reside in the loaded Xbe
	if (e->ContextRecord->Eip > XBE_MAX_VA || e->ContextRecord->Eip < XBE_IMAGE_BASE) {
		return false;
	}

	uc_engine* uc = EmuX86_Init();

	// Sync CPU state to Unicorn
	uc_reg_write(uc, UC_X86_REG_EDI, &e->ContextRecord->Edi);
	uc_reg_write(uc, UC_X86_REG_ESI, &e->ContextRecord->Esi);
	uc_reg_write(uc, UC_X86_REG_EBX, &e->ContextRecord->Ebx);
	uc_reg_write(uc, UC_X86_REG_EDX, &e->ContextRecord->Edx);
	uc_reg_write(uc, UC_X86_REG_ECX, &e->ContextRecord->Ecx);
	uc_reg_write(uc, UC_X86_REG_EAX, &e->ContextRecord->Eax);
	uc_reg_write(uc, UC_X86_REG_EIP, &e->ContextRecord->Eip);
	uc_reg_write(uc, UC_X86_REG_ESP, &e->ContextRecord->Esp);
	uc_reg_write(uc, UC_X86_REG_EBP, &e->ContextRecord->Ebp);
	uc_reg_write(uc, UC_X86_REG_EFLAGS, &e->ContextRecord->EFlags);

	// Run Unicorn for one instruction
	// NOTE: This is currently slightly slower than old EmuX86, but has potential to be massively optimised
	// It's a lot of work, but we can get much higher performance through clever use of Unicorn (and avoiding exceptions)
	// This does not count as clever use, but is a test to see how well it works for us
	uc_err err = uc_emu_start(uc, e->ContextRecord->Eip, 0, 0, 1);
	if (err) {
		CxbxKrnlCleanup("Failed on uc_emu_start() with error returned: %u\n", err);
	}

	// Write CPU state back to host
	uc_reg_read(uc, UC_X86_REG_EDI, &e->ContextRecord->Edi);
	uc_reg_read(uc, UC_X86_REG_ESI, &e->ContextRecord->Esi);
	uc_reg_read(uc, UC_X86_REG_EBX, &e->ContextRecord->Ebx);
	uc_reg_read(uc, UC_X86_REG_EDX, &e->ContextRecord->Edx);
	uc_reg_read(uc, UC_X86_REG_ECX, &e->ContextRecord->Ecx);
	uc_reg_read(uc, UC_X86_REG_EAX, &e->ContextRecord->Eax);
	uc_reg_read(uc, UC_X86_REG_EIP, &e->ContextRecord->Eip);
	uc_reg_read(uc, UC_X86_REG_ESP, &e->ContextRecord->Esp);
	uc_reg_read(uc, UC_X86_REG_EBP, &e->ContextRecord->Ebp);
	uc_reg_read(uc, UC_X86_REG_EFLAGS, &e->ContextRecord->EFlags);
	
	return true;
}

#include <unordered_map>
std::unordered_map<DWORD, uc_engine*> g_UnicornHandles;

// Unicorn requires a seperate context per-thread, we handle that by using EmuX86_Init as a GetUnicornContext function
uc_engine* EmuX86_Init()
{
	uc_engine* uc;
	uc_hook hook_in, hook_out;

	// First, attempt to fetch a uncorn instance for the current thread
	auto it = g_UnicornHandles.find(GetCurrentThreadId());
	if (it != g_UnicornHandles.end()) {
		return it->second;
	}
	
	// This thread didn't have a unicorn instance, so create one
	uc_err err = uc_open(UC_ARCH_X86, UC_MODE_32, &uc);
	if (err) {
		CxbxKrnlCleanup("Failed on uc_open() with error returned: %u\n", err);
	}

	// Set Unicorn to map 1:1 with our emulated Xbox memory (except HW registers)
	// We should map this to 0x0 but unicorn doesn't like that (it can't take a nullptr as an argument)
	err = uc_mem_map_ptr(uc, XBE_IMAGE_BASE, 0xFD000000 - XBE_IMAGE_BASE, UC_PROT_ALL, (void*)XBE_IMAGE_BASE); // XBE Region (0x00000000)
	if (err) {
		CxbxKrnlCleanup("Failed on uc_mem_map_ptr(uc, 0, XBOX_MEMORY_SIZE, UC_PROT_ALL, 0) with error returned: %u\n", err);
	}
	
	// Register MMIO and IO Hooks
	err = uc_mmio_map(uc, 0xFD000000, 0x3000000, read_cb, write_cb, nullptr);
	if (err) {
		CxbxKrnlCleanup("Failed on uc_mmio_map() with error returned: %u\n", err);
	}

	err = uc_hook_add(uc, &hook_in, UC_HOOK_INSN, hook_in_cb, NULL, 1, 0, UC_X86_INS_IN);
	if (err) {
		CxbxKrnlCleanup("Failed on uc_hook_add() with error returned: %u\n", err);
	}

	err = uc_hook_add(uc, &hook_out, UC_HOOK_INSN, hook_out_cb, NULL, 1, 0, UC_X86_INS_OUT);
	if (err) {
		CxbxKrnlCleanup("Failed on uc_hook_add() with error returned: %u\n", err);
	}


	g_UnicornHandles[GetCurrentThreadId()] = uc;

	return uc;
}
