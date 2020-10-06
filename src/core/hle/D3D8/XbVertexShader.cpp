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

#include "core\kernel\init\CxbxKrnl.h"
#include "core\kernel\support\Emu.h"
#include "core\hle\D3D8\Direct3D9\Direct3D9.h" // For g_Xbox_VertexShader_Handle
#include "core\hle\D3D8\Direct3D9\VertexShaderSource.h" // For g_VertexShaderSource
#include "core\hle\D3D8\XbVertexShader.h"
#include "core\hle\D3D8\XbD3D8Logging.h" // For DEBUG_D3DRESULT
#include "common\Logging.h" // For LOG_INIT

#include "XbD3D8Types.h" // For X_D3DVSDE_*
#include <sstream>
#include <unordered_map>
#include <array>
#include <bitset>

#define DbgVshPrintf \
	LOG_CHECK_ENABLED(LOG_LEVEL::DEBUG) \
		if(g_bPrintfOn) printf

// ****************************************************************************
// * Vertex shader function recompiler
// ****************************************************************************

class XboxVertexShaderDecoder
{
private:
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

	void VshAddIntermediateInstruction(
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
			CxbxKrnlCleanup("Shader exceeds conversion buffer!");
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

public:
	bool VshConvertToIntermediate(uint32_t* pShaderToken, IntermediateVertexShader* pShader)
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

// ****************************************************************************
// * Vertex shader declaration recompiler
// ****************************************************************************

extern D3DCAPS g_D3DCaps;

class XboxVertexDeclarationConverter
{
protected:
	// Internal variables
	CxbxVertexDeclaration* pVertexDeclarationToSet;
	CxbxVertexShaderStreamInfo* pCurrentVertexShaderStreamInfo = nullptr;
	bool IsFixedFunction;
	D3DVERTEXELEMENT* pRecompiled;
	std::array<bool, 16> RegVIsPresentInDeclaration;

public:
	// Output
	DWORD XboxDeclarationCount;

private:
	#define D3DDECLUSAGE_UNSUPPORTED ((D3DDECLUSAGE)-1)

	static D3DDECLUSAGE Xb2PCRegisterType
	(
		DWORD VertexRegister,
		BYTE& PCUsageIndex
	)
	{
		D3DDECLUSAGE PCRegisterType;
		PCUsageIndex = 0;

		switch (VertexRegister)
		{
		case (DWORD)xbox::X_D3DVSDE_VERTEX: // -1
			PCRegisterType = D3DDECLUSAGE_UNSUPPORTED;
			break;
		case xbox::X_D3DVSDE_POSITION: // 0
			PCRegisterType = D3DDECLUSAGE_POSITION;
			break;
		case xbox::X_D3DVSDE_BLENDWEIGHT: // 1
			PCRegisterType = D3DDECLUSAGE_BLENDWEIGHT;
			break;
		case xbox::X_D3DVSDE_NORMAL: // 2
			PCRegisterType = D3DDECLUSAGE_NORMAL;
			break;
		case xbox::X_D3DVSDE_DIFFUSE: // 3
			PCRegisterType = D3DDECLUSAGE_COLOR; PCUsageIndex = 0;
			break;
		case xbox::X_D3DVSDE_SPECULAR: // 4
			PCRegisterType = D3DDECLUSAGE_COLOR; PCUsageIndex = 1;
			break;
		case xbox::X_D3DVSDE_FOG: // 5
			PCRegisterType = D3DDECLUSAGE_FOG;
			break;
		case xbox::X_D3DVSDE_POINTSIZE: // 6
			PCRegisterType = D3DDECLUSAGE_PSIZE;
			break;
		case xbox::X_D3DVSDE_BACKDIFFUSE: // 7
			PCRegisterType = D3DDECLUSAGE_COLOR; PCUsageIndex = 2;
			break;
		case xbox::X_D3DVSDE_BACKSPECULAR: // 8
			PCRegisterType = D3DDECLUSAGE_COLOR; PCUsageIndex = 3;
			break;
		case xbox::X_D3DVSDE_TEXCOORD0: // 9
			PCRegisterType = D3DDECLUSAGE_TEXCOORD; PCUsageIndex = 0;
			break;
		case xbox::X_D3DVSDE_TEXCOORD1: // 10
			PCRegisterType = D3DDECLUSAGE_TEXCOORD; PCUsageIndex = 1;
			break;
		case xbox::X_D3DVSDE_TEXCOORD2: // 11
			PCRegisterType = D3DDECLUSAGE_TEXCOORD; PCUsageIndex = 2;
			break;
		case xbox::X_D3DVSDE_TEXCOORD3: // 12
			PCRegisterType = D3DDECLUSAGE_TEXCOORD; PCUsageIndex = 3;
			break;
		default:
			PCRegisterType = D3DDECLUSAGE_UNSUPPORTED;
			break;
		}

		return PCRegisterType;
	}

	static char* XboxVertexRegisterAsString(DWORD VertexRegister)
	{
		switch (VertexRegister)
		{
		case (DWORD)xbox::X_D3DVSDE_VERTEX: // -1
			return "D3DVSDE_VERTEX /* xbox ext. */";
		case xbox::X_D3DVSDE_POSITION: // 0
			return "D3DVSDE_POSITION";
		case xbox::X_D3DVSDE_BLENDWEIGHT: // 1
			return "D3DVSDE_BLENDWEIGHT";
		case xbox::X_D3DVSDE_NORMAL: // 2
			return "D3DVSDE_NORMAL";
		case xbox::X_D3DVSDE_DIFFUSE: // 3
			return "D3DVSDE_DIFFUSE";
		case xbox::X_D3DVSDE_SPECULAR: // 4
			return "D3DVSDE_SPECULAR";
		case xbox::X_D3DVSDE_FOG: // 5
			return "D3DVSDE_FOG";
		case xbox::X_D3DVSDE_POINTSIZE: // 6
			return "D3DVDSE_POINTSIZE";
		case xbox::X_D3DVSDE_BACKDIFFUSE: // 7
			return "D3DVSDE_BACKDIFFUSE /* xbox ext. */";
		case xbox::X_D3DVSDE_BACKSPECULAR: // 8
			return "D3DVSDE_BACKSPECULAR /* xbox ext. */";
		case xbox::X_D3DVSDE_TEXCOORD0: // 9
			return "D3DVSDE_TEXCOORD0";
		case xbox::X_D3DVSDE_TEXCOORD1: // 10
			return "D3DVSDE_TEXCOORD1";
		case xbox::X_D3DVSDE_TEXCOORD2: // 11
			return "D3DVSDE_TEXCOORD2";
		case xbox::X_D3DVSDE_TEXCOORD3: // 12
			return "D3DVSDE_TEXCOORD3";
		case 13:
			return "13 /* unknown register */";
		case 14:
			return "14 /* unknown register */";
		case 15:
			return "15 /* unknown register */";
		default:
			return "16 /* or higher, unknown register */";
		}
	}

	// VERTEX SHADER

	static DWORD VshGetDeclarationCount(DWORD *pXboxDeclaration)
	{
		DWORD Pos = 0;
		while (pXboxDeclaration[Pos] != X_D3DVSD_END())
		{
			Pos++;
		}

		return Pos + 1;
	}

	static inline DWORD VshGetTokenType(DWORD XboxToken)
	{
		return (XboxToken & X_D3DVSD_TOKENTYPEMASK) >> X_D3DVSD_TOKENTYPESHIFT;
	}

	static inline WORD VshGetVertexStream(DWORD XboxToken)
	{
		return (XboxToken & X_D3DVSD_STREAMNUMBERMASK) >> X_D3DVSD_STREAMNUMBERSHIFT;
	}

	static inline DWORD VshGetVertexRegister(DWORD XboxToken)
	{
		DWORD regNum = (XboxToken & X_D3DVSD_VERTEXREGMASK) >> X_D3DVSD_VERTEXREGSHIFT;
		return regNum;
	}

	static inline DWORD VshGetVertexRegisterIn(DWORD XboxToken)
	{
		DWORD regNum = (XboxToken & X_D3DVSD_VERTEXREGINMASK) >> X_D3DVSD_VERTEXREGINSHIFT;
		return regNum;
	}

	void VshDumpXboxDeclaration(DWORD* pXboxDeclaration)
	{
		DbgVshPrintf("DWORD dwVSHDecl[] =\n{\n");
		unsigned iNumberOfVertexStreams = 0;
		bool bStreamNeedsPatching = false;
		auto pXboxToken = pXboxDeclaration;
		while (*pXboxToken != X_D3DVSD_END()) // X_D3DVSD_TOKEN_END 
		{
			DWORD Step = 1;

			switch (VshGetTokenType(*pXboxToken)) {
			case xbox::X_D3DVSD_TOKEN_NOP: {
				DbgVshPrintf("\tD3DVSD_NOP(),\n");
				break;
			}
			case xbox::X_D3DVSD_TOKEN_STREAM: {
				if (*pXboxToken & X_D3DVSD_STREAMTESSMASK) {
					DbgVshPrintf("\tD3DVSD_STREAM_TESS(),\n");
				} else {
					if (iNumberOfVertexStreams > 0) {
						DbgVshPrintf("\t// NeedPatching: %d\n", bStreamNeedsPatching);
					}
					DWORD StreamNumber = VshGetVertexStream(*pXboxToken);
					DbgVshPrintf("\tD3DVSD_STREAM(%u),\n", StreamNumber);
					iNumberOfVertexStreams++;
					bStreamNeedsPatching = false;
				}
				break;
			}
			case xbox::X_D3DVSD_TOKEN_STREAMDATA: {
				if (*pXboxToken & X_D3DVSD_MASK_SKIP) {
 					WORD SkipCount = (*pXboxToken & X_D3DVSD_SKIPCOUNTMASK) >> X_D3DVSD_SKIPCOUNTSHIFT;
					if (*pXboxToken & X_D3DVSD_MASK_SKIPBYTES) {
						DbgVshPrintf("\tD3DVSD_SKIPBYTES(%d), /* xbox ext. */\n", SkipCount);
					} else {
						DbgVshPrintf("\tD3DVSD_SKIP(%d),\n", SkipCount);
					}
				} else {
					DWORD VertexRegister = VshGetVertexRegister(*pXboxToken);
					if (IsFixedFunction) {
						DbgVshPrintf("\t\tD3DVSD_REG(%s, ", XboxVertexRegisterAsString(VertexRegister));
					} else {
						DbgVshPrintf("\t\tD3DVSD_REG(%d, ", (BYTE)VertexRegister);
					}

					DWORD XboxVertexElementDataType = (*pXboxToken & X_D3DVSD_DATATYPEMASK) >> X_D3DVSD_DATATYPESHIFT;
					switch (XboxVertexElementDataType) {
					case xbox::X_D3DVSDT_FLOAT1: // 0x12:
						DbgVshPrintf("D3DVSDT_FLOAT1");
						break;
					case xbox::X_D3DVSDT_FLOAT2: // 0x22:
						DbgVshPrintf("D3DVSDT_FLOAT2");
						break;
					case xbox::X_D3DVSDT_FLOAT3: // 0x32:
						DbgVshPrintf("D3DVSDT_FLOAT3");
						break;
					case xbox::X_D3DVSDT_FLOAT4: // 0x42:
						DbgVshPrintf("D3DVSDT_FLOAT4");
						break;
					case xbox::X_D3DVSDT_D3DCOLOR: // 0x40:
						DbgVshPrintf("D3DVSDT_D3DCOLOR");
						break;
					case xbox::X_D3DVSDT_SHORT2: // 0x25:
						DbgVshPrintf("D3DVSDT_SHORT2");
						break;
					case xbox::X_D3DVSDT_SHORT4: // 0x45:
						DbgVshPrintf("D3DVSDT_SHORT4");
						break;
					case xbox::X_D3DVSDT_NORMSHORT1: // 0x11:
						DbgVshPrintf("D3DVSDT_NORMSHORT1 /* xbox ext. */");
						bStreamNeedsPatching = true;
						break;
					case xbox::X_D3DVSDT_NORMSHORT2: // 0x21:
						if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT2N) {
							DbgVshPrintf("D3DVSDT_NORMSHORT2");
						} else {
							DbgVshPrintf("D3DVSDT_NORMSHORT2 /* xbox ext. */");
							bStreamNeedsPatching = true;
						}
						break;
					case xbox::X_D3DVSDT_NORMSHORT3: // 0x31:
						DbgVshPrintf("D3DVSDT_NORMSHORT3 /* xbox ext. */");
						bStreamNeedsPatching = true;
						break;
					case xbox::X_D3DVSDT_NORMSHORT4: // 0x41:
						if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT4N) {
							DbgVshPrintf("D3DVSDT_NORMSHORT4");
							// No need for patching in D3D9
						} else {
							DbgVshPrintf("D3DVSDT_NORMSHORT4 /* xbox ext. */");
							bStreamNeedsPatching = true;
						}
						break;
					case xbox::X_D3DVSDT_NORMPACKED3: // 0x16:
						DbgVshPrintf("D3DVSDT_NORMPACKED3 /* xbox ext. */");
						bStreamNeedsPatching = true;
						break;
					case xbox::X_D3DVSDT_SHORT1: // 0x15:
						DbgVshPrintf("D3DVSDT_SHORT1 /* xbox ext. */");
						bStreamNeedsPatching = true;
						break;
					case xbox::X_D3DVSDT_SHORT3: // 0x35:
						DbgVshPrintf("D3DVSDT_SHORT3 /* xbox ext. */");
						bStreamNeedsPatching = true;
						break;
					case xbox::X_D3DVSDT_PBYTE1: // 0x14:
						DbgVshPrintf("D3DVSDT_PBYTE1 /* xbox ext. */");
						bStreamNeedsPatching = true;
						break;
					case xbox::X_D3DVSDT_PBYTE2: // 0x24:
						DbgVshPrintf("D3DVSDT_PBYTE2 /* xbox ext. */");
						bStreamNeedsPatching = true;
						break;
					case xbox::X_D3DVSDT_PBYTE3: // 0x34:
						DbgVshPrintf("D3DVSDT_PBYTE3 /* xbox ext. */");
						bStreamNeedsPatching = true;
						break;
					case xbox::X_D3DVSDT_PBYTE4: // 0x44:
						if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
							DbgVshPrintf("D3DVSDT_PBYTE4");
						} else {
							DbgVshPrintf("D3DVSDT_PBYTE4 /* xbox ext. */");
							bStreamNeedsPatching = true;
						}
						break;
					case xbox::X_D3DVSDT_FLOAT2H: // 0x72:
						DbgVshPrintf("D3DVSDT_FLOAT2H /* xbox ext. */");
						bStreamNeedsPatching = true;
						break;
					case xbox::X_D3DVSDT_NONE: // 0x02:
						DbgVshPrintf("D3DVSDT_NONE /* xbox ext. */");
						break;
					default:
						DbgVshPrintf("Unknown data type for D3DVSD_REG: 0x%02X\n", XboxVertexElementDataType);
						break;
					}

					DbgVshPrintf("),\n");
				};
				break;
			}
			case xbox::X_D3DVSD_TOKEN_TESSELLATOR: {
				DWORD VertexRegisterOut = VshGetVertexRegister(*pXboxToken);
				if (*pXboxToken & X_D3DVSD_MASK_TESSUV) {
					DbgVshPrintf("\tD3DVSD_TESSUV(%s),\n", XboxVertexRegisterAsString(VertexRegisterOut));
				} else { // D3DVSD_TESSNORMAL
					DWORD VertexRegisterIn = VshGetVertexRegisterIn(*pXboxToken);
					DbgVshPrintf("\tD3DVSD_TESSNORMAL(%s, %s),\n",
						XboxVertexRegisterAsString(VertexRegisterIn),
						XboxVertexRegisterAsString(VertexRegisterOut));
				}
				break;
			}
			case xbox::X_D3DVSD_TOKEN_CONSTMEM: {
				DWORD ConstantAddress = (*pXboxToken & X_D3DVSD_CONSTADDRESSMASK) >> X_D3DVSD_CONSTADDRESSSHIFT;
				DWORD Count = (*pXboxToken & X_D3DVSD_CONSTCOUNTMASK) >> X_D3DVSD_CONSTCOUNTSHIFT;
				DbgVshPrintf("\tD3DVSD_CONST(%d, %d),\n", ConstantAddress, Count);
				LOG_TEST_CASE("X_D3DVSD_TOKEN_CONSTMEM");
				Step = Count * 4 + 1;
				break;
			}
			case xbox::X_D3DVSD_TOKEN_EXT: {
				DWORD ExtInfo = (*pXboxToken & X_D3DVSD_EXTINFOMASK) >> X_D3DVSD_EXTINFOSHIFT;
				DWORD Count = (*pXboxToken & X_D3DVSD_EXTCOUNTMASK) >> X_D3DVSD_EXTCOUNTSHIFT;
				DbgVshPrintf("\tD3DVSD_EXT(%d, %d),\n", ExtInfo, Count);
				LOG_TEST_CASE("X_D3DVSD_TOKEN_EXT");
				Step = Count * 4 + 1; // TODO : Is this correct?
				break;
			}
			default:
				DbgVshPrintf("Unknown token type: %d\n", VshGetTokenType(*pXboxToken));
				break;
			}

			pXboxToken += Step;
		}

