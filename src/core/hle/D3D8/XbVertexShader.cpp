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

#define _DEBUG_TRACK_VS

#include "core\kernel\init\CxbxKrnl.h"
#include "core\kernel\support\Emu.h"
#include "core\hle\D3D8\Direct3D9\Direct3D9.h" // For g_Xbox_VertexShader_Handle
#include "core\hle\D3D8\XbVertexShader.h"

#include "XbD3D8Types.h" // For X_D3DVSDE_*
#include <sstream>
#include <regex>
#include <unordered_map>
#include <array>
#include <bitset>

//#define CXBX_USE_VS30 // Separate the port to Vertex Shader model 3.0 from the port to Direct3D9
#ifdef CXBX_USE_VS30
	#define VSH_MAX_INSTRUCTION_COUNT VSH_VS30_MAX_INSTRUCTION_COUNT // == 512
#else 
	#define VSH_MAX_INSTRUCTION_COUNT VSH_VS2X_MAX_INSTRUCTION_COUNT // == 256
#endif

// Internal Vertex Shader version (mustn't conflict with any VERSION_XVS*)
#define VERSION_CXBX 0x7863 // 'cx' Cxbx vertex shader, not an official value, used in VshConvertShader() and VshWriteShader()

#define DbgVshPrintf \
	LOG_CHECK_ENABLED(LOG_LEVEL::DEBUG) \
		if(g_bPrintfOn) printf

// ****************************************************************************
// * Vertex shader function recompiler
// ****************************************************************************

typedef enum _VSH_SWIZZLE
{
	SWIZZLE_X = 0,
	SWIZZLE_Y,
	SWIZZLE_Z,
	SWIZZLE_W
}
VSH_SWIZZLE;

typedef struct DxbxSwizzles { VSH_SWIZZLE s[4]; } DxbxSwizzles;

typedef DWORD DxbxMask,
*PDxbxMask;

#define MASK_X 0x001
#define MASK_Y 0x002
#define MASK_Z 0x004
#define MASK_W 0x008
#define MASK_XYZ MASK_X | MASK_Y | MASK_Z
#define MASK_XYZW MASK_X | MASK_Y | MASK_Z | MASK_W

// Local types
typedef enum _VSH_FIELD_NAME
{
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
    FLD_OUT_MAC_MASK_X,
    FLD_OUT_MAC_MASK_Y,
    FLD_OUT_MAC_MASK_Z,
    FLD_OUT_MAC_MASK_W,
    FLD_OUT_R,
    FLD_OUT_ILU_MASK_X,
    FLD_OUT_ILU_MASK_Y,
    FLD_OUT_ILU_MASK_Z,
    FLD_OUT_ILU_MASK_W,
    FLD_OUT_O_MASK_X,
    FLD_OUT_O_MASK_Y,
    FLD_OUT_O_MASK_Z,
    FLD_OUT_O_MASK_W,
    FLD_OUT_ORB,
    FLD_OUT_ADDRESS,
    FLD_OUT_MUX,
    // Relative addressing
    FLD_A0X,
    // Final instruction
    FLD_FINAL
}
VSH_FIELD_NAME;

typedef enum _VSH_OREG_NAME
{
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
}
VSH_OREG_NAME;

typedef enum _VSH_OUTPUT_TYPE
{
    OUTPUT_C = 0,
    OUTPUT_O
}
VSH_OUTPUT_TYPE;

typedef enum _VSH_ARGUMENT_TYPE
{
    PARAM_UNKNOWN = 0,
    PARAM_R,          // Temporary registers
    PARAM_V,          // Vertex registers
    PARAM_C,          // Constant registers, set by SetVertexShaderConstant
    PARAM_O
}
VSH_ARGUMENT_TYPE;

typedef VSH_ARGUMENT_TYPE VSH_PARAMETER_TYPE; // Alias, to indicate difference between a parameter and a generic argument

typedef enum _VSH_OUTPUT_MUX
{
    OMUX_MAC = 0,
    OMUX_ILU
}
VSH_OUTPUT_MUX;

typedef enum _VSH_IMD_OUTPUT_TYPE
{
    IMD_OUTPUT_C,
    IMD_OUTPUT_R,
    IMD_OUTPUT_O,
    IMD_OUTPUT_A0X
}
VSH_IMD_OUTPUT_TYPE;

// Dxbx note : ILU stands for 'Inverse Logic Unit' opcodes
typedef enum _VSH_ILU
{
    ILU_NOP = 0,
    ILU_MOV,
    ILU_RCP,
    ILU_RCC,
    ILU_RSQ,
    ILU_EXP,
    ILU_LOG,
    ILU_LIT // = 7 - all values of the 3 bits are used
}
VSH_ILU;

// Dxbx note : MAC stands for 'Multiply And Accumulate' opcodes
typedef enum _VSH_MAC
{
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
}
VSH_MAC;

typedef struct _VSH_OPCODE_PARAMS
{
	// Dxbx Note : Since we split up g_OpCodeParams into g_OpCodeParams_ILU and g_OpCodeParams_MAC
	// the following two members aren't needed anymore :
	// VSH_ILU   ILU;
    // VSH_MAC   MAC;
    boolean   A;
    boolean   B;
    boolean   C;
}
VSH_OPCODE_PARAMS;

typedef struct _VSH_PARAMETER
{
    VSH_PARAMETER_TYPE  ParameterType;   // Parameter type, R, V or C
    boolean             Neg;             // TRUE if negated, FALSE if not
    VSH_SWIZZLE         Swizzle[4];      // The four swizzles
    int16_t             Address;         // Register address
}
VSH_PARAMETER;

typedef struct _VSH_OUTPUT
{
    // Output register
    VSH_OUTPUT_MUX      OutputMux;       // MAC or ILU used as output
	VSH_OUTPUT_TYPE     OutputType;      // C or O
    boolean             OutputMask[4];
    int16_t             OutputAddress;
    // MAC output R register
    boolean             MACRMask[4];
    boolean             MACRAddress;
    // ILU output R register
    boolean             ILURMask[4];
    boolean             ILURAddress;
}
VSH_OUTPUT;

// The raw, parsed shader instruction (can be many combined [paired] instructions)
typedef struct _VSH_SHADER_INSTRUCTION
{
    VSH_ILU       ILU;
    VSH_MAC       MAC;
    VSH_OUTPUT    Output;
    VSH_PARAMETER A;
    VSH_PARAMETER B;
    VSH_PARAMETER C;
    boolean       a0x;
}
VSH_SHADER_INSTRUCTION;

typedef enum _VSH_IMD_INSTRUCTION_TYPE
{
    IMD_MAC,
    IMD_ILU
}
VSH_IMD_INSTRUCTION_TYPE;

typedef struct _VSH_IMD_OUTPUT
{
    VSH_IMD_OUTPUT_TYPE Type;
    boolean             Mask[4];
    int16_t             Address;
}
VSH_IMD_OUTPUT;

typedef struct _VSH_IMD_PARAMETER
{
    boolean         Active;
    VSH_PARAMETER   Parameter;
	// There is only a single address register in Microsoft DirectX 8.0.
	// The address register, designated as a0.x, may be used as signed
	// integer offset in relative addressing into the constant register file.
	//     c[a0.x + n]
	boolean         IndexesWithA0_X;
}
VSH_IMD_PARAMETER;

typedef struct _VSH_INTERMEDIATE_FORMAT
{

    boolean                  IsCombined;
    VSH_IMD_INSTRUCTION_TYPE InstructionType;
    VSH_MAC                  MAC;
    VSH_ILU                  ILU;
    VSH_IMD_OUTPUT           Output;
    VSH_IMD_PARAMETER        Parameters[3];
}
VSH_INTERMEDIATE_FORMAT;

// Used for xvu spec definition
typedef struct _VSH_FIELDMAPPING
{
    VSH_FIELD_NAME  FieldName;
    uint8_t          SubToken;
    uint8_t          StartBit;
    uint8_t          BitLength;
}
VSH_FIELDMAPPING;

typedef struct _VSH_XBOX_SHADER
{
    XTL::X_VSH_SHADER_HEADER       ShaderHeader;
    uint16_t                IntermediateCount;
    VSH_INTERMEDIATE_FORMAT Intermediate[VSH_MAX_INTERMEDIATE_COUNT];
}
VSH_XBOX_SHADER;

// Local constants
static const VSH_FIELDMAPPING g_FieldMapping[] =
{
    // Field Name         DWORD BitPos BitSize
    {  FLD_ILU,              1,   25,     3 },
    {  FLD_MAC,              1,   21,     4 },
    {  FLD_CONST,            1,   13,     8 },
    {  FLD_V,                1,    9,     4 },
    // Input A
    {  FLD_A_NEG,            1,    8,     1 },
    {  FLD_A_SWZ_X,          1,    6,     2 },
    {  FLD_A_SWZ_Y,          1,    4,     2 },
    {  FLD_A_SWZ_Z,          1,    2,     2 },
    {  FLD_A_SWZ_W,          1,    0,     2 },
    {  FLD_A_R,              2,   28,     4 },
    {  FLD_A_MUX,            2,   26,     2 },
    // Input B
    {  FLD_B_NEG,            2,   25,     1 },
    {  FLD_B_SWZ_X,          2,   23,     2 },
    {  FLD_B_SWZ_Y,          2,   21,     2 },
    {  FLD_B_SWZ_Z,          2,   19,     2 },
    {  FLD_B_SWZ_W,          2,   17,     2 },
    {  FLD_B_R,              2,   13,     4 },
    {  FLD_B_MUX,            2,   11,     2 },
    // Input C
    {  FLD_C_NEG,            2,   10,     1 },
    {  FLD_C_SWZ_X,          2,    8,     2 },
    {  FLD_C_SWZ_Y,          2,    6,     2 },
    {  FLD_C_SWZ_Z,          2,    4,     2 },
    {  FLD_C_SWZ_W,          2,    2,     2 },
    {  FLD_C_R_HIGH,         2,    0,     2 },
    {  FLD_C_R_LOW,          3,   30,     2 },
    {  FLD_C_MUX,            3,   28,     2 },
    // Output
    {  FLD_OUT_MAC_MASK_X,   3,   27,     1 },
    {  FLD_OUT_MAC_MASK_Y,   3,   26,     1 },
    {  FLD_OUT_MAC_MASK_Z,   3,   25,     1 },
    {  FLD_OUT_MAC_MASK_W,   3,   24,     1 },
    {  FLD_OUT_R,            3,   20,     4 },
    {  FLD_OUT_ILU_MASK_X,   3,   19,     1 },
    {  FLD_OUT_ILU_MASK_Y,   3,   18,     1 },
    {  FLD_OUT_ILU_MASK_Z,   3,   17,     1 },
    {  FLD_OUT_ILU_MASK_W,   3,   16,     1 },
    {  FLD_OUT_O_MASK_X,     3,   15,     1 },
    {  FLD_OUT_O_MASK_Y,     3,   14,     1 },
    {  FLD_OUT_O_MASK_Z,     3,   13,     1 },
    {  FLD_OUT_O_MASK_W,     3,   12,     1 },
    {  FLD_OUT_ORB,          3,   11,     1 },
    {  FLD_OUT_ADDRESS,      3,    3,     8 },
    {  FLD_OUT_MUX,          3,    2,     1 },
	// Relative addressing
    {  FLD_A0X,              3,    1,     1 },
	// Final instruction
	{  FLD_FINAL,            3,    0,     1 }
};

static const VSH_OPCODE_PARAMS g_OpCodeParams_ILU[] =
{
	//  ILU OP   MAC OP      ParamA ParamB ParamC
	{ /*ILU_NOP, MAC_NOP, */ FALSE, FALSE, FALSE },  // Dxbx note : Unused
	{ /*ILU_MOV, MAC_NOP, */ FALSE, FALSE, TRUE  },
	{ /*ILU_RCP, MAC_NOP, */ FALSE, FALSE, TRUE  },
	{ /*ILU_RCC, MAC_NOP, */ FALSE, FALSE, TRUE  },
	{ /*ILU_RSQ, MAC_NOP, */ FALSE, FALSE, TRUE  },
	{ /*ILU_EXP, MAC_NOP, */ FALSE, FALSE, TRUE  },
	{ /*ILU_LOG, MAC_NOP, */ FALSE, FALSE, TRUE  },
	{ /*ILU_LIT, MAC_NOP, */ FALSE, FALSE, TRUE  },
};

static const VSH_OPCODE_PARAMS g_OpCodeParams_MAC[] =
{
	//  ILU OP   MAC OP      ParamA ParamB ParamC
	{ /*ILU_NOP, MAC_NOP, */ FALSE, FALSE, FALSE },  // Dxbx note : Unused
	{ /*ILU_NOP, MAC_MOV, */ TRUE,  FALSE, FALSE },
	{ /*ILU_NOP, MAC_MUL, */ TRUE,  TRUE,  FALSE },
    { /*ILU_NOP, MAC_ADD, */ TRUE,  FALSE, TRUE  },
    { /*ILU_NOP, MAC_MAD, */ TRUE,  TRUE,  TRUE  },
    { /*ILU_NOP, MAC_DP3, */ TRUE,  TRUE,  FALSE },
    { /*ILU_NOP, MAC_DPH, */ TRUE,  TRUE,  FALSE },
    { /*ILU_NOP, MAC_DP4, */ TRUE,  TRUE,  FALSE },
    { /*ILU_NOP, MAC_DST, */ TRUE,  TRUE,  FALSE },
    { /*ILU_NOP, MAC_MIN, */ TRUE,  TRUE,  FALSE },
    { /*ILU_NOP, MAC_MAX, */ TRUE,  TRUE,  FALSE },
    { /*ILU_NOP, MAC_SLT, */ TRUE,  TRUE,  FALSE },
    { /*ILU_NOP, MAC_SGE, */ TRUE,  TRUE,  FALSE },
    { /*ILU_NOP, MAC_ARL, */ TRUE,  FALSE, FALSE }
};

