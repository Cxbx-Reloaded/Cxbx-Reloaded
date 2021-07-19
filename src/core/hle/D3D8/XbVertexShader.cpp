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
// *  (c) 2002-2004 Aaron Robinson <caustik@caustik.com>
// *                Kingofc <kingofc@freenet.de>
// *
// *  All rights reserved
// *
// ******************************************************************
#define LOG_PREFIX CXBXR_MODULE::VTXSH

//#define _DEBUG_TRACK_VS

#include "common\util\hasher.h" // For ComputeHash
#include "core\kernel\init\CxbxKrnl.h"
#include "core\kernel\support\Emu.h"
#include "core\hle\D3D8\Direct3D9\Direct3D9.h" // For g_Xbox_VertexShader_Handle
#include "core\hle\D3D8\Direct3D9\RenderStates.h" // For XboxRenderStateConverter
#include "core\hle\D3D8\Direct3D9\VertexShaderSource.h" // For g_VertexShaderSource
#include "core\hle\D3D8\XbVertexBuffer.h" // For CxbxImpl_SetVertexData4f
#include "core\hle\D3D8\XbVertexShader.h"
#include "core\hle\D3D8\XbD3D8Logging.h" // For DEBUG_D3DRESULT
#include "devices\xbox.h"
#include "core\hle\D3D8\XbConvert.h" // For NV2A_VP_UPLOAD_INST, NV2A_VP_UPLOAD_CONST_ID, NV2A_VP_UPLOAD_CONST
#include "devices\video\nv2a.h" // For D3DPUSH_DECODE
#include "common\Logging.h" // For LOG_INIT
#include "common\Settings.hpp" // for g_LibVersion_D3D8

#include "XbD3D8Types.h" // For X_D3DVSDE_*
#include <sstream>
#include <unordered_map>
#include <array>
#include <bitset>

// External symbols :
extern xbox::X_STREAMINPUT g_Xbox_SetStreamSource[X_VSH_MAX_STREAMS]; // Declared in XbVertexBuffer.cpp
extern XboxRenderStateConverter XboxRenderStates; // Declared in Direct3D9.cpp

// Variables set by [D3DDevice|CxbxImpl]_SetVertexShaderInput() :
                      unsigned g_Xbox_SetVertexShaderInput_Count = 0; // Read by GetXboxVertexAttributes
           xbox::X_STREAMINPUT g_Xbox_SetVertexShaderInput_Data[X_VSH_MAX_STREAMS] = { 0 }; // Active when g_Xbox_SetVertexShaderInput_Count > 0
 xbox::X_VERTEXATTRIBUTEFORMAT g_Xbox_SetVertexShaderInput_Attributes = { 0 }; // Read by GetXboxVertexAttributes when g_Xbox_SetVertexShaderInput_Count > 0

VertexShaderMode g_Xbox_VertexShaderMode = VertexShaderMode::FixedFunction;

                xbox::dword_xt g_Xbox_VertexShader_Handle = 0;
#ifdef CXBX_USE_GLOBAL_VERTEXSHADER_POINTER // TODO : Would this be more accurate / simpler?
      xbox::X_D3DVertexShader *g_Xbox_VertexShader_Ptr = nullptr;
#endif
                xbox::dword_xt g_Xbox_VertexShader_FunctionSlots_StartAddress = 0;

// Variable set by [D3DDevice|CxbxImpl]_LoadVertexShader() / [D3DDevice|CxbxImpl]_LoadVertexShaderProgram() (both through CxbxCopyVertexShaderFunctionSlots):
                xbox::dword_xt g_Xbox_VertexShader_FunctionSlots[(X_VSH_MAX_INSTRUCTION_COUNT + 1) * X_VSH_INSTRUCTION_SIZE] = { 0 }; // One extra for FLD_FINAL terminator

// Variables set by [D3DDevice|CxbxImpl]_SetScreenSpaceOffset:
				         float g_Xbox_ScreenSpaceOffset_x = 0.0f;
				         float g_Xbox_ScreenSpaceOffset_y = 0.0f;


static xbox::X_D3DVertexShader g_Xbox_VertexShader_ForFVF = {};

static uint32_t                g_X_VERTEXSHADER_FLAG_PROGRAM; // X_VERTEXSHADER_FLAG_PROGRAM flag varies per XDK, so it is set on runtime
static uint32_t                g_X_VERTEXSHADER_FLAG_VALID_MASK; // For a test case

void CxbxVertexShaderSetFlags()
{
	// Set an appropriate X_VERTEXSHADER_FLAG_PROGRAM version and mask off the "wrong" one
	// to allow the test case to spot it
	if (g_LibVersion_D3D8 <= 3948) {
		g_X_VERTEXSHADER_FLAG_PROGRAM = X_VERTEXSHADER3948_FLAG_PROGRAM;
		g_X_VERTEXSHADER_FLAG_VALID_MASK = ~X_VERTEXSHADER_FLAG_PROGRAM;
	}
	else {
		g_X_VERTEXSHADER_FLAG_PROGRAM = X_VERTEXSHADER_FLAG_PROGRAM;
		g_X_VERTEXSHADER_FLAG_VALID_MASK = ~X_VERTEXSHADER3948_FLAG_PROGRAM;
	}
}

// Converts an Xbox FVF shader handle to X_D3DVertexShader
// Note : Temporary, until we reliably locate the Xbox internal state for this
// See D3DXDeclaratorFromFVF docs https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dxdeclaratorfromfvf
// and https://github.com/reactos/wine/blob/2e8dfbb1ad71f24c41e8485a39df01bb9304127f/dlls/d3dx9_36/mesh.c#L2041
static xbox::X_D3DVertexShader* XboxVertexShaderFromFVF(DWORD xboxFvf) // TODO : Rename CxbxFVFToXboxVertexAttributeFormat?
{
	using namespace xbox;

	// Note : FVFs don't tessellate, all slots read from stream zero, therefore
	// the following zero-initialization of StreamIndex (like all other fields)
	// is never updated below.
	g_Xbox_VertexShader_ForFVF = { 0 };

	// Shorthand, glueing earlier implementation below to global g_Xbox_VertexShader_ForFVF variable :
	X_VERTEXATTRIBUTEFORMAT &declaration = g_Xbox_VertexShader_ForFVF.VertexAttribute;

	static DWORD X_D3DVSDT_FLOAT[] = { 0, X_D3DVSDT_FLOAT1, X_D3DVSDT_FLOAT2, X_D3DVSDT_FLOAT3, X_D3DVSDT_FLOAT4 };

	static const DWORD InvalidXboxFVFBits = X_D3DFVF_RESERVED0 | X_D3DFVF_RESERVED1 /* probably D3DFVF_PSIZE if detected */
		| 0x0000F000 // Bits between texture count and the texture formats
		| 0xFF000000; // All bits above the four alllowed texture formats

	if (xboxFvf & InvalidXboxFVFBits) {
		// Test-case : JSRF (after "now Loading...") TODO : Figure out what's going on
		// LOG_TEST_CASE("Invalid Xbox FVF bits detected!");
	}

	// Position & Blendweights
	int nrPositionFloats = 3;
	int nrBlendWeights = 0;
	unsigned offset = 0;
	DWORD position = (xboxFvf & X_D3DFVF_POSITION_MASK);
	switch (position) {
		case 0: nrPositionFloats = 0; LOG_TEST_CASE("FVF without position"); break; // Note : Remove logging if this occurs often
		case X_D3DFVF_XYZ: /*nrPositionFloats is set to 3 by default*/ break;
		case X_D3DFVF_XYZRHW: nrPositionFloats = 4; g_Xbox_VertexShader_ForFVF.Flags |= X_VERTEXSHADER_FLAG_PASSTHROUGH; break;
		case X_D3DFVF_XYZB1: nrBlendWeights = 1; break;
		case X_D3DFVF_XYZB2: nrBlendWeights = 2; break;
		case X_D3DFVF_XYZB3: nrBlendWeights = 3; break;
		case X_D3DFVF_XYZB4: nrBlendWeights = 4; break;
		case X_D3DFVF_POSITION_MASK: /*Keep nrPositionFloats set to 3*/ LOG_TEST_CASE("FVF invalid (5th blendweight?)"); break;
		DEFAULT_UNREACHABLE;
	}

	// Assign vertex element (attribute) slots
	X_VERTEXSHADERINPUT* pSlot;

	// Write Position
	if (nrPositionFloats > 0) {
		pSlot = &declaration.Slots[X_D3DVSDE_POSITION];
		pSlot->Format = X_D3DVSDT_FLOAT[nrPositionFloats];
		pSlot->Offset = offset;
		offset += sizeof(float) * nrPositionFloats;
		// Write Blend Weights
		if (nrBlendWeights > 0) {
			pSlot = &declaration.Slots[X_D3DVSDE_BLENDWEIGHT];
			pSlot->Format = X_D3DVSDT_FLOAT[nrBlendWeights];
			pSlot->Offset = offset;
			offset += sizeof(float) * nrBlendWeights;
		}
	}
	else if (nrBlendWeights > 0) LOG_TEST_CASE("BlendWeights given without position?");

	// Write Normal, Diffuse, and Specular
	if (xboxFvf & X_D3DFVF_NORMAL) {
		if (position == X_D3DFVF_XYZRHW) {
			LOG_TEST_CASE("X_D3DFVF_NORMAL shouldn't use X_D3DFVF_XYZRHW");
		}

		pSlot = &declaration.Slots[X_D3DVSDE_NORMAL];
		pSlot->Format = X_D3DVSDT_FLOAT[3];
		pSlot->Offset = offset;
		offset += sizeof(float) * 3;
	}

	if (xboxFvf & X_D3DFVF_DIFFUSE) {
		g_Xbox_VertexShader_ForFVF.Flags |= X_VERTEXSHADER_FLAG_HASDIFFUSE;
		pSlot = &declaration.Slots[X_D3DVSDE_DIFFUSE];
		pSlot->Format = X_D3DVSDT_D3DCOLOR;
		pSlot->Offset = offset;
		offset += sizeof(DWORD) * 1;
	}

	if (xboxFvf & X_D3DFVF_SPECULAR) {
		g_Xbox_VertexShader_ForFVF.Flags |= X_VERTEXSHADER_FLAG_HASSPECULAR; 
		pSlot = &declaration.Slots[X_D3DVSDE_SPECULAR];
		pSlot->Format = X_D3DVSDT_D3DCOLOR;
		pSlot->Offset = offset;
		offset += sizeof(DWORD) * 1;
	}

	// Write Texture Coordinates
	int textureCount = (xboxFvf & X_D3DFVF_TEXCOUNT_MASK) >> X_D3DFVF_TEXCOUNT_SHIFT;
	if (textureCount > 4) {
		LOG_TEST_CASE("Limiting FVF to 4 textures");
		textureCount = 4; // Safeguard, since the X_D3DFVF_TEXCOUNT bitfield could contain invalid values (5 up to 15)
	}

	for (int i = 0; i < textureCount; i++) {
		auto FVFTextureFormat = (xboxFvf >> X_D3DFVF_TEXCOORDSIZE_SHIFT(i)) & 0x003;
#if 1
		int numberOfCoordinates = ((FVFTextureFormat + 1) & 3) + 1;
#else
		int numberOfCoordinates = 0;
		switch (FVFTextureFormat) { // Note : Below enums are not ordered; In a math expression mapped as :
			case X_D3DFVF_TEXTUREFORMAT1: numberOfCoordinates = 1; break; // input = 3 -> 4 -> 0 -> 1 = output
			case X_D3DFVF_TEXTUREFORMAT2: numberOfCoordinates = 2; break; // input = 0 -> 1 -> 1 -> 2 = output
			case X_D3DFVF_TEXTUREFORMAT3: numberOfCoordinates = 3; break; // input = 1 -> 2 -> 2 -> 3 = output
			case X_D3DFVF_TEXTUREFORMAT4: numberOfCoordinates = 4; break; // input = 2 -> 3 -> 3 -> 4 = output
			DEFAULT_UNREACHABLE;                                          // ((input   +1 ) &3 ) +1 ) = output
		}

		assert(numberOfCoordinates > 0);
#endif
		pSlot = &declaration.Slots[X_D3DVSDE_TEXCOORD0 + i];
		pSlot->Format = X_D3DVSDT_FLOAT[numberOfCoordinates];
		pSlot->Offset = offset;
		offset += sizeof(float) * numberOfCoordinates;
		// Update the VertexShader texture Dimensionality field here as well
		g_Xbox_VertexShader_ForFVF.Dimensionality[i] = numberOfCoordinates;
	}

	// Make sure all unused slots have a X_D3DVSDT_NONE format
	// TODO : Actually, maybe not, since this could avoid VshConvertToken_STREAMDATA_REG() calls!
	for (unsigned i = 0; i < X_VSH_MAX_ATTRIBUTES; i++) {
		pSlot = &declaration.Slots[i];
		if (pSlot->Format == 0) {
			pSlot->Format = X_D3DVSDT_NONE;
		}
	}

	// Return the global g_Xbox_VertexShader_ForFVF variable 
	return &g_Xbox_VertexShader_ForFVF;
}