		if (iNumberOfVertexStreams > 0) {
			DbgVshPrintf("\t// NeedPatching: %d\n", bStreamNeedsPatching);
		}

		DbgVshPrintf("\tD3DVSD_END()\n};\n");

		DbgVshPrintf("// NbrStreams: %d\n", iNumberOfVertexStreams);
	}

	static void VshConvertToken_NOP(DWORD *pXboxToken)
	{
		if(*pXboxToken != X_D3DVSD_NOP())
		{
			LOG_TEST_CASE("Token NOP found, but extra parameters are given!");
		}
	}

	static DWORD VshConvertToken_CONSTMEM(DWORD *pXboxToken)
	{
		// DWORD ConstantAddress = (*pXboxToken & X_D3DVSD_CONSTADDRESSMASK) >> X_D3DVSD_CONSTADDRESSSHIFT;
		DWORD Count           = (*pXboxToken & X_D3DVSD_CONSTCOUNTMASK) >> X_D3DVSD_CONSTCOUNTSHIFT;
		LOG_TEST_CASE("CONST"); // TODO : Implement
		return Count * 4 + 1;
	}

	void VshConvertToken_TESSELATOR(DWORD *pXboxToken)
	{
		BYTE Index;

		if(*pXboxToken & X_D3DVSD_MASK_TESSUV)
		{
			DWORD VertexRegister    = VshGetVertexRegister(*pXboxToken);
			DWORD NewVertexRegister = VertexRegister;

			NewVertexRegister = Xb2PCRegisterType(VertexRegister, Index);
			// TODO : Expand on the setting of this TESSUV register element :
			pRecompiled->Usage = D3DDECLUSAGE(NewVertexRegister);
			pRecompiled->UsageIndex = Index;
		}
		else // D3DVSD_TESSNORMAL
		{
			DWORD VertexRegisterIn  = VshGetVertexRegisterIn(*pXboxToken);
			DWORD VertexRegisterOut = VshGetVertexRegister(*pXboxToken);

			DWORD NewVertexRegisterIn  = VertexRegisterIn;
			DWORD NewVertexRegisterOut = VertexRegisterOut;

			NewVertexRegisterIn = Xb2PCRegisterType(VertexRegisterIn, Index);
			// TODO : Expand on the setting of this TESSNORMAL input register element :
			pRecompiled->Usage = D3DDECLUSAGE(NewVertexRegisterIn);
			pRecompiled->UsageIndex = Index;

			NewVertexRegisterOut = Xb2PCRegisterType(VertexRegisterOut, Index);
			// TODO : Expand on the setting of this TESSNORMAL output register element :
			pRecompiled++;
			pRecompiled->Usage = D3DDECLUSAGE(NewVertexRegisterOut);
			pRecompiled->UsageIndex = Index;
		}
	}

	void VshConvertToken_STREAM(DWORD *pXboxToken)
	{
		// D3DVSD_STREAM_TESS
		if(*pXboxToken & X_D3DVSD_STREAMTESSMASK)
		{
			// TODO
		}
		else // D3DVSD_STREAM
		{
			DWORD StreamNumber = VshGetVertexStream(*pXboxToken);

			// new stream
			pCurrentVertexShaderStreamInfo = &(pVertexDeclarationToSet->VertexStreams[StreamNumber]);
			pCurrentVertexShaderStreamInfo->NeedPatch = FALSE;
			pCurrentVertexShaderStreamInfo->DeclPosition = FALSE;
			pCurrentVertexShaderStreamInfo->CurrentStreamNumber = 0;
			pCurrentVertexShaderStreamInfo->HostVertexStride = 0;
			pCurrentVertexShaderStreamInfo->NumberOfVertexElements = 0;

			// Dxbx note : Use Dophin(s), FieldRender, MatrixPaletteSkinning and PersistDisplay as a testcase

			pCurrentVertexShaderStreamInfo->CurrentStreamNumber = VshGetVertexStream(*pXboxToken);
			pVertexDeclarationToSet->NumberOfVertexStreams++;
			// TODO : Keep a bitmask for all StreamNumber's seen?
		}
	}

	void VshConvert_RegisterVertexElement(
		UINT XboxVertexElementDataType,
		UINT XboxVertexElementByteSize,
		UINT HostVertexElementByteSize,
		BOOL NeedPatching)
	{
		CxbxVertexShaderStreamElement* pCurrentElement = &(pCurrentVertexShaderStreamInfo->VertexElements[pCurrentVertexShaderStreamInfo->NumberOfVertexElements]);
		pCurrentElement->XboxType = XboxVertexElementDataType;
		pCurrentElement->XboxByteSize = XboxVertexElementByteSize;
		pCurrentElement->HostByteSize = HostVertexElementByteSize;
		pCurrentVertexShaderStreamInfo->NumberOfVertexElements++;
		pCurrentVertexShaderStreamInfo->NeedPatch |= NeedPatching;
	}

	void VshConvert_SkipBytes(int SkipBytesCount)
	{
		if (SkipBytesCount % sizeof(DWORD)) {
			LOG_TEST_CASE("D3DVSD_SKIPBYTES not divisble by 4!");
		}
#if 0 // Potential optimization, for now disabled for simplicity :
		else {
			// Skip size is a whole multiple of 4 bytes;
			// Is stream patching not needed up until this element?
			if (!pCurrentVertexShaderStreamInfo->NeedPatch) {
				// Then we can get away with increasing the host stride,
				// which avoids otherwise needless vertex buffer patching :
				pCurrentVertexShaderStreamInfo->HostVertexStride += SkipBytesCount;
				return;
			}
		}
#endif

		// Register a 'skip' element, so that Xbox data will be skipped
		// without increasing host stride - this does require patching :
		VshConvert_RegisterVertexElement(xbox::X_D3DVSDT_NONE, SkipBytesCount, /*HostSize=*/0, /*NeedPatching=*/TRUE);
	}

	void VshConvertToken_STREAMDATA_SKIP(DWORD *pXboxToken)
	{
		WORD SkipCount = (*pXboxToken & X_D3DVSD_SKIPCOUNTMASK) >> X_D3DVSD_SKIPCOUNTSHIFT;
		VshConvert_SkipBytes(SkipCount * sizeof(DWORD));
	}

	void VshConvertToken_STREAMDATA_SKIPBYTES(DWORD* pXboxToken)
	{
		WORD SkipBytesCount = (*pXboxToken & X_D3DVSD_SKIPCOUNTMASK) >> X_D3DVSD_SKIPCOUNTSHIFT;
		VshConvert_SkipBytes(SkipBytesCount);
	}

	void VshConvertToken_STREAMDATA_REG(DWORD *pXboxToken)
	{
		DWORD VertexRegister = VshGetVertexRegister(*pXboxToken);
		BOOL NeedPatching = FALSE;
		BYTE Index;
		BYTE HostVertexRegisterType;

		if (IsFixedFunction) {
			HostVertexRegisterType = Xb2PCRegisterType(VertexRegister, Index);
		} else {
			// D3DDECLUSAGE_TEXCOORD can be useds for any user-defined data
			// We need this because there is no reliable way to detect the real usage
			// Xbox has no concept of 'usage types', it only requires a list of attribute register numbers.
			// So we treat them all as 'user-defined' with an Index of the Vertex Register Index
			// this prevents information loss in shaders due to non-matching dcl types!
			HostVertexRegisterType = D3DDECLUSAGE_TEXCOORD;
			Index = (BYTE)VertexRegister;
		}

		// Add this register to the list of declared registers
		RegVIsPresentInDeclaration[VertexRegister] = true;

		DWORD XboxVertexElementDataType = (*pXboxToken & X_D3DVSD_DATATYPEMASK) >> X_D3DVSD_DATATYPESHIFT;
		WORD XboxVertexElementByteSize = 0;
		BYTE HostVertexElementDataType = 0;
		WORD HostVertexElementByteSize = 0;

		switch (XboxVertexElementDataType)
		{
		case xbox::X_D3DVSDT_FLOAT1: // 0x12:
			HostVertexElementDataType = D3DDECLTYPE_FLOAT1;
			HostVertexElementByteSize = 1 * sizeof(FLOAT);
			break;
		case xbox::X_D3DVSDT_FLOAT2: // 0x22:
			HostVertexElementDataType = D3DDECLTYPE_FLOAT2;
			HostVertexElementByteSize = 2 * sizeof(FLOAT);
			break;
		case xbox::X_D3DVSDT_FLOAT3: // 0x32:
			HostVertexElementDataType = D3DDECLTYPE_FLOAT3;
			HostVertexElementByteSize = 3 * sizeof(FLOAT);
			break;
		case xbox::X_D3DVSDT_FLOAT4: // 0x42:
			HostVertexElementDataType = D3DDECLTYPE_FLOAT4;
			HostVertexElementByteSize = 4 * sizeof(FLOAT);
			break;
		case xbox::X_D3DVSDT_D3DCOLOR: // 0x40:
			HostVertexElementDataType = D3DDECLTYPE_D3DCOLOR;
			HostVertexElementByteSize = 1 * sizeof(D3DCOLOR);
			break;
		case xbox::X_D3DVSDT_SHORT2: // 0x25:
			HostVertexElementDataType = D3DDECLTYPE_SHORT2;
			HostVertexElementByteSize = 2 * sizeof(SHORT);
			break;
		case xbox::X_D3DVSDT_SHORT4: // 0x45:
			HostVertexElementDataType = D3DDECLTYPE_SHORT4;
			HostVertexElementByteSize = 4 * sizeof(SHORT);
			break;
		case xbox::X_D3DVSDT_NORMSHORT1: // 0x11:
			if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT2N) {
				HostVertexElementDataType = D3DDECLTYPE_SHORT2N;
				HostVertexElementByteSize = 2 * sizeof(SHORT);
			}
			else
			{
				HostVertexElementDataType = D3DDECLTYPE_FLOAT1;
				HostVertexElementByteSize = 1 * sizeof(FLOAT);
			}
			XboxVertexElementByteSize = 1 * sizeof(xbox::short_xt);
			NeedPatching = TRUE;
			break;
		case xbox::X_D3DVSDT_NORMSHORT2: // 0x21:
			if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT2N) {
				HostVertexElementDataType = D3DDECLTYPE_SHORT2N;
				HostVertexElementByteSize = 2 * sizeof(SHORT);
				// No need for patching in D3D9
			}
			else
			{
				HostVertexElementDataType = D3DDECLTYPE_FLOAT2;
				HostVertexElementByteSize = 2 * sizeof(FLOAT);
				XboxVertexElementByteSize = 2 * sizeof(xbox::short_xt);
				NeedPatching = TRUE;
			}
			break;
		case xbox::X_D3DVSDT_NORMSHORT3: // 0x31:
			if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT4N) {
				HostVertexElementDataType = D3DDECLTYPE_SHORT4N;
				HostVertexElementByteSize = 4 * sizeof(SHORT);
			}
			else
			{
				HostVertexElementDataType = D3DDECLTYPE_FLOAT3;
				HostVertexElementByteSize = 3 * sizeof(FLOAT);
			}
			XboxVertexElementByteSize = 3 * sizeof(xbox::short_xt);
			NeedPatching = TRUE;
			break;
		case xbox::X_D3DVSDT_NORMSHORT4: // 0x41:
			if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT4N) {
				HostVertexElementDataType = D3DDECLTYPE_SHORT4N;
				HostVertexElementByteSize = 4 * sizeof(SHORT);
				// No need for patching in D3D9
			}
			else
			{
				HostVertexElementDataType = D3DDECLTYPE_FLOAT4;
				HostVertexElementByteSize = 4 * sizeof(FLOAT);
				XboxVertexElementByteSize = 4 * sizeof(xbox::short_xt);
				NeedPatching = TRUE;
			}
			break;
		case xbox::X_D3DVSDT_NORMPACKED3: // 0x16:
			HostVertexElementDataType = D3DDECLTYPE_FLOAT3;
			HostVertexElementByteSize = 3 * sizeof(FLOAT);
			XboxVertexElementByteSize = 1 * sizeof(xbox::dword_xt);
			NeedPatching = TRUE;
			break;
		case xbox::X_D3DVSDT_SHORT1: // 0x15:
			HostVertexElementDataType = D3DDECLTYPE_SHORT2;
			HostVertexElementByteSize = 2 * sizeof(SHORT);
			XboxVertexElementByteSize = 1 * sizeof(xbox::short_xt);
			NeedPatching = TRUE;
			break;
		case xbox::X_D3DVSDT_SHORT3: // 0x35:
			HostVertexElementDataType = D3DDECLTYPE_SHORT4;
			HostVertexElementByteSize = 4 * sizeof(SHORT);
			XboxVertexElementByteSize = 3 * sizeof(xbox::short_xt);
			NeedPatching = TRUE;
			break;
		case xbox::X_D3DVSDT_PBYTE1: // 0x14:
			if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
				HostVertexElementDataType = D3DDECLTYPE_UBYTE4N;
				HostVertexElementByteSize = 4 * sizeof(BYTE);
			}
			else
			{
				HostVertexElementDataType = D3DDECLTYPE_FLOAT1;
				HostVertexElementByteSize = 1 * sizeof(FLOAT);
			}
			XboxVertexElementByteSize = 1 * sizeof(xbox::byte_xt);
			NeedPatching = TRUE;
			break;
		case xbox::X_D3DVSDT_PBYTE2: // 0x24:
			if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
				HostVertexElementDataType = D3DDECLTYPE_UBYTE4N;
				HostVertexElementByteSize = 4 * sizeof(BYTE);
			}
			else
			{
				HostVertexElementDataType = D3DDECLTYPE_FLOAT2;
				HostVertexElementByteSize = 2 * sizeof(FLOAT);
			}
			XboxVertexElementByteSize = 2 * sizeof(xbox::byte_xt);
			NeedPatching = TRUE;
			break;
		case xbox::X_D3DVSDT_PBYTE3: // 0x34:
			if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
				HostVertexElementDataType = D3DDECLTYPE_UBYTE4N;
				HostVertexElementByteSize = 4 * sizeof(BYTE);
			}
			else
			{
				HostVertexElementDataType = D3DDECLTYPE_FLOAT3;
				HostVertexElementByteSize = 3 * sizeof(FLOAT);
			}
			XboxVertexElementByteSize = 3 * sizeof(xbox::byte_xt);
			NeedPatching = TRUE;
			break;
		case xbox::X_D3DVSDT_PBYTE4: // 0x44:
			// Test-case : Panzer
			if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
				HostVertexElementDataType = D3DDECLTYPE_UBYTE4N;
				HostVertexElementByteSize = 4 * sizeof(BYTE);
				// No need for patching when D3D9 supports D3DDECLTYPE_UBYTE4N
			}
			else
			{
				HostVertexElementDataType = D3DDECLTYPE_FLOAT4;
				HostVertexElementByteSize = 4 * sizeof(FLOAT);
				XboxVertexElementByteSize = 4 * sizeof(xbox::byte_xt);
				NeedPatching = TRUE;
			}
			break;
		case xbox::X_D3DVSDT_FLOAT2H: // 0x72:
			HostVertexElementDataType = D3DDECLTYPE_FLOAT4;
			HostVertexElementByteSize = 4 * sizeof(FLOAT);
			XboxVertexElementByteSize = 3 * sizeof(FLOAT);
			NeedPatching = TRUE;
			break;
		case xbox::X_D3DVSDT_NONE: // 0x02:
			// No host element data, so no patching
			break;
		default:
			//LOG_TEST_CASE("Unknown data type for D3DVSD_REG: 0x%02X\n", XboxVertexElementDataType);
			break;
		}

		// On X_D3DVSDT_NONE skip this token
		if (XboxVertexElementDataType == xbox::X_D3DVSDT_NONE)
		{
			// Xbox elements with X_D3DVSDT_NONE have size zero, so there's no need to register those.
			// Note, that for skip tokens, we DO call VshConvert_RegisterVertexElement with a X_D3DVSDT_NONE!
			return;
		}

		// save patching information
		VshConvert_RegisterVertexElement(
			XboxVertexElementDataType,
			NeedPatching ? XboxVertexElementByteSize : HostVertexElementByteSize,
			HostVertexElementByteSize,
			NeedPatching);

		pRecompiled->Stream = pCurrentVertexShaderStreamInfo->CurrentStreamNumber;
		pRecompiled->Offset = pCurrentVertexShaderStreamInfo->HostVertexStride;
		pRecompiled->Type = HostVertexElementDataType;
		pRecompiled->Method = D3DDECLMETHOD_DEFAULT;
		pRecompiled->Usage = HostVertexRegisterType;
		pRecompiled->UsageIndex = Index;

		pRecompiled++;

		pCurrentVertexShaderStreamInfo->HostVertexStride += HostVertexElementByteSize;
	}

	void VshConvertToken_STREAMDATA(DWORD *pXboxToken)
	{
		if (*pXboxToken & X_D3DVSD_MASK_SKIP)
		{
			// For D3D9, use D3DDECLTYPE_UNUSED ?
			if (*pXboxToken & X_D3DVSD_MASK_SKIPBYTES) {
				VshConvertToken_STREAMDATA_SKIPBYTES(pXboxToken);
			} else {
				VshConvertToken_STREAMDATA_SKIP(pXboxToken);
			}
		}
		else // D3DVSD_REG
		{
			VshConvertToken_STREAMDATA_REG(pXboxToken);
		}
	}

	DWORD VshRecompileToken(DWORD *pXboxToken)
	{
		DWORD Step = 1;

		switch(VshGetTokenType(*pXboxToken))
		{
		case xbox::X_D3DVSD_TOKEN_NOP:
			VshConvertToken_NOP(pXboxToken);
			break;
		case xbox::X_D3DVSD_TOKEN_STREAM:
		{
			VshConvertToken_STREAM(pXboxToken);
			break;
		}
		case xbox::X_D3DVSD_TOKEN_STREAMDATA:
		{
			VshConvertToken_STREAMDATA(pXboxToken);
			break;
		}
		case xbox::X_D3DVSD_TOKEN_TESSELLATOR:
		{
			VshConvertToken_TESSELATOR(pXboxToken);
			break;
		}
		case xbox::X_D3DVSD_TOKEN_CONSTMEM:
		{
			Step = VshConvertToken_CONSTMEM(pXboxToken);
			break;
		}
		default:
			//LOG_TEST_CASE("Unknown token type: %d\n", VshGetTokenType(*pXboxToken));
			break;
		}

		return Step;
	}

	static DWORD* RemoveXboxDeclarationRedefinition(DWORD* pXboxDeclaration)
	{
		// Detect and remove register redefinitions by preprocessing the Xbox Vertex Declaration
		// Test Case: King Kong

		// Find the last token
		DWORD* pXboxToken = pXboxDeclaration;
		while (*pXboxToken != X_D3DVSD_END()){
			pXboxToken++;
		}

		// Operate on a copy of the Xbox declaration, rather than messing with the Xbox's memory
		auto declarationBytes = sizeof(DWORD) * (pXboxToken - pXboxDeclaration + 1);
		auto pXboxDeclarationCopy = (DWORD*)malloc(declarationBytes);
		memcpy(pXboxDeclarationCopy, pXboxDeclaration, declarationBytes);
		pXboxToken = pXboxDeclarationCopy + (pXboxToken - pXboxDeclaration); // Move to end of the copy

		// Remember if we've seen a given output register
		std::bitset<16> seen;

		// We want to keep later definitions, and remove earlier ones
		// Scan back from the end of the declaration, and replace redefinitions with nops
		while (pXboxToken > pXboxDeclarationCopy) {
			auto type = VshGetTokenType(*pXboxToken);
			if (type == xbox::X_D3DVSD_TOKEN_STREAMDATA && !(*pXboxToken & X_D3DVSD_MASK_SKIP) ||
				type == xbox::X_D3DVSD_TOKEN_TESSELLATOR)
			{
				auto outputRegister = VshGetVertexRegister(*pXboxToken);
				if (seen[outputRegister])
				{
					// Blank out tokens for mapped registers
					*pXboxToken = X_D3DVSD_NOP();
					EmuLog(LOG_LEVEL::DEBUG, "Replacing duplicate definition of register %d with D3DVSD_NOP", outputRegister);
				}
				else
				{
					// Mark register as seen
					seen[outputRegister] = true;
				}
			}

			pXboxToken--;
		}

		return pXboxDeclarationCopy;
	}

