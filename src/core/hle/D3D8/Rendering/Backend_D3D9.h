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
#ifndef BACKEND_D3D9_H
#define BACKEND_D3D9_H

#ifndef CXBX_USE_D3D11

#include "core\hle\D3D8\XbD3D8Types.h"

// ******************************************************************
// * D3D9 globals (initialized in EmuD3DInit)
// ******************************************************************
extern IDirect3D9Ex *g_pDirect3D;
extern D3DCAPS       g_D3DCaps;

// ******************************************************************
// * D3D9 backend functions (counterparts to Backend_D3D11 functions)
// ******************************************************************

// Read back vertex shader constants directly from the D3D9 device
void CxbxGetVertexShaderConstants(UINT startRegister, float* pConstantData, UINT Vector4fCount);

#endif // !CXBX_USE_D3D11

#endif // BACKEND_D3D9_H
