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
#define LOG_PREFIX CXBXR_MODULE::D3D8

#include "core\kernel\support\Emu.h"
#include "core\hle\D3D8\Rendering\RenderGlobals.h"
#include "core\hle\D3D8\Rendering\Shaders\Shader.h"
#include "core\hle\D3D8\XbPixelShader.h"
#include "core\hle\D3D8\Rendering\Shaders\PixelShader.h"
#include "core\hle\D3D8\XbD3D8Logging.h"
#include "core\hle\D3D8\XbConvert.h"
#include "core\kernel\init\CxbxKrnl.h"
#include "util\hasher.h"
#include "core\hle\D3D8\Rendering\Shaders\FixedFunctionPixelShader.hlsli"
#include "common/FilePaths.hpp"
#include <assert.h>
#include <process.h>
#include <locale.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "Rendering\RenderStates.h"
#include "Rendering\TextureStates.h"
#include <wrl/client.h>
#include <cstring> // For std::memcpy

// The Xbox kernel's SetRenderState_FogColor swaps R↔B before calling
// SetRenderState_Simple, so D3D__RenderState stores the fog color in NV2A
// ABGR format (0xAABBGGRR) rather than D3DCOLOR ARGB (0xAARRGGBB).
// This helper reverses that swap when reading the stored value.
static inline DWORD FogColor_ABGR_to_ARGB(DWORD color)
{
	return (color & 0xFF00FF00) | ((color & 0x00FF0000) >> 16) | ((color & 0x000000FF) << 16);
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

				// Test-case: MS-033 Crimson Skies (Plane texturing in-game and selection menu)
				if (ActiveTextureTypes[i] == xbox::X_D3DRTYPE_CUBETEXTURE && RC.PSTextureModes[i] == PS_TEXTUREMODES_DOT_STR_3D) {
					EmuLog(LOG_LEVEL::WARNING, "DOT_STR_3D sampling is used with a cubemap texture - using DOT_STR_CUBE sampling instead");
					RC.PSTextureModes[i] = PS_TEXTUREMODES_DOT_STR_CUBE;
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
			DWORD hRet = CxbxCreatePixelShader(pShader->GetBufferPointer(), pShader->GetBufferSize(), &(Result.ConvertedPixelShader));
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
// Texture color sign
constexpr int PSH_XBOX_CONSTANT_COLORSIGN = PSH_XBOX_CONSTANT_FOG + 1; // = 19..22
// Texture color key operation
constexpr int PSH_XBOX_CONSTANT_COLORKEYOP = PSH_XBOX_CONSTANT_COLORSIGN + 4; // = 23..26
// Texture color key color
constexpr int PSH_XBOX_CONSTANT_COLORKEYCOLOR = PSH_XBOX_CONSTANT_COLORKEYOP + 4; // = 27..30
// Bump Environment Material registers
constexpr int PSH_XBOX_CONSTANT_BEM = PSH_XBOX_CONSTANT_COLORKEYCOLOR + 4; // = 31..34
// Bump map Luminance registers
constexpr int PSH_XBOX_CONSTANT_LUM = PSH_XBOX_CONSTANT_BEM + 4; // = 35..38
// Which winding order to consider as the front face
constexpr int PSH_XBOX_CONSTANT_FRONTFACE_FACTOR = PSH_XBOX_CONSTANT_LUM + 4; // = 39
// Fog table information {Table mode, density, start and end}
constexpr int CXBX_D3DPS_CONSTREG_FOGINFO = PSH_XBOX_CONSTANT_FRONTFACE_FACTOR + 1; // = 40
//Fog enable flag
constexpr int PSH_XBOX_CONSTANT_FOGENABLE = CXBX_D3DPS_CONSTREG_FOGINFO + 1; // = 41
// D3D11: Per-stage texture format channel fixup (packed float4, one component per stage)
constexpr int PSH_XBOX_CONSTANT_TEXFMTFIXUP = PSH_XBOX_CONSTANT_FOGENABLE + 1; // = 42
// D3D11: Alpha test state (enable, ref, func, 0) — D3D11 has no fixed-function alpha test
constexpr int PSH_XBOX_CONSTANT_ALPHATEST = PSH_XBOX_CONSTANT_TEXFMTFIXUP + 1; // = 43
// This concludes the set of constants that need to be set on host :
constexpr int PSH_XBOX_CONSTANT_MAX = PSH_XBOX_CONSTANT_ALPHATEST + 1; // = 44

std::string_view GetD3DTOPString(int d3dtop) {
	static constexpr std::string_view opToString[] = {
		"X_D3DTOP_DISABLE", // 0 (initialized for disabled stages)
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

	if (d3dtop < 0 || d3dtop > 26) {
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

	// Support hotloading hlsl
	static int pixelShaderVersion = -1;
	int shaderVersion = g_ShaderSources.Update();
	if (pixelShaderVersion != shaderVersion) {
		pixelShaderVersion = shaderVersion;
		CxbxRawSetPixelShader(nullptr);

		for (auto& hostShader : ffPsCache) {
			if (hostShader.second)
				hostShader.second->Release();
		}

		ffPsCache.clear();
	}

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
	std::string hlslTemplate = g_ShaderSources.fixedFunctionPixelShaderHlsl;

	// In D3D9 it seems we need to know hardcode if we're doing a 2D or 3D lookup
	const std::string sampleTypePattern = "TEXTURE_SAMPLE_TYPE;";
	auto sampleTypeReplace = hlslTemplate.find(sampleTypePattern);
	std::string finalShader = hlslTemplate;

	if (sampleTypeReplace != std::string::npos) {
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

		finalShader = hlslTemplate.replace(sampleTypeReplace, sampleTypePattern.size(), sampleTypeString.str());
	}

	// Hardcode the texture stage operations and arguments
	// So the shader handles exactly one combination of values
	const std::string stageDef = "// STAGE DEFINITIONS";
	auto stageDefInsert = finalShader.find(stageDef);
	if (stageDefInsert != std::string::npos) {
		stageDefInsert += stageDef.size();

		std::stringstream stageSetup;
		stageSetup << '\n';

		for (int i = 0; i < 4; i++) {
			// Even when a stage is disabled, we still have to fully initialize it's values, to prevent
			// "error X4000: variable 'stages' used without having been completely initialized"
			std::string target = "stages[" + std::to_string(i) + "].";

			auto s = states[i];
			stageSetup << target << "COLOROP = " << GetD3DTOPString(static_cast<int>(s.COLOROP)) << ";\n";

			stageSetup << target << "COLORARG0 = " << GetD3DTASumString(static_cast<int>(s.COLORARG0)) << ";\n";
			stageSetup << target << "COLORARG1 = " << GetD3DTASumString(static_cast<int>(s.COLORARG1)) << ";\n";
			stageSetup << target << "COLORARG2 = " << GetD3DTASumString(static_cast<int>(s.COLORARG2)) << ";\n";

			stageSetup << target << "ALPHAOP = " << GetD3DTOPString(static_cast<int>(s.ALPHAOP)) << ";\n";

			stageSetup << target << "ALPHAARG0 = " << GetD3DTASumString(static_cast<int>(s.ALPHAARG0)) << ";\n";
			stageSetup << target << "ALPHAARG1 = " << GetD3DTASumString(static_cast<int>(s.ALPHAARG1)) << ";\n";
			stageSetup << target << "ALPHAARG2 = " << GetD3DTASumString(static_cast<int>(s.ALPHAARG2)) << ";\n";

			stageSetup << target << "RESULTARG = " << GetD3DTASumString(static_cast<int>(s.RESULTARG), false) << ";\n";
			stageSetup << '\n';
		}

		finalShader = finalShader.insert(stageDefInsert, stageSetup.str());
	}

	// Compile the shader
	ID3DBlob* pShaderBlob = nullptr;

	auto hlslDir = std::filesystem::path(szFilePath_CxbxReloaded_Exe)
		.parent_path()
		.append("hlsl");

	auto pseudoFileName = "FixedFunctionPixelShader-" + std::to_string(key) + ".hlsl";
	auto pseudoSourceFile = hlslDir.append(pseudoFileName).string();
	EmuCompileShader(finalShader, _9_11("ps_3_0", "ps_5_0"), &pShaderBlob, pseudoSourceFile.c_str());

	IDirect3DPixelShader* pShader = nullptr;
	if (pShaderBlob) {
		// Create shader object for the device
		auto hRet = CxbxCreatePixelShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), &pShader);
		if (hRet != S_OK) {
			EmuLog(LOG_LEVEL::ERROR2, "Failed to compile fixed function pixel shader");
		}
		pShaderBlob->Release();
	}

	// Insert the shader into the cache
	ffPsCache[key] = pShader;

	return pShader;
};

float AsFloat(uint32_t value)
{
	float f; std::memcpy(&f, &value, sizeof(f)); return f;
}

// Determines the Cxbx ColorSign requirement, as handled in the HLSL shaders by PerformColorSign()
float CxbxComponentColorSignFromXboxAndHost(bool XboxMarksComponentSigned, bool HostComponentIsSigned)
{
	// Equal "signedness" between Xbox and host implies we must not convert the component scale :
	if (XboxMarksComponentSigned == HostComponentIsSigned)
		return 0.0f;

	// Xbox wants the components to be signed (even though host has them unsigned)
	if (XboxMarksComponentSigned)
		return 1.0f; // Mark the component for scaling from unsigned_to_signed

	// Xbox doesn't want signed values, but host has them signed :
	return -1.0f; // Mark the component for scaling from signed_to_unsigned
}

float CxbxGetTexFmtFixup(int stage_nr)
{
	using namespace FixedFunctionPixelShader;

#ifdef CXBX_USE_D3D11
	auto pXboxTex = g_pXbox_SetTexture[stage_nr];
	if (pXboxTex == xbox::zeroptr)
		return TEXFMTFIXUP_IDENTITY;

	xbox::X_D3DFORMAT xboxFmt = GetXboxPixelContainerFormat((xbox::X_D3DPixelContainer*)pXboxTex);
	switch (xboxFmt) {
	case xbox::X_D3DFMT_L8:
	case xbox::X_D3DFMT_LIN_L8:
	case xbox::X_D3DFMT_L16:
	case xbox::X_D3DFMT_LIN_L16:
		return TEXFMTFIXUP_LUM;
	case xbox::X_D3DFMT_A8L8:
	case xbox::X_D3DFMT_LIN_A8L8:
		return TEXFMTFIXUP_ALUM;
	// B8G8R8A8 and R8G8B8A8 use GBAR/ABGR swizzles when uploaded raw
	// (requires corresponding skip of CPU conversion in HostResourceCreate.cpp)
	// Exception: render targets use B8G8R8A8_UNORM and don't need a swizzle.
	case xbox::X_D3DFMT_B8G8R8A8:
	case xbox::X_D3DFMT_LIN_B8G8R8A8:
	{
		// Check if the host texture is B8G8R8A8_UNORM (render target) — data is already correct
		auto key = GetHostResourceKey(pXboxTex, stage_nr);
		auto& cache = GetResourceCache(key);
		auto it = cache.find(key);
		if (it != cache.end() && it->second.HostFormat == EMUFMT_A8R8G8B8)
			return TEXFMTFIXUP_IDENTITY;
		return TEXFMTFIXUP_GBAR;
	}
	case xbox::X_D3DFMT_R8G8B8A8:
	case xbox::X_D3DFMT_LIN_R8G8B8A8:
	{
		auto key = GetHostResourceKey(pXboxTex, stage_nr);
		auto& cache = GetResourceCache(key);
		auto it = cache.find(key);
		if (it != cache.end() && it->second.HostFormat == EMUFMT_A8R8G8B8)
			return TEXFMTFIXUP_IDENTITY;
		return TEXFMTFIXUP_ABGR;
	}
	default:
		break;
	}
#endif
	return TEXFMTFIXUP_IDENTITY;
}

D3DXCOLOR CxbxCalcColorSign(int stage_nr)
{
	// Initially use what the running executable put in COLORSIGN :
	DWORD XboxColorSign = XboxTextureStates.Get(stage_nr, xbox::X_D3DTSS_COLORSIGN);

	{ // This mimicks behaviour of XDK LazySetShaderStageProgram, which we bypass due to our drawing patches without trampolines.
		// When bump environment mapping is enabled
		if (XboxTextureStates.Get(stage_nr, xbox::X_D3DTSS_COLOROP) >= xbox::X_D3DTOP_BUMPENVMAP)
			// Always mark the blue (alias for U) and green (alias for  V) color channels as signed :
			XboxColorSign |= xbox::X_D3DTSIGN_GSIGNED | xbox::X_D3DTSIGN_BSIGNED;
	}

#if 0 // When this block is enabled, XDK samples BumpEarth and BumpLens turn red-ish, so keep this off for now...
	// Check if the pixel shader specifies bump mapping for this stage (TODO : How to handle this with the fixed function shader?)
	DWORD PSTextureModes = XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_PSTEXTUREMODES);
	PS_TEXTUREMODES StageTextureMode = (PS_TEXTUREMODES)((PSTextureModes >> (stage_nr * 5)) & PS_TEXTUREMODES_MASK);
	if (StageTextureMode == PS_TEXTUREMODES_BUMPENVMAP || StageTextureMode == PS_TEXTUREMODES_BUMPENVMAP_LUM)
		XboxColorSign |= xbox::X_D3DTSIGN_GSIGNED | xbox::X_D3DTSIGN_BSIGNED;

#endif
	// Host D3DFMT's with one or more signed components : D3DFMT_V8U8, D3DFMT_Q8W8V8U8, D3DFMT_V16U16, D3DFMT_Q16W16V16U16, D3DFMT_CxV8U8
	EMUFORMAT H/*ostTextureFormat*/ = g_HostTextureFormats[stage_nr];
	// See https://docs.microsoft.com/en-us/windows/win32/direct3d9/bump-map-pixel-formats
	// No need to check for unused formats : D3DFMT_Q16W16V16U16, D3DFMT_CxV8U8, D3DFMT_A2W10V10U10
#if 0 // Original signed-ness checking code gave effectively this :
	// Host format     | Signed components
	// ----------------+------------------
	// D3DFMT_Q8W8V8U8 | A,R,G,B
	// D3DFMT_L6V5U5   |     G,B
	// D3DFMT_V8U8     |   R,G
	// D3DFMT_V16U16   |   R,G
	// D3DFMT_X8L8V8U8 |   R,G
	bool HostTextureFormatIsSignedForA = (H == EMUFMT_Q8W8V8U8);
	bool HostTextureFormatIsSignedForR = (H == EMUFMT_Q8W8V8U8)                         || (H == EMUFMT_V8U8) || (H == EMUFMT_V16U16) || (H == EMUFMT_X8L8V8U8);
	bool HostTextureFormatIsSignedForG = (H == EMUFMT_Q8W8V8U8) || (H == EMUFMT_L6V5U5) || (H == EMUFMT_V8U8) || (H == EMUFMT_V16U16) || (H == EMUFMT_X8L8V8U8);
	bool HostTextureFormatIsSignedForB = (H == EMUFMT_Q8W8V8U8) || (H == EMUFMT_L6V5U5);
#else // New, as experimentally discovered by medievil :
	// Host format     | Signed components
	// ----------------+------------------
	// D3DFMT_Q8W8V8U8 | A,R,G,B
	// D3DFMT_L6V5U5   | A,R
	// D3DFMT_V8U8     |   R,G
	// D3DFMT_V16U16   |   R,G
	// D3DFMT_X8L8V8U8 |   R,G
	// TODO : Verify D3DFMT_L6V5U5 indeed maps to A,R (instead of G,B).
	// If not, research why this (then incorret) change *does* improve both BumpEarth samples
	// (while keeping BumpLens and JSFR boost dash effect working). Perhaps duplicate signed range conversion in the shader?
	bool HostTextureFormatIsSignedForA = (H == EMUFMT_Q8W8V8U8) || (H == EMUFMT_L6V5U5);
	bool HostTextureFormatIsSignedForR = (H == EMUFMT_Q8W8V8U8) || (H == EMUFMT_L6V5U5) || (H == EMUFMT_V8U8) || (H == EMUFMT_V16U16) || (H == EMUFMT_X8L8V8U8);
	bool HostTextureFormatIsSignedForG = (H == EMUFMT_Q8W8V8U8)                         || (H == EMUFMT_V8U8) || (H == EMUFMT_V16U16) || (H == EMUFMT_X8L8V8U8);
	bool HostTextureFormatIsSignedForB = (H == EMUFMT_Q8W8V8U8);
#endif
	D3DXCOLOR CxbxColorSign;
	CxbxColorSign.r = CxbxComponentColorSignFromXboxAndHost(XboxColorSign & xbox::X_D3DTSIGN_RSIGNED, HostTextureFormatIsSignedForR); // Maps to COLORSIGN.r
	CxbxColorSign.g = CxbxComponentColorSignFromXboxAndHost(XboxColorSign & xbox::X_D3DTSIGN_GSIGNED, HostTextureFormatIsSignedForG); // Maps to COLORSIGN.g
	CxbxColorSign.b = CxbxComponentColorSignFromXboxAndHost(XboxColorSign & xbox::X_D3DTSIGN_BSIGNED, HostTextureFormatIsSignedForB); // Maps to COLORSIGN.b
	CxbxColorSign.a = CxbxComponentColorSignFromXboxAndHost(XboxColorSign & xbox::X_D3DTSIGN_ASIGNED, HostTextureFormatIsSignedForA); // Maps to COLORSIGN.a
	return CxbxColorSign;
}

// Set constant state for the fixed function pixel shader
void UpdateFixedFunctionPixelShaderState()
{
	using namespace FixedFunctionPixelShader;

	FixedFunctionPixelShaderState ffPsState;
	{ D3DXCOLOR c(XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_TEXTUREFACTOR)); ffPsState.TextureFactor = D3DXVECTOR4(c.r, c.g, c.b, c.a); }
	ffPsState.SpecularEnable = static_cast<float>(XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_SPECULARENABLE));
	ffPsState.FogEnable = static_cast<float>(XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_FOGENABLE));
	{ D3DXCOLOR c(FogColor_ABGR_to_ARGB(XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_FOGCOLOR))); ffPsState.FogColor = D3DXVECTOR3(c.r, c.g, c.b); }
	ffPsState.FogTableMode = static_cast<float>(XboxRenderStates.GetXboxRenderState(xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGTABLEMODE));
	ffPsState.FogDensity = XboxRenderStates.GetXboxRenderStateAsFloat(xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGDENSITY);
	ffPsState.FogStart = XboxRenderStates.GetXboxRenderStateAsFloat(xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGSTART);
	ffPsState.FogEnd = XboxRenderStates.GetXboxRenderStateAsFloat(xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGEND);
	// Alpha test state (D3D11 has no fixed-function alpha test)
	ffPsState.AlphaTest.x = static_cast<float>(XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_ALPHATESTENABLE));
	ffPsState.AlphaTest.y = static_cast<float>(XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_ALPHAREF)) / 255.0f;
	ffPsState.AlphaTest.z = static_cast<float>(EmuXB2PC_D3DCMPFUNC((xbox::X_D3DCMPFUNC)XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_ALPHAFUNC)));
	ffPsState.AlphaTest.w = 0;
	// Texture state
	for (int i = 0; i < xbox::X_D3DTS_STAGECOUNT; i++) {
		auto stage = &ffPsState.stages[i];
		stage->COLORKEYOP = (float)XboxTextureStates.Get(i, xbox::X_D3DTSS_COLORKEYOP);
		auto CxbxColorSign = CxbxCalcColorSign(i);
		stage->COLORSIGN.x = CxbxColorSign.r;
		stage->COLORSIGN.y = CxbxColorSign.g;
		stage->COLORSIGN.z = CxbxColorSign.b;
		stage->COLORSIGN.w = CxbxColorSign.a;
		stage->ALPHAKILL = (float)XboxTextureStates.Get(i, xbox::X_D3DTSS_ALPHAKILL);
		stage->BUMPENVMAT00 = AsFloat(XboxTextureStates.Get(i, xbox::X_D3DTSS_BUMPENVMAT00));
		stage->BUMPENVMAT01 = AsFloat(XboxTextureStates.Get(i, xbox::X_D3DTSS_BUMPENVMAT01));
		stage->BUMPENVMAT10 = AsFloat(XboxTextureStates.Get(i, xbox::X_D3DTSS_BUMPENVMAT10));
		stage->BUMPENVMAT11 = AsFloat(XboxTextureStates.Get(i, xbox::X_D3DTSS_BUMPENVMAT11));
		stage->BUMPENVLSCALE = AsFloat(XboxTextureStates.Get(i, xbox::X_D3DTSS_BUMPENVLSCALE));
		stage->BUMPENVLOFFSET = AsFloat(XboxTextureStates.Get(i, xbox::X_D3DTSS_BUMPENVLOFFSET));
		{ D3DXCOLOR c(XboxTextureStates.Get(i, xbox::X_D3DTSS_COLORKEYCOLOR)); stage->COLORKEYCOLOR = D3DXVECTOR4(c.r, c.g, c.b, c.a); }
		stage->TEXFMTFIXUP = CxbxGetTexFmtFixup(i);
	}

	const int size = (sizeof(FixedFunctionPixelShaderState) + 16 - 1) / 16;
	CxbxSetPixelShaderConstantF(0, (float*)&ffPsState, size);
}