public:
	D3DVERTEXELEMENT *Convert(DWORD* pXboxDeclaration, bool bIsFixedFunction, CxbxVertexDeclaration* pCxbxVertexDeclaration)
	{
		// Get a preprocessed copy of the original Xbox Vertex Declaration
		auto pXboxVertexDeclarationCopy = RemoveXboxDeclarationRedefinition(pXboxDeclaration);

		pVertexDeclarationToSet = pCxbxVertexDeclaration;

		IsFixedFunction = bIsFixedFunction;

		RegVIsPresentInDeclaration.fill(false);

		// First of all some info:
		// We have to figure out which flags are set and then
		// we have to patch their params

		// some token values
		// 0xFFFFFFFF - end of the declaration
		// 0x00000000 - nop (means that this value is ignored)

		// Calculate size of declaration
		XboxDeclarationCount = VshGetDeclarationCount(pXboxVertexDeclarationCopy);
		// For Direct3D9, we need to reserve at least twice the number of elements, as one token can generate two registers (in and out) :
		unsigned HostDeclarationSize = XboxDeclarationCount * sizeof(D3DVERTEXELEMENT) * 2;
	
		D3DVERTEXELEMENT *Result = (D3DVERTEXELEMENT *)calloc(1, HostDeclarationSize);
		pRecompiled = Result;
		uint8_t *pRecompiledBufferOverflow = ((uint8_t*)pRecompiled) + HostDeclarationSize;

		VshDumpXboxDeclaration(pXboxDeclaration);

		auto pXboxToken = pXboxVertexDeclarationCopy;
		while (*pXboxToken != X_D3DVSD_END())
		{
			if ((uint8_t*)pRecompiled >= pRecompiledBufferOverflow) {
				DbgVshPrintf("Detected buffer-overflow, breaking out...\n");
				break;
			}

			DWORD Step = VshRecompileToken(pXboxToken);
			pXboxToken += Step;
		}

		*pRecompiled = D3DDECL_END();

		// Ensure valid ordering of the vertex declaration (http://doc.51windows.net/Directx9_SDK/graphics/programmingguide/gettingstarted/vertexdeclaration/vertexdeclaration.htm)
		// In particular "All vertex elements for a stream must be consecutive and sorted by offset"
		// Test case: King Kong (due to register redefinition)
		std::sort(Result, pRecompiled, [] (const auto& x, const auto& y)
			{ return std::tie(x.Stream, x.Method, x.Offset) < std::tie(y.Stream, y.Method, y.Offset); });

		// Free the preprocessed declaration copy
		free(pXboxVertexDeclarationCopy);

		// Record which registers are in the vertex declaration
		for (size_t i = 0; i < RegVIsPresentInDeclaration.size(); i++) {
			pCxbxVertexDeclaration->vRegisterInDeclaration[i] = RegVIsPresentInDeclaration[i];
		}

		return Result;
	}
};

