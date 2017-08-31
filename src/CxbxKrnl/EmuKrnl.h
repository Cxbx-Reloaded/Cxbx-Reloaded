// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuKrnl.h
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
#ifndef EMUKRNL_H
#define EMUKRNL_H

#include "CxbxKrnl.h"
#include "Emu.h"

// CONTAINING_RECORD macro
// Gets the value of structure member (field - num1),given the type(MYSTRUCT, in this code) and the List_Entry head(temp, in this code)
// See http://stackoverflow.com/questions/8240273/a-portable-way-to-calculate-pointer-to-the-whole-structure-using-pointer-to-a-fi
//#define CONTAINING_RECORD(ptr, type, field) \
//	(((type) *)((char *)(ptr) - offsetof((type), member)))

#define OBJECT_TO_OBJECT_HEADER(Object) \
    CONTAINING_RECORD(Object, OBJECT_HEADER, Body)

void InitializeListHead(xboxkrnl::PLIST_ENTRY pListHead);
bool IsListEmpty(xboxkrnl::PLIST_ENTRY pListHead);
void InsertHeadList(xboxkrnl::PLIST_ENTRY pListHead, xboxkrnl::PLIST_ENTRY pEntry);
void InsertTailList(xboxkrnl::PLIST_ENTRY pListHead, xboxkrnl::PLIST_ENTRY pEntry);
//#define RemoveEntryList(e) do { PLIST_ENTRY f = (e)->Flink, b = (e)->Blink; f->Blink = b; b->Flink = f; (e)->Flink = (e)->Blink = NULL; } while (0)

void RemoveEntryList(xboxkrnl::PLIST_ENTRY pEntry);
xboxkrnl::PLIST_ENTRY RemoveHeadList(xboxkrnl::PLIST_ENTRY pListHead);
xboxkrnl::PLIST_ENTRY RemoveTailList(xboxkrnl::PLIST_ENTRY pListHead);

extern xboxkrnl::LAUNCH_DATA_PAGE DefaultLaunchDataPage;
extern xboxkrnl::PKINTERRUPT EmuInterruptList[MAX_BUS_INTERRUPT_LEVEL + 1];

class HalSystemInterrupt {
public:
	void Assert(bool state) {
		// If the interrupt was marked as Asserted, and was previously not, set the pending flag too!
		if (m_Asserted == 0 && state == 1) {
			m_Pending = true;
		}

		m_Asserted = state;
	};

	void Enable() {
		m_Enabled = true;
	}

	void Disable() {
		m_Enabled = false;
	}

	bool IsEnabled() {
		return m_Enabled;
	}

	bool IsPending() {
		return m_Asserted && m_Pending;
	}

	void SetInterruptMode(xboxkrnl::KINTERRUPT_MODE InterruptMode) {
		m_InterruptMode = InterruptMode;
	}

	void Trigger(xboxkrnl::PKINTERRUPT Interrupt) {
		// If interrupt was level sensitive, we clear the pending flag, oreventing the interrupt from being triggered 
		// until it is deasserted then asserted again. Latched interrupts are triggered until the line is Deasserted!
		if (m_InterruptMode == xboxkrnl::KINTERRUPT_MODE::LevelSensitive) {
			m_Pending = false;
		}

		__try {
			BOOLEAN(__stdcall *ServiceRoutine)(xboxkrnl::PKINTERRUPT, void*) = (BOOLEAN(__stdcall *)(xboxkrnl::PKINTERRUPT, void*))Interrupt->ServiceRoutine;
			BOOLEAN result = ServiceRoutine(Interrupt, Interrupt->ServiceContext);
		}
		__except (EmuException(GetExceptionInformation()))
		{
			EmuWarning("Problem with ExceptionFilter!");
		}
	}
private:
	bool m_Asserted = false;
	bool m_Enabled = false;
	xboxkrnl::KINTERRUPT_MODE m_InterruptMode;
	bool m_Pending = false;
};

extern HalSystemInterrupt HalSystemInterrupts[MAX_BUS_INTERRUPT_LEVEL + 1];

#endif
