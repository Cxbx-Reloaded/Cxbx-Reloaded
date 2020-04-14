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
#include <vector>
#include <future>

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
	BOOL  NeedPatch;       // This is to know whether it's data which must be patched
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
	uint64_t VertexShaderKey; // if nullptr, use SetFVF(HostFVF);
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
    CxbxVertexShaderInfo *pCxbxVertexShaderInfo
);

// Intermediate vertex shader structures

enum VSH_OREG_NAME {
	OREG_OPOS,    //  0
	OREG_UNUSED1, //  1
	OREG_UNUSED2, //  2
	OREG_OD0,     //  3
	OREG_OD1,     //  4
	OREG_OFOG,    //  5
	OREG_OPTS,    //  6
	OREG_OB0,     //  7
	OREG_OB1,     //  8
	OREG_OT0,     //  9
	OREG_OT1,     // 10
	OREG_OT2,     // 11
	OREG_OT3,     // 12
	OREG_UNUSED3, // 13
	OREG_UNUSED4, // 14
	OREG_A0X      // 15 - all values of the 4 bits are used
};

static const int MASK_X = 0x008;
static const int MASK_Y = 0x004;
static const int MASK_Z = 0x002;
static const int MASK_W = 0x001;

enum VSH_ILU { // Dxbx note : ILU stands for 'Inverse Logic Unit' opcodes
	ILU_NOP = 0,
	ILU_MOV,
	ILU_RCP,
	ILU_RCC,
	ILU_RSQ,
	ILU_EXP,
	ILU_LOG,
	ILU_LIT // = 7 - all values of the 3 bits are used
};

enum VSH_MAC { // Dxbx note : MAC stands for 'Multiply And Accumulate' opcodes
	MAC_NOP = 0,
	MAC_MOV,
	MAC_MUL,
	MAC_ADD,
	MAC_MAD,
	MAC_DP3,
	MAC_DPH,
	MAC_DP4,
	MAC_DST,
	MAC_MIN,
	MAC_MAX,
	MAC_SLT,
	MAC_SGE,
	MAC_ARL
	// ??? 14
	// ??? 15 - 2 values of the 4 bits are undefined
};

enum VSH_IMD_OUTPUT_TYPE {
	IMD_OUTPUT_C,
	IMD_OUTPUT_R,
	IMD_OUTPUT_O,
	IMD_OUTPUT_A0X
};

typedef struct _VSH_IMD_OUTPUT {
	VSH_IMD_OUTPUT_TYPE Type;
	int16_t             Address;
	int8_t              Mask;
} VSH_IMD_OUTPUT;

enum VSH_SWIZZLE {
	SWIZZLE_X = 0,
	SWIZZLE_Y,
	SWIZZLE_Z,
	SWIZZLE_W
};

enum VSH_PARAMETER_TYPE {
	PARAM_UNKNOWN = 0,
	PARAM_R,          // Temporary (scRatch) registers
	PARAM_V,          // Vertex registers
	PARAM_C,          // Constant registers, set by SetVertexShaderConstant
	PARAM_O // = 0??
};

typedef struct _VSH_IMD_PARAMETER {
	VSH_PARAMETER_TYPE  ParameterType;   // Parameter type, R, V or C
	bool                Neg;             // true if negated, false if not
	VSH_SWIZZLE         Swizzle[4];      // The four swizzles
	int16_t             Address;         // Register address
} VSH_IMD_PARAMETER;

typedef struct _VSH_INTERMEDIATE_FORMAT {
	VSH_MAC                  MAC;
	VSH_ILU                  ILU;
	VSH_IMD_OUTPUT           Output;
	unsigned                 ParamCount;
	VSH_IMD_PARAMETER        Parameters[3];
	// There is only a single address register in Microsoft DirectX 8.0.
	// The address register, designated as a0.x, may be used as signed
	// integer offset in relative addressing into the constant register file.
	//     c[a0.x + n]
	bool                     IndexesWithA0_X;
} VSH_INTERMEDIATE_FORMAT;

typedef struct _IntermediateVertexShader {
	XTL::X_VSH_SHADER_HEADER Header;
	std::vector<VSH_INTERMEDIATE_FORMAT> Instructions;
} IntermediateVertexShader;

// parse xbox vertex shader function into an intermediate format
extern void EmuParseVshFunction
(
	DWORD* pXboxFunction,
	DWORD* pXboxFunctionSize,
	IntermediateVertexShader* pShader
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
