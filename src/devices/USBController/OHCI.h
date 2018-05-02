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
// *   Cxbx->devices->USBController->OHCI.h
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

#ifndef OHCI_H_
#define OHCI_H_

#include <stdint.h>
#include "Cxbx.h"

// All these registers are well documented in the OHCI standard
typedef struct _OHCI_Registers
{
	// Control and Status partition
	uint32_t HcRevision;
	uint32_t HcControl;
	uint32_t HcCommandStatus;
	uint32_t HcInterruptStatus;
	uint32_t HcInterruptEnable;
	uint32_t HcInterruptDisable;

	// Memory Pointer partition
	uint32_t HcHCCA;
	uint32_t HcPeriodCurrentED;
	uint32_t HcControlHeadED;
	uint32_t HcControlCurrentED;
	uint32_t HcBulkHeadED;
	uint32_t HcBulkCurrentED;
	uint32_t HcDoneHead;

	// Frame Counter partition
	uint32_t HcFmInterval;
	uint32_t HcFmRemaining;
	uint32_t HcFmNumber;
	uint32_t HcPeriodicStart;
	uint32_t HcLSThreshold;

	// Root Hub partition
	uint32_t HcRhDescriptorA;
	uint32_t HcRhDescriptorB;
	uint32_t HcRhStatus;
	uint32_t HcRhPortStatus1; // 2 ports per HC, for a total of 4 USB ports
	uint32_t HcRhPortStatus2;
}
OHCI_Registers;


/* OHCI class representing the state of the HC */
class OHCI_State
{
	public:
		// constructor
		OHCI_State() {}
		// destructor
		~OHCI_State() {}
		// read a register
		uint32_t ReadRegister(xbaddr addr);
		// write a register
		void WriteRegister(xbaddr addr, uint32_t value);

	private:
		// all the registers available on the OHCI standard
		OHCI_Registers Registers;
};

extern OHCI_State* g_pHostController1;
extern OHCI_State* g_pHostController2;

#endif