static xbox::X_D3DVertexShader* CxbxGetXboxVertexShaderForHandle(DWORD Handle)
{
	if (VshHandleIsVertexShader(Handle)) {
		return VshHandleToXboxVertexShader(Handle);
	} else {
		return XboxVertexShaderFromFVF(Handle);
	}
}

// TODO : Start using this function everywhere g_Xbox_VertexShader_Handle is accessed currently!
xbox::X_D3DVertexShader* GetXboxVertexShader()
{
	// LOG_INIT; // Allows use of DEBUG_D3DRESULT

	using namespace xbox;

	X_D3DVertexShader* pXboxVertexShader = xbox::zeroptr;
#if 0 // TODO : Retrieve vertex shader from actual Xbox D3D state
	// Only when we're sure of the location of the Xbox Device.m_pVertexShader variable
	if (XboxVertexShaders.g_XboxAddr_pVertexShader) {
		// read that (so that we get access to internal vertex shaders, like those generated
		// to contain the attribute-information for FVF shaders) :
		pXboxVertexShader = (X_D3DVertexShader*)(*XboxVertexShaders.g_XboxAddr_pVertexShader);
	}
	else
	{
		LOG_TEST_CASE("Unknown pVertexShader symbol location!");
#endif
		// Otherwise, we have no choice but to use what we've last stored in the
		// g_Xbox_VertexShader_Handle variable via our D3DDevice_SetVertexShader
		// and D3DDevice_SelectVertexShader* patches.

		// Note, that once we have a fail-safe way to determine the location of the
		// Xbox Device.m_pVertexShader symbol, the FVF and the accompanying Address,
		// we no longer need this statement block, nor patches on D3DDevice_SetVertexShader
		// nor D3DDevice_SelectVertexShader* !

		// Now, to convert, we do need to have a valid vertex shader :
		if (g_Xbox_VertexShader_Handle == 0) {
			LOG_TEST_CASE("Unassigned Xbox vertex shader!");
			return nullptr;
		}

#if 0 // TODO : Retrieve vertex shader from actual Xbox D3D state
		if (!VshHandleIsVertexShader(g_Xbox_VertexShader_Handle)) {
			LOG_TEST_CASE("Xbox vertex shader lacks X_D3DFVF_RESERVED0 bit!");
			return nullptr;
		}
#endif

#ifdef CXBX_USE_GLOBAL_VERTEXSHADER_POINTER
		pXboxVertexShader = g_Xbox_VertexShader_Ptr;
#else
		pXboxVertexShader = CxbxGetXboxVertexShaderForHandle(g_Xbox_VertexShader_Handle);
#endif

#if 0 // TODO : Retrieve vertex shader from actual Xbox D3D state
	}
#endif

	return pXboxVertexShader;
}

static bool UseXboxD3DVertexShaderTypeForVersion3948(const xbox::X_D3DVertexShader* pXboxVertexShader)
{
	// Don't check XDK version for our internal FVF vertex shader
	// because g_Xbox_VertexShader_ForFVF is an internal variable
	// that's compiled in as a xbox::X_D3DVertexShader
	if (pXboxVertexShader == &g_Xbox_VertexShader_ForFVF) {
		return false;
	}

	return g_LibVersion_D3D8 <= 3948;
}

static xbox::X_VERTEXATTRIBUTEFORMAT* CxbxGetVertexShaderAttributes(xbox::X_D3DVertexShader* pXboxVertexShader)
{
	if (UseXboxD3DVertexShaderTypeForVersion3948(pXboxVertexShader)) {
		auto pXboxVertexShader3948 = reinterpret_cast<xbox::X_D3DVertexShader3948*>(pXboxVertexShader);
		return &(pXboxVertexShader3948->VertexAttribute);
	}

	return &(pXboxVertexShader->VertexAttribute);
}

static DWORD* CxbxGetVertexShaderTokens(xbox::X_D3DVertexShader* pXboxVertexShader, DWORD* pNrTokens)
{
	if (UseXboxD3DVertexShaderTypeForVersion3948(pXboxVertexShader)) {
		auto pXboxVertexShader3948 = reinterpret_cast<xbox::X_D3DVertexShader3948*>(pXboxVertexShader);
		*pNrTokens = pXboxVertexShader3948->ProgramAndConstantsDwords;
		return &pXboxVertexShader3948->ProgramAndConstants[0];
	}

	*pNrTokens = pXboxVertexShader->ProgramAndConstantsDwords;
	return &pXboxVertexShader->ProgramAndConstants[0];
}

int GetXboxVertexDataComponentCount(int d3dvsdt) {
	using namespace xbox;
	switch (d3dvsdt) {
	case X_D3DVSDT_NORMPACKED3:
		return 3;
	case X_D3DVSDT_FLOAT2H:
		LOG_TEST_CASE("Attempting to use component count for X_D3DVSDT_FLOAT2H, which uses an odd (value, value, 0, value) layout");
		// This is a bit of an odd case. Will call it 4 since it writes a value to the 4th component...
		return 4;
	default:
		// Most data types have a representation consistent with the number of components
		const int countMask = 0x7;
		const int countShift = 4;
		return (d3dvsdt >> countShift) & countMask;
	}
}

extern bool g_InlineVertexBuffer_DeclarationOverride; // TMP glue
extern xbox::X_VERTEXATTRIBUTEFORMAT g_InlineVertexBuffer_AttributeFormat; // TMP glue

xbox::X_VERTEXATTRIBUTEFORMAT* GetXboxVertexAttributeFormat()
{
	// Special case for CxbxImpl_End() based drawing
	if (g_InlineVertexBuffer_DeclarationOverride) {
		return &g_InlineVertexBuffer_AttributeFormat;
	}

	xbox::X_D3DVertexShader* pXboxVertexShader = GetXboxVertexShader();
	if (pXboxVertexShader == xbox::zeroptr) {
		// Despite possibly not being used, the pXboxVertexShader argument must always be assigned
		LOG_TEST_CASE("Xbox should always have a VertexShader set (even for FVF's)");
		return &g_Xbox_SetVertexShaderInput_Attributes; // WRONG result, but it's already strange this happens
	}

	// If SetVertexShaderInput is active, its arguments overrule those of the active vertex shader
	if (g_Xbox_SetVertexShaderInput_Count > 0) {
		// Take overrides (on declarations and streaminputs, as optionally set by SetVertexShaderInput) into account :
		// Test-case : Crazy taxi 3
		LOG_TEST_CASE("SetVertexShaderInput_Attributes override in effect!");
		return &g_Xbox_SetVertexShaderInput_Attributes;
	}

	return CxbxGetVertexShaderAttributes(pXboxVertexShader);
}

// Reads the active Xbox stream input values (containing VertexBuffer, Offset and Stride) for the given stream index.
// (These values are set through SetStreamSource and can be overridden by SetVertexShaderInput.)
xbox::X_STREAMINPUT& GetXboxVertexStreamInput(unsigned XboxStreamNumber)
{
	// If SetVertexShaderInput is active, its arguments overrule those of SetStreamSource
	if (g_Xbox_SetVertexShaderInput_Count > 0) {
		return g_Xbox_SetVertexShaderInput_Data[XboxStreamNumber];
	}

	return g_Xbox_SetStreamSource[XboxStreamNumber];
}

#define DbgVshPrintf \
	LOG_CHECK_ENABLED(LOG_LEVEL::DEBUG) \
		if(g_bPrintfOn) printf

// ****************************************************************************
// * Vertex shader function recompiler
// ****************************************************************************

namespace XboxVertexShaderDecoder
{
	// Xbox Vertex SHader microcode types

	enum VSH_OUTPUT_TYPE {
		OUTPUT_C = 0,
		OUTPUT_O
	};

	enum VSH_OUTPUT_MUX {
		OMUX_MAC = 0,
		OMUX_ILU
	};

	// Host intermediate vertex shader types

	enum VSH_FIELD_NAME {
		FLD_ILU = 0,
		FLD_MAC,
		FLD_CONST,
		FLD_V,
		// Input A
		FLD_A_NEG,
		FLD_A_SWZ_X,
		FLD_A_SWZ_Y,
		FLD_A_SWZ_Z,
		FLD_A_SWZ_W,
		FLD_A_R,
		FLD_A_MUX,
		// Input B
		FLD_B_NEG,
		FLD_B_SWZ_X,
		FLD_B_SWZ_Y,
		FLD_B_SWZ_Z,
		FLD_B_SWZ_W,
		FLD_B_R,
		FLD_B_MUX,
		// Input C
		FLD_C_NEG,
		FLD_C_SWZ_X,
		FLD_C_SWZ_Y,
		FLD_C_SWZ_Z,
		FLD_C_SWZ_W,
		FLD_C_R_HIGH,
		FLD_C_R_LOW,
		FLD_C_MUX,
		// Output
		FLD_OUT_MAC_MASK,
		FLD_OUT_R,
		FLD_OUT_ILU_MASK,
		FLD_OUT_O_MASK,
		FLD_OUT_ORB,
		FLD_OUT_ADDRESS,
		FLD_OUT_MUX,
		// Relative addressing
		FLD_A0X,
		// Final instruction
		FLD_FINAL
	};