D3DVERTEXELEMENT *EmuRecompileVshDeclaration
(
    DWORD                *pXboxDeclaration,
    bool                  bIsFixedFunction,
    DWORD                *pXboxDeclarationCount,
    CxbxVertexDeclaration *pCxbxVertexDeclaration
)
{
	XboxVertexDeclarationConverter Converter;

	D3DVERTEXELEMENT* pHostVertexElements = Converter.Convert(pXboxDeclaration, bIsFixedFunction, pCxbxVertexDeclaration);

	*pXboxDeclarationCount = Converter.XboxDeclarationCount;

    return pHostVertexElements;
}

extern void FreeVertexDynamicPatch(CxbxVertexShader *pVertexShader)
{
    pVertexShader->Declaration.NumberOfVertexStreams = 0;
}

// Checks for failed vertex shaders, and shaders that would need patching
boolean VshHandleIsValidShader(DWORD XboxVertexShaderHandle)
{
#if 0
	//printf( "VS = 0x%.08X\n", XboxVertexShaderHandle );

    CxbxVertexShader *pCxbxVertexShader = GetCxbxVertexShader(XboxVertexShaderHandle);
    if (pCxbxVertexShader) {
        if (pCxbxVertexShader->XboxStatus != 0)
        {
            return FALSE;
        }
        /*
        for (uint32 i = 0; i < pCxbxVertexShader->VertexShaderInfo.NumberOfVertexStreams; i++)
        {
            if (pCxbxVertexShader->VertexShaderInfo.VertexStreams[i].NeedPatch)
            {
                // Just for caching purposes
                pCxbxVertexShader->XboxStatus = 0x80000001;
                return FALSE;
            }
        }
        */
    }
#endif
    return TRUE;
}