static const char* MAC_OpCode[] =
{
    "nop",
    "mov",
    "mul",
    "add",
    "mad",
    "dp3",
    "dph",
    "dp4",
    "dst",
    "min",
    "max",
    "slt",
    "sge",
    "mova", // really "arl" Dxbx note : Alias for 'mov a0.x'
    "???",
    "???"
};

static const char* ILU_OpCode[] =
{
    "nop",
    "mov",
    "rcp",
    "rcc",
    "rsq",
    "expp", // The Xbox EXPP instruction behaves like vs_1_1
    "log",
    "lit"
};

static const char* OReg_Name[] =
{
    "oPos",
    "???",
    "???",
    "oD0",
    "oD1",
    "oFog",
    "oPts",
    "oB0",
    "oB1",
    "oT0",
    "oT1",
    "oT2",
    "oT3",
    "???",
    "???",
    "a0.x"
};

std::array<bool, 16> RegVIsPresentInDeclaration;
std::array<bool, 16> RegVIsUsedByShader;

/* TODO : map non-FVF Xbox vertex shader handle to CxbxVertexShader (a struct containing a host Xbox vertex shader handle and the original members)
std::unordered_map<DWORD, CxbxVertexShader> g_CxbxVertexShaders;

void CxbxUpdateVertexShader(DWORD XboxVertexShaderHandle)
{
	CxbxVertexShader &VertexShader = g_CxbxVertexShaders[XboxVertexShaderHandle];
}*/

static inline int IsInUse(const boolean *pMask)
{
    return (pMask[0] || pMask[1] || pMask[2] || pMask[3]);
}

static inline boolean HasMACR(VSH_SHADER_INSTRUCTION *pInstruction)
{
    return IsInUse(pInstruction->Output.MACRMask) && pInstruction->MAC != MAC_NOP;
}

static inline boolean HasMACO(VSH_SHADER_INSTRUCTION *pInstruction)
{
    return IsInUse(pInstruction->Output.OutputMask) &&
            pInstruction->Output.OutputMux == OMUX_MAC &&
            pInstruction->MAC != MAC_NOP;
}

static inline boolean HasMACARL(VSH_SHADER_INSTRUCTION *pInstruction)
{
    return /*!IsInUse(pInstruction->Output.OutputMask) &&
            pInstruction->Output.OutputMux == OMUX_MAC &&*/
            pInstruction->MAC == MAC_ARL;
}

static inline boolean HasILUR(VSH_SHADER_INSTRUCTION *pInstruction)
{
    return IsInUse(pInstruction->Output.ILURMask) && pInstruction->ILU != ILU_NOP;
}

static inline boolean HasILUO(VSH_SHADER_INSTRUCTION *pInstruction)
{
    return IsInUse(pInstruction->Output.OutputMask) &&
            pInstruction->Output.OutputMux == OMUX_ILU &&
            pInstruction->ILU != ILU_NOP;
}

// Retrieves a number of bits in the instruction token
static inline int VshGetFromToken(uint32_t *pShaderToken,
                                  uint8_t SubToken,
                                  uint8_t StartBit,
                                  uint8_t BitLength)
{
    return (pShaderToken[SubToken] >> StartBit) & ~(0xFFFFFFFF << BitLength);
}

// Converts the C register address to disassembly format
static inline int16_t ConvertCRegister(const int16_t CReg)
{
    return ((((CReg >> 5) & 7) - 3) * 32) + (CReg & 31);
}

uint8_t VshGetField(uint32_t         *pShaderToken,
                   VSH_FIELD_NAME FieldName)
{
    return (uint8_t)(VshGetFromToken(pShaderToken,
                                   g_FieldMapping[FieldName].SubToken,
                                   g_FieldMapping[FieldName].StartBit,
                                   g_FieldMapping[FieldName].BitLength));
}

static VSH_OPCODE_PARAMS* VshGetOpCodeParams(VSH_ILU ILU,
                                             VSH_MAC MAC)
{
	if (ILU >= ILU_MOV && ILU <= ILU_LIT)
		return (VSH_OPCODE_PARAMS*)&g_OpCodeParams_ILU[ILU];
	else
		if (MAC >= MAC_MOV && MAC <= MAC_ARL)
			return (VSH_OPCODE_PARAMS*)&g_OpCodeParams_MAC[MAC];
		else
		    return nullptr;
}

static void VshParseInstruction(uint32_t               *pShaderToken,
                                VSH_SHADER_INSTRUCTION *pInstruction)
{
    // First get the instruction(s).
    pInstruction->ILU = (VSH_ILU)VshGetField(pShaderToken, FLD_ILU);
    pInstruction->MAC = (VSH_MAC)VshGetField(pShaderToken, FLD_MAC);

    // Get parameter A
    pInstruction->A.ParameterType = (VSH_PARAMETER_TYPE)VshGetField(pShaderToken, FLD_A_MUX);
    switch(pInstruction->A.ParameterType)
    {
    case PARAM_R:
        pInstruction->A.Address = VshGetField(pShaderToken, FLD_A_R);
        break;
    case PARAM_V:
        pInstruction->A.Address = VshGetField(pShaderToken, FLD_V);
        break;
    case PARAM_C:
        pInstruction->A.Address = ConvertCRegister(VshGetField(pShaderToken, FLD_CONST));
        break;
    default:
        EmuLog(LOG_LEVEL::WARNING, "Invalid instruction, parameter A type unknown %d", pInstruction->A.ParameterType);
        return;
    }
    pInstruction->A.Neg = VshGetField(pShaderToken, FLD_A_NEG);
    pInstruction->A.Swizzle[0] = (VSH_SWIZZLE)VshGetField(pShaderToken, FLD_A_SWZ_X);
    pInstruction->A.Swizzle[1] = (VSH_SWIZZLE)VshGetField(pShaderToken, FLD_A_SWZ_Y);
    pInstruction->A.Swizzle[2] = (VSH_SWIZZLE)VshGetField(pShaderToken, FLD_A_SWZ_Z);
    pInstruction->A.Swizzle[3] = (VSH_SWIZZLE)VshGetField(pShaderToken, FLD_A_SWZ_W);
    // Get parameter B
    pInstruction->B.ParameterType = (VSH_PARAMETER_TYPE)VshGetField(pShaderToken, FLD_B_MUX);
    switch(pInstruction->B.ParameterType)
    {
    case PARAM_R:
        pInstruction->B.Address = VshGetField(pShaderToken, FLD_B_R);
        break;
    case PARAM_V:
        pInstruction->B.Address = VshGetField(pShaderToken, FLD_V);
        break;
    case PARAM_C:
        pInstruction->B.Address = ConvertCRegister(VshGetField(pShaderToken, FLD_CONST));
        break;
    default:
        DbgVshPrintf("Invalid instruction, parameter B type unknown %d\n", pInstruction->B.ParameterType);
        return;
    }
    pInstruction->B.Neg = VshGetField(pShaderToken, FLD_B_NEG);
    pInstruction->B.Swizzle[0] = (VSH_SWIZZLE)VshGetField(pShaderToken, FLD_B_SWZ_X);
    pInstruction->B.Swizzle[1] = (VSH_SWIZZLE)VshGetField(pShaderToken, FLD_B_SWZ_Y);
    pInstruction->B.Swizzle[2] = (VSH_SWIZZLE)VshGetField(pShaderToken, FLD_B_SWZ_Z);
    pInstruction->B.Swizzle[3] = (VSH_SWIZZLE)VshGetField(pShaderToken, FLD_B_SWZ_W);
    // Get parameter C
    pInstruction->C.ParameterType = (VSH_PARAMETER_TYPE)VshGetField(pShaderToken, FLD_C_MUX);
    switch(pInstruction->C.ParameterType)
    {
    case PARAM_R:
        pInstruction->C.Address = VshGetField(pShaderToken, FLD_C_R_HIGH) << 2 |
                                  VshGetField(pShaderToken, FLD_C_R_LOW);
        break;
    case PARAM_V:
        pInstruction->C.Address = VshGetField(pShaderToken, FLD_V);
        break;
    case PARAM_C:
        pInstruction->C.Address = ConvertCRegister(VshGetField(pShaderToken, FLD_CONST));
        break;
    default:
        DbgVshPrintf("Invalid instruction, parameter C type unknown %d\n", pInstruction->C.ParameterType);
        return;
    }
    pInstruction->C.Neg = VshGetField(pShaderToken, FLD_C_NEG);
    pInstruction->C.Swizzle[0] = (VSH_SWIZZLE)VshGetField(pShaderToken, FLD_C_SWZ_X);
    pInstruction->C.Swizzle[1] = (VSH_SWIZZLE)VshGetField(pShaderToken, FLD_C_SWZ_Y);
    pInstruction->C.Swizzle[2] = (VSH_SWIZZLE)VshGetField(pShaderToken, FLD_C_SWZ_Z);
    pInstruction->C.Swizzle[3] = (VSH_SWIZZLE)VshGetField(pShaderToken, FLD_C_SWZ_W);
    // Get output
    // Output register
    pInstruction->Output.OutputType = (VSH_OUTPUT_TYPE)VshGetField(pShaderToken, FLD_OUT_ORB);
    switch(pInstruction->Output.OutputType)
    {
    case OUTPUT_C:
        pInstruction->Output.OutputAddress = ConvertCRegister(VshGetField(pShaderToken, FLD_OUT_ADDRESS));
        break;
    case OUTPUT_O:
        pInstruction->Output.OutputAddress = VshGetField(pShaderToken, FLD_OUT_ADDRESS) & 0xF;
        break;
    }
    pInstruction->Output.OutputMux = (VSH_OUTPUT_MUX)VshGetField(pShaderToken, FLD_OUT_MUX);
    pInstruction->Output.OutputMask[0] = VshGetField(pShaderToken, FLD_OUT_O_MASK_X);
    pInstruction->Output.OutputMask[1] = VshGetField(pShaderToken, FLD_OUT_O_MASK_Y);
    pInstruction->Output.OutputMask[2] = VshGetField(pShaderToken, FLD_OUT_O_MASK_Z);
    pInstruction->Output.OutputMask[3] = VshGetField(pShaderToken, FLD_OUT_O_MASK_W);
    // MAC output
    pInstruction->Output.MACRMask[0] = VshGetField(pShaderToken, FLD_OUT_MAC_MASK_X);
    pInstruction->Output.MACRMask[1] = VshGetField(pShaderToken, FLD_OUT_MAC_MASK_Y);
    pInstruction->Output.MACRMask[2] = VshGetField(pShaderToken, FLD_OUT_MAC_MASK_Z);
    pInstruction->Output.MACRMask[3] = VshGetField(pShaderToken, FLD_OUT_MAC_MASK_W);
    pInstruction->Output.MACRAddress = VshGetField(pShaderToken, FLD_OUT_R);
    // ILU output
    pInstruction->Output.ILURMask[0] = VshGetField(pShaderToken, FLD_OUT_ILU_MASK_X);
    pInstruction->Output.ILURMask[1] = VshGetField(pShaderToken, FLD_OUT_ILU_MASK_Y);
    pInstruction->Output.ILURMask[2] = VshGetField(pShaderToken, FLD_OUT_ILU_MASK_Z);
    pInstruction->Output.ILURMask[3] = VshGetField(pShaderToken, FLD_OUT_ILU_MASK_W);
    pInstruction->Output.ILURAddress = VshGetField(pShaderToken, FLD_OUT_R);
    // Finally, get a0.x indirect constant addressing
    pInstruction->a0x = VshGetField(pShaderToken, FLD_A0X);
}

// Print functions
static char *VshGetRegisterName(VSH_PARAMETER_TYPE ParameterType)
{
    switch(ParameterType)
    {
    case PARAM_R:
        return "r";
    case PARAM_V:
        return "v";
    case PARAM_C:
        return "c";
	case PARAM_O:
		return "oPos";
	default:
        return "?";
    }
}

static void VshWriteOutputMask(boolean *OutputMask,
                               std::stringstream& pDisassembly)
{
    if(OutputMask[0] && OutputMask[1] && OutputMask[2] && OutputMask[3])
    {
        // All components are there, no need to print the mask
        return;
    }
	pDisassembly << "." << (OutputMask[0] ? "x" : "")
						<< (OutputMask[1] ? "y" : "")
						<< (OutputMask[2] ? "z" : "")
						<< (OutputMask[3] ? "w" : "");
}

static void VshWriteParameter(VSH_IMD_PARAMETER *pParameter,
                              std::stringstream& pDisassembly)
{
    pDisassembly << ", " << (pParameter->Parameter.Neg ? "-" : "") << VshGetRegisterName(pParameter->Parameter.ParameterType);
    if(pParameter->Parameter.ParameterType == PARAM_C && pParameter->IndexesWithA0_X)
    {
        // Only display the offset if it's not 0.
        if(pParameter->Parameter.Address)
        {
			pDisassembly << "[a0.x+" << pParameter->Parameter.Address << "]";
        }
        else
        {
			pDisassembly << "[a0.x]";
        }
    }
    else
    {
		pDisassembly << pParameter->Parameter.Address;
    }
    // Only bother printing the swizzle if it is not .xyzw
    if(!(pParameter->Parameter.Swizzle[0] == SWIZZLE_X &&
         pParameter->Parameter.Swizzle[1] == SWIZZLE_Y &&
         pParameter->Parameter.Swizzle[2] == SWIZZLE_Z &&
         pParameter->Parameter.Swizzle[3] == SWIZZLE_W))
    {
        int i;

		pDisassembly << ".";
        for (i = 0; i < 4; i++)
        {
            int j;
            char Swizzle = '?';
            switch(pParameter->Parameter.Swizzle[i])
            {
            case SWIZZLE_X:
                Swizzle = 'x';
                break;
            case SWIZZLE_Y:
                Swizzle = 'y';
                break;
            case SWIZZLE_Z:
                Swizzle = 'z';
                break;
            case SWIZZLE_W:
                Swizzle = 'w';
                break;
            }
			pDisassembly << Swizzle;
            for (j = i; j < 4; j++)
            {
                if(pParameter->Parameter.Swizzle[i] != pParameter->Parameter.Swizzle[j])
                {
                    break;
                }
            }
            if(j == 4)
            {
                break;
            }
        }
    }
}


