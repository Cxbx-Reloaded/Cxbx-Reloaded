// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDatabase->D3D8LTCG.OOVPA.inl
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
// *  (c) 2017 jarupxx
// *
// *  All rights reserved
// *
// ******************************************************************

// Titles which did compiled with full libary version
//   [LibV] Title Name                       |  Verify   |   Comments
//-----------------------------------------------------------------------
// * [3925] Nascar Heat 2002                 |    20%    | Only verified an actually used library.
// * [4039] NBA 2K2                          |    20%    | Only verified an actually used library.
// * [4432] Bruce Lee                        |    20%    | Only verified an actually used library.
// * [4627] Battle Engine Aquila             |   100%    | Only has 50%-ish of the library compiled with xbe build.
// * [4928] Shin Megami Tensei - Nine        |   100%    | Only has 80% of the library compiled with xbe build.
// * [5233] Midtown Madness 3                |    50%    | Only has 50%-ish of the library compiled with xbe build.
// * [5455] Freaky Flyers                    |    50%    | Only has 50%-ish of the library compiled with xbe build.
// * [5659] Ninja Gaiden                     |    50%    | Only has a few library.
// * [5849] Grand Theft Auto - San Andreas   |   100%    | Only has 50%-ish of the library compiled with xbe build.

// * About the number of OOVPA
//   * 1024 and 1xxx - Cmpatible with known functions probably.
//   * 2028 and 2xxx - Remade by Link-time Code Generation, will not work with known functions.

#ifndef D3D8LTCG_OOVPA_INL
#define D3D8LTCG_OOVPA_INL

#include "../OOVPA.h"

#include "HLEDataBase/D3D8LTCG.1.0.3911.inl"
#include "HLEDataBase/D3D8LTCG.1.0.4039.inl"
#include "HLEDataBase/D3D8LTCG.1.0.4432.inl"
#include "HLEDataBase/D3D8LTCG.1.0.4531.inl"
#include "HLEDataBase/D3D8LTCG.1.0.4627.inl"
#include "HLEDataBase/D3D8LTCG.1.0.4721.inl"
#include "HLEDataBase/D3D8LTCG.1.0.4928.inl"
#include "HLEDataBase/D3D8LTCG.1.0.5028.inl"
#include "HLEDataBase/D3D8LTCG.1.0.5120.inl"
#include "HLEDataBase/D3D8LTCG.1.0.5233.inl"
#include "HLEDataBase/D3D8LTCG.1.0.5344.inl"
#include "HLEDataBase/D3D8LTCG.1.0.5455.inl"
#include "HLEDataBase/D3D8LTCG.1.0.5788.inl"
#include "HLEDataBase/D3D8LTCG.1.0.5849.inl"

