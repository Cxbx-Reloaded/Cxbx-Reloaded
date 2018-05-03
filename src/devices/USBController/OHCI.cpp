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
// *   Cxbx->devices->USBController->OHCI.cpp
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

#include "OHCI.h"
#include "..\CxbxKrnl\CxbxKrnl.h"

// global pointers to the two USB host controllers available on the Xbox
OHCI_State* g_pHostController1 = nullptr;
OHCI_State* g_pHostController2 = nullptr;


void OHCI_State::HC_Reset(USB_State new_state)
{
	// The usb state can be USB_Suspend if it is a software reset, and USB_Reset if it is a hardware
	// reset or cold boot

	// TODO: stop all the list processing here

	// Reset all registers
	if (new_state == USB_Reset) {
		// Remark: the standard says that RemoteWakeupConnected bit should be set during POST, cleared during hw reset
		// and ignored during a sw reset. However, VBox sets it on hw reset and XQEMU clears it. Considering that the Xbox
		// doesn't do POST, I will clear it.
		HC_Registers.HcControl = 0;
	}
	else {
		HC_Registers.HcControl &= (OHCI_CTL_IR | OHCI_CTL_RWC);
	}
	HC_Registers.HcControl = ~OHCI_CTL_HCFS;
	HC_Registers.HcControl = new_state;
	HC_Registers.HcCommandStatus = 0;
	HC_Registers.HcInterruptStatus = 0;
	HC_Registers.HcInterruptEnable = OHCI_INTR_MASTER_INTERRUPT_ENABLED; // enable interrupts

	HC_Registers.HcHCCA = 0;
	HC_Registers.HcPeriodCurrentED = 0;
	HC_Registers.HcControlHeadED = HC_Registers.HcControlCurrentED = 0;
	HC_Registers.HcBulkHeadED = HC_Registers.HcBulkCurrentED = 0;
	HC_Registers.HcDoneHead = 0;

	HC_Registers.HcFmInterval = 0;
	HC_Registers.HcFmInterval |= (0x2778 << 16); // TBD according to the standard, using what XQEMU sets (FSLargestDataPacket)
	HC_Registers.HcFmInterval |= (0 << 31);      // redundant, but we'll do it for the sake of completeness (FrameIntervalToggle)
	HC_Registers.HcFmInterval |= 0x2EDF;         // bit-time of a frame. 1 frame = 1 ms (FrameInterval)
	HC_Registers.HcFmRemaining = 0;
	HC_Registers.HcFmNumber = 0;
	HC_Registers.HcPeriodicStart = 0;

	HC_Registers.HcRhDescriptorA = OHCI_RHA_NPS | 2; // The xbox lacks the hw to switch off the power on the ports and has 2 ports per HC
	HC_Registers.HcRhDescriptorB = 0; // The attached devices are removable and use PowerSwitchingMode to control the power on the ports
	for (int i = 0; i < 2; i++)
	{
		OHCIPort* Port = &HC_Registers.RhPort[i];
		Port->HcRhPortStatus = 0;
		//if (Port->port.device && Port->port.device->attached) {
			//usb_port_reset(&Port->port);
		//}
	}

	DbgPrintf("usb-ohci: Reset\n");
}