char* XboxVertexRegisterAsString(DWORD VertexRegister)
{
	switch (VertexRegister)
	{
	case XTL::X_D3DVSDE_VERTEX: // -1
		return "D3DVSDE_VERTEX /* xbox ext. */";
	case XTL::X_D3DVSDE_POSITION: // 0
		return "D3DVSDE_POSITION";
	case XTL::X_D3DVSDE_BLENDWEIGHT: // 1
		return "D3DVSDE_BLENDWEIGHT";
	case XTL::X_D3DVSDE_NORMAL: // 2
		return "D3DVSDE_NORMAL";
	case XTL::X_D3DVSDE_DIFFUSE: // 3
		return "D3DVSDE_DIFFUSE";
	case XTL::X_D3DVSDE_SPECULAR: // 4
		return "D3DVSDE_SPECULAR";
	case XTL::X_D3DVSDE_FOG: // 5
		return "D3DVSDE_FOG";
	case XTL::X_D3DVSDE_POINTSIZE: // 6
		return "D3DVDSE_POINTSIZE";
	case XTL::X_D3DVSDE_BACKDIFFUSE: // 7
		return "D3DVSDE_BACKDIFFUSE /* xbox ext. */";
	case XTL::X_D3DVSDE_BACKSPECULAR: // 8
		return "D3DVSDE_BACKSPECULAR /* xbox ext. */";
	case XTL::X_D3DVSDE_TEXCOORD0: // 9
		return "D3DVSDE_TEXCOORD0";
	case XTL::X_D3DVSDE_TEXCOORD1: // 10
		return "D3DVSDE_TEXCOORD1";
	case XTL::X_D3DVSDE_TEXCOORD2: // 11
		return "D3DVSDE_TEXCOORD2";
	case XTL::X_D3DVSDE_TEXCOORD3: // 12
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

#define D3DDECLUSAGE_UNSUPPORTED ((D3DDECLUSAGE)-1)

D3DDECLUSAGE Xb2PCRegisterType
(
	DWORD VertexRegister,
	BYTE& PCUsageIndex
)
{
	D3DDECLUSAGE PCRegisterType;
	PCUsageIndex = 0;

	switch (VertexRegister)
	{
	case XTL::X_D3DVSDE_VERTEX: // -1
		PCRegisterType = D3DDECLUSAGE_UNSUPPORTED;
		break;
	case XTL::X_D3DVSDE_POSITION: // 0
		PCRegisterType = D3DDECLUSAGE_POSITION;
		break;
	case XTL::X_D3DVSDE_BLENDWEIGHT: // 1
		PCRegisterType = D3DDECLUSAGE_BLENDWEIGHT;
		break;
	case XTL::X_D3DVSDE_NORMAL: // 2
		PCRegisterType = D3DDECLUSAGE_NORMAL;
		break;
	case XTL::X_D3DVSDE_DIFFUSE: // 3
		PCRegisterType = D3DDECLUSAGE_COLOR; PCUsageIndex = 0;
		break;
	case XTL::X_D3DVSDE_SPECULAR: // 4
		PCRegisterType = D3DDECLUSAGE_COLOR; PCUsageIndex = 1;
		break;
	case XTL::X_D3DVSDE_FOG: // 5
		PCRegisterType = D3DDECLUSAGE_FOG;
		break;
	case XTL::X_D3DVSDE_POINTSIZE: // 6
		PCRegisterType = D3DDECLUSAGE_PSIZE;
		break;
	case XTL::X_D3DVSDE_BACKDIFFUSE: // 7
		PCRegisterType = D3DDECLUSAGE_COLOR; PCUsageIndex = 2;
		break;
	case XTL::X_D3DVSDE_BACKSPECULAR: // 8
		PCRegisterType = D3DDECLUSAGE_COLOR; PCUsageIndex = 3;
		break;
	case XTL::X_D3DVSDE_TEXCOORD0: // 9
		PCRegisterType = D3DDECLUSAGE_TEXCOORD; PCUsageIndex = 0;
		break;
	case XTL::X_D3DVSDE_TEXCOORD1: // 10
		PCRegisterType = D3DDECLUSAGE_TEXCOORD; PCUsageIndex = 1;
		break;
	case XTL::X_D3DVSDE_TEXCOORD2: // 11
		PCRegisterType = D3DDECLUSAGE_TEXCOORD; PCUsageIndex = 2;
		break;
	case XTL::X_D3DVSDE_TEXCOORD3: // 12
		PCRegisterType = D3DDECLUSAGE_TEXCOORD; PCUsageIndex = 3;
		break;
	default:
		PCRegisterType = D3DDECLUSAGE_UNSUPPORTED;
		break;
	}

	return PCRegisterType;
}

extern D3DCAPS g_D3DCaps;

enum {
	X_VSH_TEMPORARY_REGISTER_COUNT = 12, // For Xbox temporary registers r0 to r11, mapped one-on-one to host
	X_VSH_TEMP_OPOS = 12, // Used as intermediate storage for oPos (which Xbox can read through r12)
	// X_VSH_TEMP_OFOG, // Enable once we treat oFog similar to oPos
	// X_VSH_TEMP_OPTS, // Enable once we treat oPts similar to oPos
	X_VSH_TEMP_SCRATCH = 13, // Used as intermediate storage in Xbox-to-host opcode conversion
	X_VSH_TEMP_VERTEXREGBASE = 14 // Used for (1 up to 16) SetVertexData4f constants
};

static void VshWriteShader(VSH_XBOX_SHADER *pShader,
                           std::stringstream& pDisassembly,
						   D3DVERTEXELEMENT *pRecompiled,
                           boolean Truncate)
{
    switch(pShader->ShaderHeader.Version)
    {
        case VERSION_CXBX:
#ifdef CXBX_USE_VS30
			pDisassembly << "vs.3.0\n";
#else
			pDisassembly << "vs.2.x\n";
#endif
            break;
        case VERSION_XVS:
			pDisassembly << "xvs.1.1\n";
            break;
        case VERSION_XVSS:
			pDisassembly << "xvss.1.1\n";
            break;
        case VERSION_XVSW:
			pDisassembly << "xvsw.1.1\n";
            break;
        default:
            break;
    }

	// Ensure extra temporary registers are assigned at the beginning, as stand-ins for undeclared v registers
	// Abusing the truncate flag, which implies we're writing the final host shader
	if (Truncate) {
		std::stringstream moveConstantsToTemporaries;

		pDisassembly << "; Input usage declarations --\n";
		for(size_t i = 0; i < RegVIsUsedByShader.size(); i++){
			if (RegVIsUsedByShader[i]) {
				if (!RegVIsPresentInDeclaration[i]) {
					// Log test case and skip
					// Any registers hitting this critera were already replaced with constant/temporary reads
					// To correctly use the values given in SetVertexData4f.
					// We need to move these constant values to temporaries so they can be used as input alongside other constants!
					// We count down from the highest available on the host because Xbox titles don't use values that high, and we read from c192 (one above maximum Xbox c191 constant) and up
					moveConstantsToTemporaries << "mov r" << (X_VSH_TEMP_VERTEXREGBASE + i) << ", c" << (CXBX_D3DVS_CONSTREG_VERTEXDATA4F_BASE + i) << "\n";
					// test-case : Blade II (before menu's)
					// test-case : Namco Museum 50th Anniversary (at boot)
					// test-case : Pac-Man World 2 (at boot)
					// test-case : The Simpsons Road Rage (leaving menu's, before entering in-game)
					// test-case : The SpongeBob SquarePants Movie (before menu's)
					LOG_TEST_CASE("Shader uses undeclared Vertex Input Registers");
					continue;
				}

				// dcl_texcoord can be useds for any user-defined data
				// We need this because there is no reliable way to detect the real usage
				// Xbox has no concept of 'usage types', it only requires a list of attribute register numbers.
				// So we treat them all as 'user-defined'
				pDisassembly << "dcl_texcoord" << i << " v" << i << "\n";
			}
		}

		pDisassembly << moveConstantsToTemporaries.str();
	}

    for (int i = 0; i < pShader->IntermediateCount && (i < VSH_MAX_INSTRUCTION_COUNT || !Truncate); i++)
    {
        VSH_INTERMEDIATE_FORMAT *pIntermediate = &pShader->Intermediate[i];

        if(i == VSH_MAX_INSTRUCTION_COUNT)
        {
			pDisassembly << "; -- Passing the truncation limit --\n";
        }

        // Writing combining sign if neccessary
        if(pIntermediate->IsCombined)
        {
			pDisassembly << "+";
        }

        // Print the op code
        if(pIntermediate->InstructionType == IMD_MAC)
        {
			// Dxbx addition : Safeguard against incorrect MAC opcodes :
			if (pIntermediate->MAC > MAC_ARL)
				pDisassembly << "??? ";
			else
				pDisassembly << MAC_OpCode[pIntermediate->MAC] << " ";
        }
        else // IMD_ILU
        {
			// Dxbx addition : Safeguard against incorrect ILU opcodes :
			if (pIntermediate->ILU > ILU_LIT)
				pDisassembly << "??? ";
			else
				pDisassembly << ILU_OpCode[pIntermediate->ILU] << " ";
        }

        // Print the output parameter
        if(pIntermediate->Output.Type == IMD_OUTPUT_A0X)
        {
			pDisassembly << "a0.x";
        }
        else
        {
            switch(pIntermediate->Output.Type)
            {
            case IMD_OUTPUT_C:
				pDisassembly << "c" << pIntermediate->Output.Address;
                break;
            case IMD_OUTPUT_R:
				pDisassembly << "r" << pIntermediate->Output.Address;
                break;
            case IMD_OUTPUT_O:
				// Dxbx addition : Safeguard against incorrect VSH_OREG_NAME values :
				if ((int)pIntermediate->Output.Address > OREG_A0X)
					; // don't add anything
				else
					pDisassembly << OReg_Name[pIntermediate->Output.Address];
                break;
            default:
                CxbxKrnlCleanup("Invalid output register in vertex shader!");
                break;
            }
            VshWriteOutputMask(pIntermediate->Output.Mask, pDisassembly);
        }
        // Print the parameters
        for (int p = 0; p < 3; p++)
        {
            VSH_IMD_PARAMETER *pParameter = &pIntermediate->Parameters[p];
            if(pParameter->Active)
            {
                VshWriteParameter(pParameter, pDisassembly);
            }
        }
		pDisassembly << "\n";
    }
}

static void VshAddParameter(VSH_PARAMETER     *pParameter,
                            boolean           a0x,
                            VSH_IMD_PARAMETER *pIntermediateParameter)
{
    pIntermediateParameter->Parameter = *pParameter;
    pIntermediateParameter->Active    = TRUE;
    pIntermediateParameter->IndexesWithA0_X     = a0x;
}

static void VshAddParameters(VSH_SHADER_INSTRUCTION  *pInstruction,
                             VSH_ILU                 ILU,
                             VSH_MAC                 MAC,
                             VSH_IMD_PARAMETER       *pParameters)
{
    uint8_t ParamCount = 0;
    VSH_OPCODE_PARAMS* pParams = VshGetOpCodeParams(ILU, MAC);

    // param A
    if(pParams->A)
    {
        VshAddParameter(&pInstruction->A, pInstruction->a0x, &pParameters[ParamCount]);
        ParamCount++;
    }

    // param B
    if(pParams->B)
    {
        VshAddParameter(&pInstruction->B, pInstruction->a0x, &pParameters[ParamCount]);
        ParamCount++;
    }

    // param C
    if(pParams->C)
    {
        VshAddParameter(&pInstruction->C, pInstruction->a0x, &pParameters[ParamCount]);
        ParamCount++;
    }
}

static void VshVerifyBufferBounds(VSH_XBOX_SHADER *pShader)
{
    if(pShader->IntermediateCount >= VSH_MAX_INTERMEDIATE_COUNT)
    {
        CxbxKrnlCleanup("Shader exceeds conversion buffer!");
    }
}

static VSH_INTERMEDIATE_FORMAT *VshNewIntermediate(VSH_XBOX_SHADER *pShader)
{
    VshVerifyBufferBounds(pShader);

    ZeroMemory(&pShader->Intermediate[pShader->IntermediateCount], sizeof(VSH_INTERMEDIATE_FORMAT));

    return &pShader->Intermediate[pShader->IntermediateCount++];
}

static void VshInsertIntermediate(VSH_XBOX_SHADER         *pShader,
                                  VSH_INTERMEDIATE_FORMAT *pIntermediate,
                                  uint16_t                 Pos)
{
    VshVerifyBufferBounds(pShader);

    for (int i = pShader->IntermediateCount; i >= Pos; i--)
    {
        pShader->Intermediate[i + 1] = pShader->Intermediate[i];
    }
    pShader->Intermediate[Pos] = *pIntermediate;
    pShader->IntermediateCount++;
}

static void VshDeleteIntermediate(VSH_XBOX_SHADER *pShader,
                                  uint16_t         Pos)
{
    for (int i = Pos; i < (pShader->IntermediateCount - 1); i++)
    {
        pShader->Intermediate[i] = pShader->Intermediate[i + 1];
    }
    pShader->IntermediateCount--;
}

static boolean VshAddInstructionMAC_R(VSH_SHADER_INSTRUCTION *pInstruction,
                                      VSH_XBOX_SHADER        *pShader,
                                      boolean                IsCombined)
{
    VSH_INTERMEDIATE_FORMAT *pIntermediate;
    if(!HasMACR(pInstruction))
    {
        return FALSE;
    }

    pIntermediate = VshNewIntermediate(pShader);
    pIntermediate->IsCombined = IsCombined;

    // Opcode
    pIntermediate->InstructionType = IMD_MAC;
    pIntermediate->MAC = pInstruction->MAC;

    // Output param
    pIntermediate->Output.Type = IMD_OUTPUT_R;
    pIntermediate->Output.Address = pInstruction->Output.MACRAddress;
    memcpy(pIntermediate->Output.Mask, pInstruction->Output.MACRMask, sizeof(boolean) * 4);

    // Other parameters
    VshAddParameters(pInstruction, ILU_NOP, pInstruction->MAC, pIntermediate->Parameters);

    return TRUE;
}

static boolean VshAddInstructionMAC_O(VSH_SHADER_INSTRUCTION* pInstruction,
                                      VSH_XBOX_SHADER        *pShader,
                                      boolean                IsCombined)
{
    VSH_INTERMEDIATE_FORMAT *pIntermediate;
    if(!HasMACO(pInstruction))
    {
        return FALSE;
    }

    pIntermediate = VshNewIntermediate(pShader);
    pIntermediate->IsCombined = IsCombined;

    // Opcode
    pIntermediate->InstructionType = IMD_MAC;
    pIntermediate->MAC = pInstruction->MAC;

    // Output param
	pIntermediate->Output.Type = pInstruction->Output.OutputType == OUTPUT_C ? IMD_OUTPUT_C : IMD_OUTPUT_O;
    pIntermediate->Output.Address = pInstruction->Output.OutputAddress;
    memcpy(pIntermediate->Output.Mask, pInstruction->Output.OutputMask, sizeof(boolean) * 4);

    // Other parameters
    VshAddParameters(pInstruction, ILU_NOP, pInstruction->MAC, pIntermediate->Parameters);

    return TRUE;
}

static boolean VshAddInstructionMAC_ARL(VSH_SHADER_INSTRUCTION *pInstruction,
                                        VSH_XBOX_SHADER        *pShader,
                                        boolean                IsCombined)
{
    VSH_INTERMEDIATE_FORMAT *pIntermediate;
    if(!HasMACARL(pInstruction))
    {
        return FALSE;
    }

    pIntermediate = VshNewIntermediate(pShader);
    pIntermediate->IsCombined = IsCombined;

    // Opcode
    pIntermediate->InstructionType = IMD_MAC;
    pIntermediate->MAC = pInstruction->MAC;

    // Output param
    pIntermediate->Output.Type = IMD_OUTPUT_A0X;
    pIntermediate->Output.Address = pInstruction->Output.OutputAddress;

    // Other parameters
    VshAddParameters(pInstruction, ILU_NOP, pInstruction->MAC, pIntermediate->Parameters);

    return TRUE;
}

// Dxbx addition : Scalar instructions reading from W should read from X instead
static boolean DxbxFixupScalarParameter(VSH_SHADER_INSTRUCTION *pInstruction,
    VSH_XBOX_SHADER *pShader,
    VSH_PARAMETER *pParameter);

static boolean VshAddInstructionILU_R(VSH_SHADER_INSTRUCTION *pInstruction,
                                      VSH_XBOX_SHADER        *pShader,
                                      boolean                IsCombined)
{
    VSH_INTERMEDIATE_FORMAT *pIntermediate;
    if(!HasILUR(pInstruction))
    {
        return FALSE;
    }

	pIntermediate = VshNewIntermediate(pShader);
    pIntermediate->IsCombined = IsCombined;

    // Opcode
    pIntermediate->InstructionType = IMD_ILU;
    pIntermediate->ILU = pInstruction->ILU;

    // Output param
    pIntermediate->Output.Type = IMD_OUTPUT_R;
    // If this is a combined instruction, only r1 is allowed (R address should not be used)
    pIntermediate->Output.Address = IsCombined ? 1 : pInstruction->Output.ILURAddress;
    memcpy(pIntermediate->Output.Mask, pInstruction->Output.ILURMask, sizeof(boolean) * 4);

    // Other parameters
    VshAddParameters(pInstruction, pInstruction->ILU, MAC_NOP, pIntermediate->Parameters);

    return TRUE;
}

static boolean VshAddInstructionILU_O(VSH_SHADER_INSTRUCTION *pInstruction,
                                      VSH_XBOX_SHADER        *pShader,
                                      boolean                IsCombined)
{
    VSH_INTERMEDIATE_FORMAT *pIntermediate;
    if(!HasILUO(pInstruction))
    {
        return FALSE;
    }

    pIntermediate = VshNewIntermediate(pShader);
    pIntermediate->IsCombined = IsCombined;

    // Opcode
    pIntermediate->InstructionType = IMD_ILU;
    pIntermediate->ILU = pInstruction->ILU;

    // Output param
    pIntermediate->Output.Type = pInstruction->Output.OutputType == OUTPUT_C ? IMD_OUTPUT_C : IMD_OUTPUT_O;
    pIntermediate->Output.Address = pInstruction->Output.OutputAddress;
    memcpy(pIntermediate->Output.Mask, pInstruction->Output.OutputMask, sizeof(boolean) * 4);

    // Other parameters
    VshAddParameters(pInstruction, pInstruction->ILU, MAC_NOP, pIntermediate->Parameters);

    return TRUE;
}

static void VshConvertToIntermediate(VSH_SHADER_INSTRUCTION *pInstruction,
                                     VSH_XBOX_SHADER        *pShader)
{
    // Five types of instructions:
    //   MAC
    //
    //   ILU
    //
    //   MAC
    //   +ILU
    //
    //   MAC
    //   +MAC
    //   +ILU
    //
    //   MAC
    //   +ILU
    //   +ILU
    boolean IsCombined = FALSE;

    // Dxbx note : Scalar instructions read from C, but use X instead of W, fix that :
    DxbxFixupScalarParameter(pInstruction, pShader, &pInstruction->C);

    if(VshAddInstructionMAC_R(pInstruction, pShader, IsCombined))
    {
        if(HasMACO(pInstruction) ||
            HasILUR(pInstruction) ||
            HasILUO(pInstruction))
        {
            IsCombined = TRUE;
        }
    }
    if(VshAddInstructionMAC_O(pInstruction, pShader, IsCombined))
    {
        if(HasILUR(pInstruction) ||
            HasILUO(pInstruction))
        {
            IsCombined = TRUE;
        }
    }
    // Special case, arl (mov a0.x, ...)
    if(VshAddInstructionMAC_ARL(pInstruction, pShader, IsCombined))
    {
        if(HasILUR(pInstruction) ||
            HasILUO(pInstruction))
        {
            IsCombined = TRUE;
        }
    }
    if(VshAddInstructionILU_R(pInstruction, pShader, IsCombined))
    {
        if(HasILUO(pInstruction))
        {
            IsCombined = TRUE;
        }
    }
    (void)VshAddInstructionILU_O(pInstruction, pShader, IsCombined);
}

static inline void VshSetSwizzle(VSH_PARAMETER *pParameter,
    VSH_SWIZZLE       x,
    VSH_SWIZZLE       y,
    VSH_SWIZZLE       z,
    VSH_SWIZZLE       w)
{
    pParameter->Swizzle[0] = x;
    pParameter->Swizzle[1] = y;
    pParameter->Swizzle[2] = z;
    pParameter->Swizzle[3] = w;
}

static inline void VshSetSwizzle(VSH_IMD_PARAMETER *pParameter,
                                 VSH_SWIZZLE       x,
                                 VSH_SWIZZLE       y,
                                 VSH_SWIZZLE       z,
                                 VSH_SWIZZLE       w)
{
    VshSetSwizzle(&pParameter->Parameter, x, y, z, w);
}

static inline void VshSetOutputMask(VSH_IMD_OUTPUT* pOutput,
                                    boolean MaskX,
                                    boolean MaskY,
                                    boolean MaskZ,
                                    boolean MaskW)
{
    pOutput->Mask[0] = MaskX;
    pOutput->Mask[1] = MaskY;
    pOutput->Mask[2] = MaskZ;
    pOutput->Mask[3] = MaskW;
}

// Dxbx addition : Scalar instructions reading from W should read from X instead
static boolean DxbxFixupScalarParameter(VSH_SHADER_INSTRUCTION *pInstruction,
    VSH_XBOX_SHADER *pShader,
    VSH_PARAMETER *pParameter)
{
    boolean Result;

    // The DirectX vertex shader language specifies that the exp, log, rcc, rcp, and rsq instructions
    // all operate on the "w" component of the input. But the microcode versions of these instructions
    // actually operate on the "x" component of the input.
    Result = false;

    // Test if this is a scalar instruction :
    if (pInstruction->ILU == ILU_RCP ||
        pInstruction->ILU == ILU_RCC ||
        pInstruction->ILU == ILU_RSQ ||
        pInstruction->ILU == ILU_LOG)
    {
        // Test if this parameter reads all components, including W (TODO : Or should we fixup any W reading swizzle?) :
        if ((pParameter->Swizzle[0] == SWIZZLE_X)
            && (pParameter->Swizzle[1] == SWIZZLE_Y)
            && (pParameter->Swizzle[2] == SWIZZLE_Z)
            && (pParameter->Swizzle[3] == SWIZZLE_W))
        {
            // Change the read from W into a read from X (this fixes the XDK VolumeLight sample) :
            VshSetSwizzle(pParameter, SWIZZLE_X, SWIZZLE_X, SWIZZLE_X, SWIZZLE_X);
            DbgVshPrintf("Dxbx fixup on scalar instruction applied; Changed read of uninitialized W into a read of X!\n");
            Result = true;
        }
    }

    return Result;
}

/*
    mul oPos.xyz, r12, c-38
    +rcc r1.x, r12.w

    mad oPos.xyz, r12, r1.x, c-37
*/
static void VshRemoveScreenSpaceInstructions(VSH_XBOX_SHADER *pShader)
{
    int16_t PosC38    = -1;
    int deleted     = 0;

    for (int i = 0; i < pShader->IntermediateCount; i++)
    {
        VSH_INTERMEDIATE_FORMAT* pIntermediate = &pShader->Intermediate[i];

        for (int k = 0; k < 3; k++)
        {
            if(pIntermediate->Parameters[k].Active)
            {
                if(pIntermediate->Parameters[k].Parameter.ParameterType == PARAM_C &&
                   !pIntermediate->Parameters[k].IndexesWithA0_X)
                {
                    if(pIntermediate->Parameters[k].Parameter.Address == -37)
                    {
                        // Found c-37, remove the instruction
                        if(k == 2 &&
                           pIntermediate->Parameters[1].Active &&
                           pIntermediate->Parameters[1].Parameter.ParameterType == PARAM_R)
                        {
                            DbgVshPrintf("PosC38 = %d i = %d\n", PosC38, i);
                            for (int j = (i-1); j >= 0; j--)
                            {
                                VSH_INTERMEDIATE_FORMAT* pIntermediate1W = &pShader->Intermediate[j];
                                // Time to start searching for +rcc r#.x, r12.w
                                if(pIntermediate1W->InstructionType == IMD_ILU &&
                                    pIntermediate1W->ILU == ILU_RCC &&
                                    pIntermediate1W->Output.Type == IMD_OUTPUT_R &&
                                    pIntermediate1W->Output.Address ==
                                    pIntermediate->Parameters[1].Parameter.Address)
                                {
                                    DbgVshPrintf("Deleted +rcc r1.x, r12.w\n");
                                    VshDeleteIntermediate(pShader, j);
                                    deleted++;
                                    i--;
                                    //j--;
                                    break;
                                }
                            }
                        }
                        VshDeleteIntermediate(pShader, i);
                        deleted++;
                        i--;
                        DbgVshPrintf("Deleted mad oPos.xyz, r12, r1.x, c-37\n");
                        break;
                    }
                    else if(pIntermediate->Parameters[k].Parameter.Address == -38)
                    {
                        VshDeleteIntermediate(pShader, i);
                        PosC38 = i;
                        deleted++;
                        i--;
                        DbgVshPrintf("Deleted mul oPos.xyz, r12, c-38\n");
                    }
                }
            }
        }
    }

    // If we couldn't find the generic screen space transformation we're
    // assuming that the shader writes direct screen coordinates that must be
    // normalized. This hack will fail if (a) the shader uses custom screen
    // space transformation, (b) reads r13 or r12 after we have written to
    // them, or (c) doesn't reserve c-38 and c-37 for scale and offset.
    if(deleted != 3)
    {
        EmuLog(LOG_LEVEL::WARNING, "Applying screen space vertex shader patching hack!");
        for (int i = 0; i < pShader->IntermediateCount; i++)
        {
            VSH_INTERMEDIATE_FORMAT* pIntermediate = &pShader->Intermediate[i];

            // Find instructions outputting to oPos.
            if( pIntermediate->Output.Type    == IMD_OUTPUT_O &&
                pIntermediate->Output.Address == OREG_OPOS)
            {
                // Redirect output to r12.
                pIntermediate->Output.Type    = IMD_OUTPUT_R;
                pIntermediate->Output.Address = X_VSH_TEMP_OPOS;

                // Scale r12 to r13. (mul r13.[mask], r12, c58)
                VSH_INTERMEDIATE_FORMAT MulIntermediate;
                MulIntermediate.IsCombined        = FALSE;
                MulIntermediate.InstructionType   = IMD_MAC;
                MulIntermediate.MAC               = MAC_MUL;
                MulIntermediate.Output.Type       = IMD_OUTPUT_R;
                MulIntermediate.Output.Address    = X_VSH_TEMP_SCRATCH;
                MulIntermediate.Output.Mask[0]    = pIntermediate->Output.Mask[0];
                MulIntermediate.Output.Mask[1]    = pIntermediate->Output.Mask[1];
                MulIntermediate.Output.Mask[2]    = pIntermediate->Output.Mask[2];
                MulIntermediate.Output.Mask[3]    = pIntermediate->Output.Mask[3];
                MulIntermediate.Parameters[0].Active                  = TRUE;
                MulIntermediate.Parameters[0].IndexesWithA0_X                   = FALSE;
                MulIntermediate.Parameters[0].Parameter.ParameterType = PARAM_R;
                MulIntermediate.Parameters[0].Parameter.Address       = X_VSH_TEMP_OPOS;
                MulIntermediate.Parameters[0].Parameter.Neg           = FALSE;
                VshSetSwizzle(&MulIntermediate.Parameters[0], SWIZZLE_X, SWIZZLE_Y, SWIZZLE_Z, SWIZZLE_W);
                MulIntermediate.Parameters[1].Active                  = TRUE;
                MulIntermediate.Parameters[1].IndexesWithA0_X                   = FALSE;
                MulIntermediate.Parameters[1].Parameter.ParameterType = PARAM_C;
                MulIntermediate.Parameters[1].Parameter.Address       = ConvertCRegister(X_D3DSCM_RESERVED_CONSTANT_SCALE);
                MulIntermediate.Parameters[1].Parameter.Neg           = FALSE;
                VshSetSwizzle(&MulIntermediate.Parameters[1], SWIZZLE_X, SWIZZLE_Y, SWIZZLE_Z, SWIZZLE_W);
                MulIntermediate.Parameters[2].Active                  = FALSE;
                VshInsertIntermediate(pShader, &MulIntermediate, ++i);

                // Add offset with r13 to oPos (add oPos.[mask], r13, c59)
                VSH_INTERMEDIATE_FORMAT AddIntermediate = MulIntermediate;
                AddIntermediate.MAC               = MAC_ADD;
                AddIntermediate.Output.Type       = IMD_OUTPUT_O;
                AddIntermediate.Output.Address    = OREG_OPOS;
                AddIntermediate.Parameters[0].Parameter.ParameterType = PARAM_R;
                AddIntermediate.Parameters[0].Parameter.Address       = X_VSH_TEMP_SCRATCH;
                AddIntermediate.Parameters[1].Parameter.Address       = ConvertCRegister(X_D3DSCM_RESERVED_CONSTANT_OFFSET);
                VshInsertIntermediate(pShader, &AddIntermediate, ++i);
            }
        }
    }
}

static void VshRemoveUnsupportedObRegisters(VSH_XBOX_SHADER *pShader)
{
	int deleted = 0;

	for (int i = 0; i < pShader->IntermediateCount; i++) {
		VSH_INTERMEDIATE_FORMAT* pIntermediate = &pShader->Intermediate[i];

		if (pIntermediate->Output.Type == IMD_OUTPUT_O && (pIntermediate->Output.Address == OREG_OB0 || pIntermediate->Output.Address == OREG_OB1)) {
			DbgVshPrintf("Deleted unsupported write to %s\n", OReg_Name[pIntermediate->Output.Address]);
			VshDeleteIntermediate(pShader, i);
			i--;
		}
	}
}

// Converts the intermediate format vertex shader to DirectX 8/9 format
static boolean VshConvertShader(VSH_XBOX_SHADER *pShader,
                                boolean         bNoReservedConstants
)
{
    // TODO: What about state shaders and such?

    pShader->ShaderHeader.Version = VERSION_CXBX;

    // Search for the screen space instructions, and remove them
    if(!bNoReservedConstants)
    {
        VshRemoveScreenSpaceInstructions(pShader);
    }

	// Windows does not support back-facing colours, so we remove them from the shaders
	// Test Case: Panzer Dragoon Orta
	VshRemoveUnsupportedObRegisters(pShader);

    // TODO: Add routine for compacting r register usage so that at least one is freed (two if dph and r12)

    for (int i = 0; i < pShader->IntermediateCount; i++)
    {
        VSH_INTERMEDIATE_FORMAT* pIntermediate = &pShader->Intermediate[i];
        // Combining not supported in vs.1.1
        pIntermediate->IsCombined = FALSE;

        if(pIntermediate->Output.Type == IMD_OUTPUT_O && (pIntermediate->Output.Address == OREG_OPTS || pIntermediate->Output.Address == OREG_OFOG))
        {
            // The PC shader assembler doesn't like masks on scalar registers
			VshSetOutputMask(&pIntermediate->Output, TRUE, TRUE, TRUE, TRUE);

			// Fix when mad or mov to a scaler input does not use a replicate swizzle
			// MAD Test case: Panzer Dragoon Orta
			// MOV Test case: DOA3, Mechassault (Const)
			// MUL Test case: Amped
			// TODO Previously we applied this fix for specified instructions
			// When should we not apply the correction?
			if (true)
			{
				// Clear all but the first swizzle for each parameter
				// TODO: Is this sufficient? Perhaps we need to be smart about which swizzle to select
				for (int param = 0; param < 3; param++) {
					pIntermediate->Parameters[param].Parameter.Swizzle[1] = pIntermediate->Parameters[param].Parameter.Swizzle[0];
					pIntermediate->Parameters[param].Parameter.Swizzle[2] = pIntermediate->Parameters[param].Parameter.Swizzle[0];
					pIntermediate->Parameters[param].Parameter.Swizzle[3] = pIntermediate->Parameters[param].Parameter.Swizzle[0];
				}
			}
        }

        if(pIntermediate->InstructionType == IMD_ILU && pIntermediate->ILU == ILU_RCC)
        {
            // Convert rcc to rcp
            pIntermediate->ILU = ILU_RCP;
        }

        auto sw = pIntermediate->Parameters[0].Parameter.Swizzle;
        bool singleSwizzle = sw[0] == sw[1] && sw[1] == sw[2] && sw[2] == sw[3];

        if (!singleSwizzle)
        {
            // Fix when RSQ reads from unitialized components
            if (pIntermediate->InstructionType == IMD_ILU && pIntermediate->ILU == ILU_RSQ) {
                int swizzle = (pIntermediate->Output.Mask[0]) | (pIntermediate->Output.Mask[1] << 1) | (pIntermediate->Output.Mask[2] << 2) | (pIntermediate->Output.Mask[3] << 3);
                switch (swizzle)
                {
                case 1:
                    VshSetSwizzle(&pIntermediate->Parameters[0], SWIZZLE_X, SWIZZLE_X, SWIZZLE_X, SWIZZLE_X);
                    break;
                case 2:
                    VshSetSwizzle(&pIntermediate->Parameters[0], SWIZZLE_Y, SWIZZLE_Y, SWIZZLE_Y, SWIZZLE_Y);
                    break;
                case 4:
                    VshSetSwizzle(&pIntermediate->Parameters[0], SWIZZLE_Z, SWIZZLE_Z, SWIZZLE_Z, SWIZZLE_Z);
                    break;
                case 8:
                    VshSetSwizzle(&pIntermediate->Parameters[0], SWIZZLE_W, SWIZZLE_W, SWIZZLE_W, SWIZZLE_W);
                    break;
                case 15:
                default:
                    LOG_TEST_CASE("rsq instruction with invalid swizzle");
                    break;
                }
            }
        }

		for (int j = 0; j < 3; j++)
		{
			//if(pIntermediate->Parameters[j].Active)
			{
				// Make constant registers range from 0 to 191 instead of -96 to 95
				if (pIntermediate->Parameters[j].Parameter.ParameterType == PARAM_C)
				{
					//if(pIntermediate->Parameters[j].Parameter.Address < 0)
					pIntermediate->Parameters[j].Parameter.Address += X_D3DSCM_CORRECTION;
				}

				if (pIntermediate->Parameters[j].Parameter.ParameterType == PARAM_V) {
					RegVIsUsedByShader[pIntermediate->Parameters[j].Parameter.Address] = TRUE;

					if (!RegVIsPresentInDeclaration[pIntermediate->Parameters[j].Parameter.Address]) {
						// This vertex register was not declared and therefore is not present within the Vertex Data object
						// We read from temporary registers instead, that are set based on constants, in-turn, set by SetVertexData4f
						// We count down from the highest available on the host because Xbox titles don't use values that high, and we read from c192 (one above maximum Xbox c191 constant) and up
						pIntermediate->Parameters[j].Parameter.ParameterType = PARAM_R;
						pIntermediate->Parameters[j].Parameter.Address += X_VSH_TEMP_VERTEXREGBASE;
					} 
				}
			}
		}

        // Make constant registers range from 0 to 191 instead of -96 to 95
        if(pIntermediate->Output.Type == IMD_OUTPUT_C)
        {
			//if(pIntermediate->Output.Address < 0)
				pIntermediate->Output.Address += X_D3DSCM_CORRECTION;
        }



        if(pIntermediate->InstructionType == IMD_MAC && pIntermediate->MAC == MAC_DPH)
        {
			// 2010/01/12 - revel8n - attempt to alleviate conversion issues relate to the dph instruction

            // Replace dph with dp3 and add
            if(pIntermediate->Output.Type != IMD_OUTPUT_R)
            {
                // TODO: Complete dph support
                EmuLog(LOG_LEVEL::WARNING, "Can't simulate dph for other than output r registers (yet)");

				VSH_INTERMEDIATE_FORMAT TmpIntermediate = *pIntermediate;

				// modify the instructions
				pIntermediate->MAC = MAC_DP3;
				pIntermediate->Output.Type = IMD_OUTPUT_R;
				pIntermediate->Output.Address = X_VSH_TEMP_SCRATCH;
				VshSetOutputMask(&pIntermediate->Output, TRUE, TRUE, TRUE, TRUE);

				TmpIntermediate.MAC = MAC_ADD;
				TmpIntermediate.Parameters[0].IndexesWithA0_X = FALSE;
				TmpIntermediate.Parameters[0].Parameter.ParameterType = PARAM_R;
				TmpIntermediate.Parameters[0].Parameter.Address = X_VSH_TEMP_SCRATCH;
				TmpIntermediate.Parameters[0].Parameter.Neg = FALSE;
				VshSetSwizzle(&TmpIntermediate.Parameters[1], SWIZZLE_W, SWIZZLE_W, SWIZZLE_W, SWIZZLE_W);
				// Is this output register a scalar
				if (TmpIntermediate.Output.Type == IMD_OUTPUT_O) {
					if ((TmpIntermediate.Output.Address == OREG_OFOG) || (TmpIntermediate.Output.Address == OREG_OPTS)) {
						// This fixes test case "Namco Museum 50th Anniversary"
						// The PC shader assembler doesn't like masks on scalar registers
						VshSetOutputMask(&TmpIntermediate.Output, TRUE, TRUE, TRUE, TRUE);
						// Make the first source parameter use the w swizzle too
						VshSetSwizzle(&TmpIntermediate.Parameters[0], SWIZZLE_W, SWIZZLE_W, SWIZZLE_W, SWIZZLE_W);
					}
				}

				VshInsertIntermediate(pShader, &TmpIntermediate, i + 1);
            }
			else
			{
				VSH_INTERMEDIATE_FORMAT TmpIntermediate = *pIntermediate;
				pIntermediate->MAC = MAC_DP3;
				TmpIntermediate.MAC = MAC_ADD;
				TmpIntermediate.Parameters[0].IndexesWithA0_X = FALSE;
				TmpIntermediate.Parameters[0].Parameter.ParameterType = PARAM_R;
				TmpIntermediate.Parameters[0].Parameter.Address = TmpIntermediate.Output.Address;
				TmpIntermediate.Parameters[0].Parameter.Neg = FALSE;

				int swizzle = (TmpIntermediate.Output.Mask[0]) | (TmpIntermediate.Output.Mask[1] << 1) | (TmpIntermediate.Output.Mask[2] << 2) | (TmpIntermediate.Output.Mask[3] << 3);
				switch (swizzle)
				{
				case 1:
					VshSetSwizzle(&TmpIntermediate.Parameters[0], SWIZZLE_X, SWIZZLE_X, SWIZZLE_X, SWIZZLE_X);
					break;
				case 2:
					VshSetSwizzle(&TmpIntermediate.Parameters[0], SWIZZLE_Y, SWIZZLE_Y, SWIZZLE_Y, SWIZZLE_Y);
					break;
				case 4:
					VshSetSwizzle(&TmpIntermediate.Parameters[0], SWIZZLE_Z, SWIZZLE_Z, SWIZZLE_Z, SWIZZLE_Z);
					break;
				case 8:
					VshSetSwizzle(&TmpIntermediate.Parameters[0], SWIZZLE_W, SWIZZLE_W, SWIZZLE_W, SWIZZLE_W);
					break;
				case 15:
				default:
					VshSetSwizzle(&TmpIntermediate.Parameters[0], SWIZZLE_X, SWIZZLE_Y, SWIZZLE_Z, SWIZZLE_W);
					break;
				}
				//VshSetSwizzle(&TmpIntermediate.Parameters[0], SWIZZLE_W, SWIZZLE_W, SWIZZLE_W, SWIZZLE_W);
				VshSetSwizzle(&TmpIntermediate.Parameters[1], SWIZZLE_W, SWIZZLE_W, SWIZZLE_W, SWIZZLE_W);
				//VshSetOutputMask(&TmpIntermediate.Output, FALSE, FALSE, FALSE, TRUE);
				VshInsertIntermediate(pShader, &TmpIntermediate, i + 1);
			}
            i++;
        }
    }

    // Replace all writes to oPos with writes to r12.
	// On Xbox, oPos is read/write, essentially a 13th temporary register
	// In DX9 and vs_2_x, oPos is write-only, so we'll use r12 in its place
	// And at the end of the shader, write r12 to oPos
    for (int i = 0; i < pShader->IntermediateCount; i++) {
        VSH_INTERMEDIATE_FORMAT* pIntermediate = &pShader->Intermediate[i];
        if (pIntermediate->Output.Type == IMD_OUTPUT_O && pIntermediate->Output.Address == OREG_OPOS) {
            pIntermediate->Output.Type = IMD_OUTPUT_R;
            pIntermediate->Output.Address = X_VSH_TEMP_OPOS;
        }
    }

	// We append one additional instruction to mov oPos, r12
	// TODO : *IF* r12 is not read after the final write to oPos,
	// it'd be more efficient to not-replace this oPos write by r12,
	// so that we don't have to do the following :
    VSH_INTERMEDIATE_FORMAT MovIntermediate = {0};
    MovIntermediate.MAC = MAC_MOV;
    MovIntermediate.Output.Type = IMD_OUTPUT_O;
    MovIntermediate.Output.Address = OREG_OPOS;
    MovIntermediate.Output.Mask[0] = true;
    MovIntermediate.Output.Mask[1] = true;
    MovIntermediate.Output.Mask[2] = true;
    MovIntermediate.Output.Mask[3] = true;
    MovIntermediate.Parameters[0].Active = true;
    MovIntermediate.Parameters[0].Parameter.ParameterType = PARAM_R;
    MovIntermediate.Parameters[0].Parameter.Address = X_VSH_TEMP_OPOS;
    MovIntermediate.Parameters[0].Parameter.Swizzle[0] = SWIZZLE_X;
    MovIntermediate.Parameters[0].Parameter.Swizzle[1] = SWIZZLE_Y;
    MovIntermediate.Parameters[0].Parameter.Swizzle[2] = SWIZZLE_Z;
    MovIntermediate.Parameters[0].Parameter.Swizzle[3] = SWIZZLE_W;
    VshInsertIntermediate(pShader, &MovIntermediate, pShader->IntermediateCount);

    return TRUE;
}

// ****************************************************************************
// * Vertex shader declaration recompiler
// ****************************************************************************

class XboxVertexDeclarationConverter
{
protected:
	// Internal variables
	CxbxVertexShaderInfo* pVertexShaderInfoToSet;
	CxbxVertexShaderStreamInfo* pCurrentVertexShaderStreamInfo = nullptr;
	DWORD hostTemporaryRegisterCount;
	bool IsFixedFunction;
	D3DVERTEXELEMENT* pRecompiled;

public:
	// Output
	DWORD XboxDeclarationCount;
	DWORD HostDeclarationSize;

private:
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

	inline DWORD VshGetVertexRegister(DWORD XboxToken)
	{
		DWORD regNum = (XboxToken & X_D3DVSD_VERTEXREGMASK) >> X_D3DVSD_VERTEXREGSHIFT;
		if (regNum >= hostTemporaryRegisterCount /*12 for D3D8, D3D9 value depends on host GPU */) {
			// test-case : BLiNX: the time sweeper
			// test-case : Lego Star Wars
			LOG_TEST_CASE("RegNum > NumTemps");
		}
		return regNum;
	}

	inline DWORD VshGetVertexRegisterIn(DWORD XboxToken)
	{
		DWORD regNum = (XboxToken & X_D3DVSD_VERTEXREGINMASK) >> X_D3DVSD_VERTEXREGINSHIFT;
		if (regNum >= hostTemporaryRegisterCount /*12 for D3D8, D3D9 value depends on host GPU */) {
			// test-case : Lego Star Wars
			LOG_TEST_CASE("RegNum > NumTemps");
		}
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
			case XTL::X_D3DVSD_TOKEN_NOP: {
				DbgVshPrintf("\tD3DVSD_NOP(),\n");
				break;
			}
			case XTL::X_D3DVSD_TOKEN_STREAM: {
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
			case XTL::X_D3DVSD_TOKEN_STREAMDATA: {
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
					case XTL::X_D3DVSDT_FLOAT1: // 0x12:
						DbgVshPrintf("D3DVSDT_FLOAT1");
						break;
					case XTL::X_D3DVSDT_FLOAT2: // 0x22:
						DbgVshPrintf("D3DVSDT_FLOAT2");
						break;
					case XTL::X_D3DVSDT_FLOAT3: // 0x32:
						DbgVshPrintf("D3DVSDT_FLOAT3");
						break;
					case XTL::X_D3DVSDT_FLOAT4: // 0x42:
						DbgVshPrintf("D3DVSDT_FLOAT4");
						break;
					case XTL::X_D3DVSDT_D3DCOLOR: // 0x40:
						DbgVshPrintf("D3DVSDT_D3DCOLOR");
						break;
					case XTL::X_D3DVSDT_SHORT2: // 0x25:
						DbgVshPrintf("D3DVSDT_SHORT2");
						break;
					case XTL::X_D3DVSDT_SHORT4: // 0x45:
						DbgVshPrintf("D3DVSDT_SHORT4");
						break;
					case XTL::X_D3DVSDT_NORMSHORT1: // 0x11:
						DbgVshPrintf("D3DVSDT_NORMSHORT1 /* xbox ext. */");
						bStreamNeedsPatching = true;
						break;
					case XTL::X_D3DVSDT_NORMSHORT2: // 0x21:
						if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT2N) {
							DbgVshPrintf("D3DVSDT_NORMSHORT2");
						} else {
							DbgVshPrintf("D3DVSDT_NORMSHORT2 /* xbox ext. */");
							bStreamNeedsPatching = true;
						}
						break;
					case XTL::X_D3DVSDT_NORMSHORT3: // 0x31:
						DbgVshPrintf("D3DVSDT_NORMSHORT3 /* xbox ext. */");
						bStreamNeedsPatching = true;
						break;
					case XTL::X_D3DVSDT_NORMSHORT4: // 0x41:
						if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT4N) {
							DbgVshPrintf("D3DVSDT_NORMSHORT4");
							// No need for patching in D3D9
						} else {
							DbgVshPrintf("D3DVSDT_NORMSHORT4 /* xbox ext. */");
							bStreamNeedsPatching = true;
						}
						break;
					case XTL::X_D3DVSDT_NORMPACKED3: // 0x16:
						DbgVshPrintf("D3DVSDT_NORMPACKED3 /* xbox ext. */");
						bStreamNeedsPatching = true;
						break;
					case XTL::X_D3DVSDT_SHORT1: // 0x15:
						DbgVshPrintf("D3DVSDT_SHORT1 /* xbox ext. */");
						bStreamNeedsPatching = true;
						break;
					case XTL::X_D3DVSDT_SHORT3: // 0x35:
						DbgVshPrintf("D3DVSDT_SHORT3 /* xbox ext. */");
						bStreamNeedsPatching = true;
						break;
					case XTL::X_D3DVSDT_PBYTE1: // 0x14:
						DbgVshPrintf("D3DVSDT_PBYTE1 /* xbox ext. */");
						bStreamNeedsPatching = true;
						break;
					case XTL::X_D3DVSDT_PBYTE2: // 0x24:
						DbgVshPrintf("D3DVSDT_PBYTE2 /* xbox ext. */");
						bStreamNeedsPatching = true;
						break;
					case XTL::X_D3DVSDT_PBYTE3: // 0x34:
						DbgVshPrintf("D3DVSDT_PBYTE3 /* xbox ext. */");
						bStreamNeedsPatching = true;
						break;
					case XTL::X_D3DVSDT_PBYTE4: // 0x44:
						if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
							DbgVshPrintf("D3DVSDT_PBYTE4");
						} else {
							DbgVshPrintf("D3DVSDT_PBYTE4 /* xbox ext. */");
							bStreamNeedsPatching = true;
						}
						break;
					case XTL::X_D3DVSDT_FLOAT2H: // 0x72:
						DbgVshPrintf("D3DVSDT_FLOAT2H /* xbox ext. */");
						bStreamNeedsPatching = true;
						break;
					case XTL::X_D3DVSDT_NONE: // 0x02:
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
			case XTL::X_D3DVSD_TOKEN_TESSELLATOR: {
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
			case XTL::X_D3DVSD_TOKEN_CONSTMEM: {
				DWORD ConstantAddress = (*pXboxToken & X_D3DVSD_CONSTADDRESSMASK) >> X_D3DVSD_CONSTADDRESSSHIFT;
				DWORD Count = (*pXboxToken & X_D3DVSD_CONSTCOUNTMASK) >> X_D3DVSD_CONSTCOUNTSHIFT;
				DbgVshPrintf("\tD3DVSD_CONST(%d, %d),\n", ConstantAddress, Count);
				LOG_TEST_CASE("X_D3DVSD_TOKEN_CONSTMEM");
				Step = Count * 4 + 1;
				break;
			}
			case XTL::X_D3DVSD_TOKEN_EXT: {
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

	void VshConvertToken_NOP(DWORD *pXboxToken)
	{
		if(*pXboxToken != X_D3DVSD_NOP())
		{
			LOG_TEST_CASE("Token NOP found, but extra parameters are given!");
		}
	}

	DWORD VshConvertToken_CONSTMEM(DWORD *pXboxToken)
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
			pCurrentVertexShaderStreamInfo = &(pVertexShaderInfoToSet->VertexStreams[StreamNumber]);
			pCurrentVertexShaderStreamInfo->NeedPatch = FALSE;
			pCurrentVertexShaderStreamInfo->DeclPosition = FALSE;
			pCurrentVertexShaderStreamInfo->CurrentStreamNumber = 0;
			pCurrentVertexShaderStreamInfo->HostVertexStride = 0;
			pCurrentVertexShaderStreamInfo->NumberOfVertexElements = 0;

			// Dxbx note : Use Dophin(s), FieldRender, MatrixPaletteSkinning and PersistDisplay as a testcase

			pCurrentVertexShaderStreamInfo->CurrentStreamNumber = VshGetVertexStream(*pXboxToken);
			pVertexShaderInfoToSet->NumberOfVertexStreams++;
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
		VshConvert_RegisterVertexElement(XTL::X_D3DVSDT_NONE, SkipBytesCount, /*HostSize=*/0, /*NeedPatching=*/TRUE);
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
		case XTL::X_D3DVSDT_FLOAT1: // 0x12:
			HostVertexElementDataType = D3DDECLTYPE_FLOAT1;
			HostVertexElementByteSize = 1 * sizeof(FLOAT);
			break;
		case XTL::X_D3DVSDT_FLOAT2: // 0x22:
			HostVertexElementDataType = D3DDECLTYPE_FLOAT2;
			HostVertexElementByteSize = 2 * sizeof(FLOAT);
			break;
		case XTL::X_D3DVSDT_FLOAT3: // 0x32:
			HostVertexElementDataType = D3DDECLTYPE_FLOAT3;
			HostVertexElementByteSize = 3 * sizeof(FLOAT);
			break;
		case XTL::X_D3DVSDT_FLOAT4: // 0x42:
			HostVertexElementDataType = D3DDECLTYPE_FLOAT4;
			HostVertexElementByteSize = 4 * sizeof(FLOAT);
			break;
		case XTL::X_D3DVSDT_D3DCOLOR: // 0x40:
			HostVertexElementDataType = D3DDECLTYPE_D3DCOLOR;
			HostVertexElementByteSize = 1 * sizeof(D3DCOLOR);
			break;
		case XTL::X_D3DVSDT_SHORT2: // 0x25:
			HostVertexElementDataType = D3DDECLTYPE_SHORT2;
			HostVertexElementByteSize = 2 * sizeof(SHORT);
			break;
		case XTL::X_D3DVSDT_SHORT4: // 0x45:
			HostVertexElementDataType = D3DDECLTYPE_SHORT4;
			HostVertexElementByteSize = 4 * sizeof(SHORT);
			break;
		case XTL::X_D3DVSDT_NORMSHORT1: // 0x11:
			if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT2N) {
				HostVertexElementDataType = D3DDECLTYPE_SHORT2N;
				HostVertexElementByteSize = 2 * sizeof(SHORT);
			}
			else
			{
				HostVertexElementDataType = D3DDECLTYPE_FLOAT1;
				HostVertexElementByteSize = 1 * sizeof(FLOAT);
			}
			XboxVertexElementByteSize = 1 * sizeof(XTL::SHORT);
			NeedPatching = TRUE;
			break;
		case XTL::X_D3DVSDT_NORMSHORT2: // 0x21:
			if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT2N) {
				HostVertexElementDataType = D3DDECLTYPE_SHORT2N;
				HostVertexElementByteSize = 2 * sizeof(SHORT);
				// No need for patching in D3D9
			}
			else
			{
				HostVertexElementDataType = D3DDECLTYPE_FLOAT2;
				HostVertexElementByteSize = 2 * sizeof(FLOAT);
				XboxVertexElementByteSize = 2 * sizeof(XTL::SHORT);
				NeedPatching = TRUE;
			}
			break;
		case XTL::X_D3DVSDT_NORMSHORT3: // 0x31:
			if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT4N) {
				HostVertexElementDataType = D3DDECLTYPE_SHORT4N;
				HostVertexElementByteSize = 4 * sizeof(SHORT);
			}
			else
			{
				HostVertexElementDataType = D3DDECLTYPE_FLOAT3;
				HostVertexElementByteSize = 3 * sizeof(FLOAT);
			}
			XboxVertexElementByteSize = 3 * sizeof(XTL::SHORT);
			NeedPatching = TRUE;
			break;
		case XTL::X_D3DVSDT_NORMSHORT4: // 0x41:
			if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT4N) {
				HostVertexElementDataType = D3DDECLTYPE_SHORT4N;
				HostVertexElementByteSize = 4 * sizeof(SHORT);
				// No need for patching in D3D9
			}
			else
			{
				HostVertexElementDataType = D3DDECLTYPE_FLOAT4;
				HostVertexElementByteSize = 4 * sizeof(FLOAT);
				XboxVertexElementByteSize = 4 * sizeof(XTL::SHORT);
				NeedPatching = TRUE;
			}
			break;
		case XTL::X_D3DVSDT_NORMPACKED3: // 0x16:
			HostVertexElementDataType = D3DDECLTYPE_FLOAT3;
			HostVertexElementByteSize = 3 * sizeof(FLOAT);
			XboxVertexElementByteSize = 1 * sizeof(XTL::DWORD);
			NeedPatching = TRUE;
			break;
		case XTL::X_D3DVSDT_SHORT1: // 0x15:
			HostVertexElementDataType = D3DDECLTYPE_SHORT2;
			HostVertexElementByteSize = 2 * sizeof(SHORT);
			XboxVertexElementByteSize = 1 * sizeof(XTL::SHORT);
			NeedPatching = TRUE;
			break;
		case XTL::X_D3DVSDT_SHORT3: // 0x35:
			HostVertexElementDataType = D3DDECLTYPE_SHORT4;
			HostVertexElementByteSize = 4 * sizeof(SHORT);
			XboxVertexElementByteSize = 3 * sizeof(XTL::SHORT);
			NeedPatching = TRUE;
			break;
		case XTL::X_D3DVSDT_PBYTE1: // 0x14:
			if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
				HostVertexElementDataType = D3DDECLTYPE_UBYTE4N;
				HostVertexElementByteSize = 4 * sizeof(BYTE);
			}
			else
			{
				HostVertexElementDataType = D3DDECLTYPE_FLOAT1;
				HostVertexElementByteSize = 1 * sizeof(FLOAT);
			}
			XboxVertexElementByteSize = 1 * sizeof(XTL::BYTE);
			NeedPatching = TRUE;
			break;
		case XTL::X_D3DVSDT_PBYTE2: // 0x24:
			if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
				HostVertexElementDataType = D3DDECLTYPE_UBYTE4N;
				HostVertexElementByteSize = 4 * sizeof(BYTE);
			}
			else
			{
				HostVertexElementDataType = D3DDECLTYPE_FLOAT2;
				HostVertexElementByteSize = 2 * sizeof(FLOAT);
			}
			XboxVertexElementByteSize = 2 * sizeof(XTL::BYTE);
			NeedPatching = TRUE;
			break;
		case XTL::X_D3DVSDT_PBYTE3: // 0x34:
			if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
				HostVertexElementDataType = D3DDECLTYPE_UBYTE4N;
				HostVertexElementByteSize = 4 * sizeof(BYTE);
			}
			else
			{
				HostVertexElementDataType = D3DDECLTYPE_FLOAT3;
				HostVertexElementByteSize = 3 * sizeof(FLOAT);
			}
			XboxVertexElementByteSize = 3 * sizeof(XTL::BYTE);
			NeedPatching = TRUE;
			break;
		case XTL::X_D3DVSDT_PBYTE4: // 0x44:
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
				XboxVertexElementByteSize = 4 * sizeof(XTL::BYTE);
				NeedPatching = TRUE;
			}
			break;
		case XTL::X_D3DVSDT_FLOAT2H: // 0x72:
			HostVertexElementDataType = D3DDECLTYPE_FLOAT4;
			HostVertexElementByteSize = 4 * sizeof(FLOAT);
			XboxVertexElementByteSize = 3 * sizeof(FLOAT);
			NeedPatching = TRUE;
			break;
		case XTL::X_D3DVSDT_NONE: // 0x02:
			// No host element data, so no patching
			break;
		default:
			//LOG_TEST_CASE("Unknown data type for D3DVSD_REG: 0x%02X\n", XboxVertexElementDataType);
			break;
		}

		// On X_D3DVSDT_NONE skip this token
		if (XboxVertexElementDataType == XTL::X_D3DVSDT_NONE)
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
		case XTL::X_D3DVSD_TOKEN_NOP:
			VshConvertToken_NOP(pXboxToken);
			break;
		case XTL::X_D3DVSD_TOKEN_STREAM:
		{
			VshConvertToken_STREAM(pXboxToken);
			break;
		}
		case XTL::X_D3DVSD_TOKEN_STREAMDATA:
		{
			VshConvertToken_STREAMDATA(pXboxToken);
			break;
		}
		case XTL::X_D3DVSD_TOKEN_TESSELLATOR:
		{
			VshConvertToken_TESSELATOR(pXboxToken);
			break;
		}
		case XTL::X_D3DVSD_TOKEN_CONSTMEM:
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

	DWORD* RemoveXboxDeclarationRedefinition(DWORD* pXboxDeclaration)
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
			if (type == XTL::X_D3DVSD_TOKEN_STREAMDATA && !(*pXboxToken & X_D3DVSD_MASK_SKIP) ||
				type == XTL::X_D3DVSD_TOKEN_TESSELLATOR)
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
	D3DVERTEXELEMENT *Convert(DWORD* pXboxDeclaration, bool bIsFixedFunction, CxbxVertexShaderInfo* pCxbxVertexShaderInfo)
	{
		// Get a preprocessed copy of the original Xbox Vertex Declaration
		auto pXboxVertexDeclarationCopy = RemoveXboxDeclarationRedefinition(pXboxDeclaration);

		pVertexShaderInfoToSet = pCxbxVertexShaderInfo;
		hostTemporaryRegisterCount = g_D3DCaps.VS20Caps.NumTemps;
		if (hostTemporaryRegisterCount < VSH_MIN_TEMPORARY_REGISTERS) {
			LOG_TEST_CASE("g_D3DCaps.VS20Caps.NumTemps < 12 (Host minimal vertex shader temporary register count)");
		}
		if (hostTemporaryRegisterCount < 12+1) { // TODO : Use a constant (see X_D3DVSD_REG)
			LOG_TEST_CASE("g_D3DCaps.VS20Caps.NumTemps < 12+1 (Xbox vertex shader temporary register count + r12, reading oPos)");
		}

		// Note, that some Direct3D 9 drivers return only the required minimum temporary register count of 12,
		// but regardless, shaders that use temporary register numbers above r12 still seem to work correctly.
		// So it seems we can't rely on VS20Caps.NumTemps indicating accurately what host hardware supports.
		// (Although it could be that the driver switches to software vertex processing when a shader exceeds hardware limits.)

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
		HostDeclarationSize = XboxDeclarationCount * sizeof(D3DVERTEXELEMENT) * 2;
	
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

		return Result;
	}
};

