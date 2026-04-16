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
// ******************************************************************
// *  CPU-side tessellation of rect and tri patches for D3D11
// *  (D3D11 has no DrawRectPatch/DrawTriPatch API)
// ******************************************************************
#ifndef PATCHDRAW_H
#define PATCHDRAW_H

#ifdef CXBX_USE_D3D11

#include "core\hle\D3D8\XbD3D8Types.h"

// Tessellate a rectangular patch and draw it via D3D11
HRESULT CxbxDrawRectPatchD3D11(
	UINT Handle,
	const float *pNumSegs,
	const xbox::X_D3DRECTPATCH_INFO *pRectPatchInfo
);

// Tessellate a triangular patch and draw it via D3D11
HRESULT CxbxDrawTriPatchD3D11(
	UINT Handle,
	const float *pNumSegs,
	const xbox::X_D3DTRIPATCH_INFO *pTriPatchInfo
);

#endif // CXBX_USE_D3D11

#endif // PATCHDRAW_H