	// Retrieves a number of bits in the instruction token
	static inline uint32_t VshGetFromToken(
		uint32_t* pShaderToken,
		uint8_t SubToken,
		uint8_t StartBit,
		uint8_t BitLength)
	{
		return (pShaderToken[SubToken] >> StartBit) & ~(0xFFFFFFFF << BitLength);
	}

	static uint8_t VshGetField(
		uint32_t* pShaderToken,
		VSH_FIELD_NAME FieldName)
	{
		// Used for xvu spec definition
		static const struct {
			uint8_t          SubToken;
			uint8_t          StartBit;
			uint8_t          BitLength;
		} FieldMapping[/*VSH_FIELD_NAME*/] = {
			// SubToken BitPos  BitSize
			{  1,   25,     3 }, // FLD_ILU,              
			{  1,   21,     4 }, // FLD_MAC,              
			{  1,   13,     8 }, // FLD_CONST,            
			{  1,    9,     4 }, // FLD_V,                
			// Input A
			{  1,    8,     1 }, // FLD_A_NEG,            
			{  1,    6,     2 }, // FLD_A_SWZ_X,          
			{  1,    4,     2 }, // FLD_A_SWZ_Y,          
			{  1,    2,     2 }, // FLD_A_SWZ_Z,          
			{  1,    0,     2 }, // FLD_A_SWZ_W,          
			{  2,   28,     4 }, // FLD_A_R,              
			{  2,   26,     2 }, // FLD_A_MUX,            
			// Input B
			{  2,   25,     1 }, // FLD_B_NEG,            
			{  2,   23,     2 }, // FLD_B_SWZ_X,          
			{  2,   21,     2 }, // FLD_B_SWZ_Y,          
			{  2,   19,     2 }, // FLD_B_SWZ_Z,          
			{  2,   17,     2 }, // FLD_B_SWZ_W,          
			{  2,   13,     4 }, // FLD_B_R,              
			{  2,   11,     2 }, // FLD_B_MUX,            
			// Input C
			{  2,   10,     1 }, // FLD_C_NEG,            
			{  2,    8,     2 }, // FLD_C_SWZ_X,          
			{  2,    6,     2 }, // FLD_C_SWZ_Y,          
			{  2,    4,     2 }, // FLD_C_SWZ_Z,          
			{  2,    2,     2 }, // FLD_C_SWZ_W,          
			{  2,    0,     2 }, // FLD_C_R_HIGH,         
			{  3,   30,     2 }, // FLD_C_R_LOW,          
			{  3,   28,     2 }, // FLD_C_MUX,            
			// Output
			{  3,   24,     4 }, // FLD_OUT_MAC_MASK,   
			{  3,   20,     4 }, // FLD_OUT_R,            
			{  3,   16,     4 }, // FLD_OUT_ILU_MASK,
			{  3,   12,     4 }, // FLD_OUT_O_MASK,
			{  3,   11,     1 }, // FLD_OUT_ORB,          
			{  3,    3,     8 }, // FLD_OUT_ADDRESS,      
			{  3,    2,     1 }, // FLD_OUT_MUX,          
			// Relative addressing
			{  3,    1,     1 }, // FLD_A0X,              
			// Final instruction
			{  3,    0,     1 }  // FLD_FINAL,            
		};

		return (uint8_t)(VshGetFromToken(pShaderToken,
			FieldMapping[FieldName].SubToken,
			FieldMapping[FieldName].StartBit,
			FieldMapping[FieldName].BitLength));
	}

	// Converts the C register address to disassembly format
	static inline int16_t ConvertCRegister(const int16_t CReg)
	{
		return ((((CReg >> 5) & 7) - 3) * 32) + (CReg & 31);
	}

	static void VshConvertIntermediateParam(VSH_IMD_PARAMETER& Param,
		uint32_t* pShaderToken,
		VSH_FIELD_NAME FLD_MUX,
		VSH_FIELD_NAME FLD_NEG,
		uint16_t R,
		uint16_t V,
		uint16_t C)
	{
		Param.ParameterType = (VSH_PARAMETER_TYPE)VshGetField(pShaderToken, FLD_MUX);
		switch (Param.ParameterType) {
		case PARAM_R:
			Param.Address = R;
			break;
		case PARAM_V:
			Param.Address = V;
			break;
		case PARAM_C:
			Param.Address = C;
			break;
		default:
			LOG_TEST_CASE("parameter type unknown");
		}

		int d = FLD_NEG - FLD_A_NEG;
		Param.Neg = VshGetField(pShaderToken, (VSH_FIELD_NAME)(d + FLD_A_NEG)) > 0;
		Param.Swizzle[0] = (VSH_SWIZZLE)VshGetField(pShaderToken, (VSH_FIELD_NAME)(d + FLD_A_SWZ_X));
		Param.Swizzle[1] = (VSH_SWIZZLE)VshGetField(pShaderToken, (VSH_FIELD_NAME)(d + FLD_A_SWZ_Y));
		Param.Swizzle[2] = (VSH_SWIZZLE)VshGetField(pShaderToken, (VSH_FIELD_NAME)(d + FLD_A_SWZ_Z));
		Param.Swizzle[3] = (VSH_SWIZZLE)VshGetField(pShaderToken, (VSH_FIELD_NAME)(d + FLD_A_SWZ_W));
	}

	static void VshAddIntermediateInstruction(
		uint32_t* pShaderToken,
		IntermediateVertexShader* pShader,
		VSH_MAC MAC,
		VSH_ILU ILU,
		VSH_IMD_OUTPUT_TYPE output_type,
		int16_t output_address,
		int8_t output_mask)
	{
		// Is the output mask set?
		if (output_mask == 0) {
			return;
		}

		if (pShader->Instructions.size() >= VSH_MAX_INTERMEDIATE_COUNT) {
			CxbxrAbort("Shader exceeds conversion buffer!");
		}

		VSH_INTERMEDIATE_FORMAT intermediate;
		intermediate.MAC = MAC;
		intermediate.ILU = ILU;
		intermediate.Output.Type = output_type;
		intermediate.Output.Address = output_address;
		intermediate.Output.Mask = output_mask;
		// Get a0.x indirect constant addressing
		intermediate.IndexesWithA0_X = VshGetField(pShaderToken, FLD_A0X) > 0; // Applies to PARAM_C parameter reads

		int16_t R;
		int16_t V = VshGetField(pShaderToken, FLD_V);
		int16_t C = ConvertCRegister(VshGetField(pShaderToken, FLD_CONST));
		intermediate.ParamCount = 0;
		if (MAC >= MAC_MOV) {
			// Get parameter A
			R = VshGetField(pShaderToken, FLD_A_R);
			VshConvertIntermediateParam(intermediate.Parameters[intermediate.ParamCount++], pShaderToken, FLD_A_MUX, FLD_A_NEG, R, V, C);
		}

		if ((MAC == MAC_MUL) || ((MAC >= MAC_MAD) && (MAC <= MAC_SGE))) {
			// Get parameter B
			R = VshGetField(pShaderToken, FLD_B_R);
			VshConvertIntermediateParam(intermediate.Parameters[intermediate.ParamCount++], pShaderToken, FLD_B_MUX, FLD_B_NEG, R, V, C);
		}

		if ((ILU >= ILU_MOV) || (MAC == MAC_ADD) || (MAC == MAC_MAD)) {
			// Get parameter C
			R = VshGetField(pShaderToken, FLD_C_R_HIGH) << 2 | VshGetField(pShaderToken, FLD_C_R_LOW);
			VshConvertIntermediateParam(intermediate.Parameters[intermediate.ParamCount++], pShaderToken, FLD_C_MUX, FLD_C_NEG, R, V, C);
		}

		// Add the instruction to the shader
		pShader->Instructions.push_back(intermediate);
	}

	static bool VshConvertToIntermediate(uint32_t* pShaderToken, IntermediateVertexShader* pShader)
	{
		// First get the instruction(s).
		VSH_ILU ILU = (VSH_ILU)VshGetField(pShaderToken, FLD_ILU);
		VSH_MAC MAC = (VSH_MAC)VshGetField(pShaderToken, FLD_MAC);
		if (MAC > MAC_ARL) LOG_TEST_CASE("Unknown MAC");

		// Output register
		VSH_OUTPUT_MUX OutputMux = (VSH_OUTPUT_MUX)VshGetField(pShaderToken, FLD_OUT_MUX);
		int16_t OutputAddress = VshGetField(pShaderToken, FLD_OUT_ADDRESS);
		VSH_IMD_OUTPUT_TYPE OutputType;
		if ((VSH_OUTPUT_TYPE)VshGetField(pShaderToken, FLD_OUT_ORB) == OUTPUT_C) {
			OutputType = IMD_OUTPUT_C;
			OutputAddress = ConvertCRegister(OutputAddress);
		} else { // OUTPUT_O:
			OutputType = IMD_OUTPUT_O;
			OutputAddress = OutputAddress & 0xF;
		}

		// MAC,ILU output R register
		int16_t RAddress = VshGetField(pShaderToken, FLD_OUT_R);

		// Test for paired opcodes
		bool bIsPaired = (MAC != MAC_NOP) && (ILU != ILU_NOP);

		// Check if there's a MAC opcode
		if (MAC > MAC_NOP && MAC <= MAC_ARL) {
			if (bIsPaired && RAddress == 1) {
				// Ignore paired MAC opcodes that write to R1
			} else {
				if (MAC == MAC_ARL) {
					VshAddIntermediateInstruction(pShaderToken, pShader, MAC, ILU_NOP, IMD_OUTPUT_A0X, 0, MASK_X);
				} else {
					VshAddIntermediateInstruction(pShaderToken, pShader, MAC, ILU_NOP, IMD_OUTPUT_R, RAddress, VshGetField(pShaderToken, FLD_OUT_MAC_MASK));
				}
			}

			// Check if we must add a muxed MAC opcode as well
			if (OutputMux == OMUX_MAC) {
				VshAddIntermediateInstruction(pShaderToken, pShader, MAC, ILU_NOP, OutputType, OutputAddress, VshGetField(pShaderToken, FLD_OUT_O_MASK));
			}
		}

		// Check if there's an ILU opcode
		if (ILU != ILU_NOP) {
			// Paired ILU opcodes will only write to R1
			VshAddIntermediateInstruction(pShaderToken, pShader, MAC_NOP, ILU, IMD_OUTPUT_R, bIsPaired ? 1 : RAddress, VshGetField(pShaderToken, FLD_OUT_ILU_MASK));
			// Check if we must add a muxed ILU opcode as well
			if (OutputMux == OMUX_ILU) {
				VshAddIntermediateInstruction(pShaderToken, pShader, MAC_NOP, ILU, OutputType, OutputAddress, VshGetField(pShaderToken, FLD_OUT_O_MASK));
			}
		}

		return VshGetField(pShaderToken, FLD_FINAL) == 0;
	}
};

// Get the function size excluding the final field
size_t GetVshFunctionSize(const xbox::dword_xt* pXboxFunction) {
	auto curToken = (uint32_t*)pXboxFunction;

	while (!XboxVertexShaderDecoder::VshGetField(curToken, XboxVertexShaderDecoder::FLD_FINAL)) {
		curToken += X_VSH_INSTRUCTION_SIZE; // TODO use a struct to represent these instructions
	}

	curToken += X_VSH_INSTRUCTION_SIZE; // For the final instruction

	return (curToken - pXboxFunction) * sizeof(xbox::dword_xt);
}

