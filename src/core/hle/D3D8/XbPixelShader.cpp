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
// *  (c) 2002-2003 kingofc <kingofc@freenet.de>
// *  2020 PatrickvL
// *
// *  All rights reserved
// *
// ******************************************************************

/*

  This is a parser for the Xbox Pixel Shader Format.

  With the help of this parser it is possible to generate
  Direct3D pixel shader assembly code.

*/

#define LOG_PREFIX CXBXR_MODULE::PXSH

#include "core\kernel\support\Emu.h"
#include "core\hle\D3D8\Direct3D9\Direct3D9.h" // For g_pD3DDevice, g_pXbox_PixelShader
#include "core\hle\D3D8\XbPixelShader.h"
#include "core\hle\D3D8\Direct3D9\PixelShader.h" // EmuCompilePixelShader
#include "core\hle\D3D8\XbD3D8Logging.h" // For D3DErrorString()

#include "core\kernel\init\CxbxKrnl.h" // For CxbxrAbort()
#include "util\hasher.h"
#include "core\hle\D3D8\Direct3D9\FixedFunctionPixelShader.hlsli"
#include "common/FilePaths.hpp" // For szFilePath_CxbxReloaded_Exe

#include <assert.h> // assert()
#include <process.h>
#include <locale.h>
#include <fstream>
#include <sstream>

#include "Direct3D9\RenderStates.h" // For XboxRenderStateConverter
#include "Direct3D9\TextureStates.h" // For XboxTextureStateConverter

#include <wrl/client.h>

extern XboxRenderStateConverter XboxRenderStates; // Declared in Direct3D9.cpp
extern XboxTextureStateConverter XboxTextureStates; // Declared in Direct3D9.cpp


#define DbgPshPrintf \
	LOG_CHECK_ENABLED(LOG_LEVEL::DEBUG) \
		if(g_bPrintfOn) printf

// Note : On a hardware level, there are only 4 pixel shaders instructions present in the Nvidia NV2A GPU :
// - xdd (dot/dot/discard) > calculating AB=A.B and CD=C.D
// - xdm (dot/mul/discard) > calculating AB=A.B and CD=C*D
// - xmmc (mul/mul/mux)    > calculating AB=A*B and CD=C*D and Mux=AB?CD
// - xmma (mul/mul/sum)    > calculating AB=A*B and CD=C*D and Sum=AB+CD
// (One of the implications is, that once a dot-product is issued, no Sum or Mux operation is possible.)
// All other instructions (mov, add, sub, mul, lrp, dp3) are compiled into one of these 4 using varying arguments.
// All 4 instruction specify up to three output registers, all of which must be unique (or be discarded).
//
// Apart from the r0,r1 and t0-t3 registers, the NV2A allows writing to the v0,v1 (this conflicts with PS.1.3!)
//
// The precision of registers is also different; On the Xbox, all 4 color components (RGBA) for constant registers
// range from 0.0 to 1.0 (with 8 bits of precision), while all other registers (r, t and v) range from -1.0 to 1.0.
//
// This is different from native PS.1.3 in which constant registers suddenly have a range -1.0 to 1.0, but vertex
// registers (v0 and v1) range from 0.0 to 1.0 instead, and the temporary and texture registers have a range
// from negative 'MaxPixelShaderValue' to positive 'MaxPixelShaderValue', which value must at least be 1.0
// (but depending on hardware capabilities can be higher).
//
// TODO : Correct emulation should correct these differences; The range of constant-registers must be converted
// from 0.0-1.0 to -1.0-1.0, and vertex-registers must be converted from -1.0..1.0 to 0.0..1.0 (if anything like
// that is at all possible!)
//
// register | Xbox range | Native range | Xbox      | Native    |
//  C0..C8  |  0.0 - 1.0 !   -1.0 - 1.0 | readonly  | readonly  |
//  R0..R1  | -1.0 - 1.0 |   -1.0 - 1.0 | writeable | writeable |
//  T0..T3  | -1.0 - 1.0 |   -1.0 - 1.0 | writeable | writeable |
//  V0..V1  | -1.0 - 1.0 !    0.0 - 1.0 | writeable ! readonly  |
//
// "-C0_bias_x2" shifts range from [ 0..1] to [-1..1]
// "-V0_bias_d2" shifts range from [-1..1] to [ 0..1]

/* RPSRegisterObject */

void RPSRegisterObject::Decode(uint8_t Value)
{
	Reg = (PS_REGISTER)(Value & PS_REGISTER_MASK); // = 0x0f

	// Validate correctness (see NOTE below)
	if (Reg == 6) LOG_TEST_CASE("Unknown PS_REGISTER : 6");
	if (Reg == 7) LOG_TEST_CASE("Unknown PS_REGISTER : 7");
}

/* RPSInputRegister */

void RPSInputRegister::Decode(uint8_t Value, unsigned stage_nr, bool isRGB)
{
	RPSRegisterObject::Decode(Value);

	Channel = (PS_CHANNEL)(Value & PS_CHANNEL_MASK); // = 0x10
	InputMapping = (PS_INPUTMAPPING)(Value & PS_INPUTMAPPING_MASK); // = 0xe0

	if (stage_nr == 9) {
		// In final combiner stage, convert C0 into FC0, and C1 into FC1, to discern them as separate registers
		if (Reg == PS_REGISTER_C0) Reg = PS_REGISTER_FC0;
		if (Reg == PS_REGISTER_C1) Reg = PS_REGISTER_FC1;
	}

	// Validate correctness (see NOTE below)
	if (stage_nr <= xbox::X_PSH_COMBINECOUNT) {
		if (Reg == PS_REGISTER_FOG) {
			if (!isRGB) LOG_TEST_CASE("PS_REGISTER_FOG input not allowed in Alpha register combiner");
			else if (Channel == PS_CHANNEL_ALPHA) LOG_TEST_CASE("PS_REGISTER_FOG.a input not allowed in RGB register combiner");
		}
		if (Reg == PS_REGISTER_V1R0_SUM) LOG_TEST_CASE("PS_REGISTER_V1R0_SUM input only allowed in final combiner");
		if (Reg == PS_REGISTER_EF_PROD) LOG_TEST_CASE("PS_REGISTER_EF_PROD input only allowed in final combiner");
	}
	else { // final combiner
		if (InputMapping == PS_INPUTMAPPING_EXPAND_NORMAL) LOG_TEST_CASE("PS_INPUTMAPPING_EXPAND_NORMAL not allowed in final combiner");
		if (InputMapping == PS_INPUTMAPPING_EXPAND_NEGATE) LOG_TEST_CASE("PS_INPUTMAPPING_EXPAND_NEGATE not allowed in final combiner");
		if (InputMapping == PS_INPUTMAPPING_HALFBIAS_NORMAL) LOG_TEST_CASE("PS_INPUTMAPPING_HALFBIAS_NORMAL not allowed in final combiner");
		if (InputMapping == PS_INPUTMAPPING_HALFBIAS_NEGATE) LOG_TEST_CASE("PS_INPUTMAPPING_HALFBIAS_NEGATE not allowed in final combiner");
		if (InputMapping == PS_INPUTMAPPING_SIGNED_IDENTITY) LOG_TEST_CASE("PS_INPUTMAPPING_SIGNED_IDENTITY not allowed in final combiner");
		if (InputMapping == PS_INPUTMAPPING_SIGNED_NEGATE) LOG_TEST_CASE("PS_INPUTMAPPING_SIGNED_NEGATE not allowed in final combiner");
	}
}

/* RPSCombinerOutput */

void RPSCombinerOutput::Decode(uint8_t Value, uint16_t PSInputs, unsigned stage_nr, bool isRGB)
{
	RPSRegisterObject::Decode(Value);

	// Decode PSAlphaInputs / PSRGBInputs :
	Input[0].Decode((uint8_t)(PSInputs >> 8), stage_nr, isRGB);
	Input[1].Decode((uint8_t)(PSInputs >> 0), stage_nr, isRGB);

	// Validate correctness (see NOTE below)
	if (Reg == PS_REGISTER_C0) LOG_TEST_CASE("PS_REGISTER_C0 not allowed as output");
	if (Reg == PS_REGISTER_C1) LOG_TEST_CASE("PS_REGISTER_C1 not allowed as output");
	if (Reg == PS_REGISTER_FOG) LOG_TEST_CASE("PS_REGISTER_FOG not allowed as output");
	if (Reg == PS_REGISTER_V1R0_SUM) LOG_TEST_CASE("PS_REGISTER_V1R0_SUM not allowed as output");
	if (Reg == PS_REGISTER_EF_PROD) LOG_TEST_CASE("PS_REGISTER_EF_PROD not allowed as output");
}

/* RPSCombinerStageChannel */

