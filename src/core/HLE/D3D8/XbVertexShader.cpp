// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   core->HLE->D3D8->XbVertexShader.cpp
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
#define _XBOXKRNL_DEFEXTRN_
#define LOG_PREFIX CXBXR_MODULE::VTXSH

#define _DEBUG_TRACK_VS

#include "CxbxKrnl/CxbxKrnl.h"
#include "CxbxKrnl/Emu.h"
#include "CxbxKrnl/EmuFS.h"
#include "CxbxKrnl/EmuXTL.h"
#include "XbD3D8Types.h" // For X_D3DVSDE_*
#include <sstream>
#include <unordered_map>
#include <array>

#ifdef CXBX_USE_VS30
//#define CXBX_USE_VS30 // Separate the port to Vertex Shader model 3.0 from the port to Direct3D9
#endif

#define DbgVshPrintf \
	LOG_CHECK_ENABLED(LOG_LEVEL::DEBUG) \
		if(g_bPrintfOn) printf

// ****************************************************************************
// * Vertex shader function recompiler
// ****************************************************************************

// Local macros
#define VERSION_VS                     0xF0 // vs.1.1, not an official value
#define VERSION_XVS                    0x20 // Xbox vertex shader
#define VERSION_XVSS                   0x73 // Xbox vertex state shader
#define VERSION_XVSW                   0x77 // Xbox vertex read/write shader
#define VSH_XBOX_MAX_INSTRUCTION_COUNT 136  // The maximum Xbox shader instruction count
#define VSH_MAX_INTERMEDIATE_COUNT     1024 // The maximum number of intermediate format slots

#define VSH_MAX_TEMPORARY_REGISTERS 32
#define VSH_VS11_MAX_INSTRUCTION_COUNT 128
#define VSH_VS2X_MAX_INSTRUCTION_COUNT 256
#define VSH_VS30_MAX_INSTRUCTION_COUNT 512

#define VSH_MAX_INSTRUCTION_COUNT VSH_VS2X_MAX_INSTRUCTION_COUNT

#define X_D3DVSD_MASK_TESSUV 0x10000000
#define X_D3DVSD_MASK_SKIP 0x10000000 // Skips (normally) dwords
#define X_D3DVSD_MASK_SKIPBYTES 0x08000000 // Skips bytes (no, really?!)
#define X_D3DVSD_STREAMTESSMASK (1 << 28)

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
    int16               Address;         // Register address
}
VSH_PARAMETER;

typedef struct _VSH_OUTPUT
{
    // Output register
    VSH_OUTPUT_MUX      OutputMux;       // MAC or ILU used as output
	VSH_OUTPUT_TYPE     OutputType;      // C or O
    boolean             OutputMask[4];
    int16               OutputAddress;
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
    int16               Address;
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
    uint08          SubToken;
    uint08          StartBit;
    uint08          BitLength;
}
VSH_FIELDMAPPING;