D3DVERTEXELEMENT *EmuRecompileVshDeclaration
(
    DWORD                *pXboxDeclaration,
    bool                  bIsFixedFunction,
    DWORD                *pXboxDeclarationCount,
    DWORD                *pHostDeclarationSize,
    CxbxVertexShaderInfo *pCxbxVertexShaderInfo
)
{
	XboxVertexDeclarationConverter Converter;

	D3DVERTEXELEMENT* pHostVertexElements = Converter.Convert(pXboxDeclaration, bIsFixedFunction, pCxbxVertexShaderInfo);

	*pXboxDeclarationCount = Converter.XboxDeclarationCount;
	*pHostDeclarationSize = Converter.HostDeclarationSize;

    return pHostVertexElements;
}

std::string UsingScratch(std::string input) {
	return std::regex_replace(input, std::regex("tmp"), "r" + std::to_string(X_VSH_TEMP_SCRATCH));
}

// Xbox expp seems to behave the as vs_1_1
std::string VshPostProcess_Expp(std::string shader) {
	// Find usages of exp with each swizzle
	// If there's no swizzle, we should still match so we do the calculation
	// for all components
	const auto xbox_expp_x = std::regex("expp (\\w\\d\\d?)(\\.x)?, (.+)$");
	const auto xbox_expp_y = std::regex("expp (\\w\\d\\d?)(\\.y)?, (.+)$");
	const auto xbox_expp_z = std::regex("expp (\\w\\d\\d?)(\\.z)?, (.+)$");
	const auto xbox_expp_w = std::regex("expp (\\w\\d\\d?)(\\.w)?, (.+)$");

	// We operate on a scalar so the input should have a swizzle?

	if (std::regex_search(shader, xbox_expp_x))
		LOG_TEST_CASE("Title uses the x component result of expp");
	if (std::regex_search(shader, xbox_expp_w))
		LOG_TEST_CASE("Title uses the w component result of expp");

	// dest.x = 2 ^ floor(x)
	// Test Case: ???
	static auto host_expp_x = UsingScratch(
		"; patch expp: dest.x = 2 ^ floor(x)\n"
		"frc tmp.x, $3\n"
		"add tmp.x, $1$2, -tmp.x\n"
		"exp $1.x,  tmp.x");
	shader = std::regex_replace(shader, xbox_expp_x, host_expp_x);

	// dest.y = x - floor(x)
	// Test Case: Tony Hawk Pro Skater 2X
	const auto host_expp_y =
		"; patch expp: dest.y = x - floor(x)\n"
		"frc $1.y, $3";
	shader = std::regex_replace(shader, xbox_expp_y, host_expp_y);

	// dest.z = approximate 2 ^ x
	// Test Case: Mechassault
	const auto host_expp_z =
		"; patch expp: dest.z = 2 ^ x\n"
		"exp $1.z, $3";
	shader = std::regex_replace(shader, xbox_expp_z, host_expp_z);

	// dest.w = 1
	// Test Case: ???
	// TODO do a constant read here
	const auto host_expp_w = UsingScratch(
		"; patch expp: dest.w = 1\n"
		"sub tmp.x, tmp.x, tmp.x\n" // Get 0
		"exp $1.w, tmp.x"); // 2 ^ 0 = 1
	shader = std::regex_replace(shader, xbox_expp_w, host_expp_w);

	return shader;
}