void RPSCombinerStageChannel::Decode(uint32_t PSInputs, uint32_t PSOutputs, unsigned stage_nr, bool isRGB)
{
	// Decode PSAlphaOutputs / PSRGBOutputs and PSAlphaInputs / PSRGBInputs :
	OutputCD.Decode((uint8_t)(PSOutputs >> 0), (uint16_t)(PSInputs >> 0 ), stage_nr, isRGB);
	OutputAB.Decode((uint8_t)(PSOutputs >> 4), (uint16_t)(PSInputs >> 16), stage_nr, isRGB);
	OutputMUX_SUM.Decode((uint8_t)(PSOutputs >> 8));

	// Get the combiner output flags :
	PS_COMBINEROUTPUT CombinerOutputFlags = (PS_COMBINEROUTPUT)(PSOutputs >> 12);

	// Decompose the combiner output flags :
	OutputCD.DotProduct = (CombinerOutputFlags & PS_COMBINEROUTPUT_CD_DOT_PRODUCT) > 0; // False=Multiply, True=DotProduct
	OutputAB.DotProduct = (CombinerOutputFlags & PS_COMBINEROUTPUT_AB_DOT_PRODUCT) > 0; // False=Multiply, True=DotProduct
	AB_CD_MUX = (CombinerOutputFlags & PS_COMBINEROUTPUT_AB_CD_MUX) > 0; // False=AB+CD, True=MUX(AB,CD) based on R0.a
	CombinerOutputMapping = (PS_COMBINEROUTPUT_OUTPUTMAPPING)(CombinerOutputFlags & PS_COMBINEROUTPUT_OUTPUTMAPPING_MASK); // = 0x38
	OutputCD.BlueToAlpha = (CombinerOutputFlags & PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA) >> 6; // 0=Alpha-to-Alpha, 1=Blue-to-Alpha
	OutputAB.BlueToAlpha = (CombinerOutputFlags & PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA) >> 7; // 0=Alpha-to-Alpha, 1=Blue-to-Alpha

	// Discover test-cases (see TODO below)
	// Check for 'discard-all-outputs'
	if (OutputAB.DotProduct || OutputCD.DotProduct) {
		if ((OutputAB.Reg == PS_REGISTER_DISCARD) && (OutputCD.Reg == PS_REGISTER_DISCARD)) LOG_TEST_CASE("All two outputs discarded");
	} else {
		// if ((OutputAB.Reg == PS_REGISTER_DISCARD) && (OutputCD.Reg == PS_REGISTER_DISCARD) && (OutputMUX_SUM.Reg == PS_REGISTER_DISCARD)) LOG_TEST_CASE("All three outputs discarded"); // Test-case : XDK sample : Minnaert (on Stage2.Alpha)
	}

	// Validate correctness (see NOTE below)
	if ((PSOutputs & ~0x000FFFFF) > 0) LOG_TEST_CASE("Unknown PS_COMBINEROUTPUT flag bits detected");
	if (CombinerOutputMapping == PS_COMBINEROUTPUT_OUTPUTMAPPING_SHIFTLEFT_2_BIAS) LOG_TEST_CASE("PS_COMBINEROUTPUT_SHIFTLEFT_2_BIAS unsupported on NV2A?");
	if (CombinerOutputMapping == PS_COMBINEROUTPUT_OUTPUTMAPPING_SHIFTRIGHT_1_BIAS) LOG_TEST_CASE("PS_COMBINEROUTPUT_SHIFTRIGHT_1_BIAS unsupported on NV2A?");
	if (isRGB) {
		if (OutputMUX_SUM.Reg != PS_REGISTER_DISCARD) {
			if (OutputCD.DotProduct) LOG_TEST_CASE("PS_COMBINEROUTPUT_CD_DOT_PRODUCT detected without PS_REGISTER_DISCARD in MUX_SUM");
			if (OutputAB.DotProduct) LOG_TEST_CASE("PS_COMBINEROUTPUT_AB_DOT_PRODUCT detected without PS_REGISTER_DISCARD in MUX_SUM");
		}
		if (OutputCD.DotProduct) if (!OutputAB.DotProduct) LOG_TEST_CASE("PS_COMBINEROUTPUT_CD_DOT_PRODUCT detected without PS_COMBINEROUTPUT_AB_DOT_PRODUCT (so, xmd 'opcode')"); // Need test-cases  Note : Undefined xmd (mul/dot) *is* supported in CxbxPixelShaderTemplate.hlsl
	} else { // DotProduct and BlueToAlpha are not valid for Alpha
		if (OutputCD.DotProduct)  LOG_TEST_CASE("PS_COMBINEROUTPUT_CD_DOT_PRODUCT shouldn't be set for Alpha");
		if (OutputAB.DotProduct)  LOG_TEST_CASE("PS_COMBINEROUTPUT_AB_DOT_PRODUCT shouldn't be set for Alpha");
		if (OutputCD.BlueToAlpha) LOG_TEST_CASE("PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA shouldn't be set for Alpha");
		if (OutputAB.BlueToAlpha) LOG_TEST_CASE("PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA shouldn't be set for Alpha");
	}
}

/* RPSFinalCombiner */

void RPSFinalCombiner::Decode(const uint32_t PSFinalCombinerInputsABCD, const uint32_t PSFinalCombinerInputsEFG)
{
	Input[0].Decode((uint8_t)(PSFinalCombinerInputsABCD >> 24), /*stage_nr=*/9, /*isRGB=*/true);
	Input[1].Decode((uint8_t)(PSFinalCombinerInputsABCD >> 16), /*stage_nr=*/9, /*isRGB=*/true);
	Input[2].Decode((uint8_t)(PSFinalCombinerInputsABCD >>  8), /*stage_nr=*/9, /*isRGB=*/true);
	Input[3].Decode((uint8_t)(PSFinalCombinerInputsABCD >>  0), /*stage_nr=*/9, /*isRGB=*/true);
	Input[4].Decode((uint8_t)(PSFinalCombinerInputsEFG  >> 24), /*stage_nr=*/9, /*isRGB=*/true);
	Input[5].Decode((uint8_t)(PSFinalCombinerInputsEFG  >> 16), /*stage_nr=*/9, /*isRGB=*/true);
	Input[6].Decode((uint8_t)(PSFinalCombinerInputsEFG  >>  8), /*stage_nr=*/9, /*isRGB=*/false); // Note : Final combiner input G must be a single component, and must thus be decoded as Alpha
	PS_FINALCOMBINERSETTING FinalCombinerSettingFlags = (PS_FINALCOMBINERSETTING)((PSFinalCombinerInputsEFG >> 0) & 0xFF);

	ComplementV1 = (FinalCombinerSettingFlags & PS_FINALCOMBINERSETTING_COMPLEMENT_V1) > 0;
	ComplementR0 = (FinalCombinerSettingFlags & PS_FINALCOMBINERSETTING_COMPLEMENT_R0) > 0;
	ClampSum = (FinalCombinerSettingFlags & PS_FINALCOMBINERSETTING_CLAMP_SUM) > 0;

	// Discover test-cases (see TODO below)
	// if (Input[0].Channel != PS_CHANNEL_ALPHA) LOG_TEST_CASE("PS_CHANNEL_RGB/PS_CHANNEL_BLUE detected on final combiner A input"); // Note : test-case ModifyPixelShader uses PS_REGISTER_FOG.rgb and seems to expect .rgb handling (not PS_CHANNEL_BLUE's .b)
	if (Input[4].Channel == PS_CHANNEL_ALPHA) LOG_TEST_CASE("PS_CHANNEL_ALPHA detected on final combiner E input"); // Need test-case to determine how this should behave (calculating EF_PROD) : .aaa instead of .rgb?
	if (Input[5].Channel == PS_CHANNEL_ALPHA) LOG_TEST_CASE("PS_CHANNEL_ALPHA detected on final combiner F input"); // Need test-case to determine how this should behave (calculating EF_PROD) : .aaa instead of .rgb?
	// if (Input[6].Channel == PS_CHANNEL_BLUE) LOG_TEST_CASE("PS_CHANNEL_ALPHA detected on final combiner G input"); // PS_CHANNEL_BLUE (==0==PS_CHANNEL_RGB) uses G.b
	// if (Input[6].Channel == PS_CHANNEL_ALPHA) LOG_TEST_CASE("PS_CHANNEL_ALPHA detected on final combiner G input"); // PS_CHANNEL_ALPHA (==1) uses .a  Test-case : XDK samples BumpDemo,BumpEarth,BumpLens,Explosion

	// Validate correctness (see NOTE below)
	if ((FinalCombinerSettingFlags & ~0xE0) > 0) LOG_TEST_CASE("Unknown FinalCombinerSetting bits detected");
}

/* DecodedRegisterCombiner */

