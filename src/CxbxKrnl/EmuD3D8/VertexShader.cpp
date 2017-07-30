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
// *   Cxbx->Win32->CxbxKrnl->EmuD3D8->VertexShader.cpp
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
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

#include "CxbxKrnl/CxbxKrnl.h"
#include "CxbxKrnl/Emu.h"
#include "CxbxKrnl/EmuFS.h"
#include "CxbxKrnl/EmuAlloc.h"
#include "CxbxKrnl/EmuXTL.h"
#include "CxbxKrnl/MemoryManager.h"
#include "CxbxKrnl/EmuD3D8Types.h" // For X_D3DVSDE_*

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
    "mov", // really "arl" Dxbx note : Alias for 'mov a0.x'
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
    "exp",
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

// Dxbx note : This tooling function is never used, but clearly illustrates the relation
// between vertex shader's being passed around, and the actual handle value used on PC.
DWORD VshHandleGetRealHandle(DWORD aHandle)
{
	using namespace XTL;

	if (VshHandleIsVertexShader(aHandle))
	{
		X_D3DVertexShader *pD3DVertexShader = VshHandleGetVertexShader(aHandle);
		// assert(pD3DVertexShader);

		VERTEX_SHADER *pVertexShader = (VERTEX_SHADER*)(pD3DVertexShader->Handle);
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
        EmuWarning("Invalid instruction, parameter A type unknown %d", pInstruction->A.ParameterType);
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

static void VshWriteShader(VSH_XBOX_SHADER *pShader,
                           char* pDisassembly,
                           boolean Truncate)
{
    uint32 DisassemblyPos = 0;
    switch(pShader->ShaderHeader.Version)
    {
        case VERSION_VS:
            DisassemblyPos += sprintf(pDisassembly + DisassemblyPos, "vs.1.1\n");
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
    for (int i = 0; i < pShader->IntermediateCount && (i < 128 || !Truncate); i++)
    {
        VSH_INTERMEDIATE_FORMAT *pIntermediate = &pShader->Intermediate[i];

        if(i == 128)
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
        for (int i = 0; i < 3; i++)
        {
            VSH_IMD_PARAMETER *pParameter = &pIntermediate->Parameters[i];
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

/*
// Dxbx addition : Scalar instructions reading from W should read from X instead
boolean DxbxFixupScalarParameter(VSH_SHADER_INSTRUCTION *pInstruction,
	VSH_XBOX_SHADER *pShader,
	VSH_PARAMETER *pParameter)
{
	boolean Result;
	int i;
	boolean WIsWritten;

	// The DirectX vertex shader language specifies that the exp, log, rcc, rcp, and rsq instructions
	// all operate on the "w" component of the input. But the microcode versions of these instructions
	// actually operate on the "x" component of the input.
	Result = false;

	// Test if this is a scalar instruction :
	if (pInstruction->ILU in [ILU_RCP, ILU_RCC, ILU_RSQ, ILU_EXP, ILU_LOG])
	{
		// Test if this parameter reads all components, including W (TODO : Or should we fixup any W reading swizzle?) :
		if ((pParameter->Swizzle[0] = SWIZZLE_X)
			&& (pParameter->Swizzle[1] = SWIZZLE_Y)
			&& (pParameter->Swizzle[2] = SWIZZLE_Z)
			&& (pParameter->Swizzle[3] = SWIZZLE_W))
		{
			// Also test that the .W component is never written to before:
			WIsWritten = false;
			for (i = 0; i < pShader->IntermediateCount; i++)
			{
				// Stop when we reached this instruction :
				if (&(pShader->Intermediate[i]) == pInstruction)
					break;

				// Check if this instruction writes to the .W component of the same input parameter :
				if (((pShader->Intermediate[i].Output.Type == IMD_OUTPUT_C) && (pParameter->ParameterType == PARAM_C))
					|| ((pShader->Intermediate[i].Output.Type == IMD_OUTPUT_R) && (pParameter->ParameterType == PARAM_R)))
				{
					WIsWritten = (pShader->Intermediate[i].Output.Address == pParameter->Address)
						&& ((pShader->Intermediate[i].Output.Mask && MASK_W) > 0);
					if (WIsWritten)
						break;
				}
			}

			if (!WIsWritten)
			{
				// Change the read from W into a read from X (this fixes the XDK VolumeLight sample) :
				VshSetSwizzle(pParameter, SWIZZLE_X, SWIZZLE_X, SWIZZLE_X, SWIZZLE_X);
				DbgVshPrintf("Dxbx fixup on scalar instruction applied; Changed read of uninitialized W into a read of X!\n");
				Result = true;
			}
		}
	}

	return Result;
}
*/

static boolean VshAddInstructionILU_R(VSH_SHADER_INSTRUCTION *pInstruction,
                                      VSH_XBOX_SHADER        *pShader,
                                      boolean                IsCombined)
{
    VSH_INTERMEDIATE_FORMAT *pIntermediate;
    if(!HasILUR(pInstruction))
    {
        return FALSE;
    }

/* TODO
	// Dxbx note : Scalar instructions read from C, but use X instead of W, fix that :
	DxbxFixupScalarParameter(pInstruction, pShader, &pInstruction.C);
*/
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

static inline void VshSetSwizzle(VSH_IMD_PARAMETER *pParameter,
                                 VSH_SWIZZLE       x,
                                 VSH_SWIZZLE       y,
                                 VSH_SWIZZLE       z,
                                 VSH_SWIZZLE       w)
{
    pParameter->Parameter.Swizzle[0] = x;
    pParameter->Parameter.Swizzle[1] = y;
    pParameter->Parameter.Swizzle[2] = z;
    pParameter->Parameter.Swizzle[3] = w;
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
        EmuWarning("Applying screen space vertex shader patching hack!");
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

// Converts the intermediate format vertex shader to DirectX 8 format
static boolean VshConvertShader(VSH_XBOX_SHADER *pShader,
                                boolean         bNoReservedConstants)
{
    boolean RUsage[13] = { FALSE };
    // TODO: What about state shaders and such?
    pShader->ShaderHeader.Version = VERSION_VS;

    // Search for the screen space instructions, and remove them
    if(!bNoReservedConstants)
    {
        VshRemoveScreenSpaceInstructions(pShader);
    }

    // TODO: Add routine for compacting r register usage so that at least one is freed (two if dph and r12)

    for (int i = 0; i < pShader->IntermediateCount; i++)
    {
        VSH_INTERMEDIATE_FORMAT* pIntermediate = &pShader->Intermediate[i];
        // Combining not supported in vs.1.1
        pIntermediate->IsCombined = FALSE;

        /*
        if(pIntermediate->Output.Type == IMD_OUTPUT_O && pIntermediate->Output.Address == OREG_OFOG)
        {
            // The PC shader assembler doesn't like masks on scalar registers
            VshSetOutputMask(&pIntermediate->Output, TRUE, TRUE, TRUE, TRUE);
        }*/

        if(pIntermediate->InstructionType == IMD_ILU && pIntermediate->ILU == ILU_RCC)
        {
            // Convert rcc to rcp
            pIntermediate->ILU = ILU_RCP;
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

        for (int j = 0; j < 3; j++)
        {
            if(pIntermediate->Parameters[j].Active)
            {
                if(pIntermediate->Parameters[j].Parameter.ParameterType == PARAM_R)
                {
                    RUsage[pIntermediate->Parameters[j].Parameter.Address] = TRUE;
                }
                // Make constant registers range from 0 to 192 instead of -96 to 96
                if(pIntermediate->Parameters[j].Parameter.ParameterType == PARAM_C)
                {
					//if(pIntermediate->Parameters[j].Parameter.Address < 0)
						pIntermediate->Parameters[j].Parameter.Address += 96;
                }
            }
        }

        if(pIntermediate->InstructionType == IMD_MAC && pIntermediate->MAC == MAC_DPH)
        {
			// 2010/01/12 - revel8n - attempt to alleviate conversion issues relate to the dph instruction

            // Replace dph with dp3 and add
            if(pIntermediate->Output.Type != IMD_OUTPUT_R)
            {
                // TODO: Complete dph support
                EmuWarning("Can't simulate dph for other than output r registers (yet)");

				// attempt to find unused register...
				int outRegister = -1;
				for (int j = 11; j >= 0; --j)
				{
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
				//VshSetSwizzle(&TmpIntermediate.Parameters[0], SWIZZLE_W, SWIZZLE_W, SWIZZLE_W, SWIZZLE_W);
				VshSetSwizzle(&TmpIntermediate.Parameters[1], SWIZZLE_W, SWIZZLE_W, SWIZZLE_W, SWIZZLE_W);
				//VshSetOutputMask(&TmpIntermediate.Output, FALSE, FALSE, FALSE, TRUE);
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
    if(RUsage[12])
    {
        // Sigh, they absolutely had to use r12, didn't they?
        for (int i = 11; i >= 0; i--)
        {
            if(!RUsage[i])
            {
                R12Replacement = i;
                break;
            }
        }
        if(R12Replacement == -1)
        {
            EmuWarning("Vertex shader uses all r registers, including r12; impossible to convert!");
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

typedef struct _VSH_TYPE_PATCH_DATA
{
    DWORD NbrTypes;
    UINT  Types[256];
	UINT  NewSizes[256];
}
VSH_TYPE_PATCH_DATA;

typedef struct _VSH_STREAM_PATCH_DATA
{
    DWORD                     NbrStreams;
    XTL::STREAM_DYNAMIC_PATCH pStreamPatches[256];
}
VSH_STREAM_PATCH_DATA;

typedef struct _VSH_PATCH_DATA
{
    boolean              NeedPatching;
	WORD                 CurrentStreamNumber;
	DWORD                ConvertedStride;
    VSH_TYPE_PATCH_DATA  TypePatchData;
    VSH_STREAM_PATCH_DATA StreamPatchData;
}
VSH_PATCH_DATA;

// VERTEX SHADER
#define DEF_VSH_END 0xFFFFFFFF
#define DEF_VSH_NOP 0x00000000

static DWORD VshGetDeclarationSize(DWORD *pDeclaration)
{
    DWORD Pos = 0;
    while (*(pDeclaration + Pos) != DEF_VSH_END)
    {
        Pos++;
    }
    return (Pos + 1) * sizeof(DWORD);
}

typedef DWORD D3DDECLUSAGE;

#define D3DDECLUSAGE_UNSUPPORTED ((D3DDECLUSAGE)-1)

D3DDECLUSAGE Xb2PCRegisterType
(
	DWORD VertexRegister,
	boolean IsFixedFunction
)
{
	using namespace XTL;

	D3DDECLUSAGE PCRegisterType;
	// For fixed function vertex shaders, print D3DVSDE_*, for custom shaders print numbered registers.
	if (IsFixedFunction) {
		switch (VertexRegister)
		{
		case X_D3DVSDE_VERTEX: // -1
			DbgVshPrintf("D3DVSDE_VERTEX /* xbox ext. */");
			PCRegisterType = D3DDECLUSAGE_UNSUPPORTED;
			break;
		case X_D3DVSDE_POSITION: // 0
			DbgVshPrintf("D3DVSDE_POSITION");
			PCRegisterType = D3DVSDE_POSITION;
			break;
		case X_D3DVSDE_BLENDWEIGHT: // 1
			DbgVshPrintf("D3DVSDE_BLENDWEIGHT");
			PCRegisterType = D3DVSDE_BLENDWEIGHT;
			break;
		case X_D3DVSDE_NORMAL: // 2
			DbgVshPrintf("D3DVSDE_NORMAL");
			PCRegisterType = D3DVSDE_NORMAL;
			break;
		case X_D3DVSDE_DIFFUSE: // 3
			DbgVshPrintf("D3DVSDE_DIFFUSE");
			PCRegisterType = D3DVSDE_DIFFUSE;
			break;
		case X_D3DVSDE_SPECULAR: // 4
			DbgVshPrintf("D3DVSDE_SPECULAR");
			PCRegisterType = D3DVSDE_SPECULAR;
			break;
		case X_D3DVSDE_FOG: // 5
			DbgVshPrintf("D3DVSDE_FOG /* xbox ext. */");
			PCRegisterType = D3DDECLUSAGE_UNSUPPORTED;
			break;
		case X_D3DVSDE_BACKDIFFUSE: // 7
			DbgVshPrintf("D3DVSDE_BACKDIFFUSE /* xbox ext. */");
			PCRegisterType = D3DDECLUSAGE_UNSUPPORTED;
			break;
		case X_D3DVSDE_BACKSPECULAR: // 8
			DbgVshPrintf("D3DVSDE_BACKSPECULAR /* xbox ext. */");
			PCRegisterType = D3DDECLUSAGE_UNSUPPORTED;
			break;
		case X_D3DVSDE_TEXCOORD0: // 9
			DbgVshPrintf("D3DVSDE_TEXCOORD0");
			PCRegisterType = D3DVSDE_TEXCOORD0;
			break;
		case X_D3DVSDE_TEXCOORD1: // 10
			DbgVshPrintf("D3DVSDE_TEXCOORD1");
			PCRegisterType = D3DVSDE_TEXCOORD1;
			break;
		case X_D3DVSDE_TEXCOORD2: // 11
			DbgVshPrintf("D3DVSDE_TEXCOORD2");
			PCRegisterType = D3DVSDE_TEXCOORD2;
			break;
		case X_D3DVSDE_TEXCOORD3: // 12
			DbgVshPrintf("D3DVSDE_TEXCOORD3");
			PCRegisterType = D3DVSDE_TEXCOORD3;
			break;
		default:
			DbgVshPrintf("%d /* unknown register */", VertexRegister);
			PCRegisterType = D3DDECLUSAGE_UNSUPPORTED;
			break;
		}
	} else {
		PCRegisterType = (D3DDECLUSAGE)VertexRegister;
		DbgVshPrintf("%d", VertexRegister);
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

static void VshConvertToken_NOP(DWORD *pToken)
{
    // D3DVSD_NOP
    if(*pToken != DEF_VSH_NOP)
    {
        EmuWarning("Token NOP found, but extra parameters are given!");
    }
    DbgVshPrintf("\tD3DVSD_NOP(),\n");
}

static DWORD VshConvertToken_CONSTMEM(DWORD *pToken)
{
    // D3DVSD_CONST
    DbgVshPrintf("\tD3DVSD_CONST(");

    DWORD ConstantAddress = (*pToken & X_D3DVSD_CONSTADDRESSMASK) >> X_D3DVSD_CONSTADDRESSSHIFT;
    DWORD Count           = (*pToken & X_D3DVSD_CONSTCOUNTMASK) >> X_D3DVSD_CONSTCOUNTSHIFT;

    DbgVshPrintf("%d, %d),\n", ConstantAddress, Count);

    //pToken = D3DVSD_CONST(ConstantAddress, Count);

    for (uint i = 0; i < Count; i++)
    {
        DbgVshPrintf("\t0x%08X,\n", pToken);
    }
    return Count;
}

static void VshConverToken_TESSELATOR(DWORD   *pToken,
                                      boolean IsFixedFunction)
{
    using namespace XTL;

    // TODO: Investigate why Xb2PCRegisterType is only used for fixed function vertex shaders
    // D3DVSD_TESSUV
    if(*pToken & 0x10000000)
    {
        XTL::DWORD VertexRegister    = VshGetVertexRegister(*pToken);
        XTL::DWORD NewVertexRegister = VertexRegister;

        DbgVshPrintf("\tD3DVSD_TESSUV(");
		NewVertexRegister = Xb2PCRegisterType(VertexRegister, IsFixedFunction);
        DbgVshPrintf("),\n");

        *pToken = D3DVSD_TESSUV(NewVertexRegister);
    }
    // D3DVSD_TESSNORMAL
    else
    {
        XTL::DWORD VertexRegisterIn  = VshGetVertexRegisterIn(*pToken);
        XTL::DWORD VertexRegisterOut = VshGetVertexRegister(*pToken);

        XTL::DWORD NewVertexRegisterIn  = VertexRegisterIn;
        XTL::DWORD NewVertexRegisterOut = VertexRegisterOut;

        DbgVshPrintf("\tD3DVSD_TESSNORMAL(");
        NewVertexRegisterIn = Xb2PCRegisterType(VertexRegisterIn, IsFixedFunction);
        DbgVshPrintf(", ");
        NewVertexRegisterOut = Xb2PCRegisterType(VertexRegisterOut, IsFixedFunction);
        DbgVshPrintf("),\n");
        *pToken = D3DVSD_TESSNORMAL(NewVertexRegisterIn, NewVertexRegisterOut);
    }
}

static boolean VshAddStreamPatch(VSH_PATCH_DATA *pPatchData)
{
    int CurrentStream = pPatchData->StreamPatchData.NbrStreams - 1; // TODO : Should we use pPatchData->CurrentStreamNumber?

    if(CurrentStream >= 0)
    {
        DbgVshPrintf("NeedPatching: %d\n", pPatchData->NeedPatching);

        XTL::STREAM_DYNAMIC_PATCH* pStreamPatch = &pPatchData->StreamPatchData.pStreamPatches[CurrentStream];

        pStreamPatch->ConvertedStride = pPatchData->ConvertedStride;
        pStreamPatch->NbrTypes = pPatchData->TypePatchData.NbrTypes;
        pStreamPatch->NeedPatch = pPatchData->NeedPatching;
		// 2010/01/12 - revel8n - fixed allocated data size and type
        pStreamPatch->pTypes = (UINT *)malloc(pPatchData->TypePatchData.NbrTypes * sizeof(UINT)); //VSH_TYPE_PATCH_DATA));
        memcpy(pStreamPatch->pTypes, pPatchData->TypePatchData.Types, pPatchData->TypePatchData.NbrTypes * sizeof(UINT)); //VSH_TYPE_PATCH_DATA));
        // 2010/12/06 - PatrickvL - do the same for new sizes :
		pStreamPatch->pSizes = (UINT *)malloc(pPatchData->TypePatchData.NbrTypes * sizeof(UINT));
		memcpy(pStreamPatch->pSizes, pPatchData->TypePatchData.NewSizes, pPatchData->TypePatchData.NbrTypes * sizeof(UINT));

        return TRUE;
    }
    return FALSE;
}

static void VshConvertToken_STREAM(DWORD          *pToken,
                                   VSH_PATCH_DATA *pPatchData)
{
    // D3DVSD_STREAM_TESS
    if(*pToken & D3DVSD_STREAMTESSMASK)
    {
        DbgVshPrintf("\tD3DVSD_STREAM_TESS(),\n");
    }
    // D3DVSD_STREAM
    else
    {
        XTL::DWORD StreamNumber = VshGetVertexStream(*pToken);
        DbgVshPrintf("\tD3DVSD_STREAM(%d),\n", StreamNumber);

        // new stream
        // copy current data to structure
		// Dxbx note : Use Dophin(s), FieldRender, MatrixPaletteSkinning and PersistDisplay as a testcase
		if(VshAddStreamPatch(pPatchData))
        {
			// Reset fields for next patch :
			pPatchData->ConvertedStride = 0;
            pPatchData->TypePatchData.NbrTypes = 0;
            pPatchData->NeedPatching = FALSE;
			pPatchData->StreamPatchData.NbrStreams = 0; // Dxbx addition
		}

		pPatchData->CurrentStreamNumber = (WORD)StreamNumber;
		
		pPatchData->StreamPatchData.NbrStreams++;
    }
}

static void VshConvertToken_STREAMDATA_SKIP(DWORD *pToken)
{
    using namespace XTL;

    XTL::DWORD SkipCount = (*pToken & X_D3DVSD_SKIPCOUNTMASK) >> X_D3DVSD_SKIPCOUNTSHIFT;
    DbgVshPrintf("\tD3DVSD_SKIP(%d),\n", SkipCount);
}

static void VshConvertToken_STREAMDATA_SKIPBYTES(DWORD *pToken)
{
    using namespace XTL;

    XTL::DWORD SkipBytesCount = (*pToken & X_D3DVSD_SKIPCOUNTMASK) >> X_D3DVSD_SKIPCOUNTSHIFT;
    DbgVshPrintf("\tD3DVSD_SKIPBYTES(%d), /* xbox ext. */\n", SkipBytesCount);
    if(SkipBytesCount % sizeof(XTL::DWORD))
    {
        EmuWarning("D3DVSD_SKIPBYTES can't be converted to D3DVSD_SKIP, not divisble by 4.");
    }
    *pToken = D3DVSD_SKIP(SkipBytesCount / sizeof(XTL::DWORD));
}

static void VshConvertToken_STREAMDATA_REG(DWORD          *pToken,
                                           boolean         IsFixedFunction,
                                           VSH_PATCH_DATA *pPatchData)
{
    using namespace XTL;

    XTL::DWORD VertexRegister = VshGetVertexRegister(*pToken);
    XTL::DWORD NewVertexRegister;

    DbgVshPrintf("\tD3DVSD_REG(");
    NewVertexRegister = Xb2PCRegisterType(VertexRegister, IsFixedFunction);
    DbgVshPrintf(", ");

    XTL::DWORD DataType = (*pToken & X_D3DVSD_DATATYPEMASK) >> X_D3DVSD_DATATYPESHIFT;
    XTL::DWORD NewDataType = 0;
	XTL::DWORD NewSize = 0;

    switch(DataType)
    {
	case X_D3DVSDT_FLOAT1: // 0x12:
        DbgVshPrintf("D3DVSDT_FLOAT1");
        NewDataType = D3DVSDT_FLOAT1;
		NewSize = 1*sizeof(FLOAT);
        break;
	case X_D3DVSDT_FLOAT2: // 0x22:
        DbgVshPrintf("D3DVSDT_FLOAT2");
        NewDataType = D3DVSDT_FLOAT2;
		NewSize = 2*sizeof(FLOAT);
        break;
	case X_D3DVSDT_FLOAT3: // 0x32:
        DbgVshPrintf("D3DVSDT_FLOAT3");
        NewDataType = D3DVSDT_FLOAT3;
		NewSize = 3*sizeof(FLOAT);
        break;
	case X_D3DVSDT_FLOAT4: // 0x42:
        DbgVshPrintf("D3DVSDT_FLOAT4");
        NewDataType = D3DVSDT_FLOAT4;
		NewSize = 4*sizeof(FLOAT);
        break;
	case X_D3DVSDT_D3DCOLOR: // 0x40:
        DbgVshPrintf("D3DVSDT_D3DCOLOR");
        NewDataType = D3DVSDT_D3DCOLOR;
		NewSize = sizeof(D3DCOLOR);
        break;
	case X_D3DVSDT_SHORT2: // 0x25:
        DbgVshPrintf("D3DVSDT_SHORT2");
        NewDataType = D3DVSDT_SHORT2;
		NewSize = 2*sizeof(XTL::SHORT);
        break;
	case X_D3DVSDT_SHORT4: // 0x45:
        DbgVshPrintf("D3DVSDT_SHORT4");
        NewDataType = D3DVSDT_SHORT4;
		NewSize = 4*sizeof(XTL::SHORT);
        break;
	case X_D3DVSDT_NORMSHORT1: // 0x11:
        DbgVshPrintf("D3DVSDT_NORMSHORT1 /* xbox ext. */");
        NewDataType = D3DVSDT_FLOAT1; // TODO -oDxbx : Is it better to use D3DVSDT_NORMSHORT2 in Direct3D9 ?
		NewSize = sizeof(FLOAT);
        pPatchData->NeedPatching = TRUE;
        break;
	case X_D3DVSDT_NORMSHORT2: // 0x21:
        DbgVshPrintf("D3DVSDT_NORMSHORT2 /* xbox ext. */");
        NewDataType = D3DVSDT_FLOAT2;
		NewSize = 2*sizeof(FLOAT);
        pPatchData->NeedPatching = TRUE;
        break;
	case X_D3DVSDT_NORMSHORT3: // 0x31:
        DbgVshPrintf("D3DVSDT_NORMSHORT3 /* xbox ext. nsp */");
        NewDataType = D3DVSDT_FLOAT3; // TODO -oDxbx : Is it better to use D3DVSDT_NORMSHORT4 in Direct3D9 ?
		NewSize = 3*sizeof(FLOAT);
        pPatchData->NeedPatching = TRUE;
        break;
	case X_D3DVSDT_NORMSHORT4: // 0x41:
        DbgVshPrintf("D3DVSDT_NORMSHORT4 /* xbox ext. */");
        NewDataType = D3DVSDT_FLOAT4;
		NewSize = 4*sizeof(FLOAT);
        pPatchData->NeedPatching = TRUE;
        break;
	case X_D3DVSDT_NORMPACKED3: // 0x16:
        DbgVshPrintf("D3DVSDT_NORMPACKED3 /* xbox ext. nsp */");
        NewDataType = D3DVSDT_FLOAT3;
		NewSize = 3*sizeof(FLOAT);
        pPatchData->NeedPatching = TRUE;
        break;
	case X_D3DVSDT_SHORT1: // 0x15:
        DbgVshPrintf("D3DVSDT_SHORT1 /* xbox ext. nsp */");
        NewDataType = D3DVSDT_SHORT2;
		NewSize = 2*sizeof(XTL::SHORT);
        pPatchData->NeedPatching = TRUE;
        break;
	case X_D3DVSDT_SHORT3: // 0x35:
        DbgVshPrintf("D3DVSDT_SHORT3 /* xbox ext. nsp */");
        NewDataType = D3DVSDT_SHORT4;
		NewSize = 4*sizeof(XTL::SHORT);
        pPatchData->NeedPatching = TRUE;
        break;
	case X_D3DVSDT_PBYTE1: // 0x14:
        DbgVshPrintf("D3DVSDT_PBYTE1 /* xbox ext. nsp */");
        NewDataType = D3DVSDT_FLOAT1; // TODO -oDxbx : Is it better to use D3DVSDT_NORMSHORT2 in Direct3D9 ?
		NewSize = 1*sizeof(FLOAT);
        pPatchData->NeedPatching = TRUE;
        break;
	case X_D3DVSDT_PBYTE2: // 0x24:
        DbgVshPrintf("D3DVSDT_PBYTE2 /* xbox ext. nsp */");
        NewDataType = D3DVSDT_FLOAT2; // TODO -oDxbx : Is it better to use D3DVSDT_NORMSHORT2 in Direct3D9 ?
		NewSize = 2*sizeof(FLOAT);
        pPatchData->NeedPatching = TRUE;
        break;
	case X_D3DVSDT_PBYTE3: // 0x34:
        DbgVshPrintf("D3DVSDT_PBYTE3 /* xbox ext. nsp */");
        NewDataType = D3DVSDT_FLOAT3; // TODO -oDxbx : Is it better to use D3DVSDT_NORMSHORT4 in Direct3D9 ?
		NewSize = 3*sizeof(FLOAT);
        pPatchData->NeedPatching = TRUE;
        break;
	case X_D3DVSDT_PBYTE4: // 0x44: // Hit by Panzer
        DbgVshPrintf("D3DVSDT_PBYTE4 /* xbox ext. */");
        NewDataType = D3DVSDT_FLOAT4; // TODO -oDxbx : Is it better to use D3DVSDT_NORMSHORT4 or D3DDECLTYPE_UBYTE4N (if in caps) in Direct3D9 ?
		NewSize = 4*sizeof(FLOAT);
		pPatchData->NeedPatching = TRUE;
		break;
	case X_D3DVSDT_FLOAT2H: // 0x72:
        DbgVshPrintf("D3DVSDT_FLOAT2H /* xbox ext. */");
        NewDataType = D3DVSDT_FLOAT4;
		NewSize = 4*sizeof(FLOAT);
        pPatchData->NeedPatching = TRUE;
        break;
	case X_D3DVSDT_NONE: // 0x02:
        DbgVshPrintf("D3DVSDT_NONE /* xbox ext. nsp */");
#if DXBX_USE_D3D9
		NewDataType = D3DVSDT_NONE;
#endif
	    // TODO -oDxbx: Use D3DVSD_NOP ?
        NewDataType = 0xFF;
        break;
    default:
        DbgVshPrintf("Unknown data type for D3DVSD_REG: 0x%02X\n", DataType);
        break;
    }

	// save patching information
	pPatchData->TypePatchData.Types[pPatchData->TypePatchData.NbrTypes] = DataType;
	pPatchData->TypePatchData.NewSizes[pPatchData->TypePatchData.NbrTypes] = NewSize;
	pPatchData->TypePatchData.NbrTypes++;

    *pToken = D3DVSD_REG(NewVertexRegister, NewDataType);

    pPatchData->ConvertedStride += NewSize;

    DbgVshPrintf("),\n");

    if(NewDataType == 0xFF)
    {
        EmuWarning("/* WARNING: Fatal type mismatch, no fitting type! */");
    }
}

#define D3DVSD_MASK_TESSUV 0x10000000
#define D3DVSD_MASK_SKIP 0x10000000 // Skips (normally) dwords
#define D3DVSD_MASK_SKIPBYTES 0x08000000 // Skips bytes (no, really?!)


static void VshConvertToken_STREAMDATA(DWORD          *pToken,
                                       boolean         IsFixedFunction,
                                       VSH_PATCH_DATA *pPatchData)
{
    using namespace XTL;
	if (*pToken & D3DVSD_MASK_SKIP)
	{
		// For D3D9, use D3DDECLTYPE_UNUSED ?
		if (*pToken & D3DVSD_MASK_SKIPBYTES) {
			VshConvertToken_STREAMDATA_SKIPBYTES(pToken);
		} else {
			VshConvertToken_STREAMDATA_SKIP(pToken);
		}
	}
	else // D3DVSD_REG
    {
        VshConvertToken_STREAMDATA_REG(pToken, IsFixedFunction, pPatchData);
    }
}

static DWORD VshRecompileToken(DWORD          *pToken,
                               boolean         IsFixedFunction,
                               VSH_PATCH_DATA *pPatchData)
{
    using namespace XTL;

    XTL::DWORD Step = 1;

    switch(VshGetTokenType(*pToken))
    {
    case D3DVSD_TOKEN_NOP:
        VshConvertToken_NOP(pToken);
        break;
    case D3DVSD_TOKEN_STREAM:
    {
        VshConvertToken_STREAM(pToken, pPatchData);
        break;
    }
    case D3DVSD_TOKEN_STREAMDATA:
    {
        VshConvertToken_STREAMDATA(pToken, IsFixedFunction, pPatchData);
        break;
    }
    case D3DVSD_TOKEN_TESSELLATOR:
    {
        VshConverToken_TESSELATOR(pToken, IsFixedFunction);
        break;
    }
    case D3DVSD_TOKEN_CONSTMEM:
    {
        Step = VshConvertToken_CONSTMEM(pToken);
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
    DWORD               **ppRecompiledDeclaration,
    DWORD                *pDeclarationSize,
    boolean               IsFixedFunction,
    VERTEX_DYNAMIC_PATCH *pVertexDynamicPatch
)
{
    // First of all some info:
    // We have to figure out which flags are set and then
    // we have to patch their params

    // some token values
    // 0xFFFFFFFF - end of the declaration
    // 0x00000000 - nop (means that this value is ignored)

    // Calculate size of declaration
    DWORD DeclarationSize = VshGetDeclarationSize(pDeclaration);
    *ppRecompiledDeclaration = (DWORD *)malloc(DeclarationSize);
    DWORD *pRecompiled = *ppRecompiledDeclaration;
    memcpy(pRecompiled, pDeclaration, DeclarationSize);
    *pDeclarationSize = DeclarationSize;

    // TODO: Put these in one struct
    VSH_PATCH_DATA       PatchData = { 0 };

    DbgVshPrintf("DWORD dwVSHDecl[] =\n{\n");

    while (*pRecompiled != DEF_VSH_END)
    {
        DWORD Step = VshRecompileToken(pRecompiled, IsFixedFunction, &PatchData);
        pRecompiled += Step;
    }

	// copy last current data to structure
	VshAddStreamPatch(&PatchData);
    DbgVshPrintf("\tD3DVSD_END()\n};\n");

    DbgVshPrintf("NbrStreams: %d\n", PatchData.StreamPatchData.NbrStreams);

    // Copy the patches to the vertex shader struct
    DWORD StreamsSize = PatchData.StreamPatchData.NbrStreams * sizeof(STREAM_DYNAMIC_PATCH);
    pVertexDynamicPatch->NbrStreams = PatchData.StreamPatchData.NbrStreams;
    pVertexDynamicPatch->pStreamPatches = (STREAM_DYNAMIC_PATCH *)malloc(StreamsSize);
    memcpy(pVertexDynamicPatch->pStreamPatches,
           PatchData.StreamPatchData.pStreamPatches,
           StreamsSize);

    return D3D_OK;
}

// recompile xbox vertex shader function
extern HRESULT XTL::EmuRecompileVshFunction
(
    DWORD        *pFunction,
    LPD3DXBUFFER *ppRecompiled,
    DWORD        *pOriginalSize,
    boolean      bNoReservedConstants,
	boolean		 *pbUseDeclarationOnly
)
{
    VSH_SHADER_HEADER   *pShaderHeader = (VSH_SHADER_HEADER*)pFunction;
    DWORD               *pToken;
    boolean             EOI = false;
    VSH_XBOX_SHADER     *pShader = (VSH_XBOX_SHADER*)calloc(1, sizeof(VSH_XBOX_SHADER));
	LPD3DXBUFFER		pErrors = NULL;
    HRESULT             hRet = 0;

    // TODO: support this situation..
    if(pFunction == NULL)
        return E_FAIL;

    *ppRecompiled = NULL;
    *pOriginalSize = 0;
	*pbUseDeclarationOnly = 0;

    if(!pShader)
    {
        EmuWarning("Couldn't allocate memory for vertex shader conversion buffer");
        hRet = E_OUTOFMEMORY;
    }
    pShader->ShaderHeader = *pShaderHeader;
    switch(pShaderHeader->Version)
    {
        case VERSION_XVS:
            break;
        case VERSION_XVSS:
            EmuWarning("Might not support vertex state shaders?");
            hRet = E_FAIL;
            break;
        case VERSION_XVSW:
            EmuWarning("Might not support vertex read/write shaders?");
            hRet = E_FAIL;
            break;
        default:
            EmuWarning("Unknown vertex shader version 0x%02X", pShaderHeader->Version);
            hRet = E_FAIL;
            break;
    }

    if(SUCCEEDED(hRet))
    {

        for (pToken = (DWORD*)((uint08*)pFunction + sizeof(VSH_SHADER_HEADER)); !EOI; pToken += VSH_INSTRUCTION_SIZE)
        {
            VSH_SHADER_INSTRUCTION Inst;

            VshParseInstruction(pToken, &Inst);
            VshConvertToIntermediate(&Inst, pShader);
            EOI = (boolean)VshGetField(pToken, FLD_FINAL);
        }

        // The size of the shader is
        *pOriginalSize = (DWORD)pToken - (DWORD)pFunction;

        char* pShaderDisassembly = (char*)malloc(pShader->IntermediateCount * 100); // Should be plenty
        DbgVshPrintf("-- Before conversion --\n");
        VshWriteShader(pShader, pShaderDisassembly, FALSE);
        DbgVshPrintf("%s", pShaderDisassembly);
        DbgVshPrintf("-----------------------\n");

        VshConvertShader(pShader, bNoReservedConstants);
        VshWriteShader(pShader, pShaderDisassembly, TRUE);

        DbgVshPrintf("-- After conversion ---\n");
        DbgVshPrintf("%s", pShaderDisassembly);
        DbgVshPrintf("-----------------------\n");

        // HACK: Azurik. Prevent Direct3D from trying to assemble this.
		if(!strcmp(pShaderDisassembly, "vs.1.1\n"))
		{
			EmuWarning("Replacing empty vertex shader with fallback");

			static const char dummy[] =
				"vs.1.1\n"
				"mov oPos, v0\n";

			hRet = D3DXAssembleShader(dummy,
				strlen(dummy),
				D3DXASM_SKIPVALIDATION,
				NULL,
				ppRecompiled,
				NULL);
		}
		else
		{

			hRet = D3DXAssembleShader(pShaderDisassembly,
                                  strlen(pShaderDisassembly),
                                  D3DXASM_SKIPVALIDATION,
                                  NULL,
                                  ppRecompiled,
                                  &pErrors);
		}

        if (FAILED(hRet))
        {
            EmuWarning("Couldn't assemble recompiled vertex shader");
			EmuWarning("%s", pErrors->GetBufferPointer());
        }

		if( pErrors )
			pErrors->Release();

        free(pShaderDisassembly);
    }

    free(pShader);

    return hRet;
}

extern void XTL::FreeVertexDynamicPatch(VERTEX_SHADER *pVertexShader)
{
    for (DWORD i = 0; i < pVertexShader->VertexDynamicPatch.NbrStreams; i++)
    {
        free(pVertexShader->VertexDynamicPatch.pStreamPatches[i].pTypes);
		pVertexShader->VertexDynamicPatch.pStreamPatches[i].pTypes = nullptr;
		free(pVertexShader->VertexDynamicPatch.pStreamPatches[i].pSizes);
		pVertexShader->VertexDynamicPatch.pStreamPatches[i].pSizes = nullptr;
    }

    free(pVertexShader->VertexDynamicPatch.pStreamPatches);
    pVertexShader->VertexDynamicPatch.pStreamPatches = NULL;
    pVertexShader->VertexDynamicPatch.NbrStreams = 0;
}

extern boolean XTL::IsValidCurrentShader(void)
{
	// Dxbx addition : There's no need to call
	// XTL_EmuIDirect3DDevice_GetVertexShader, just check g_CurrentVertexShader :
	return VshHandleIsValidShader(g_CurrentVertexShader);
}

// Checks for failed vertex shaders, and shaders that would need patching
boolean XTL::VshHandleIsValidShader(DWORD Handle)
{
	//printf( "VS = 0x%.08X\n", Handle );

    if (VshHandleIsVertexShader(Handle))
    {
        X_D3DVertexShader *pD3DVertexShader = VshHandleGetVertexShader(Handle);
        VERTEX_SHADER *pVertexShader = (VERTEX_SHADER *)pD3DVertexShader->Handle;
        if (pVertexShader->Status != 0)
        {
            return FALSE;
        }
        /*
        for (uint32 i = 0; i < pVertexShader->VertexDynamicPatch.NbrStreams; i++)
        {
            if (pVertexShader->VertexDynamicPatch.pStreamPatches[i].NeedPatch)
            {
                // Just for caching purposes
                pVertexShader->Status = 0x80000001;
                return FALSE;
            }
        }
        */
    }
    return TRUE;
}

extern XTL::VERTEX_DYNAMIC_PATCH *XTL::VshGetVertexDynamicPatch(DWORD Handle)
{
    X_D3DVertexShader *pD3DVertexShader = VshHandleGetVertexShader(Handle);
    VERTEX_SHADER *pVertexShader = (VERTEX_SHADER *)pD3DVertexShader->Handle;

    for (uint32 i = 0; i < pVertexShader->VertexDynamicPatch.NbrStreams; i++)
    {
        if (pVertexShader->VertexDynamicPatch.pStreamPatches[i].NeedPatch)
        {
            return &pVertexShader->VertexDynamicPatch;
        }
    }
    return NULL;
}