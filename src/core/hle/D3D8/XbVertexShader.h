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
#ifndef XBVERTEXSHADER_H
#define XBVERTEXSHADER_H

#include "Cxbx.h"

// recompile xbox vertex shader declaration
extern D3DVERTEXELEMENT *EmuRecompileVshDeclaration
(
    DWORD                *pXboxDeclaration,
    bool                  bIsFixedFunction,
    DWORD                *pXboxDeclarationCount,
    DWORD                *pHostDeclarationSize,
    XTL::CxbxVertexShaderInfo *pCxbxVertexShaderInfo
);

// recompile xbox vertex shader function
extern HRESULT EmuRecompileVshFunction
(
    DWORD        *pXboxFunction,
    bool          bNoReservedConstants,
    D3DVERTEXELEMENT *pRecompiledDeclaration,
    bool   		 *pbUseDeclarationOnly,
    DWORD        *pXboxFunctionSize,
    LPD3DXBUFFER *ppRecompiledShader
);

extern void FreeVertexDynamicPatch(CxbxVertexShader *pVertexShader);

// Checks for failed vertex shaders, and shaders that would need patching
extern boolean IsValidCurrentShader(void);

inline boolean VshHandleIsVertexShader(DWORD Handle) { return (Handle & X_D3DFVF_RESERVED0) ? TRUE : FALSE; }
inline boolean VshHandleIsFVF(DWORD Handle) { return !VshHandleIsVertexShader(Handle); }
inline X_D3DVertexShader *VshHandleToXboxVertexShader(DWORD Handle) { return (X_D3DVertexShader *)(Handle & ~X_D3DFVF_RESERVED0);}

extern CxbxVertexShader* GetCxbxVertexShader(DWORD XboxVertexShaderHandle);
extern void SetCxbxVertexShader(DWORD XboxVertexShaderHandle, CxbxVertexShader* shader);

#endif