extern boolean IsValidCurrentShader(void)
{
	// Dxbx addition : There's no need to call
	// XTL_EmuIDirect3DDevice_GetVertexShader, just check g_Xbox_VertexShader_Handle :
	return VshHandleIsValidShader(g_Xbox_VertexShader_Handle);
}

// Vertex shader state
static DWORD g_CxbxVertexShaderSlotAddress = 0;
static DWORD g_CxbxVertexShaderSlots[X_VSH_MAX_INSTRUCTION_COUNT * X_VSH_INSTRUCTION_SIZE] = { 0 };

DWORD* GetCxbxVertexShaderSlotPtr(const DWORD SlotIndexAddress)
{
	if (SlotIndexAddress < X_VSH_MAX_INSTRUCTION_COUNT) {
		return &g_CxbxVertexShaderSlots[SlotIndexAddress * X_VSH_INSTRUCTION_SIZE];
	} else {
		LOG_TEST_CASE("SlotIndexAddress out of range"); // FIXME : extend with value (once supported by LOG_TEST_CASE)
		return nullptr;
	}
}

CxbxVertexDeclaration *GetCxbxVertexDeclaration(DWORD XboxVertexShaderHandle)
{
    CxbxVertexShader *pCxbxVertexShader = GetCxbxVertexShader(XboxVertexShaderHandle);

    for (uint32_t i = 0; i < pCxbxVertexShader->Declaration.NumberOfVertexStreams; i++)
    {
        if (pCxbxVertexShader->Declaration.VertexStreams[i].NeedPatch)
        {
            return &pCxbxVertexShader->Declaration;
        }
    }
    return nullptr;
}

