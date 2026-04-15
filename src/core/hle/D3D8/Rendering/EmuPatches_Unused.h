#ifndef EMUPATCHES_UNUSED_H
#define EMUPATCHES_UNUSED_H

// EMUPATCH declarations that have no implementation yet.
// Corresponding function bodies are in Direct3D9.cpp.unused-patches.

#include "core\hle\XAPI\Xapi.h" // For EMUPATCH
#include "core\hle\D3D8\XbD3D8Types.h"

namespace xbox {

xbox::hresult_xt WINAPI EMUPATCH(Direct3D_CreateDevice_16__LTCG_eax4_ebx6)
(
    uint_xt                     Adapter,
	X_D3DDEVTYPE                DeviceType,
	X_HWND                      hFocusWindow,
    X_D3DPRESENT_PARAMETERS    *pPresentationParameters
);
xbox::hresult_xt WINAPI EMUPATCH(Direct3D_CreateDevice_16__LTCG_eax4_ecx6)
(
    uint_xt                     Adapter,
	X_D3DDEVTYPE                DeviceType,
	X_HWND                      hFocusWindow,
    X_D3DPRESENT_PARAMETERS    *pPresentationParameters
);
xbox::hresult_xt WINAPI EMUPATCH(Direct3D_CreateDevice_4__LTCG_eax1_ecx3)
(
    X_D3DPRESENT_PARAMETERS    *pPresentationParameters
);
// ******************************************************************
// * patch: D3DDevice_GetCreationParameters
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetCreationParameters)
(
	X_D3DDEVICE_CREATION_PARAMETERS *pParameters
);
#endif
// ******************************************************************
// * patch: D3D_CheckDeviceFormat
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3D_CheckDeviceFormat)
(
    uint_xt                     Adapter,
	X_D3DDEVTYPE                DeviceType,
    X_D3DFORMAT                 AdapterFormat,
    dword_xt                    Usage,
    X_D3DRESOURCETYPE           RType,
    X_D3DFORMAT                 CheckFormat
);
#endif
// ******************************************************************
// * patch: D3DDevice_GetDeviceCaps
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetDeviceCaps)
(
    X_D3DCAPS                   *pCaps
);
#endif
// ******************************************************************
// * patch: D3D_KickOffAndWaitForIdle
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3D_KickOffAndWaitForIdle)();
// ******************************************************************
// * patch: D3D_KickOffAndWaitForIdle
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3D_KickOffAndWaitForIdle2)(dword_xt dwDummy1, dword_xt dwDummy2);
// ******************************************************************
// * patch: D3DDevice_AddRef
// ******************************************************************
xbox::ulong_xt WINAPI EMUPATCH(D3DDevice_AddRef)();
// ******************************************************************
// * patch: D3DDevice_BeginStateBlock
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_BeginStateBlock)();
// ******************************************************************
// * patch: D3DDevice_CaptureStateBlock
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_CaptureStateBlock)(dword_xt Token);
// ******************************************************************
// * patch: D3DDevice_ApplyStateBlock
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_ApplyStateBlock)(dword_xt Token);
// ******************************************************************
// * patch: D3DDevice_EndStateBlock
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_EndStateBlock)(dword_xt *pToken);
// ******************************************************************
// * patch: D3DDevice_CreateImageSurface
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_CreateImageSurface)
(
    uint_xt                Width,
    uint_xt                Height,
    X_D3DFORMAT         Format,
    X_D3DSurface      **ppBackBuffer
);
// ******************************************************************
// * patch: D3DDevice_GetViewport
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetViewport)
(
    X_D3DVIEWPORT8 *pViewport
);
// ******************************************************************
// * patch: D3DDevice_GetViewportOffsetAndScale
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetViewportOffsetAndScale)
(
	X_D3DXVECTOR4 *pOffset,
	X_D3DXVECTOR4 *pScale
);
xbox::void_xt __stdcall EMUPATCH(D3DDevice_GetViewportOffsetAndScale_0)();
// ******************************************************************
// * patch: D3DDevice_GetRenderTarget
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_GetRenderTarget)
(
    X_D3DSurface  **ppRenderTarget
);
// ******************************************************************
// * patch: D3DDevice_GetRenderTarget
// ******************************************************************
X_D3DSurface * WINAPI EMUPATCH(D3DDevice_GetRenderTarget2)();
// ******************************************************************
// * patch: D3DDevice_GetDepthStencilSurface
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_GetDepthStencilSurface)
(
    X_D3DSurface  **ppZStencilSurface
);
// ******************************************************************
// * patch: D3DDevice_GetDepthStencilSurface
// ******************************************************************
X_D3DSurface * WINAPI EMUPATCH(D3DDevice_GetDepthStencilSurface2)();
// ******************************************************************
// * patch: D3DDevice_GetTile
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetTile)
(
    dword_xt           Index,
    X_D3DTILE      *pTile
);
// ******************************************************************
// * patch: D3DDevice_SetTile
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetTile)
(
    dword_xt               Index,
    CONST X_D3DTILE    *pTile
);
// ******************************************************************
// * patch: D3DDevice_CreateVertexShader
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_CreateVertexShader)
(
    CONST dword_xt    *pDeclaration,
    CONST dword_xt    *pFunction,
    dword_xt          *pHandle,
    dword_xt           Usage
);
// ******************************************************************
// * patch: D3DDevice_DeletePixelShader
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_DeletePixelShader)
(
    dword_xt          Handle
);
// ******************************************************************
// * patch: D3DDevice_CreatePixelShader
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_CreatePixelShader)
(
    X_D3DPIXELSHADERDEF    *pPSDef,
    dword_xt				   *pHandle
);
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetPixelShader_0__LTCG_eax1)();
// ******************************************************************
// * patch: D3DDevice_CreateTexture2
// ******************************************************************
X_D3DResource * WINAPI EMUPATCH(D3DDevice_CreateTexture2)
(
    uint_xt                Width,
    uint_xt                Height,
    uint_xt                Depth,
    uint_xt                Levels,
    dword_xt               Usage,
    X_D3DFORMAT         Format,
    X_D3DRESOURCETYPE   D3DResource
);
// ******************************************************************
// * patch: D3DDevice_CreateTexture
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_CreateTexture)
(
    uint_xt             Width,
    uint_xt             Height,
    uint_xt             Levels,
    dword_xt            Usage,
    X_D3DFORMAT         Format,
	X_D3DPOOL           Pool,
    X_D3DTexture      **ppTexture
);
// ******************************************************************
// * patch: D3DDevice_CreateVolumeTexture
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_CreateVolumeTexture)
(
    uint_xt              Width,
    uint_xt              Height,
    uint_xt              Depth,
    uint_xt              Levels,
    dword_xt             Usage,
    X_D3DFORMAT          Format,
	X_D3DPOOL            Pool,
    X_D3DVolumeTexture **ppVolumeTexture
);
// ******************************************************************
// * patch: D3DDevice_CreateCubeTexture
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_CreateCubeTexture)
(
    uint_xt             EdgeLength,
    uint_xt             Levels,
    dword_xt            Usage,
    X_D3DFORMAT         Format,
	X_D3DPOOL           Pool,
    X_D3DCubeTexture  **ppCubeTexture
);
// ******************************************************************
// * patch: D3DDevice_CreateIndexBuffer
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_CreateIndexBuffer)
(
    uint_xt              Length,
    dword_xt             Usage,
    X_D3DFORMAT          Format,
	X_D3DPOOL            Pool,
    X_D3DIndexBuffer   **ppIndexBuffer
);
// ******************************************************************
// * patch: D3DDevice_CreateIndexBuffer2
// ******************************************************************
X_D3DIndexBuffer * WINAPI EMUPATCH(D3DDevice_CreateIndexBuffer2)(uint_xt Length);
// ******************************************************************
// * patch: D3DDevice_SetIndices_4__LTCG_ebx1
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetIndices_4__LTCG_ebx1)
(
    uint_xt                BaseVertexIndex
);
// ******************************************************************
// * patch: D3DDevice_GetDisplayMode
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetDisplayMode)
(
    X_D3DDISPLAYMODE         *pModes
);
// ******************************************************************
// * patch: D3DDevice_Clear
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_Clear)
(
    dword_xt                  Count,
    CONST X_D3DRECT          *pRects,
    dword_xt                  Flags,
    X_D3DCOLOR                Color,
    float                     Z,
    dword_xt                  Stencil
);
// ******************************************************************
// * patch: IDirect3DResource8_Register
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DResource_Register)
(
    X_D3DResource      *pThis,
    PVOID               pBase
);
// ******************************************************************
// * patch: IDirect3DResource8_Release
// ******************************************************************
xbox::ulong_xt WINAPI EMUPATCH(D3DResource_Release)
(
    X_D3DResource      *pThis
);
// ******************************************************************
// * patch: IDirect3DResource8_GetType
// ******************************************************************
X_D3DRESOURCETYPE WINAPI EMUPATCH(D3DResource_GetType)
(
    X_D3DResource      *pThis
);
#endif
// ******************************************************************
// * patch: IDirect3DResource8_IsBusy
// ******************************************************************
xbox::bool_xt WINAPI EMUPATCH(D3DResource_IsBusy)
(
    X_D3DResource      *pThis
);
// ******************************************************************
// * patch: Get2DSurfaceDesc
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(Get2DSurfaceDesc)
(
    X_D3DPixelContainer *pPixelContainer,
    dword_xt                dwLevel,
    X_D3DSURFACE_DESC   *pDesc
);
// ******************************************************************
// * patch: IDirect3DSurface8_GetDesc
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DSurface_GetDesc)
(
    X_D3DResource      *pThis,
    X_D3DSURFACE_DESC  *pDesc
);
// ******************************************************************
// * patch: IDirect3DSurface8_LockRect
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DSurface_LockRect)
(
    X_D3DResource    *pThis,
    X_D3DLOCKED_RECT *pLockedRect,
    CONST X_RECT     *pRect,
    dword_xt          Flags
);
// ******************************************************************
// * patch: IDirect3DBaseTexture8_GetLevelCount
// ******************************************************************
xbox::dword_xt WINAPI EMUPATCH(D3DBaseTexture_GetLevelCount)
(
    X_D3DBaseTexture   *pThis
);
// ******************************************************************
// * patch: IDirect3DTexture8_GetSurfaceLevel2
// ******************************************************************
X_D3DSurface * WINAPI EMUPATCH(D3DTexture_GetSurfaceLevel2)
(
    X_D3DTexture   *pThis,
    uint_xt            Level
);
// ******************************************************************
// * patch: IDirect3DTexture8_LockRect
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DTexture_LockRect)
(
    X_D3DTexture     *pThis,
    uint_xt           Level,
    X_D3DLOCKED_RECT *pLockedRect,
    CONST X_RECT     *pRect,
    dword_xt          Flags
);
// ******************************************************************
// * patch: IDirect3DTexture8_GetSurfaceLevel
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DTexture_GetSurfaceLevel)
(
    X_D3DTexture   *pThis,
    uint_xt            Level,
    X_D3DSurface  **ppSurfaceLevel
);
// ******************************************************************
// * patch: IDirect3DVolumeTexture8_LockBox
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DVolumeTexture_LockBox)
(
    X_D3DVolumeTexture *pThis,
    uint_xt                Level,
    X_D3DLOCKED_BOX      *pLockedVolume,
    CONST X_D3DBOX       *pBox,
    dword_xt               Flags
);
// ******************************************************************
// * patch: IDirect3DCubeTexture8_LockRect
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DCubeTexture_LockRect)
(
    X_D3DCubeTexture   *pThis,
    X_D3DCUBEMAP_FACES  FaceType,
    uint_xt             Level,
    X_D3DLOCKED_RECT   *pLockedBox,
    CONST X_RECT       *pRect,
    dword_xt            Flags
);
// ******************************************************************
// * patch: D3DDevice_CreateVertexBuffer
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_CreateVertexBuffer)
(
    uint_xt             Length,
    dword_xt            Usage,
    dword_xt            FVF,
	X_D3DPOOL           Pool,
    X_D3DVertexBuffer **ppVertexBuffer
);
#endif
// ******************************************************************
// * patch: D3DDevice_CreateVertexBuffer2
// ******************************************************************
X_D3DVertexBuffer* WINAPI EMUPATCH(D3DDevice_CreateVertexBuffer2)
(
    uint_xt Length
);
#endif
// ******************************************************************
// * patch: D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_BlockUntilVerticalBlank)();
// ******************************************************************
// * patch: D3DDevice_SetVerticalBlankCallback
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetVerticalBlankCallback)
(
    X_D3DVBLANKCALLBACK pCallback
);
// ******************************************************************
// * patch: D3DDevice_SetTextureState_TexCoordIndex
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetTextureState_TexCoordIndex)
(
    dword_xt Stage,
    dword_xt Value
);
xbox::void_xt __stdcall EMUPATCH(D3DDevice_SetTextureState_TexCoordIndex_0)();
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetTextureState_TexCoordIndex_4)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_TwoSidedLighting
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_TwoSidedLighting)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_BackFillMode
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_BackFillMode)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetTextureState_BorderColor
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetTextureState_BorderColor)
(
    dword_xt Stage,
    dword_xt Value
);
xbox::void_xt EMUPATCH(D3DDevice_SetTextureState_BorderColor_0)();
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetTextureState_BorderColor_4)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetTextureState_ColorKeyColor
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetTextureState_ColorKeyColor)
(
    dword_xt Stage,
    dword_xt Value
);
xbox::void_xt EMUPATCH(D3DDevice_SetTextureState_ColorKeyColor_0)();
xbox::void_xt __stdcall EMUPATCH(D3DDevice_SetTextureState_ColorKeyColor_4)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetTextureState_BumpEnv
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetTextureState_BumpEnv)
(
    dword_xt                      Stage,
    X_D3DTEXTURESTAGESTATETYPE Type,
    dword_xt                      Value
);
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetTextureState_BumpEnv_8)
(
    X_D3DTEXTURESTAGESTATETYPE Type,
    dword_xt                      Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_FrontFace
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_FrontFace)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_LogicOp
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_LogicOp)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_NormalizeNormals
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_NormalizeNormals)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_TextureFactor
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_TextureFactor)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_ZBias
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_ZBias)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_EdgeAntiAlias
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_EdgeAntiAlias)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_FillMode
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_FillMode)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_FogColor
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_FogColor)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_Dxt1NoiseEnable)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_VertexBlend
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_VertexBlend)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_PSTextureModes
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_PSTextureModes)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_CullMode
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_CullMode)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_LineWidth
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_LineWidth)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_StencilFail
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_StencilFail)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_OcclusionCullEnable
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_OcclusionCullEnable)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_StencilCullEnable
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_StencilCullEnable)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_RopZCmpAlwaysRead
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_RopZCmpAlwaysRead)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_RopZRead
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_RopZRead)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_DoNotCullUncompressed
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_DoNotCullUncompressed)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_ZEnable
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_ZEnable)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_StencilEnable
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_StencilEnable)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_MultiSampleMask
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_MultiSampleMask)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_MultiSampleMode
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_MultiSampleMode)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_MultiSampleRenderTargetMode
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_MultiSampleRenderTargetMode)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_MultiSampleAntiAlias)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_ShadowFunc
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_ShadowFunc)
(
    dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_GetTransform
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetTransform)
(
    X_D3DTRANSFORMSTATETYPE State,
    X_D3DMATRIX             *pMatrix
);
// ******************************************************************
// * patch: IDirect3DVertexBuffer8_Lock
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DVertexBuffer_Lock)
(
    X_D3DVertexBuffer   *pVertexBuffer,
    uint_xt                OffsetToLock,
    uint_xt                SizeToLock,
    byte_xt              **ppbData,
    dword_xt               Flags
);
// ******************************************************************
// * patch: IDirect3DVertexBuffer8_Lock2
// ******************************************************************
byte_xt* WINAPI EMUPATCH(D3DVertexBuffer_Lock2)
(
    X_D3DVertexBuffer  *pVertexBuffer,
    dword_xt               Flags
);
// ******************************************************************
// * patch: D3DDevice_GetStreamSource2
// ******************************************************************
xbox::X_D3DVertexBuffer* WINAPI EMUPATCH(D3DDevice_GetStreamSource2)
(
    uint_xt  StreamNumber,
    uint_xt *pStride
);
// ******************************************************************
// * patch: D3DDevice_GetLight
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetLight)
(
    dword_xt            Index,
    X_D3DLIGHT8       *pLight
);
// ******************************************************************
// * patch: D3DDevice_Release
// ******************************************************************
xbox::ulong_xt WINAPI EMUPATCH(D3DDevice_Release)();
// ******************************************************************
// * patch: D3DDevice_CreatePalette
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_CreatePalette)
(
    X_D3DPALETTESIZE    Size,
    X_D3DPalette      **ppPalette
);
// ******************************************************************
// * patch: D3DDevice_CreatePalette2
// ******************************************************************
X_D3DPalette * WINAPI EMUPATCH(D3DDevice_CreatePalette2)
(
    X_D3DPALETTESIZE    Size
);
// ******************************************************************
// * patch: D3DDevice_SetFlickerFilter
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetFlickerFilter)
(
    dword_xt         Filter
);
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetFlickerFilter_0__LTCG_esi1)();
// ******************************************************************
// * patch: D3DDevice_SetSoftDisplayFilter
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetSoftDisplayFilter)
(
    bool_xt Enable
);
// ******************************************************************
// * patch: IDirect3DPalette8_Lock
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DPalette_Lock)
(
    X_D3DPalette      *pThis,
    X_D3DCOLOR       **ppColors,
    dword_xt           Flags
);
// ******************************************************************
// * patch: IDirect3DPalette8_Lock2
// ******************************************************************
xbox::X_D3DCOLOR * WINAPI EMUPATCH(D3DPalette_Lock2)
(
    X_D3DPalette   *pThis,
    dword_xt           Flags
);
// ******************************************************************
// * patch: D3DDevice_GetVertexShaderSize
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetVertexShaderSize)
(
    dword_xt Handle,
    uint_xt* pSize
);
// ******************************************************************
// * patch: D3DDevice_GetVertexShaderInput
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_GetVertexShaderInput)
(
    dword_xt              *pHandle,
    uint_xt               *pStreamCount,
    X_STREAMINPUT      *pStreamInputs
);
// ******************************************************************
// * patch: D3DDevice_GetVertexShaderType
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetVertexShaderType)
(
    dword_xt  Handle,
    dword_xt *pType
);
// ******************************************************************
// * patch: D3DDevice_GetVertexShaderDeclaration
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_GetVertexShaderDeclaration)
(
    dword_xt  Handle,
    PVOID  pData,
    dword_xt *pSizeOfData
);
// ******************************************************************
// * patch: D3DDevice_GetVertexShaderFunction
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_GetVertexShaderFunction)
(
    dword_xt  Handle,
    PVOID *pData,
    dword_xt *pSizeOfData
);
// ******************************************************************
// * patch: D3D_AllocContiguousMemory
// ******************************************************************
PVOID WINAPI EMUPATCH(D3D_AllocContiguousMemory)
(
    size_xt dwSize,
    dword_xt dwAllocAttributes
);
#endif
// ******************************************************************
// * patch: D3DTexture_GetLevelDesc
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DTexture_GetLevelDesc)
(
    uint_xt Level,
    X_D3DSURFACE_DESC* pDesc
);
#endif
// ******************************************************************
// * patch: Direct3D_CheckDeviceMultiSampleType
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(Direct3D_CheckDeviceMultiSampleType)
(
    uint_xt              Adapter,
	X_D3DDEVTYPE         DeviceType,
    X_D3DFORMAT          SurfaceFormat,
    bool_xt              Windowed,
    D3DMULTISAMPLE_TYPE  MultiSampleType
);
#endif
// ******************************************************************
// * patch: D3D_GetDeviceCaps
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3D_GetDeviceCaps)
(
    uint_xt       Adapter,
	X_D3DDEVTYPE  DeviceType,
    X_D3DCAPS    *pCaps
);
#endif
// ******************************************************************
// * patch: D3D_SetPushBufferSize
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3D_SetPushBufferSize)
(
    dword_xt PushBufferSize,
    dword_xt KickOffSize
);
#endif
// ******************************************************************
// * patch: D3DVertexBuffer_GetDesc
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DVertexBuffer_GetDesc)
(
    X_D3DVertexBuffer    *pThis,
    X_D3DVERTEXBUFFER_DESC *pDesc
);
#endif
// ******************************************************************
// * patch: D3DDevice_SetScissors
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetScissors)
(
    dword_xt          Count,
    bool_xt           Exclusive,
    CONST X_D3DRECT  *pRects
);
#endif
// ******************************************************************
// * patch: D3DDevice_SetPixelShaderProgram
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetPixelShaderProgram)
(
	X_D3DPIXELSHADERDEF *pPSDef
);
// ******************************************************************
// * patch: D3DDevice_CreateStateBlock
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_CreateStateBlock)
(
	X_D3DSTATEBLOCKTYPE Type,
	dword_xt			  *pToken
);
#endif
// ******************************************************************
// * patch: D3DDevice_KickOff (D3D::CDevice::KickOff)
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_KickOff)();
// ******************************************************************
// * patch: D3DDevice_KickPushBuffer
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_KickPushBuffer)();
// ******************************************************************
// * patch: D3DDevice_GetTexture2
// ******************************************************************
X_D3DBaseTexture* WINAPI EMUPATCH(D3DDevice_GetTexture2)(dword_xt Stage);
// ******************************************************************
// * patch: D3DDevice_GetTexture
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetTexture)
(
	dword_xt           Stage,
	X_D3DBaseTexture  **pTexture
);
// ******************************************************************
// * patch: D3DDevice_GetPersistedSurface
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetPersistedSurface)(X_D3DSurface **ppSurface);
X_D3DSurface* WINAPI EMUPATCH(D3DDevice_GetPersistedSurface2)();
// ******************************************************************
// * patch: D3D_CMiniport_GetDisplayCapabilities
// ******************************************************************
xbox::dword_xt WINAPI EMUPATCH(D3D_CMiniport_GetDisplayCapabilities)();
// ******************************************************************
// * patch: D3DDevice_SetRenderState_SampleAlpha
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_SetRenderState_SampleAlpha)
(
	dword_xt dwSampleAlpha
);
// ******************************************************************
// * patch: D3DDevice_SetRenderState_Deferred
// ******************************************************************
xbox::void_xt __fastcall EMUPATCH(D3DDevice_SetRenderState_Deferred)
(
	dword_xt State,
	dword_xt Value
);
// ******************************************************************
// * patch: D3DDevice_DeleteStateBlock
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_DeleteStateBlock)
(
	dword_xt Token
);
// ******************************************************************
// * patch: D3DDevice_BeginPushBuffer
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_BeginPushBuffer)
(
	X_D3DPushBuffer *pPushBuffer
);
// ******************************************************************
// * patch: D3DDevice_EndPushBuffer
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_EndPushBuffer)();
// ******************************************************************
// * patch: XMETAL_StartPush
// ******************************************************************
PDWORD WINAPI EMUPATCH(XMETAL_StartPush)(void* Unknown);
// ******************************************************************
// * patch: D3D_GetAdapterIdentifier
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3D_GetAdapterIdentifier)
(
	uint_xt					Adapter,
	dword_xt					Flags,
	X_D3DADAPTER_IDENTIFIER *pIdentifier
);
#endif
// ******************************************************************
// * patch: D3D::MakeRequestedSpace
// ******************************************************************
PDWORD WINAPI EMUPATCH(D3D_MakeRequestedSpace)
(
	dword_xt MinimumSpace,
	dword_xt RequestedSpace
);
// ******************************************************************
// * patch: D3DDevice_MakeSpace
// ******************************************************************
void WINAPI EMUPATCH(D3DDevice_MakeSpace)();
// ******************************************************************
// * patch: D3D_BlockOnResource
// ******************************************************************
void WINAPI EMUPATCH(D3D_BlockOnResource)( X_D3DResource* pResource );
// ******************************************************************
// * patch: D3DDevice_GetPushBufferOffset
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetPushBufferOffset)
(
	dword_xt *pOffset
);
// ******************************************************************
// * patch: IDirect3DCubeTexture8_GetCubeMapSurface
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DCubeTexture_GetCubeMapSurface)
(
	X_D3DCubeTexture*	pThis,
	X_D3DCUBEMAP_FACES	FaceType,
	uint_xt				Level,
	X_D3DSurface**		ppCubeMapSurface
);
// ******************************************************************
// * patch: IDirect3DCubeTexture8_GetCubeMapSurface2
// ******************************************************************
X_D3DSurface* WINAPI EMUPATCH(D3DCubeTexture_GetCubeMapSurface2)
(
	X_D3DCubeTexture*	pThis,
	X_D3DCUBEMAP_FACES	FaceType,
	uint_xt				Level
);
// ******************************************************************
// * patch: D3DDevice_GetPixelShader
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetPixelShader)
(
	dword_xt  Name,
	dword_xt* pHandle
);
// ******************************************************************
// * patch: D3DDevice_GetScissors
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetScissors)
(
	dword_xt	*pCount, 
	bool_xt	  *pExclusive, 
	X_D3DRECT *pRects
);
// ******************************************************************
// * patch: D3DDevice_GetBackMaterial
// ******************************************************************
// * patch: D3DDevice_GetBackMaterial
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetBackMaterial)
(
	X_D3DMATERIAL8* pMaterial
);

} // namespace xbox

#endif // EMUPATCHES_UNUSED_H
