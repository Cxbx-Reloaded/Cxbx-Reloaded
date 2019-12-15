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

#include <d3dcompiler.h>

#include "core\hle\D3D8\XbD3D8Types.h" // for X_VSH_MAX_ATTRIBUTES

// Host vertex shader counts
#define VSH_VS11_MAX_INSTRUCTION_COUNT 128
#define VSH_VS2X_MAX_INSTRUCTION_COUNT 256
#define VSH_VS30_MAX_INSTRUCTION_COUNT 512

#define VSH_MAX_INTERMEDIATE_COUNT (X_VSH_MAX_INSTRUCTION_COUNT * 3) // The maximum number of shader function slots

typedef struct _CxbxVertexShaderStreamElement
{
	UINT XboxType; // The stream element data types (xbox)
	UINT XboxByteSize; // The stream element data sizes (xbox)
	UINT HostByteSize; // The stream element data sizes (pc)
}
CxbxVertexShaderStreamElement;

/* See host typedef struct _D3DVERTEXELEMENT9
{
	WORD    Stream;     // Stream index
	WORD    Offset;     // Offset in the stream in bytes
	BYTE    Type;       // Data type
	BYTE    Method;     // Processing method
	BYTE    Usage;      // Semantics
	BYTE    UsageIndex; // Semantic index
} D3DVERTEXELEMENT9, *LPD3DVERTEXELEMENT9;
*/

typedef struct _CxbxVertexShaderStreamInfo
{
	bool  NeedPatch;       // This is to know whether it's data which must be patched
	BOOL DeclPosition;
	WORD HostVertexStride;
	DWORD NumberOfVertexElements;        // Number of the stream data types
	WORD CurrentStreamNumber;
	CxbxVertexShaderStreamElement VertexElements[X_VSH_MAX_ATTRIBUTES + 16]; // TODO : Why 16 extra host additions?)
}
CxbxVertexShaderStreamInfo;

typedef struct _CxbxVertexShaderInfo
{
	UINT                       NumberOfVertexStreams; // The number of streams the vertex shader uses
	CxbxVertexShaderStreamInfo VertexStreams[X_VSH_MAX_STREAMS];
	bool vRegisterInDeclaration[16];
}
CxbxVertexShaderInfo;

typedef struct _CxbxVertexShader
{
	// These are the parameters given by the XBE,
	// we save them to be able to return them when necessary.
	DWORD* pXboxDeclarationCopy;
	DWORD                 XboxDeclarationCount; // Xbox's number of DWORD-sized X_D3DVSD* tokens
	DWORD                 XboxFunctionSize;
	DWORD* pXboxFunctionCopy;
	UINT                  XboxNrAddressSlots;
	DWORD                 XboxVertexShaderType;
	// DWORD              XboxStatus; // Used by VshHandleIsValidShader()

	// The resulting host variables
	DWORD HostFVF; // Flexible Vertex Format (used when there's no host vertex shader)
	IDirect3DVertexShader* pHostVertexShader; // if nullptr, use SetFVF(HostFVF);
	IDirect3DVertexDeclaration* pHostVertexDeclaration;

	// Needed for dynamic stream patching
	CxbxVertexShaderInfo  VertexShaderInfo;
}
CxbxVertexShader;

// recompile xbox vertex shader declaration
extern D3DVERTEXELEMENT *EmuRecompileVshDeclaration
(
    DWORD                *pXboxDeclaration,
    bool                  bIsFixedFunction,
    DWORD                *pXboxDeclarationCount,
    DWORD                *pHostDeclarationSize,
    CxbxVertexShaderInfo *pCxbxVertexShaderInfo
);

// recompile xbox vertex shader function
extern HRESULT EmuRecompileVshFunction
(
    DWORD        *pXboxFunction,
    bool          bNoReservedConstants,
    D3DVERTEXELEMENT *pRecompiledDeclaration,
    bool   		 *pbUseDeclarationOnly,
    DWORD        *pXboxFunctionSize,
	ID3DBlob **ppRecompiledShader
);

extern void FreeVertexDynamicPatch(CxbxVertexShader *pVertexShader);

// Checks for failed vertex shaders, and shaders that would need patching
extern boolean IsValidCurrentShader(void);

inline boolean VshHandleIsVertexShader(DWORD Handle) { return (Handle & X_D3DFVF_RESERVED0) ? TRUE : FALSE; }
inline boolean VshHandleIsFVF(DWORD Handle) { return !VshHandleIsVertexShader(Handle); }
inline XTL::X_D3DVertexShader *VshHandleToXboxVertexShader(DWORD Handle) { return (XTL::X_D3DVertexShader *)(Handle & ~X_D3DFVF_RESERVED0);}

extern CxbxVertexShader* GetCxbxVertexShader(DWORD XboxVertexShaderHandle);
extern void SetCxbxVertexShader(DWORD XboxVertexShaderHandle, CxbxVertexShader* shader);

#endif