void DecodedRegisterCombiner::GetPSTextureModes(xbox::X_D3DPIXELSHADERDEF* pPSDef, PS_TEXTUREMODES psTextureModes[xbox::X_D3DTS_STAGECOUNT])
{
	for (int i = 0; i < xbox::X_D3DTS_STAGECOUNT; i++) {
		psTextureModes[i] = (PS_TEXTUREMODES)((pPSDef->PSTextureModes >> (i * 5)) & PS_TEXTUREMODES_MASK);

		// Discover test-cases (see TODO below)
		// if (psTextureModes[i] == PS_TEXTUREMODES_NONE) LOG_TEST_CASE("PS_TEXTUREMODES_NONE");
		// if (psTextureModes[i] == PS_TEXTUREMODES_PROJECT2D) LOG_TEST_CASE("PS_TEXTUREMODES_PROJECT2D");
		if (psTextureModes[i] == PS_TEXTUREMODES_PROJECT3D) LOG_TEST_CASE("PS_TEXTUREMODES_PROJECT3D"); // Test-case: XDK sample TechCertGame,NoSortAlphaBlend,VolumeLight
		if (psTextureModes[i] == PS_TEXTUREMODES_CUBEMAP) LOG_TEST_CASE("PS_TEXTUREMODES_CUBEMAP"); // Test-case : XDK sample TechCertGame,Minnaert  TODO : More test cases needed
		if (psTextureModes[i] == PS_TEXTUREMODES_PASSTHRU) LOG_TEST_CASE("PS_TEXTUREMODES_PASSTHRU"); // Test-case : XDK sample BumpDemo  TODO : More test cases needed
		if (psTextureModes[i] == PS_TEXTUREMODES_CLIPPLANE) LOG_TEST_CASE("PS_TEXTUREMODES_CLIPPLANE"); // Test-case : XDK sample UserClipPlane  TODO : More test cases needed
		// if (psTextureModes[i] == PS_TEXTUREMODES_BUMPENVMAP) LOG_TEST_CASE("PS_TEXTUREMODES_BUMPENVMAP"); // Test-case : XDK sample BumpEarth, BumpLens
		if (psTextureModes[i] == PS_TEXTUREMODES_BUMPENVMAP_LUM) LOG_TEST_CASE("PS_TEXTUREMODES_BUMPENVMAP_LUM"); // Test-case : XDK sample BumpEarth  TODO : More test cases needed
		if (psTextureModes[i] == PS_TEXTUREMODES_BRDF) LOG_TEST_CASE("PS_TEXTUREMODES_BRDF");
		if (psTextureModes[i] == PS_TEXTUREMODES_DOT_ST) LOG_TEST_CASE("PS_TEXTUREMODES_DOT_ST");
		if (psTextureModes[i] == PS_TEXTUREMODES_DOT_ZW) LOG_TEST_CASE("PS_TEXTUREMODES_DOT_ZW");
		if (psTextureModes[i] == PS_TEXTUREMODES_DOT_RFLCT_DIFF) LOG_TEST_CASE("PS_TEXTUREMODES_DOT_RFLCT_DIFF");
		if (psTextureModes[i] == PS_TEXTUREMODES_DOT_RFLCT_SPEC) LOG_TEST_CASE("PS_TEXTUREMODES_DOT_RFLCT_SPEC");
		if (psTextureModes[i] == PS_TEXTUREMODES_DOT_STR_3D) LOG_TEST_CASE("PS_TEXTUREMODES_DOT_STR_3D");
		if (psTextureModes[i] == PS_TEXTUREMODES_DOT_STR_CUBE) LOG_TEST_CASE("PS_TEXTUREMODES_DOT_STR_CUBE");
		if (psTextureModes[i] == PS_TEXTUREMODES_DPNDNT_AR) LOG_TEST_CASE("PS_TEXTUREMODES_DPNDNT_AR");
		if (psTextureModes[i] == PS_TEXTUREMODES_DPNDNT_GB) LOG_TEST_CASE("PS_TEXTUREMODES_DPNDNT_GB");
		if (psTextureModes[i] == PS_TEXTUREMODES_DOTPRODUCT) LOG_TEST_CASE("PS_TEXTUREMODES_DOTPRODUCT");
		if (psTextureModes[i] == PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST) LOG_TEST_CASE("PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST");

		// Validate correctness (see NOTE below)
		if (psTextureModes[i] == PS_TEXTUREMODES_BUMPENVMAP) if (i < 1) LOG_TEST_CASE("PS_TEXTUREMODES_BUMPENVMAP only allowed in stage 1, 2 or 3");
		if (psTextureModes[i] == PS_TEXTUREMODES_BUMPENVMAP_LUM) if (i < 1) LOG_TEST_CASE("PS_TEXTUREMODES_BUMPENVMAP_LUM only allowed in stage 1, 2 or 3"); 
		if (psTextureModes[i] == PS_TEXTUREMODES_BRDF) if (i < 2) LOG_TEST_CASE("PS_TEXTUREMODES_BRDF only allowed in stage 2 or 3");
		if (psTextureModes[i] == PS_TEXTUREMODES_DOT_ST) {
			if (i < 2) LOG_TEST_CASE("PS_TEXTUREMODES_DOT_ST only allowed in stage 2 or 3");
			if (psTextureModes[i - 1] != PS_TEXTUREMODES_DOTPRODUCT) LOG_TEST_CASE("PS_TEXTUREMODES_DOT_ST only allowed after PS_TEXTUREMODES_DOTPRODUCT");
		}
		if (psTextureModes[i] == PS_TEXTUREMODES_DOT_ZW) {
			if (i < 2) LOG_TEST_CASE("PS_TEXTUREMODES_DOT_ZW only allowed in stage 2 or 3");
			if (psTextureModes[i - 1] != PS_TEXTUREMODES_DOTPRODUCT) LOG_TEST_CASE("PS_TEXTUREMODES_DOT_ZW only allowed after PS_TEXTUREMODES_DOTPRODUCT");
		}
		if (psTextureModes[i] == PS_TEXTUREMODES_DOT_RFLCT_DIFF) if (i != 2) LOG_TEST_CASE("PS_TEXTUREMODES_DOT_RFLCT_DIFF only allowed in stage 2");
		if (psTextureModes[i] == PS_TEXTUREMODES_DOT_RFLCT_SPEC) if (i < 3) LOG_TEST_CASE("PS_TEXTUREMODES_DOT_RFLCT_SPEC only allowed in stage 3");
		if (psTextureModes[i] == PS_TEXTUREMODES_DOT_STR_3D) if (i < 3) LOG_TEST_CASE("PS_TEXTUREMODES_DOT_STR_3D only allowed in stage 3");
		if (psTextureModes[i] == PS_TEXTUREMODES_DOT_STR_CUBE) if (i < 3) LOG_TEST_CASE("PS_TEXTUREMODES_DOT_STR_CUBE only allowed in stage 1, 2 or 3");
		if (psTextureModes[i] == PS_TEXTUREMODES_DPNDNT_AR) if (i < 1) LOG_TEST_CASE("PS_TEXTUREMODES_DPNDNT_AR only allowed in stage 1, 2 or 3");
		if (psTextureModes[i] == PS_TEXTUREMODES_DPNDNT_GB) if (i < 1) LOG_TEST_CASE("PS_TEXTUREMODES_DPNDNT_GB only allowed in stage 1, 2 or 3");
		if (psTextureModes[i] == PS_TEXTUREMODES_DOTPRODUCT) if (i < 1 || i > 2) LOG_TEST_CASE("PS_TEXTUREMODES_DOTPRODUCT only allowed in stage 1 or 2");
		if (psTextureModes[i] == PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST) if (i < 3) LOG_TEST_CASE("PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST only allowed in 3");
		if (psTextureModes[i] > PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST) LOG_TEST_CASE("Invalid PS_TEXTUREMODES in stage?");
	}

	// Validate correctness (see NOTE below)
	if ((pPSDef->PSTextureModes  & ~0x000FFFFF) > 0) LOG_TEST_CASE("Unknown PSTextureModes bits detected");
}

void DecodedRegisterCombiner::GetPSDotMapping(xbox::X_D3DPIXELSHADERDEF* pPSDef, PS_DOTMAPPING psDotMapping[xbox::X_D3DTS_STAGECOUNT])
{
	psDotMapping[0] = (PS_DOTMAPPING)(0);
	for (int i = 1; i < xbox::X_D3DTS_STAGECOUNT; i++) {
		psDotMapping[i] = (PS_DOTMAPPING)((pPSDef->PSDotMapping >> ((i - 1) * 4)) & PS_DOTMAPPING_MASK);

		// Discover test-cases (see TODO below)
		// if (psDotMapping[i] == PS_DOTMAPPING_ZERO_TO_ONE) LOG_TEST_CASE("PS_DOTMAPPING_ZERO_TO_ONE"); // Note : Most common scenario, no need for test-cases
		if (psDotMapping[i] == PS_DOTMAPPING_MINUS1_TO_1_D3D) LOG_TEST_CASE("PS_DOTMAPPING_MINUS1_TO_1_D3D"); // Test-case : XDK samples BumpDemo, Minnaert
		if (psDotMapping[i] == PS_DOTMAPPING_MINUS1_TO_1_GL) LOG_TEST_CASE("PS_DOTMAPPING_MINUS1_TO_1_GL");
		if (psDotMapping[i] == PS_DOTMAPPING_MINUS1_TO_1) LOG_TEST_CASE("PS_DOTMAPPING_MINUS1_TO_1");
		if (psDotMapping[i] == PS_DOTMAPPING_HILO_1) LOG_TEST_CASE("PS_DOTMAPPING_HILO_1");
		if (psDotMapping[i] == PS_DOTMAPPING_HILO_HEMISPHERE_D3D) LOG_TEST_CASE("PS_DOTMAPPING_HILO_HEMISPHERE_D3D");
		if (psDotMapping[i] == PS_DOTMAPPING_HILO_HEMISPHERE_GL) LOG_TEST_CASE("PS_DOTMAPPING_HILO_HEMISPHERE_GL");
		if (psDotMapping[i] == PS_DOTMAPPING_HILO_HEMISPHERE) LOG_TEST_CASE("PS_DOTMAPPING_HILO_HEMISPHERE");
	}

	// Validate correctness (see NOTE below)
	if ((pPSDef->PSDotMapping & ~0x00000777) > 0) LOG_TEST_CASE("Unknown PSDotMapping bits detected");
}

void DecodedRegisterCombiner::GetPSCompareModes(xbox::X_D3DPIXELSHADERDEF* pPSDef, bool psCompareModes[xbox::X_D3DTS_STAGECOUNT][4])
{
	for (int i = 0; i < xbox::X_D3DTS_STAGECOUNT; i++) {
		uint32_t CompareMode = (pPSDef->PSCompareMode >> (i * 4)) & PS_COMPAREMODE_MASK;
		psCompareModes[i][0] = (CompareMode & PS_COMPAREMODE_S_GE) > 0;
		psCompareModes[i][1] = (CompareMode & PS_COMPAREMODE_T_GE) > 0;
		psCompareModes[i][2] = (CompareMode & PS_COMPAREMODE_R_GE) > 0;
		psCompareModes[i][3] = (CompareMode & PS_COMPAREMODE_Q_GE) > 0;
	}

	// Validate correctness (see NOTE below)
	if ((pPSDef->PSCompareMode & ~0x0000FFFF) > 0) LOG_TEST_CASE("Unknown PSCompareMode bits detected");
}

