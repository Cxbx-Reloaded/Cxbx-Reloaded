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
#include "core\hle\D3D8\Direct3D9/Direct3D9.h"
#include "core\hle\DSOUND\DirectSound\DirectSound.hpp"
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
const uint32_t PATCH_IS_FIBER = 1 << 4;

#define PATCH_ENTRY(Name, Func, Flags) \
    { Name, xbox_patch_t { (void *)&Func, Flags} }

// Map of Xbox Patch names to Emulator Patches
// A std::string is used as it's possible for a symbol to have multiple names
// This allows for the eventual importing of Dxbx symbol files and even IDA signatures too!
std::map<const std::string, const xbox_patch_t> g_PatchTable = {
	// Direct3D
	PATCH_ENTRY("CDevice_SetStateUP", xbox::EMUPATCH(CDevice_SetStateUP), PATCH_HLE_D3D),
	PATCH_ENTRY("CDevice_SetStateVB", xbox::EMUPATCH(CDevice_SetStateVB), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_Begin", xbox::EMUPATCH(D3DDevice_Begin), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_BeginPush", xbox::EMUPATCH(D3DDevice_BeginPush), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_BeginPush2", xbox::EMUPATCH(D3DDevice_BeginPush2), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_BeginVisibilityTest", xbox::EMUPATCH(D3DDevice_BeginVisibilityTest), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_BlockOnFence", xbox::EMUPATCH(D3DDevice_BlockOnFence), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_BlockUntilVerticalBlank", xbox::EMUPATCH(D3DDevice_BlockUntilVerticalBlank), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_Clear", xbox::EMUPATCH(D3DDevice_Clear), PATCH_HLE_D3D),
    PATCH_ENTRY("D3DDevice_CopyRects", xbox::EMUPATCH(D3DDevice_CopyRects), PATCH_HLE_D3D),
	// PATCH_ENTRY("D3DDevice_CreateVertexShader", xbox::EMUPATCH(D3DDevice_CreateVertexShader), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_DeleteVertexShader", xbox::EMUPATCH(D3DDevice_DeleteVertexShader), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_DeleteVertexShader_0", xbox::EMUPATCH(D3DDevice_DeleteVertexShader_0), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_DrawIndexedVertices", xbox::EMUPATCH(D3DDevice_DrawIndexedVertices), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_DrawIndexedVerticesUP", xbox::EMUPATCH(D3DDevice_DrawIndexedVerticesUP), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_DrawRectPatch", xbox::EMUPATCH(D3DDevice_DrawRectPatch), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_DrawTriPatch", xbox::EMUPATCH(D3DDevice_DrawTriPatch), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_DrawVertices", xbox::EMUPATCH(D3DDevice_DrawVertices), PATCH_HLE_D3D),
    PATCH_ENTRY("D3DDevice_DrawVertices_4", xbox::EMUPATCH(D3DDevice_DrawVertices_4), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_DrawVerticesUP", xbox::EMUPATCH(D3DDevice_DrawVerticesUP), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_DrawVerticesUP_12", xbox::EMUPATCH(D3DDevice_DrawVerticesUP_12), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_EnableOverlay", xbox::EMUPATCH(D3DDevice_EnableOverlay), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_End", xbox::EMUPATCH(D3DDevice_End), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_EndPush", xbox::EMUPATCH(D3DDevice_EndPush), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_EndVisibilityTest", xbox::EMUPATCH(D3DDevice_EndVisibilityTest), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_EndVisibilityTest_0", xbox::EMUPATCH(D3DDevice_EndVisibilityTest_0), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_FlushVertexCache", xbox::EMUPATCH(D3DDevice_FlushVertexCache), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetBackBuffer", xbox::EMUPATCH(D3DDevice_GetBackBuffer), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetBackBuffer2", xbox::EMUPATCH(D3DDevice_GetBackBuffer2), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetBackBuffer2_0__LTCG_eax1", xbox::EMUPATCH(D3DDevice_GetBackBuffer2_0__LTCG_eax1), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetDisplayFieldStatus", xbox::EMUPATCH(D3DDevice_GetDisplayFieldStatus), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetGammaRamp", xbox::EMUPATCH(D3DDevice_GetGammaRamp), PATCH_HLE_D3D),
	//PATCH_ENTRY("D3DDevice_GetMaterial", xbox::EMUPATCH(D3DDevice_GetMaterial), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetModelView", xbox::EMUPATCH(D3DDevice_GetModelView), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetOverlayUpdateStatus", xbox::EMUPATCH(D3DDevice_GetOverlayUpdateStatus), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetProjectionViewportMatrix", xbox::EMUPATCH(D3DDevice_GetProjectionViewportMatrix), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetShaderConstantMode", xbox::EMUPATCH(D3DDevice_GetShaderConstantMode), PATCH_HLE_D3D),
	//PATCH_ENTRY("D3DDevice_GetTransform", xbox::EMUPATCH(D3DDevice_GetTransform), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetVertexShader", xbox::EMUPATCH(D3DDevice_GetVertexShader), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetVertexShaderConstant", xbox::EMUPATCH(D3DDevice_GetVertexShaderConstant), PATCH_HLE_D3D),
	//PATCH_ENTRY("D3DDevice_GetVertexShaderDeclaration", xbox::EMUPATCH(D3DDevice_GetVertexShaderDeclaration), PATCH_HLE_D3D),
	//PATCH_ENTRY("D3DDevice_GetVertexShaderFunction", xbox::EMUPATCH(D3DDevice_GetVertexShaderFunction), PATCH_HLE_D3D),
	//PATCH_ENTRY("D3DDevice_GetVertexShaderInput", xbox::EMUPATCH(D3DDevice_GetVertexShaderInput), PATCH_HLE_D3D),
	//PATCH_ENTRY("D3DDevice_GetVertexShaderSize", xbox::EMUPATCH(D3DDevice_GetVertexShaderSize), PATCH_HLE_D3D),
	//PATCH_ENTRY("D3DDevice_GetVertexShaderType", xbox::EMUPATCH(D3DDevice_GetVertexShaderType), PATCH_HLE_D3D),
	//PATCH_ENTRY("D3DDevice_GetViewportOffsetAndScale", xbox::EMUPATCH(D3DDevice_GetViewportOffsetAndScale), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_GetVisibilityTestResult", xbox::EMUPATCH(D3DDevice_GetVisibilityTestResult), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_InsertCallback", xbox::EMUPATCH(D3DDevice_InsertCallback), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_InsertFence", xbox::EMUPATCH(D3DDevice_InsertFence), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_IsBusy", xbox::EMUPATCH(D3DDevice_IsBusy), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_IsFencePending", xbox::EMUPATCH(D3DDevice_IsFencePending), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_LightEnable", xbox::EMUPATCH(D3DDevice_LightEnable), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_LoadVertexShader", xbox::EMUPATCH(D3DDevice_LoadVertexShader), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_LoadVertexShaderProgram", xbox::EMUPATCH(D3DDevice_LoadVertexShaderProgram), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_LoadVertexShader_0__LTCG_eax_Address_ecx_Handle", xbox::EMUPATCH(D3DDevice_LoadVertexShader_0__LTCG_eax_Address_ecx_Handle), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_LoadVertexShader_0__LTCG_eax_Address_edx_Handle", xbox::EMUPATCH(D3DDevice_LoadVertexShader_0__LTCG_eax_Address_edx_Handle), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_LoadVertexShader_4", xbox::EMUPATCH(D3DDevice_LoadVertexShader_4), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_MultiplyTransform", xbox::EMUPATCH(D3DDevice_MultiplyTransform), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_PersistDisplay", xbox::EMUPATCH(D3DDevice_PersistDisplay), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_Present", xbox::EMUPATCH(D3DDevice_Present), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_PrimeVertexCache", xbox::EMUPATCH(D3DDevice_PrimeVertexCache), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_Reset", xbox::EMUPATCH(D3DDevice_Reset), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_RunPushBuffer", xbox::EMUPATCH(D3DDevice_RunPushBuffer), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_RunVertexStateShader", xbox::EMUPATCH(D3DDevice_RunVertexStateShader), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SelectVertexShader", xbox::EMUPATCH(D3DDevice_SelectVertexShader), PATCH_HLE_D3D),
	//PATCH_ENTRY("D3DDevice_SelectVertexShaderDirect", xbox::EMUPATCH(D3DDevice_SelectVertexShaderDirect), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SelectVertexShader_0", xbox::EMUPATCH(D3DDevice_SelectVertexShader_0), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SelectVertexShader_4", xbox::EMUPATCH(D3DDevice_SelectVertexShader_4), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetBackBufferScale", xbox::EMUPATCH(D3DDevice_SetBackBufferScale), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetBackMaterial", xbox::EMUPATCH(D3DDevice_SetBackMaterial), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetDepthClipPlanes", xbox::EMUPATCH(D3DDevice_SetDepthClipPlanes), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetFlickerFilter", xbox::EMUPATCH(D3DDevice_SetFlickerFilter), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetFlickerFilter_0", xbox::EMUPATCH(D3DDevice_SetFlickerFilter_0), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetGammaRamp", xbox::EMUPATCH(D3DDevice_SetGammaRamp), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetIndices", xbox::EMUPATCH(D3DDevice_SetIndices), PATCH_HLE_D3D),
    PATCH_ENTRY("D3DDevice_SetIndices_4", xbox::EMUPATCH(D3DDevice_SetIndices_4), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetLight", xbox::EMUPATCH(D3DDevice_SetLight), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetMaterial", xbox::EMUPATCH(D3DDevice_SetMaterial), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetModelView", xbox::EMUPATCH(D3DDevice_SetModelView), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetPalette", xbox::EMUPATCH(D3DDevice_SetPalette), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetPalette_4", xbox::EMUPATCH(D3DDevice_SetPalette_4), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetPixelShader", xbox::EMUPATCH(D3DDevice_SetPixelShader), PATCH_HLE_D3D),
	//PATCH_ENTRY("D3DDevice_SetPixelShaderConstant_4", xbox::EMUPATCH(D3DDevice_SetPixelShaderConstant_4), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetPixelShader_0", xbox::EMUPATCH(D3DDevice_SetPixelShader_0), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderState_Simple", xbox::EMUPATCH(D3DDevice_SetRenderState_Simple), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderTarget", xbox::EMUPATCH(D3DDevice_SetRenderTarget), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderTargetFast", xbox::EMUPATCH(D3DDevice_SetRenderTargetFast), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetRenderTarget_0", xbox::EMUPATCH(D3DDevice_SetRenderTarget_0), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetScreenSpaceOffset", xbox::EMUPATCH(D3DDevice_SetScreenSpaceOffset), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetShaderConstantMode", xbox::EMUPATCH(D3DDevice_SetShaderConstantMode), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetShaderConstantMode_0__LTCG_eax1", xbox::EMUPATCH(D3DDevice_SetShaderConstantMode_0__LTCG_eax1), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetSoftDisplayFilter", xbox::EMUPATCH(D3DDevice_SetSoftDisplayFilter), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetStipple", xbox::EMUPATCH(D3DDevice_SetStipple), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetStreamSource", xbox::EMUPATCH(D3DDevice_SetStreamSource), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetStreamSource_0__LTCG_eax_StreamNumber_edi_pStreamData_ebx_Stride", xbox::EMUPATCH(D3DDevice_SetStreamSource_0__LTCG_eax_StreamNumber_edi_pStreamData_ebx_Stride), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetStreamSource_4", xbox::EMUPATCH(D3DDevice_SetStreamSource_4), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetStreamSource_8", xbox::EMUPATCH(D3DDevice_SetStreamSource_8), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetStreamSource_8__LTCG_edx_StreamNumber", xbox::EMUPATCH(D3DDevice_SetStreamSource_8__LTCG_edx_StreamNumber), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetSwapCallback", xbox::EMUPATCH(D3DDevice_SetSwapCallback), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetTexture", xbox::EMUPATCH(D3DDevice_SetTexture), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetTexture_4__LTCG_eax_pTexture", xbox::EMUPATCH(D3DDevice_SetTexture_4__LTCG_eax_pTexture), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetTexture_4", xbox::EMUPATCH(D3DDevice_SetTexture_4), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetTransform", xbox::EMUPATCH(D3DDevice_SetTransform), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetTransform_0__LTCG_eax1_edx2", xbox::EMUPATCH(D3DDevice_SetTransform_0__LTCG_eax1_edx2), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexData2f", xbox::EMUPATCH(D3DDevice_SetVertexData2f), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexData2s", xbox::EMUPATCH(D3DDevice_SetVertexData2s), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexData4f", xbox::EMUPATCH(D3DDevice_SetVertexData4f), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexData4f_16", xbox::EMUPATCH(D3DDevice_SetVertexData4f_16), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexData4s", xbox::EMUPATCH(D3DDevice_SetVertexData4s), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexData4ub", xbox::EMUPATCH(D3DDevice_SetVertexData4ub), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexDataColor", xbox::EMUPATCH(D3DDevice_SetVertexDataColor), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexShader", xbox::EMUPATCH(D3DDevice_SetVertexShader), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexShader_0", xbox::EMUPATCH(D3DDevice_SetVertexShader_0), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexShaderConstant", xbox::EMUPATCH(D3DDevice_SetVertexShaderConstant), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexShaderConstant1", xbox::EMUPATCH(D3DDevice_SetVertexShaderConstant1), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexShaderConstant1Fast", xbox::EMUPATCH(D3DDevice_SetVertexShaderConstant1Fast), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexShaderConstant4", xbox::EMUPATCH(D3DDevice_SetVertexShaderConstant4), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexShaderConstantNotInline", xbox::EMUPATCH(D3DDevice_SetVertexShaderConstantNotInline), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexShaderConstantNotInlineFast", xbox::EMUPATCH(D3DDevice_SetVertexShaderConstantNotInlineFast), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexShaderConstant_8", xbox::EMUPATCH(D3DDevice_SetVertexShaderConstant_8), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVertexShaderInput", xbox::EMUPATCH(D3DDevice_SetVertexShaderInput), PATCH_HLE_D3D),
	//PATCH_ENTRY("D3DDevice_SetVertexShaderInputDirect", xbox::EMUPATCH(D3DDevice_SetVertexShaderInputDirect), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetVerticalBlankCallback", xbox::EMUPATCH(D3DDevice_SetVerticalBlankCallback), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SetViewport", xbox::EMUPATCH(D3DDevice_SetViewport), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_Swap", xbox::EMUPATCH(D3DDevice_Swap), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_Swap_0", xbox::EMUPATCH(D3DDevice_Swap_0), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_SwitchTexture", xbox::EMUPATCH(D3DDevice_SwitchTexture), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DDevice_UpdateOverlay", xbox::EMUPATCH(D3DDevice_UpdateOverlay), PATCH_HLE_D3D),
	PATCH_ENTRY("D3DResource_BlockUntilNotBusy", xbox::EMUPATCH(D3DResource_BlockUntilNotBusy), PATCH_HLE_D3D),
	PATCH_ENTRY("D3D_BlockOnTime", xbox::EMUPATCH(D3D_BlockOnTime), PATCH_HLE_D3D),
	PATCH_ENTRY("D3D_BlockOnTime_4", xbox::EMUPATCH(D3D_BlockOnTime_4), PATCH_HLE_D3D),
	PATCH_ENTRY("D3D_CommonSetRenderTarget", xbox::EMUPATCH(D3D_CommonSetRenderTarget), PATCH_HLE_D3D),
    PATCH_ENTRY("D3D_DestroyResource", xbox::EMUPATCH(D3D_DestroyResource), PATCH_HLE_D3D),
    PATCH_ENTRY("D3D_DestroyResource__LTCG", xbox::EMUPATCH(D3D_DestroyResource__LTCG), PATCH_HLE_D3D),
	PATCH_ENTRY("D3D_LazySetPointParams", xbox::EMUPATCH(D3D_LazySetPointParams), PATCH_HLE_D3D),
	PATCH_ENTRY("D3D_SetCommonDebugRegisters", xbox::EMUPATCH(D3D_SetCommonDebugRegisters), PATCH_HLE_D3D),
	PATCH_ENTRY("Direct3D_CreateDevice", xbox::EMUPATCH(Direct3D_CreateDevice), PATCH_HLE_D3D),
	PATCH_ENTRY("Direct3D_CreateDevice_16__LTCG_eax_BehaviorFlags_ebx_ppReturnedDeviceInterface", xbox::EMUPATCH(Direct3D_CreateDevice_16__LTCG_eax_BehaviorFlags_ebx_ppReturnedDeviceInterface), PATCH_HLE_D3D),
	PATCH_ENTRY("Direct3D_CreateDevice_16__LTCG_eax_BehaviorFlags_ecx_ppReturnedDeviceInterface", xbox::EMUPATCH(Direct3D_CreateDevice_16__LTCG_eax_BehaviorFlags_ecx_ppReturnedDeviceInterface), PATCH_HLE_D3D),
	PATCH_ENTRY("Direct3D_CreateDevice_4", xbox::EMUPATCH(Direct3D_CreateDevice_4), PATCH_HLE_D3D),
	PATCH_ENTRY("Lock2DSurface", xbox::EMUPATCH(Lock2DSurface), PATCH_HLE_D3D),
	PATCH_ENTRY("Lock3DSurface", xbox::EMUPATCH(Lock3DSurface), PATCH_HLE_D3D),

	// DSOUND
	PATCH_ENTRY("CDirectSound3DCalculator_Calculate3D", xbox::EMUPATCH(CDirectSound3DCalculator_Calculate3D), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSound3DCalculator_GetVoiceData", xbox::EMUPATCH(CDirectSound3DCalculator_GetVoiceData), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_AddRef", xbox::EMUPATCH(CDirectSoundStream_AddRef), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_Discontinuity", xbox::EMUPATCH(CDirectSoundStream_Discontinuity), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_Flush", xbox::EMUPATCH(CDirectSoundStream_Flush), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_FlushEx", xbox::EMUPATCH(CDirectSoundStream_FlushEx), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_GetInfo", xbox::EMUPATCH(CDirectSoundStream_GetInfo), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_GetStatus__r1", xbox::EMUPATCH(CDirectSoundStream_GetStatus__r1), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_GetStatus__r2", xbox::EMUPATCH(CDirectSoundStream_GetStatus__r2), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_GetVoiceProperties", xbox::EMUPATCH(CDirectSoundStream_GetVoiceProperties), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_Pause", xbox::EMUPATCH(CDirectSoundStream_Pause), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_PauseEx", xbox::EMUPATCH(CDirectSoundStream_PauseEx), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_Process", xbox::EMUPATCH(CDirectSoundStream_Process), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_Release", xbox::EMUPATCH(CDirectSoundStream_Release), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetAllParameters", xbox::EMUPATCH(CDirectSoundStream_SetAllParameters), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetConeAngles", xbox::EMUPATCH(CDirectSoundStream_SetConeAngles), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetConeOrientation", xbox::EMUPATCH(CDirectSoundStream_SetConeOrientation), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetConeOutsideVolume", xbox::EMUPATCH(CDirectSoundStream_SetConeOutsideVolume), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetDistanceFactor", xbox::EMUPATCH(CDirectSoundStream_SetDistanceFactor), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetDopplerFactor", xbox::EMUPATCH(CDirectSoundStream_SetDopplerFactor), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetEG", xbox::EMUPATCH(CDirectSoundStream_SetEG), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetFilter", xbox::EMUPATCH(CDirectSoundStream_SetFilter), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetFormat", xbox::EMUPATCH(CDirectSoundStream_SetFormat), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetFrequency", xbox::EMUPATCH(CDirectSoundStream_SetFrequency), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetHeadroom", xbox::EMUPATCH(CDirectSoundStream_SetHeadroom), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetI3DL2Source", xbox::EMUPATCH(CDirectSoundStream_SetI3DL2Source), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetLFO", xbox::EMUPATCH(CDirectSoundStream_SetLFO), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetMaxDistance", xbox::EMUPATCH(CDirectSoundStream_SetMaxDistance), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetMinDistance", xbox::EMUPATCH(CDirectSoundStream_SetMinDistance), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetMixBinVolumes_12", xbox::EMUPATCH(CDirectSoundStream_SetMixBinVolumes_12), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetMixBinVolumes_8", xbox::EMUPATCH(CDirectSoundStream_SetMixBinVolumes_8), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetMixBins", xbox::EMUPATCH(CDirectSoundStream_SetMixBins), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetMode", xbox::EMUPATCH(CDirectSoundStream_SetMode), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetOutputBuffer", xbox::EMUPATCH(CDirectSoundStream_SetOutputBuffer), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetPitch", xbox::EMUPATCH(CDirectSoundStream_SetPitch), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetPosition", xbox::EMUPATCH(CDirectSoundStream_SetPosition), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetRolloffCurve", xbox::EMUPATCH(CDirectSoundStream_SetRolloffCurve), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetRolloffFactor", xbox::EMUPATCH(CDirectSoundStream_SetRolloffFactor), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetVelocity", xbox::EMUPATCH(CDirectSoundStream_SetVelocity), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSoundStream_SetVolume", xbox::EMUPATCH(CDirectSoundStream_SetVolume), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSound_CommitDeferredSettings", xbox::EMUPATCH(CDirectSound_CommitDeferredSettings), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSound_GetSpeakerConfig", xbox::EMUPATCH(CDirectSound_GetSpeakerConfig), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSound_SynchPlayback", xbox::EMUPATCH(CDirectSound_SynchPlayback), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CMcpxStream_Dummy_0x10", xbox::EMUPATCH(CMcpxStream_Dummy_0x10), PATCH_HLE_DSOUND),
	PATCH_ENTRY("DirectSoundCreate", xbox::EMUPATCH(DirectSoundCreate), PATCH_HLE_DSOUND),
	PATCH_ENTRY("DirectSoundCreateBuffer", xbox::EMUPATCH(DirectSoundCreateBuffer), PATCH_HLE_DSOUND),
	PATCH_ENTRY("DirectSoundCreateStream", xbox::EMUPATCH(DirectSoundCreateStream), PATCH_HLE_DSOUND),
	PATCH_ENTRY("DirectSoundDoWork", xbox::EMUPATCH(DirectSoundDoWork), PATCH_HLE_DSOUND),
	PATCH_ENTRY("DirectSoundGetSampleTime", xbox::EMUPATCH(DirectSoundGetSampleTime), PATCH_HLE_DSOUND),
	PATCH_ENTRY("DirectSoundUseFullHRTF", xbox::EMUPATCH(DirectSoundUseFullHRTF), PATCH_HLE_DSOUND),
	PATCH_ENTRY("DirectSoundUseFullHRTF4Channel", xbox::EMUPATCH(DirectSoundUseFullHRTF4Channel), PATCH_HLE_DSOUND),
	PATCH_ENTRY("DirectSoundUseLightHRTF", xbox::EMUPATCH(DirectSoundUseLightHRTF), PATCH_HLE_DSOUND),
	PATCH_ENTRY("DirectSoundUseLightHRTF4Channel", xbox::EMUPATCH(DirectSoundUseLightHRTF4Channel), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_AddRef", xbox::EMUPATCH(IDirectSoundBuffer_AddRef), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_GetCurrentPosition", xbox::EMUPATCH(IDirectSoundBuffer_GetCurrentPosition), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_GetStatus", xbox::EMUPATCH(IDirectSoundBuffer_GetStatus), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_GetVoiceProperties", xbox::EMUPATCH(IDirectSoundBuffer_GetVoiceProperties), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_Lock", xbox::EMUPATCH(IDirectSoundBuffer_Lock), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_Pause", xbox::EMUPATCH(IDirectSoundBuffer_Pause), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_PauseEx", xbox::EMUPATCH(IDirectSoundBuffer_PauseEx), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_Play", xbox::EMUPATCH(IDirectSoundBuffer_Play), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_PlayEx", xbox::EMUPATCH(IDirectSoundBuffer_PlayEx), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_Release", xbox::EMUPATCH(IDirectSoundBuffer_Release), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_Set3DVoiceData", xbox::EMUPATCH(IDirectSoundBuffer_Set3DVoiceData), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetAllParameters", xbox::EMUPATCH(IDirectSoundBuffer_SetAllParameters), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetBufferData", xbox::EMUPATCH(IDirectSoundBuffer_SetBufferData), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetConeAngles", xbox::EMUPATCH(IDirectSoundBuffer_SetConeAngles), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetConeOrientation", xbox::EMUPATCH(IDirectSoundBuffer_SetConeOrientation), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetConeOutsideVolume", xbox::EMUPATCH(IDirectSoundBuffer_SetConeOutsideVolume), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetCurrentPosition", xbox::EMUPATCH(IDirectSoundBuffer_SetCurrentPosition), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetDistanceFactor", xbox::EMUPATCH(IDirectSoundBuffer_SetDistanceFactor), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetDopplerFactor", xbox::EMUPATCH(IDirectSoundBuffer_SetDopplerFactor), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetEG", xbox::EMUPATCH(IDirectSoundBuffer_SetEG), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetFilter", xbox::EMUPATCH(IDirectSoundBuffer_SetFilter), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetFormat", xbox::EMUPATCH(IDirectSoundBuffer_SetFormat), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetFrequency", xbox::EMUPATCH(IDirectSoundBuffer_SetFrequency), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetHeadroom", xbox::EMUPATCH(IDirectSoundBuffer_SetHeadroom), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetI3DL2Source", xbox::EMUPATCH(IDirectSoundBuffer_SetI3DL2Source), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetLFO", xbox::EMUPATCH(IDirectSoundBuffer_SetLFO), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetLoopRegion", xbox::EMUPATCH(IDirectSoundBuffer_SetLoopRegion), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetMaxDistance", xbox::EMUPATCH(IDirectSoundBuffer_SetMaxDistance), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetMinDistance", xbox::EMUPATCH(IDirectSoundBuffer_SetMinDistance), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetMixBinVolumes_12", xbox::EMUPATCH(IDirectSoundBuffer_SetMixBinVolumes_12), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetMixBinVolumes_8", xbox::EMUPATCH(IDirectSoundBuffer_SetMixBinVolumes_8), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetMixBins", xbox::EMUPATCH(IDirectSoundBuffer_SetMixBins), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetMode", xbox::EMUPATCH(IDirectSoundBuffer_SetMode), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetNotificationPositions", xbox::EMUPATCH(IDirectSoundBuffer_SetNotificationPositions), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetOutputBuffer", xbox::EMUPATCH(IDirectSoundBuffer_SetOutputBuffer), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetPitch", xbox::EMUPATCH(IDirectSoundBuffer_SetPitch), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetPlayRegion", xbox::EMUPATCH(IDirectSoundBuffer_SetPlayRegion), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetPosition", xbox::EMUPATCH(IDirectSoundBuffer_SetPosition), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetRolloffCurve", xbox::EMUPATCH(IDirectSoundBuffer_SetRolloffCurve), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetRolloffFactor", xbox::EMUPATCH(IDirectSoundBuffer_SetRolloffFactor), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetVelocity", xbox::EMUPATCH(IDirectSoundBuffer_SetVelocity), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_SetVolume", xbox::EMUPATCH(IDirectSoundBuffer_SetVolume), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_Stop", xbox::EMUPATCH(IDirectSoundBuffer_Stop), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_StopEx", xbox::EMUPATCH(IDirectSoundBuffer_StopEx), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_Unlock", xbox::EMUPATCH(IDirectSoundBuffer_Unlock), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundBuffer_Use3DVoiceData", xbox::EMUPATCH(IDirectSoundBuffer_Use3DVoiceData), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundStream_Set3DVoiceData", xbox::EMUPATCH(IDirectSoundStream_Set3DVoiceData), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundStream_SetEG", xbox::EMUPATCH(IDirectSoundStream_SetEG), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundStream_SetFilter", xbox::EMUPATCH(IDirectSoundStream_SetFilter), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundStream_SetFrequency", xbox::EMUPATCH(IDirectSoundStream_SetFrequency), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundStream_SetHeadroom", xbox::EMUPATCH(IDirectSoundStream_SetHeadroom), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundStream_SetLFO", xbox::EMUPATCH(IDirectSoundStream_SetLFO), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundStream_SetMixBins", xbox::EMUPATCH(IDirectSoundStream_SetMixBins), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundStream_SetPitch", xbox::EMUPATCH(IDirectSoundStream_SetPitch), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundStream_SetVolume", xbox::EMUPATCH(IDirectSoundStream_SetVolume), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSoundStream_Use3DVoiceData", xbox::EMUPATCH(IDirectSoundStream_Use3DVoiceData), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_AddRef", xbox::EMUPATCH(IDirectSound_AddRef), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_CommitDeferredSettings", xbox::EMUPATCH(IDirectSound_CommitDeferredSettings), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_CommitEffectData", xbox::EMUPATCH(IDirectSound_CommitEffectData), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_CreateSoundBuffer", xbox::EMUPATCH(IDirectSound_CreateSoundBuffer), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_CreateSoundStream", xbox::EMUPATCH(IDirectSound_CreateSoundStream), PATCH_HLE_DSOUND),
//	PATCH_ENTRY("IDirectSound_DownloadEffectsImage", xbox::EMUPATCH(IDirectSound_DownloadEffectsImage), PATCH_HLE_DSOUND),
	PATCH_ENTRY("CDirectSound_DownloadEffectsImage", xbox::EMUPATCH(CDirectSound_DownloadEffectsImage), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_EnableHeadphones", xbox::EMUPATCH(IDirectSound_EnableHeadphones), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_GetCaps", xbox::EMUPATCH(IDirectSound_GetCaps), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_GetEffectData", xbox::EMUPATCH(IDirectSound_GetEffectData), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_GetOutputLevels", xbox::EMUPATCH(IDirectSound_GetOutputLevels), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_GetSpeakerConfig", xbox::EMUPATCH(IDirectSound_GetSpeakerConfig), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_Release", xbox::EMUPATCH(IDirectSound_Release), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_SetAllParameters", xbox::EMUPATCH(IDirectSound_SetAllParameters), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_SetDistanceFactor", xbox::EMUPATCH(IDirectSound_SetDistanceFactor), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_SetDopplerFactor", xbox::EMUPATCH(IDirectSound_SetDopplerFactor), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_SetEffectData", xbox::EMUPATCH(IDirectSound_SetEffectData), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_SetI3DL2Listener", xbox::EMUPATCH(IDirectSound_SetI3DL2Listener), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_SetMixBinHeadroom", xbox::EMUPATCH(IDirectSound_SetMixBinHeadroom), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_SetOrientation", xbox::EMUPATCH(IDirectSound_SetOrientation), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_SetPosition", xbox::EMUPATCH(IDirectSound_SetPosition), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_SetRolloffFactor", xbox::EMUPATCH(IDirectSound_SetRolloffFactor), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_SetVelocity", xbox::EMUPATCH(IDirectSound_SetVelocity), PATCH_HLE_DSOUND),
	PATCH_ENTRY("IDirectSound_SynchPlayback", xbox::EMUPATCH(IDirectSound_SynchPlayback), PATCH_HLE_DSOUND),
	//PATCH_ENTRY("XAudioCreateAdpcmFormat", xbox::EMUPATCH(XAudioCreateAdpcmFormat), PATCH_HLE_DSOUND), // NOTE: Not require to patch
	PATCH_ENTRY("XAudioDownloadEffectsImage", xbox::EMUPATCH(XAudioDownloadEffectsImage), PATCH_HLE_DSOUND),
	PATCH_ENTRY("XAudioSetEffectData", xbox::EMUPATCH(XAudioSetEffectData), PATCH_HLE_DSOUND),

	// OHCI
	PATCH_ENTRY("XGetDeviceChanges", xbox::EMUPATCH(XGetDeviceChanges), PATCH_HLE_OHCI),
	PATCH_ENTRY("XGetDeviceEnumerationStatus", xbox::EMUPATCH(XGetDeviceEnumerationStatus), PATCH_HLE_OHCI),
	PATCH_ENTRY("XGetDevices", xbox::EMUPATCH(XGetDevices), PATCH_HLE_OHCI),
	PATCH_ENTRY("XInitDevices", xbox::EMUPATCH(XInitDevices), PATCH_HLE_OHCI),
	PATCH_ENTRY("XInputClose", xbox::EMUPATCH(XInputClose), PATCH_HLE_OHCI),
	PATCH_ENTRY("XInputGetCapabilities", xbox::EMUPATCH(XInputGetCapabilities), PATCH_HLE_OHCI),
	PATCH_ENTRY("XInputGetDeviceDescription", xbox::EMUPATCH(XInputGetDeviceDescription), PATCH_HLE_OHCI),
	PATCH_ENTRY("XInputGetState", xbox::EMUPATCH(XInputGetState), PATCH_HLE_OHCI),
	PATCH_ENTRY("XInputOpen", xbox::EMUPATCH(XInputOpen), PATCH_HLE_OHCI),
	PATCH_ENTRY("XInputPoll", xbox::EMUPATCH(XInputPoll), PATCH_HLE_OHCI),
	PATCH_ENTRY("XInputSetLightgunCalibration", xbox::EMUPATCH(XInputSetLightgunCalibration), PATCH_HLE_OHCI),
	PATCH_ENTRY("XInputSetState", xbox::EMUPATCH(XInputSetState), PATCH_HLE_OHCI),

	// XAPI
	PATCH_ENTRY("ConvertThreadToFiber", xbox::EMUPATCH(ConvertThreadToFiber), PATCH_IS_FIBER),
	PATCH_ENTRY("CreateFiber", xbox::EMUPATCH(CreateFiber), PATCH_IS_FIBER),
	PATCH_ENTRY("DeleteFiber", xbox::EMUPATCH(DeleteFiber), PATCH_IS_FIBER),
	//PATCH_ENTRY("GetExitCodeThread", xbox::EMUPATCH(GetExitCodeThread), PATCH_ALWAYS),
	//PATCH_ENTRY("GetThreadPriority", xbox::EMUPATCH(GetThreadPriority), PATCH_ALWAYS),
	PATCH_ENTRY("OutputDebugStringA", xbox::EMUPATCH(OutputDebugStringA), PATCH_ALWAYS),
	//PATCH_ENTRY("RaiseException", xbox::EMUPATCH(RaiseException), PATCH_ALWAYS),
	//PATCH_ENTRY("SetThreadPriority", xbox::EMUPATCH(SetThreadPriority), PATCH_ALWAYS),
	//PATCH_ENTRY("SetThreadPriorityBoost", xbox::EMUPATCH(SetThreadPriorityBoost), PATCH_ALWAYS),
	PATCH_ENTRY("SignalObjectAndWait", xbox::EMUPATCH(SignalObjectAndWait), PATCH_ALWAYS),
	PATCH_ENTRY("SwitchToFiber", xbox::EMUPATCH(SwitchToFiber), PATCH_IS_FIBER),
	PATCH_ENTRY("XMountMUA", xbox::EMUPATCH(XMountMUA), PATCH_ALWAYS),
	PATCH_ENTRY("XMountMURootA", xbox::EMUPATCH(XMountMURootA), PATCH_ALWAYS),
	//PATCH_ENTRY("XSetProcessQuantumLength", xbox::EMUPATCH(XSetProcessQuantumLength), PATCH_ALWAYS),
	//PATCH_ENTRY("timeKillEvent", xbox::EMUPATCH(timeKillEvent), PATCH_ALWAYS),
	//PATCH_ENTRY("timeSetEvent", xbox::EMUPATCH(timeSetEvent), PATCH_ALWAYS),
	PATCH_ENTRY("XReadMUMetaData", xbox::EMUPATCH(XReadMUMetaData), PATCH_ALWAYS),
	PATCH_ENTRY("XUnmountMU", xbox::EMUPATCH(XUnmountMU), PATCH_ALWAYS),
};

std::unordered_map<std::string, subhook::Hook> g_FunctionHooks;

inline bool TitleRequiresUnpatchedFibers()
{
    static bool detected = false;
    static bool result = false;

    // Prevent running the check every time this function is called
    if (detected) {
        return result;
    }

    // Array of known games that require the fiber unpatch hack
    DWORD titleIds[] = {
        0x46490002, // Futurama PAL
        0x56550008, // Futurama NTSC
        0
    };

    DWORD* pTitleId = &titleIds[0];
    while (*pTitleId != 0) {
        if (g_pCertificate->dwTitleId == *pTitleId) {
            result = true;
            break;
        }

        pTitleId++;
    }

    detected = true;
    return result;
}


// NOTE: EmuInstallPatch do not get to be in XbSymbolDatabase, do the patches in Cxbx project only.
inline void EmuInstallPatch(const std::string FunctionName, const xbox::addr_xt FunctionAddr)
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

    // HACK: Some titles require unpatched Fibers, otherwise they enter an infinite loop
    // while others require patched Fibers, otherwise they outright crash
    // This is caused by limitations of Direct Code Execution and Cxbx-R's threading model
    if ((patch.flags & PATCH_IS_FIBER) && TitleRequiresUnpatchedFibers()) {
        printf("HLE: %s: Skipped (Game requires unpatched Fibers)\n", FunctionName.c_str());
        return;
    }

	auto success = g_FunctionHooks[FunctionName].Install((void*)(FunctionAddr), (void*)patch.patchFunc);
	if (success) {
		printf("HLE: %s Patched\n", FunctionName.c_str());
	}
	else {
		printf("HLE: %s Patch Failed\n", FunctionName.c_str());
	}
	
}

void EmuInstallPatches()
{
	for (const auto& it : g_SymbolAddresses) {
		EmuInstallPatch(it.first, it.second);
	}

	LookupTrampolinesD3D();
	LookupTrampolinesXAPI();
}

void* GetPatchedFunctionTrampoline(const std::string functionName)
{
	auto it = g_FunctionHooks.find(functionName);
	if (it != g_FunctionHooks.end()) {
		auto trampoline = it->second.GetTrampoline();
		if (trampoline == nullptr) {
			EmuLogEx(CXBXR_MODULE::HLE, LOG_LEVEL::WARNING, "Failed to get XB_Trampoline for %s", functionName.c_str());
		}

		return trampoline;
	}

	return nullptr;
}