typedef struct _VSH_XBOX_SHADER
{
    XTL::VSH_SHADER_HEADER       ShaderHeader;
    uint16                  IntermediateCount;
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
    "frc", // The Xbox EXP instruction behaves like FRC on PC, so we just translate it as frc. Fixes broken polygons in THPS2X
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

std::array<bool, 16> RegVUsage;
std::array<int, 16> RegVDeclUsage;

/* TODO : map non-FVF Xbox vertex shader handle to CxbxVertexShader (a struct containing a host Xbox vertex shader handle and the original members)
std::unordered_map<DWORD, CxbxVertexShader> g_CxbxVertexShaders;

void CxbxUpdateVertexShader(DWORD XboxVertexShaderHandle)
{
	CxbxVertexShader &VertexShader = g_CxbxVertexShaders[XboxVertexShaderHandle];
}*/

// Dxbx note : This tooling function is never used, but clearly illustrates the relation
// between vertex shader's being passed around, and the actual handle value used on PC.
DWORD VshHandleGetRealHandle(DWORD aHandle)
{
	using namespace XTL;

	CxbxVertexShader *pVertexShader = GetCxbxVertexShader(aHandle);
	if (pVertexShader)
	{
		// assert(pVertexShader);

		return pVertexShader->Handle;
	}
	else
		return aHandle;
}

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
static inline int VshGetFromToken(uint32 *pShaderToken,
                                  uint08 SubToken,
                                  uint08 StartBit,
                                  uint08 BitLength)
{
    return (pShaderToken[SubToken] >> StartBit) & ~(0xFFFFFFFF << BitLength);
}

// Converts the C register address to disassembly format
static inline int16 ConvertCRegister(const int16 CReg)
{
    return ((((CReg >> 5) & 7) - 3) * 32) + (CReg & 31);
}

uint08 VshGetField(uint32         *pShaderToken,
                   VSH_FIELD_NAME FieldName)
{
    return (uint08)(VshGetFromToken(pShaderToken,
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
		    return NULL;
}

static void VshParseInstruction(uint32                 *pShaderToken,
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
                               char    *pDisassembly,
                               uint32  *pDisassemblyPos)
{
    if(OutputMask[0] && OutputMask[1] && OutputMask[2] && OutputMask[3])
    {
        // All components are there, no need to print the mask
        return;
    }
    *pDisassemblyPos += sprintf(pDisassembly + *pDisassemblyPos, ".%s%s%s%s",
                                OutputMask[0] ? "x" : "",
                                OutputMask[1] ? "y" : "",
                                OutputMask[2] ? "z" : "",
                                OutputMask[3] ? "w" : "");
}

static void VshWriteParameter(VSH_IMD_PARAMETER *pParameter,
                              char              *pDisassembly,
                              uint32            *pDisassemblyPos)
{
    *pDisassemblyPos += sprintf(pDisassembly + *pDisassemblyPos, ", %s%s",
                                pParameter->Parameter.Neg ? "-" : "",
                                VshGetRegisterName(pParameter->Parameter.ParameterType));
    if(pParameter->Parameter.ParameterType == PARAM_C && pParameter->IndexesWithA0_X)
    {
        // Only display the offset if it's not 0.
        if(pParameter->Parameter.Address)
        {
            *pDisassemblyPos += sprintf(pDisassembly + *pDisassemblyPos, "[a0.x+%d]", pParameter->Parameter.Address);
        }
        else
        {
            *pDisassemblyPos += sprintf(pDisassembly + *pDisassemblyPos, "[a0.x]");
        }
    }
    else
    {
        *pDisassemblyPos += sprintf(pDisassembly + *pDisassemblyPos, "%d", pParameter->Parameter.Address);
    }
    // Only bother printing the swizzle if it is not .xyzw
    if(!(pParameter->Parameter.Swizzle[0] == SWIZZLE_X &&
          pParameter->Parameter.Swizzle[1] == SWIZZLE_Y &&
          pParameter->Parameter.Swizzle[2] == SWIZZLE_Z &&
          pParameter->Parameter.Swizzle[3] == SWIZZLE_W))
    {
        int i;

        *pDisassemblyPos += sprintf(pDisassembly + *pDisassemblyPos, ".");
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
            *pDisassemblyPos += sprintf(pDisassembly + *pDisassemblyPos, "%c", Swizzle);
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


// From D3D8to9
static const BYTE DeclAddressUsages[][2] =
{
	{ XTL::D3DDECLUSAGE_POSITION, 0 },
	{ XTL::D3DDECLUSAGE_BLENDWEIGHT, 0 },
	{ XTL::D3DDECLUSAGE_BLENDINDICES, 0 },
	{ XTL::D3DDECLUSAGE_NORMAL, 0 },
	{ XTL::D3DDECLUSAGE_PSIZE, 0 },
	{ XTL::D3DDECLUSAGE_COLOR, 0 },
	{ XTL::D3DDECLUSAGE_COLOR, 1 },
	{ XTL::D3DDECLUSAGE_TEXCOORD, 0 },
	{ XTL::D3DDECLUSAGE_TEXCOORD, 1 },
	{ XTL::D3DDECLUSAGE_TEXCOORD, 2 },
	{ XTL::D3DDECLUSAGE_TEXCOORD, 3 },
	{ XTL::D3DDECLUSAGE_TEXCOORD, 4 },
	{ XTL::D3DDECLUSAGE_TEXCOORD, 5 },
	{ XTL::D3DDECLUSAGE_TEXCOORD, 6 },
	{ XTL::D3DDECLUSAGE_TEXCOORD, 7 },
	{ XTL::D3DDECLUSAGE_POSITION, 1 },
	{ XTL::D3DDECLUSAGE_NORMAL, 1 }
};


static void VshWriteShader(VSH_XBOX_SHADER *pShader,
                           char* pDisassembly,
						   XTL::D3DVERTEXELEMENT *pRecompiled,
                           boolean Truncate)
{
    uint32 DisassemblyPos = 0;
    switch(pShader->ShaderHeader.Version)
    {
        case VERSION_VS:
#ifdef CXBX_USE_VS30
            DisassemblyPos += sprintf(pDisassembly + DisassemblyPos, "vs.3.0\n");
#else
            DisassemblyPos += sprintf(pDisassembly + DisassemblyPos, "vs.2.x\n");
#endif
            break;
        case VERSION_XVS:
            DisassemblyPos += sprintf(pDisassembly + DisassemblyPos, "xvs.1.1\n");
            break;
        case VERSION_XVSS:
            DisassemblyPos += sprintf(pDisassembly + DisassemblyPos, "xvss.1.1\n");
            break;
        case VERSION_XVSW:
            DisassemblyPos += sprintf(pDisassembly + DisassemblyPos, "xvsw.1.1\n");
            break;
        default:
            break;
    }

	if (Truncate) {
		DisassemblyPos += sprintf(pDisassembly + DisassemblyPos, "; Input usage declarations --\n");
		int i = 0;
		do {
			if (RegVUsage[i]) {
				DWORD PCUsageIndex = DeclAddressUsages[i][1];
				DWORD usage = DeclAddressUsages[i][0];

				// If an override exists, use it
				if (RegVDeclUsage[i] >= 0) {
					usage = RegVDeclUsage[i];
				}

				std::stringstream dclStream;
				switch (usage) {
				case XTL::D3DDECLUSAGE_POSITION:
                    dclStream << "dcl_position" << (int)PCUsageIndex;
					break;
				case XTL::D3DDECLUSAGE_BLENDWEIGHT:
					dclStream << "dcl_blendweight";
					break;
				case XTL::D3DDECLUSAGE_BLENDINDICES:
					dclStream << "dcl_blendindices";
					break;
				case XTL::D3DDECLUSAGE_NORMAL:
					dclStream << "dcl_normal";
					break;
				case XTL::D3DDECLUSAGE_COLOR:
					dclStream << "dcl_color" << (int)PCUsageIndex;
					break;
				case XTL::D3DDECLUSAGE_FOG:
					dclStream << "dcl_fog";
					break;
				case XTL::D3DDECLUSAGE_TEXCOORD:
					dclStream << "dcl_texcoord" << (int)PCUsageIndex;
					break;
				case XTL::D3DDECLUSAGE_PSIZE:
					dclStream << "dcl_psize";
					break;
				default:
					dclStream << "dcl_unknown ("<< (int)PCUsageIndex << ")";
					LOG_TEST_CASE("Encountered unknown declaration");
					break;
				}

				DisassemblyPos += sprintf(pDisassembly + DisassemblyPos, "%s v%d\n", dclStream.str().c_str(), i);
			}

			i++;
		} while (i < RegVUsage.size());
	}

    for (int i = 0; i < pShader->IntermediateCount && (i < VSH_MAX_INSTRUCTION_COUNT || !Truncate); i++)
    {
        VSH_INTERMEDIATE_FORMAT *pIntermediate = &pShader->Intermediate[i];

        if(i == VSH_MAX_INSTRUCTION_COUNT)
        {
            DisassemblyPos += sprintf(pDisassembly + DisassemblyPos, "; -- Passing the truncation limit --\n");
        }

        // Writing combining sign if neccessary
        if(pIntermediate->IsCombined)
        {
            DisassemblyPos += sprintf(pDisassembly + DisassemblyPos, "+");
        }

        // Print the op code
        if(pIntermediate->InstructionType == IMD_MAC)
        {
			// Dxbx addition : Safeguard against incorrect MAC opcodes :
			if (pIntermediate->MAC > MAC_ARL)
				DisassemblyPos += sprintf(pDisassembly + DisassemblyPos, "??? ");
			else
				DisassemblyPos += sprintf(pDisassembly + DisassemblyPos, "%s ", MAC_OpCode[pIntermediate->MAC]);
        }
        else // IMD_ILU
        {
			// Dxbx addition : Safeguard against incorrect ILU opcodes :
			if (pIntermediate->ILU > ILU_LIT)
				DisassemblyPos += sprintf(pDisassembly + DisassemblyPos, "??? ");
			else
				DisassemblyPos += sprintf(pDisassembly + DisassemblyPos, "%s ", ILU_OpCode[pIntermediate->ILU]);
        }

        // Print the output parameter
        if(pIntermediate->Output.Type == IMD_OUTPUT_A0X)
        {
            DisassemblyPos += sprintf(pDisassembly + DisassemblyPos, "a0.x");
        }
        else
        {
            switch(pIntermediate->Output.Type)
            {
            case IMD_OUTPUT_C:
                DisassemblyPos += sprintf(pDisassembly + DisassemblyPos, "c%d", pIntermediate->Output.Address);
                break;
            case IMD_OUTPUT_R:
                DisassemblyPos += sprintf(pDisassembly + DisassemblyPos, "r%d", pIntermediate->Output.Address);
                break;
            case IMD_OUTPUT_O:
				// Dxbx addition : Safeguard against incorrect VSH_OREG_NAME values :
				if ((int)pIntermediate->Output.Address > OREG_A0X)
					; // don't add anything
				else
					DisassemblyPos += sprintf(pDisassembly + DisassemblyPos, "%s", OReg_Name[pIntermediate->Output.Address]);
                break;
            default:
                CxbxKrnlCleanup("Invalid output register in vertex shader!");
                break;
            }
            VshWriteOutputMask(pIntermediate->Output.Mask, pDisassembly, &DisassemblyPos);
        }
        // Print the parameters
        for (int p = 0; p < 3; p++)
        {
            VSH_IMD_PARAMETER *pParameter = &pIntermediate->Parameters[p];
            if(pParameter->Active)
            {
                VshWriteParameter(pParameter, pDisassembly, &DisassemblyPos);
            }
        }
        DisassemblyPos += sprintf(pDisassembly + DisassemblyPos, "\n");
    }
    *(pDisassembly + DisassemblyPos) = 0;
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
    uint08 ParamCount = 0;
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
    if(pShader->IntermediateCount == VSH_MAX_INTERMEDIATE_COUNT)
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
                                  uint16                  Pos)
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
                                  uint16          Pos)
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
    int i;
    boolean WIsWritten;
    boolean XIsWritten;

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
    int16 PosC38    = -1;
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
    // space transformation, (b) reads r10 or r11 after we have written to
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
                // Redirect output to r11.
                pIntermediate->Output.Type    = IMD_OUTPUT_R;
                pIntermediate->Output.Address = 11;

                // Scale r11 to r10. (mul r10.[mask], r11, c58)
                VSH_INTERMEDIATE_FORMAT MulIntermediate;
                MulIntermediate.IsCombined        = FALSE;
                MulIntermediate.InstructionType   = IMD_MAC;
                MulIntermediate.MAC               = MAC_MUL;
                MulIntermediate.Output.Type       = IMD_OUTPUT_R;
                MulIntermediate.Output.Address    = 10;
                MulIntermediate.Output.Mask[0]    = pIntermediate->Output.Mask[0];
                MulIntermediate.Output.Mask[1]    = pIntermediate->Output.Mask[1];
                MulIntermediate.Output.Mask[2]    = pIntermediate->Output.Mask[2];
                MulIntermediate.Output.Mask[3]    = pIntermediate->Output.Mask[3];
                MulIntermediate.Parameters[0].Active                  = TRUE;
                MulIntermediate.Parameters[0].IndexesWithA0_X                   = FALSE;
                MulIntermediate.Parameters[0].Parameter.ParameterType = PARAM_R;
                MulIntermediate.Parameters[0].Parameter.Address       = 11;
                MulIntermediate.Parameters[0].Parameter.Neg           = FALSE;
                VshSetSwizzle(&MulIntermediate.Parameters[0], SWIZZLE_X, SWIZZLE_Y, SWIZZLE_Z, SWIZZLE_W);
                MulIntermediate.Parameters[1].Active                  = TRUE;
                MulIntermediate.Parameters[1].IndexesWithA0_X                   = FALSE;
                MulIntermediate.Parameters[1].Parameter.ParameterType = PARAM_C;
                MulIntermediate.Parameters[1].Parameter.Address       = ConvertCRegister(58);
                MulIntermediate.Parameters[1].Parameter.Neg           = FALSE;
                VshSetSwizzle(&MulIntermediate.Parameters[1], SWIZZLE_X, SWIZZLE_Y, SWIZZLE_Z, SWIZZLE_W);
                MulIntermediate.Parameters[2].Active                  = FALSE;
                VshInsertIntermediate(pShader, &MulIntermediate, ++i);

                // Add offset with r10 to oPos (add oPos.[mask], r10, c59)
                VSH_INTERMEDIATE_FORMAT AddIntermediate = MulIntermediate;
                AddIntermediate.MAC               = MAC_ADD;
                AddIntermediate.Output.Type       = IMD_OUTPUT_O;
                AddIntermediate.Output.Address    = OREG_OPOS;
                AddIntermediate.Parameters[0].Parameter.ParameterType = PARAM_R;
                AddIntermediate.Parameters[0].Parameter.Address       = 10;
                AddIntermediate.Parameters[1].Parameter.Address       = ConvertCRegister(59);
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

// Converts the intermediate format vertex shader to DirectX 8 format
static boolean VshConvertShader(VSH_XBOX_SHADER *pShader,
                                boolean         bNoReservedConstants,
								bool			*pDeclaredRegisters
)
{
    using namespace XTL;

    extern XTL::D3DCAPS g_D3DCaps;

    const DWORD temporaryCount = g_D3DCaps.VS20Caps.NumTemps;

    boolean RUsage[VSH_MAX_TEMPORARY_REGISTERS] = { FALSE };
    // TODO: What about state shaders and such?
    pShader->ShaderHeader.Version = VERSION_VS;

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

			// Fix when mad to a scaler input does not use a replicate swizzle
			// Test case: Panzer Dragoon Orta
			if (pIntermediate->InstructionType == IMD_MAC && pIntermediate->MAC == MAC_MAD) {
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
				if (pIntermediate->Parameters[j].Parameter.ParameterType == PARAM_R)
				{
					RUsage[pIntermediate->Parameters[j].Parameter.Address] = TRUE;
				}
				// Make constant registers range from 0 to 192 instead of -96 to 96
				if (pIntermediate->Parameters[j].Parameter.ParameterType == PARAM_C)
				{
					//if(pIntermediate->Parameters[j].Parameter.Address < 0)
					pIntermediate->Parameters[j].Parameter.Address += 96;
				}

				if (pIntermediate->Parameters[j].Parameter.ParameterType == PARAM_V) {
					RegVUsage[pIntermediate->Parameters[j].Parameter.Address] = TRUE;
				}
			}
		}

        if(pIntermediate->Output.Type == IMD_OUTPUT_R)
        {
            RUsage[pIntermediate->Output.Address] = TRUE;
        }
        // Make constant registers range from 0 to 192 instead of -96 to 96
        if(pIntermediate->Output.Type == IMD_OUTPUT_C)
        {
			//if(pIntermediate->Output.Address < 0)
				pIntermediate->Output.Address += 96;
        }



        if(pIntermediate->InstructionType == IMD_MAC && pIntermediate->MAC == MAC_DPH)
        {
			// 2010/01/12 - revel8n - attempt to alleviate conversion issues relate to the dph instruction

            // Replace dph with dp3 and add
            if(pIntermediate->Output.Type != IMD_OUTPUT_R)
            {
                // TODO: Complete dph support
                EmuLog(LOG_LEVEL::WARNING, "Can't simulate dph for other than output r registers (yet)");

				// attempt to find unused register...
				int outRegister = -1;
				for (int j = temporaryCount - 1; j >= 0; --j)
				{
                    if (j == 12) continue;

					if(!RUsage[j])
					{
						outRegister = j;
						break;
					}
				}

				// return failure if there are no available registers
				if (outRegister == -1)
				{
					return FALSE;
				}

				VSH_INTERMEDIATE_FORMAT TmpIntermediate = *pIntermediate;

				// modify the instructions
				// the register value is not needed beyond these instructions so setting the usage flag should not be necessary (??)
				pIntermediate->MAC = MAC_DP3;
				pIntermediate->Output.Type = IMD_OUTPUT_R;
				pIntermediate->Output.Address = outRegister;
				VshSetOutputMask(&pIntermediate->Output, TRUE, TRUE, TRUE, TRUE);

				TmpIntermediate.MAC = MAC_ADD;
				TmpIntermediate.Parameters[0].IndexesWithA0_X = FALSE;
				TmpIntermediate.Parameters[0].Parameter.ParameterType = PARAM_R;
				TmpIntermediate.Parameters[0].Parameter.Address = outRegister;
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
    int16 R12Replacement = -1;
    if(temporaryCount <= 12 && RUsage[12])
    {
        // Sigh, they absolutely had to use r12, didn't they?
        for (int i = temporaryCount - 1; i >= 0; i--)
        {
            if(!RUsage[i])
            {
                R12Replacement = i;
                break;
            }
        }
        if(R12Replacement == -1)
        {
            EmuLog(LOG_LEVEL::WARNING, "Vertex shader uses all r registers, including r12; impossible to convert!");
            return FALSE;
        }
        for (int j = 0; j < pShader->IntermediateCount; j++)
        {
            VSH_INTERMEDIATE_FORMAT* pIntermediate = &pShader->Intermediate[j];
            if(pIntermediate->Output.Type == IMD_OUTPUT_O &&
                pIntermediate->Output.Address == OREG_OPOS)
            {
                // Found instruction writing to oPos
                pIntermediate->Output.Type = IMD_OUTPUT_R;
                pIntermediate->Output.Address = R12Replacement;
            }

            for (int k = 0; k < 3; k++)
            {
                if(pIntermediate->Parameters[k].Active)
                {
                    if(pIntermediate->Parameters[k].Parameter.ParameterType == PARAM_R &&
                        pIntermediate->Parameters[k].Parameter.Address == 12)
                    {
                        // Found a r12 used as a parameter; replace
                        pIntermediate->Parameters[k].Parameter.Address = R12Replacement;
                    }
                    else if(pIntermediate->Parameters[k].Parameter.ParameterType == PARAM_C &&
                             pIntermediate->Parameters[k].Parameter.Address == 58 &&
                             !pIntermediate->Parameters[k].IndexesWithA0_X)
                    {
                        // Found c-38, replace it with r12.w
                        pIntermediate->Parameters[k].Parameter.ParameterType = PARAM_R;
                        pIntermediate->Parameters[k].Parameter.Address = R12Replacement;
                        VshSetSwizzle(&pIntermediate->Parameters[k], SWIZZLE_W, SWIZZLE_W, SWIZZLE_W, SWIZZLE_W);
                    }
                }
            }
        }
        // Insert mov oPos, r## in the end
        VSH_INTERMEDIATE_FORMAT *pOPosWriteBack = VshNewIntermediate(pShader);
        pOPosWriteBack->InstructionType = IMD_ILU;
        pOPosWriteBack->ILU = ILU_MOV;
        pOPosWriteBack->MAC = MAC_NOP;
        pOPosWriteBack->Output.Type = IMD_OUTPUT_O;
        pOPosWriteBack->Output.Address = OREG_OPOS;
        VshSetOutputMask(&pOPosWriteBack->Output, TRUE, TRUE, TRUE, TRUE);
        pOPosWriteBack->Parameters[0].Active = TRUE;
        pOPosWriteBack->Parameters[0].Parameter.ParameterType = PARAM_R;
        pOPosWriteBack->Parameters[0].Parameter.Address = R12Replacement;
        VshSetSwizzle(&pOPosWriteBack->Parameters[0], SWIZZLE_X, SWIZZLE_Y, SWIZZLE_Z, SWIZZLE_W);
    }
    return TRUE;
}

// ****************************************************************************
// * Vertex shader declaration recompiler
// ****************************************************************************

typedef struct _CxbxVertexShaderPatch
{
	XTL::CxbxVertexShaderInfo *pVertexShaderInfoToSet;
    XTL::CxbxVertexShaderStreamInfo  *pCurrentVertexShaderStreamInfo;
}
CxbxVertexShaderPatch;

// VERTEX SHADER
#define DEF_VSH_END 0xFFFFFFFF
#define DEF_VSH_NOP 0x00000000

static DWORD VshGetDeclarationCount(DWORD *pDeclaration)
{
    DWORD Pos = 0;
    while (*(pDeclaration + Pos) != DEF_VSH_END)
    {
        Pos++;
    }
    return Pos + 1;
}

#define D3DDECLUSAGE_UNSUPPORTED ((D3DDECLUSAGE)-1)

XTL::D3DDECLUSAGE Xb2PCRegisterType
(
	DWORD VertexRegister,
	boolean IsFixedFunction,
	DWORD& PCUsageIndex
)
{
	using namespace XTL;

	D3DDECLUSAGE PCRegisterType;
	PCUsageIndex = 0;

	if (!IsFixedFunction) {
		DbgVshPrintf("%d", VertexRegister);
		PCUsageIndex = DeclAddressUsages[VertexRegister][1];
		return (D3DDECLUSAGE)DeclAddressUsages[VertexRegister][0];
	}

	switch (VertexRegister)
	{
	case X_D3DVSDE_VERTEX: // -1
		DbgVshPrintf("D3DVSDE_VERTEX /* xbox ext. */");
		PCRegisterType = D3DDECLUSAGE_UNSUPPORTED;
		break;
	case X_D3DVSDE_POSITION: // 0
		DbgVshPrintf("D3DVSDE_POSITION");
		PCRegisterType = D3DDECLUSAGE_POSITION;
		break;
	case X_D3DVSDE_BLENDWEIGHT: // 1
		DbgVshPrintf("D3DVSDE_BLENDWEIGHT");
		PCRegisterType = D3DDECLUSAGE_BLENDWEIGHT;
		break;
	case X_D3DVSDE_NORMAL: // 2
		DbgVshPrintf("D3DVSDE_NORMAL");
		PCRegisterType = D3DDECLUSAGE_NORMAL;
		break;
	case X_D3DVSDE_DIFFUSE: // 3
		DbgVshPrintf("D3DVSDE_DIFFUSE");
		PCRegisterType = D3DDECLUSAGE_COLOR; PCUsageIndex = 0;
		break;
	case X_D3DVSDE_SPECULAR: // 4
		DbgVshPrintf("D3DVSDE_SPECULAR");
		PCRegisterType = D3DDECLUSAGE_COLOR; PCUsageIndex = 1;
		break;
	case X_D3DVSDE_FOG: // 5
		DbgVshPrintf("D3DVSDE_FOG");
		PCRegisterType = D3DDECLUSAGE_FOG;
		break;
	case X_D3DVSDE_POINTSIZE: // 6
		DbgVshPrintf("D3DVDSE_POINTSIZE");
		PCRegisterType = D3DDECLUSAGE_PSIZE;
		break;
	case X_D3DVSDE_BACKDIFFUSE: // 7
		DbgVshPrintf("D3DVSDE_BACKDIFFUSE /* xbox ext. */");
		PCRegisterType = D3DDECLUSAGE_COLOR; PCUsageIndex = 2;
		break;
	case X_D3DVSDE_BACKSPECULAR: // 8
		DbgVshPrintf("D3DVSDE_BACKSPECULAR /* xbox ext. */");
		PCRegisterType = D3DDECLUSAGE_COLOR; PCUsageIndex = 3;
		break;
	case X_D3DVSDE_TEXCOORD0: // 9
		DbgVshPrintf("D3DVSDE_TEXCOORD0");
		PCRegisterType = D3DDECLUSAGE_TEXCOORD; PCUsageIndex = 0;
		break;
	case X_D3DVSDE_TEXCOORD1: // 10
		DbgVshPrintf("D3DVSDE_TEXCOORD1");
		PCRegisterType = D3DDECLUSAGE_TEXCOORD; PCUsageIndex = 1;
		break;
	case X_D3DVSDE_TEXCOORD2: // 11
		DbgVshPrintf("D3DVSDE_TEXCOORD2");
		PCRegisterType = D3DDECLUSAGE_TEXCOORD; PCUsageIndex = 2;
		break;
	case X_D3DVSDE_TEXCOORD3: // 12
		DbgVshPrintf("D3DVSDE_TEXCOORD3");
		PCRegisterType = D3DDECLUSAGE_TEXCOORD; PCUsageIndex = 3;
		break;
	default:
		DbgVshPrintf("%d /* unknown register */", VertexRegister);
		PCRegisterType = D3DDECLUSAGE_UNSUPPORTED;
		break;
	}

    return PCRegisterType;
}

static inline DWORD VshGetTokenType(DWORD Token)
{
    return (Token & X_D3DVSD_TOKENTYPEMASK) >> X_D3DVSD_TOKENTYPESHIFT;
}

static inline DWORD VshGetVertexRegister(DWORD Token)
{
    return (Token & X_D3DVSD_VERTEXREGMASK) >> X_D3DVSD_VERTEXREGSHIFT;
}

static inline DWORD VshGetVertexRegisterIn(DWORD Token)
{
    return (Token & X_D3DVSD_VERTEXREGINMASK) >> X_D3DVSD_VERTEXREGINSHIFT;
}

static inline DWORD VshGetVertexStream(DWORD Token)
{
    return (Token & X_D3DVSD_STREAMNUMBERMASK) >> X_D3DVSD_STREAMNUMBERSHIFT;
}

static void VshConvertToken_NOP(
	DWORD *pToken,
	XTL::D3DVERTEXELEMENT *&pRecompiled
)
{
    // D3DVSD_NOP
    if(*pToken != DEF_VSH_NOP)
    {
        EmuLog(LOG_LEVEL::WARNING, "Token NOP found, but extra parameters are given!");
    }
    DbgVshPrintf("\tD3DVSD_NOP(),\n");
}

static DWORD VshConvertToken_CONSTMEM(
	DWORD *pToken,
	XTL::D3DVERTEXELEMENT *&pRecompiled
)
{
    using namespace XTL;

    // D3DVSD_CONST
    DbgVshPrintf("\tD3DVSD_CONST(");

    XTL::DWORD ConstantAddress = (*pToken & X_D3DVSD_CONSTADDRESSMASK) >> X_D3DVSD_CONSTADDRESSSHIFT;
    XTL::DWORD Count           = (*pToken & X_D3DVSD_CONSTCOUNTMASK) >> X_D3DVSD_CONSTCOUNTSHIFT;
    DbgVshPrintf("%d, %d),\n", ConstantAddress, Count);

	// TODO

    return Count * 4 + 1;
}

static void VshConvertToken_TESSELATOR(
	DWORD *pToken,
	XTL::D3DVERTEXELEMENT *&pRecompiled,
	boolean IsFixedFunction
)
{
    using namespace XTL;
	DWORD Index;

    // TODO: Investigate why Xb2PCRegisterType is only used for fixed function vertex shaders
    if(*pToken & X_D3DVSD_MASK_TESSUV)
    {
        XTL::DWORD VertexRegister    = VshGetVertexRegister(*pToken);
        XTL::DWORD NewVertexRegister = VertexRegister;

        DbgVshPrintf("\tD3DVSD_TESSUV(");
		NewVertexRegister = Xb2PCRegisterType(VertexRegister, IsFixedFunction, Index);
        DbgVshPrintf("),\n");

		// TODO : Expand on the setting of this TESSUV register element :
		pRecompiled->Usage = D3DDECLUSAGE(NewVertexRegister);
		pRecompiled->UsageIndex = Xb2PCRegisterType(NewVertexRegister, IsFixedFunction, Index); // TODO : Get Index from Xb2PCRegisterType
	}
    // D3DVSD_TESSNORMAL
    else
    {
        XTL::DWORD VertexRegisterIn  = VshGetVertexRegisterIn(*pToken);
        XTL::DWORD VertexRegisterOut = VshGetVertexRegister(*pToken);

        XTL::DWORD NewVertexRegisterIn  = VertexRegisterIn;
        XTL::DWORD NewVertexRegisterOut = VertexRegisterOut;

        DbgVshPrintf("\tD3DVSD_TESSNORMAL(");
        NewVertexRegisterIn = Xb2PCRegisterType(VertexRegisterIn, IsFixedFunction, Index);
        DbgVshPrintf(", ");
		// TODO : Expand on the setting of this TESSNORMAL input register element :
		pRecompiled->Usage = D3DDECLUSAGE(NewVertexRegisterIn);
		pRecompiled->UsageIndex = 0; // TODO : Get Index from Xb2PCRegisterType
        NewVertexRegisterOut = Xb2PCRegisterType(VertexRegisterOut, IsFixedFunction, Index);
        DbgVshPrintf("),\n");

		// TODO : Expand on the setting of this TESSNORMAL output register element :
		pRecompiled++;
		pRecompiled->Usage = D3DDECLUSAGE(NewVertexRegisterOut);
		pRecompiled->UsageIndex = 0; // TODO : Get Index from Xb2PCRegisterType
	}
}

static void VshEndPreviousStreamPatch(CxbxVertexShaderPatch *pPatchData)
{
    if(pPatchData->pCurrentVertexShaderStreamInfo) {
		DbgVshPrintf("\t// NeedPatching: %d\n", pPatchData->pCurrentVertexShaderStreamInfo->NeedPatch);
    }
}

static void VshConvertToken_STREAM(
	DWORD          *pToken,
	XTL::D3DVERTEXELEMENT *&pRecompiled,
	CxbxVertexShaderPatch *pPatchData
)
{
    using namespace XTL;

    // D3DVSD_STREAM_TESS
    if(*pToken & X_D3DVSD_STREAMTESSMASK)
    {
        DbgVshPrintf("\tD3DVSD_STREAM_TESS(),\n");
    }
    // D3DVSD_STREAM
    else
    {
		VshEndPreviousStreamPatch(pPatchData);

        XTL::DWORD StreamNumber = VshGetVertexStream(*pToken);

        // new stream
		pPatchData->pCurrentVertexShaderStreamInfo = &(pPatchData->pVertexShaderInfoToSet->VertexStreams[StreamNumber]);
		pPatchData->pCurrentVertexShaderStreamInfo->NeedPatch = FALSE;
		pPatchData->pCurrentVertexShaderStreamInfo->DeclPosition = FALSE;
		pPatchData->pCurrentVertexShaderStreamInfo->CurrentStreamNumber = 0;
		pPatchData->pCurrentVertexShaderStreamInfo->HostVertexStride = 0;
		pPatchData->pCurrentVertexShaderStreamInfo->NumberOfVertexElements = 0;

		// Dxbx note : Use Dophin(s), FieldRender, MatrixPaletteSkinning and PersistDisplay as a testcase

        DbgVshPrintf("\tD3DVSD_STREAM(%u),\n", StreamNumber);

		pPatchData->pCurrentVertexShaderStreamInfo->CurrentStreamNumber = VshGetVertexStream(*pToken);
		pPatchData->pVertexShaderInfoToSet->NumberOfVertexStreams++;
		// TODO : Keep a bitmask for all StreamNumber's seen?
    }
}

static void VshConvertToken_STREAMDATA_SKIP(
	DWORD *pToken,
	XTL::D3DVERTEXELEMENT *&pRecompiled,
	CxbxVertexShaderPatch *pPatchData
)
{
    using namespace XTL;

    XTL::DWORD SkipCount = (*pToken & X_D3DVSD_SKIPCOUNTMASK) >> X_D3DVSD_SKIPCOUNTSHIFT;
    DbgVshPrintf("\tD3DVSD_SKIP(%d),\n", SkipCount);
	pPatchData->pCurrentVertexShaderStreamInfo->HostVertexStride += (SkipCount * sizeof(DWORD));
}

static void VshConvertToken_STREAMDATA_SKIPBYTES(
	DWORD *pToken,
	XTL::D3DVERTEXELEMENT *&pRecompiled,
	CxbxVertexShaderPatch *pPatchData
)
{
    using namespace XTL;

    XTL::DWORD SkipBytesCount = (*pToken & X_D3DVSD_SKIPCOUNTMASK) >> X_D3DVSD_SKIPCOUNTSHIFT;

    DbgVshPrintf("\tD3DVSD_SKIPBYTES(%d), /* xbox ext. */\n", SkipBytesCount);
    if (SkipBytesCount % sizeof(XTL::DWORD)) {
        EmuLog(LOG_LEVEL::WARNING, "D3DVSD_SKIPBYTES can't be converted to D3DVSD_SKIP, not divisble by 4.");
    }

	pPatchData->pCurrentVertexShaderStreamInfo->HostVertexStride += SkipBytesCount;
}

static void VshConvertToken_STREAMDATA_REG(
	DWORD *pToken,
	XTL::D3DVERTEXELEMENT *&pRecompiled,
	boolean IsFixedFunction,
	CxbxVertexShaderPatch *pPatchData
)
{
	using namespace XTL;

	extern XTL::D3DCAPS g_D3DCaps;

	XTL::DWORD VertexRegister = VshGetVertexRegister(*pToken);
	XTL::DWORD HostVertexRegister;
	XTL::BOOL NeedPatching = FALSE;
	XTL::DWORD Index;

	DbgVshPrintf("\t\tD3DVSD_REG(");
	DWORD XboxVertexRegister = Xb2PCRegisterType(VertexRegister, IsFixedFunction, Index);
	HostVertexRegister = XboxVertexRegister;
	DbgVshPrintf(", ");

	XTL::DWORD XboxVertexElementDataType = (*pToken & X_D3DVSD_DATATYPEMASK) >> X_D3DVSD_DATATYPESHIFT;
	XTL::DWORD HostVertexElementDataType = 0;
	XTL::DWORD HostVertexElementByteSize = 0;

	switch (XboxVertexElementDataType)
	{
	case X_D3DVSDT_FLOAT1: // 0x12:
		DbgVshPrintf("D3DVSDT_FLOAT1");
		HostVertexElementDataType = D3DDECLTYPE_FLOAT1;
		HostVertexElementByteSize = 1 * sizeof(FLOAT);
		break;
	case X_D3DVSDT_FLOAT2: // 0x22:
		DbgVshPrintf("D3DVSDT_FLOAT2");
		HostVertexElementDataType = D3DDECLTYPE_FLOAT2;
		HostVertexElementByteSize = 2 * sizeof(FLOAT);
		//HostVertexRegister = D3DDECLUSAGE_TEXCOORD;	
		break;
	case X_D3DVSDT_FLOAT3: // 0x32:
		DbgVshPrintf("D3DVSDT_FLOAT3");
		HostVertexElementDataType = D3DDECLTYPE_FLOAT3;
		HostVertexElementByteSize = 3 * sizeof(FLOAT);

		/*
		if (pPatchData->pCurrentVertexShaderStreamInfo->DeclPosition) {
			pPatchData->pCurrentVertexShaderStreamInfo->DeclPosition = true;
			HostVertexRegister = D3DDECLUSAGE_POSITION;
		} else {
			HostVertexRegister = D3DDECLUSAGE_NORMAL;
		} */
		break;
	case X_D3DVSDT_FLOAT4: // 0x42:
		DbgVshPrintf("D3DVSDT_FLOAT4");
		HostVertexElementDataType = D3DDECLTYPE_FLOAT4;
		HostVertexElementByteSize = 4 * sizeof(FLOAT);
		//HostVertexRegister = D3DDECLUSAGE_COLOR;
		break;
	case X_D3DVSDT_D3DCOLOR: // 0x40:
		DbgVshPrintf("D3DVSDT_D3DCOLOR");
		HostVertexElementDataType = D3DDECLTYPE_D3DCOLOR;
		HostVertexElementByteSize = 1 * sizeof(D3DCOLOR);
		//HostVertexRegister = D3DDECLUSAGE_COLOR;
		break;
	case X_D3DVSDT_SHORT2: // 0x25:
		DbgVshPrintf("D3DVSDT_SHORT2");
		HostVertexElementDataType = D3DDECLTYPE_SHORT2;
		HostVertexElementByteSize = 2 * sizeof(XTL::SHORT);
		break;
	case X_D3DVSDT_SHORT4: // 0x45:
		DbgVshPrintf("D3DVSDT_SHORT4");
		HostVertexElementDataType = D3DDECLTYPE_SHORT4;
		HostVertexElementByteSize = 4 * sizeof(XTL::SHORT);
		break;
	case X_D3DVSDT_NORMSHORT1: // 0x11:
		DbgVshPrintf("D3DVSDT_NORMSHORT1 /* xbox ext. */");
		if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT2N) {
			HostVertexElementDataType = D3DDECLTYPE_SHORT2N;
			HostVertexElementByteSize = 2 * sizeof(SHORT);
		}
		else
		{
			HostVertexElementDataType = D3DDECLTYPE_FLOAT1;
			HostVertexElementByteSize = 1 * sizeof(FLOAT);
		}
		NeedPatching = TRUE;
		break;
	case X_D3DVSDT_NORMSHORT2: // 0x21:
		if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT2N) {
			DbgVshPrintf("D3DVSDT_NORMSHORT2");
			HostVertexElementDataType = D3DDECLTYPE_SHORT2N;
			HostVertexElementByteSize = 2 * sizeof(SHORT);
			// No need for patching in D3D9
		}
		else
		{
			DbgVshPrintf("D3DVSDT_NORMSHORT2 /* xbox ext. */");
			HostVertexElementDataType = D3DDECLTYPE_FLOAT2;
			HostVertexElementByteSize = 2 * sizeof(FLOAT);
			NeedPatching = TRUE;
		}
		break;
	case X_D3DVSDT_NORMSHORT3: // 0x31:
		DbgVshPrintf("D3DVSDT_NORMSHORT3 /* xbox ext. */");
		if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT4N) {
			HostVertexElementDataType = D3DDECLTYPE_SHORT4N;
			HostVertexElementByteSize = 4 * sizeof(SHORT);
		}
		else
		{
			HostVertexElementDataType = D3DDECLTYPE_FLOAT3;
			HostVertexElementByteSize = 3 * sizeof(FLOAT);
		}
		NeedPatching = TRUE;
		break;
	case X_D3DVSDT_NORMSHORT4: // 0x41:
		if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT4N) {
			DbgVshPrintf("D3DVSDT_NORMSHORT4");
			HostVertexElementDataType = D3DDECLTYPE_SHORT4N;
			HostVertexElementByteSize = 4 * sizeof(SHORT);
			// No need for patching in D3D9
		}
		else
		{
			DbgVshPrintf("D3DVSDT_NORMSHORT4 /* xbox ext. */");
			HostVertexElementDataType = D3DDECLTYPE_FLOAT4;
			HostVertexElementByteSize = 4 * sizeof(FLOAT);
			NeedPatching = TRUE;
		}
		break;
	case X_D3DVSDT_NORMPACKED3: // 0x16:
		DbgVshPrintf("D3DVSDT_NORMPACKED3 /* xbox ext. */");
		HostVertexElementDataType = D3DDECLTYPE_FLOAT3;
		HostVertexElementByteSize = 3 * sizeof(FLOAT);
		NeedPatching = TRUE;
		break;
	case X_D3DVSDT_SHORT1: // 0x15:
		DbgVshPrintf("D3DVSDT_SHORT1 /* xbox ext. */");
		HostVertexElementDataType = D3DDECLTYPE_SHORT2;
		HostVertexElementByteSize = 2 * sizeof(XTL::SHORT);
		NeedPatching = TRUE;
		break;
	case X_D3DVSDT_SHORT3: // 0x35:
		DbgVshPrintf("D3DVSDT_SHORT3 /* xbox ext. */");
		HostVertexElementDataType = D3DDECLTYPE_SHORT4;
		HostVertexElementByteSize = 4 * sizeof(XTL::SHORT);
		NeedPatching = TRUE;
		break;
	case X_D3DVSDT_PBYTE1: // 0x14:
		DbgVshPrintf("D3DVSDT_PBYTE1 /* xbox ext. */");
		if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
			HostVertexElementDataType = D3DDECLTYPE_UBYTE4N;
			HostVertexElementByteSize = 4 * sizeof(BYTE);
		}
		else
		{
			HostVertexElementDataType = D3DDECLTYPE_FLOAT1;
			HostVertexElementByteSize = 1 * sizeof(FLOAT);
		}
		NeedPatching = TRUE;
		break;
	case X_D3DVSDT_PBYTE2: // 0x24:
		DbgVshPrintf("D3DVSDT_PBYTE2 /* xbox ext. */");
		if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
			HostVertexElementDataType = D3DDECLTYPE_UBYTE4N;
			HostVertexElementByteSize = 4 * sizeof(BYTE);
		}
		else
		{
			HostVertexElementDataType = D3DDECLTYPE_FLOAT2;
			HostVertexElementByteSize = 2 * sizeof(FLOAT);
		}
		NeedPatching = TRUE;
		break;
	case X_D3DVSDT_PBYTE3: // 0x34:
		DbgVshPrintf("D3DVSDT_PBYTE3 /* xbox ext. */");
		if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
			HostVertexElementDataType = D3DDECLTYPE_UBYTE4N;
			HostVertexElementByteSize = 4 * sizeof(BYTE);
		}
		else
		{
			HostVertexElementDataType = D3DDECLTYPE_FLOAT3;
			HostVertexElementByteSize = 3 * sizeof(FLOAT);
		}
		NeedPatching = TRUE;
		break;
	case X_D3DVSDT_PBYTE4: // 0x44:
		// Test-case : Panzer
		if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
			DbgVshPrintf("D3DVSDT_PBYTE4");
			HostVertexElementDataType = D3DDECLTYPE_UBYTE4N;
			HostVertexElementByteSize = 4 * sizeof(BYTE);
			// No need for patching when D3D9 supports D3DDECLTYPE_UBYTE4N
		}
		else
		{
			DbgVshPrintf("D3DVSDT_PBYTE4 /* xbox ext. */");
			HostVertexElementDataType = D3DDECLTYPE_FLOAT4;
			HostVertexElementByteSize = 4 * sizeof(FLOAT);
			NeedPatching = TRUE;
		}
		break;
	case X_D3DVSDT_FLOAT2H: // 0x72:
		DbgVshPrintf("D3DVSDT_FLOAT2H /* xbox ext. */");
		HostVertexElementDataType = D3DDECLTYPE_FLOAT4;
		HostVertexElementByteSize = 4 * sizeof(FLOAT);
		NeedPatching = TRUE;
		break;
	case X_D3DVSDT_NONE: // 0x02:
		DbgVshPrintf("D3DVSDT_NONE /* xbox ext. */");
		HostVertexElementDataType = D3DDECLTYPE_UNUSED;
		// NeedPatching = TRUE; // TODO : This seems to cause regressions?
		break;
	default:
		DbgVshPrintf("Unknown data type for D3DVSD_REG: 0x%02X\n", XboxVertexElementDataType);
		break;
	}

	// save patching information
	XTL::CxbxVertexShaderStreamElement *pCurrentElement = &(pPatchData->pCurrentVertexShaderStreamInfo->VertexElements[pPatchData->pCurrentVertexShaderStreamInfo->NumberOfVertexElements]);
	pCurrentElement->XboxType = XboxVertexElementDataType;
	pCurrentElement->HostByteSize = HostVertexElementByteSize;
	pPatchData->pCurrentVertexShaderStreamInfo->NumberOfVertexElements++;
	pPatchData->pCurrentVertexShaderStreamInfo->NeedPatch |= NeedPatching;

	pRecompiled->Stream = pPatchData->pCurrentVertexShaderStreamInfo->CurrentStreamNumber;
	pRecompiled->Offset = pPatchData->pCurrentVertexShaderStreamInfo->HostVertexStride;
	pRecompiled->Type = HostVertexElementDataType;
	pRecompiled->Method = D3DDECLMETHOD_DEFAULT;
	pRecompiled->Usage = HostVertexRegister;
	pRecompiled->UsageIndex = Index;

	// If the xbox and host register number and usage differ, store an override!
	if (XboxVertexRegister != HostVertexRegister) {
		RegVDeclUsage[XboxVertexRegister] = HostVertexRegister;
	}

	pRecompiled++;

    pPatchData->pCurrentVertexShaderStreamInfo->HostVertexStride += HostVertexElementByteSize;

    DbgVshPrintf("),\n");

    if(HostVertexElementDataType == D3DDECLTYPE_UNUSED)
    {
        EmuLog(LOG_LEVEL::WARNING, "/* WARNING: Fatal type mismatch, no fitting type! */");
    }
}