void DecodedRegisterCombiner::GetPSInputTexture(xbox::X_D3DPIXELSHADERDEF* pPSDef, int psInputTexture[xbox::X_D3DTS_STAGECOUNT])
{
	psInputTexture[0] = -1; // Stage 0 has no predecessors (should not be used)
	psInputTexture[1] = 0; // Stage 1 can only use stage 0
	psInputTexture[2] = (pPSDef->PSInputTexture >> 16) & 0x1; // Stage 2 can use stage 0 or 1
	psInputTexture[3] = (pPSDef->PSInputTexture >> 20) & 0x3; // Stage 3 can only use stage 0, 1 or 2

	// Discover test-cases (see TODO below)
	if (psInputTexture[2] == 0) LOG_TEST_CASE("PS_INPUTTEXTURE(2) uses texture 0");
	// if (psInputTexture[2] == 1) LOG_TEST_CASE("PS_INPUTTEXTURE(2) uses texture 1"); // Test-case : XDK sample BumpEarth,Explosion,ZSprite
	if (psInputTexture[3] == 0) LOG_TEST_CASE("PS_INPUTTEXTURE(3) uses texture 0");
	// if (psInputTexture[3] == 1) LOG_TEST_CASE("PS_INPUTTEXTURE(3) uses texture 1"); // Test-case : XDK sample Explosion,ZSprite
	if (psInputTexture[3] == 2) LOG_TEST_CASE("PS_INPUTTEXTURE(3) uses texture 2");

	// Validate correctness (see NOTE below)
	if (psInputTexture[3] == 3) LOG_TEST_CASE("PS_INPUTTEXTURE(3) incorrectly uses texture 3");
	if ((pPSDef->PSInputTexture & ~0x00310000) > 0) LOG_TEST_CASE("Unknown PSInputTexture bits detected");
}

void DecodedRegisterCombiner::Decode(xbox::X_D3DPIXELSHADERDEF *pPSDef)
{
	NumberOfCombiners = (pPSDef->PSCombinerCount >> 0) & 0xF;
	uint32_t CombinerCountFlags = (pPSDef->PSCombinerCount >> 8); // = PS_COMBINERCOUNTFLAGS

	CombinerMuxesOnMsb = (CombinerCountFlags & PS_COMBINERCOUNT_MUX_MSB) > 0;
	CombinerHasUniqueC0 = (CombinerCountFlags & PS_COMBINERCOUNT_UNIQUE_C0) > 0;
	CombinerHasUniqueC1 = (CombinerCountFlags & PS_COMBINERCOUNT_UNIQUE_C1) > 0;

	hasFinalCombiner = (pPSDef->PSFinalCombinerInputsABCD > 0) || (pPSDef->PSFinalCombinerInputsEFG > 0);

	GetPSTextureModes(pPSDef, PSTextureModes);
	GetPSCompareModes(pPSDef, PSCompareMode);
	GetPSDotMapping(pPSDef, PSDotMapping);
	GetPSInputTexture(pPSDef, PSInputTexture);

	for (unsigned i = 0; i < NumberOfCombiners; i++) {
		Combiners[i].RGB.Decode(pPSDef->PSRGBInputs[i], pPSDef->PSRGBOutputs[i], /*stage_nr=*/i, /*isRGB=*/true);
		Combiners[i].Alpha.Decode(pPSDef->PSAlphaInputs[i], pPSDef->PSAlphaOutputs[i], /*stage_nr=*/i, /*isRGB=*/false);
	}

	if (hasFinalCombiner) {
		FinalCombiner.Decode(pPSDef->PSFinalCombinerInputsABCD, pPSDef->PSFinalCombinerInputsEFG);
	}

	TexModeAdjust = ((pPSDef->PSFinalCombinerConstants >> PS_GLOBALFLAGS_SHIFT) & PS_GLOBALFLAGS_TEXMODE_ADJUST) > 0;

	// Discover test-cases (see TODO below)
	if (NumberOfCombiners == 0) LOG_TEST_CASE("NumberOfCombiners is zero");
	if (!CombinerMuxesOnMsb) LOG_TEST_CASE("PS_COMBINERCOUNT_MUX_LSB detected"); // Test case required for how to implement the FCS_MUX check on LSB (see PS_COMBINERCOUNT_MUX_LSB in CxbxPixelShaderTemplate.hlsl) Note : test-case ModifyPixelShader hits this by mistake
	if (TexModeAdjust) LOG_TEST_CASE("PS_GLOBALFLAGS_TEXMODE_ADJUST detected");

	// Validate correctness (see NOTE below)
	if (NumberOfCombiners > 8) LOG_TEST_CASE("NumberOfCombiners bigger than maximum (of 8)");
	if ((pPSDef->PSCombinerCount & ~0x0001110F) > 0) LOG_TEST_CASE("Unknown PSCombinerCount bits detected");
}

// * TODO : For all "Discover test-cases" LOG_TEST_CASE's that lack sufficient test-case mentions, find some, note them in an EOL comment, and comment out the entire check.
// * NOTE : For all "Validate correctness" LOG_TEST_CASE's that ever get hit, investigate what caused it, what should be done, implement that, and update the verification.

/* PSH_RECOMPILED_SHADER */