// ******************************************************************
// * D3D8LTCG_OOVPA
// ******************************************************************
OOVPATable D3D8LTCG_OOVPAV2[] = {

	REGISTER_OOVPAS(D3D_MakeRequestedSpace, PATCH, 1024, 1036, 1048),
	REGISTER_OOVPAS(D3D_MakeRequestedSpace, UNPATCHED, 2048),
	REGISTER_OOVPAS(D3D_SetFence, PATCH, 1024, 1036, 1048, 1060),
	REGISTER_OOVPAS(D3D_BlockOnTime, PATCH, 1024, 1036, 1048),
	REGISTER_OOVPAS(D3D_BlockOnTime_4, UNPATCHED, 2048, 2060),
	REGISTER_OOVPAS(D3DResource_AddRef, PATCH, 3911),

	REGISTER_OOVPAS(CMiniport_InitHardware, PATCH, 1024),
	REGISTER_OOVPAS(D3DCubeTexture_GetCubeMapSurface2, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_Begin, PATCH, 1024, 1036, 1048),
	REGISTER_OOVPAS(D3DDevice_BeginPush, PATCH, 1024, 1036, 1048),
	REGISTER_OOVPAS(D3DDevice_BeginPushBuffer, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_BeginVisibilityTest, PATCH, 1024, 1048),
	REGISTER_OOVPAS(D3DDevice_Clear, PATCH, 1024, 1036),
	REGISTER_OOVPAS(D3DDevice_CopyRects, PATCH, 1024, 1036, 1048),
	REGISTER_OOVPAS(D3DDevice_CreatePixelShader, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_CreateTexture, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_CreateTexture2, PATCH, 1024, 1036, 1048),
	REGISTER_OOVPAS(D3DDevice_CreateVertexShader, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_DeletePixelShader_0, UNPATCHED, 2024),
	REGISTER_OOVPAS(D3DDevice_DeleteStateBlock, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_DeleteVertexShader_0, UNPATCHED, 2024, 2036),
	REGISTER_OOVPAS(D3DDevice_DrawIndexedVertices, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_DrawIndexedVerticesUP, PATCH, 1024, 1036, 1048, 1060),
	REGISTER_OOVPAS(D3DDevice_DrawVertices, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_DrawVerticesUP, PATCH, 1024, 1036),
	REGISTER_OOVPAS(D3DDevice_DrawVerticesUP_12, UNPATCHED, 2024),
	REGISTER_OOVPAS(D3DDevice_End, PATCH, 1024, 1036, 1048),
	REGISTER_OOVPAS(D3DDevice_EndPushBuffer, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_EndVisibilityTest, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_GetBackBuffer2, UNPATCHED, 1024, 1036),
	REGISTER_OOVPAS(D3DDevice_GetBackBuffer2_0, UNPATCHED, 2024, 2048),
	REGISTER_OOVPAS(D3DDevice_GetBackBuffer_8, UNPATCHED, 2048),
	REGISTER_OOVPAS(D3DDevice_GetDepthStencilSurface, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_GetDepthStencilSurface2, PATCH, 1024, 1048),
	REGISTER_OOVPAS(D3DDevice_GetDisplayMode, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_GetRenderTarget2, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_GetTransform, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_GetViewportOffsetAndScale, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_IsBusy, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_IsFencePending, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_LightEnable, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_LoadVertexShaderProgram, PATCH, 1024, 1036),
	REGISTER_OOVPAS(D3DDevice_LoadVertexShader_0, UNPATCHED, 2024, 2036),
	REGISTER_OOVPAS(D3DDevice_LoadVertexShader_4, UNPATCHED, 2048),
	REGISTER_OOVPAS(D3DDevice_MultiplyTransform_0, UNPATCHED, 2024),
	REGISTER_OOVPAS(D3DDevice_PersistDisplay_0, UNPATCHED, 2048, 2060),
	REGISTER_OOVPAS(D3DDevice_Present, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_Release, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_Reset, PATCH, 1024, 1036),
	REGISTER_OOVPAS(D3DDevice_Reset_0, UNPATCHED, 2024),
	REGISTER_OOVPAS(D3DDevice_RunVertexStateShader_4, UNPATCHED, 2048),
	REGISTER_OOVPAS(D3DDevice_SelectVertexShader, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_SelectVertexShader_0, UNPATCHED, 2060),
	REGISTER_OOVPAS(D3DDevice_SelectVertexShader_4, UNPATCHED, 2024, 2048),
	REGISTER_OOVPAS(D3DDevice_SetFlickerFilter_0, UNPATCHED, 2048),
	REGISTER_OOVPAS(D3DDevice_SetGammaRamp, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_SetLight, PATCH, 1024, 1048),
	REGISTER_OOVPAS(D3DDevice_SetPalette, PATCH, 1024, 1036),
	REGISTER_OOVPAS(D3DDevice_SetPalette_4, UNPATCHED, 2024),
	REGISTER_OOVPAS(D3DDevice_SetPixelShader, PATCH, 1024, 1036, 1048),
	REGISTER_OOVPAS(D3DDevice_SetPixelShaderConstant, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_SetPixelShaderConstant_4, UNPATCHED, 2024),
	REGISTER_OOVPAS(D3DDevice_SetPixelShaderProgram, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_SetPixelShader_0, UNPATCHED, 2024, 2036),
	REGISTER_OOVPAS(D3DDevice_SetRenderStateNotInline, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_SetRenderStateNotInline_0, UNPATCHED, 2048),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_BackFillMode, PATCH, 1024, 1036),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_CullMode, PATCH, 1045, 1049, 1052, 1053),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_DoNotCullUncompressed, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_Dxt1NoiseEnable, PATCH, 1024, 1036),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_EdgeAntiAlias, PATCH, 1024, 1036, 1048, 1060),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_FillMode, PATCH, 1024, 1036, 1048, 1060),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_FogColor, PATCH, 1024, 1036, 1048, 1060),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_FrontFace, PATCH, 1024, 1036, 1048, 1060),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_LineWidth, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_LogicOp, PATCH, 1024, 1036, 1048, 1060),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_MultiSampleAntiAlias, PATCH, 1024, 1036, 1048, 1060),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_MultiSampleMask, PATCH, 1024, 1036, 1048, 1060, 1072),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_MultiSampleMode, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_NormalizeNormals, PATCH, 1024, 1036, 1048, 1060),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_OcclusionCullEnable, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_RopZCmpAlwaysRead, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_RopZRead, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_SampleAlpha, PATCH, 1024, 1036),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_ShadowFunc, PATCH, 1024, 1036, 1048, 1060),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_StencilCullEnable, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_StencilEnable, PATCH, 1024, 1036, 1048, 1060, 1072, 1084),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_StencilFail, PATCH, 1024, 1036, 1048, 1060, 1072, 1084),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_TextureFactor, PATCH, 1036),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_TwoSidedLighting, PATCH, 1024, 1036, 1048, 1060, 1072, 1084),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_VertexBlend, PATCH, 1024, 1036),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_YuvEnable, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_ZBias, PATCH, 1024, 1036, 1048, 1060),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_ZEnable, PATCH, 1024, 1036, 1048, 1060),
	REGISTER_OOVPAS(D3DDevice_SetRenderTarget, PATCH, 1024, 1036, 1048, 1060, 1072),
	REGISTER_OOVPAS(D3DDevice_SetRenderTargetFast, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_SetScissors, PATCH, 1024, 1036, 1048),
	REGISTER_OOVPAS(D3DDevice_SetScreenSpaceOffset, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_SetShaderConstantMode, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_SetShaderConstantMode_0, UNPATCHED, 2048),
	REGISTER_OOVPAS(D3DDevice_SetStreamSource, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_SetTexture, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_SetTextureStageStateNotInline, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_SetTextureStageStateNotInline_0, UNPATCHED, 2024),
	REGISTER_OOVPAS(D3DDevice_SetTextureState_BorderColor, PATCH, 1024, 1048),
	REGISTER_OOVPAS(D3DDevice_SetTextureState_BorderColor_0, UNPATCHED, 2024, 2036, 2048),
	REGISTER_OOVPAS(D3DDevice_SetTextureState_BorderColor_4, UNPATCHED, 2048, 2060),
	REGISTER_OOVPAS(D3DDevice_SetTextureState_BumpEnv, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_SetTextureState_BumpEnv_8, UNPATCHED, 2024),
	REGISTER_OOVPAS(D3DDevice_SetTextureState_ColorKeyColor, PATCH, 1024, 1036),
	REGISTER_OOVPAS(D3DDevice_SetTextureState_ColorKeyColor_0, UNPATCHED, 2024, 2036, 2048),
	REGISTER_OOVPAS(D3DDevice_SetTextureState_ColorKeyColor_4, UNPATCHED, 2048, 2060),
	REGISTER_OOVPAS(D3DDevice_SetTextureState_TexCoordIndex, PATCH, 1944, 1958),
	REGISTER_OOVPAS(D3DDevice_SetTextureState_TexCoordIndex_0, UNPATCHED, 2039),
	REGISTER_OOVPAS(D3DDevice_SetTextureState_TexCoordIndex_4, UNPATCHED, 2040, 2045, 2058),
	REGISTER_OOVPAS(D3DDevice_SetTexture_4, UNPATCHED, 2024, 2036),
	REGISTER_OOVPAS(D3DDevice_SetTile_0, UNPATCHED, 2024, 2036, 2048, 2060, 2072),
	REGISTER_OOVPAS(D3DDevice_SetTransform, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_SetTransform_0, UNPATCHED, 2024, 2048),
	REGISTER_OOVPAS(D3DDevice_SetVertexData2f, PATCH, 1024, 1036, 1048),
	REGISTER_OOVPAS(D3DDevice_SetVertexData2s, PATCH, 1024, 1036, 1048),
	REGISTER_OOVPAS(D3DDevice_SetVertexData4f, PATCH, 1024, 1036),
	REGISTER_OOVPAS(D3DDevice_SetVertexData4f_16, UNPATCHED, 2024, 2036),
	REGISTER_OOVPAS(D3DDevice_SetVertexData4s, PATCH, 1024, 1036, 1048),
	REGISTER_OOVPAS(D3DDevice_SetVertexData4ub, PATCH, 1024, 1036, 1048),
	REGISTER_OOVPAS(D3DDevice_SetVertexShader, PATCH, 1024, 1036),
	REGISTER_OOVPAS(D3DDevice_SetVertexShaderConstant, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_SetVertexShaderConstantNotInline, PATCH, 1024),
	REGISTER_OOVPAS(D3DDevice_SetVertexShaderConstant_0, UNPATCHED, 2024),
	REGISTER_OOVPAS(D3DDevice_SetVertexShader_0, UNPATCHED, 2024, 2036),
	REGISTER_OOVPAS(D3DDevice_SetViewport, PATCH, 1024, 1036, 1048),
	REGISTER_OOVPAS(D3DDevice_Swap, PATCH, 1024, 1036),
	REGISTER_OOVPAS(D3DDevice_Swap_0, PATCH, 2024, 2036),
	REGISTER_OOVPAS(D3DDevice_UpdateOverlay, PATCH, 1024, 1036, 1048),
	REGISTER_OOVPAS(D3DPalette_Lock2, PATCH, 1024),
	REGISTER_OOVPAS(D3DResource_GetType, UNPATCHED, 1024),
	REGISTER_OOVPAS(D3DResource_GetType_0, UNPATCHED, 2024),
	REGISTER_OOVPAS(D3DResource_Release, PATCH, 1024, 1036),
	REGISTER_OOVPAS(D3DSurface_GetDesc, PATCH, 1024),
	REGISTER_OOVPAS(D3DTexture_GetSurfaceLevel2, PATCH, 1024),
	REGISTER_OOVPAS(D3DTexture_LockRect, PATCH, 1024),
	REGISTER_OOVPAS(D3DVertexBuffer_Lock2, PATCH, 1024),
	REGISTER_OOVPAS(D3D_BlockOnResource_0, UNPATCHED, 2024, 2036, 2048, 2060),
	REGISTER_OOVPAS(D3D_CommonSetRenderTarget, UNPATCHED, 1024),
	REGISTER_OOVPAS(D3D_KickOffAndWaitForIdle2, PATCH, 1024, 1036, 1048),
	REGISTER_OOVPAS(Direct3D_CreateDevice, PATCH, 1024, 1036),
	REGISTER_OOVPAS(Direct3D_CreateDevice_16, UNPATCHED, 2024, 2048),
	REGISTER_OOVPAS(Direct3D_CreateDevice_4, UNPATCHED, 2060),
	REGISTER_OOVPAS(Get2DSurfaceDesc, UNPATCHED, 1024),
	REGISTER_OOVPAS(Get2DSurfaceDesc_0, UNPATCHED, 2024, 2036),
	REGISTER_OOVPAS(Get2DSurfaceDesc_4, UNPATCHED, 2048),
	REGISTER_OOVPAS(Lock3DSurface_16, UNPATCHED, 2048),
	REGISTER_OOVPAS(D3DDevice_SetFlickerFilter_0, UNPATCHED, 2048),

