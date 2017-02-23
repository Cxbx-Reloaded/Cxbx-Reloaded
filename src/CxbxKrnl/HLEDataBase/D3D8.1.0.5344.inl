// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.5344.cpp
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
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *
// *  All rights reserved
// *
// ******************************************************************

// ******************************************************************
// * D3D8_5344
// ******************************************************************
OOVPATable D3D8_5344[] = {

	REGISTER_OOVPA(D3D_CreateDevice, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayFieldStatus, 5233, PATCH),
	REGISTER_OOVPA(D3D_CheckDeviceFormat, 4361, PATCH),
	REGISTER_OOVPA(D3D_KickOffAndWaitForIdle, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_CopyRects, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScissors, 5233, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetScissors, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_GetGammaRamp, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetGammaRamp, 4928, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDeviceCaps, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetShaderConstantMode, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_GetRenderTarget2, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDepthStencilSurface2, 5233, PATCH),
	REGISTER_OOVPA(D3D_CommonSetRenderTarget, 5233, XREF),
	REGISTER_OOVPA(D3DDevice_SetRenderTarget, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_AddRef, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_Reset, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTileNoWait, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateIndexBuffer2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVertexShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginStateBlock, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_EndStateBlock, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_RunVertexStateShader, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderSize, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant1, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant4, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstantNotInline, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_DeletePixelShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePixelShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShader, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateTexture2, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateTexture2, 4627, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetIndices, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTexture, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTexture, 5233, ALIAS, D3DDevice_SetTextureB),
	REGISTER_OOVPA(D3DDevice_SetTexture, 4928, DISABLED),
	REGISTER_OOVPA(D3DDevice_GetDisplayMode, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData2f, 4627, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetVertexData2f, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_Begin, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_End, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_Clear, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_Swap, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_EnableOverlay, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVertexBuffer2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_TexCoordIndex, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BumpEnv, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BorderColor, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_ColorKeyColor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_VertexBlend, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_CullMode, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_CullMode, 4134, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetRenderState_NormalizeNormals, 4432, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TextureFactor, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZBias, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_EdgeAntiAlias, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FillMode, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_BackFillMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FogColor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Dxt1NoiseEnable, 5233, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Dxt1NoiseEnable, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Simple, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZEnable, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilEnable, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilFail, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LineWidth, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_OcclusionCullEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilCullEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZCmpAlwaysRead, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZRead, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_DoNotCullUncompressed, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleAntiAlias, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMode, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 5233, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ShadowFunc, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SwitchTexture, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_YuvEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTransform, 4134, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetTransform, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_SetStreamSource, 4361, PATCH),
	REGISTER_OOVPA(D3D_GetDeviceCaps, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetLight, 5233, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetLight, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_LightEnable, 5233, DISABLED),
	REGISTER_OOVPA(D3DDevice_LightEnable, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVertices, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetMaterial, 4627, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetMaterial, 5344, PATCH),
	REGISTER_OOVPA(D3DVertexBuffer_GetDesc, 5233, PATCH),
	REGISTER_OOVPA(D3DVertexBuffer_Lock2, 4627, PATCH),
	REGISTER_OOVPA(D3DResource_Register, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackBuffer2, 5233, DISABLED),
	REGISTER_OOVPA(D3DDevice_GetBackBuffer2, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_UpdateOverlay, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_GetOverlayUpdateStatus, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_BlockUntilVerticalBlank, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_GetViewport, 4627, DISABLED),
	REGISTER_OOVPA(D3DDevice_GetViewport, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_SetViewport, 5233, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetViewport, 5344, PATCH),
	REGISTER_OOVPA(D3DResource_AddRef, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_Release, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_IsBusy, 4361, PATCH),
	REGISTER_OOVPA(D3DPalette_Lock2, 5233, PATCH),
	REGISTER_OOVPA(D3DTexture_LockRect, 5233, PATCH),
	REGISTER_OOVPA(Lock2DSurface, 3925, PATCH),
	REGISTER_OOVPA(Get2DSurfaceDesc, 5233, DISABLED),
	REGISTER_OOVPA(Get2DSurfaceDesc, 5344, PATCH),
	REGISTER_OOVPA(D3DSurface_GetDesc, 4361, PATCH),
	REGISTER_OOVPA(D3DSurface_LockRect, 3925, PATCH),
	REGISTER_OOVPA(D3DBaseTexture_GetLevelCount, 4361, PATCH),
	REGISTER_OOVPA(D3DTexture_GetSurfaceLevel2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShader, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVertices, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_GetViewportOffsetAndScale, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteVertexShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SelectVertexShaderDirect, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_LoadVertexShader, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_Release, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScreenSpaceOffset, 5233, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetScreenSpaceOffset, 5344, PATCH),
	REGISTER_OOVPA(D3D_SetFence, 5233, XREF),
	REGISTER_OOVPA(D3DDevice_InsertFence, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_IsFencePending, 5233, PATCH),
	REGISTER_OOVPA(D3D_BlockOnTime, 5233, XREF),
	REGISTER_OOVPA(D3DDevice_BlockOnFence, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTransform, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPalette, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_GetCreationParameters, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetFlickerFilter, 5233, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetFlickerFilter, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSoftDisplayFilter, 5233, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetSoftDisplayFilter, 5344, PATCH),
	// REGISTER_OOVPA(D3DDevice_Unknown1, 5233, PATCH)
	REGISTER_OOVPA(D3DDevice_SetRenderState_TwoSidedLighting, 4134, DISABLED), // Beware of the typo...
	REGISTER_OOVPA(D3DDevice_SetRenderState_TwoSidedLighting, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FrontFace, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LogicOp, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginPush, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginVisibilityTest, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_EndPush, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_EndPush, 4627, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetVerticalBlankCallback, 5233, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetVerticalBlankCallback, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_SampleAlpha, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMask, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4ub, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4f, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSwapCallback, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderTargetFast, 5344, PATCH),
	REGISTER_OOVPA(D3DCubeTexture_GetCubeMapSurface2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_ApplyStateBlock, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_EndVisibilityTest, 4627, PATCH),
};

// ******************************************************************
// * D3D8_5344_SIZE
// ******************************************************************
uint32 D3D8_5344_SIZE = sizeof(D3D8_5344);