typedef struct s_CxbxPSDef {
	xbox::X_D3DPIXELSHADERDEF PSDef;
	xbox::X_D3DRESOURCETYPE ActiveTextureTypes[xbox::X_D3DTS_STAGECOUNT];
	bool DecodedTexModeAdjust;
	bool DecodedHasFinalCombiner;
	bool RenderStateFogEnable;
	bool RenderStateSpecularEnable;
	bool AlphaKill[4]; // Read from XboxTextureStates.Get(stage, xbox::X_D3DTSS_ALPHAKILL);

	bool IsEquivalent(const s_CxbxPSDef &Another)
	{
		// Only compare the [*]-marked members, which forms the unique shader declaration (ignore the constants and most Xbox Direct3D8 run-time fields) :
		// [*] DWORD    PSAlphaInputs[8];          // X_D3DRS_PSALPHAINPUTS0..X_D3DRS_PSALPHAINPUTS7 : Alpha inputs for each stage
		// [*] DWORD    PSFinalCombinerInputsABCD; // X_D3DRS_PSFINALCOMBINERINPUTSABCD : Final combiner inputs
		// [*] DWORD    PSFinalCombinerInputsEFG;  // X_D3DRS_PSFINALCOMBINERINPUTSEFG : Final combiner inputs (continued)
		if (memcmp(&(PSDef.PSAlphaInputs[0]), &(Another.PSDef.PSAlphaInputs[0]), (8 + 1 + 1) * sizeof(DWORD)) != 0)
			return false;

		// [-] DWORD    PSConstant0[8];            // X_D3DRS_PSCONSTANT0_0..X_D3DRS_PSCONSTANT0_7 : C0 for each stage
		// [-] DWORD    PSConstant1[8];            // X_D3DRS_PSCONSTANT1_0..X_D3DRS_PSCONSTANT1_7 : C1 for each stage
		// [*] DWORD    PSAlphaOutputs[8];         // X_D3DRS_PSALPHAOUTPUTS0..X_D3DRS_PSALPHAOUTPUTS7 : Alpha output for each stage
		// [*] DWORD    PSRGBInputs[8];            // X_D3DRS_PSRGBINPUTS0..X_D3DRS_PSRGBINPUTS7 : RGB inputs for each stage
		// [*] DWORD    PSCompareMode;             // X_D3DRS_PSCOMPAREMODE : Compare modes for clipplane texture mode
		if (memcmp(&(PSDef.PSAlphaOutputs[0]), &(Another.PSDef.PSAlphaOutputs[0]), (8 + 8 + 1) * sizeof(DWORD)) != 0)
			return false;

		// [-] DWORD    PSFinalCombinerConstant0;  // X_D3DRS_PSFINALCOMBINERCONSTANT0 : C0 in final combiner
		// [-] DWORD    PSFinalCombinerConstant1;  // X_D3DRS_PSFINALCOMBINERCONSTANT1 : C1 in final combiner
		// [*] DWORD    PSRGBOutputs[8];           // X_D3DRS_PSRGBOUTPUTS0..X_D3DRS_PSRGBOUTPUTS7 : Stage 0 RGB outputs
		// [*] DWORD    PSCombinerCount;           // X_D3DRS_PSCOMBINERCOUNT : Active combiner count (Stages 0-7)
		// [*] DWORD    PSTextureModes;            // X_D3DRS_PS_RESERVED (copied from out-of-range X_D3DRS_PSTEXTUREMODES) : Texture addressing modes
		// [*] DWORD    PSDotMapping;              // X_D3DRS_PSDOTMAPPING : Input mapping for dot product modes
		// [*] DWORD    PSInputTexture;            // X_D3DRS_PSINPUTTEXTURE : Texture source for some texture modes
		if (memcmp(&(PSDef.PSRGBOutputs[0]), &(Another.PSDef.PSRGBOutputs[0]), (8 + 1 + 1 + 1 + 1) * sizeof(DWORD)) != 0)
			return false;

		// [-] DWORD    PSC0Mapping;               // Mapping of c0 regs to D3D constants
		// [-] DWORD    PSC1Mapping;               // Mapping of c1 regs to D3D constants
		// [*] DWORD    PSFinalCombinerConstants;  // Final combiner constant mapping
		// Note : From PSFinalCombinerConstants, only the PS_GLOBALFLAGS_TEXMODE_ADJUST flag must correspond
		if (DecodedTexModeAdjust != Another.DecodedTexModeAdjust)
			return false;

		for (unsigned i = 0; i < xbox::X_D3DTS_STAGECOUNT; i++)
			if (AlphaKill[i] != Another.AlphaKill[i])
				return false;

		// All ActiveTextureTypes must correspond as well (otherwise the recompiled shader would sample incorrectly) :
		for (unsigned i = 0; i < xbox::X_D3DTS_STAGECOUNT; i++)
			if (ActiveTextureTypes[i] != Another.ActiveTextureTypes[i])
				return false;

		// Fail if they don't correspond in their use of the final combiner unit
		if (DecodedHasFinalCombiner != Another.DecodedHasFinalCombiner)
			return false;

		// If they don't use the final combiner unit
		if (!DecodedHasFinalCombiner) {
			// Fail if they don't correspond on the render states that impact AdjustFinalCombiner
			if (RenderStateFogEnable != Another.RenderStateFogEnable)
				return false;

			if (RenderStateSpecularEnable != Another.RenderStateSpecularEnable)
				return false;
		}

		return true;
	}

	void SnapshotRuntimeVariables()
	{
		// These values are checked in IsEquivalent to see if a cached pixel shader matches this declaration

		// Fetch currently active texture types, which impact AdjustTextureModes
		for (unsigned i = 0; i < xbox::X_D3DTS_STAGECOUNT; i++) {
			extern xbox::X_D3DRESOURCETYPE GetXboxD3DResourceType(const xbox::X_D3DResource *pXboxResource);

			if (g_pXbox_SetTexture[i])
				ActiveTextureTypes[i] = GetXboxD3DResourceType(g_pXbox_SetTexture[i]);
			else
				ActiveTextureTypes[i] = xbox::X_D3DRTYPE_NONE;
		}

		// Pre-decode TexModeAdjust, which impacts AdjustTextureModes
		DecodedTexModeAdjust = ((PSDef.PSFinalCombinerConstants >> PS_GLOBALFLAGS_SHIFT) & PS_GLOBALFLAGS_TEXMODE_ADJUST) > 0;

		// Pre-decode hasFinalCombiner, which impacts AdjustFinalCombiner
		DecodedHasFinalCombiner = (PSDef.PSFinalCombinerInputsABCD > 0) || (PSDef.PSFinalCombinerInputsEFG > 0);

		// Fetch all render states that impact AdjustFinalCombiner
		RenderStateFogEnable = XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_FOGENABLE) > 0;
		RenderStateSpecularEnable = XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_SPECULARENABLE) > 0;

		for (unsigned i = 0; i < xbox::X_D3DTS_STAGECOUNT; i++) {
			// Test-cases : XDK sample nosortalphablend, Xbmc-fork (https://github.com/superpea/xbmc-fork/blob/bba40d57db52d11dea7bbf9509c298f7c2b05f4b/xbmc/cores/VideoRenderers/XBoxRenderer.cpp#L134)
			// Star Wars: Jedi Academy (https://github.com/RetailGameSourceCode/StarWars_JediAcademy/blob/5b8f0040b3177d8855f7d575ef49b23ed52ff42a/codemp/win32/win_lighteffects.cpp#L299)
			AlphaKill[i] = XboxTextureStates.Get(/*stage=*/i, xbox::X_D3DTSS_ALPHAKILL) & 4; // D3DTALPHAKILL_ENABLE
		}
	}

	void AdjustTextureModes(DecodedRegisterCombiner &RC)
	{
		// if this flag is set, the texture mode for each texture stage is adjusted as follows:
		if (RC.TexModeAdjust) {
			for (int i = 0; i < xbox::X_D3DTS_STAGECOUNT; i++) {
				// First, disable not-assigned textures
				if (ActiveTextureTypes[i] == xbox::X_D3DRTYPE_NONE) {
					RC.PSTextureModes[i] = PS_TEXTUREMODES_NONE;
					continue;
				}

				// Then adjust some texture mode according to the currently active textures, so that the shader will use the appropriate sampling method
				switch (RC.PSTextureModes[i]) {
				case PS_TEXTUREMODES_PROJECT2D:
				case PS_TEXTUREMODES_PROJECT3D:
				case PS_TEXTUREMODES_CUBEMAP:
					if (ActiveTextureTypes[i] == xbox::X_D3DRTYPE_CUBETEXTURE)
						RC.PSTextureModes[i] = PS_TEXTUREMODES_CUBEMAP;
					else
						if (ActiveTextureTypes[i] == xbox::X_D3DRTYPE_VOLUMETEXTURE)
							// TODO : Also do this for DepthBuffers (but not EmuXBFormatIsLinear!) :
							// || EmuXBFormatIsDepthBuffer(GetXboxPixelContainerFormat(g_pXbox_SetTexture[i])) in { X_D3DFMT_D24S8, X_D3DFMT_F24S8, X_D3DFMT_D16, X_D3DFMT_F16}
							RC.PSTextureModes[i] = PS_TEXTUREMODES_PROJECT3D;
						else
							RC.PSTextureModes[i] = PS_TEXTUREMODES_PROJECT2D;
					break;
				case PS_TEXTUREMODES_DOT_STR_3D:
				case PS_TEXTUREMODES_DOT_STR_CUBE:
					if (ActiveTextureTypes[i] == xbox::X_D3DRTYPE_CUBETEXTURE)
						RC.PSTextureModes[i] = PS_TEXTUREMODES_DOT_STR_CUBE;
					else
						RC.PSTextureModes[i] = PS_TEXTUREMODES_DOT_STR_3D;
					break;
				}
				/* Was :
							switch (ActiveTextureTypes[i]) {
							case xbox::X_D3DRTYPE_CUBETEXTURE:
								switch (RC.PSTextureModes[i]) {
								case PS_TEXTUREMODES_PROJECT2D: RC.PSTextureModes[i] = PS_TEXTUREMODES_CUBEMAP; break;
								case PS_TEXTUREMODES_PROJECT3D: RC.PSTextureModes[i] = PS_TEXTUREMODES_CUBEMAP; break;
								case PS_TEXTUREMODES_DOT_STR_3D: RC.PSTextureModes[i] = PS_TEXTUREMODES_DOT_STR_CUBE; break;
								} break;
							case xbox::X_D3DRTYPE_VOLUMETEXTURE:
								switch (RC.PSTextureModes[i]) {
								case PS_TEXTUREMODES_PROJECT2D: RC.PSTextureModes[i] = PS_TEXTUREMODES_PROJECT3D; break;
								case PS_TEXTUREMODES_CUBEMAP: RC.PSTextureModes[i] = PS_TEXTUREMODES_PROJECT3D; break;
								case PS_TEXTUREMODES_DOT_STR_CUBE: RC.PSTextureModes[i] = PS_TEXTUREMODES_DOT_STR_3D; break;
								} break;
							case xbox::X_D3DRTYPE_TEXTURE:
								switch (RC.PSTextureModes[i]) {
								case PS_TEXTUREMODES_PROJECT3D: RC.PSTextureModes[i] = PS_TEXTUREMODES_PROJECT2D; break;
								case PS_TEXTUREMODES_CUBEMAP: RC.PSTextureModes[i] = PS_TEXTUREMODES_PROJECT2D; break;
								} break;
							case xbox::X_D3DRTYPE_NONE:
								RC.PSTextureModes[i] = PS_TEXTUREMODES_NONE;
								break;
							}
				*/
			}
		}
		else {
			// Texture modes were specified manually - but fix them up if necessary
			for (int i = 0; i < xbox::X_D3DTS_STAGECOUNT; i++) {
				// Fixup sampling cube textures with PROJECT2D
				// Test case: Splinter Cell Chaos Theory (lighting)
				if (ActiveTextureTypes[i] == xbox::X_D3DRTYPE_CUBETEXTURE && RC.PSTextureModes[i] == PS_TEXTUREMODES_PROJECT2D) {
					EmuLog(LOG_LEVEL::WARNING, "PROJECT2D sampling is used with a cubemap texture - using CUBEMAP sampling instead");
					RC.PSTextureModes[i] = PS_TEXTUREMODES_CUBEMAP;
				}
			}
		}
	}

	void AdjustFinalCombiner(DecodedRegisterCombiner &RC)
	{
		if (RC.hasFinalCombiner) return;

		// Since we're HLE'ing Xbox D3D, mimick how it configures the final combiner when PSDef doesn't :
		// TODO : Use the same final combiner when no pixel shader is set! Possibly by generating a DecodedRegisterCombiner with PSCombinerCount zero?
		// (This forms the entire Xbox fixed function pixel pipeline, which uses only two renderstates : X_D3DRS_SPECULARENABLE and X_D3DRS_SPECULARENABLE.)
		//
		// If X_D3DRS_FOGENABLE, configure final combiner to perform this operation :
		//   if (X_D3DRS_SPECULARENABLE) r0.rgb = lerp(fog.rgb, r0.rgb, fog.a) + v1.rgb;
		//                          else r0.rgb = lerp(fog.rgb, r0.rgb, fog.a);
		//   r0.a = abs(r0.a);
		// Otherwise, if not X_D3DRS_FOGENABLE, configure final combiner to perform this operation :
		//   if (X_D3DRS_SPECULARENABLE) r0.rgb = r0.rgb + v1.rgb;
		//                          else r0.rgb = r0.rgb;
		// Remember : 
		//   xfc.rgb = lerp(C, B, A) + D
		//   xfc.a = G.b
		// Whereby A, B, C and G can use the two xfc-special purpose registers :
		//   V1R0 = V1 + R0
		//   EFPROD = E * F
		// ( Or in shorthand : sum=r0+v1, prod=s4*s5, r0.rgb=s0*s1+{1-s0}*s2+s3, r0.a=s6.b )
		RC.FinalCombiner.Input[0/*A*/].Channel = PS_CHANNEL_ALPHA;
		RC.FinalCombiner.Input[0/*A*/].Reg = PS_REGISTER_FOG;
		RC.FinalCombiner.Input[1/*B*/].Reg = PS_REGISTER_R0;
		// If fog is disabled, blend R0 with itself
		RC.FinalCombiner.Input[2/*C*/].Reg = RenderStateFogEnable ? PS_REGISTER_FOG : PS_REGISTER_R0;
		RC.FinalCombiner.Input[3/*D*/].Reg = RenderStateSpecularEnable ? PS_REGISTER_V1 : PS_REGISTER_ZERO;
		RC.FinalCombiner.Input[4/*E*/].Reg = PS_REGISTER_ZERO; // Note : Not really needed, should be 0 already
		RC.FinalCombiner.Input[5/*F*/].Reg = PS_REGISTER_ZERO; // Note : Not really needed, should be 0 already
		RC.FinalCombiner.Input[6/*G*/].Reg = PS_REGISTER_R0;
		RC.FinalCombiner.Input[6/*G*/].Channel = PS_CHANNEL_ALPHA;
	}

	void PerformRuntimeAdjustments(DecodedRegisterCombiner &RC)
	{
		RC.AlphaKill[0] = AlphaKill[0];
		RC.AlphaKill[1] = AlphaKill[1];
		RC.AlphaKill[2] = AlphaKill[2];
		RC.AlphaKill[3] = AlphaKill[3];
		AdjustTextureModes(RC);
		AdjustFinalCombiner(RC);
	}
}
CxbxPSDef;

