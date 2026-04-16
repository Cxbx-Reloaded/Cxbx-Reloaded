// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
// *  All rights reserved
// *
// ******************************************************************
#ifndef EMUPATCHES_DEVICE_H
#define EMUPATCHES_DEVICE_H

#include "core\hle\XAPI\Xapi.h" // For EMUPATCH
#include "core\hle\D3D8\XbD3D8Types.h"

namespace xbox {

// ******************************************************************
// * patch: Direct3D_CreateDevice
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(Direct3D_CreateDevice)
(
    uint_xt                     Adapter,
	X_D3DDEVTYPE                DeviceType,
	X_HWND                      hFocusWindow,
    dword_xt                    BehaviorFlags,
    X_D3DPRESENT_PARAMETERS    *pPresentationParameters,
    X_D3DDevice           **ppReturnedDeviceInterface
);

// ******************************************************************
// * patch: D3DDevice_GetDisplayFieldStatus
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetDisplayFieldStatus)
(
	X_D3DFIELD_STATUS *pFieldStatus
);

// ******************************************************************
// * patch: D3DDevice_BeginPush_4
// ******************************************************************
xbox::PDWORD WINAPI EMUPATCH(D3DDevice_BeginPush_4)(dword_xt Count);

// ******************************************************************
// * patch: D3DDevice_BeginPush_8
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_BeginPush_8)(dword_xt Count, dword_xt **ppPush);

// ******************************************************************
// * patch: D3DDevice_EndPush
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_EndPush)(dword_xt *pPush);

// ******************************************************************
// * patch: D3DDevice_Reset
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_Reset)
(
    X_D3DPRESENT_PARAMETERS *pPresentationParameters
);

xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_Reset_0__LTCG_edi1)();

xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_Reset_0__LTCG_ebx1)();

// ******************************************************************
// * patch: D3DDevice_SetIndices
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetIndices)
(
    X_D3DIndexBuffer   *pIndexData,
    uint_xt                BaseVertexIndex
);


} // namespace xbox

#endif // EMUPATCHES_DEVICE_H