static void VshConvertToken_STREAMDATA(
	DWORD          *pToken,
	XTL::D3DVERTEXELEMENT *&pRecompiled,
	boolean         IsFixedFunction,
	CxbxVertexShaderPatch *pPatchData
)
{
    using namespace XTL;
	if (*pToken & X_D3DVSD_MASK_SKIP)
	{
		// For D3D9, use D3DDECLTYPE_UNUSED ?
		if (*pToken & X_D3DVSD_MASK_SKIPBYTES) {
			VshConvertToken_STREAMDATA_SKIPBYTES(pToken, pRecompiled, pPatchData);
		} else {
			VshConvertToken_STREAMDATA_SKIP(pToken, pRecompiled, pPatchData);
		}
	}
	else // D3DVSD_REG
    {
        VshConvertToken_STREAMDATA_REG(pToken, pRecompiled, IsFixedFunction, pPatchData);
    }
}

static DWORD VshRecompileToken(
	DWORD          *pToken,
	XTL::D3DVERTEXELEMENT *&pRecompiled,
	boolean         IsFixedFunction,
	CxbxVertexShaderPatch *pPatchData
)
{
    using namespace XTL;

    XTL::DWORD Step = 1;

    switch(VshGetTokenType(*pToken))
    {
    case X_D3DVSD_TOKEN_NOP:
        VshConvertToken_NOP(pToken, pRecompiled);
        break;
    case X_D3DVSD_TOKEN_STREAM:
    {
        VshConvertToken_STREAM(pToken, pRecompiled, pPatchData);
        break;
    }
    case X_D3DVSD_TOKEN_STREAMDATA:
    {
        VshConvertToken_STREAMDATA(pToken, pRecompiled, IsFixedFunction, pPatchData);
        break;
    }
    case X_D3DVSD_TOKEN_TESSELLATOR:
    {
        VshConvertToken_TESSELATOR(pToken, pRecompiled, IsFixedFunction);
        break;
    }
    case X_D3DVSD_TOKEN_CONSTMEM:
    {
        Step = VshConvertToken_CONSTMEM(pToken, pRecompiled);
        break;
    }
    default:
        DbgVshPrintf("Unknown token type: %d\n", VshGetTokenType(*pToken));
        break;
    }

    return Step;
}

