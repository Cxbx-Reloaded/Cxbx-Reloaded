// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.5849.cpp
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
// * D3DDevice_SetRenderState_StencilEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilEnable, 5849, 8)

        { 0x12, 0x8B },
        { 0x24, 0x33 },
        { 0x37, 0x74 },
        { 0x4A, 0x1E },
        { 0x5D, 0x74 },
        { 0x70, 0xB9 },
        { 0x83, 0x40 },
        { 0x96, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3D::SetFence
// * SOURCE: Spiderman 2
// ******************************************************************
OOVPA_XREF(D3D_SetFence, 5849, 7, XREF_D3D_SETFENCE, XRefZero)
	{ 0x0E, 0x05 },
	{ 0x17, 0xC7 },
	{ 0x3E, 0x40 },
	{ 0x5E, 0x00 },
	{ 0x87, 0x4E },
	{ 0x98, 0x83 },
	{ 0xA8, 0x75 },
OOVPA_END;

// ******************************************************************
// * D3D::BlockOnTime
// * Source: Spiderman 2
// ******************************************************************
OOVPA_XREF(D3D_BlockOnTime, 5849, 6, XREF_D3D_BLOCKONTIME, XRefZero)
	{ 0x09, 0x30 },
	{ 0x27, 0x07 },
	{ 0x7E, 0x2B },
	{ 0xA5, 0x20 },
	{ 0xD9, 0x56 },
	{ 0xF8, 0x47 },
OOVPA_END;

// ******************************************************************
// * Get2DSurfaceDesc
// ******************************************************************
// * NOTE: D3DTexture_GetLevelDesc and D3DSurface_GetDesc redirect here
// ******************************************************************
OOVPA_NO_XREF(Get2DSurfaceDesc, 5849, 10)

        // Get2DSurfaceDesc+0x2B : movzx edx, byte ptr [edi+0x0D]
        { 0x2B, 0x0F },
        { 0x2C, 0xB6 },
        { 0x2D, 0x57 },
        { 0x2E, 0x0D },

        // Get2DSurfaceDesc+0x52 : mov edx, [eax+0x1A14]
        { 0x52, 0x8B },
        { 0x53, 0x90 },
        { 0x54, 0x14 },
        { 0x55, 0x1A },

        // Get2DSurfaceDesc+0xAE : retn 0x0C
        { 0xAE, 0xC2 },
        { 0xAF, 0x0C },
OOVPA_END;

#if 0 // No longer used, replaced by generic 5455 version
// ******************************************************************
// * D3DDevice_SetScreenSpaceOffset
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetScreenSpaceOffset, 5849, 8)
	// D3DDevice_SetScreenSpaceOffset+0x13 : fstp [esi+0x0EF8]
	{ 0x13, 0xD9 },
	{ 0x14, 0x9E },
	{ 0x15, 0xF8 },
	{ 0x16, 0x0E },

	// D3DDevice_SetScreenSpaceOffset+0x33 : jb +0x05
	{ 0x33, 0x72 },
	{ 0x34, 0x05 },

	// D3DDevice_SetScreenSpaceOffset+0x46 : retn 0x08
	{ 0x46, 0xC2 },
	{ 0x47, 0x08 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice8::SetDepthClipPlanes
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetDepthClipPlanes, 5849, 11)

        // _D3DDevice_SetDepthClipPlanes+0x00 : mov eax, [esp+Flags]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x0C },

        // _D3DDevice_SetDepthClipPlanes+0x0F : ja short loc_27ABD0 ; jumptable 0027AB71 default case
        { 0x0F, 0x77 },
        { 0x10, 0x5F },

        // _D3DDevice_SetDepthClipPlanes+0x11 : jmp ds:off_27ABEC[eax*4] ; switch jump
        { 0x11, 0xFF },
        { 0x12, 0x24 },
        { 0x13, 0x85 },

        // _D3DDevice_SetDepthClipPlanes+0x88 : retn 0Ch
        { 0x88, 0xC2 },
        { 0x89, 0x0C },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_StencilFail
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilFail, 5849, 8)

        { 0x0C, 0x89 },
        { 0x1E, 0x8B },
        { 0x28, 0x74 },
        { 0x36, 0xD2 },
        { 0x44, 0x3D },
        { 0x52, 0xC7 },
        { 0x60, 0x04 },
        { 0x6E, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3D8_5849
// ******************************************************************
OOVPATable D3D8_5849[] = {

	REGISTER_OOVPA(CMiniport_InitHardware, 5558, PATCH),
	REGISTER_OOVPA(CMiniport_CreateCtxDmaObject, 4034, PATCH),
	REGISTER_OOVPA(D3D_CMiniport_GetDisplayCapabilities, 3911, PATCH),

	REGISTER_OOVPA(Direct3D_CreateDevice, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginVisibilityTest, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_EndVisibilityTest, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVisibilityTestResult, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_GetViewport, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_SetBackBufferScale, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_GetCreationParameters, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_Clear, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_Swap, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackBuffer2, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDepthStencilSurface2, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVertexShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePixelShader, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShader, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayMode, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData2f, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4f, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4ub, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexDataColor, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_End, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVerticalBlankCallback, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_TexCoordIndex, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BumpEnv, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BorderColor, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_CullMode, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TextureFactor, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_NormalizeNormals, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZBias, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_EdgeAntiAlias, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FillMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FogColor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Simple, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZEnable, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilEnable, 5849, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_OcclusionCullEnable, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleAntiAlias, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ShadowFunc, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_YuvEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShader, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetIndices, 5558, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetTexture, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_LazySetStateVB, 5028, XREF),
	REGISTER_OOVPA(D3DDevice_BeginPush, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_EndPush, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginPushBuffer, 4134, PATCH), // Not implemented yet.
	REGISTER_OOVPA(D3DDevice_EndPushBuffer, 5788, PATCH), // Not implemented yet.
	REGISTER_OOVPA(D3DDevice_RunPushBuffer, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_Begin, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginStateBig, 5028, XREF), 
	REGISTER_OOVPA(D3DDevice_GetTransform, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTransform, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVertices, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVerticesUP, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_SetMaterial, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_SetStreamSource, 4034, PATCH),
	REGISTER_OOVPA(D3DResource_Register, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_AddRef, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_Release, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_IsBusy, 4039, PATCH),
	REGISTER_OOVPA(D3DSurface_GetDesc, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CopyRects, 5233, PATCH),
	REGISTER_OOVPA(D3DPalette_Lock2, 4627, PATCH),
	REGISTER_OOVPA(D3DTexture_LockRect, 3925, PATCH),
	REGISTER_OOVPA(D3DSurface_LockRect, 3925, PATCH),
	REGISTER_OOVPA(D3DBaseTexture_GetLevelCount, 3911, PATCH),
	REGISTER_OOVPA(D3DTexture_GetSurfaceLevel2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetLight, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetLight, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVertexBuffer2, 5558, DISABLED),
	REGISTER_OOVPA(D3DVertexBuffer_Lock, 5788, PATCH),
	REGISTER_OOVPA(D3DVertexBuffer_Lock2, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_LightEnable, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVertices, 5028, PATCH),
	REGISTER_OOVPA(D3D_SetFence, 5558, XREF),
	REGISTER_OOVPA(D3DDevice_InsertFence, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_IsFencePending, 5558, PATCH),
	REGISTER_OOVPA(D3D_BlockOnTime, 5849, XREF),
	REGISTER_OOVPA(D3DDevice_BlockOnFence, 5233, PATCH),
	REGISTER_OOVPA(D3D_AllocContiguousMemory, 5788, DISABLED), // Just calls MmAllocateContiguousMemory. Was PATCH
	REGISTER_OOVPA(Get2DSurfaceDesc, 5849, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateTexture2, 5028, PATCH),
	REGISTER_OOVPA(Lock2DSurface, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant4, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant1, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant1, 4627, ALIAS, D3DDevice_SetVertexShaderConstant1Fast),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstantNotInline, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstantNotInline, 4627, ALIAS, D3DDevice_SetVertexShaderConstantNotInlineFast),
	REGISTER_OOVPA(D3DDevice_DrawVerticesUP, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderTarget, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetShaderConstantMode, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetViewport, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_BlockUntilVerticalBlank, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScreenSpaceOffset, 5455, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FrontFace, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetBackMaterial, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TwoSidedLighting, 5558, PATCH),
	REGISTER_OOVPA(D3D_KickOffAndWaitForIdle, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTile, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_SetGammaRamp, 4928, PATCH),
	REGISTER_OOVPA(D3DDevice_GetGammaRamp, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSwapCallback, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_GetRenderTarget2, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilFail, 5849, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_VertexBlend, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_Reset, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_Release, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_InsertCallback, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LineWidth, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_SampleAlpha, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_ColorKeyColor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_EnableOverlay, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_UpdateOverlay, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_GetOverlayUpdateStatus, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LogicOp, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMask, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPalette, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_LoadVertexShaderProgram, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_GetPushBufferOffset, 4627, PATCH),
	REGISTER_OOVPA(MakeRequestedSpace, 5788, XREF),
	REGISTER_OOVPA(D3D_Unknown, 5788, XREF),
	REGISTER_OOVPA(D3DDevice_FlushVertexCache, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderSize, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_LoadVertexShader, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SelectVertexShader, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_RunVertexStateShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_AddRef, 5788, PATCH),
	REGISTER_OOVPA(D3D_ClearStateBlockFlags, 5788, XREF),
	REGISTER_OOVPA(D3D_RecordStateBlock, 5788, XREF),
	REGISTER_OOVPA(D3DDevice_BeginStateBlock, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_EndStateBlock, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_BackFillMode, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Dxt1NoiseEnable, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTile, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_KickOff, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZCmpAlwaysRead, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZRead, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_DoNotCullUncompressed, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSoftDisplayFilter, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetFlickerFilter, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMode, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderConstant, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayFieldStatus, 5788, PATCH),
	REGISTER_OOVPA(D3DCubeTexture_GetCubeMapSurface2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderProgram, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScissors, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_GetScissors, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackMaterial, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_ApplyStateBlock, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_CaptureStateBlock, 5455, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteStateBlock, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilCullEnable, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTexture2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderTargetFast, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_SetDepthClipPlanes, 5849, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderInput, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_GetViewportOffsetAndScale, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateStateBlock, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateIndexBuffer2, 5344, DISABLED),
	REGISTER_OOVPA(D3DDevice_CreatePalette2, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SwitchTexture, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_DeletePixelShader, 5344, PATCH),
	REGISTER_OOVPA(D3D_BlockOnResource, 4034, XREF),
	REGISTER_OOVPA(D3DResource_BlockUntilNotBusy, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawTriPatch, 3911, DISABLED),
	REGISTER_OOVPA(D3DDevice_IsBusy, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_GetMaterial, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_GetStreamSource2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData2s, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateCubeTexture, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant1Fast, 4627, XREF),
	REGISTER_OOVPA(D3DDevice_SetRenderState_PSTextureModes, 4034, PATCH),
	REGISTER_OOVPA(D3DVolumeTexture_LockBox, 3925, PATCH),// Just calls Lock3DSurface
	REGISTER_OOVPA(D3DCubeTexture_LockRect, 3925, PATCH),
	REGISTER_OOVPA(Lock3DSurface, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_PersistDisplay, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4s, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_GetProjectionViewportMatrix, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_PrimeVertexCache, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteVertexShader, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_GetModelView, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetShaderConstantMode, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShader, 5788, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderConstant, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderInput, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderType, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderDeclaration, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderFunction, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetPixelShader, 5788, PATCH),
	REGISTER_OOVPA(D3D_LazySetPointParams, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SelectVertexShaderDirect, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetStipple, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderInputDirect, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetModelView, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVolumeTexture, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawRectPatch, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateImageSurface, 4627, PATCH),
};

// ******************************************************************
// * D3D8_5849_SIZE
// ******************************************************************
uint32 D3D8_5849_SIZE = sizeof(D3D8_5849);