// ****************************************************************************
// * Vertex shader declaration recompiler
// ****************************************************************************

#ifndef CXBX_USE_D3D11
extern D3DCAPS g_D3DCaps;
#endif

class XboxVertexDeclarationConverter
{
protected:
	// Internal variables
	CxbxVertexDeclaration* pCurrentVertexDeclaration;
	CxbxVertexShaderStreamInfo* pCurrentVertexShaderStreamInfo = nullptr;
	bool IsFixedFunction;
	D3DVERTEXELEMENT* pCurrentHostVertexElement;
	std::array<bool, 16> RegVIsPresentInDeclaration;

private:
	#define D3DDECLUSAGE_UNSUPPORTED ((D3DDECLUSAGE)-1)

	D3DDECLUSAGE Xb2PCRegisterType(DWORD VertexRegister, BYTE &UsageIndex)
	{
		UsageIndex = 0;
		switch (VertexRegister) {
			case xbox::X_D3DVSDE_POSITION    /*= 0*/:                 return D3DDECLUSAGE_POSITION;
			case xbox::X_D3DVSDE_BLENDWEIGHT /*= 1*/:                 return D3DDECLUSAGE_BLENDWEIGHT;
			case xbox::X_D3DVSDE_NORMAL      /*= 2*/:                 return D3DDECLUSAGE_NORMAL;
			case xbox::X_D3DVSDE_DIFFUSE     /*= 3*/:                 return D3DDECLUSAGE_COLOR;
			case xbox::X_D3DVSDE_SPECULAR    /*= 4*/: UsageIndex = 1; return D3DDECLUSAGE_COLOR;
			case xbox::X_D3DVSDE_FOG         /*= 5*/:                 return D3DDECLUSAGE_FOG;
			case xbox::X_D3DVSDE_POINTSIZE   /*= 6*/:                 return D3DDECLUSAGE_PSIZE;
			case xbox::X_D3DVSDE_BACKDIFFUSE /*= 7*/: UsageIndex = 2; return D3DDECLUSAGE_COLOR;
			case xbox::X_D3DVSDE_BACKSPECULAR/*= 8*/: UsageIndex = 3; return D3DDECLUSAGE_COLOR;
			case xbox::X_D3DVSDE_TEXCOORD0   /*= 9*/:                 return D3DDECLUSAGE_TEXCOORD;
			case xbox::X_D3DVSDE_TEXCOORD1   /*=10*/: UsageIndex = 1; return D3DDECLUSAGE_TEXCOORD;
			case xbox::X_D3DVSDE_TEXCOORD2   /*=11*/: UsageIndex = 2; return D3DDECLUSAGE_TEXCOORD;
			case xbox::X_D3DVSDE_TEXCOORD3   /*=12*/: UsageIndex = 3; return D3DDECLUSAGE_TEXCOORD;
			// Unused registers. Assign them a semantic anyway so we don't have to worry about
			// binding a vertex declaration with them in it
			case 13: UsageIndex = 4; return D3DDECLUSAGE_TEXCOORD;
			case 14: UsageIndex = 5; return D3DDECLUSAGE_TEXCOORD;
			case 15: UsageIndex = 6; return D3DDECLUSAGE_TEXCOORD;
			default:
				return D3DDECLUSAGE_UNSUPPORTED;
		}
	}

	// VERTEX SHADER

	bool VshConvertToken_STREAMDATA_REG(DWORD VertexRegister, xbox::X_VERTEXSHADERINPUT &slot)
	{
		DWORD XboxVertexElementDataType = slot.Format;

		// Does this attribute use no storage present the vertex (check this as early as possible to avoid needless processing) ?
		if (XboxVertexElementDataType == xbox::X_D3DVSDT_NONE) {
			// Handle tessellating attributes
			switch (slot.TessellationType) {
			case 0: return false; // AUTONONE
			case 1: // AUTONORMAL
				// Note : .Stream, .Offset and .Type are copied from pAttributeSlot->TessellationSource in a post-processing step below,
				// because these could all go through an Xbox to host conversion step, so must be copied over afterwards.
				pCurrentHostVertexElement->Method = D3DDECLMETHOD_CROSSUV; // for D3DVSD_TESSNORMAL
				pCurrentHostVertexElement->Usage = D3DDECLUSAGE_NORMAL; // TODO : Is this correct?
				pCurrentHostVertexElement->_9_11(UsageIndex, SemanticIndex) = 0; // Note : 1 would be wrong
				return true;
			case 2: // AUTOTEXCOORD
				// pCurrentHostVertexElement->Stream = 0; // The input stream is unused (but must be set to 0), which is the current default value
				// pCurrentHostVertexElement->_9_11(Offset, AlignedByteOffset) = 0; // The input offset is unused (but must be set to 0), which is the current default value
				pCurrentHostVertexElement->_9_11(Type, Format) = _9_11(D3DDECLTYPE_UNUSED, DXGI_FORMAT_UNKNOWN); // The input type for D3DDECLMETHOD_UV must be D3DDECLTYPE_UNUSED (the output type implied by D3DDECLMETHOD_UV is D3DDECLTYPE_FLOAT2)
				pCurrentHostVertexElement->Method = D3DDECLMETHOD_UV; // For X_D3DVSD_MASK_TESSUV
				pCurrentHostVertexElement->Usage = D3DDECLUSAGE_NORMAL; // Note : In Fixed Function Vertex Pipeline, D3DDECLMETHOD_UV must specify usage D3DDECLUSAGE_TEXCOORD or D3DDECLUSAGE_BLENDWEIGHT. TODO : So, what to do?
				pCurrentHostVertexElement->_9_11(UsageIndex, SemanticIndex) = 1; // TODO ; Is this correct?
				return true;
			default:
				LOG_TEST_CASE("invalid TessellationType");
				return false;
			}
		}

		WORD XboxVertexElementByteSize = 0; // When set above 0, implies NeedPatching
		_9_11(BYTE, DXGI_FORMAT) HostVertexElementDataType = _9_11(0, DXGI_FORMAT_UNKNOWN);
		WORD HostVertexElementByteSize = 0;

		switch (XboxVertexElementDataType) {
		case xbox::X_D3DVSDT_FLOAT1: // 0x12:
			HostVertexElementDataType = _9_11(D3DDECLTYPE_FLOAT1, DXGI_FORMAT_R32_FLOAT);
			HostVertexElementByteSize = 1 * sizeof(FLOAT);
			break;
		case xbox::X_D3DVSDT_FLOAT2: // 0x22:
			HostVertexElementDataType = _9_11(D3DDECLTYPE_FLOAT2, DXGI_FORMAT_R32G32_FLOAT);
			HostVertexElementByteSize = 2 * sizeof(FLOAT);
			break;
		case xbox::X_D3DVSDT_FLOAT3: // 0x32:
			HostVertexElementDataType = _9_11(D3DDECLTYPE_FLOAT3, DXGI_FORMAT_R32G32B32_FLOAT);
			HostVertexElementByteSize = 3 * sizeof(FLOAT);
			break;
		case xbox::X_D3DVSDT_FLOAT4: // 0x42:
			HostVertexElementDataType = _9_11(D3DDECLTYPE_FLOAT4, DXGI_FORMAT_R32G32B32A32_FLOAT);
			HostVertexElementByteSize = 4 * sizeof(FLOAT);
			break;
		case xbox::X_D3DVSDT_D3DCOLOR: // 0x40:
			HostVertexElementDataType = _9_11(D3DDECLTYPE_D3DCOLOR, DXGI_FORMAT_R8G8B8A8_UNORM); // D3D11 docs state: "not available"?
			HostVertexElementByteSize = 1 * sizeof(D3DCOLOR);
			break;
		case xbox::X_D3DVSDT_SHORT2: // 0x25:
			HostVertexElementDataType = _9_11(D3DDECLTYPE_SHORT2, DXGI_FORMAT_R16G16_SINT);
			HostVertexElementByteSize = 2 * sizeof(SHORT);
			break;
		case xbox::X_D3DVSDT_SHORT4: // 0x45:
			HostVertexElementDataType = _9_11(D3DDECLTYPE_SHORT4, DXGI_FORMAT_R16G16B16A16_SINT);
			HostVertexElementByteSize = 4 * sizeof(SHORT);
			break;
		case xbox::X_D3DVSDT_NORMSHORT1: // 0x11:
			// Test-cases : Halo - Combat Evolved
#ifdef CXBX_USE_D3D11
			HostVertexElementDataType = DXGI_FORMAT_R16_SNORM;
			HostVertexElementByteSize = 1 * sizeof(SHORT);
#else
			if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT2N) {
				HostVertexElementDataType = D3DDECLTYPE_SHORT2N;
				HostVertexElementByteSize = 2 * sizeof(SHORT);
			} else {
				HostVertexElementDataType = D3DDECLTYPE_FLOAT1;
				HostVertexElementByteSize = 1 * sizeof(FLOAT);
			}
			XboxVertexElementByteSize = 1 * sizeof(xbox::short_xt);
#endif
			break;
		case xbox::X_D3DVSDT_NORMSHORT2: // 0x21:
			// Test-cases : Baldur's Gate: Dark Alliance 2, F1 2002, Gun, Halo - Combat Evolved, Scrapland
#ifdef CXBX_USE_D3D11
			HostVertexElementDataType = DXGI_FORMAT_R16G16_SNORM;
			HostVertexElementByteSize = 2 * sizeof(SHORT);
#else
			if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT2N) {
				HostVertexElementDataType = D3DDECLTYPE_SHORT2N;
				HostVertexElementByteSize = 2 * sizeof(SHORT);
				// No need for patching in D3D9
			} else {
				HostVertexElementDataType = D3DDECLTYPE_FLOAT2;
				HostVertexElementByteSize = 2 * sizeof(FLOAT);
				XboxVertexElementByteSize = 2 * sizeof(xbox::short_xt);
			}
#endif
			break;
		case xbox::X_D3DVSDT_NORMSHORT3: // 0x31:
			// Test-cases : Cel Damage, Constantine, Destroy All Humans!
#ifdef CXBX_USE_D3D11
			HostVertexElementDataType = DXGI_FORMAT_R16G16B16A16_SNORM;
			HostVertexElementByteSize = 4 * sizeof(SHORT);
#else
			if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT4N) {
				HostVertexElementDataType = D3DDECLTYPE_SHORT4N;
				HostVertexElementByteSize = 4 * sizeof(SHORT);
			} else {
				HostVertexElementDataType = D3DDECLTYPE_FLOAT3;
				HostVertexElementByteSize = 3 * sizeof(FLOAT);
			}
#endif
			XboxVertexElementByteSize = 3 * sizeof(xbox::short_xt);
			break;
		case xbox::X_D3DVSDT_NORMSHORT4: // 0x41:
			// Test-cases : Judge Dredd: Dredd vs Death, NHL Hitz 2002, Silent Hill 2, Sneakers, Tony Hawk Pro Skater 4
#ifdef CXBX_USE_D3D11
			HostVertexElementDataType = DXGI_FORMAT_R16G16B16A16_SNORM;
			HostVertexElementByteSize = 4 * sizeof(SHORT);