DWORD XTL::EmuRecompileVshDeclaration
(
    DWORD                *pDeclaration,
	D3DVERTEXELEMENT    **ppRecompiledDeclaration,
    DWORD                *pOriginalDeclarationCount,
	DWORD                *pHostDeclarationSize,
    boolean               IsFixedFunction,
    CxbxVertexShaderInfo *pVertexShaderInfo
)
{
	RegVDeclUsage.fill(-1);

    // First of all some info:
    // We have to figure out which flags are set and then
    // we have to patch their params

    // some token values
    // 0xFFFFFFFF - end of the declaration
    // 0x00000000 - nop (means that this value is ignored)

    // Calculate size of declaration
    DWORD DeclarationCount = VshGetDeclarationCount(pDeclaration);
	// For Direct3D9, we need to reserve at least twice the number of elements, as one token can generate two registers (in and out) :
	DWORD HostDeclarationSize = DeclarationCount * sizeof(D3DVERTEXELEMENT) * 2;
	*pOriginalDeclarationCount = DeclarationCount;
	*pHostDeclarationSize = HostDeclarationSize;
	
	D3DVERTEXELEMENT *pRecompiled = (D3DVERTEXELEMENT *)malloc(HostDeclarationSize);
	memset(pRecompiled, 0, HostDeclarationSize);

	uint8_t *pRecompiledBufferOverflow = ((uint8_t*)pRecompiled) + HostDeclarationSize;
    *ppRecompiledDeclaration = pRecompiled;

	CxbxVertexShaderPatch PatchData = { 0 };
	PatchData.pVertexShaderInfoToSet = pVertexShaderInfo;

    DbgVshPrintf("DWORD dwVSHDecl[] =\n{\n");

    while (*pDeclaration != DEF_VSH_END)
    {
		if ((uint8*)pRecompiled >= pRecompiledBufferOverflow) {
			DbgVshPrintf("Detected buffer-overflow, breaking out...\n");
			break;
		}

        DWORD Step = VshRecompileToken(pDeclaration, pRecompiled, IsFixedFunction, &PatchData);
		pDeclaration += Step;
		//pRecompiled += Step;
	}

	*pRecompiled = D3DDECL_END();

	VshEndPreviousStreamPatch(&PatchData);
    DbgVshPrintf("\tD3DVSD_END()\n};\n");

    DbgVshPrintf("// NbrStreams: %d\n", PatchData.pVertexShaderInfoToSet->NumberOfVertexStreams);

    return D3D_OK;
}

