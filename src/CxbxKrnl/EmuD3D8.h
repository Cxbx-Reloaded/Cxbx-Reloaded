// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuD3D8.h
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
#ifndef EMUD3D8_H
#define EMUD3D8_H

#include "EmuD3D8Types.h"

#include "Common/Xbe.h"
#include "Emu.h"

#define DIRECTDRAW_VERSION 0x0700
#include <ddraw.h>

// initialize direct3d
extern VOID EmuD3DInit(Xbe::Header *XbeHeader, uint32 XbeHeaderSize);

// cleanup direct3d
extern VOID EmuD3DCleanup();

// EmuD3DTileCache (8 tiles maximum)
extern X_D3DTILE EmuD3DTileCache[0x08];

// EmuD3DActiveTexture
extern X_D3DResource *EmuD3DActiveTexture[4];

// ******************************************************************
// * func: EmuIDirect3D8_CreateDevice
// ******************************************************************
HRESULT WINAPI EmuIDirect3D8_CreateDevice
(
    UINT                        Adapter,
    D3DDEVTYPE                  DeviceType,
    HWND                        hFocusWindow,
    DWORD                       BehaviorFlags,
    X_D3DPRESENT_PARAMETERS    *pPresentationParameters,
    IDirect3DDevice8          **ppReturnedDeviceInterface
);

// ******************************************************************
// * func: EmuIDirect3DResource8_IsBusy
// ******************************************************************
BOOL WINAPI EmuIDirect3DDevice8_IsBusy();

// ******************************************************************
// * func: EmuIDirect3D8_CheckDeviceFormat
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters);