#else
			if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT4N) {
				HostVertexElementDataType = D3DDECLTYPE_SHORT4N;
				HostVertexElementByteSize = 4 * sizeof(SHORT);
				// No need for patching in D3D9
			} else {
				HostVertexElementDataType = D3DDECLTYPE_FLOAT4;
				HostVertexElementByteSize = 4 * sizeof(FLOAT);
				XboxVertexElementByteSize = 4 * sizeof(xbox::short_xt);
			}
#endif
			break;
		case xbox::X_D3DVSDT_NORMPACKED3: // 0x16:
			// Test-cases : Dashboard
			HostVertexElementDataType = _9_11(D3DDECLTYPE_FLOAT3, DXGI_FORMAT_R32G32B32_FLOAT); // D3D11 Note : DXGI_FORMAT_R11G11B10_FLOAT can't be used (it contains floats, while X_D3DVSDT_NORMPACKED3 contains ints) and DXGI_FORMAT_R11G11B10_SNORM doesn't exist
			HostVertexElementByteSize = 3 * sizeof(FLOAT);
			XboxVertexElementByteSize = 1 * sizeof(xbox::dword_xt);
			break;
		case xbox::X_D3DVSDT_SHORT1: // 0x15:
#ifdef CXBX_USE_D3D11
			HostVertexElementDataType = DXGI_FORMAT_R16_SINT;
			HostVertexElementByteSize = 1 * sizeof(SHORT);
#else
			HostVertexElementDataType = D3DDECLTYPE_SHORT2;
			HostVertexElementByteSize = 2 * sizeof(SHORT);
			XboxVertexElementByteSize = 1 * sizeof(xbox::short_xt);
#endif
			break;
		case xbox::X_D3DVSDT_SHORT3: // 0x35:
			// Test-cases : Turok
			HostVertexElementDataType = _9_11(D3DDECLTYPE_SHORT4, DXGI_FORMAT_R16G16B16A16_SINT);
			HostVertexElementByteSize = 4 * sizeof(SHORT);
			XboxVertexElementByteSize = 3 * sizeof(xbox::short_xt);
			break;
		case xbox::X_D3DVSDT_PBYTE1: // 0x14:
#ifdef CXBX_USE_D3D11
			HostVertexElementDataType = DXGI_FORMAT_R8_UNORM;
			HostVertexElementByteSize = 1 * sizeof(BYTE);
#else
			if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
				HostVertexElementDataType = D3DDECLTYPE_UBYTE4N;
				HostVertexElementByteSize = 4 * sizeof(BYTE);
			} else {
				HostVertexElementDataType = D3DDECLTYPE_FLOAT1;
				HostVertexElementByteSize = 1 * sizeof(FLOAT);
			}
			XboxVertexElementByteSize = 1 * sizeof(xbox::byte_xt);
#endif
			break;
		case xbox::X_D3DVSDT_PBYTE2: // 0x24:
#ifdef CXBX_USE_D3D11
			HostVertexElementDataType = DXGI_FORMAT_R8G8_UNORM;
			HostVertexElementByteSize = 2 * sizeof(BYTE);
#else
			if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
				HostVertexElementDataType = D3DDECLTYPE_UBYTE4N;
				HostVertexElementByteSize = 4 * sizeof(BYTE);
			} else {
				HostVertexElementDataType = D3DDECLTYPE_FLOAT2;
				HostVertexElementByteSize = 2 * sizeof(FLOAT);
			}
			XboxVertexElementByteSize = 2 * sizeof(xbox::byte_xt);
#endif
			break;
		case xbox::X_D3DVSDT_PBYTE3: // 0x34:
			// Test-cases : Turok
#ifdef CXBX_USE_D3D11
			HostVertexElementDataType = DXGI_FORMAT_R8G8B8A8_UNORM; // D3D11 note : DXGI_FORMAT_R8G8B8_UNORM doesn't exist, so expand to 4 bytes
			HostVertexElementByteSize = 4 * sizeof(BYTE);
#else
			if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
				HostVertexElementDataType = D3DDECLTYPE_UBYTE4N;
				HostVertexElementByteSize = 4 * sizeof(BYTE);
			} else {
				HostVertexElementDataType = D3DDECLTYPE_FLOAT3;
				HostVertexElementByteSize = 3 * sizeof(FLOAT);
			}
#endif
			XboxVertexElementByteSize = 3 * sizeof(xbox::byte_xt);
			break;
		case xbox::X_D3DVSDT_PBYTE4: // 0x44:
			// Test-case : Jet Set Radio Future, Panzer
#ifdef CXBX_USE_D3D11
			HostVertexElementDataType = DXGI_FORMAT_R8G8B8A8_UNORM;
			HostVertexElementByteSize = 4 * sizeof(BYTE);
#else
			if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
				HostVertexElementDataType = D3DDECLTYPE_UBYTE4N;
				HostVertexElementByteSize = 4 * sizeof(BYTE);
				// No need for patching when D3D9 supports D3DDECLTYPE_UBYTE4N
			} else {
				HostVertexElementDataType = D3DDECLTYPE_FLOAT4;
				HostVertexElementByteSize = 4 * sizeof(FLOAT);
				XboxVertexElementByteSize = 4 * sizeof(xbox::byte_xt);
			}
#endif
			break;
		case xbox::X_D3DVSDT_FLOAT2H: // 0x72:
			HostVertexElementDataType = _9_11(D3DDECLTYPE_FLOAT4, DXGI_FORMAT_R32G32B32A32_FLOAT);
			HostVertexElementByteSize = 4 * sizeof(FLOAT);
			XboxVertexElementByteSize = 3 * sizeof(FLOAT);
			break;
		case xbox::X_D3DVSDT_NONE: // 0x02:
			assert(false); // Should already be handled above
			break;
		default:
			LOG_TEST_CASE("Unknown data type for D3DVSD_REG"); // TODO : Add : 0x % 02X\n", XboxVertexElementDataType);
			return false;
		}

		assert(HostVertexElementDataType < D3DDECLTYPE_UNUSED);
		assert(HostVertexElementByteSize > 0);

		// Select new stream, if needed
		if ((pCurrentVertexShaderStreamInfo == nullptr)
		 || (pCurrentVertexShaderStreamInfo->XboxStreamIndex != slot.StreamIndex)) {
			assert(slot.StreamIndex < X_VSH_MAX_STREAMS);
			assert(pCurrentVertexDeclaration->NumberOfVertexStreams < X_VSH_MAX_STREAMS);

			pCurrentVertexShaderStreamInfo =
				&(pCurrentVertexDeclaration->VertexStreams[
					pCurrentVertexDeclaration->NumberOfVertexStreams++]);
			pCurrentVertexShaderStreamInfo->NeedPatch = FALSE;
			pCurrentVertexShaderStreamInfo->XboxStreamIndex = (WORD)slot.StreamIndex;
			pCurrentVertexShaderStreamInfo->HostVertexStride = (WORD)slot.Offset;
			pCurrentVertexShaderStreamInfo->NumberOfVertexElements = 0;
			// Dxbx note : Use Dophin(s), FieldRender, MatrixPaletteSkinning and PersistDisplay as a testcase
		}

		// save patching information
		assert(pCurrentVertexShaderStreamInfo->NumberOfVertexElements < X_VSH_MAX_ATTRIBUTES);

		CxbxVertexShaderStreamElement* pCurrentVertexShaderStreamElementInfo =
			&(pCurrentVertexShaderStreamInfo->VertexElements[
				pCurrentVertexShaderStreamInfo->NumberOfVertexElements++]);

		if (XboxVertexElementByteSize == 0) {
			XboxVertexElementByteSize = HostVertexElementByteSize;
		} else {
			pCurrentVertexShaderStreamInfo->NeedPatch |= TRUE;
		}

		pCurrentVertexShaderStreamElementInfo->XboxType = XboxVertexElementDataType;
		pCurrentVertexShaderStreamElementInfo->XboxByteSize = XboxVertexElementByteSize;
		pCurrentVertexShaderStreamElementInfo->HostDataType = HostVertexElementDataType;
		pCurrentVertexShaderStreamElementInfo->HostByteSize = HostVertexElementByteSize;

		// Convert to host vertex element
		pCurrentHostVertexElement->Stream = pCurrentVertexShaderStreamInfo->XboxStreamIndex; // Use Xbox stream index on host
		// FIXME Don't assume vertex elements are contiguous!
		pCurrentHostVertexElement->_9_11(Offset, AlignedByteOffset) = pCurrentVertexShaderStreamInfo->HostVertexStride;
		pCurrentHostVertexElement->_9_11(Type, Format) = pCurrentVertexShaderStreamElementInfo->HostDataType;
		pCurrentHostVertexElement->Method = D3DDECLMETHOD_DEFAULT;
		if (IsFixedFunction) {
			pCurrentHostVertexElement->Usage = Xb2PCRegisterType(VertexRegister, /*&*/pCurrentHostVertexElement->_9_11(UsageIndex, SemanticIndex));
		}
		else {
			// D3DDECLUSAGE_TEXCOORD can be useds for any user-defined data
			// We need this because there is no reliable way to detect the real usage
			// Xbox has no concept of 'usage types', it only requires a list of attribute register numbers.
			// So we treat them all as 'user-defined' with an Index of the Vertex Register Index
			// this prevents information loss in shaders due to non-matching dcl types!
			pCurrentHostVertexElement->Usage = D3DDECLUSAGE_TEXCOORD;
			pCurrentHostVertexElement->_9_11(UsageIndex, SemanticIndex) = (BYTE)VertexRegister;
		}

		pCurrentVertexShaderStreamInfo->HostVertexStride += HostVertexElementByteSize;

		return true;
	}

