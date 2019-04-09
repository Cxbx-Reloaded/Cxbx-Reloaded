// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
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
// *  (c) 2018 Luke Usher <luke.usher@outlook.com>
// *
// *  All rights reserved
// *
// ******************************************************************

#include "core\kernel\init\CxbxKrnl.h"
#include "core\kernel\support\Emu.h"
#include "core\kernel\support\EmuXTL.h"
#include "Patches.hpp"
#include "Intercept.hpp"

#include <map>
#include <unordered_map>
#include <subhook.h>

typedef struct {
	const void* patchFunc;		// Function pointer of the patch in Cxbx-R codebase
	const uint32_t flags;		// Patch Flags
} xbox_patch_t;

const uint32_t PATCH_ALWAYS = 1 << 0;
const uint32_t PATCH_HLE_D3D = 1 << 1;
const uint32_t PATCH_HLE_DSOUND = 1 << 2;
const uint32_t PATCH_HLE_OHCI = 1 << 3;

#define PATCH_ENTRY(Name, Func, Flags) \
    { Name, { &Func, Flags} }

// Map of Xbox Patch names to Emulator Patches
// A std::string is used as it's possible for a symbol to have multiple names
// This allows for the eventual importing of Dxbx symbol files and even IDA signatures too!
std::map<const std::string, const xbox_patch_t> g_PatchTable = {
	// Direct3D
	PATCH_ENTRY("D3DCubeTexture_GetCubeMapSurface", XTL::EMUPATCH(D3DCubeTexture_GetCubeMapSurface), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DCubeTexture_GetCubeMapSurface2", XTL::EMUPATCH(D3DCubeTexture_GetCubeMapSurface2), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_Begin", XTL::EMUPATCH(D3DDevice_Begin), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_BeginPush", XTL::EMUPATCH(D3DDevice_BeginPush), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_BeginPush2", XTL::EMUPATCH(D3DDevice_BeginPush2), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_BeginVisibilityTest", XTL::EMUPATCH(D3DDevice_BeginVisibilityTest), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_BlockOnFence", XTL::EMUPATCH(D3DDevice_BlockOnFence), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_BlockUntilVerticalBlank", XTL::EMUPATCH(D3DDevice_BlockUntilVerticalBlank), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_Clear", XTL::EMUPATCH(D3DDevice_Clear), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_CreateVertexShader", XTL::EMUPATCH(D3DDevice_CreateVertexShader), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_DeleteVertexShader", XTL::EMUPATCH(D3DDevice_DeleteVertexShader), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_DeleteVertexShader_0", XTL::EMUPATCH(D3DDevice_DeleteVertexShader_0), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_DrawIndexedVertices", XTL::EMUPATCH(D3DDevice_DrawIndexedVertices), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_DrawIndexedVerticesUP", XTL::EMUPATCH(D3DDevice_DrawIndexedVerticesUP), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_DrawRectPatch", XTL::EMUPATCH(D3DDevice_DrawRectPatch), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_DrawTriPatch", XTL::EMUPATCH(D3DDevice_DrawTriPatch), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_DrawVertices", XTL::EMUPATCH(D3DDevice_DrawVertices), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_DrawVerticesUP", XTL::EMUPATCH(D3DDevice_DrawVerticesUP), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_EnableOverlay", XTL::EMUPATCH(D3DDevice_EnableOverlay), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_End", XTL::EMUPATCH(D3DDevice_End), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_EndPush", XTL::EMUPATCH(D3DDevice_EndPush), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_EndVisibilityTest", XTL::EMUPATCH(D3DDevice_EndVisibilityTest), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_EndVisibilityTest_0", XTL::EMUPATCH(D3DDevice_EndVisibilityTest_0), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_FlushVertexCache", XTL::EMUPATCH(D3DDevice_FlushVertexCache), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetBackBuffer", XTL::EMUPATCH(D3DDevice_GetBackBuffer), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetBackBuffer2", XTL::EMUPATCH(D3DDevice_GetBackBuffer2), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetDisplayFieldStatus", XTL::EMUPATCH(D3DDevice_GetDisplayFieldStatus), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetGammaRamp", XTL::EMUPATCH(D3DDevice_GetGammaRamp), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetMaterial", XTL::EMUPATCH(D3DDevice_GetMaterial), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetModelView", XTL::EMUPATCH(D3DDevice_GetModelView), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetOverlayUpdateStatus", XTL::EMUPATCH(D3DDevice_GetOverlayUpdateStatus), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetProjectionViewportMatrix", XTL::EMUPATCH(D3DDevice_GetProjectionViewportMatrix), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetShaderConstantMode", XTL::EMUPATCH(D3DDevice_GetShaderConstantMode), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetTransform", XTL::EMUPATCH(D3DDevice_GetTransform), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetVertexShader", XTL::EMUPATCH(D3DDevice_GetVertexShader), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetVertexShaderConstant", XTL::EMUPATCH(D3DDevice_GetVertexShaderConstant), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetVertexShaderDeclaration", XTL::EMUPATCH(D3DDevice_GetVertexShaderDeclaration), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetVertexShaderFunction", XTL::EMUPATCH(D3DDevice_GetVertexShaderFunction), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetVertexShaderInput", XTL::EMUPATCH(D3DDevice_GetVertexShaderInput), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetVertexShaderSize", XTL::EMUPATCH(D3DDevice_GetVertexShaderSize), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetVertexShaderType", XTL::EMUPATCH(D3DDevice_GetVertexShaderType), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetViewportOffsetAndScale", XTL::EMUPATCH(D3DDevice_GetViewportOffsetAndScale), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetVisibilityTestResult", XTL::EMUPATCH(D3DDevice_GetVisibilityTestResult), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_InsertCallback", XTL::EMUPATCH(D3DDevice_InsertCallback), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_InsertFence", XTL::EMUPATCH(D3DDevice_InsertFence), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_IsBusy", XTL::EMUPATCH(D3DDevice_IsBusy), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_IsFencePending", XTL::EMUPATCH(D3DDevice_IsFencePending), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_LightEnable", XTL::EMUPATCH(D3DDevice_LightEnable), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_LoadVertexShader", XTL::EMUPATCH(D3DDevice_LoadVertexShader), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_LoadVertexShaderProgram", XTL::EMUPATCH(D3DDevice_LoadVertexShaderProgram), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_LoadVertexShader_0", XTL::EMUPATCH(D3DDevice_LoadVertexShader_0), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_LoadVertexShader_4", XTL::EMUPATCH(D3DDevice_LoadVertexShader_4), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_MultiplyTransform", XTL::EMUPATCH(D3DDevice_MultiplyTransform), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_PersistDisplay", XTL::EMUPATCH(D3DDevice_PersistDisplay), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_Present", XTL::EMUPATCH(D3DDevice_Present), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_PrimeVertexCache", XTL::EMUPATCH(D3DDevice_PrimeVertexCache), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_RunPushBuffer", XTL::EMUPATCH(D3DDevice_RunPushBuffer), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_RunVertexStateShader", XTL::EMUPATCH(D3DDevice_RunVertexStateShader), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SelectVertexShader", XTL::EMUPATCH(D3DDevice_SelectVertexShader), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SelectVertexShaderDirect", XTL::EMUPATCH(D3DDevice_SelectVertexShaderDirect), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SelectVertexShader_0", XTL::EMUPATCH(D3DDevice_SelectVertexShader_0), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SelectVertexShader_4", XTL::EMUPATCH(D3DDevice_SelectVertexShader_4), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetBackBufferScale", XTL::EMUPATCH(D3DDevice_SetBackBufferScale), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetDepthClipPlanes", XTL::EMUPATCH(D3DDevice_SetDepthClipPlanes), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetFlickerFilter", XTL::EMUPATCH(D3DDevice_SetFlickerFilter), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetFlickerFilter_0", XTL::EMUPATCH(D3DDevice_SetFlickerFilter_0), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetGammaRamp", XTL::EMUPATCH(D3DDevice_SetGammaRamp), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetIndices", XTL::EMUPATCH(D3DDevice_SetIndices), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetLight", XTL::EMUPATCH(D3DDevice_SetLight), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetMaterial", XTL::EMUPATCH(D3DDevice_SetMaterial), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetModelView", XTL::EMUPATCH(D3DDevice_SetModelView), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetPalette", XTL::EMUPATCH(D3DDevice_SetPalette), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetPalette_4", XTL::EMUPATCH(D3DDevice_SetPalette_4), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetPixelShader", XTL::EMUPATCH(D3DDevice_SetPixelShader), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetPixelShaderConstant_4", XTL::EMUPATCH(D3DDevice_SetPixelShaderConstant_4), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetPixelShader_0", XTL::EMUPATCH(D3DDevice_SetPixelShader_0), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_BackFillMode", XTL::EMUPATCH(D3DDevice_SetRenderState_BackFillMode), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_CullMode", XTL::EMUPATCH(D3DDevice_SetRenderState_CullMode), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_DoNotCullUncompressed", XTL::EMUPATCH(D3DDevice_SetRenderState_DoNotCullUncompressed), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_Dxt1NoiseEnable", XTL::EMUPATCH(D3DDevice_SetRenderState_Dxt1NoiseEnable), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_EdgeAntiAlias", XTL::EMUPATCH(D3DDevice_SetRenderState_EdgeAntiAlias), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_FillMode", XTL::EMUPATCH(D3DDevice_SetRenderState_FillMode), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_FogColor", XTL::EMUPATCH(D3DDevice_SetRenderState_FogColor), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_FrontFace", XTL::EMUPATCH(D3DDevice_SetRenderState_FrontFace), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_LineWidth", XTL::EMUPATCH(D3DDevice_SetRenderState_LineWidth), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_LogicOp", XTL::EMUPATCH(D3DDevice_SetRenderState_LogicOp), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_MultiSampleAntiAlias", XTL::EMUPATCH(D3DDevice_SetRenderState_MultiSampleAntiAlias), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_MultiSampleMask", XTL::EMUPATCH(D3DDevice_SetRenderState_MultiSampleMask), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_MultiSampleMode", XTL::EMUPATCH(D3DDevice_SetRenderState_MultiSampleMode), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_MultiSampleRenderTargetMode", XTL::EMUPATCH(D3DDevice_SetRenderState_MultiSampleRenderTargetMode), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_NormalizeNormals", XTL::EMUPATCH(D3DDevice_SetRenderState_NormalizeNormals), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_OcclusionCullEnable", XTL::EMUPATCH(D3DDevice_SetRenderState_OcclusionCullEnable), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_PSTextureModes", XTL::EMUPATCH(D3DDevice_SetRenderState_PSTextureModes), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_RopZCmpAlwaysRead", XTL::EMUPATCH(D3DDevice_SetRenderState_RopZCmpAlwaysRead), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_RopZRead", XTL::EMUPATCH(D3DDevice_SetRenderState_RopZRead), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_SampleAlpha", XTL::EMUPATCH(D3DDevice_SetRenderState_SampleAlpha), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_ShadowFunc", XTL::EMUPATCH(D3DDevice_SetRenderState_ShadowFunc), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_Simple", XTL::EMUPATCH(D3DDevice_SetRenderState_Simple), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_StencilCullEnable", XTL::EMUPATCH(D3DDevice_SetRenderState_StencilCullEnable), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_StencilEnable", XTL::EMUPATCH(D3DDevice_SetRenderState_StencilEnable), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_StencilFail", XTL::EMUPATCH(D3DDevice_SetRenderState_StencilFail), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_TextureFactor", XTL::EMUPATCH(D3DDevice_SetRenderState_TextureFactor), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_TwoSidedLighting", XTL::EMUPATCH(D3DDevice_SetRenderState_TwoSidedLighting), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_VertexBlend", XTL::EMUPATCH(D3DDevice_SetRenderState_VertexBlend), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_ZBias", XTL::EMUPATCH(D3DDevice_SetRenderState_ZBias), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_ZEnable", XTL::EMUPATCH(D3DDevice_SetRenderState_ZEnable), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderTarget", XTL::EMUPATCH(D3DDevice_SetRenderTarget), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderTargetFast", XTL::EMUPATCH(D3DDevice_SetRenderTargetFast), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetScreenSpaceOffset", XTL::EMUPATCH(D3DDevice_SetScreenSpaceOffset), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetShaderConstantMode", XTL::EMUPATCH(D3DDevice_SetShaderConstantMode), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetShaderConstantMode_0", XTL::EMUPATCH(D3DDevice_SetShaderConstantMode_0), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetSoftDisplayFilter", XTL::EMUPATCH(D3DDevice_SetSoftDisplayFilter), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetStateUP", XTL::EMUPATCH(D3DDevice_SetStateUP), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetStateVB", XTL::EMUPATCH(D3DDevice_SetStateVB), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetStipple", XTL::EMUPATCH(D3DDevice_SetStipple), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetStreamSource", XTL::EMUPATCH(D3DDevice_SetStreamSource), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetStreamSource_4", XTL::EMUPATCH(D3DDevice_SetStreamSource_4), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetStreamSource_8", XTL::EMUPATCH(D3DDevice_SetStreamSource_8), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetSwapCallback", XTL::EMUPATCH(D3DDevice_SetSwapCallback), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetTexture", XTL::EMUPATCH(D3DDevice_SetTexture), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetTextureState_BorderColor", XTL::EMUPATCH(D3DDevice_SetTextureState_BorderColor), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetTextureState_BorderColor_0", XTL::EMUPATCH(D3DDevice_SetTextureState_BorderColor_0), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetTextureState_BorderColor_4", XTL::EMUPATCH(D3DDevice_SetTextureState_BorderColor_4), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetTextureState_BumpEnv", XTL::EMUPATCH(D3DDevice_SetTextureState_BumpEnv), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetTextureState_BumpEnv_8", XTL::EMUPATCH(D3DDevice_SetTextureState_BumpEnv_8), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetTextureState_ColorKeyColor", XTL::EMUPATCH(D3DDevice_SetTextureState_ColorKeyColor), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetTextureState_ColorKeyColor_0", XTL::EMUPATCH(D3DDevice_SetTextureState_ColorKeyColor_0), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetTextureState_ColorKeyColor_4", XTL::EMUPATCH(D3DDevice_SetTextureState_ColorKeyColor_4), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetTextureState_TexCoordIndex", XTL::EMUPATCH(D3DDevice_SetTextureState_TexCoordIndex), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetTextureState_TexCoordIndex_0", XTL::EMUPATCH(D3DDevice_SetTextureState_TexCoordIndex_0), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetTextureState_TexCoordIndex_4", XTL::EMUPATCH(D3DDevice_SetTextureState_TexCoordIndex_4), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetTexture_4", XTL::EMUPATCH(D3DDevice_SetTexture_4), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetTransform", XTL::EMUPATCH(D3DDevice_SetTransform), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetTransform_0", XTL::EMUPATCH(D3DDevice_SetTransform_0), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexData2f", XTL::EMUPATCH(D3DDevice_SetVertexData2f), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexData2s", XTL::EMUPATCH(D3DDevice_SetVertexData2s), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexData4f", XTL::EMUPATCH(D3DDevice_SetVertexData4f), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexData4f_16", XTL::EMUPATCH(D3DDevice_SetVertexData4f_16), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexData4s", XTL::EMUPATCH(D3DDevice_SetVertexData4s), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexData4ub", XTL::EMUPATCH(D3DDevice_SetVertexData4ub), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexDataColor", XTL::EMUPATCH(D3DDevice_SetVertexDataColor), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexShader", XTL::EMUPATCH(D3DDevice_SetVertexShader), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexShaderConstant", XTL::EMUPATCH(D3DDevice_SetVertexShaderConstant), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexShaderConstant1", XTL::EMUPATCH(D3DDevice_SetVertexShaderConstant1), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexShaderConstant1Fast", XTL::EMUPATCH(D3DDevice_SetVertexShaderConstant1Fast), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexShaderConstant4", XTL::EMUPATCH(D3DDevice_SetVertexShaderConstant4), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexShaderConstantNotInline", XTL::EMUPATCH(D3DDevice_SetVertexShaderConstantNotInline), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexShaderConstantNotInlineFast", XTL::EMUPATCH(D3DDevice_SetVertexShaderConstantNotInlineFast), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexShaderConstant_8", XTL::EMUPATCH(D3DDevice_SetVertexShaderConstant_8), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexShaderInput", XTL::EMUPATCH(D3DDevice_SetVertexShaderInput), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexShaderInputDirect", XTL::EMUPATCH(D3DDevice_SetVertexShaderInputDirect), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVerticalBlankCallback", XTL::EMUPATCH(D3DDevice_SetVerticalBlankCallback), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetViewport", XTL::EMUPATCH(D3DDevice_SetViewport), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_Swap", XTL::EMUPATCH(D3DDevice_Swap), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_Swap_0", XTL::EMUPATCH(D3DDevice_Swap_0), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SwitchTexture", XTL::EMUPATCH(D3DDevice_SwitchTexture), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_UpdateOverlay", XTL::EMUPATCH(D3DDevice_UpdateOverlay), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DPalette_Lock", XTL::EMUPATCH(D3DPalette_Lock), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DPalette_Lock2", XTL::EMUPATCH(D3DPalette_Lock2), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DResource_BlockUntilNotBusy", XTL::EMUPATCH(D3DResource_BlockUntilNotBusy), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DResource_Release", XTL::EMUPATCH(D3DResource_Release), PATCH_HLE_D3D),
	PATCH_ENTRY("D3D_BlockOnTime", XTL::EMUPATCH(D3D_BlockOnTime), PATCH_HLE_D3D),
	PATCH_ENTRY("D3D_LazySetPointParams", XTL::EMUPATCH(D3D_LazySetPointParams), PATCH_HLE_D3D),
	PATCH_ENTRY("D3D_SetCommonDebugRegisters", XTL::EMUPATCH(D3D_SetCommonDebugRegisters), PATCH_HLE_D3D),
	PATCH_ENTRY("Direct3D_CreateDevice", XTL::EMUPATCH(Direct3D_CreateDevice), PATCH_HLE_D3D),
	PATCH_ENTRY("Direct3D_CreateDevice_16", XTL::EMUPATCH(Direct3D_CreateDevice_16), PATCH_HLE_D3D),
	PATCH_ENTRY("Direct3D_CreateDevice_4", XTL::EMUPATCH(Direct3D_CreateDevice_4), PATCH_HLE_D3D),
	PATCH_ENTRY("Lock2DSurface", XTL::EMUPATCH(Lock2DSurface), PATCH_HLE_D3D),
	PATCH_ENTRY("Lock3DSurface", XTL::EMUPATCH(Lock3DSurface), PATCH_HLE_D3D),

	// DSOUND
	PATCH_ENTRY("CDirectSoundStream_AddRef", XTL::EMUPATCH(CDirectSoundStream_AddRef), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_Discontinuity", XTL::EMUPATCH(CDirectSoundStream_Discontinuity), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_Flush", XTL::EMUPATCH(CDirectSoundStream_Flush), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_FlushEx", XTL::EMUPATCH(CDirectSoundStream_FlushEx), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_GetInfo", XTL::EMUPATCH(CDirectSoundStream_GetInfo), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_GetStatus", XTL::EMUPATCH(CDirectSoundStream_GetStatus), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_GetVoiceProperties", XTL::EMUPATCH(CDirectSoundStream_GetVoiceProperties), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_Pause", XTL::EMUPATCH(CDirectSoundStream_Pause), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_PauseEx", XTL::EMUPATCH(CDirectSoundStream_PauseEx), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_Process", XTL::EMUPATCH(CDirectSoundStream_Process), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_Release", XTL::EMUPATCH(CDirectSoundStream_Release), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetAllParameters", XTL::EMUPATCH(CDirectSoundStream_SetAllParameters), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetConeAngles", XTL::EMUPATCH(CDirectSoundStream_SetConeAngles), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetConeOrientation", XTL::EMUPATCH(CDirectSoundStream_SetConeOrientation), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetConeOutsideVolume", XTL::EMUPATCH(CDirectSoundStream_SetConeOutsideVolume), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetDistanceFactor", XTL::EMUPATCH(CDirectSoundStream_SetDistanceFactor), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetDopplerFactor", XTL::EMUPATCH(CDirectSoundStream_SetDopplerFactor), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetEG", XTL::EMUPATCH(CDirectSoundStream_SetEG), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetFilter", XTL::EMUPATCH(CDirectSoundStream_SetFilter), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetFormat", XTL::EMUPATCH(CDirectSoundStream_SetFormat), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetFrequency", XTL::EMUPATCH(CDirectSoundStream_SetFrequency), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetHeadroom", XTL::EMUPATCH(CDirectSoundStream_SetHeadroom), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetI3DL2Source", XTL::EMUPATCH(CDirectSoundStream_SetI3DL2Source), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetLFO", XTL::EMUPATCH(CDirectSoundStream_SetLFO), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetMaxDistance", XTL::EMUPATCH(CDirectSoundStream_SetMaxDistance), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetMinDistance", XTL::EMUPATCH(CDirectSoundStream_SetMinDistance), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetMixBinVolumes_12", XTL::EMUPATCH(CDirectSoundStream_SetMixBinVolumes_12), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetMixBinVolumes_8", XTL::EMUPATCH(CDirectSoundStream_SetMixBinVolumes_8), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetMixBins", XTL::EMUPATCH(CDirectSoundStream_SetMixBins), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetMode", XTL::EMUPATCH(CDirectSoundStream_SetMode), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetOutputBuffer", XTL::EMUPATCH(CDirectSoundStream_SetOutputBuffer), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetPitch", XTL::EMUPATCH(CDirectSoundStream_SetPitch), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetPosition", XTL::EMUPATCH(CDirectSoundStream_SetPosition), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetRolloffCurve", XTL::EMUPATCH(CDirectSoundStream_SetRolloffCurve), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetRolloffFactor", XTL::EMUPATCH(CDirectSoundStream_SetRolloffFactor), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetVelocity", XTL::EMUPATCH(CDirectSoundStream_SetVelocity), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetVolume", XTL::EMUPATCH(CDirectSoundStream_SetVolume), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSound_CommitDeferredSettings", XTL::EMUPATCH(CDirectSound_CommitDeferredSettings), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSound_GetSpeakerConfig", XTL::EMUPATCH(CDirectSound_GetSpeakerConfig), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSound_SynchPlayback", XTL::EMUPATCH(CDirectSound_SynchPlayback), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CMcpxStream_Dummy_0x10", XTL::EMUPATCH(CMcpxStream_Dummy_0x10), PATCH_HLE_DSOUND),
	PATCH_ENTRY("DirectSoundCreate", XTL::EMUPATCH(DirectSoundCreate), PATCH_HLE_DSOUND),
	PATCH_ENTRY("DirectSoundCreateBuffer", XTL::EMUPATCH(DirectSoundCreateBuffer), PATCH_HLE_DSOUND),
	PATCH_ENTRY("DirectSoundCreateStream", XTL::EMUPATCH(DirectSoundCreateStream), PATCH_HLE_DSOUND),
	PATCH_ENTRY("DirectSoundDoWork", XTL::EMUPATCH(DirectSoundDoWork), PATCH_HLE_DSOUND),
	PATCH_ENTRY("DirectSoundGetSampleTime", XTL::EMUPATCH(DirectSoundGetSampleTime), PATCH_HLE_DSOUND),
	PATCH_ENTRY("DirectSoundUseFullHRTF", XTL::EMUPATCH(DirectSoundUseFullHRTF), PATCH_HLE_DSOUND),
	PATCH_ENTRY("DirectSoundUseFullHRTF4Channel", XTL::EMUPATCH(DirectSoundUseFullHRTF4Channel), PATCH_HLE_DSOUND),
	PATCH_ENTRY("DirectSoundUseLightHRTF", XTL::EMUPATCH(DirectSoundUseLightHRTF), PATCH_HLE_DSOUND),
	PATCH_ENTRY("DirectSoundUseLightHRTF4Channel", XTL::EMUPATCH(DirectSoundUseLightHRTF4Channel), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_AddRef", XTL::EMUPATCH(IDirectSoundBuffer_AddRef), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_GetCurrentPosition", XTL::EMUPATCH(IDirectSoundBuffer_GetCurrentPosition), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_GetStatus", XTL::EMUPATCH(IDirectSoundBuffer_GetStatus), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_GetVoiceProperties", XTL::EMUPATCH(IDirectSoundBuffer_GetVoiceProperties), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_Lock", XTL::EMUPATCH(IDirectSoundBuffer_Lock), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_Pause", XTL::EMUPATCH(IDirectSoundBuffer_Pause), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_PauseEx", XTL::EMUPATCH(IDirectSoundBuffer_PauseEx), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_Play", XTL::EMUPATCH(IDirectSoundBuffer_Play), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_PlayEx", XTL::EMUPATCH(IDirectSoundBuffer_PlayEx), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_Release", XTL::EMUPATCH(IDirectSoundBuffer_Release), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetAllParameters", XTL::EMUPATCH(IDirectSoundBuffer_SetAllParameters), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetBufferData", XTL::EMUPATCH(IDirectSoundBuffer_SetBufferData), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetConeAngles", XTL::EMUPATCH(IDirectSoundBuffer_SetConeAngles), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetConeOrientation", XTL::EMUPATCH(IDirectSoundBuffer_SetConeOrientation), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetConeOutsideVolume", XTL::EMUPATCH(IDirectSoundBuffer_SetConeOutsideVolume), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetCurrentPosition", XTL::EMUPATCH(IDirectSoundBuffer_SetCurrentPosition), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetDistanceFactor", XTL::EMUPATCH(IDirectSoundBuffer_SetDistanceFactor), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetDopplerFactor", XTL::EMUPATCH(IDirectSoundBuffer_SetDopplerFactor), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetEG", XTL::EMUPATCH(IDirectSoundBuffer_SetEG), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetFilter", XTL::EMUPATCH(IDirectSoundBuffer_SetFilter), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetFormat", XTL::EMUPATCH(IDirectSoundBuffer_SetFormat), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetFrequency", XTL::EMUPATCH(IDirectSoundBuffer_SetFrequency), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetHeadroom", XTL::EMUPATCH(IDirectSoundBuffer_SetHeadroom), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetI3DL2Source", XTL::EMUPATCH(IDirectSoundBuffer_SetI3DL2Source), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetLFO", XTL::EMUPATCH(IDirectSoundBuffer_SetLFO), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetLoopRegion", XTL::EMUPATCH(IDirectSoundBuffer_SetLoopRegion), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetMaxDistance", XTL::EMUPATCH(IDirectSoundBuffer_SetMaxDistance), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetMinDistance", XTL::EMUPATCH(IDirectSoundBuffer_SetMinDistance), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetMixBinVolumes_12", XTL::EMUPATCH(IDirectSoundBuffer_SetMixBinVolumes_12), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetMixBinVolumes_8", XTL::EMUPATCH(IDirectSoundBuffer_SetMixBinVolumes_8), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetMixBins", XTL::EMUPATCH(IDirectSoundBuffer_SetMixBins), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetMode", XTL::EMUPATCH(IDirectSoundBuffer_SetMode), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetNotificationPositions", XTL::EMUPATCH(IDirectSoundBuffer_SetNotificationPositions), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetOutputBuffer", XTL::EMUPATCH(IDirectSoundBuffer_SetOutputBuffer), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetPitch", XTL::EMUPATCH(IDirectSoundBuffer_SetPitch), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetPlayRegion", XTL::EMUPATCH(IDirectSoundBuffer_SetPlayRegion), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetPosition", XTL::EMUPATCH(IDirectSoundBuffer_SetPosition), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetRolloffCurve", XTL::EMUPATCH(IDirectSoundBuffer_SetRolloffCurve), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetRolloffFactor", XTL::EMUPATCH(IDirectSoundBuffer_SetRolloffFactor), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetVelocity", XTL::EMUPATCH(IDirectSoundBuffer_SetVelocity), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetVolume", XTL::EMUPATCH(IDirectSoundBuffer_SetVolume), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_Stop", XTL::EMUPATCH(IDirectSoundBuffer_Stop), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_StopEx", XTL::EMUPATCH(IDirectSoundBuffer_StopEx), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_Unlock", XTL::EMUPATCH(IDirectSoundBuffer_Unlock), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_Use3DVoiceData", XTL::EMUPATCH(IDirectSoundBuffer_Use3DVoiceData), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundStream_SetEG", XTL::EMUPATCH(IDirectSoundStream_SetEG), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundStream_SetFilter", XTL::EMUPATCH(IDirectSoundStream_SetFilter), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundStream_SetFrequency", XTL::EMUPATCH(IDirectSoundStream_SetFrequency), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundStream_SetHeadroom", XTL::EMUPATCH(IDirectSoundStream_SetHeadroom), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundStream_SetLFO", XTL::EMUPATCH(IDirectSoundStream_SetLFO), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundStream_SetMixBins", XTL::EMUPATCH(IDirectSoundStream_SetMixBins), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundStream_SetPitch", XTL::EMUPATCH(IDirectSoundStream_SetPitch), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundStream_SetVolume", XTL::EMUPATCH(IDirectSoundStream_SetVolume), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_AddRef", XTL::EMUPATCH(IDirectSound_AddRef), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_CommitDeferredSettings", XTL::EMUPATCH(IDirectSound_CommitDeferredSettings), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_CommitEffectData", XTL::EMUPATCH(IDirectSound_CommitEffectData), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_CreateSoundBuffer", XTL::EMUPATCH(IDirectSound_CreateSoundBuffer), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_CreateSoundStream", XTL::EMUPATCH(IDirectSound_CreateSoundStream), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_DownloadEffectsImage", XTL::EMUPATCH(IDirectSound_DownloadEffectsImage), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_EnableHeadphones", XTL::EMUPATCH(IDirectSound_EnableHeadphones), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_GetCaps", XTL::EMUPATCH(IDirectSound_GetCaps), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_GetEffectData", XTL::EMUPATCH(IDirectSound_GetEffectData), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_GetOutputLevels", XTL::EMUPATCH(IDirectSound_GetOutputLevels), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_GetSpeakerConfig", XTL::EMUPATCH(IDirectSound_GetSpeakerConfig), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_Release", XTL::EMUPATCH(IDirectSound_Release), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_SetAllParameters", XTL::EMUPATCH(IDirectSound_SetAllParameters), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_SetDistanceFactor", XTL::EMUPATCH(IDirectSound_SetDistanceFactor), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_SetDopplerFactor", XTL::EMUPATCH(IDirectSound_SetDopplerFactor), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_SetEffectData", XTL::EMUPATCH(IDirectSound_SetEffectData), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_SetI3DL2Listener", XTL::EMUPATCH(IDirectSound_SetI3DL2Listener), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_SetMixBinHeadroom", XTL::EMUPATCH(IDirectSound_SetMixBinHeadroom), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_SetOrientation", XTL::EMUPATCH(IDirectSound_SetOrientation), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_SetPosition", XTL::EMUPATCH(IDirectSound_SetPosition), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_SetRolloffFactor", XTL::EMUPATCH(IDirectSound_SetRolloffFactor), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_SetVelocity", XTL::EMUPATCH(IDirectSound_SetVelocity), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_SynchPlayback", XTL::EMUPATCH(IDirectSound_SynchPlayback), PATCH_HLE_DSOUND),
	PATCH_ENTRY("XAudioCreateAdpcmFormat", XTL::EMUPATCH(XAudioCreateAdpcmFormat), PATCH_HLE_DSOUND),
	PATCH_ENTRY("XAudioDownloadEffectsImage", XTL::EMUPATCH(XAudioDownloadEffectsImage), PATCH_HLE_DSOUND),
	PATCH_ENTRY("XAudioSetEffectData", XTL::EMUPATCH(XAudioSetEffectData), PATCH_HLE_DSOUND),

	// OHCI
	PATCH_ENTRY("XGetDeviceChanges", XTL::EMUPATCH(XGetDeviceChanges), PATCH_HLE_OHCI),
	PATCH_ENTRY("XGetDeviceEnumerationStatus", XTL::EMUPATCH(XGetDeviceEnumerationStatus), PATCH_HLE_OHCI),
	PATCH_ENTRY("XGetDevices", XTL::EMUPATCH(XGetDevices), PATCH_HLE_OHCI),
	PATCH_ENTRY("XInitDevices", XTL::EMUPATCH(XInitDevices), PATCH_HLE_OHCI),
	PATCH_ENTRY("XInputClose", XTL::EMUPATCH(XInputClose), PATCH_HLE_OHCI),
	PATCH_ENTRY("XInputGetCapabilities", XTL::EMUPATCH(XInputGetCapabilities), PATCH_HLE_OHCI),
	PATCH_ENTRY("XInputGetDeviceDescription", XTL::EMUPATCH(XInputGetDeviceDescription), PATCH_HLE_OHCI),
	PATCH_ENTRY("XInputGetState", XTL::EMUPATCH(XInputGetState), PATCH_HLE_OHCI),
	PATCH_ENTRY("XInputOpen", XTL::EMUPATCH(XInputOpen), PATCH_HLE_OHCI),
	PATCH_ENTRY("XInputPoll", XTL::EMUPATCH(XInputPoll), PATCH_HLE_OHCI),
	PATCH_ENTRY("XInputSetState", XTL::EMUPATCH(XInputSetState), PATCH_HLE_OHCI),

	// XAPI
	PATCH_ENTRY("ConvertThreadToFiber", XTL::EMUPATCH(ConvertThreadToFiber), PATCH_ALWAYS),
	PATCH_ENTRY("CreateFiber", XTL::EMUPATCH(CreateFiber), PATCH_ALWAYS),
	PATCH_ENTRY("DeleteFiber", XTL::EMUPATCH(DeleteFiber), PATCH_ALWAYS),
	PATCH_ENTRY("GetExitCodeThread", XTL::EMUPATCH(GetExitCodeThread), PATCH_ALWAYS),
	PATCH_ENTRY("GetThreadPriority", XTL::EMUPATCH(GetThreadPriority), PATCH_ALWAYS),
	PATCH_ENTRY("OutputDebugStringA", XTL::EMUPATCH(OutputDebugStringA), PATCH_ALWAYS),
	PATCH_ENTRY("RaiseException", XTL::EMUPATCH(RaiseException), PATCH_ALWAYS),
	PATCH_ENTRY("SetThreadPriority", XTL::EMUPATCH(SetThreadPriority), PATCH_ALWAYS),
	PATCH_ENTRY("SetThreadPriorityBoost", XTL::EMUPATCH(SetThreadPriorityBoost), PATCH_ALWAYS),
	PATCH_ENTRY("SignalObjectAndWait", XTL::EMUPATCH(SignalObjectAndWait), PATCH_ALWAYS),
	PATCH_ENTRY("SwitchToFiber", XTL::EMUPATCH(SwitchToFiber), PATCH_ALWAYS),
	PATCH_ENTRY("XMountMUA", XTL::EMUPATCH(XMountMUA), PATCH_ALWAYS),
	PATCH_ENTRY("XMountMURootA", XTL::EMUPATCH(XMountMURootA), PATCH_ALWAYS),
	PATCH_ENTRY("XSetProcessQuantumLength", XTL::EMUPATCH(XSetProcessQuantumLength), PATCH_ALWAYS),
	PATCH_ENTRY("timeKillEvent", XTL::EMUPATCH(timeKillEvent), PATCH_ALWAYS),
	PATCH_ENTRY("timeSetEvent", XTL::EMUPATCH(timeSetEvent), PATCH_ALWAYS),
};