extern XTL::D3DCAPS g_D3DCaps;

// recompile xbox vertex shader function
extern HRESULT XTL::EmuRecompileVshFunction
(
    DWORD        *pFunction,
    LPD3DXBUFFER *ppRecompiled,
    DWORD        *pOriginalSize,
    boolean      bNoReservedConstants,
	boolean		 *pbUseDeclarationOnly,
	DWORD		 *pRecompiledDeclaration,
    DWORD        DeclarationSize
)
{
    const DWORD temporaryCount = g_D3DCaps.VS20Caps.NumTemps;

    VSH_SHADER_HEADER   *pShaderHeader = (VSH_SHADER_HEADER*)pFunction;
    DWORD               *pToken;
    boolean             EOI = false;
    VSH_XBOX_SHADER     *pShader = (VSH_XBOX_SHADER*)calloc(1, sizeof(VSH_XBOX_SHADER));
	LPD3DXBUFFER		pErrors = NULL;
    HRESULT             hRet = 0;

	// Build an array of registers that are declared
	// This is used to remove instructions that haven't been declared
	// as they cause CreateVertexShader to fail
	bool declaredRegisters[VSH_MAX_TEMPORARY_REGISTERS] = { false };
	DWORD* pDeclToken = pRecompiledDeclaration;
    DWORD* pDeclEnd = (DWORD*)((BYTE*)pDeclToken + DeclarationSize);
	do {
		DWORD regNum = *pDeclToken & X_D3DVSD_VERTEXREGMASK;
		if (regNum >= temporaryCount /*12*/) {
			// Lego Star Wars hits this
			LOG_TEST_CASE("RegNum > 12");
			pDeclToken++;
			continue;
		}

		declaredRegisters[regNum] = true;
		pDeclToken++;
	} while (pDeclToken < pDeclEnd && *pDeclToken != X_D3DVSD_END());

    // TODO: support this situation..
    if(pFunction == NULL)
        return E_FAIL;

    *ppRecompiled = NULL;
    *pOriginalSize = 0;
	*pbUseDeclarationOnly = 0;

    if(!pShader)
    {
        EmuLog(LOG_LEVEL::WARNING, "Couldn't allocate memory for vertex shader conversion buffer");
        return E_OUTOFMEMORY;
    }
    pShader->ShaderHeader = *pShaderHeader;
    switch(pShaderHeader->Version)
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
            EmuLog(LOG_LEVEL::WARNING, "Unknown vertex shader version 0x%02X", pShaderHeader->Version);
            hRet = E_FAIL;
            break;
    }

    if(SUCCEEDED(hRet))
    {
		RegVUsage.fill(false);

        for (pToken = (DWORD*)((uint08*)pFunction + sizeof(VSH_SHADER_HEADER)); !EOI; pToken += VSH_INSTRUCTION_SIZE)
        {
            VSH_SHADER_INSTRUCTION Inst;

            VshParseInstruction(pToken, &Inst);
            VshConvertToIntermediate(&Inst, pShader);
            EOI = (boolean)VshGetField(pToken, FLD_FINAL);
        }

        // The size of the shader is
        *pOriginalSize = (DWORD)pToken - (DWORD)pFunction;

		// Do not attempt to compile empty shaders
		if (pShader->IntermediateCount == 0) {
			EmuLog(LOG_LEVEL::WARNING, "Skipped empty Vertex Shader");
			return STATUS_INVALID_PARAMETER;
		}

        char* pShaderDisassembly = (char*)malloc(pShader->IntermediateCount * 100); // Should be plenty
		DbgVshPrintf("-- Before conversion --\n");
        VshWriteShader(pShader, pShaderDisassembly, (XTL::D3DVERTEXELEMENT9*)pRecompiledDeclaration, FALSE);
		DbgVshPrintf("%s", pShaderDisassembly);
		DbgVshPrintf("-----------------------\n");

        VshConvertShader(pShader, bNoReservedConstants, declaredRegisters);
        VshWriteShader(pShader, pShaderDisassembly, (XTL::D3DVERTEXELEMENT9*)pRecompiledDeclaration, TRUE);

		DbgVshPrintf("-- After conversion ---\n");
		DbgVshPrintf("%s", pShaderDisassembly);
		DbgVshPrintf("-----------------------\n");

        // HACK: Azurik. Prevent Direct3D from trying to assemble this.
		if(!strcmp(pShaderDisassembly, "vs.2.x\n"))
		{
			EmuLog(LOG_LEVEL::WARNING, "Replacing empty vertex shader with fallback");

			static const char dummy[] =
				"vs.2.x\n"
				"dp4 oPos.x, v0, c96\n"
				"dp4 oPos.y, v0, c97\n"
				"dp4 oPos.z, v0, c98\n"
				"dp4 oPos.w, v0, c99\n";

			hRet = D3DXAssembleShader(
				dummy,
				strlen(dummy),
				/*pDefines=*/nullptr,
				/*pInclude=*/nullptr,
				/*Flags=*/0, // Was D3DXASM_SKIPVALIDATION,
				/*ppCompiledShader=*/ppRecompiled,
				/*ppCompilationErrors*/nullptr);
		}
		else
		{
			hRet = D3DXAssembleShader(
				pShaderDisassembly,
				strlen(pShaderDisassembly),
				/*pDefines=*/nullptr,
				/*pInclude=*/nullptr,
				/*Flags=*/0, // Was D3DXASM_SKIPVALIDATION,
				/*ppCompiledShader=*/ppRecompiled,
				/*ppCompilationErrors*/&pErrors);
		}

        if (FAILED(hRet))
        {
            EmuLog(LOG_LEVEL::WARNING, "Couldn't assemble recompiled vertex shader");
			EmuLog(LOG_LEVEL::WARNING, "%s", pErrors->GetBufferPointer());
        }

		if( pErrors )
			pErrors->Release();

        free(pShaderDisassembly);
    }

    free(pShader);

    return hRet;
}

