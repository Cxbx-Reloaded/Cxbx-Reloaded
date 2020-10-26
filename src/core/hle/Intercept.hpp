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
#ifndef HLEINTERCEPT_HPP
#define HLEINTERCEPT_HPP

#include <map>

extern bool bLLE_APU; // Set this to true for experimental APU (sound) LLE
extern bool bLLE_GPU; // Set this to true for experimental GPU (graphics) LLE
extern bool bLLE_USB; // Set this to true for experimental USB (input) LLE
extern bool bLLE_JIT; // Set this to true for experimental JIT

extern std::map<std::string, xbox::addr_xt> g_SymbolAddresses;

void EmuHLEIntercept(Xbe::Header *XbeHeader);

std::string GetDetectedSymbolName(const xbox::addr_xt address, int * const symbolOffset);
void* GetXboxFunctionPointer(std::string functionName);

#define XB_TYPE(func) XB_TRAMPOLINE_##func##_t
#define XB_NAME(func) XB_TRAMPOLINE_##func##_str
#define XB_TRMP(func) XB_TRAMPOLINE_##func

// Declares an unpatched Xbox function trampoline, callable by name (with a 'XB_' prefix attached)
#define XB_trampoline_declare(ret, conv, func, arguments) \
    typedef ret(conv * XB_TYPE(func))arguments; \
    static const std::string XB_NAME(func) = #func; \
    static XB_TYPE(func) XB_TRMP(func) = nullptr

#define XB_trampoline_lookup(ret, conv, func, arguments) \
    XB_TRMP(func) = (XB_TYPE(func))GetXboxFunctionPointer(XB_NAME(func))

#ifdef _DEBUG_TRACE
void VerifyHLEDataBase();
#endif

// A helper class to allow code patches to detect nested calls (patches called from patches)
class NestedPatchCounter
{
public:
	explicit NestedPatchCounter(uint32_t& counter) noexcept
		: m_counter(counter), m_level(counter)
	{
		m_counter++;
	}

	~NestedPatchCounter() noexcept
	{
		m_counter--;
	}

	uint32_t GetLevel() const noexcept { return m_level; }

public:
	uint32_t& m_counter;
	const uint32_t m_level;
};

#endif