public:
	D3DVERTEXELEMENT* Convert(xbox::X_VERTEXATTRIBUTEFORMAT* pXboxDeclaration, bool bIsFixedFunction, CxbxVertexDeclaration* pCxbxVertexDeclaration)
	{
		// Get a preprocessed copy of the original Xbox Vertex Declaration
		pCurrentVertexDeclaration = pCxbxVertexDeclaration;

		IsFixedFunction = bIsFixedFunction;

		RegVIsPresentInDeclaration.fill(false);

		// Mapping between Xbox register and the resulting host vertex element
		D3DVERTEXELEMENT* HostVertexElementPerRegister[X_VSH_MAX_ATTRIBUTES] = { 0 };

		// For Direct3D9, we need to reserve the maximum number of elements, plus one token for D3DDECL_END :
		unsigned HostDeclarationSize = (X_VSH_MAX_ATTRIBUTES + 1) * sizeof(D3DVERTEXELEMENT);

		D3DVERTEXELEMENT* HostVertexElements = (D3DVERTEXELEMENT*)calloc(1, HostDeclarationSize);
		pCurrentHostVertexElement = HostVertexElements;

		std::array<byte, X_VSH_MAX_ATTRIBUTES> orderedRegisterIndices;
		for (byte i = 0; i < orderedRegisterIndices.size(); i++)
			orderedRegisterIndices[i] = i;

		// Make sure we convert registers in order of offset, per stream
		// TODO fix elements with identical positions?
		std::sort(orderedRegisterIndices.begin(), orderedRegisterIndices.end(),
			[pXboxDeclaration](const auto& x, const auto& y)
			{
				auto regX = pXboxDeclaration->Slots[x];
				auto regY = pXboxDeclaration->Slots[y];
				return std::tie(regX.StreamIndex, regX.Offset)
					 < std::tie(regY.StreamIndex, regY.Offset);
			});

		EmuLog(LOG_LEVEL::DEBUG, "Parsing vertex declaration");
		for (size_t i = 0; i < orderedRegisterIndices.size(); i++) {
			auto regIndex = orderedRegisterIndices[i];
			auto &slot = pXboxDeclaration->Slots[regIndex];
			if (slot.Format > xbox::X_D3DVSDT_NONE) {
				// Set Direct3D9 vertex element (declaration) members :
				if (VshConvertToken_STREAMDATA_REG(regIndex, slot)) {
					// Add this register to the list of declared registers
					RegVIsPresentInDeclaration[regIndex] = true;
					// Remember a pointer to this register
					HostVertexElementPerRegister[regIndex] = pCurrentHostVertexElement;
					pCurrentHostVertexElement++;

					EmuLog(LOG_LEVEL::DEBUG, "\tXbox Stream %d, Offset %d, Format %d, Slot %d",
						slot.StreamIndex, slot.Offset, slot.Format, regIndex);
					EmuLog(LOG_LEVEL::DEBUG, "\tHost Stream %d, Offset %d, Format %d, Usage %d-%d",
						pCurrentHostVertexElement->Stream,
						pCurrentHostVertexElement->_9_11(Offset, AlignedByteOffset),
						pCurrentHostVertexElement->Type,
						pCurrentHostVertexElement->Usage,
						pCurrentHostVertexElement->_9_11(UsageIndex, SemanticIndex));
				}
			}
		}

		*pCurrentHostVertexElement = D3DDECL_END();

		// Post-process host vertex elements that have a D3DDECLMETHOD_CROSSUV method :
		for (int AttributeIndex = 0; AttributeIndex < X_VSH_MAX_ATTRIBUTES; AttributeIndex++) {
			auto pHostElement = HostVertexElementPerRegister[AttributeIndex];
			if (pHostElement == nullptr) continue;
			if (pHostElement->Method == D3DDECLMETHOD_CROSSUV) {
				int TessellationSource = pXboxDeclaration->Slots[AttributeIndex].TessellationSource;
				auto pSourceElement = HostVertexElementPerRegister[TessellationSource];
				// Copy over the Stream, Offset and Type of the host vertex element that serves as 'TessellationSource' :
				pHostElement->Stream = pSourceElement->Stream;
				pHostElement->_9_11(Offset, AlignedByteOffset) = pSourceElement->_9_11(Offset, AlignedByteOffset);
				pHostElement->Type = pSourceElement->Type;
				// Note, the input type for D3DDECLMETHOD_CROSSUV can be D3DDECLTYPE_FLOAT[43], D3DDECLTYPE_D3DCOLOR, D3DDECLTYPE_UBYTE4, or D3DDECLTYPE_SHORT4
				// (the output type implied by D3DDECLMETHOD_CROSSUV is D3DDECLTYPE_FLOAT3).
				// TODO : Should we assert this?
			}
		}

		// Ensure valid ordering of the vertex declaration (http://doc.51windows.net/Directx9_SDK/graphics/programmingguide/gettingstarted/vertexdeclaration/vertexdeclaration.htm)
		// In particular "All vertex elements for a stream must be consecutive and sorted by offset"
		// Test case: King Kong (due to register redefinition)
		// Note : Xbox slots might use non-ordered stream indices, so we can't rely on the output ordering of our converted elements!
		std::sort(/*First=*/HostVertexElements, /*Last=*/pCurrentHostVertexElement, /*Pred=*/[] (const auto& x, const auto& y)
			{ return std::tie(x.Stream, x.Method, x._9_11(Offset, AlignedByteOffset)) < std::tie(y.Stream, y.Method, y._9_11(Offset, AlignedByteOffset)); });

		// Record which registers are in the vertex declaration
		for (size_t i = 0; i < RegVIsPresentInDeclaration.size(); i++) {
			pCxbxVertexDeclaration->vRegisterInDeclaration[i] = RegVIsPresentInDeclaration[i];
		}

		return HostVertexElements;
	}
};

D3DVERTEXELEMENT *EmuRecompileVshDeclaration
(
	xbox::X_VERTEXATTRIBUTEFORMAT* pXboxDeclaration,
    bool                  bIsFixedFunction,
    CxbxVertexDeclaration *pCxbxVertexDeclaration
)
{
	XboxVertexDeclarationConverter Converter;

	D3DVERTEXELEMENT* pHostVertexElements = Converter.Convert(pXboxDeclaration, bIsFixedFunction, pCxbxVertexDeclaration);

    return pHostVertexElements;
}

static bool FreeCxbxVertexDeclaration(CxbxVertexDeclaration *pCxbxVertexDeclaration)
{
	LOG_INIT; // Allows use of DEBUG_D3DRESULT

	if (pCxbxVertexDeclaration) {
		if (pCxbxVertexDeclaration->pHostVertexDeclaration) {
			HRESULT hRet = pCxbxVertexDeclaration->pHostVertexDeclaration->Release();
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DeleteVertexShader(pHostVertexDeclaration)");
		}
		free(pCxbxVertexDeclaration);
		return true;
	}

	return false;
}

xbox::dword_xt* GetCxbxVertexShaderSlotPtr(const DWORD SlotIndexAddress)
{
	if (SlotIndexAddress < X_VSH_MAX_INSTRUCTION_COUNT) {
		return &g_Xbox_VertexShader_FunctionSlots[SlotIndexAddress * X_VSH_INSTRUCTION_SIZE];
	} else {
		LOG_TEST_CASE("SlotIndexAddress out of range"); // FIXME : extend with value (once supported by LOG_TEST_CASE)
		return nullptr;
	}
}

VertexDeclarationKey GetXboxVertexAttributesKey(xbox::X_VERTEXATTRIBUTEFORMAT* pXboxVertexAttributeFormat)
{
	auto attributeHash = ComputeHash((void*)pXboxVertexAttributeFormat, sizeof(xbox::X_VERTEXATTRIBUTEFORMAT));
	// For now, we use different declarations depending on if the fixed function pipeline
	// is in use, even if the attributes are the same
	return g_Xbox_VertexShaderMode == VertexShaderMode::FixedFunction
		? attributeHash
		: attributeHash ^ 1;
}

std::unordered_map<VertexDeclarationKey, CxbxVertexDeclaration*> g_CxbxVertexDeclarations;

void RegisterCxbxVertexDeclaration(VertexDeclarationKey CacheKey, CxbxVertexDeclaration* pCxbxVertexDeclaration)
{
	auto it = g_CxbxVertexDeclarations.find(CacheKey);
	if (it != g_CxbxVertexDeclarations.end() && it->second != nullptr) {
		LOG_TEST_CASE("Overwriting existing Vertex Declaration");
		FreeCxbxVertexDeclaration(it->second); // Avoid memory leak
	}

	g_CxbxVertexDeclarations[CacheKey] = pCxbxVertexDeclaration;
}

CxbxVertexDeclaration* FetchCachedCxbxVertexDeclaration(VertexDeclarationKey CacheKey)
{
	auto it = g_CxbxVertexDeclarations.find(CacheKey);
	if (it != g_CxbxVertexDeclarations.end()) {
		return it->second;
	}

	return nullptr;
}

IDirect3DVertexDeclaration* CxbxCreateHostVertexDeclaration(D3DVERTEXELEMENT *pDeclaration)
{
	LOG_INIT; // Allows use of DEBUG_D3DRESULT

	IDirect3DVertexDeclaration* pHostVertexDeclaration = nullptr;
	HRESULT hRet = g_pD3DDevice->CreateVertexDeclaration(pDeclaration, &pHostVertexDeclaration);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateVertexDeclaration");

	return pHostVertexDeclaration;
}

static IDirect3DVertexShader* g_pHostPassthroughVertexShader = nullptr;
static IDirect3DVertexShader* g_pHostFixedFunctionVertexShader = nullptr;
static IDirect3DVertexShader* g_pHostActiveVertexShader = nullptr; // Used for state tracking

extern IDirect3DVertexShader* CxbxCreateVertexShader(ID3DBlob* pCompiledShader, char *shader_category); // Implemented in VertexShaderSource.cpp

void CxbxUpdateHostVertexShader()
{
	extern bool g_bUsePassthroughHLSL; // TMP glue

	LOG_INIT; // Allows use of DEBUG_D3DRESULT

	char *shader_category;
	IDirect3DVertexShader *pHostVertexShader;

	if (g_Xbox_VertexShaderMode == VertexShaderMode::FixedFunction) {
		shader_category = "fixed function";
		if (g_pHostPassthroughVertexShader == nullptr) {
			ID3DBlob* pBlob = nullptr;
			EmuCompileFixedFunction(&pBlob);
			if (pBlob) {
				g_pHostPassthroughVertexShader = CxbxCreateVertexShader(pBlob, shader_category);
				pBlob->Release();
			}
		}

		pHostVertexShader = g_pHostPassthroughVertexShader;
	}
	else if (g_Xbox_VertexShaderMode == VertexShaderMode::Passthrough && g_bUsePassthroughHLSL) {
		shader_category = "passthrough";
		if (g_pHostFixedFunctionVertexShader == nullptr) {
			ID3DBlob* pBlob = nullptr;
			EmuCompileXboxPassthrough(&pBlob);
			if (pBlob) {
				g_pHostFixedFunctionVertexShader = CxbxCreateVertexShader(pBlob, shader_category);
				pBlob->Release();
			}
		}

		pHostVertexShader = g_pHostFixedFunctionVertexShader;
	}
	else {
		shader_category = "regular";
		auto pTokens = GetCxbxVertexShaderSlotPtr(g_Xbox_VertexShader_FunctionSlots_StartAddress);
		assert(pTokens);
		// Create a vertex shader from the tokens
		DWORD shaderSize;
		auto VertexShaderKey = g_VertexShaderSource.CreateShader(pTokens, &shaderSize);
		pHostVertexShader = g_VertexShaderSource.GetShader(VertexShaderKey);
	}

	// Avoid setting already-active vertex shader again :
	if (g_pHostActiveVertexShader == pHostVertexShader)
		return;

	g_pHostActiveVertexShader = pHostVertexShader;
	HRESULT hRet;
#ifdef CXBX_USE_D3D11
	hRet = g_pD3DDevice->VSSetShader(
		pHostVertexShader,
		nullptr, // ppClassInstances
		0 // NumClassInstances
	);
#else
	hRet = g_pD3DDevice->SetVertexShader(pHostVertexShader);
#endif
	DEBUG_D3DRESULT(hRet, "SetVertexShader");
	if (FAILED(hRet)) CxbxrAbort("Failed to set %s vertex shader", shader_category);
}

