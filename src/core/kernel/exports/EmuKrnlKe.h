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
// *  (c) 2019 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#pragma once

namespace xbox
{
	extern std::atomic_flag KeSystemTimeChanged;

	void_xt NTAPI KeSetSystemTime
	(
		IN  PLARGE_INTEGER NewTime,
		OUT PLARGE_INTEGER OldTime
	);

	void_xt NTAPI KeInitializeTimer
	(
		IN PKTIMER Timer
	);

	template<bool IsHostThread = false>
	void_xt KeInitializeThread(
		IN OUT PKTHREAD Thread,
		IN PVOID KernelStack,
		IN ulong_xt KernelStackSize,
		IN ulong_xt TlsDataSize,
		IN PKSYSTEM_ROUTINE SystemRoutine,
		IN PKSTART_ROUTINE StartRoutine,
		IN PVOID StartContext,
		IN PKPROCESS Process
	);

	xbox::void_xt KeResumeThreadEx
	(
		IN PKTHREAD Thread
	);

	xbox::void_xt KeSuspendThreadEx
	(
		IN PKTHREAD Thread
	);

	void_xt KeEmptyQueueApc();
	void_xt KeWaitForDpc();
}