std::string VshPostProcess_Log(std::string shader) {
	const auto xbox_log_x = std::regex("log (\\w\\d\\d?)(\\.x)?, (.+)$");
	const auto xbox_log_y = std::regex("log (\\w\\d\\d?)(\\.y)?, (.+)$");
	const auto xbox_log_z = std::regex("log (\\w\\d\\d?)(\\.z)?, (.+)$");
	const auto xbox_log_w = std::regex("log (\\w\\d\\d?)(\\.w)?, (.+)$");

	if (std::regex_search(shader, xbox_log_x))
		LOG_TEST_CASE("Title uses the x component result of log");
	if (std::regex_search(shader, xbox_log_y))
		LOG_TEST_CASE("Title uses the y component result of log");
	if (std::regex_search(shader, xbox_log_w))
		LOG_TEST_CASE("Title uses the w component result of log");

	// exponent and mantissa functions such that
	// x = mantissa(x) * 2 ^ exponent(x)

	// dest.x = exponent(x)
	// Test Case: ???
	// floor(log(x))
	static auto host_log_x = UsingScratch(
		"; patch log: dest.x = exponent(x)\n"
		"log tmp.x, $3\n"
		"frc $tmp.y, $tmp.x\n"
		"sub $1.x, tmp.x, tmp.y");
	shader = std::regex_replace(shader, xbox_log_x, host_log_x);

	// dest.y = mantissa(x)
	// Test Case: ???
	// x / 2 ^ exponent(x)
	static auto host_log_y = UsingScratch(
		"; patch log: dest.y = mantissa(x)\n"
		"log tmp.x, $3\n"
		"frc $tmp.y, $tmp.x\n"
		"sub tmp.x, tmp.x, tmp.y\n" // tmp.x = exponent(x) = floor(log(x))
		"exp tmp.x, tmp.x\n"
		"rcp tmp.x, tmp.x\n" // tmp.x = 1 / (2 ^ exponent(x))
		"mul $1.y, $3, tmp.x");
	shader = std::regex_replace(shader, xbox_log_y, host_log_y);

	// dest.z = log(x)
	// Test Case: Mechassault (part of the mech glows depending on heat level)
	static auto host_log_z =
		"; patch log: dest.z = log(x)\n"
		"log $1.z, $3";
	shader = std::regex_replace(shader, xbox_log_z, host_log_z);

	// dest.w = 1
	// Test Case: ???
	// TODO do a constant read here
	const auto host_log_w = UsingScratch(
		"; patch log: dest.w = 1\n"
		"sub tmp.x, tmp.x, tmp.x\n" // Get 0
		"exp $1.w, tmp.x"); // 2 ^ 0 = 1
	shader = std::regex_replace(shader, xbox_log_w, host_log_w);

	return shader;
}

