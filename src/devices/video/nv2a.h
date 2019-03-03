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
// *  (c) 2016-2018 Luke Usher <luke.usher@outlook.com>
// *  (c) 2018 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#pragma once

#include "devices\PCIDevice.h" // For PCIDevice

#include "nv2a_int.h" // For NV2AState

typedef struct NV2ABlockInfo {
	const char* name;
	hwaddr offset;
	uint64_t size;
	MemoryRegionOps ops;
} NV2ABlockInfo;

const NV2ABlockInfo* EmuNV2A_Block(xbaddr addr);

void CxbxReserveNV2AMemory(NV2AState *d);

class NV2ADevice : public PCIDevice {
public:
	// constructor
	NV2ADevice();
	// destructor
	~NV2ADevice();

	// PCI Device functions
	void Init();
	void Reset();

	// State Getter: Used for HLE reading of device state
	NV2AState* GetDeviceState() { return m_nv2a_state; };

	uint32_t IORead(int barIndex, uint32_t port, unsigned size);
	void IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size);
	uint32_t BlockRead(const NV2ABlockInfo* block, uint32_t addr, unsigned size);
	uint32_t MMIORead(int barIndex, uint32_t addr, unsigned size);
	void BlockWrite(const NV2ABlockInfo* block, uint32_t addr, uint32_t value, unsigned size);
	void MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size);

	static void UpdateHostDisplay(NV2AState *d);

	static int GetFrameWidth(NV2AState *d);
	static int GetFrameHeight(NV2AState *d);
private:
	NV2AState *m_nv2a_state;
};
