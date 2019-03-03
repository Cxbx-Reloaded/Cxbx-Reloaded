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
extern bool bLLE_JIT; // Set this to true for experimental JIT

extern std::map<std::string, xbaddr> g_SymbolAddresses;

void EmuHLEIntercept(Xbe::Header *XbeHeader);

std::string GetDetectedSymbolName(xbaddr address, int *symbolOffset);
void* GetXboxFunctionPointer(std::string functionName);

// Declares an unpatched Xbox function trampoline, callable by name (with a 'XB_' prefix attached)
#define XB_trampoline(ret, conv, name, arguments) \
    typedef ret(conv *XB_##name##_t)arguments; \
    static XB_##name##_t XB_##name = (XB_##name##_t)GetXboxFunctionPointer(#name);

#ifdef _DEBUG_TRACE
void VerifyHLEDataBase();
#endif

#endif