typedef struct _PSH_RECOMPILED_SHADER {
	CxbxPSDef CompletePSDef;
	IDirect3DPixelShader* ConvertedPixelShader;
} PSH_RECOMPILED_SHADER;

PSH_RECOMPILED_SHADER CxbxRecompilePixelShader(CxbxPSDef &CompletePSDef)
{
	DecodedRegisterCombiner RC = {};
	RC.Decode(&(CompletePSDef.PSDef));
	CompletePSDef.PerformRuntimeAdjustments(RC);

	ID3DBlob *pShader = nullptr;
	EmuCompilePixelShader(&RC, &pShader);

	PSH_RECOMPILED_SHADER Result;
	Result.CompletePSDef = CompletePSDef;
	Result.ConvertedPixelShader = nullptr;
	if (pShader) {
		DWORD *pFunction = (DWORD*)pShader->GetBufferPointer();
		if (pFunction) {
			DWORD hRet = g_pD3DDevice->CreatePixelShader(pFunction, &(Result.ConvertedPixelShader));
			if (hRet != D3D_OK) {
				printf(D3DErrorString(hRet));
			}
		}
		pShader->Release();
	}

	return Result;
} // CxbxRecompilePixelShader

std::vector<PSH_RECOMPILED_SHADER> g_RecompiledPixelShaders;

// Mapping indices of Xbox register combiner constants to host pixel shader constants;
// The first 16 are identity-mapped (C0_1 .. C0_7 are C0 .. C7 on host, C1_0 .. C1_7 are C8 .. C15 on host) :
constexpr int PSH_XBOX_CONSTANT_C0 = 0; // = 0..15
// Then two final combiner constants :
constexpr int PSH_XBOX_CONSTANT_FC0 = PSH_XBOX_CONSTANT_C0 + PSH_XBOX_MAX_C_REGISTER_COUNT; // = 16
constexpr int PSH_XBOX_CONSTANT_FC1 = PSH_XBOX_CONSTANT_FC0 + 1; // = 17
// Fog requires a constant (as host PS1.4 doesn't support the FOG register)
constexpr int PSH_XBOX_CONSTANT_FOG = PSH_XBOX_CONSTANT_FC1 + 1; // = 18
// Bump Environment Material registers
constexpr int PSH_XBOX_CONSTANT_BEM = PSH_XBOX_CONSTANT_FOG + 1; // = 19..22
// Bump map Luminance registers
constexpr int PSH_XBOX_CONSTANT_LUM = PSH_XBOX_CONSTANT_BEM + 4; // = 23..26
// Which winding order to consider as the front face
constexpr int PSH_XBOX_CONSTANT_FRONTFACE_FACTOR = PSH_XBOX_CONSTANT_LUM + 4; // = 27
// This concludes the set of constants that need to be set on host :
constexpr int PSH_XBOX_CONSTANT_MAX = PSH_XBOX_CONSTANT_FRONTFACE_FACTOR + 1; // = 28

std::string GetFixedFunctionShaderTemplate() {
	static bool loaded = false;
	static std::string hlslString;

	// TODO does this need to be thread safe?
	if (!loaded) {
		loaded = true;

		// Determine the filename and directory for the fixed function shader
		// TODO make this a relative path so we guarantee an LPCSTR for D3DCompile
		auto hlslDir = std::filesystem::path(szFilePath_CxbxReloaded_Exe)
			.parent_path()
			.append("hlsl");

		auto sourceFile = hlslDir.append("FixedFunctionPixelShader.hlsl").string();

		// Load the shader into a string
		std::ifstream hlslStream(sourceFile);
		std::stringstream hlsl;
		hlsl << hlslStream.rdbuf();

		hlslString = hlsl.str();
	}

	return hlslString;
}