static IDirect3DPixelShader* g_pActivePixelShader = nullptr; // TODO : Reset when device resets!

void CxbxSetPixelShader(IDirect3DPixelShader* pPixelShader)
{
	// Here no call to (PS)GetPixelShader, but our own state tracking; See https://gamedev.stackexchange.com/a/88117
	if (g_pActivePixelShader == pPixelShader)
		return;

	// Switch to the converted pixel shader (if it's any different from our currently active
	// pixel shader, to avoid many unnecessary state changes on the local side).
	CxbxRawSetPixelShader(pPixelShader);
	g_pActivePixelShader = pPixelShader;
}

bool g_UseFixedFunctionPixelShader = true;
void CxbxUpdateActivePixelShader() // NOPATCH
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

	CxbxSetPixelShader(pShader);
   	return;
  }

  // Create a copy of the pixel shader definition, as it is residing in render state register slots :
  CxbxPSDef CompletePSDef;
  CompletePSDef.PSDef = *pPSDef;
  // Copy-in the PSTextureModes value which is stored outside the range of Xbox pixel shader render state slots :
  CompletePSDef.PSDef.PSTextureModes = XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_PSTEXTUREMODES);
  // Fetch all other values that are used in the IsEquivalent check :
  CompletePSDef.SnapshotRuntimeVariables();

  // Support hotloading hlsl
  static int pixelShaderVersion = -1;
  int shaderVersion = g_ShaderSources.Update();
  if (pixelShaderVersion != shaderVersion) {
	  pixelShaderVersion = shaderVersion;
	  CxbxRawSetPixelShader(nullptr);

	  for (auto& hostShader : g_RecompiledPixelShaders) {
		  if (hostShader.ConvertedPixelShader)
			  hostShader.ConvertedPixelShader->Release();
	  }

	  g_RecompiledPixelShaders.clear();
  }

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

  CxbxSetPixelShader(RecompiledPixelShader->ConvertedPixelShader);

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
  fColor[PSH_XBOX_CONSTANT_FOG] = FogColor_ABGR_to_ARGB(XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_FOGCOLOR));

  // Texture color sign
  for (int stage_nr = 0; stage_nr < xbox::X_D3DTS_STAGECOUNT; stage_nr++) {
   	fColor[PSH_XBOX_CONSTANT_COLORSIGN + stage_nr] = CxbxCalcColorSign(stage_nr);
  }

  // Diagnostic: log non-zero colorsign values (first 10 occurrences)
  {
    static int s_csdiag = 0;
    for (int s = 0; s < xbox::X_D3DTS_STAGECOUNT && s_csdiag < 10; s++) {
      auto& cs = fColor[PSH_XBOX_CONSTANT_COLORSIGN + s];
      if (cs.r != 0 || cs.g != 0 || cs.b != 0 || cs.a != 0) {
        DWORD xboxCS = XboxTextureStates.Get(s, xbox::X_D3DTSS_COLORSIGN);
        DWORD colorOp = XboxTextureStates.Get(s, xbox::X_D3DTSS_COLOROP);
        s_csdiag++;
        EmuLog(LOG_LEVEL::WARNING, "ColorSign diag [%d]: stage=%d COLORSIGN=(%f,%f,%f,%f) XboxCS=0x%08X ColorOp=%u HostFmt=%d",
          s_csdiag, s, cs.r, cs.g, cs.b, cs.a, xboxCS, colorOp, (int)g_HostTextureFormats[s]);
      }
    }
  }

  // Texture color key operation
  for (int stage_nr = 0; stage_nr < xbox::X_D3DTS_STAGECOUNT; stage_nr++) {
   	fColor[PSH_XBOX_CONSTANT_COLORKEYOP + stage_nr].r = (float)XboxTextureStates.Get(stage_nr, xbox::X_D3DTSS_COLORKEYOP);
  }

  // Texture color key color
  for (int stage_nr = 0; stage_nr < xbox::X_D3DTS_STAGECOUNT; stage_nr++) {
   	fColor[PSH_XBOX_CONSTANT_COLORKEYCOLOR + stage_nr] = XboxTextureStates.Get(stage_nr, xbox::X_D3DTSS_COLORKEYCOLOR);
  }

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
   	   	DWORD* value = (DWORD*)&fColor[i]; // Note : This overlays D3DXCOLOR's FLOAT r, g, b, a

   	   	CxbxGetBumpEnvMatrix(stage_nr, value);
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

   	   	CxbxGetBumpEnvLuminance(stage_nr, value);
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
	  frontfaceFactor = cwFrontface ? 1.0f : -1.0f;
  }
  fColor[PSH_XBOX_CONSTANT_FRONTFACE_FACTOR].r = frontfaceFactor;
  float fogEnable = XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_FOGENABLE) > 0;
  const float fogTableMode = static_cast<float>(XboxRenderStates.GetXboxRenderState(xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGTABLEMODE));
  const float fogDensity = XboxRenderStates.GetXboxRenderStateAsFloat(xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGDENSITY);
  const float fogStart = XboxRenderStates.GetXboxRenderStateAsFloat(xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGSTART);
  const float fogEnd = XboxRenderStates.GetXboxRenderStateAsFloat(xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGEND);
  
  fColor[CXBX_D3DPS_CONSTREG_FOGINFO].r = fogTableMode;
  fColor[CXBX_D3DPS_CONSTREG_FOGINFO].g = fogDensity;
  fColor[CXBX_D3DPS_CONSTREG_FOGINFO].b = fogStart;
  fColor[CXBX_D3DPS_CONSTREG_FOGINFO].a = fogEnd;
  fColor[PSH_XBOX_CONSTANT_FOGENABLE].r = fogEnable;
  
  // D3D11: Compute per-stage texture format channel fixup
  fColor[PSH_XBOX_CONSTANT_TEXFMTFIXUP].r = CxbxGetTexFmtFixup(0);
  fColor[PSH_XBOX_CONSTANT_TEXFMTFIXUP].g = CxbxGetTexFmtFixup(1);
  fColor[PSH_XBOX_CONSTANT_TEXFMTFIXUP].b = CxbxGetTexFmtFixup(2);
  fColor[PSH_XBOX_CONSTANT_TEXFMTFIXUP].a = CxbxGetTexFmtFixup(3);

  // D3D11: Alpha test state (D3D11 has no fixed-function alpha test, so we implement it in the pixel shader)
  fColor[PSH_XBOX_CONSTANT_ALPHATEST].r = static_cast<float>(XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_ALPHATESTENABLE));
  fColor[PSH_XBOX_CONSTANT_ALPHATEST].g = static_cast<float>(XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_ALPHAREF)) / 255.0f;
  fColor[PSH_XBOX_CONSTANT_ALPHATEST].b = static_cast<float>(EmuXB2PC_D3DCMPFUNC((xbox::X_D3DCMPFUNC)XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_ALPHAFUNC)));
  fColor[PSH_XBOX_CONSTANT_ALPHATEST].a = 0;

  // Assume all constants are in use (this is much easier than tracking them for no other purpose than to skip a few here)
  // Read the color from the corresponding render state slot :
  // Set all host constant values using a single call:
  CxbxSetPixelShaderConstantF(0, reinterpret_cast<const float*>(fColor), PSH_XBOX_CONSTANT_MAX);

  // Diagnostic: dump PS constant values (first 3 occurrences only)
  {
    static int s_diagCount = 0;
    if (s_diagCount < 3) {
      s_diagCount++;
      EmuLog(LOG_LEVEL::INFO, "PS diag [%d]: fog=(%f,%f,%f,%f) fogInfo=(%f,%f,%f,%f) fogEnable=%f",
        s_diagCount,
        fColor[PSH_XBOX_CONSTANT_FOG].r, fColor[PSH_XBOX_CONSTANT_FOG].g,
        fColor[PSH_XBOX_CONSTANT_FOG].b, fColor[PSH_XBOX_CONSTANT_FOG].a,
        fColor[CXBX_D3DPS_CONSTREG_FOGINFO].r, fColor[CXBX_D3DPS_CONSTREG_FOGINFO].g,
        fColor[CXBX_D3DPS_CONSTREG_FOGINFO].b, fColor[CXBX_D3DPS_CONSTREG_FOGINFO].a,
        fColor[PSH_XBOX_CONSTANT_FOGENABLE].r);
      EmuLog(LOG_LEVEL::INFO, "PS diag [%d]: c0=(%f,%f,%f,%f) c1=(%f,%f,%f,%f) texfmt=(%f,%f,%f,%f)",
        s_diagCount,
        fColor[0].r, fColor[0].g, fColor[0].b, fColor[0].a,
        fColor[1].r, fColor[1].g, fColor[1].b, fColor[1].a,
        fColor[PSH_XBOX_CONSTANT_TEXFMTFIXUP].r, fColor[PSH_XBOX_CONSTANT_TEXFMTFIXUP].g,
        fColor[PSH_XBOX_CONSTANT_TEXFMTFIXUP].b, fColor[PSH_XBOX_CONSTANT_TEXFMTFIXUP].a);
    }
  }
}