// On Xbox, the special indexing register, a0.x, is truncated
// But on vs_2_x and up, it's rounded to the closest integer
// So we have to truncate it ourselves
// Test Case: Buffy the Vampire Slayer
std::string VshPostProcess_TruncateMovA(std::string shader) {
	// find usages of mova
	const auto movA = std::regex("mova a0\\.x, (.*)$");
	// The equivalent of floor() with a temp register
	// and use the floored value
	static auto truncate = UsingScratch(
		"; patch mova: a = floor(x)\n"
		"frc  tmp,  $1\n"
		"add  tmp,  $1, -tmp\n"
		"mova a0.x, tmp");
	return std::regex_replace(shader, movA, truncate);
}

#include <fstream>
#include <streambuf>

// Post process the shader as a string
std::string VshPostProcess(std::string shader) {
	shader = VshPostProcess_Expp(shader);
	shader = VshPostProcess_Log(shader);
	return VshPostProcess_TruncateMovA(shader);
}

extern std::string BuildShader(VSH_XBOX_SHADER* pShader);

// recompile xbox vertex shader function
extern HRESULT EmuRecompileVshFunction
(
    DWORD        *pXboxFunction,
    bool          bNoReservedConstants,
	D3DVERTEXELEMENT *pRecompiledDeclaration,
    bool   		 *pbUseDeclarationOnly,
    DWORD        *pXboxFunctionSize,
	ID3DBlob **ppRecompiledShader
)
{
	XTL::X_VSH_SHADER_HEADER   *pXboxVertexShaderHeader = (XTL::X_VSH_SHADER_HEADER*)pXboxFunction;
    DWORD              *pToken;
    boolean             EOI = false;
    VSH_XBOX_SHADER    *pShader = (VSH_XBOX_SHADER*)calloc(1, sizeof(VSH_XBOX_SHADER));
	ID3DBlob           *pErrors;
    HRESULT             hRet = 0;

    // TODO: support this situation..
    if(pXboxFunction == xbnullptr)
        return E_FAIL;

	// Initialize output arguments to zero
	*pbUseDeclarationOnly = 0;
    *pXboxFunctionSize = 0;
    *ppRecompiledShader = nullptr;

    if(!pShader)
    {
        EmuLog(LOG_LEVEL::WARNING, "Couldn't allocate memory for vertex shader conversion buffer");
        return E_OUTOFMEMORY;
    }
    pShader->ShaderHeader = *pXboxVertexShaderHeader;
    switch(pXboxVertexShaderHeader->Version)
    {
        case VERSION_XVS:
            break;
        case VERSION_XVSS:
            EmuLog(LOG_LEVEL::WARNING, "Might not support vertex state shaders?");
            hRet = E_FAIL;
            break;
        case VERSION_XVSW:
            EmuLog(LOG_LEVEL::WARNING, "Might not support vertex read/write shaders?");
            hRet = E_FAIL;
            break;
        default:
            EmuLog(LOG_LEVEL::WARNING, "Unknown vertex shader version 0x%02X", pXboxVertexShaderHeader->Version);
            hRet = E_FAIL;
            break;
    }

    if(SUCCEEDED(hRet))
    {
		RegVIsUsedByShader.fill(false);

        for (pToken = (DWORD*)((uint8_t*)pXboxFunction + sizeof(XTL::X_VSH_SHADER_HEADER)); !EOI; pToken += X_VSH_INSTRUCTION_SIZE)
        {
            VSH_SHADER_INSTRUCTION Inst;

            VshParseInstruction((uint32_t*)pToken, &Inst);
            VshConvertToIntermediate(&Inst, pShader);
            EOI = (boolean)VshGetField((uint32_t*)pToken, FLD_FINAL);
        }

        // The size of the shader is
        *pXboxFunctionSize = (intptr_t)pToken - (intptr_t)pXboxFunction;

		// Do not attempt to compile empty shaders
		if (pShader->IntermediateCount == 0) {
            // This is a declaration only shader, so there is no function to recompile
            *pbUseDeclarationOnly = 1;
			return D3D_OK;
		}

		std::stringstream& pXboxShaderDisassembly = std::stringstream();
		std::stringstream& pHostShaderDisassembly = std::stringstream();

		static std::string hlslTemplate =
			#include "core\hle\D3D8\Direct3D9\Xb.hlsl" // Note : This included .hlsl defines a raw string
			;

		DbgVshPrintf("-- Before conversion --\n");
		VshWriteShader(pShader, pXboxShaderDisassembly, pRecompiledDeclaration, FALSE);
		DbgVshPrintf("%s", pXboxShaderDisassembly.str().c_str());
		DbgVshPrintf("-----------------------\n");

		auto hlslTest = BuildShader(pShader);
		hlslTest = std::regex_replace(hlslTemplate, std::regex("// <Xbox Shader>"), hlslTest);

		DbgVshPrintf("-- HLSL conversion 1 ---\n");
		DbgVshPrintf(hlslTest.c_str());
		DbgVshPrintf("-----------------------\n");

		VshConvertShader(pShader, bNoReservedConstants);
		VshWriteShader(pShader, pHostShaderDisassembly, pRecompiledDeclaration, TRUE);

		//auto hlslTest = BuildShader(pShader);
		//hlslTest = std::regex_replace(hlslTemplate, std::regex("// <Xbox Shader>"), hlslTest);

		// Post process the final shader
		auto finalHostShader = VshPostProcess(pHostShaderDisassembly.str());

		DbgVshPrintf("-- After conversion ---\n");
		DbgVshPrintf("%s", finalHostShader.c_str());
		DbgVshPrintf("-----------------------\n");

		DbgVshPrintf("-- HLSL conversion 2 ---\n");
		DbgVshPrintf(BuildShader(pShader).c_str());
		DbgVshPrintf("-----------------------\n");

        // HACK: Azurik. Prevent Direct3D from trying to assemble this.
		if(finalHostShader == "vs.2.x\n")
		{
			EmuLog(LOG_LEVEL::WARNING, "Replacing empty vertex shader with fallback");

			finalHostShader = std::string(
				"vs.2.x\n"
				"dcl_position v0\n"
				"dp4 oPos.x, v0, c96\n"
				"dp4 oPos.y, v0, c97\n"
				"dp4 oPos.z, v0, c98\n"
				"dp4 oPos.w, v0, c99\n"
			);
		}
		else
		{
			hRet = D3DCompile(
				hlslTest.c_str(),
				hlslTest.length(),
				nullptr, // pSourceName
				nullptr, // pDefines
				nullptr, // pInclude // TODO precompile x_* HLSL functions?
				"main", // shader entry poiint
				"vs_3_0", // shader profile
				0, // flags1
				0, // flags2
				ppRecompiledShader, // out
				&pErrors // ppErrorMsgs out
			);
		}

        if (FAILED(hRet))
        {
            EmuLog(LOG_LEVEL::WARNING, "Couldn't assemble recompiled vertex shader");
			EmuLog(LOG_LEVEL::WARNING, "%s", (char*)(pErrors)->GetBufferPointer());
			//LOG_TEST_CASE((char *)pErrors->GetBufferPointer());
        }

		if (pErrors)
			(pErrors)->Release();
    }

    free(pShader);

    return hRet;
}

