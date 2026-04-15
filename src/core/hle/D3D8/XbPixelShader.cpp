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
#include "core\hle\D3D8\Rendering\EmuD3D8.h" // For g_pD3DDevice, g_pXbox_PixelShader
#include "core\hle\D3D8\Rendering\Shader.h" // For g_ShaderSources
#include "core\hle\D3D8\XbPixelShader.h"
#include "core\hle\D3D8\Rendering\PixelShader.h" // EmuCompilePixelShader
#include "core\hle\D3D8\XbD3D8Logging.h" // For D3DErrorString()

#include "core\kernel\init\CxbxKrnl.h" // For CxbxrAbort()
#include "util\hasher.h"
#include "core\hle\D3D8\Rendering\FixedFunctionPixelShader.hlsli"
#include "common/FilePaths.hpp" // For szFilePath_CxbxReloaded_Exe

#include <assert.h> // assert()
#include <process.h>
#include <locale.h>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "Rendering\RenderStates.h" // For XboxRenderStateConverter
#include "Rendering\TextureStates.h" // For XboxTextureStateConverter

#include <wrl/client.h>


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