std::string_view GetD3DTOPString(int d3dtop) {
	static constexpr std::string_view opToString[] = {
#ifdef ENABLE_FF_ALPHAKILL
		"X_D3DTOP_DISABLE", // 0 (Was UNDEFINED, but that doesn't compile)
#else
		"UNDEFINED", // 0
#endif
		"X_D3DTOP_DISABLE", // 1
		"X_D3DTOP_SELECTARG1", // 2
		"X_D3DTOP_SELECTARG2", // 3
		"X_D3DTOP_MODULATE", // 4
		"X_D3DTOP_MODULATE2X", // 5
		"X_D3DTOP_MODULATE4X", // 6
		"X_D3DTOP_ADD", // 7
		"X_D3DTOP_ADDSIGNED", // 8
		"X_D3DTOP_ADDSIGNED2X", // 9
		"X_D3DTOP_SUBTRACT", // 10
		"X_D3DTOP_ADDSMOOTH", // 11
		"X_D3DTOP_BLENDDIFFUSEALPHA", // 12
		"X_D3DTOP_BLENDCURRENTALPHA", // 13
		"X_D3DTOP_BLENDTEXTUREALPHA", // 14
		"X_D3DTOP_BLENDFACTORALPHA", // 15
		"X_D3DTOP_BLENDTEXTUREALPHAPM", // 16
		"X_D3DTOP_PREMODULATE", // 17
		"X_D3DTOP_MODULATEALPHA_ADDCOLOR", // 18
		"X_D3DTOP_MODULATECOLOR_ADDALPHA", // 19
		"X_D3DTOP_MODULATEINVALPHA_ADDCOLOR", // 20
		"X_D3DTOP_MODULATEINVCOLOR_ADDALPHA", // 21
		"X_D3DTOP_DOTPRODUCT3", // 22
		"X_D3DTOP_MULTIPLYADD", // 23
		"X_D3DTOP_LERP", // 24
		"X_D3DTOP_BUMPENVMAP", // 25
		"X_D3DTOP_BUMPENVMAPLUMINANCE", // 26
	};

#ifdef ENABLE_FF_ALPHAKILL
	if (d3dtop < 0 || d3dtop > 26) {
#else
	if (d3dtop < 1 || d3dtop > 26) {
#endif
		EmuLog(LOG_LEVEL::ERROR2, "Unmapped texture operation %d", d3dtop);
		d3dtop = 0; // undefined
	}

	return opToString[d3dtop];
}

// Get a string equivalent of '<Texture Argument> + <Modifier>'
std::string GetD3DTASumString(int d3dta, bool allowModifier = true) {
	using namespace FixedFunctionPixelShader;

	static const std::string argToString[] = {
		"X_D3DTA_DIFFUSE", // 0
		"X_D3DTA_CURRENT", // 1
		"X_D3DTA_TEXTURE", // 2
		"X_D3DTA_TFACTOR", // 3
		"X_D3DTA_SPECULAR", // 4
		"X_D3DTA_TEMP", // 5
		"X_D3DTA_CONSTANT", // 6
		"UNDEFINED", // 7
	};

	// Write a texture argument
	const int flagMask = 0x30;
	int iFlags = d3dta & flagMask;
	int i = d3dta & ~flagMask;

	if (i < 0 || i > 6) {
		EmuLog(LOG_LEVEL::ERROR2, "Unmapped texture argument %d on texture arg", i);
		i = 7; // undefined
	}

	auto str = argToString[i];
	if (iFlags) {
		if (!allowModifier) {
			EmuLog(LOG_LEVEL::ERROR2, "Modifier not expected on texture argument");
		}

		if (iFlags == X_D3DTA_COMPLEMENT)
			str += " + X_D3DTA_COMPLEMENT";
		else if (iFlags == X_D3DTA_ALPHAREPLICATE)
			str += " + X_D3DTA_ALPHAREPLICATE";
		else {
			EmuLog(LOG_LEVEL::ERROR2, "Unmapped texture modifier %d", iFlags);
			str += " /* + UNKNOWN MODIFIER */";
		}
	}

	return str;
}

// TODO we have to create and cache shaders over and over and over and over
// Deduplicate this resource management
IDirect3DPixelShader* GetFixedFunctionShader()
{
	using namespace FixedFunctionPixelShader;

	// TODO move this cache elsewhere - and flush it when the device is released!
	static std::unordered_map<uint64_t, IDirect3DPixelShader*> ffPsCache = {};

	// Create a key from state that will be baked in to the shader
	PsTextureHardcodedState states[4] = {};
	int sampleType[4] = { SAMPLE_NONE, SAMPLE_NONE, SAMPLE_NONE, SAMPLE_NONE };
	bool pointSpriteEnable = XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_POINTSPRITEENABLE);

	bool previousStageDisabled = false;
	for (int i = 0; i < 4; i++) {
		// Determine COLOROP
		// This controls both the texture operation for the colour of the stage
		// and when to stop processing
		// Under certain circumstances we force it to be DISABLE
		auto colorOp = XboxTextureStates.Get(i, xbox::X_D3DTSS_COLOROP);

		// Usually we execute stages up to the first disabled stage
		// However, if point sprites are enabled, we just execute stage 3
		bool forceDisable =
			(!pointSpriteEnable && previousStageDisabled) ||
			(pointSpriteEnable && i < 3);

		// When a texture stage has D3DTSS_COLORARG1 equal to D3DTA_TEXTURE
		// and the texture pointer for the stage is NULL, this stage
		// and all stages after it are not processed.
		// Test cases: Red Dead Revolver, JSRF
		// https://docs.microsoft.com/en-us/windows/win32/direct3d9/texture-blending
		// Don't follow the D3D9 docs if SELECTARG2 is in use (PC D3D9 behaviour, nvidia quirk?)
		// Test case: Crash Nitro Kart (engine speed UI)
		if (!g_pXbox_SetTexture[i]
			&& (XboxTextureStates.Get(i, xbox::X_D3DTSS_COLORARG1) & 0x7) == X_D3DTA_TEXTURE
			&& colorOp != xbox::X_D3DTOP_SELECTARG2)
		{
			forceDisable = true;
		}

		// Set the final COLOROP value
		states[i].COLOROP = forceDisable ? X_D3DTOP_DISABLE : colorOp;

		// If the stage is disabled we don't want its configuration to affect the key
		// Move on to the next stage
		if (colorOp == X_D3DTOP_DISABLE) {
			previousStageDisabled = true;
			continue;
		}

		// Get sample type
		// TODO move XD3D8 resource query functions out of Direct3D9.cpp so we can use them here
		if (g_pXbox_SetTexture[i]) {
			auto format = g_pXbox_SetTexture[i]->Format;
			if (format & X_D3DFORMAT_CUBEMAP)
				sampleType[i] = SAMPLE_CUBE;
			else if (((format & X_D3DFORMAT_DIMENSION_MASK) >> X_D3DFORMAT_DIMENSION_SHIFT) > 2)
				sampleType[i] = SAMPLE_3D;
			else
				sampleType[i] = SAMPLE_2D;
		}

		states[i].COLORARG0 = (float)XboxTextureStates.Get(i, xbox::X_D3DTSS_COLORARG0);
		states[i].COLORARG1 = (float)XboxTextureStates.Get(i, xbox::X_D3DTSS_COLORARG1);
		states[i].COLORARG2 = (float)XboxTextureStates.Get(i, xbox::X_D3DTSS_COLORARG2);

		auto alphaOp = XboxTextureStates.Get(i, xbox::X_D3DTSS_ALPHAOP);
		if (alphaOp == X_D3DTOP_DISABLE) LOG_TEST_CASE("Alpha stage disabled when colour stage is enabled");

		states[i].ALPHAOP = (float)alphaOp;
		states[i].ALPHAARG0 = (float)XboxTextureStates.Get(i, xbox::X_D3DTSS_ALPHAARG0);
		states[i].ALPHAARG1 = (float)XboxTextureStates.Get(i, xbox::X_D3DTSS_ALPHAARG1);
		states[i].ALPHAARG2 = (float)XboxTextureStates.Get(i, xbox::X_D3DTSS_ALPHAARG2);

		states[i].RESULTARG = (float)XboxTextureStates.Get(i, xbox::X_D3DTSS_RESULTARG);
	}

	// Create a key from the shader state
	// Note currently this is padded since it's what we send to the GPU
	auto key = 3 * ComputeHash(states, sizeof(states))
		+ ComputeHash(sampleType, sizeof(sampleType));

	auto got = ffPsCache.find(key);
	if (got != ffPsCache.end()) {
		// We have a shader. Great!
		return got->second;
	}

	// Build and compile a new shader
	auto hlslTemplate = GetFixedFunctionShaderTemplate();

	// In D3D9 it seems we need to know hardcode if we're doing a 2D or 3D lookup
	const std::string sampleTypePattern = "TEXTURE_SAMPLE_TYPE;";
	auto sampleTypeReplace = hlslTemplate.find(sampleTypePattern);

	static constexpr std::string_view typeToString[] = {
		"SAMPLE_NONE",
		"SAMPLE_2D",
		"SAMPLE_3D",
		"SAMPLE_CUBE"
	};

	std::stringstream sampleTypeString;
	sampleTypeString << "{"
		<< typeToString[sampleType[0]] << ", "
		<< typeToString[sampleType[1]] << ", "
		<< typeToString[sampleType[2]] << ", "
		<< typeToString[sampleType[3]] << "};";

	auto finalShader = hlslTemplate.replace(sampleTypeReplace, sampleTypePattern.size(), sampleTypeString.str());

	// Hardcode the texture stage operations and arguments
	// So the shader handles exactly one combination of values
	const std::string stageDef = "// STAGE DEFINITIONS";
	auto stageDefInsert = finalShader.find(stageDef) + stageDef.size();

	std::stringstream stageSetup;
	stageSetup << '\n';

	for (int i = 0; i < 4; i++) {
#ifdef ENABLE_FF_ALPHAKILL
		// Even when a stage is disabled, we still have to fully initialize it's values, to prevent
		// "error X4000: variable 'stages' used without having been completely initialized"
#else
		// The stage is initialized to be disabled
		// We don't have to output anything
		if (states[i].COLOROP == X_D3DTOP_DISABLE)
			continue;

#endif
		std::string target = "stages[" + std::to_string(i) + "].";

		auto s = states[i];
		stageSetup << target << "COLOROP = " << GetD3DTOPString(s.COLOROP) << ";\n";

		stageSetup << target << "COLORARG0 = " << GetD3DTASumString(s.COLORARG0) << ";\n";
		stageSetup << target << "COLORARG1 = " << GetD3DTASumString(s.COLORARG1) << ";\n";
		stageSetup << target << "COLORARG2 = " << GetD3DTASumString(s.COLORARG2) << ";\n";

		stageSetup << target << "ALPHAOP = " << GetD3DTOPString(s.ALPHAOP) << ";\n";

		stageSetup << target << "ALPHAARG0 = " << GetD3DTASumString(s.ALPHAARG0) << ";\n";
		stageSetup << target << "ALPHAARG1 = " << GetD3DTASumString(s.ALPHAARG1) << ";\n";
		stageSetup << target << "ALPHAARG2 = " << GetD3DTASumString(s.ALPHAARG2) << ";\n";

		stageSetup << target << "RESULTARG = " << GetD3DTASumString(s.RESULTARG, false) << ";\n";
		stageSetup << '\n';
	}

	finalShader = finalShader.insert(stageDefInsert, stageSetup.str());

	// Compile the shader
	ID3DBlob* pShaderBlob;

	auto hlslDir = std::filesystem::path(szFilePath_CxbxReloaded_Exe)
		.parent_path()
		.append("hlsl");

	auto pseudoFileName = "FixedFunctionPixelShader-" + std::to_string(key) + ".hlsl";
	auto pseudoSourceFile = hlslDir.append(pseudoFileName).string();
	EmuCompileShader(finalShader, "ps_3_0", &pShaderBlob, pseudoSourceFile.c_str());

	// Create shader object for the device
	IDirect3DPixelShader* pShader = nullptr;
	auto hRet = g_pD3DDevice->CreatePixelShader((DWORD*)pShaderBlob->GetBufferPointer(), &pShader);
	if (hRet != S_OK)
		CxbxrAbort("Failed to compile fixed function pixel shader");
	pShaderBlob->Release();

	// Insert the shader into the cache
	ffPsCache[key] = pShader;

	return pShader;
};

float AsFloat(uint32_t value) {
	auto v = value;
	return *(float*)&v;
}

// Set constant state for the fixed function pixel shader
void UpdateFixedFunctionPixelShaderState()
{
	using namespace FixedFunctionPixelShader;

	FixedFunctionPixelShaderState ffPsState;
	ffPsState.TextureFactor = (D3DXVECTOR4)((D3DXCOLOR)(XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_TEXTUREFACTOR)));
	ffPsState.SpecularEnable = XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_SPECULARENABLE);
	ffPsState.FogEnable = XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_FOGENABLE);
	ffPsState.FogColor = (D3DXVECTOR3)((D3DXCOLOR)XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_FOGCOLOR));

	// Texture state
	for (int i = 0; i < xbox::X_D3DTS_STAGECOUNT; i++) {

		auto stage = &ffPsState.stages[i];

		stage->COLORKEYOP = XboxTextureStates.Get(i, xbox::X_D3DTSS_COLORKEYOP);
		stage->COLORSIGN = XboxTextureStates.Get(i, xbox::X_D3DTSS_COLORSIGN);
		stage->ALPHAKILL = XboxTextureStates.Get(i, xbox::X_D3DTSS_ALPHAKILL);
		stage->BUMPENVMAT00 = AsFloat(XboxTextureStates.Get(i, xbox::X_D3DTSS_BUMPENVMAT00));
		stage->BUMPENVMAT01 = AsFloat(XboxTextureStates.Get(i, xbox::X_D3DTSS_BUMPENVMAT01));
		stage->BUMPENVMAT10 = AsFloat(XboxTextureStates.Get(i, xbox::X_D3DTSS_BUMPENVMAT10));
		stage->BUMPENVMAT11 = AsFloat(XboxTextureStates.Get(i, xbox::X_D3DTSS_BUMPENVMAT11));
		stage->BUMPENVLSCALE = AsFloat(XboxTextureStates.Get(i, xbox::X_D3DTSS_BUMPENVLSCALE));
		stage->BUMPENVLOFFSET = AsFloat(XboxTextureStates.Get(i, xbox::X_D3DTSS_BUMPENVLOFFSET));
		stage->COLORKEYCOLOR = XboxTextureStates.Get(i, xbox::X_D3DTSS_COLORKEYCOLOR);
	}

	const int size = (sizeof(FixedFunctionPixelShaderState) + 16 - 1) / 16;
	g_pD3DDevice->SetPixelShaderConstantF(0, (float*)&ffPsState, size);
}