extern void XTL::FreeVertexDynamicPatch(CxbxVertexShader *pVertexShader)
{
    pVertexShader->VertexShaderInfo.NumberOfVertexStreams = 0;
}

// Checks for failed vertex shaders, and shaders that would need patching
boolean VshHandleIsValidShader(DWORD Handle)
{
#if 0
	//printf( "VS = 0x%.08X\n", Handle );

    XTL::CxbxVertexShader *pVertexShader = XTL::GetCxbxVertexShader(Handle);
    if (pVertexShader) {
        if (pVertexShader->Status != 0)
        {
            return FALSE;
        }
        /*
        for (uint32 i = 0; i < pVertexShader->VertexShaderInfo.NumberOfVertexStreams; i++)
        {
            if (pVertexShader->VertexShaderInfo.VertexStreams[i].NeedPatch)
            {
                // Just for caching purposes
                pVertexShader->Status = 0x80000001;
                return FALSE;
            }
        }
        */
    }
#endif
    return TRUE;
}

extern boolean XTL::IsValidCurrentShader(void)
{
	// Dxbx addition : There's no need to call
	// XTL_EmuIDirect3DDevice_GetVertexShader, just check g_CurrentXboxVertexShaderHandle :
	return VshHandleIsValidShader(g_CurrentXboxVertexShaderHandle);
}

