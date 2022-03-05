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
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef EMUKRNL_H
#define EMUKRNL_H

#include "core\kernel\init\CxbxKrnl.h"
#include "core\kernel\support\Emu.h"
#include "core\kernel\support\EmuFS.h"
#include <future>

// CONTAINING_RECORD macro
// Gets the value of structure member (field - num1),given the type(MYSTRUCT, in this code) and the List_Entry head(temp, in this code)
// See https://stackoverflow.com/questions/8240273/a-portable-way-to-calculate-pointer-to-the-whole-structure-using-pointer-to-a-fi
//#define CONTAINING_RECORD(ptr, type, field) \
//	(((type) *)((char *)(ptr) - offsetof((type), member)))

#define OBJECT_TO_OBJECT_HEADER(Object) \
    CONTAINING_RECORD(Object, OBJECT_HEADER, Body)

void InitializeListHead(xbox::PLIST_ENTRY pListHead);
bool IsListEmpty(xbox::PLIST_ENTRY pListHead);
void InsertHeadList(xbox::PLIST_ENTRY pListHead, xbox::PLIST_ENTRY pEntry);
void InsertTailList(xbox::PLIST_ENTRY pListHead, xbox::PLIST_ENTRY pEntry);
//#define RemoveEntryList(e) do { PLIST_ENTRY f = (e)->Flink, b = (e)->Blink; f->Blink = b; b->Flink = f; (e)->Flink = (e)->Blink = NULL; } while (0)

xbox::boolean_xt RemoveEntryList(xbox::PLIST_ENTRY pEntry);
xbox::PLIST_ENTRY RemoveHeadList(xbox::PLIST_ENTRY pListHead);
xbox::PLIST_ENTRY RemoveTailList(xbox::PLIST_ENTRY pListHead);

extern xbox::LAUNCH_DATA_PAGE DefaultLaunchDataPage;
extern xbox::PKINTERRUPT EmuInterruptList[MAX_BUS_INTERRUPT_LEVEL + 1];
inline std::condition_variable g_InterruptSignal;
inline std::atomic_bool g_AnyInterruptAsserted = false;

class HalSystemInterrupt {
public:
	void Assert(bool state) {
		// If the interrupt was marked as Asserted, and was previously not, set the pending flag too!
		if (m_Asserted == 0 && state == 1) {
			m_Pending = true;
		}

		m_Asserted = state;
		g_AnyInterruptAsserted = true;
		g_InterruptSignal.notify_one();
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

	void SetInterruptMode(xbox::KINTERRUPT_MODE InterruptMode) {
		m_InterruptMode = InterruptMode;
	}

	void Trigger(xbox::PKINTERRUPT Interrupt) {
		// If interrupt was level sensitive, we clear the pending flag, preventing the interrupt from being triggered
		// until it is deasserted then asserted again. Latched interrupts are triggered until the line is Deasserted!
		if (m_InterruptMode == xbox::KINTERRUPT_MODE::LevelSensitive) {
			m_Pending = false;
		}

		xbox::boolean_xt(__stdcall *ServiceRoutine)(xbox::PKINTERRUPT, void*) = (xbox::boolean_xt(__stdcall *)(xbox::PKINTERRUPT, void*))Interrupt->ServiceRoutine;
		xbox::boolean_xt result = ServiceRoutine(Interrupt, Interrupt->ServiceContext);
	}
private:
	bool m_Asserted = false;
	bool m_Enabled = false;
	xbox::KINTERRUPT_MODE m_InterruptMode;
	bool m_Pending = false;
};

extern HalSystemInterrupt HalSystemInterrupts[MAX_BUS_INTERRUPT_LEVEL + 1];

bool DisableInterrupts();
void RestoreInterruptMode(bool value);
void CallSoftwareInterrupt(const xbox::KIRQL SoftwareIrql);

template<typename T>
xbox::ntstatus_xt WaitApc(T &&Lambda, xbox::PLARGE_INTEGER AbsoluteExpireTime, xbox::boolean_xt Alertable, xbox::char_xt WaitMode)
{
	// NOTE: kThread->Alerted is currently never set. When the alerted mechanism is implemented, the alerts should
	// also interrupt the wait

	xbox::ulonglong_xt now = xbox::KeQueryInterruptTime();
	xbox::PETHREAD eThread = reinterpret_cast<xbox::PETHREAD>(EmuKeGetPcr()->Prcb->CurrentThread);

	if (AbsoluteExpireTime->QuadPart == 0) {
		// This will only happen when the title specifies a zero timeout
		AbsoluteExpireTime->QuadPart = now;
	}

	while (now <= static_cast<xbox::ulonglong_xt>(AbsoluteExpireTime->QuadPart)) {
		if (const auto ret = Lambda()) {
			return *ret;
		}

		xbox::KiApcListMtx.lock();
		bool EmptyKernel = IsListEmpty(&eThread->Tcb.ApcState.ApcListHead[xbox::KernelMode]);
		bool EmptyUser = IsListEmpty(&eThread->Tcb.ApcState.ApcListHead[xbox::UserMode]);
		xbox::KiApcListMtx.unlock();
		if (EmptyKernel == false) {
			xbox::KiExecuteKernelApc();
		}
		if ((EmptyUser == false) &&
			(Alertable == TRUE) &&
			(WaitMode == xbox::UserMode)) {
			xbox::KiExecuteUserApc();
			return X_STATUS_USER_APC;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		now = xbox::KeQueryInterruptTime();
	}

	return X_STATUS_TIMEOUT;
}

#endif
