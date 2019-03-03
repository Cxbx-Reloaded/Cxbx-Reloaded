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
// *  (c) 2018 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#pragma once

#include "devices\PCIDevice.h" // For PCIDevice

typedef enum {
	MCPX_1_0,
	MCPX_1_1,
} MCPXROMVersion;

typedef enum {
	MCPX_X2,
	MCPX_X3,
} MCPXRevision;

class MCPXDevice : public PCIDevice {
public:
	// constructor
	MCPXDevice(MCPXRevision revision);

	// PCI Device functions
	void Init();
	void Reset();

	uint32_t IORead(int barIndex, uint32_t port, unsigned size);
	void IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size);
	uint32_t MMIORead(int barIndex, uint32_t addr, unsigned size);
	void MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size);
private:
	MCPXRevision m_revision;
};