std::unordered_map<std::string, subhook::Hook> g_FunctionHooks;

// NOTE: EmuInstallPatch do not get to be in XbSymbolDatabase, do the patches in Cxbx project only.
inline void EmuInstallPatch(std::string FunctionName, xbaddr FunctionAddr)
{
	auto it = g_PatchTable.find(FunctionName);
	if (it == g_PatchTable.end()) {
		return;
	}

	auto patch = it->second;

	if ((patch.flags & PATCH_HLE_D3D) && bLLE_GPU) {
		printf("HLE: %s: Skipped (LLE GPU Enabled)\n", FunctionName.c_str());
		return;
	}

	if ((patch.flags & PATCH_HLE_DSOUND) && bLLE_APU) {
		printf("HLE: %s: Skipped (LLE APU Enabled)\n", FunctionName.c_str());
		return;
	}

	if ((patch.flags & PATCH_HLE_OHCI) && bLLE_USB) {
		printf("HLE: %s: Skipped (LLE OHCI Enabled)\n", FunctionName.c_str());
		return;
	}

	g_FunctionHooks[FunctionName].Install((void*)(FunctionAddr), (void*)patch.patchFunc);
	printf("HLE: %s Patched\n", FunctionName.c_str());
}

void EmuInstallPatches()
{
	for (const auto& it : g_SymbolAddresses) {
		EmuInstallPatch(it.first, it.second);
	}
}

void* GetPatchedFunctionTrampoline(std::string functionName)
{
	if (g_FunctionHooks.find(functionName) != g_FunctionHooks.end()) {
		auto trampoline = g_FunctionHooks[functionName].GetTrampoline();
		if (trampoline == nullptr) {
			EmuLogEx(CXBXR_MODULE::HLE, LOG_LEVEL::WARNING, "Failed to get XB_Trampoline for %s", functionName.c_str());
		}

		return trampoline;
	}

	return nullptr;
}
