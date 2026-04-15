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

#ifndef CXBX_USE_D3D11

#define LOG_PREFIX CXBXR_MODULE::D3D8

#include "Backend_D3D9.h"
#include "EmuD3D8.h" // g_pD3DDevice

// ******************************************************************
// * D3D9 globals — definitions
// ******************************************************************
IDirect3D9Ex *g_pDirect3D = nullptr;
D3DCAPS       g_D3DCaps = {};

// ******************************************************************
// * Shader constant functions (D3D9: thin wrappers around device calls)
// ******************************************************************
void CxbxSetVertexShaderConstantF(UINT startRegister, const float* pConstantData, UINT Vector4fCount)
{
	if (g_pD3DDevice && pConstantData && Vector4fCount > 0) {
		g_pD3DDevice->SetVertexShaderConstantF(startRegister, pConstantData, Vector4fCount);
	}
}

void CxbxSetPixelShaderConstantF(UINT startRegister, const float* pConstantData, UINT Vector4fCount)
{
	if (g_pD3DDevice && pConstantData && Vector4fCount > 0) {
		g_pD3DDevice->SetPixelShaderConstantF(startRegister, pConstantData, Vector4fCount);
	}
}

void CxbxGetVertexShaderConstants(UINT startRegister, float* pConstantData, UINT Vector4fCount)
{
	if (g_pD3DDevice && pConstantData && Vector4fCount > 0) {
		g_pD3DDevice->GetVertexShaderConstantF(startRegister, pConstantData, Vector4fCount);
	}
}

#endif // !CXBX_USE_D3D11