bool g_UseFixedFunctionPixelShader = true;
void DxbxUpdateActivePixelShader() // NOPATCH
{
  // The first RenderState is PSAlpha,
  // The pixel shader is stored in pDevice->m_pPixelShader
  // For now, we still patch SetPixelShader and read from there...

  // Use the pixel shader stored in D3D__RenderState rather than the set handle
  // This allows changes made via SetRenderState to actually take effect!
  // NOTE: PSTextureModes is in a different location in the X_D3DPIXELSHADERDEF than in Render State mappings
  // All other fields are the same.
  // We cast D3D__RenderState to a pPSDef for these fields, but
  // manually read from D3D__RenderState[X_D3DRS_PSTEXTUREMODES] for that one field.
  // See D3DDevice_SetPixelShaderCommon which implements this

  const xbox::X_D3DPIXELSHADERDEF *pPSDef = g_pXbox_PixelShader != nullptr ? (xbox::X_D3DPIXELSHADERDEF*)(XboxRenderStates.GetPixelShaderRenderStatePointer()) : nullptr;
  if (pPSDef == nullptr) {
	IDirect3DPixelShader* pShader = nullptr;
	if (g_UseFixedFunctionPixelShader) {
		pShader = GetFixedFunctionShader();
		UpdateFixedFunctionPixelShaderState();
	}

    g_pD3DDevice->SetPixelShader(pShader);
    return;
  }

  // Create a copy of the pixel shader definition, as it is residing in render state register slots :
  CxbxPSDef CompletePSDef;
  CompletePSDef.PSDef = *pPSDef;
  // Copy-in the PSTextureModes value which is stored outside the range of Xbox pixel shader render state slots :
  CompletePSDef.PSDef.PSTextureModes = XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_PSTEXTUREMODES);
  // Fetch all other values that are used in the IsEquivalent check :
  CompletePSDef.SnapshotRuntimeVariables();

  // Now, see if we already have a shader compiled for this definition :
  // TODO : Change g_RecompiledPixelShaders into an unordered_map, hash just the identifying PSDef members, and add cache eviction (clearing host resources when pruning)
  const PSH_RECOMPILED_SHADER* RecompiledPixelShader = nullptr;
  for (const auto& it : g_RecompiledPixelShaders) {
    if (CompletePSDef.IsEquivalent(it.CompletePSDef)) {
      RecompiledPixelShader = &it;
      break;
    }
  }

  // If none was found, recompile this shader and remember it :
  if (RecompiledPixelShader == nullptr) {
    // Recompile this pixel shader :
    g_RecompiledPixelShaders.push_back(CxbxRecompilePixelShader(CompletePSDef));
    RecompiledPixelShader = &g_RecompiledPixelShaders.back();
  }

  // Switch to the converted pixel shader (if it's any different from our currently active
  // pixel shader, to avoid many unnecessary state changes on the local side).
  Microsoft::WRL::ComPtr<IDirect3DPixelShader> CurrentPixelShader;
  g_pD3DDevice->GetPixelShader(/*out*/CurrentPixelShader.GetAddressOf());
  if (CurrentPixelShader.Get() != RecompiledPixelShader->ConvertedPixelShader) {
    g_pD3DDevice->SetPixelShader(RecompiledPixelShader->ConvertedPixelShader);
  }

  //PS_TEXTUREMODES psTextureModes[xbox::X_D3DTS_STAGECOUNT];
  //PSH_XBOX_SHADER::GetPSTextureModes(pPSDef, psTextureModes);
  //
  //for (i = 0; i < xbox::X_D3DTS_STAGECOUNT; i++)
  //{
  //    switch (psTextureModes[i])
  //    {
  //    default:
  //        break;
  //    }
  //}

  // Set constants, not based on g_PixelShaderConstants, but based on
  // the render state slots containing the pixel shader constants,
  // as these could have been updated via SetRenderState or otherwise :
  D3DXCOLOR fColor[PSH_XBOX_CONSTANT_MAX];

  // PSH_XBOX_CONSTANT_C0..C15 are stored as-is in (and should thus be read from) the Xbox render state pixel shader constant slots
  for (unsigned constant_nr = 0; constant_nr < 16; constant_nr++) {
    fColor[PSH_XBOX_CONSTANT_C0 + constant_nr] = XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_PSCONSTANT0_0 + constant_nr); // Note : 0xAARRGGBB format
  }

  fColor[PSH_XBOX_CONSTANT_FC0] = XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_PSFINALCOMBINERCONSTANT0);
  fColor[PSH_XBOX_CONSTANT_FC1] = XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_PSFINALCOMBINERCONSTANT1);

  // Fog requires a constant (as host PS1.4 doesn't support the FOG register)
  // Note : FOG.RGB is correct like this, but FOG.a should be coming
  // from the vertex shader (oFog) - however, D3D8 does not forward this...
  fColor[PSH_XBOX_CONSTANT_FOG] = XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_FOGCOLOR);
#if 0 // New, doesn't work yet
  // Bump Environment Material registers
  for (int stage_nr = 0; stage_nr < xbox::X_D3DTS_STAGECOUNT; stage_nr++) {
    // Note : No loop, because X_D3DTSS_BUMPENVMAT11 and X_D3DTSS_BUMPENVMAT10 are swapped
    fColor[PSH_XBOX_CONSTANT_BEM + stage_nr].r = XboxTextureStates.Get(stage_nr, xbox::X_D3DTSS_BUMPENVMAT00); // Maps to BEM[stage].x
    fColor[PSH_XBOX_CONSTANT_BEM + stage_nr].g = XboxTextureStates.Get(stage_nr, xbox::X_D3DTSS_BUMPENVMAT01); // Maps to BEM[stage].y
    fColor[PSH_XBOX_CONSTANT_BEM + stage_nr].b = XboxTextureStates.Get(stage_nr, xbox::X_D3DTSS_BUMPENVMAT10); // Maps to BEM[stage].z
    fColor[PSH_XBOX_CONSTANT_BEM + stage_nr].a = XboxTextureStates.Get(stage_nr, xbox::X_D3DTSS_BUMPENVMAT11); // Maps to BEM[stage].w
  }

  // Bump map Luminance registers
  for (int stage_nr = 0; stage_nr < xbox::X_D3DTS_STAGECOUNT; stage_nr++) {
	  fColor[PSH_XBOX_CONSTANT_LUM + stage_nr].r = XboxTextureStates.Get(stage_nr, xbox::X_D3DTSS_BUMPENVLSCALE); // Maps to LUM[stage].x
	  fColor[PSH_XBOX_CONSTANT_LUM + stage_nr].g = XboxTextureStates.Get(stage_nr, xbox::X_D3DTSS_BUMPENVLOFFSET); // Maps to LUM[stage].y
	  fColor[PSH_XBOX_CONSTANT_LUM + stage_nr].b = 0;
	  fColor[PSH_XBOX_CONSTANT_LUM + stage_nr].a = 0;
  }
#else
  for (int i = 0; i < PSH_XBOX_CONSTANT_MAX; i++) {
    switch (i) {
      case PSH_XBOX_CONSTANT_BEM + 0:
      case PSH_XBOX_CONSTANT_BEM + 1:
      case PSH_XBOX_CONSTANT_BEM + 2:
      case PSH_XBOX_CONSTANT_BEM + 3:
      {
        int stage_nr = i - PSH_XBOX_CONSTANT_BEM;
        DWORD* value = (DWORD*)&fColor[i];; // Note : This overlays D3DXCOLOR's FLOAT r, g, b, a

        g_pD3DDevice->GetTextureStageState(stage_nr, D3DTSS_BUMPENVMAT00, &value[0]); // Maps to BEM[stage].x
        g_pD3DDevice->GetTextureStageState(stage_nr, D3DTSS_BUMPENVMAT01, &value[1]); // Maps to BEM[stage].y
        g_pD3DDevice->GetTextureStageState(stage_nr, D3DTSS_BUMPENVMAT10, &value[2]); // Maps to BEM[stage].z
        g_pD3DDevice->GetTextureStageState(stage_nr, D3DTSS_BUMPENVMAT11, &value[3]); // Maps to BEM[stage].w
        // Note : The TSS values being read here, have been transfered from Xbox to host in XboxTextureStateConverter::Apply()
        break;
      }
      case PSH_XBOX_CONSTANT_LUM + 0:
      case PSH_XBOX_CONSTANT_LUM + 1:
      case PSH_XBOX_CONSTANT_LUM + 2:
      case PSH_XBOX_CONSTANT_LUM + 3:
      {
        int stage_nr = i - PSH_XBOX_CONSTANT_LUM;
        DWORD* value = (DWORD*)&fColor[i]; // Note : This overlays D3DXCOLOR's FLOAT r, g, b, a

        g_pD3DDevice->GetTextureStageState(stage_nr, D3DTSS_BUMPENVLSCALE,  &value[0]); // Maps to LUM[stage].x
        g_pD3DDevice->GetTextureStageState(stage_nr, D3DTSS_BUMPENVLOFFSET, &value[1]); // Maps to LUM[stage].y
        value[2] = 0;
        value[3] = 0;
        break;
      }
    }
  }
#endif

  // Control whether to use front or back diffuse/specular colours
  // This factor should be multipled with VFACE
  // Test cases:
  // Amped (snowboard trails should use front colours, but use both CW and CCW winding)
  // TwoSidedLighting sample
  float frontfaceFactor = 0; // 0 == always use the front colours
  if (XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_TWOSIDEDLIGHTING)) {
	  LOG_TEST_CASE("Two sided lighting");
	  // VFACE is positive for clockwise faces
	  // If Xbox designates counter-clockwise as front-facing, we invert VFACE
	  auto cwFrontface = XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_FRONTFACE) == 0x900; // clockwise; = NV097_SET_FRONT_FACE_V_CW = NV2A_FRONT_FACE_CW
	  frontfaceFactor = cwFrontface ? 1.0 : -1.0;
  }
  fColor[PSH_XBOX_CONSTANT_FRONTFACE_FACTOR].r = frontfaceFactor;

  // Assume all constants are in use (this is much easier than tracking them for no other purpose than to skip a few here)
  // Read the color from the corresponding render state slot :
  // Set all host constant values using a single call:
  g_pD3DDevice->SetPixelShaderConstantF(0, reinterpret_cast<const float*>(fColor), PSH_XBOX_CONSTANT_MAX);
}