std::unordered_map<DWORD, CxbxVertexShader*> g_CxbxVertexShaders;

CxbxVertexShader* GetCxbxVertexShader(DWORD XboxVertexShaderHandle)
{
	if (VshHandleIsVertexShader(XboxVertexShaderHandle)) {
		auto it = g_CxbxVertexShaders.find(XboxVertexShaderHandle);
		if (it != g_CxbxVertexShaders.end()) {
			return it->second;
		}
	}

	return nullptr;
}

void RegisterCxbxVertexShader(DWORD XboxVertexShaderHandle, CxbxVertexShader* shader)
{
	auto it = g_CxbxVertexShaders.find(XboxVertexShaderHandle);
	if (it != g_CxbxVertexShaders.end() && it->second != nullptr && shader != nullptr) {
		LOG_TEST_CASE("Overwriting existing Vertex Shader");
	}

	g_CxbxVertexShaders[XboxVertexShaderHandle] = shader;
}

void SetCxbxVertexDeclaration(CxbxVertexDeclaration& pCxbxVertexDeclaration) {
	LOG_INIT

	HRESULT hRet;

	// Set vertex declaration
	hRet = g_pD3DDevice->SetVertexDeclaration(pCxbxVertexDeclaration.pHostVertexDeclaration);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexDeclaration");

	// Titles can specify default values for registers via calls like SetVertexData4f
	// HLSL shaders need to know whether to use vertex data or default vertex shader values
	// Any register not in the vertex declaration should be set to the default value
	float vertexDefaultFlags[X_VSH_MAX_ATTRIBUTES];
	for (int i = 0; i < X_VSH_MAX_ATTRIBUTES; i++) {
		vertexDefaultFlags[i] = pCxbxVertexDeclaration.vRegisterInDeclaration[i] ? 0.0f : 1.0f;
	}
	g_pD3DDevice->SetVertexShaderConstantF(CXBX_D3DVS_CONSTREG_VREGDEFAULTS_FLAG_BASE, vertexDefaultFlags, 4);
}

// TODO Call this when state is dirty in UpdateNativeD3DResources
// Rather than every time state changes
void SetVertexShaderFromSlots() {
	LOG_INIT

	auto pTokens = GetCxbxVertexShaderSlotPtr(g_CxbxVertexShaderSlotAddress);
	if (pTokens) {
		// Create a vertex shader from the tokens
		DWORD shaderSize;
		auto shaderKey = g_VertexShaderSource.CreateShader(pTokens, &shaderSize);
		HRESULT hRet = g_pD3DDevice->SetVertexShader(g_VertexShaderSource.GetShader(shaderKey));
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexShader");
	}
}

void SetCxbxVertexShaderHandle(CxbxVertexShader* pCxbxVertexShader)
{
	LOG_INIT

	HRESULT hRet;

	// Get vertex shader if we have a key
	auto pHostShader = pCxbxVertexShader->VertexShaderKey
		? g_VertexShaderSource.GetShader(pCxbxVertexShader->VertexShaderKey)
		: nullptr;

	// Set vertex shader
	hRet = g_pD3DDevice->SetVertexShader(pHostShader);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexShader");

	SetCxbxVertexDeclaration(pCxbxVertexShader->Declaration);
}

