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
#ifndef EMUPATCHES_DRAW_H
#define EMUPATCHES_DRAW_H

#include "core\hle\XAPI\Xapi.h" // For EMUPATCH
#include "core\hle\D3D8\XbD3D8Types.h"

namespace xbox {

// ******************************************************************
// * patch: D3DDevice_Begin
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_Begin)
(
    X_D3DPRIMITIVETYPE     PrimitiveType
);

// ******************************************************************
// * patch: D3DDevice_SetVertexData2f
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetVertexData2f)
(
    int_xt     Register,
    float_xt   a,
    float_xt   b
);

// ******************************************************************
// * patch: D3DDevice_SetVertexData2s
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetVertexData2s)
(
    int_xt     Register,
    short_xt   a,
    short_xt   b
);

// ******************************************************************
// * patch: D3DDevice_SetVertexData4f
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetVertexData4f)
(
    int_xt     Register,
    float_xt   a,
    float_xt   b,
    float_xt   c,
    float_xt   d
);

// ******************************************************************
// * patch: D3DDevice_SetVertexData4f_16__LTCG_edi1
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetVertexData4f_16__LTCG_edi1)
(
	float_xt   a,
	float_xt   b,
	float_xt   c,
	float_xt   d
);

// ******************************************************************
// * patch: D3DDevice_SetVertexData4ub
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetVertexData4ub)
(
	int_xt	Register,
	byte_xt	a,
	byte_xt	b,
	byte_xt	c,
	byte_xt	d
);

// ******************************************************************
// * patch: D3DDevice_SetVertexData4s
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetVertexData4s)
(
	int_xt		Register,
	short_xt	a,
	short_xt	b,
	short_xt	c,
	short_xt	d
);

// ******************************************************************
// * patch: D3DDevice_SetVertexDataColor
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetVertexDataColor)
(
    int_xt      Register,
    X_D3DCOLOR  Color
);

// ******************************************************************
// * patch: D3DDevice_End
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_End)();

// ******************************************************************
// * patch: D3DDevice_RunPushBuffer
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_RunPushBuffer)
(
    X_D3DPushBuffer       *pPushBuffer,
    X_D3DFixup            *pFixup
);

// ******************************************************************
// * patch: D3DDevice_RunPushBuffer_4__LTCG_eax2
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_RunPushBuffer_4__LTCG_eax2)
(
    X_D3DPushBuffer *pPushBuffer
);

// ******************************************************************
// * patch: D3DDevice_DrawVerticesUP
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_DrawVerticesUP)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    uint_xt                VertexCount,
    CONST PVOID         pVertexStreamZeroData,
    uint_xt                VertexStreamZeroStride
);

xbox::void_xt WINAPI EMUPATCH(D3DDevice_DrawVerticesUP_12__LTCG_ebx3)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    uint_xt             VertexCount,
    uint_xt             VertexStreamZeroStride
);


// ******************************************************************
// * patch: D3DDevice_DrawIndexedVertices
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_DrawIndexedVertices)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    uint_xt                VertexCount,
    CONST PWORD         pIndexData
);

// ******************************************************************
// * patch: D3DDevice_DrawIndexedVerticesUP
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_DrawIndexedVerticesUP)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    uint_xt                VertexCount,
    CONST PVOID         pIndexData,
    CONST PVOID         pVertexStreamZeroData,
    uint_xt                VertexStreamZeroStride
);

// ******************************************************************
// * patch: CDevice_SetStateVB (D3D::CDevice::SetStateVB)
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(CDevice_SetStateVB)(xbox::ulong_xt Unknown1);
xbox::void_xt WINAPI EMUPATCH(CDevice_SetStateVB_8)(xbox::addr_xt _this, xbox::ulong_xt Unknown1);

// ******************************************************************
// * patch: CDevice_SetStateUP (D3D::CDevice::SetStateUP)
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(CDevice_SetStateUP)();
xbox::void_xt WINAPI EMUPATCH(CDevice_SetStateUP_4)(xbox::addr_xt _this);
xbox::void_xt WINAPI EMUPATCH(CDevice_SetStateUP_0__LTCG_esi1)();

// ******************************************************************
// * patch: D3DDevice_SetStipple
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetStipple)( dword_xt* pPattern );

// ******************************************************************
// * patch: D3DDevice_SetSwapCallback
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetSwapCallback)
(
	X_D3DSWAPCALLBACK		pCallback
);

// ******************************************************************
// * patch: D3DDevice_PrimeVertexCache
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_PrimeVertexCache)
(
	uint_xt VertexCount,
	word_xt *pIndexData
);


} // namespace xbox

#endif // EMUPATCHES_DRAW_H
