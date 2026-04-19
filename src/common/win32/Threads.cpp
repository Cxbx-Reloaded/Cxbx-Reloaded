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

// Override _WIN32_WINNT for this .cpp file to gain access to the CPU Sets API
#undef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_WIN10

#include <windows.h>
#include "Threads.h"

#include "core/kernel/support/Emu.h"
#include "core/kernel/init/CxbxKrnl.h"

#include <processthreadsapi.h>
#include <vector>
#include <set>

std::unique_ptr<AffinityPolicy> g_AffinityPolicy;

// Exception structure and method from:
// https://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx

const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)  
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.  
	LPCSTR szName; // Pointer to name (in user addr space).  
	DWORD dwThreadID; // Thread ID (-1=caller thread).  
	DWORD dwFlags; // Reserved for future use, must be zero.  
} THREADNAME_INFO;
#pragma pack(pop)  

#ifdef _MSC_VER
void SetThreadName(DWORD dwThreadID, const char* szThreadName)
{
	if (!IsDebuggerPresent())
		return;

	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = szThreadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;
	// NOTE: Require to support Windows platform older than Windows 10 version 1607. Or alternative method will require Windows 10. (see link at top of source file)
	__try {
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
#pragma warning(suppress : 6320) // silence /analyze: Exception-filter expression is the constant
	// EXCEPTION_EXECUTE_HANDLER. This might mask exceptions that were
	// not intended to be handled
#pragma warning(suppress : 6322) // silence /analyze: Empty _except block
	__except (EXCEPTION_EXECUTE_HANDLER) {
	}
}
#else
void SetThreadName(DWORD dwThreadID, const char* szThreadName)
{
	// TODO: Use SetThreadDescription
}
#endif

void SetCurrentThreadName(const char* szThreadName)
{
	SetThreadName(GetCurrentThreadId(), szThreadName);
}

// Windows 10 affinity policy - uses CPU sets to pin threads accordingly
class Win10Policy final : public AffinityPolicy
{
public:
	bool Initialize() {
		HMODULE kernel32 = GetModuleHandleW(L"kernel32");

		auto getSystemCpuSetInformation = reinterpret_cast<decltype(GetSystemCpuSetInformation)*>(GetProcAddress(kernel32, "GetSystemCpuSetInformation"));
		auto setProcessDefaultCpuSets = reinterpret_cast<decltype(SetProcessDefaultCpuSets)*>(GetProcAddress(kernel32, "SetProcessDefaultCpuSets"));
		m_setThreadSelectedCpuSets = reinterpret_cast<decltype(SetThreadSelectedCpuSets)*>(GetProcAddress(kernel32, "SetThreadSelectedCpuSets"));

		// Those functions are available only in Windows 10, so if bail out if they don't exist
		if (getSystemCpuSetInformation == nullptr || setProcessDefaultCpuSets == nullptr || m_setThreadSelectedCpuSets == nullptr) {
			return false;
		}

		const HANDLE process = GetCurrentProcess();
		ULONG bufSize;
		getSystemCpuSetInformation(nullptr, 0, &bufSize, process, 0);

		auto buffer = std::make_unique<uint8_t[]>(bufSize);
		if (!getSystemCpuSetInformation(reinterpret_cast<PSYSTEM_CPU_SET_INFORMATION>(buffer.get()), bufSize, &bufSize, process, 0)) {
			return false;
		}

		// SYSTEM_CPU_SET_INFORMATION is a variable length structure and may be expanded in the future,
		// so "real" pointers to elements need to be calculated and filtered
		std::vector<const decltype(SYSTEM_CPU_SET_INFORMATION::CpuSet)*> cpuSets;
		const uint8_t* ptr = buffer.get();
		for (ULONG size = 0; size < bufSize; ) {
			auto info = reinterpret_cast<const SYSTEM_CPU_SET_INFORMATION*>(ptr);
			if (info->Type == CpuSetInformation) {
				cpuSets.push_back(&info->CpuSet);
			}
			ptr += info->Size;
			size += info->Size;
		}

		// Count logical and physical CPU cores
		size_t numLogicalCores, numPhysicalCores;
		{
			std::set<BYTE> logicalCores, physicalCores;
			for (const auto& info : cpuSets) {
				logicalCores.insert(info->LogicalProcessorIndex);
				physicalCores.insert(info->CoreIndex);
			}
			numLogicalCores = logicalCores.size();
			numPhysicalCores = physicalCores.size();
		}

		// Case #1: Single core machines
		// Don't change affinity at all, report failure
		if (numLogicalCores == 1) {
			return false;
		}

		// Case #2: Single physical core, multiple logical cores
		// Assign the first logical core to Xbox, leave the rest to other threads
		if (numPhysicalCores == 1 && numLogicalCores != 1) {
			m_xboxCPUSet = cpuSets[0]->Id;
			cpuSets.erase(cpuSets.begin());
		}
		// Otherwise: Multiple physical cores
		// Assign the first highest performance logical and physical core to Xbox,
		// leave the rest of that physical core unassigned (if hyperthreading is active),
		// give the remaining physical cores to other threads
		else {
			auto highPerfCore = std::max_element(cpuSets.begin(), cpuSets.end(), [](const auto* left, const auto* right)
				{
					return left->EfficiencyClass < right->EfficiencyClass;
				});
			const BYTE physicalCore = (*highPerfCore)->CoreIndex;
			m_xboxCPUSet = (*highPerfCore)->Id;
			for (auto it = cpuSets.begin(); it != cpuSets.end(); ) {
				if ((*it)->CoreIndex == physicalCore) {
					it = cpuSets.erase(it);
				} else {
					++it;
				}
			}
		}

		// Finally, extract the CPU IDs and assign them as a default process group
		std::vector<DWORD> cpuIds;
		cpuIds.reserve(cpuSets.size());
		for (const auto& info : cpuSets) {
			cpuIds.push_back(info->Id);
		}

		return setProcessDefaultCpuSets(process, cpuIds.data(), cpuIds.size()) != FALSE;
	}

		
	virtual void SetAffinityXbox(HANDLE thread) const override {
		m_setThreadSelectedCpuSets(thread, &m_xboxCPUSet, 1);
	}

	virtual void SetAffinityOther(HANDLE /*thread*/) const override {
		// CPU sets for the process have already been set, so do nothing.
	}

private:
	ULONG m_xboxCPUSet = 0;
	decltype(SetThreadSelectedCpuSets)* m_setThreadSelectedCpuSets = nullptr;
};

// Windows 7/8.1 affinity policy - uses thread affinity to pin threads accordingly
class Win7Policy final : public AffinityPolicy
{
public:
	bool Initialize() {
		if (!GetProcessAffinityMask(g_CurrentProcessHandle, &CPUXbox, &CPUOthers))
			CxbxrAbortEx(CXBXR_MODULE::INIT, "GetProcessAffinityMask failed.");

		// For the other threads, remove one bit from the processor mask:
		CPUOthers = ((CPUXbox - 1) & CPUXbox);

		// Test if there are any other cores available:
		if (CPUOthers == 0) {
			// If not, fail the policy
			return false;
		}
		CPUXbox = CPUXbox & (~CPUOthers);
		return true;
	}

	virtual void SetAffinityXbox(HANDLE thread) const override {
		SetThreadAffinityMask(thread, CPUXbox);
	}

	virtual void SetAffinityOther(HANDLE thread) const override {
		SetThreadAffinityMask(thread, CPUOthers);
	}

private:
	DWORD_PTR CPUXbox = 0;
	DWORD_PTR CPUOthers = 0;
};

// Empty affinity policy - used on single core host machines and if "All Cores Hack" is enabled
class EmptyPolicy final : public AffinityPolicy
{
public:
	virtual void SetAffinityXbox(HANDLE /*thread*/) const override {
	}

	virtual void SetAffinityOther(HANDLE /*thread*/) const override {
	}
};

std::unique_ptr<AffinityPolicy> AffinityPolicy::InitPolicy()
{
	std::unique_ptr<AffinityPolicy> result;

	if (!g_UseAllCores) {
		if (auto win10Policy = std::make_unique<Win10Policy>(); win10Policy->Initialize()) {
			result = std::move(win10Policy);
		} else if (auto win7Policy = std::make_unique<Win7Policy>(); win7Policy->Initialize()) {
			result = std::move(win7Policy);
		}
	}

	if (!result) {
		result = std::make_unique<EmptyPolicy>();
	}

	return result;
}

void AffinityPolicy::SetAffinityXbox() const {
	SetAffinityXbox(GetCurrentThread());
}

void AffinityPolicy::SetAffinityOther() const {
	SetAffinityOther(GetCurrentThread());
}