// *********************** Copy from D3D8.OOVPA.inl *****************
	REGISTER_OOVPAS(CMiniport_CreateCtxDmaObject, PATCH, 3911, 4034),
	REGISTER_OOVPAS(CMiniport_InitHardware, PATCH, 3911, 4034, 5455),
	REGISTER_OOVPAS(CMiniport_IsFlipPending, UNPATCHED, 3911, 4242, 4627, 4928),
	REGISTER_OOVPAS(D3DBaseTexture_GetLevelCount, PATCH, 3911),
	REGISTER_OOVPAS(D3DCubeTexture_GetCubeMapSurface, PATCH, 3911, 4627), // Called D3DCubeTexture_GetCubeMapSurface2 (from 4627's comment) NOTE: Use D3DCubeTexture_GetCubeMapSurface2 for 4627 and above
	REGISTER_OOVPAS(D3DCubeTexture_GetCubeMapSurface2, PATCH, 4627),
	REGISTER_OOVPAS(D3DCubeTexture_LockRect, PATCH, 3911), // Just calls Lock2DSurface (from 4134, 4432's comment)
	REGISTER_OOVPAS(D3DDevice_AddRef, PATCH, 3911, 4039, 4134, 4242, 4627, 5028, 5344, 5558, 5788),
	REGISTER_OOVPAS(D3DDevice_ApplyStateBlock, PATCH, 3911, 4627),
	REGISTER_OOVPAS(D3DDevice_Begin, PATCH, 3911, 4039),
	REGISTER_OOVPAS(D3DDevice_BeginPush, PATCH, 4134, 4627, 5028),
	REGISTER_OOVPAS(D3DDevice_BeginPushBuffer, PATCH, 3911, 4134), // Not implemented yet. (from 5788's comment)
	REGISTER_OOVPAS(D3DDevice_BeginStateBig, XREF, 5028),
	REGISTER_OOVPAS(D3DDevice_BeginStateBlock, PATCH, 3911, 4134),
	REGISTER_OOVPAS(D3DDevice_BeginVisibilityTest, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_BlockOnFence, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_BlockUntilVerticalBlank, PATCH, 3911, 4034, 4134, 4242, 4432, 4627, 5028, 5233, 5344, 5455, 5558, 5788),
	REGISTER_OOVPAS(D3DDevice_CaptureStateBlock, PATCH, 3911, 4134),
	REGISTER_OOVPAS(D3DDevice_Clear, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_CopyRects, PATCH, 3911, 4034, 4627, 5120),
	REGISTER_OOVPAS(D3DDevice_CreateCubeTexture, PATCH, 3911, 4627),
	REGISTER_OOVPAS(D3DDevice_CreateImageSurface, PATCH, 3911, 4034, 4627),//TODO 4721,4928 (from 4627's comment)
	REGISTER_OOVPAS(D3DDevice_CreateIndexBuffer, UNPATCHED, 3911, 4627), // TODO: This needs to be verified on 4361
	REGISTER_OOVPAS(D3DDevice_CreateIndexBuffer2, UNPATCHED, 4627, 5344),
	REGISTER_OOVPAS(D3DDevice_CreatePalette, PATCH, 3911, 4627), // Called D3DDevice_CreatePalette2 (from 4627's comment) NOTE: Use D3DDevice_CreatePalette2 for 4627 and above
	REGISTER_OOVPAS(D3DDevice_CreatePalette2, PATCH, 4627, 5344, 5455),
	REGISTER_OOVPAS(D3DDevice_CreatePixelShader, PATCH, 3911, 5344),
	REGISTER_OOVPAS(D3DDevice_CreateStateBlock, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_CreateTexture, PATCH, 3911, 4627),// Called D3DDevice_CreateTexture2 (from 4627's comment) NOTE: Use D3DDevice_CreateTexture2 for 4627 and above
	REGISTER_OOVPAS(D3DDevice_CreateTexture2, PATCH, 4627, 4831), // Was 4627 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_CreateVertexBuffer, UNPATCHED, 3911, 4627),
	REGISTER_OOVPAS(D3DDevice_CreateVertexBuffer2, UNPATCHED, 4627, 5344, 5455),
	REGISTER_OOVPAS(D3DDevice_CreateVertexShader, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_CreateVolumeTexture, PATCH, 3911, 4627),
	REGISTER_OOVPAS(D3DDevice_DeletePatch, UNPATCHED, 3911), // (TODO) (from 4034's comment)
	REGISTER_OOVPAS(D3DDevice_DeletePixelShader, PATCH, 3911, 5344),
	REGISTER_OOVPAS(D3DDevice_DeleteStateBlock, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_DeleteVertexShader, PATCH, 3911, 5344),
	REGISTER_OOVPAS(D3DDevice_DrawIndexedVertices, PATCH, 3911, 4034, 4627, 5028),
	REGISTER_OOVPAS(D3DDevice_DrawIndexedVerticesUP, PATCH, 3911, 4039, 5028, 5344),
	REGISTER_OOVPAS(D3DDevice_DrawRectPatch, PATCH, 3911), // TODO: Unused? (from 4034's comment)
	REGISTER_OOVPAS(D3DDevice_DrawTriPatch, UNPATCHED, 3911),
	REGISTER_OOVPAS(D3DDevice_DrawVertices, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_DrawVerticesUP, PATCH, 3911, 4039, 5344),
	REGISTER_OOVPAS(D3DDevice_EnableOverlay, PATCH, 3911, 4134),
	REGISTER_OOVPAS(D3DDevice_End, PATCH, 3911, 4039, 5344),
	REGISTER_OOVPAS(D3DDevice_EndPush, PATCH, 4134), // Was 4627 (from 5233's comment)
	REGISTER_OOVPAS(D3DDevice_EndPushBuffer, PATCH, 3911), // Not implemented yet. (from 5788's comment)
	REGISTER_OOVPAS(D3DDevice_EndStateBlock, PATCH, 3911, 4134),
	REGISTER_OOVPAS(D3DDevice_EndVisibilityTest, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_FlushVertexCache, PATCH, 3911, 4134),
	REGISTER_OOVPAS(D3DDevice_GetBackBuffer, PATCH, 3911, 4034, 4134, 4627), // Called D3DDevice_GetBackBuffer2 (from 4627's comment) NOTE: Use D3DDevice_GetBackBuffer2 for 4627 and above
	REGISTER_OOVPAS(D3DDevice_GetBackBuffer2, PATCH, 4627), // 5233 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_GetBackMaterial, PATCH, 3911, 4134, 4627, 5344, 5558, 5788),
	REGISTER_OOVPAS(D3DDevice_GetCreationParameters, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_GetDepthStencilSurface, PATCH, 3911, 4627), // Called D3DDevice_GetDepthStencilSurface2 (from 4627's comment) NOTE: Use D3DDevice_GetDepthStencilSurface2 for 4627 and above
	REGISTER_OOVPAS(D3DDevice_GetDepthStencilSurface2, PATCH, 4627),
	REGISTER_OOVPAS(D3DDevice_GetDeviceCaps, UNPATCHED, 3911),
	REGISTER_OOVPAS(D3DDevice_GetDisplayFieldStatus, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_GetDisplayMode, PATCH, 3911, 4039),
	REGISTER_OOVPAS(D3DDevice_GetGammaRamp, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_GetLight, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_GetLightEnable, PATCH, 3911, 5344),
	REGISTER_OOVPAS(D3DDevice_GetMaterial, PATCH, 3911, 4134, 4627, 5344, 5558, 5788),
	REGISTER_OOVPAS(D3DDevice_GetModelView, PATCH, 3911, 4134),
	REGISTER_OOVPAS(D3DDevice_GetOverlayUpdateStatus, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_GetPersistedSurface2, PATCH, 4928), // For only on Unreal Championship (from 4627's comment)
	REGISTER_OOVPAS(D3DDevice_GetPixelShader, PATCH, 3911, 4039, 4134, 5028, 5558, 5788),
	REGISTER_OOVPAS(D3DDevice_GetProjectionViewportMatrix, PATCH, 3911, 4134, 4627, 5344, 5558), // For 5455 (from 5558's comment)
	REGISTER_OOVPAS(D3DDevice_GetPushBufferOffset, PATCH, 3911, 4627),//TODO 4831 (from 4627's comment)
	REGISTER_OOVPAS(D3DDevice_GetRenderTarget, PATCH, 3911, 4627), // Called D3DDevice_GetRenderTarget2 (from 4627's comment) NOTE: Use D3DDevice_GetRenderTarget2 for 4627 and above
	REGISTER_OOVPAS(D3DDevice_GetRenderTarget2, PATCH, 4627),
	REGISTER_OOVPAS(D3DDevice_GetScissors, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_GetShaderConstantMode, PATCH, 3911, 4134, 4627, 5028, 5344, 5558, 5788),
	REGISTER_OOVPAS(D3DDevice_GetStreamSource2, PATCH, 4627),
	REGISTER_OOVPAS(D3DDevice_GetTexture2, PATCH, 3911, 4134, 4627, 5344, 5558, 5788),
	REGISTER_OOVPAS(D3DDevice_GetTile, PATCH, 3911, 5455),
	REGISTER_OOVPAS(D3DDevice_GetTransform, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_GetVertexShader, PATCH, 3911, 4039, 4134, 5028, 5558, 5788),
	REGISTER_OOVPAS(D3DDevice_GetVertexShaderConstant, PATCH, 3911, 4039, 5028),
	REGISTER_OOVPAS(D3DDevice_GetVertexShaderDeclaration, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_GetVertexShaderFunction, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_GetVertexShaderInput, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_GetVertexShaderSize, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_GetVertexShaderType, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_GetViewport, PATCH, 3911, 5455), // Was 4627 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_GetViewportOffsetAndScale, PATCH, 4432),
	REGISTER_OOVPAS(D3DDevice_GetVisibilityTestResult, PATCH, 3911, 5233),
	REGISTER_OOVPAS(D3DDevice_InsertCallback, PATCH, 3911, 4134, 5028),
	REGISTER_OOVPAS(D3DDevice_InsertFence, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_IsBusy, PATCH, 3911, 5028),
	REGISTER_OOVPAS(D3DDevice_IsFencePending, PATCH, 3911, 4039, 5028),
	REGISTER_OOVPAS(D3DDevice_KickOff, PATCH, 3911, 4034, 4531, 5028, 5455),
	REGISTER_OOVPAS(D3DDevice_KickPushBuffer, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_LazySetStateVB, XREF, 5028, 5558),
	REGISTER_OOVPAS(D3DDevice_LazySetStateUP, UNPATCHED, 5028, 5558),
	REGISTER_OOVPAS(D3DDevice_LightEnable, PATCH, 3911, 5344), // Was 5233 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_LoadVertexShader, PATCH, 3911, 4034, 4627, 5028),
	REGISTER_OOVPAS(D3DDevice_LoadVertexShaderProgram, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_MakeSpace, UNPATCHED, 4134),
	REGISTER_OOVPAS(D3DDevice_PersistDisplay, PATCH, 3911, 4039, 4627, 4831, 5455, 5558),
	REGISTER_OOVPAS(D3DDevice_Present, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_PrimeVertexCache, PATCH, 3911, 4134),
	REGISTER_OOVPAS(D3DDevice_Release, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_Reset, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_RunPushBuffer, PATCH, 3911, 4039, 4627, 5120, 5558), // for 5455 (from 5558's comment)
	REGISTER_OOVPAS(D3DDevice_RunVertexStateShader, PATCH, 3911, 4134),
	REGISTER_OOVPAS(D3DDevice_SelectVertexShader, PATCH, 3911, 4034, 5455),
	REGISTER_OOVPAS(D3DDevice_SelectVertexShaderDirect, PATCH, 4361),
	REGISTER_OOVPAS(D3DDevice_SetBackBufferScale, PATCH, 4134),
	REGISTER_OOVPAS(D3DDevice_SetBackMaterial, PATCH, 3911, 4134, 4627, 5344, 5558, 5788),
	REGISTER_OOVPAS(D3DDevice_SetDepthClipPlanes, PATCH, 4432),
	REGISTER_OOVPAS(D3DDevice_SetFlickerFilter, PATCH, 3911, 4034, 4134),
	REGISTER_OOVPAS(D3DDevice_SetGammaRamp, PATCH, 3911, 4627),
	REGISTER_OOVPAS(D3DDevice_SetIndices, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetLight, PATCH, 3911, 5344),
	REGISTER_OOVPAS(D3DDevice_SetMaterial, PATCH, 3911, 4034, 4134, 4627, 5344, 5558, 5788), // Was 4627 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_SetModelView, PATCH, 3911, 4134, 4627),
	REGISTER_OOVPAS(D3DDevice_SetPalette, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetPixelShader, PATCH, 3911, 4034, 4627),
	REGISTER_OOVPAS(D3DDevice_SetPixelShaderConstant, PATCH, 3911, 4831),
	REGISTER_OOVPAS(D3DDevice_SetPixelShaderProgram, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_BackFillMode, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_CullMode, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_Deferred, UNPATCHED, 3911),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_DoNotCullUncompressed, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_Dxt1NoiseEnable, PATCH, 3911, 4034, 4627), // Was 5233 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_SetRenderState_EdgeAntiAlias, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_FillMode, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_FogColor, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_FrontFace, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_LineWidth, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_LogicOp, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_MultiSampleAntiAlias, PATCH, 3911, 4034, 4627),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_MultiSampleMask, PATCH, 3911, 4034, 4627),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_MultiSampleMode, PATCH, 3925, 4034, 4134, 4627, 5233),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, PATCH, 4034), // Was 5233 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_SetRenderState_NormalizeNormals, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_OcclusionCullEnable, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_PSTextureModes, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_RopZCmpAlwaysRead, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_RopZRead, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_SampleAlpha, PATCH, 4627),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_ShadowFunc, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_Simple, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_StencilCullEnable, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_StencilEnable, PATCH, 3911, 4034, 5849),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_StencilFail, PATCH, 3911, 4034, 5849),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_TextureFactor, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_TwoSidedLighting, PATCH, 3911, 4034, 5344), // Beware of the typo... (from 4627's comment)
	REGISTER_OOVPAS(D3DDevice_SetRenderState_VertexBlend, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_YuvEnable, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_ZBias, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_ZEnable, PATCH, 3911, 4034, 4134, 4432),
	REGISTER_OOVPAS(D3DDevice_SetRenderTarget, PATCH, 3911, 3948, 4039, 4627, 5344, 5455),
	REGISTER_OOVPAS(D3DDevice_SetRenderTargetFast, PATCH, 5233),
	REGISTER_OOVPAS(D3DDevice_SetScissors, PATCH, 3911, 5344, 5558), // Was 5233 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_SetScreenSpaceOffset, PATCH, 4034, 5455), // Was 5233 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_SetShaderConstantMode, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetSoftDisplayFilter, PATCH, 3911, 4034, 4134),
	REGISTER_OOVPAS(D3DDevice_SetStateUP, UNPATCHED, 3911, 4034, 4134),
	REGISTER_OOVPAS(D3DDevice_SetStateVB, UNPATCHED, 3911, 4034, 4134),
	REGISTER_OOVPAS(D3DDevice_SetStipple, PATCH, 4627),
	REGISTER_OOVPAS(D3DDevice_SetStreamSource, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetSwapCallback, PATCH, 4134, 4242, 4432, 4627, 5028, 5233, 5344, 5455, 5558, 5788),
	REGISTER_OOVPAS(D3DDevice_SetTexture, PATCH, 3911, 4034, 4361, 4831),
	REGISTER_OOVPAS(D3DDevice_SetTextureState_BorderColor, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetTextureState_BumpEnv, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetTextureState_ColorKeyColor, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetTextureState_TexCoordIndex, PATCH, 3911, 4034, 4242, 4627),
	REGISTER_OOVPAS(D3DDevice_SetTile, PATCH, 3911, 4034, 4627), // Then it has changed calls ?SetTileNoWait@D3D@@YGXKPBU_D3DTILE@@@Z in XDK 4627 and higher.
	REGISTER_OOVPAS(D3DDevice_SetTransform, PATCH, 3911, 4034, 5344, 5558), // Was 4134 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_MultiplyTransform, PATCH, 3911, 4034), // TODO: Verify 4034 is correct
	REGISTER_OOVPAS(D3DDevice_SetVertexData2f, PATCH, 3911, 4039), // Was 4627 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_SetVertexData2s, PATCH, 3911, 4134),
	REGISTER_OOVPAS(D3DDevice_SetVertexData4f, PATCH, 3911, 4039),
	REGISTER_OOVPAS(D3DDevice_SetVertexData4s, PATCH, 3911, 4134),
	REGISTER_OOVPAS(D3DDevice_SetVertexData4ub, PATCH, 3911, 4134),
	REGISTER_OOVPAS(D3DDevice_SetVertexDataColor, PATCH, 3911, 4039),
	REGISTER_OOVPAS(D3DDevice_SetVertexShader, PATCH, 3911, 4034, 4134, 5028, 5455),
	REGISTER_OOVPAS(D3DDevice_SetVertexShaderConstant, PATCH, 3911, 4034, 4627),
	REGISTER_OOVPAS(D3DDevice_SetVertexShaderConstant1, PATCH, 4627),
	REGISTER_OOVPAS(D3DDevice_SetVertexShaderConstant1Fast, PATCH, 4627),
	REGISTER_OOVPAS(D3DDevice_SetVertexShaderConstant4, PATCH, 4627),
	REGISTER_OOVPAS(D3DDevice_SetVertexShaderConstantNotInline, PATCH, 4627, 5028, 5558),
	REGISTER_OOVPAS(D3DDevice_SetVertexShaderConstantNotInlineFast, PATCH, 4627),
	REGISTER_OOVPAS(D3DDevice_SetVertexShaderInput, PATCH, 3911, 4134),
	REGISTER_OOVPAS(D3DDevice_SetVertexShaderInputDirect, PATCH, 4361),
	REGISTER_OOVPAS(D3DDevice_SetVerticalBlankCallback, PATCH, 3911, 4039, 4134, 4242, 4432, 4627, 5028, 5233, 5344, 5455, 5558, 5788), // Was 5233 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_SetViewport, PATCH, 3911, 4034, 5344, 5455), // Was 5233 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_Swap, PATCH, 4034, 4531, 4627),
	REGISTER_OOVPAS(D3DDevice_SwitchTexture, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_UpdateOverlay, PATCH, 3911, 4531, 5233),
	REGISTER_OOVPAS(D3DPalette_Lock, PATCH, 3911, 4531, 4627), // Called D3DPalette_Lock2 (from 4627's comment) NOTE: Use D3DPalette_Lock2 for 4627 and above
	REGISTER_OOVPAS(D3DPalette_Lock2, PATCH, 4627),
	REGISTER_OOVPAS(D3DResource_AddRef, PATCH, 3911),
	REGISTER_OOVPAS(D3DResource_BlockUntilNotBusy, PATCH, 3911),
	REGISTER_OOVPAS(D3DResource_GetType, UNPATCHED, 3911, 4034), // Probably not even necessary... (from 3925's comment) // I forget why I had this in here... (from 5028's comment)
	REGISTER_OOVPAS(D3DResource_IsBusy, PATCH, 3911, 4039),
	REGISTER_OOVPAS(D3DResource_Register, PATCH, 3911),
	REGISTER_OOVPAS(D3DResource_Release, PATCH, 3911),
	REGISTER_OOVPAS(D3DSurface_GetDesc, PATCH, 3911),
	REGISTER_OOVPAS(D3DSurface_LockRect, PATCH, 3911),
	REGISTER_OOVPAS(D3DTexture_GetSurfaceLevel, PATCH, 3911, 4627),
	REGISTER_OOVPAS(D3DTexture_GetSurfaceLevel2, PATCH, 4627),
	REGISTER_OOVPAS(D3DTexture_LockRect, PATCH, 3911),
	REGISTER_OOVPAS(D3DVertexBuffer_GetDesc, UNPATCHED, 3911),
	REGISTER_OOVPAS(D3DVertexBuffer_Lock, PATCH, 3911, 4034, 4531, 4627),
	REGISTER_OOVPAS(D3DVertexBuffer_Lock2, PATCH, 4627),
	REGISTER_OOVPAS(D3DVolumeTexture_LockBox, PATCH, 3911), // Just calls Lock3DSurface (from 4134, 4432's comment)
	REGISTER_OOVPAS(D3D_AllocContiguousMemory, UNPATCHED, 3911, 5455), // Just calls MmAllocateContiguousMemory. Was PATCH (from 3925, 5788's comment)
	REGISTER_OOVPAS(D3D_BlockOnResource, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3D_BlockOnTime, PATCH, 3911, 4034, 4627, 5028, 5558),
	REGISTER_OOVPAS(D3D_CMiniport_GetDisplayCapabilities, PATCH, 3911),
	REGISTER_OOVPAS(D3D_CheckDeviceFormat, UNPATCHED, 3911),
	REGISTER_OOVPAS(D3D_ClearStateBlockFlags, XREF, 3911),
	REGISTER_OOVPAS(D3D_CommonSetRenderTarget, XREF, 4627, 5028), // Used between 4627 to 5233 (from 5344's comment)
	REGISTER_OOVPAS(D3D_CreateStandAloneSurface, XREF, 4034),
	REGISTER_OOVPAS(D3D_EnumAdapterModes, UNPATCHED, 3911),
	REGISTER_OOVPAS(D3D_GetAdapterDisplayMode, UNPATCHED, 3911, 4627),
	REGISTER_OOVPAS(D3D_GetAdapterIdentifier, UNPATCHED, 3911),
	REGISTER_OOVPAS(D3D_GetAdapterModeCount, UNPATCHED, 3911),
	REGISTER_OOVPAS(D3D_GetDeviceCaps, UNPATCHED, 3911),
	REGISTER_OOVPAS(D3D_KickOffAndWaitForIdle, PATCH, 3911, 4034, 4627, 5028),
	REGISTER_OOVPAS(D3D_LazySetPointParams, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3D_RecordStateBlock, XREF, 3911),
	REGISTER_OOVPAS(D3D_SetCommonDebugRegisters, PATCH, 3911),
	REGISTER_OOVPAS(D3D_SetFence, XREF, 3911, 4034, 4134, 5028, 5558),
	REGISTER_OOVPAS(D3D_SetPushBufferSize, UNPATCHED, 3911),
	REGISTER_OOVPAS(D3D_SetTileNoWait, XREF, 4627, 5455),
	REGISTER_OOVPAS(Direct3D_CheckDeviceMultiSampleType, UNPATCHED, 3911),
	REGISTER_OOVPAS(Direct3D_CreateDevice, PATCH, 3911, 5028),
	REGISTER_OOVPAS(Get2DSurfaceDesc, PATCH, 3911, 4034), // Was 5233 (from 5344's comment)
	REGISTER_OOVPAS(Lock2DSurface, PATCH, 3911),
	REGISTER_OOVPAS(Lock3DSurface, PATCH, 3911),
	REGISTER_OOVPAS(D3D_MakeRequestedSpace, PATCH, 4134, 5028, 5558),
	REGISTER_OOVPAS(XMETAL_StartPush, UNPATCHED, 3911),
	REGISTER_OOVPAS(IDirect3DVertexBuffer8_Lock, UNPATCHED, 4627),
};

// ******************************************************************
// * D3D8LTCG_OOVPA_COUNT
// ******************************************************************
uint32 D3D8LTCG_OOVPA_COUNT = OOVPA_TABLE_COUNT(D3D8LTCG_OOVPAV2);

#endif