extern void FreeVertexDynamicPatch(CxbxVertexShader *pVertexShader)
{
    pVertexShader->VertexShaderInfo.NumberOfVertexStreams = 0;
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

CxbxVertexShaderInfo *GetCxbxVertexShaderInfo(DWORD XboxVertexShaderHandle)
{
    CxbxVertexShader *pCxbxVertexShader = GetCxbxVertexShader(XboxVertexShaderHandle);

    for (uint32_t i = 0; i < pCxbxVertexShader->VertexShaderInfo.NumberOfVertexStreams; i++)
    {
        if (pCxbxVertexShader->VertexShaderInfo.VertexStreams[i].NeedPatch)
        {
            return &pCxbxVertexShader->VertexShaderInfo;
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

void SetCxbxVertexShader(DWORD XboxVertexShaderHandle, CxbxVertexShader* shader)
{
	auto it = g_CxbxVertexShaders.find(XboxVertexShaderHandle);
	if (it != g_CxbxVertexShaders.end() && it->second != nullptr && shader != nullptr) {
		LOG_TEST_CASE("Overwriting existing Vertex Shader");
	}

	g_CxbxVertexShaders[XboxVertexShaderHandle] = shader;
}

void CxbxImpl_SetVertexShaderInput
(
	DWORD              Handle,
	UINT               StreamCount,
	XTL::X_STREAMINPUT* pStreamInputs
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
	XTL::X_VERTEXATTRIBUTEFORMAT* pVAF,
	DWORD Address
)
{
	LOG_INIT;

	// When pVAF is non-null, this vertex attribute format takes precedence over the the one	
	LOG_UNIMPLEMENTED();
}

// HLSL outputs

std::string ToHlsl(VSH_IMD_OUTPUT& dest) {
	auto hlsl = std::stringstream();
	switch (dest.Type)
	{
	case IMD_OUTPUT_O:
		hlsl << OReg_Name[dest.Address];
		break;
	case IMD_OUTPUT_A0X:
		hlsl << "a";
		break;
	case IMD_OUTPUT_C:
		hlsl << "c[" << dest.Address << "]"; //todo we can output to constants...?
		break;
	case IMD_OUTPUT_R:
			hlsl << "r" << dest.Address;
		break;
	default:
		break;
	}

	// If we're not writing all channels, write the mask
	if (!(dest.Mask[0] && dest.Mask[1] && dest.Mask[2] && dest.Mask[3]))
	{
		hlsl << "." << (dest.Mask[0] ? "x" : "")
					<< (dest.Mask[1] ? "y" : "")
					<< (dest.Mask[2] ? "z" : "")
					<< (dest.Mask[3] ? "w" : "");
	}

	return hlsl.str();
}

std::string ToHlsl(VSH_IMD_PARAMETER& paramMeta)
{
	auto hlsl = std::stringstream();
	auto param = paramMeta.Parameter;

	if (param.Neg) {
		hlsl << "-";
	}

	if (param.ParameterType == PARAM_C) {
		// We'll use the c() function instead of direct indexing
		if (paramMeta.IndexesWithA0_X) {
			// Only display the offset if it's not 0.
			if (param.Address != 0) {
				hlsl << "c(a+" << param.Address << ")";
			} else {
				hlsl << "c(a)";
			}
		} else {
			hlsl << "c(" << param.Address << ")";
		}
	} else {
		hlsl << VshGetRegisterName(param.ParameterType) << param.Address;
	}

	// Write the swizzle if we need to
	// Only bother printing the swizzle if it is not the default .xyzw
	if (!(param.Swizzle[0] == SWIZZLE_X &&
		  param.Swizzle[1] == SWIZZLE_Y &&
		  param.Swizzle[2] == SWIZZLE_Z &&
	      param.Swizzle[3] == SWIZZLE_W ))
	{
		// We'll try to simplify swizzles if we can
		// If all swizzles are the same, we only need to write one out
		unsigned swizzles = 1;

		// Otherwise, we need to use the full swizzle
		if (param.Swizzle[0] != param.Swizzle[1] ||
			param.Swizzle[0] != param.Swizzle[2] ||
			param.Swizzle[0] != param.Swizzle[3]) {
			// Note, we can't remove trailing repeats, like in VS asm,
			// as it may change the type from float4 to float3, float2 or float1!
			swizzles = 4;
		}

		hlsl << ".";
		for (unsigned i = 0; i < swizzles; i++) {
			hlsl << "xyzw"[param.Swizzle[i]];
		}
	}

	return hlsl.str();
}

std::string ToHlsl(std::string pattern, VSH_INTERMEDIATE_FORMAT& instruction) {
	auto static regDest = std::regex("dest");
	const std::regex regSrc[] = { std::regex("src0"), std::regex("src1"), std::regex("src2") };

	// TODO use simple string replace
	// Warn if we didn't replace anything etc.
	// Replace dest
	auto hlsl = std::regex_replace(pattern, regDest, ToHlsl(instruction.Output));

	int srcNum = 0;
	for (int i = 0; i < 3; i++) { // TODO remove magic number
		if (instruction.Parameters[i].Active) {
			hlsl = std::regex_replace(hlsl, regSrc[srcNum], ToHlsl(instruction.Parameters[i]));
			srcNum += 1;
		}
	}

	return hlsl;
}

std::string BuildShader(VSH_XBOX_SHADER* pShader) {

	// HLSL strings for all MAC opcodes, indexed with VSH_MAC
	static std::string VSH_MAC_HLSL[] = {
		/*MAC_NOP:*/"// MAC_NOP\n",
		/*MAC_MOV:*/"dest = x_mov(src0);\n",
		/*MAC_MUL:*/"dest = x_mul(src0, src1);\n",
		/*MAC_ADD:*/"dest = x_add(src0, src1);\n",
		/*MAC_MAD:*/"dest = x_mad(src0, src1, src2);\n",
		/*MAC_DP3:*/"dest = x_dp3(src0, src1);\n",
		/*MAC_DPH:*/"dest = x_dph(src0, src1);\n",
		/*MAC_DP4:*/"dest = x_dp4(src0, src1);\n",
		/*MAC_DST:*/"dest = x_dst(src0, src1);\n",
		/*MAC_MIN:*/"dest = x_min(src0, src1);\n",
		/*MAC_MAX:*/"dest = x_max(src0, src1);\n",
		/*MAC_SLT:*/"dest = x_slt(src0, src1);\n",
		/*MAC_SGE:*/"dest = x_sge(src0, src1);\n",
		/*MAC_ARL:*/"a = x_arl(src0);\n", // Note : For this MAC_ARL case, ToHlsl would always replace 'dest' with 'a', so we optimized this upfront
		"// ??? VSH_MAC 14 ???;\n",
		"// ??? VSH_MAC 15 ???;\n" // VSH_MAC 2 final values of the 4 bits are undefined/unknown  TODO : Investigate their effect (if any) and emulate that as well
	};

	// HLSL strings for all ILU opcodes, indexed with VSH_ILU
	static std::string VSH_ILU_HLSL[] = {
		/*ILU_NOP:*/"// ILU_NOP\n",
		/*ILU_MOV:*/"dest = x_mov(src0);\n",
		/*ILU_RCP:*/"dest = x_rcp(src0);\n",
		/*ILU_RCC:*/"dest = x_rcc(src0);\n",
		/*ILU_RSQ:*/"dest = x_rsq(src0);\n",
		/*ILU_EXP:*/"dest = x_exp(src0);\n",
		/*ILU_LOG:*/"dest = x_log(src0);\n",
		/*ILU_LIT:*/"dest = x_lit(src0);\n" // = 7 - all values of the 3 bits are used
	};

	auto hlsl = std::stringstream();

	for (int i = 0; i < pShader->IntermediateCount; i++) {
		VSH_INTERMEDIATE_FORMAT& xboxInstruction = pShader->Intermediate[i];

		if (xboxInstruction.InstructionType == IMD_MAC) {
			hlsl << ToHlsl(VSH_MAC_HLSL[xboxInstruction.MAC], xboxInstruction);
		} else if (xboxInstruction.InstructionType == IMD_ILU) {
			hlsl << ToHlsl(VSH_ILU_HLSL[xboxInstruction.ILU], xboxInstruction);
		} else {
			EmuLog(LOG_LEVEL::WARNING, "TODO message");
		}
	}

	return hlsl.str();
}