void CxbxImpl_SetVertexShaderInput
(
	DWORD              Handle,
	UINT               StreamCount,
	xbox::X_STREAMINPUT* pStreamInputs
)
{
	LOG_INIT

	// If Handle is NULL, all VertexShader input state is cleared.
	// Otherwise, Handle is the address of an Xbox VertexShader struct, or-ed with 1 (X_D3DFVF_RESERVED0)
	// (Thus, a FVF handle is an invalid argument.)
	//

	LOG_UNIMPLEMENTED();
}

void CxbxImpl_SelectVertexShaderDirect
(
	xbox::X_VERTEXATTRIBUTEFORMAT* pVAF,
	DWORD Address
)
{
	LOG_INIT;

	// When pVAF is non-null, this vertex attribute format takes precedence over the the one	
	LOG_UNIMPLEMENTED();
}

void CxbxImpl_SelectVertexShader(DWORD Handle, DWORD Address)
{
	// Address always indicates a previously loaded vertex shader slot (from where the program is used).
	// Handle can be null if the current Xbox VertexShader is assigned
	// Handle can be an address of an Xbox VertexShader struct, or-ed with 1 (X_D3DFVF_RESERVED0)
	// If Handle is assigned, it becomes the new current Xbox VertexShader,
	// which resets a bit of state (nv2a execution mode, viewport, ?)
	// Either way, the given address slot is selected as the start of the current vertex shader program
	g_Xbox_VertexShader_Handle = Handle;
	g_CxbxVertexShaderSlotAddress = Address;

	if (VshHandleIsVertexShader(Handle))
	{
		auto pCxbxVertexShader = GetCxbxVertexShader(Handle);
		if (pCxbxVertexShader == nullptr) {
			LOG_TEST_CASE("Shader handle has not been created");
		}
		else {
			// Set the shader handle declaration
			SetCxbxVertexDeclaration(pCxbxVertexShader->Declaration);
		}
	}

	SetVertexShaderFromSlots();
}

void CxbxImpl_LoadVertexShaderProgram(CONST DWORD* pFunction, DWORD Address)
{
	// D3DDevice_LoadVertexShaderProgram splits the given function buffer into batch-wise pushes to the NV2A

	// Copy shader instructions to shader slots
	auto CxbxVertexShaderSlotPtr = GetCxbxVertexShaderSlotPtr(Address);
	if (CxbxVertexShaderSlotPtr == nullptr)
		return;

	auto shaderHeader = *((xbox::X_VSH_SHADER_HEADER*) pFunction);
	auto tokens = &pFunction[1];
	memcpy(CxbxVertexShaderSlotPtr, tokens, shaderHeader.NumInst * X_VSH_INSTRUCTION_SIZE_BYTES);

	SetVertexShaderFromSlots();
}

void CxbxImpl_LoadVertexShader(DWORD Handle, DWORD Address)
{
	// Handle is always address of an X_D3DVertexShader struct, thus always or-ed with 1 (X_D3DFVF_RESERVED0)
	// Address is the slot (offset) from which the program must be written onwards (as whole DWORDS)
	// D3DDevice_LoadVertexShader pushes the program contained in the Xbox VertexShader struct to the NV2A
	auto CxbxVertexShaderSlotPtr = GetCxbxVertexShaderSlotPtr(Address);
	if (CxbxVertexShaderSlotPtr) {
		CxbxVertexShader* pCxbxVertexShader = GetCxbxVertexShader(Handle);
		if (pCxbxVertexShader) {
			// Make sure there is a shader function to load
			// from the shader handle
			if (pCxbxVertexShader->pXboxFunctionCopy == nullptr) {
				LOG_TEST_CASE("LoadVertexShader with FVF shader handle");
				return;
			}

			int upToSlot = Address + pCxbxVertexShader->XboxNrAddressSlots;
			if (upToSlot > X_VSH_MAX_INSTRUCTION_COUNT) {
				LOG_TEST_CASE("Shader does not fit in vertex shader slots");
				return;
			}

			// Skip the header DWORD at the beginning
			auto pTokens = &pCxbxVertexShader->pXboxFunctionCopy[1];
			memcpy(CxbxVertexShaderSlotPtr, pTokens, pCxbxVertexShader->XboxNrAddressSlots * X_VSH_INSTRUCTION_SIZE_BYTES);
		}
		else {
			LOG_TEST_CASE("LoadVertexShader called with unrecognized handle"); // FIXME  : extend with value (once supported by LOG_TEST_CASE)
		}
	}
}

void CxbxImpl_SetVertexShader(DWORD Handle)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

	// Checks if the Handle has bit 0 set - if not, it's a FVF
	// which is converted to a global Xbox Vertex Shader struct
	// Otherwise bit 0 is cleared and the resulting address is
	// validated to be a valid Xbox Vertex Shader
	// D3D state fields are updated.
	// If the shader contains a program, the handle is passed to
	// D3DDevice_LoadVertexShader and D3DDevice_SelectVertexShader.
	// Otherwise the shader is send using push buffer commands.

	HRESULT hRet = D3D_OK;

	g_Xbox_VertexShader_Handle = Handle;

	if (VshHandleIsVertexShader(Handle)) {
		CxbxVertexShader* pCxbxVertexShader = GetCxbxVertexShader(Handle);
		if (pCxbxVertexShader) {
			SetCxbxVertexShaderHandle(pCxbxVertexShader);

			// If the shader handle has a shader function
			// copy it to the shader slots
			if (pCxbxVertexShader->pXboxFunctionCopy != nullptr) {
				g_CxbxVertexShaderSlotAddress = 0;
				auto CxbxVertexShaderSlotPtr = GetCxbxVertexShaderSlotPtr(g_CxbxVertexShaderSlotAddress);
				if (CxbxVertexShaderSlotPtr) {
					// Skip the header DWORD at the beginning
					auto pTokens = &pCxbxVertexShader->pXboxFunctionCopy[1];
					memcpy(CxbxVertexShaderSlotPtr, pTokens, pCxbxVertexShader->XboxNrAddressSlots * X_VSH_INSTRUCTION_SIZE_BYTES);
				}
			}
		}
		else {
			EmuLog(LOG_LEVEL::DEBUG, "SetVertexShader with shader handle that has not been created");
		}
	}
	else {
		hRet = g_pD3DDevice->SetVertexShader(nullptr);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexShader");
		hRet = g_pD3DDevice->SetFVF(Handle);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetFVF");
	}
}

HRESULT CxbxImpl_CreateVertexShader(CONST DWORD *pDeclaration, CONST DWORD *pFunction, DWORD *pHandle, DWORD Usage)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

	HRESULT hRet = D3D_OK;

	if (g_pD3DDevice == nullptr) {
		LOG_TEST_CASE("D3DDevice_CreateVertexShader called before Direct3D_CreateDevice");
		// We lie to allow the game to continue for now, but it probably won't work well
		return 0; // == xbox::status_success
	}

	// HACK: TODO: support this situation
	if (pDeclaration == nullptr) {
		LOG_TEST_CASE("Vertex shader without declaration");
		*pHandle = xbox::zero;
		return D3D_OK;
	}

	// Now, we can create the host vertex shader
	DWORD             XboxDeclarationCount = 0;
	CxbxVertexShader* pCxbxVertexShader = (CxbxVertexShader*)calloc(1, sizeof(CxbxVertexShader));
	D3DVERTEXELEMENT* pRecompiledDeclaration = nullptr;

	pRecompiledDeclaration = EmuRecompileVshDeclaration((DWORD*)pDeclaration,
		/*bIsFixedFunction=*/pFunction == xbox::zeroptr,
		&XboxDeclarationCount,
		&pCxbxVertexShader->Declaration);

	// Create the vertex declaration
	hRet = g_pD3DDevice->CreateVertexDeclaration(pRecompiledDeclaration, &pCxbxVertexShader->Declaration.pHostVertexDeclaration);
	free(pRecompiledDeclaration);

	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateVertexDeclaration");

	if (FAILED(hRet)) {
		// NOTE: This is a fatal error because it ALWAYS triggers a crash within DrawVertices if not set
		CxbxKrnlCleanup("Failed to create Vertex Declaration");
	}
