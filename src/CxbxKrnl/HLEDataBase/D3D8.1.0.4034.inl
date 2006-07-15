// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.4034.cpp
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
// * D3D8_1_0_4034
// ******************************************************************
OOVPATable D3D8_1_0_4034[] =
{
    // IDirect3D8::CreateDevice (* unchanged since 1.0.3925 *)
    {
        (OOVPA*)&IDirect3D8_CreateDevice_1_0_3925,

        XTL::EmuIDirect3D8_CreateDevice,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_CreateDevice" 
        #endif
    },


	// ********************** BEG WARNING UNTESTED!!! *******************

    
	// IDirect3DDevice8::GetDeviceCaps
    {
        (OOVPA*)&IDirect3DDevice8_GetDeviceCaps_1_0_3925,

        XTL::EmuIDirect3DDevice8_GetDeviceCaps,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetDeviceCaps" 
        #endif
    },
    // IDirect3DDevice8::BeginVisibilityTest
    {
        (OOVPA*)&IDirect3DDevice8_BeginVisibilityTest_1_0_3925,

        XTL::EmuIDirect3DDevice8_BeginVisibilityTest,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_BeginVisibilityTest" 
        #endif
    },
    // IDirect3DDevice8::EndVisibilityTest
    {
        (OOVPA*)&IDirect3DDevice8_EndVisibilityTest_1_0_3925,

        XTL::EmuIDirect3DDevice8_EndVisibilityTest,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_EndVisibilityTest" 
        #endif
    },
    // IDirect3DDevice8::LoadVertexShader
    {
        (OOVPA*)&IDirect3DDevice8_LoadVertexShader_1_0_3925,

        XTL::EmuIDirect3DDevice8_LoadVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_LoadVertexShader" 
        #endif
    },
    // IDirect3DDevice8::SelectVertexShader
    {
        (OOVPA*)&IDirect3DDevice8_SelectVertexShader_1_0_3925,

        XTL::EmuIDirect3DDevice8_SelectVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SelectVertexShader" 
        #endif
    },
    // IDirect3DDevice8::Release
    {
        (OOVPA*)&IDirect3DDevice8_Release_1_0_3925,

        XTL::EmuIDirect3DDevice8_Release,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Release" 
        #endif
    },
    // IDirect3DDevice8::BlockUntilVerticalBlank
    {
        (OOVPA*)&IDirect3DDevice8_BlockUntilVerticalBlank_1_0_3925,

        XTL::EmuIDirect3DDevice8_BlockUntilVerticalBlank,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_BlockUntilVerticalBlank" 
        #endif
    },
    // IDirect3DDevice8::SetVerticalBlankCallback
    {
        (OOVPA*)&IDirect3DDevice8_SetVerticalBlankCallback_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetVerticalBlankCallback,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVerticalBlankCallback" 
        #endif
    },
    // IDirect3DDevice8::SetRenderTarget
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderTarget_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderTarget,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderTarget" 
        #endif
    },
    // IDirect3DDevice8::GetTransform
    {
        (OOVPA*)&IDirect3DDevice8_GetTransform_1_0_3925,

        XTL::EmuIDirect3DDevice8_GetTransform,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetTransform" 
        #endif
    },
    // IDirect3DDevice8::SetStreamSource
    {
        (OOVPA*)&IDirect3DDevice8_SetStreamSource_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetStreamSource,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetStreamSource" 
        #endif
    },
    // IDirect3DDevice8::SetVertexShader
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShader_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShader" 
        #endif
    },
    // IDirect3DDevice8::SetPixelShader
    {
        (OOVPA*)&IDirect3DDevice8_SetPixelShader_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetPixelShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetPixelShader" 
        #endif
    },
    // IDirect3DDevice8::SetIndices
    {
        (OOVPA*)&IDirect3DDevice8_SetIndices_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetIndices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetIndices" 
        #endif
    },
    // IDirect3DDevice8::SetViewport
    {
        (OOVPA*)&IDirect3DDevice8_SetViewport_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetViewport,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetViewport" 
        #endif
    },
    // IDirect3DDevice8::SetTexture
    {
        (OOVPA*)&IDirect3DDevice8_SetTexture_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetTexture,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTexture" 
        #endif
    },
    // IDirect3DDevice8::DrawIndexedVertices
    {
        (OOVPA*)&IDirect3DDevice8_DrawIndexedVertices_1_0_3925,

        XTL::EmuIDirect3DDevice8_DrawIndexedVertices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawIndexedVertices" 
        #endif
    },
    // IDirect3DDevice8::Begin
    {
        (OOVPA*)&IDirect3DDevice8_Begin_1_0_3925,

        XTL::EmuIDirect3DDevice8_Begin,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Begin" 
        #endif
    },
    // IDirect3DDevice8::SetVertexData2f
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexData2f_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetVertexData2f,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexData2f" 
        #endif
    },
    // IDirect3DDevice8::SetVertexData4f
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexData4f_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetVertexData4f,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexData4f" 
        #endif
    },
    // IDirect3DDevice8::SetVertexDataColor
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexDataColor_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetVertexDataColor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexDataColor" 
        #endif
    },
    // IDirect3DDevice8::End
    {
        (OOVPA*)&IDirect3DDevice8_End_1_0_3925,

        XTL::EmuIDirect3DDevice8_End,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_End" 
        #endif
    },
    // IDirect3DDevice8::Clear
    {
        (OOVPA*)&IDirect3DDevice8_Clear_1_0_3925,

        XTL::EmuIDirect3DDevice8_Clear,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Clear" 
        #endif
    },
    // IDirect3DDevice8::CreatePalette
    {
        (OOVPA*)&IDirect3DDevice8_CreatePalette_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreatePalette,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreatePalette" 
        #endif
    },
    // IDirect3DDevice8::SetPalette
    {
        (OOVPA*)&IDirect3DDevice8_SetPalette_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetPalette,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetPalette" 
        #endif
    },
    // IDirect3DDevice8::CreateTexture
    {
        (OOVPA*)&IDirect3DDevice8_CreateTexture_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreateTexture,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateTexture"
        #endif
    },
    // IDirect3DDevice8::CreateVolumeTexture
    {
        (OOVPA*)&IDirect3DDevice8_CreateVolumeTexture_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreateVolumeTexture,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateVolumeTexture"
        #endif
    },
    // IDirect3DDevice8::CreateCubeTexture
    {
        (OOVPA*)&IDirect3DDevice8_CreateCubeTexture_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreateCubeTexture,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateCubeTexture"
        #endif
    },
    // IDirect3DDevice8::CreateIndexBuffer
    {
        (OOVPA*)&IDirect3DDevice8_CreateIndexBuffer_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreateIndexBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateIndexBuffer"
        #endif
    },
    // IDirect3DDevice8::CreateVertexShader
    {
        (OOVPA*)&IDirect3DDevice8_CreateVertexShader_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreateVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateVertexShader"
        #endif
    },
    // IDirect3DDevice8::SetVertexShaderConstant
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShaderConstant_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetVertexShaderConstant,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShaderConstant" 
        #endif
    },
    // IDirect3DDevice8::SetFlickerFilter
    {
        (OOVPA*)&IDirect3DDevice8_SetFlickerFilter_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetFlickerFilter,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetFlickerFilter" 
        #endif
    },
    // IDirect3DDevice8::SetSoftDisplayFilter
    {
        (OOVPA*)&IDirect3DDevice8_SetSoftDisplayFilter_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetSoftDisplayFilter,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetSoftDisplayFilter" 
        #endif
    },
    // IDirect3DDevice8::SetTextureState_TexCoordIndex
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetTextureState_TexCoordIndex,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_TexCoordIndex" 
        #endif
    },
    // IDirect3DDevice8::SetTextureState_BorderColor
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureState_BorderColor_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetTextureState_BorderColor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_BorderColor" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_PSTextureModes
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_PSTextureModes_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_PSTextureModes,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_PSTextureModes" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_StencilFail
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_StencilFail_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_StencilFail,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_StencilFail" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_CullMode
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_CullMode_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_CullMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_CullMode" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_Simple
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_Simple_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_Simple,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_Simple" 
        #endif
    },
    // IDirect3DDevice8::SetTransform
    {
        (OOVPA*)&IDirect3DDevice8_SetTransform_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetTransform,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTransform" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_FogColor
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_FogColor_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_FogColor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_FogColor" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_FillMode
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_FillMode_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_FillMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_FillMode" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_StencilEnable
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_StencilEnable_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_StencilEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_StencilEnable" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_Dxt1NoiseEnable
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_ZBias
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_ZBias_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_ZBias,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_ZBias" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_ZEnable
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_ZEnable_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_ZEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_ZEnable" 
        #endif
    },
    // IDirect3DDevice8::Present
    {
        (OOVPA*)&IDirect3DDevice8_Present_1_0_3925,

        XTL::EmuIDirect3DDevice8_Present,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Present"
        #endif
    },
    // IDirect3DDevice8::SetShaderConstantMode
    {
        (OOVPA*)&IDirect3DDevice8_SetShaderConstantMode_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetShaderConstantMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetShaderConstantMode"
        #endif
    },
    // IDirect3DDevice8::GetBackBuffer
    {
        (OOVPA*)&IDirect3DDevice8_GetBackBuffer_1_0_3925,

        XTL::EmuIDirect3DDevice8_GetBackBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetBackBuffer"
        #endif
    },
    // IDirect3DDevice8::GetDepthStencilSurface
    {
        (OOVPA*)&IDirect3DDevice8_GetDepthStencilSurface_1_0_3925,

        XTL::EmuIDirect3DDevice8_GetDepthStencilSurface,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetDepthStencilSurface"
        #endif
    },
    // IDirect3DDevice8::CreateVertexBuffer
    {
        (OOVPA*)&IDirect3DDevice8_CreateVertexBuffer_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreateVertexBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateVertexBuffer" 
        #endif
    },
    // IDirect3DVertexBuffer8::Lock
    {
        (OOVPA*)&IDirect3DVertexBuffer8_Lock_1_0_3925,

        XTL::EmuIDirect3DVertexBuffer8_Lock,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DVertexBuffer8_Lock" 
        #endif
    },
    // IDirect3DResource8::Register
    {
        (OOVPA*)&IDirect3DResource8_Register_1_0_3925,

        XTL::EmuIDirect3DResource8_Register,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DResource8_Register"
        #endif
    },
    // IDirect3DResource8::Release
    {
        (OOVPA*)&IDirect3DResource8_Release_1_0_3925,

        XTL::EmuIDirect3DResource8_Release,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DResource8_Release"
        #endif
    },
    // IDirect3DResource8::AddRef
    {
        (OOVPA*)&IDirect3DResource8_AddRef_1_0_3925,

        XTL::EmuIDirect3DResource8_AddRef,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DResource8_AddRef"
        #endif
    },
    // IDirect3DResource8::IsBusy
    {
        (OOVPA*)&IDirect3DResource8_IsBusy_1_0_3925,

        XTL::EmuIDirect3DResource8_IsBusy,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DResource8_IsBusy"
        #endif
    },
    // IDirect3DPalette8::Lock
    {
        (OOVPA*)&IDirect3DPalette8_Lock_1_0_3925,

        XTL::EmuIDirect3DPalette8_Lock,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DPalette8_Lock"
        #endif
    },
    // IDirect3DTexture8::LockRect
    {
        (OOVPA*)&IDirect3DTexture8_LockRect_1_0_3925,

        XTL::EmuIDirect3DTexture8_LockRect,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DTexture8_LockRect" 
        #endif
    },
    // IDirect3DVolumeTexture8::LockBox
    {
        (OOVPA*)&IDirect3DVolumeTexture8_LockBox_1_0_3925,

        XTL::EmuIDirect3DVolumeTexture8_LockBox,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DVolumeTexture8_LockBox" 
        #endif
    },
    // IDirect3DCubeTexture8::LockRect
    {
        (OOVPA*)&IDirect3DCubeTexture8_LockRect_1_0_3925,

        XTL::EmuIDirect3DCubeTexture8_LockRect,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DCubeTexture8_LockRect" 
        #endif
    },
    // IDirect3DTexture8::GetSurfaceLevel
    {
        (OOVPA*)&IDirect3DTexture8_GetSurfaceLevel_1_0_3925,

        XTL::EmuIDirect3DTexture8_GetSurfaceLevel,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DTexture8_GetSurfaceLevel" 
        #endif
    },
    // Get2DSurfacDesc
    {
        (OOVPA*)&Get2DSurfaceDesc_1_0_3925,

        XTL::EmuGet2DSurfaceDesc,

        #ifdef _DEBUG_TRACE
        "EmuGet2DSurfaceDesc" 
        #endif
    },

	// ********************** END WARNING UNTESTED!!! *******************
};

// ******************************************************************
// * D3D8_1_0_4034_SIZE
// ******************************************************************
uint32 D3D8_1_0_4034_SIZE = sizeof(D3D8_1_0_4034);