XTL::CxbxVertexShaderInfo *GetCxbxVertexShaderInfo(DWORD Handle)
{
    XTL::CxbxVertexShader *pVertexShader = XTL::GetCxbxVertexShader(Handle);

    for (uint32 i = 0; i < pVertexShader->VertexShaderInfo.NumberOfVertexStreams; i++)
    {
        if (pVertexShader->VertexShaderInfo.VertexStreams[i].NeedPatch)
        {
            return &pVertexShader->VertexShaderInfo;
        }
    }
    return NULL;
}

std::unordered_map<DWORD, XTL::CxbxVertexShader*> g_CxbxVertexShaders;

XTL::CxbxVertexShader* XTL::GetCxbxVertexShader(DWORD Handle)
{
	if (VshHandleIsVertexShader(Handle)) {
		auto it = g_CxbxVertexShaders.find(Handle);
		if (it != g_CxbxVertexShaders.end()) {
			return it->second;
		}
	}

	return nullptr;
}

void XTL::SetCxbxVertexShader(DWORD Handle, XTL::CxbxVertexShader* shader)
{
	auto it = g_CxbxVertexShaders.find(Handle);
	if (it != g_CxbxVertexShaders.end() && it->second != nullptr && shader != nullptr) {
		LOG_TEST_CASE("Overwriting existing Vertex Shader");
	}

	g_CxbxVertexShaders[Handle] = shader;
}