#if 0 // Creating a vertex shader doesn't imply activating it!
	hRet = g_pD3DDevice->SetVertexDeclaration(pCxbxVertexShader->Declaration.pHostVertexDeclaration);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexDeclaration");
	if (FAILED(hRet)) {
		CxbxKrnlCleanup("Failed to set Vertex Declaration");
	}
#endif

	// Set handle declaration properties
	pCxbxVertexShader->Declaration.pXboxDeclarationCopy = (DWORD*)malloc(XboxDeclarationCount * sizeof(DWORD));
	memcpy(pCxbxVertexShader->Declaration.pXboxDeclarationCopy, pDeclaration, XboxDeclarationCount * sizeof(DWORD));
	pCxbxVertexShader->Declaration.XboxDeclarationCount = XboxDeclarationCount;

	if (pFunction != xbox::zeroptr)
	{
		// Parse and compile the shader
		DWORD xboxFunctionSize = 0;
		pCxbxVertexShader->VertexShaderKey = g_VertexShaderSource.CreateShader(pFunction, &xboxFunctionSize);

		// Set handle shader function properties
		pCxbxVertexShader->XboxFunctionSize = xboxFunctionSize;
		pCxbxVertexShader->pXboxFunctionCopy = (DWORD*)malloc(xboxFunctionSize);
		memcpy(pCxbxVertexShader->pXboxFunctionCopy, pFunction, xboxFunctionSize);
		pCxbxVertexShader->XboxNrAddressSlots = (xboxFunctionSize - sizeof(xbox::X_VSH_SHADER_HEADER)) / X_VSH_INSTRUCTION_SIZE_BYTES;
		pCxbxVertexShader->XboxVertexShaderType = X_VST_NORMAL; // TODO : This can vary
	}

	// Save the status, to remove things later
	// pCxbxVertexShader->XboxStatus = hRet; // Not even used by VshHandleIsValidShader()

	RegisterCxbxVertexShader(*pHandle, pCxbxVertexShader);

	if (FAILED(hRet))
	{
#ifdef _DEBUG_TRACK_VS
		if (pFunction)
		{
			char pFileName[30];
			static int FailedShaderCount = 0;
			xbox::X_VSH_SHADER_HEADER* pHeader = (xbox::X_VSH_SHADER_HEADER*)pFunction;
			EmuLog(LOG_LEVEL::WARNING, "Couldn't create vertex shader!");
			sprintf(pFileName, "failed%05d.xvu", FailedShaderCount);
			FILE* f = fopen(pFileName, "wb");
			if (f)
			{
				fwrite(pFunction, sizeof(xbox::X_VSH_SHADER_HEADER) + pHeader->NumInst * 16, 1, f);
				fclose(f);
			}
			FailedShaderCount++;
		}
#endif // _DEBUG_TRACK_VS
		//hRet = D3D_OK;
	}

	return hRet;
}

void CxbxImpl_DeleteVertexShader(DWORD Handle)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

	// Handle is always address of an Xbox VertexShader struct, or-ed with 1 (X_D3DFVF_RESERVED0)
	// It's reference count is lowered. If it reaches zero (0), the struct is freed.

	if (VshHandleIsVertexShader(Handle))
	{
		CxbxVertexShader* pCxbxVertexShader = GetCxbxVertexShader(Handle); // Fetch from cache
		if (pCxbxVertexShader == nullptr) {
			return; // Avoid crash if no shader was cached yet
		}

		RegisterCxbxVertexShader(Handle, nullptr); // Remove from cache

		if (pCxbxVertexShader->Declaration.pHostVertexDeclaration) {
			HRESULT hRet = pCxbxVertexShader->Declaration.pHostVertexDeclaration->Release();
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DeleteVertexShader(pHostVertexDeclaration)");
		}

		// Release the host vertex shader
		g_VertexShaderSource.ReleaseShader(pCxbxVertexShader->VertexShaderKey);

		if (pCxbxVertexShader->Declaration.pXboxDeclarationCopy)
		{
			free(pCxbxVertexShader->Declaration.pXboxDeclarationCopy);
		}

		if (pCxbxVertexShader->pXboxFunctionCopy)
		{
			free(pCxbxVertexShader->pXboxFunctionCopy);
		}

		FreeVertexDynamicPatch(pCxbxVertexShader);

		free(pCxbxVertexShader);
	}
}

void CxbxImpl_SetVertexShaderConstant(INT Register, PVOID pConstantData, DWORD ConstantCount)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

/*#ifdef _DEBUG_TRACK_VS_CONST
	for (uint32_t i = 0; i < ConstantCount; i++)
	{
		printf("SetVertexShaderConstant, c%d  = { %f, %f, %f, %f }\n",
			   Register + i,
			   *((float*)pConstantData + 4 * i),
			   *((float*)pConstantData + 4 * i + 1),
			   *((float*)pConstantData + 4 * i + 2),
			   *((float*)pConstantData + 4 * i + 3));
	}
#endif*/ // _DEBUG_TRACK_VS_CONST

// Xbox vertex shader constants range from -96 to 95
// The host does not support negative, so we adjust to 0..191
	Register += X_D3DSCM_CORRECTION;

	if (Register < 0) LOG_TEST_CASE("Register < 0");
	if (Register + ConstantCount > X_D3DVS_CONSTREG_COUNT) LOG_TEST_CASE("Register + ConstantCount > X_D3DVS_CONSTREG_COUNT");
	HRESULT hRet;
	hRet = g_pD3DDevice->SetVertexShaderConstantF(
		Register,
		(float*)pConstantData,
		ConstantCount
	);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexShaderConstant");

	if (FAILED(hRet))
	{
		EmuLog(LOG_LEVEL::WARNING, "We're lying about setting a vertex shader constant!");
		hRet = D3D_OK;
	}
}
// parse xbox vertex shader function into an intermediate format
extern void EmuParseVshFunction
(
	DWORD* pXboxFunction,
	DWORD* pXboxFunctionSize,
	IntermediateVertexShader* pShader
)
{
	auto VshDecoder = XboxVertexShaderDecoder();

	*pXboxFunctionSize = 0;

	// FIXME tidy handling of the header vs headerless cases
	// Normally, pXboxFunction has a shader header before the shader tokens
	// But we can also load shader tokens directly from the Xbox vertex shader slots too

	bool headerless = pXboxFunction[0] == 0; // if its a token instead of a header, first DWORD is unused
	auto headerSize = headerless ? 0 : sizeof(xbox::X_VSH_SHADER_HEADER);

	// Decode the vertex shader program tokens into an intermediate representation
	uint32_t* pCurToken = (uint32_t*)((uintptr_t)pXboxFunction + headerSize);

	if (headerless) {
		// We've been fed shader slots. Make up a header...
		pShader->Header.Version = VERSION_XVS;
		pShader->Header.NumInst = (uint16_t)pShader->Instructions.size();

		// Decode until we hit a token marked final
		while (VshDecoder.VshConvertToIntermediate(pCurToken, pShader)) {
			pCurToken += X_VSH_INSTRUCTION_SIZE;
		}
	}
	else {
		pShader->Header = *(xbox::X_VSH_SHADER_HEADER*)pXboxFunction;
		// Decode only up to the number of instructions in the header
		// The last instruction may not be marked final:
		// Test case: Multiple Vertex Shaders sample
		for (int i = 0; i < pShader->Header.NumInst; i++) {
			if (!VshDecoder.VshConvertToIntermediate(pCurToken, pShader)) {
				if (i < pShader->Header.NumInst - 1) {
					LOG_TEST_CASE("Shader instructions after final instruction");
				}
				break;
			}
			pCurToken += X_VSH_INSTRUCTION_SIZE;
		}
	}

	// The size of the shader is
	pCurToken += X_VSH_INSTRUCTION_SIZE; // always at least one token
	*pXboxFunctionSize = (intptr_t)pCurToken - (intptr_t)pXboxFunction;
}
