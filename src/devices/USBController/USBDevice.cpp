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
// *   Cxbx->devices->USBController->USBDevice.cpp
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
// *  (c) 2018 ergo720
// *  
// *  All rights reserved
// *
// ******************************************************************

#include "USBDevice.h"
#include "OHCI.h"


void USBDevice::Init(unsigned int address)
{
	PCIBarRegister r;

	// Register Memory bar :
	r.Raw.type = PCI_BAR_TYPE_MEMORY;
	r.Memory.address = address >> 4;
	RegisterBAR(0, USB_SIZE, r.value);

	// Taken from https://github.com/docbrown/vxb/wiki/Xbox-Hardware-Information
	m_DeviceId = 0x01C2;
	m_VendorId = PCI_VENDOR_ID_NVIDIA;

	if (address == USB0_BASE) {
		m_pHostController1 = new OHCI(1);
		return;
	}

	m_pHostController2 = new OHCI(9);
}

uint32_t USBDevice::MMIORead(int barIndex, uint32_t addr, unsigned size)
{
	// barIndex must be zero since we created the USB devices with a zero index in Init()
	assert(barIndex == 0);

	// Figure out the correct OHCI object and read the register
	if (addr < USB1_BASE) {
		// USB0 queried
		return m_pHostController1->OHCI_ReadRegister(addr);
	}

	// USB1 queried
	return m_pHostController2->OHCI_ReadRegister(addr);
}

void USBDevice::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size)
{
	// barIndex must be zero since we created the USB devices with a zero index in Init()
	assert(barIndex == 0);

	// Figure out the correct OHCI object and write the value to the register
	if (addr < USB1_BASE) {
		// USB0 queried
		m_pHostController1->OHCI_WriteRegister(addr, value);
		return;
	}

	// USB1 queried
	m_pHostController2->OHCI_WriteRegister(addr, value);
}