void CxbxSetVertexShaderSlots(DWORD* pTokens, DWORD Address, DWORD NrInstructions)
{
	int upToSlot = Address + NrInstructions;
	if (upToSlot > X_VSH_MAX_INSTRUCTION_COUNT) {
		LOG_TEST_CASE("Shader does not fit in vertex shader slots");
		return;
	}

	auto CxbxVertexShaderSlotPtr = GetCxbxVertexShaderSlotPtr(Address);
	if (CxbxVertexShaderSlotPtr == nullptr) {
		return;
	}

	memcpy(CxbxVertexShaderSlotPtr, pTokens, NrInstructions * X_VSH_INSTRUCTION_SIZE_BYTES);

	// Make sure slot parsing in EmuParseVshFunction (VshConvertToIntermediate) stops after the last slot;
	// Just setting bit 0 in 3rd DWORD suffices (see XboxVertexShaderDecoder.VshGetField.FieldMapping[FLD_FINAL]) :
	g_Xbox_VertexShader_FunctionSlots[(X_VSH_MAX_INSTRUCTION_COUNT * X_VSH_INSTRUCTION_SIZE) + 3] = 1;
}

static void CxbxSetVertexShaderPassthroughProgram()
{
	static DWORD XboxShaderBinaryPassthrough[] = {
		0, 0x0020001B, 0x0836106C, 0x2F100FF8,
		0, 0x0420061B, 0x083613FC, 0x5011F818,
		0, 0x002008FF, 0x0836106C, 0x2070F828,
		0, 0x0240081B, 0x1436186C, 0x2F20F824,
		0, 0x0060201B, 0x2436106C, 0x3070F800,
		0, 0x00200200, 0x0836106C, 0x2070F830,
		0, 0x00200E1B, 0x0836106C, 0x2070F838,
		0, 0x0020101B, 0x0836106C, 0x2070F840,
		0, 0x0020121B, 0x0836106C, 0x2070F848,
		0, 0x0020141B, 0x0836106C, 0x2070F850,
		0, 0x0020161B, 0x0836106C, 0x2070F858,
		0, 0x0020181B, 0x0836106C, 0x2070F861 // FLD_FINAL is set here!
	};

	// LOG_TEST_CASE("Setting Xbox passthrough shader");
	// Test cases : Many XDK samples & games

	// TODO : Xbox uses three variants;
	// one for FOGTABLEMODE NONE
	// one for FOGSOURCEZ
	// one for WFOG

	CxbxSetVertexShaderSlots(&XboxShaderBinaryPassthrough[0], 0, sizeof(XboxShaderBinaryPassthrough) / X_VSH_INSTRUCTION_SIZE_BYTES);

	// Passthrough programs require scale and offset to be set in constants zero and one (both minus 96)
	// (Note, these are different from GetMultiSampleOffsetAndScale / GetViewPortOffsetAndScale)
	float scale[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float offset[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
#if 0 // Based on (regular) BeginPush XDK and (multisampled) AntiAlias samples, scale and offset should just use above defaults, with both render scale factor 1, but also 2 and higher.
	scale[0] = (float)g_RenderScaleFactor;
	scale[1] = (float)g_RenderScaleFactor;
	scale[2] = 1.0f; // Passthrough should not scale Z (so don't use g_ZScale)
	scale[3] = 1.0f;

	float MultiSampleBias = 0.0f;
	if (XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_MULTISAMPLEANTIALIAS) > 0) {
		extern float GetMultiSampleOffsetDelta(); // TMP glue

		MultiSampleBias = GetMultiSampleOffsetDelta();
	}

	offset[0] = g_Xbox_ScreenSpaceOffset_x - MultiSampleBias;
	offset[1] = g_Xbox_ScreenSpaceOffset_y - MultiSampleBias;
	offset[2] = 0.0f;
	offset[3] = 0.0f;
#endif

	// Test-case : XDK Ripple sample

	// TODO : Apparently, offset and scale are swapped in some XDK versions, but which?
	CxbxImpl_SetVertexShaderConstant(0 - X_D3DSCM_CORRECTION, scale, 1);
	CxbxImpl_SetVertexShaderConstant(1 - X_D3DSCM_CORRECTION, offset, 1);
}

CxbxVertexDeclaration* CxbxGetVertexDeclaration()
{
	LOG_INIT; // Allows use of DEBUG_D3DRESULT

	xbox::X_VERTEXATTRIBUTEFORMAT *pXboxVertexAttributeFormat = GetXboxVertexAttributeFormat();

	auto XboxVertexAttributesKey = GetXboxVertexAttributesKey(pXboxVertexAttributeFormat);
	CxbxVertexDeclaration* pCxbxVertexDeclaration = FetchCachedCxbxVertexDeclaration(XboxVertexAttributesKey);
	if (pCxbxVertexDeclaration == nullptr) {
		pCxbxVertexDeclaration = (CxbxVertexDeclaration*)calloc(1, sizeof(CxbxVertexDeclaration));

		// Convert Xbox vertex attributes towards host Direct3D 9 vertex element
		D3DVERTEXELEMENT* pRecompiledVertexElements = EmuRecompileVshDeclaration(
			pXboxVertexAttributeFormat,
			g_Xbox_VertexShaderMode == VertexShaderMode::FixedFunction,
			pCxbxVertexDeclaration);

		// Create the vertex declaration
		pCxbxVertexDeclaration->pHostVertexDeclaration = CxbxCreateHostVertexDeclaration(pRecompiledVertexElements);

		free(pRecompiledVertexElements);

		// Cache resulting declarations from given inputs
		pCxbxVertexDeclaration->Key = XboxVertexAttributesKey;
		RegisterCxbxVertexDeclaration(XboxVertexAttributesKey, pCxbxVertexDeclaration);
	}

	return pCxbxVertexDeclaration;
}

void CxbxUpdateHostVertexDeclaration()
{
	CxbxVertexDeclaration* pCxbxVertexDeclaration = CxbxGetVertexDeclaration();
#ifdef CXBX_USE_D3D11
	HRESULT hRet = g_pD3DDeviceContext->IASetInputLayout(pCxbxVertexDeclaration->pHostVertexDeclaration);
#else
	HRESULT hRet = g_pD3DDevice->SetVertexDeclaration(pCxbxVertexDeclaration->pHostVertexDeclaration);
#endif

	// Titles can specify default values for registers via calls like SetVertexData4f
	// HLSL shaders need to know whether to use vertex data or default vertex shader values
	// Any register not in the vertex declaration should be set to the default value
	float vertexDefaultFlags[X_VSH_MAX_ATTRIBUTES];
	for (int i = 0; i < X_VSH_MAX_ATTRIBUTES; i++) {
		vertexDefaultFlags[i] = pCxbxVertexDeclaration->vRegisterInDeclaration[i] ? 0.0f : 1.0f;
	}
	g_pD3DDevice->SetVertexShaderConstantF(CXBX_D3DVS_CONSTREG_VREGDEFAULTS_FLAG_BASE, vertexDefaultFlags, CXBX_D3DVS_CONSTREG_VREGDEFAULTS_FLAG_SIZE);
}

void CxbxImpl_SetScreenSpaceOffset(float x, float y)
{
	// See https://microsoft.github.io/DirectX-Specs/d3d/archive/D3D11_3_FunctionalSpec.htm#3.3.1%20Pixel%20Coordinate%20System
	static float PixelOffset = 0.53125f; // 0.5 for pixel center + 1/16?

	g_Xbox_ScreenSpaceOffset_x = x + PixelOffset;
	g_Xbox_ScreenSpaceOffset_y = y + PixelOffset;
}

// Note : SetVertexShaderInputDirect needs no EMUPATCH CxbxImpl_..., since it just calls SetVertexShaderInput

void CxbxImpl_SetVertexShaderInput(DWORD Handle, UINT StreamCount, xbox::X_STREAMINPUT* pStreamInputs)
{
	using namespace xbox;

	// If Handle is NULL, all VertexShader input state is cleared.
	// Otherwise, Handle is the address of an Xbox VertexShader struct, or-ed with 1 (X_D3DFVF_RESERVED0)
	// (Thus, a FVF handle is an invalid argument.)

	if (Handle == NULL)
	{
		// Xbox doesn't remember a null-handle - this may be an XDK bug!
		// (Although, if that's skipped intentionally, we'd need to be very carefull about that!)
		// StreamCount and pStreamInputs arguments are ignored
		g_Xbox_SetVertexShaderInput_Count = 0;
	}
	else
	{
		assert(VshHandleIsVertexShader(Handle));
		assert(StreamCount > 0);
		assert(StreamCount <= X_VSH_MAX_STREAMS);
		assert(pStreamInputs != xbox::zeroptr);

		X_D3DVertexShader* pXboxVertexShader = VshHandleToXboxVertexShader(Handle);
		assert(pXboxVertexShader);

		// Xbox DOES store the Handle, but since it merely returns this through (unpatched) D3DDevice_GetVertexShaderInput, we don't have to.

		g_Xbox_SetVertexShaderInput_Count = StreamCount; // This > 0 indicates g_Xbox_SetVertexShaderInput_Data has to be used
		memcpy(g_Xbox_SetVertexShaderInput_Data, pStreamInputs, StreamCount * sizeof(xbox::X_STREAMINPUT)); // Make a copy of the supplied StreamInputs array

		g_Xbox_SetVertexShaderInput_Attributes = *CxbxGetVertexShaderAttributes(pXboxVertexShader); // Copy this vertex shaders's attribute slots
	}
}

// Note : SelectVertexShaderDirect needs no EMUPATCH CxbxImpl_..., since it just calls SelectVertexShader

void CxbxImpl_SelectVertexShader(DWORD Handle, DWORD Address)
{
	LOG_INIT; // Allows use of DEBUG_D3DRESULT

	// Address always indicates a previously loaded vertex shader slot (from where the program is used).
	// Handle can be null if the current Xbox VertexShader is assigned
	// Handle can be an address of an Xbox VertexShader struct, or-ed with 1 (X_D3DFVF_RESERVED0)
	// If Handle is assigned, it becomes the new current Xbox VertexShader,
	// which resets a bit of state (nv2a execution mode, viewport, ?)
	// Either way, the given address slot is selected as the start of the current vertex shader program
	g_Xbox_VertexShader_FunctionSlots_StartAddress = Address;

	g_Xbox_VertexShaderMode = VertexShaderMode::ShaderProgram;

	if (Handle) {
		if (!VshHandleIsVertexShader(Handle))
			LOG_TEST_CASE("Non-zero handle must be a VertexShader!");

#ifdef CXBX_USE_GLOBAL_VERTEXSHADER_POINTER
		g_Xbox_VertexShader_Ptr = VshHandleToXboxVertexShader(Handle);
#endif
		g_Xbox_VertexShader_Handle = Handle;
	}
}

void CxbxImpl_LoadVertexShaderProgram(CONST DWORD* pFunction, DWORD Address)
{
	// pFunction is a X_VSH_SHADER_HEADER pointer
	// D3DDevice_LoadVertexShaderProgram splits the given function buffer into batch-wise pushes to the NV2A
	// However, we can suffice by copying the program into our slots (and make sure these slots get converted into a vertex shader)

	// Copy shader instructions to shader slots
	auto shaderHeader = *((xbox::X_VSH_SHADER_HEADER*) pFunction);
	if (shaderHeader.Version != VERSION_XVS)
		LOG_TEST_CASE("Non-regular (state or read/write) shader detected!");

	auto tokens = (DWORD*)&pFunction[1];
	CxbxSetVertexShaderSlots(tokens, Address, shaderHeader.NumInst);
}

void CxbxImpl_LoadVertexShader(DWORD Handle, DWORD Address)
{
	// Handle is always address of an X_D3DVertexShader struct, thus always or-ed with 1 (X_D3DFVF_RESERVED0)
	// Address is the slot (offset) from which the program must be written onwards (as whole DWORDS)
	// D3DDevice_LoadVertexShader pushes the program contained in the Xbox VertexShader struct to the NV2A

	xbox::X_D3DVertexShader* pXboxVertexShader = VshHandleToXboxVertexShader(Handle);

	DWORD NrTokens;
	DWORD* pNV2ATokens = CxbxGetVertexShaderTokens(pXboxVertexShader, &NrTokens);

#if 1 // TODO : Remove dirty hack (?once CreateVertexShader trampolines to Xbox code that sets ProgramAndConstantsDwords correctly?) :
	if (NrTokens == 0)
		NrTokens = 10000;
#endif

	static unsigned ConstantAddress = 0;
	DWORD* pEnd = pNV2ATokens + NrTokens;
	while (pNV2ATokens < pEnd) {
		DWORD dwMethod, dwSubChannel, nrDWORDS;
		D3DPUSH_DECODE(*pNV2ATokens++, dwMethod, dwSubChannel, nrDWORDS);
		if (nrDWORDS == 0) { LOG_TEST_CASE("Zero-length NV2A method detected!"); break; }
		switch (dwMethod) {
		case NV2A_VP_UPLOAD_INST(0): { // = 0x00000B00
			if ((nrDWORDS & 3) != 0) LOG_TEST_CASE("NV2A_VP_UPLOAD_INST arguments should be a multiple of 4!");
			unsigned nrSlots = nrDWORDS / X_VSH_INSTRUCTION_SIZE;
			CxbxSetVertexShaderSlots(pNV2ATokens, Address, nrSlots);
			Address += nrSlots;
			break;
		}
		case NV2A_VP_UPLOAD_CONST_ID: // = 0x00001EA4
			if (nrDWORDS != 1) LOG_TEST_CASE("NV2A_VP_UPLOAD_CONST_ID should have one argument!");
			ConstantAddress = *pNV2ATokens;
			break;
		case NV2A_VP_UPLOAD_CONST(0): { // = 0x00000B80
			if ((nrDWORDS & 3) != 0) LOG_TEST_CASE("NV2A_VP_UPLOAD_CONST arguments should be a multiple of 4!");
			unsigned nrConstants = nrDWORDS / X_VSH_INSTRUCTION_SIZE;
			// TODO : FIXME : Implement and call SetVertexShaderConstants(pNV2ATokens, ConstantAddress, nrConstants);
			ConstantAddress += nrConstants;
			break;
		}
		default:
			// TODO : Remove this break-out hack once NrTokens is reliable and instead have: DEFAULT_UNREACHABLE;
			LOG_TEST_CASE("Stopping at unexpected NV2A method");
			pEnd = pNV2ATokens;
			break;
		}

		pNV2ATokens += nrDWORDS;
	}
}

// Set default values for attributes missing from vertex declaration
void SetFixedFunctionDefaultVertexAttributes(DWORD vshFlags) {
	// Test case: Mechassault (skybox)
	// Test case: KOTOR (overlay)
	auto decl = CxbxGetVertexDeclaration();
	for (int i = 0; i < xbox::X_D3DVSDE_TEXCOORD3; i++) {
		if (decl->vRegisterInDeclaration[i]) {
			continue; // only reset missing attributes
		}

		const float white[4] = { 1, 1, 1, 1 };
		const float black[4] = { 0, 0, 0, 0 };
		const float unset[4] = { 0, 0, 0, 1 };
		const float* value = unset;

		// Account for flags that override this reset behaviour
		if (i == xbox::X_D3DVSDE_DIFFUSE && !(vshFlags & X_VERTEXSHADER_FLAG_HASDIFFUSE) ||
			i == xbox::X_D3DVSDE_BACKDIFFUSE && !(vshFlags & X_VERTEXSHADER_FLAG_HASBACKDIFFUSE)) {
			value = white;
		}
		else if (i == xbox::X_D3DVSDE_SPECULAR && !(vshFlags & X_VERTEXSHADER_FLAG_HASSPECULAR) ||
			i == xbox::X_D3DVSDE_BACKSPECULAR && !(vshFlags & X_VERTEXSHADER_FLAG_HASBACKSPECULAR)) {
			value = black;
		}

		// Note : We avoid calling CxbxImpl_SetVertexData4f here, as that would
		// start populating g_InlineVertexBuffer_Table, which is not our intent here.
		CxbxSetVertexAttribute(i, value[0], value[1], value[2], value[3]);
	}
}

void CxbxImpl_SetVertexShader(DWORD Handle)
{
	LOG_INIT; // Allows use of DEBUG_D3DRESULT

	// Checks if the Handle has bit 0 set - if not, it's a FVF
	// which is converted to a global Xbox Vertex Shader struct
	// Otherwise bit 0 is cleared and the resulting address is
	// validated to be a valid Xbox Vertex Shader
	// D3D state fields are updated.
	// If the shader contains a program, the handle is passed to
	// D3DDevice_LoadVertexShader and D3DDevice_SelectVertexShader.
	// Otherwise the shader is send using push buffer commands.

	HRESULT hRet = D3D_OK;

	xbox::X_D3DVertexShader* pXboxVertexShader = CxbxGetXboxVertexShaderForHandle(Handle);

	if ((pXboxVertexShader->Flags & g_X_VERTEXSHADER_FLAG_VALID_MASK) != pXboxVertexShader->Flags) {
		LOG_TEST_CASE("Unknown vertex shader flag");
	}

	if (pXboxVertexShader->Flags & g_X_VERTEXSHADER_FLAG_PROGRAM) { // Global variable set from CxbxVertexShaderSetFlags
#if 0 // Since the D3DDevice_SetVertexShader patch already called it's trampoline, these calls have already been executed :
		CxbxImpl_LoadVertexShader(Handle, 0);
		CxbxImpl_SelectVertexShader(Handle, 0);
#else // So let's check if that indeed happened :
		bool bHackCallSelectAgain = false;
		if (g_Xbox_VertexShader_Handle != Handle) {
			LOG_TEST_CASE("g_Xbox_VertexShader_Handle != Handle");
			bHackCallSelectAgain = true;
		}
		if (g_Xbox_VertexShader_FunctionSlots_StartAddress != 0) {
			LOG_TEST_CASE("g_Xbox_VertexShader_FunctionSlots_StartAddress != 0");
			bHackCallSelectAgain = true;
		}
		if (g_Xbox_VertexShaderMode != VertexShaderMode::ShaderProgram) {
			LOG_TEST_CASE("Not in shader program mode after SetVertexShader trampoline");
			bHackCallSelectAgain = true;
		}

		if (bHackCallSelectAgain) {
			// If any of the above test-cases was hit, perhaps our patch on
			// _SelectVertexShader isn't applied;
			// 'solve' that by calling it here instead.
			CxbxImpl_SelectVertexShader(Handle, 0);
		}
#endif
	} else {
		// A shader without a program won't call LoadVertexShader nor SelectVertexShader
		// 
#ifdef CXBX_USE_GLOBAL_VERTEXSHADER_POINTER
		g_Xbox_VertexShader_Ptr = pXboxVertexShader;
#endif
		g_Xbox_VertexShader_Handle = Handle;
		g_Xbox_VertexShader_FunctionSlots_StartAddress = 0;

		SetFixedFunctionDefaultVertexAttributes(pXboxVertexShader->Flags);

		// Switch to passthrough program, if so required
		if (pXboxVertexShader->Flags & X_VERTEXSHADER_FLAG_PASSTHROUGH) {
			CxbxSetVertexShaderPassthroughProgram();
			g_Xbox_VertexShaderMode = VertexShaderMode::Passthrough;
		} else {
			// Test-case : Many XDK samples, Crazy taxi 3
			//LOG_TEST_CASE("Other or no vertex shader flags");
			g_Xbox_VertexShaderMode = VertexShaderMode::FixedFunction;
		}
	}
}

void CxbxImpl_DeleteVertexShader(DWORD Handle)
{
	LOG_INIT; // Allows use of DEBUG_D3DRESULT

	// Handle is always address of an Xbox VertexShader struct, or-ed with 1 (X_D3DFVF_RESERVED0)
	// It's reference count is lowered. If it reaches zero (0), the struct is freed.

	xbox::X_D3DVertexShader* pXboxVertexShader = VshHandleToXboxVertexShader(Handle);
	if (pXboxVertexShader == nullptr) {
		return;
	}

	if (pXboxVertexShader->RefCount > 1) {
		return;
	}

#if 0 // TODO : Decide and implement what parts to free
	RegisterCxbxVertexDeclaration(pCxbxVertexDeclaration->Key, nullptr); // Remove from cache (which will free present pCxbxVertexDeclaration)

	// Release the host vertex shader
	g_VertexShaderSource.ReleaseShader(pCxbxVertexShader->Key);
#endif
}

// TODO : Remove SetVertexShaderConstant implementation and the patch once
// CxbxUpdateHostVertexShaderConstants is reliable (ie. : when we're able to flush the NV2A push buffer)
void CxbxImpl_SetVertexShaderConstant(INT Register, PVOID pConstantData, DWORD ConstantCount)
{
	LOG_INIT; // Allows use of DEBUG_D3DRESULT

	// Xbox vertex shader constants range from -96 to 95
	// The host does not support negative, so we adjust to 0..191
	Register += X_D3DSCM_CORRECTION;

	if (Register < 0) LOG_TEST_CASE("Register < 0");
	if (Register + ConstantCount > X_D3DVS_CONSTREG_COUNT) LOG_TEST_CASE("Register + ConstantCount > X_D3DVS_CONSTREG_COUNT");

	// Write Vertex Shader constants in nv2a
	extern float* HLE_get_NV2A_vertex_constant_float4_ptr(unsigned const_index); // TMP glue
	float* constant_floats = HLE_get_NV2A_vertex_constant_float4_ptr(Register);
	memcpy(constant_floats, pConstantData, ConstantCount * sizeof(float) * 4);

	// Mark the constant as dirty, so that CxbxUpdateHostVertexShaderConstants will pick it up
	extern NV2ADevice* g_NV2A; // TMP glue
	auto nv2a = g_NV2A->GetDeviceState();
	for (DWORD i = 0; i < ConstantCount; i++) {
		nv2a->pgraph.vsh_constants_dirty[Register + i] = true;
	}
}

// parse xbox vertex shader function into an intermediate format
extern void EmuParseVshFunction
(
	// Pointer to raw Xbox vertex shader instruction slots
	DWORD* pXboxFunction,
	IntermediateVertexShader* pShader
)
{
	// Decode the vertex shader program tokens into an intermediate representation
	auto pCurToken = (uint32_t*)pXboxFunction;

	// Decode until we hit a token marked final
	// Note : CxbxSetVertexShaderSlots makes sure this always stops
	// after X_VSH_MAX_INSTRUCTION_COUNT, by setting FLD_FINAL in there.
	while (XboxVertexShaderDecoder::VshConvertToIntermediate(pCurToken, pShader)) {
		pCurToken += X_VSH_INSTRUCTION_SIZE;
	}
}