// ******************************************************************
// * func: EmuIDirect3D8_CheckDeviceFormat
// ******************************************************************
HRESULT WINAPI EmuIDirect3D8_CheckDeviceFormat
(
    UINT                        Adapter,
    D3DDEVTYPE                  DeviceType,
    D3DFORMAT                   AdapterFormat,
    DWORD                       Usage,
    X_D3DRESOURCETYPE           RType,
    X_D3DFORMAT                 CheckFormat
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetDeviceCaps
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_GetDeviceCaps
(
    D3DCAPS8                   *pCaps
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetDisplayFieldStatus
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_GetDisplayFieldStatus(X_D3DFIELD_STATUS *pFieldStatus);

// ******************************************************************
// * func: EmuIDirect3DDevice8_BeginPush
// ******************************************************************
PDWORD WINAPI EmuIDirect3DDevice8_BeginPush(DWORD Count);

// ******************************************************************
// * func: EmuIDirect3DDevice8_EndPush
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_EndPush(DWORD *pPush);

// ******************************************************************
// * func: EmuIDirect3DDevice8_BeginVisibilityTest
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_BeginVisibilityTest();

// ******************************************************************
// * func: EmuIDirect3DDevice8_EndVisibilityTest
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_EndVisibilityTest
(
    DWORD                       Index
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVisibilityTestResult
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_GetVisibilityTestResult
(
    DWORD                       Index,
    UINT                       *pResult,
    ULONGLONG                  *pTimeStamp
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetBackBufferScale
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetBackBufferScale(FLOAT x, FLOAT y);

// ******************************************************************
// * func: EmuIDirect3DDevice8_LoadVertexShader
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_LoadVertexShader
(
    DWORD                       Handle,
    DWORD                       Address
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SelectVertexShader
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SelectVertexShader
(
    DWORD                       Handle,
    DWORD                       Address
);

// ******************************************************************
// * func: EmuIDirect3D8_KickOffAndWaitForIdle
// ******************************************************************
VOID WINAPI EmuIDirect3D8_KickOffAndWaitForIdle();

// ******************************************************************
// * func: EmuIDirect3D8_KickOffAndWaitForIdle
// ******************************************************************
VOID WINAPI EmuIDirect3D8_KickOffAndWaitForIdle2(DWORD dwDummy1, DWORD dwDummy2);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetGammaRamp
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetGammaRamp
(
    DWORD                   dwFlags,
    CONST X_D3DGAMMARAMP   *pRamp
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_AddRef
// ******************************************************************
ULONG WINAPI EmuIDirect3DDevice8_AddRef();

// ******************************************************************
// * func: EmuIDirect3DDevice8_BeginStateBlock
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_BeginStateBlock();

// ******************************************************************
// * func: EmuIDirect3DDevice8_CaptureStateBlock
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_CaptureStateBlock(DWORD Token);

// ******************************************************************
// * func: EmuIDirect3DDevice8_ApplyStateBlock
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_ApplyStateBlock(DWORD Token);

// ******************************************************************
// * func: EmuIDirect3DDevice8_EndStateBlock
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_EndStateBlock(DWORD *pToken);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CopyRects
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_CopyRects
(
    X_D3DSurface       *pSourceSurface,
    CONST RECT         *pSourceRectsArray,
    UINT                cRects,
    X_D3DSurface       *pDestinationSurface,
    CONST POINT        *pDestPointsArray
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateImageSurface
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_CreateImageSurface
(
    UINT                Width,
    UINT                Height,
    X_D3DFORMAT         Format,
    X_D3DSurface      **ppBackBuffer
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetGammaRamp
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_GetGammaRamp
(
    X_D3DGAMMARAMP     *pRamp
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetBackBuffer2
// ******************************************************************
X_D3DSurface* WINAPI EmuIDirect3DDevice8_GetBackBuffer2
(
    INT                 BackBuffer
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetBackBuffer
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_GetBackBuffer
(
    INT                 BackBuffer,
    D3DBACKBUFFER_TYPE  Type,
    X_D3DSurface      **ppBackBuffer
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetViewport
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetViewport
(
    CONST D3DVIEWPORT8 *pViewport
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetViewport
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_GetViewport
(
    D3DVIEWPORT8 *pViewport
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetViewportOffsetAndScale
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_GetViewportOffsetAndScale
(
    D3DXVECTOR4 *pOffset,
    D3DXVECTOR4 *pScale
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetShaderConstantMode
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetShaderConstantMode
(
    XTL::X_VERTEXSHADERCONSTANTMODE Mode
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_Reset
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_Reset
(
    X_D3DPRESENT_PARAMETERS *pPresentationParameters
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetRenderTarget
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_GetRenderTarget
(
    X_D3DSurface  **ppRenderTarget
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetRenderTarget
// ******************************************************************
X_D3DSurface * WINAPI EmuIDirect3DDevice8_GetRenderTarget2();

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetDepthStencilSurface
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_GetDepthStencilSurface
(
    X_D3DSurface  **ppZStencilSurface
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetDepthStencilSurface
// ******************************************************************
X_D3DSurface * WINAPI EmuIDirect3DDevice8_GetDepthStencilSurface2();

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetTile
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_GetTile
(
    DWORD           Index,
    X_D3DTILE      *pTile
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTileNoWait
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetTileNoWait
(
    DWORD               Index,
    CONST X_D3DTILE    *pTile
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateVertexShader
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_CreateVertexShader
(
    CONST DWORD    *pDeclaration,
    CONST DWORD    *pFunction,
    DWORD          *pHandle,
    DWORD           Usage
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetPixelShaderConstant
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetPixelShaderConstant
(
    DWORD       Register,
    CONST PVOID pConstantData,
    DWORD       ConstantCount
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexShaderConstant
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetVertexShaderConstant
(
    INT         Register,
    CONST PVOID pConstantData,
    DWORD       ConstantCount
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexShaderConstant1
// ******************************************************************
VOID __fastcall EmuIDirect3DDevice8_SetVertexShaderConstant1
(
    INT         Register,
    CONST PVOID pConstantData
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexShaderConstant4
// ******************************************************************
VOID __fastcall EmuIDirect3DDevice8_SetVertexShaderConstant4
(
    INT         Register,
    CONST PVOID pConstantData
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexShaderConstantNotInline
// ******************************************************************
VOID __fastcall EmuIDirect3DDevice8_SetVertexShaderConstantNotInline
(
    INT         Register,
    CONST PVOID pConstantData,
    DWORD       ConstantCount
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_DeletePixelShader
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_DeletePixelShader
(
    DWORD          Handle
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreatePixelShader
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_CreatePixelShader
(
    X_D3DPIXELSHADERDEF    *pPSDef,
    DWORD				   *pHandle
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetPixelShader
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetPixelShader
(
    DWORD           Handle
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateTexture2
// ******************************************************************
X_D3DResource * WINAPI EmuIDirect3DDevice8_CreateTexture2
(
    UINT                Width,
    UINT                Height,
    UINT                Depth,
    UINT                Levels,
    DWORD               Usage,
    D3DFORMAT           Format,
    D3DRESOURCETYPE     D3DResource
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateTexture
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_CreateTexture
(
    UINT                Width,
    UINT                Height,
    UINT                Levels,
    DWORD               Usage,
    D3DFORMAT           Format,
    D3DPOOL             Pool,
    X_D3DTexture      **ppTexture
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateVolumeTexture
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_CreateVolumeTexture
(
    UINT                 Width,
    UINT                 Height,
    UINT                 Depth,
    UINT                 Levels,
    DWORD                Usage,
    D3DFORMAT            Format,
    D3DPOOL              Pool,
    X_D3DVolumeTexture **ppVolumeTexture
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateCubeTexture
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_CreateCubeTexture
(
    UINT                 EdgeLength,
    UINT                 Levels,
    DWORD                Usage,
    D3DFORMAT            Format,
    D3DPOOL              Pool,
    X_D3DCubeTexture  **ppCubeTexture
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateIndexBuffer
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_CreateIndexBuffer
(
    UINT                 Length,
    DWORD                Usage,
    D3DFORMAT            Format,
    D3DPOOL              Pool,
    X_D3DIndexBuffer   **ppIndexBuffer
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateIndexBuffer2
// ******************************************************************
X_D3DIndexBuffer * WINAPI EmuIDirect3DDevice8_CreateIndexBuffer2(UINT Length);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetIndices
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetIndices
(
    X_D3DIndexBuffer   *pIndexData,
    UINT                BaseVertexIndex
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTexture
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetTexture
(
    DWORD           Stage,
    X_D3DResource  *pTexture
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SwitchTexture
// ******************************************************************
VOID __fastcall EmuIDirect3DDevice8_SwitchTexture
(
    DWORD           Method,
    DWORD           Data,
    DWORD           Format
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetDisplayMode
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_GetDisplayMode
(
    X_D3DDISPLAYMODE         *pMode
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_Begin
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_Begin
(
    X_D3DPRIMITIVETYPE     PrimitiveType
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexData2f
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetVertexData2f
(
    int     Register,
    FLOAT   a,
    FLOAT   b
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexData2s
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetVertexData2s
(
    int     Register,
    SHORT   a,
    SHORT   b
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexData4f
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetVertexData4f
(
    int     Register,
    FLOAT   a,
    FLOAT   b,
    FLOAT   c,
    FLOAT   d
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexData4ub
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetVertexData4ub
(
	INT		Register,
	BYTE	a,
	BYTE	b,
	BYTE	c,
	BYTE	d
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexData4s
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetVertexData4s
(
	INT		Register,
	SHORT	a,
	SHORT	b,
	SHORT	c,
	SHORT	d
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexDataColor
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetVertexDataColor
(
    int         Register,
    D3DCOLOR    Color
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_End
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_End();

// ******************************************************************
// * func: EmuIDirect3DDevice8_RunPushBuffer
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_RunPushBuffer
(
    X_D3DPushBuffer       *pPushBuffer,
    X_D3DFixup            *pFixup
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_Clear
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_Clear
(
    DWORD                  Count,
    CONST D3DRECT         *pRects,
    DWORD                  Flags,
    D3DCOLOR               Color,
    float                  Z,
    DWORD                  Stencil
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_Present
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_Present
(
    CONST RECT* pSourceRect,
    CONST RECT* pDestRect,
    PVOID       pDummy1,
    PVOID       pDummy2
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_Swap
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_Swap
(
    DWORD Flags
);

// ******************************************************************
// * func: EmuIDirect3DResource8_Register
// ******************************************************************
HRESULT WINAPI EmuIDirect3DResource8_Register
(
    X_D3DResource      *pThis,
    PVOID               pBase
);

// ******************************************************************
// * func: EmuIDirect3DResource8_Release
// ******************************************************************
ULONG WINAPI EmuIDirect3DResource8_Release
(
    X_D3DResource      *pThis
);

// ******************************************************************
// * func: EmuIDirect3DResource8_GetType
// ******************************************************************
X_D3DRESOURCETYPE WINAPI EmuIDirect3DResource8_GetType
(
    X_D3DResource      *pThis
);

// ******************************************************************
// * func: EmuIDirect3DResource8_AddRef
// ******************************************************************
ULONG WINAPI EmuIDirect3DResource8_AddRef
(
    X_D3DResource      *pThis
);

// ******************************************************************
// * func: EmuIDirect3DResource8_IsBusy
// ******************************************************************
BOOL WINAPI EmuIDirect3DResource8_IsBusy
(
    X_D3DResource      *pThis
);

// ******************************************************************
// * func: EmuLock2DSurface
// ******************************************************************
VOID WINAPI EmuLock2DSurface
(
    X_D3DPixelContainer *pPixelContainer,
    D3DCUBEMAP_FACES     FaceType,
    UINT                 Level,
    D3DLOCKED_RECT      *pLockedRect,
    RECT                *pRect,
    DWORD                Flags
);

// ******************************************************************
// * func: EmuLock3DSurface
// ******************************************************************
VOID WINAPI EmuLock3DSurface
(
    X_D3DPixelContainer *pPixelContainer,
    UINT				Level,
	D3DLOCKED_BOX		*pLockedVolume,
	D3DBOX				*pBox,
	DWORD				Flags
);

// ******************************************************************
// * func: EmuGet2DSurfaceDesc
// ******************************************************************
VOID WINAPI EmuGet2DSurfaceDesc
(
    X_D3DPixelContainer *pPixelContainer,
    DWORD                dwLevel,
    X_D3DSURFACE_DESC   *pDesc
);

// ******************************************************************
// * func: EmuGet2DSurfaceDescD
// ******************************************************************
VOID WINAPI EmuGet2DSurfaceDescD
(
    X_D3DPixelContainer *pPixelContainer,
    X_D3DSURFACE_DESC   *pDesc
);

// ******************************************************************
// * func: EmuIDirect3DSurface8_GetDesc
// ******************************************************************
HRESULT WINAPI EmuIDirect3DSurface8_GetDesc
(
    X_D3DResource      *pThis,
    X_D3DSURFACE_DESC  *pDesc
);

// ******************************************************************
// * func: EmuIDirect3DSurface8_LockRect
// ******************************************************************
HRESULT WINAPI EmuIDirect3DSurface8_LockRect
(
    X_D3DResource  *pThis,
    D3DLOCKED_RECT *pLockedRect,
    CONST RECT     *pRect,
    DWORD           Flags
);

// ******************************************************************
// * func: EmuIDirect3DBaseTexture8_GetLevelCount
// ******************************************************************
DWORD WINAPI EmuIDirect3DBaseTexture8_GetLevelCount
(
    X_D3DBaseTexture   *pThis
);

// ******************************************************************
// * func: EmuIDirect3DTexture8_GetSurfaceLevel
// ******************************************************************
X_D3DResource * WINAPI EmuIDirect3DTexture8_GetSurfaceLevel2
(
    X_D3DTexture   *pThis,
    UINT            Level
);

// ******************************************************************
// * func: EmuIDirect3DTexture8_LockRect
// ******************************************************************
HRESULT WINAPI EmuIDirect3DTexture8_LockRect
(
    X_D3DTexture   *pThis,
    UINT            Level,
    D3DLOCKED_RECT *pLockedRect,
    CONST RECT     *pRect,
    DWORD           Flags
);

// ******************************************************************
// * func: EmuIDirect3DTexture8_GetSurfaceLevel
// ******************************************************************
HRESULT WINAPI EmuIDirect3DTexture8_GetSurfaceLevel
(
    X_D3DTexture   *pThis,
    UINT            Level,
    X_D3DSurface  **ppSurfaceLevel
);

// ******************************************************************
// * func: EmuIDirect3DVolumeTexture8_LockBox
// ******************************************************************
HRESULT WINAPI EmuIDirect3DVolumeTexture8_LockBox
(
    X_D3DVolumeTexture *pThis,
    UINT                Level,
    D3DLOCKED_BOX      *pLockedVolume,
    CONST D3DBOX       *pBox,
    DWORD               Flags
);

// ******************************************************************
// * func: EmuIDirect3DCubeTexture8_LockRect
// ******************************************************************
HRESULT WINAPI EmuIDirect3DCubeTexture8_LockRect
(
    X_D3DCubeTexture   *pThis,
    D3DCUBEMAP_FACES    FaceType,
    UINT                Level,
    D3DLOCKED_RECT     *pLockedBox,
    CONST RECT         *pRect,
    DWORD               Flags
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateVertexBuffer
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_CreateVertexBuffer
(
    UINT                Length,
    DWORD               Usage,
    DWORD               FVF,
    D3DPOOL             Pool,
    X_D3DVertexBuffer **ppVertexBuffer
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateVertexBuffer2
// ******************************************************************
X_D3DVertexBuffer* WINAPI EmuIDirect3DDevice8_CreateVertexBuffer2
(
    UINT Length
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_EnableOverlay
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_EnableOverlay
(
    BOOL Enable
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_UpdateOverlay
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_UpdateOverlay
(
    X_D3DSurface *pSurface,
    CONST RECT   *SrcRect,
    CONST RECT   *DstRect,
    BOOL          EnableColorKey,
    D3DCOLOR      ColorKey
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetOverlayUpdateStatus
// ******************************************************************
BOOL WINAPI EmuIDirect3DDevice8_GetOverlayUpdateStatus();

// ******************************************************************
// * func: EmuIDirect3DDevice8_BlockUntilVerticalBlank
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_BlockUntilVerticalBlank();

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVerticalBlankCallback
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetVerticalBlankCallback
(
    D3DVBLANKCALLBACK pCallback
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTextureState_TexCoordIndex
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetTextureState_TexCoordIndex
(
    DWORD Stage,
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTextureState_TwoSidedLighting
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetTextureState_TwoSidedLighting
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_BackFillMode
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_BackFillMode
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTextureState_BorderColor
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetTextureState_BorderColor
(
    DWORD Stage,
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTextureState_ColorKeyColor
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetTextureState_ColorKeyColor
(
    DWORD Stage,
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTextureState_BumpEnv
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetTextureState_BumpEnv
(
    DWORD                      Stage,
    X_D3DTEXTURESTAGESTATETYPE Type,
    DWORD                      Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_FrontFace
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_FrontFace
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_LogicOp
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_LogicOp
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_NormalizeNormals
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_NormalizeNormals
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_TextureFactor
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_TextureFactor
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_ZBias
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_ZBias
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_FillMode
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_FillMode
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_FogColor
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_FogColor
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_Simple
// ******************************************************************
VOID __fastcall EmuIDirect3DDevice8_SetRenderState_Simple
(
    DWORD Method,
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_VertexBlend
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_VertexBlend
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_PSTextureModes
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_PSTextureModes
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_CullMode
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_CullMode
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_LineWidth
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_LineWidth
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_StencilFail
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_StencilFail
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_OcclusionCullEnable
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_OcclusionCullEnable
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_StencilCullEnable
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_StencilCullEnable
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_RopZRead
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_RopZRead
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_DoNotCullUncompressed
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_DoNotCullUncompressed
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_ZEnable
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_ZEnable
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_StencilEnable
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_StencilEnable
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_MultiSampleMask
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_MultiSampleMask
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_MultiSampleMode
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_MultiSampleMode
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_ShadowFunc
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_ShadowFunc
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_YuvEnable
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_YuvEnable
(
    BOOL Enable
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTransform
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetTransform
(
    D3DTRANSFORMSTATETYPE State,
    CONST D3DMATRIX      *pMatrix
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetTransform
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_GetTransform
(
    D3DTRANSFORMSTATETYPE State,
    D3DMATRIX            *pMatrix
);

// ******************************************************************
// * func: EmuIDirect3DVertexBuffer8_Lock
// ******************************************************************
VOID WINAPI EmuIDirect3DVertexBuffer8_Lock
(
    X_D3DVertexBuffer   *ppVertexBuffer,
    UINT                OffsetToLock,
    UINT                SizeToLock,
    BYTE              **ppbData,
    DWORD               Flags
);

// ******************************************************************
// * func: EmuIDirect3DVertexBuffer8_Lock2
// ******************************************************************
BYTE* WINAPI EmuIDirect3DVertexBuffer8_Lock2
(
    X_D3DVertexBuffer  *ppVertexBuffer,
    DWORD               Flags
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetStreamSource2
// ******************************************************************
XTL::X_D3DVertexBuffer* WINAPI EmuIDirect3DDevice8_GetStreamSource2
(
    UINT  StreamNumber,
    UINT *pStride
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetStreamSource
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetStreamSource
(
    UINT                StreamNumber,
    X_D3DVertexBuffer  *pStreamData,
    UINT                Stride
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexShader
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetVertexShader
(
    DWORD            Handle
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_DrawVertices
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_DrawVertices
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    UINT                StartVertex,
    UINT                VertexCount
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_DrawVerticesUP
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_DrawVerticesUP
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    UINT                VertexCount,
    CONST PVOID         pVertexStreamZeroData,
    UINT                VertexStreamZeroStride
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_DrawIndexedVertices
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_DrawIndexedVertices
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    UINT                VertexCount,
    CONST PWORD         pIndexData
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_DrawIndexedVerticesUP
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_DrawIndexedVerticesUP
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    UINT                VertexCount,
    CONST PVOID         pIndexData,
    CONST PVOID         pVertexStreamZeroData,
    UINT                VertexStreamZeroStride
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetLight
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetLight
(
    DWORD            Index,
    CONST D3DLIGHT8 *pLight
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetMaterial
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetMaterial
(
    CONST D3DMATERIAL8 *pMaterial
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_LightEnable
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_LightEnable
(
    DWORD            Index,
    BOOL             bEnable
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_Release
// ******************************************************************
ULONG WINAPI EmuIDirect3DDevice8_Release();

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreatePalette
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_CreatePalette
(
    X_D3DPALETTESIZE    Size,
    X_D3DPalette      **ppPalette
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreatePalette2
// ******************************************************************
X_D3DPalette * WINAPI EmuIDirect3DDevice8_CreatePalette2
(
    X_D3DPALETTESIZE    Size
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderTarget
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetRenderTarget
(
    X_D3DSurface    *pRenderTarget,
    X_D3DSurface    *pNewZStencil
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetPalette
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetPalette
(
    DWORD         Stage,
    X_D3DPalette *pPalette
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetFlickerFilter
// ******************************************************************
void WINAPI EmuIDirect3DDevice8_SetFlickerFilter
(
    DWORD         Filter
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetSoftDisplayFilter
// ******************************************************************
void WINAPI EmuIDirect3DDevice8_SetSoftDisplayFilter
(
    BOOL Enable
);

// ******************************************************************
// * func: EmuIDirect3DPalette8_Lock
// ******************************************************************
HRESULT WINAPI EmuIDirect3DPalette8_Lock
(
    X_D3DPalette   *pThis,
    D3DCOLOR      **ppColors,
    DWORD           Flags
);

// ******************************************************************
// * func: EmuIDirect3DPalette8_Lock2
// ******************************************************************
D3DCOLOR * WINAPI EmuIDirect3DPalette8_Lock2
(
    X_D3DPalette   *pThis,
    DWORD           Flags
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVertexShaderSize
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_GetVertexShaderSize
(
    DWORD Handle,
    UINT* pSize
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_DeleteVertexShader
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_DeleteVertexShader
(
    DWORD Handle
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SelectVertexShaderDirect
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SelectVertexShaderDirect
(
    X_VERTEXATTRIBUTEFORMAT *pVAF,
    DWORD                    Address
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetShaderConstantMode
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_GetShaderConstantMode
(
    DWORD *pMode
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVertexShader
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_GetVertexShader
(
    DWORD *pHandle
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVertexShaderConstant
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_GetVertexShaderConstant
(
    INT   Register,
    void  *pConstantData,
    DWORD ConstantCount
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexShaderInputDirect
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetVertexShaderInputDirect
(
    X_VERTEXATTRIBUTEFORMAT *pVAF,
    UINT                     StreamCount,
    X_STREAMINPUT           *pStreamInputs
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVertexShaderInput
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_GetVertexShaderInput
(
    DWORD              *pHandle,
    UINT               *pStreamCount,
    X_STREAMINPUT      *pStreamInputs
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVertexShaderInput
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetVertexShaderInput
(
    DWORD              Handle,
    UINT               StreamCount,
    X_STREAMINPUT     *pStreamInputs
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_RunVertexStateShader
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_RunVertexStateShader
(
    DWORD        Address,
    CONST FLOAT *pData
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_LoadVertexShaderProgram
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_LoadVertexShaderProgram
(
    CONST DWORD *pFunction,
    DWORD        Address
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVertexShaderType
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_GetVertexShaderType
(
    DWORD  Handle,
    DWORD *pType
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVertexShaderDeclaration
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_GetVertexShaderDeclaration
(
    DWORD  Handle,
    PVOID  pData,
    DWORD *pSizeOfData
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVertexShaderFunction
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_GetVertexShaderFunction
(
    DWORD  Handle,
    PVOID *pData,
    DWORD *pSizeOfData
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetDepthClipPlanes
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetDepthClipPlanes
(
    FLOAT Near,
    FLOAT Far,
    DWORD Flags
);

// ******************************************************************
// * func: EmuIDirect3D8_AllocContiguousMemory
// ******************************************************************
PVOID WINAPI EmuIDirect3D8_AllocContiguousMemory
(
    SIZE_T dwSize,
    DWORD dwAllocAttributes
);

// ******************************************************************
// * func: IDirect3DTexture8_GetLevelDesc
// ******************************************************************
HRESULT WINAPI EmuIDirect3DTexture8_GetLevelDesc
(
    UINT Level,
    X_D3DSURFACE_DESC* pDesc
);

// ******************************************************************
// * func: EmuIDirect3D8_CheckDeviceMultiSampleType
// ******************************************************************
HRESULT WINAPI EmuIDirect3D8_CheckDeviceMultiSampleType
(
    UINT                 Adapter,
    D3DDEVTYPE           DeviceType,
    D3DFORMAT            SurfaceFormat,
    BOOL                 Windowed,
    D3DMULTISAMPLE_TYPE  MultiSampleType
);

// ******************************************************************
// * func: EmuIDirect3D8_GetDeviceCaps
// ******************************************************************
HRESULT WINAPI EmuIDirect3D8_GetDeviceCaps
(
    UINT        Adapter,
    D3DDEVTYPE  DeviceType,
    D3DCAPS8    *pCaps
);

// ******************************************************************
// * func: EmuIDirect3D8_SetPushBufferSize
// ******************************************************************
HRESULT WINAPI EmuIDirect3D8_SetPushBufferSize
(
    DWORD PushBufferSize,
    DWORD KickOffSize
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_InsertFence
// ******************************************************************
DWORD WINAPI EmuIDirect3DDevice8_InsertFence();

// ******************************************************************
// * func: EmuIDirect3DDevice8_IsFencePending
// ******************************************************************
BOOL WINAPI EmuIDirect3DDevice8_IsFencePending
(
    DWORD Fence
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_BlockOnFence
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_BlockOnFence
(
    DWORD Fence
);

// ******************************************************************
// * func: EmuIDirect3DResource8_BlockUntilNotBusy
// ******************************************************************
VOID WINAPI EmuIDirect3DResource8_BlockUntilNotBusy
(
    X_D3DResource *pThis
);

// ******************************************************************
// * func: EmuIDirect3DVertexBuffer8_GetDesc
// ******************************************************************
VOID WINAPI EmuIDirect3DVertexBuffer8_GetDesc
(
    X_D3DVertexBuffer    *pThis,
    D3DVERTEXBUFFER_DESC *pDesc
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetScissors
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetScissors
(
    DWORD          Count,
    BOOL           Exclusive,
    CONST D3DRECT  *pRects
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetScreenSpaceOffset
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetScreenSpaceOffset
(
    FLOAT x,
    FLOAT y
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_InsertFence
// ******************************************************************
DWORD WINAPI EmuIDirect3DDevice8_InsertFence();

// ******************************************************************
// * func: EmuIDirect3DDevice8_BlockOnFence
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_BlockOnFence
(
	DWORD Fence
);

// ******************************************************************
// * func: EmuIDirect3DResource8_BlockUntilNotBusy
// ******************************************************************
VOID WINAPI EmuIDirect3DResource8_BlockUntilNotBusy
(
	X_D3DResource *pThis
);

// ******************************************************************
// * func: EmuIDirect3DVertexBuffer8_GetDesc
// ******************************************************************
VOID WINAPI EmuIDirect3DVertexBuffer8_GetDesc
(
	X_D3DVertexBuffer    *pThis,
	D3DVERTEXBUFFER_DESC *pDesc
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetScissors
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetScissors
(
	DWORD          Count, 
	BOOL           Exclusive, 
	CONST D3DRECT  *pRects
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetPixelShaderProgram
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetPixelShaderProgram
(
	X_D3DPIXELSHADERDEF *pPSDef
);

// ******************************************************************
// * func: EmuIDirect3DDevice_SetScreenSpaceOffset
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetScreenSpaceOffset
(
	FLOAT x,
	FLOAT y
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateStateBlock
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_CreateStateBlock
(
	D3DSTATEBLOCKTYPE Type,
	DWORD			  *pToken
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_InsertCallback
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_InsertCallback
(
	X_D3DCALLBACKTYPE	Type,
	X_D3DCALLBACK		pCallback,
	DWORD				Context
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_DrawRectPatch
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_DrawRectPatch
(
	UINT					Handle,
	CONST FLOAT				*pNumSegs,
	CONST D3DRECTPATCH_INFO *pRectPatchInfo
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetProjectionViewportMatrix
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_GetProjectionViewportMatrix
(
	D3DXMATRIX *pProjectionViewport
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_BackFillMode
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_BackFillMode
(
	DWORD Value
);

// ******************************************************************
// * func: EmuD3DDevice_KickOff (D3D::CDevice::KickOff)
// ******************************************************************
VOID WINAPI EmuD3DDevice_KickOff();

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetTexture2
// ******************************************************************
X_D3DResource* WINAPI EmuIDirect3DDevice8_GetTexture2(DWORD Stage);

// ******************************************************************
// * func: EmuD3DDevice_SetStateVB (D3D::CDevice::SetStateVB)
// ******************************************************************
VOID WINAPI EmuD3DDevice_SetStateVB( ULONG Unknown1 );

// ******************************************************************
// * func: EmuD3DDevice_SetStateUP (D3D::CDevice::SetStateUP)
// ******************************************************************
VOID WINAPI EmuD3DDevice_SetStateUP();

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetStipple
// ******************************************************************
void WINAPI EmuIDirect3DDevice8_SetStipple( DWORD* pPattern );

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetSwapCallback
// ******************************************************************
void WINAPI EmuIDirect3DDevice8_SetSwapCallback
(
	D3DSWAPCALLBACK		pCallback
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_PersistDisplay
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_PersistDisplay();

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetPersistedSurface
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_GetPersistedSurface(X_D3DSurface **ppSurface);
X_D3DSurface* WINAPI EmuIDirect3DDevice8_GetPersistedSurface2();

// ******************************************************************
// * func: EmuIDirect3DDevice8_Unknown1
// ******************************************************************
void WINAPI EmuIDirect3DDevice8_Unknown1();

// ******************************************************************
// * func: EmuIDirect3DDevice8_PrimeVertexCache
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_PrimeVertexCache
(
	UINT VertexCount,
	WORD *pIndexData
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_SampleAlpha
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetRenderState_SampleAlpha
(
	DWORD dwSampleAlpha
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_Deferred
// ******************************************************************
VOID __fastcall EmuIDirect3DDevice8_SetRenderState_Deferred
(
	DWORD State,
	DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_DeleteStateBlock
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_DeleteStateBlock
(
	DWORD Token
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetModelView
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetModelView
(
	CONST D3DMATRIX *pModelView, 
	CONST D3DMATRIX *pInverseModelView, 
	CONST D3DMATRIX *pComposite
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_FlushVertexCache
// ******************************************************************
void WINAPI EmuIDirect3DDevice8_FlushVertexCache();

// ******************************************************************
// * func: EmuIDirect3DDevice8_BeginPushBuffer
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_BeginPushBuffer
(
	X_D3DPushBuffer *pPushBuffer
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_EndPushBuffer
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_EndPushBuffer();

// ******************************************************************
// * func: EmuXMETAL_StartPush
// ******************************************************************
void WINAPI EmuXMETAL_StartPush(void* Unknown);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetModelView
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_GetModelView(D3DXMATRIX* pModelView);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetBackMaterial
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetBackMaterial(D3DMATERIAL8* pMaterial);

// ******************************************************************
// * func: EmuIDirect3D8_GetAdapterIdentifier
// ******************************************************************
HRESULT WINAPI EmuIDirect3D8_GetAdapterIdentifier
(
	UINT					Adapter,
	DWORD					Flags,
	D3DADAPTER_IDENTIFIER8* pIdentifier
);

// ******************************************************************
// * func: D3D::MakeRequestedSpace
// ******************************************************************
HRESULT WINAPI EmuD3D_MakeRequestedSpace( DWORD Unknown1, DWORD Unknown2 );

// ******************************************************************
// * func: D3DDevice_MakeSpace
// ******************************************************************
void WINAPI EmuD3DDevice_MakeSpace();

// ******************************************************************
// * func: D3D::SetCommonDebugRegisters
// ******************************************************************
void WINAPI EmuD3D_SetCommonDebugRegisters();

// ******************************************************************
// * func: D3D::BlockOnTime
// ******************************************************************
void WINAPI EmuD3D_BlockOnTime( DWORD Unknown1, int Unknown2 );

// ******************************************************************
// * func: D3D::BlockOnResource
// ******************************************************************
void WINAPI EmuD3D_BlockOnResource( X_D3DResource* pResource );

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetPushBufferOffset
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_GetPushBufferOffset
(
	DWORD *pOffset
);

// ******************************************************************
// * func: EmuIDirect3DCubeTexture8_GetCubeMapSurface
// ******************************************************************
HRESULT WINAPI EmuIDirect3DCubeTexture8_GetCubeMapSurface
(
	X_D3DCubeTexture*	pThis,
	D3DCUBEMAP_FACES	FaceType,
	UINT				Level,
	X_D3DSurface**		ppCubeMapSurface
);

// ******************************************************************
// * func: EmuIDirect3DCubeTexture8_GetCubeMapSurface2
// ******************************************************************
X_D3DSurface* WINAPI EmuIDirect3DCubeTexture8_GetCubeMapSurface2
(
	X_D3DCubeTexture*	pThis,
	D3DCUBEMAP_FACES	FaceType,
	UINT				Level
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetPixelShader
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_GetPixelShader
(
	DWORD  Name,
	DWORD* pHandle
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderTargetFast
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetRenderTargetFast
(
    X_D3DSurface	*pRenderTarget,
    X_D3DSurface	*pNewZStencil,
    DWORD			Flags
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetScissors
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_GetScissors
(
	DWORD	*pCount, 
	BOOL	*pExclusive, 
	D3DRECT *pRects
);
// ******************************************************************
// * func: EmuIDirect3DDevice8_GetBackMaterial
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_GetBackMaterial(D3DMATERIAL8* pMaterial);

// ******************************************************************
// * func: EmuD3D::LazySetPointParams
// ******************************************************************
void WINAPI EmuD3D_LazySetPointParams( void* Device );

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetMaterial
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_GetMaterial(D3DMATERIAL8* pMaterial);

#endif
