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
#include "CxbxKrnl.h"
#include "Common/Xbe.h"
#include "Emu.h"

#define DIRECTDRAW_VERSION 0x0700
#include <ddraw.h>

// initialize render window
extern VOID CxbxInitWindow(Xbe::Header *XbeHeader, uint32 XbeHeaderSize);

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
HRESULT WINAPI EMUPATCH(D3D_CreateDevice)
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
BOOL WINAPI EMUPATCH(D3DDevice_IsBusy)();

// ******************************************************************
// * func: EmuIDirect3D8_CheckDeviceFormat
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_GetCreationParameters)(D3DDEVICE_CREATION_PARAMETERS *pParameters);

// ******************************************************************
// * func: EmuIDirect3D8_CheckDeviceFormat
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3D_CheckDeviceFormat)
(
    UINT                        Adapter,
    D3DDEVTYPE                  DeviceType,
    X_D3DFORMAT                 AdapterFormat,
    DWORD                       Usage,
    X_D3DRESOURCETYPE           RType,
    X_D3DFORMAT                 CheckFormat
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetDeviceCaps
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_GetDeviceCaps)
(
    D3DCAPS8                   *pCaps
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetDisplayFieldStatus
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_GetDisplayFieldStatus)(X_D3DFIELD_STATUS *pFieldStatus);

// ******************************************************************
// * func: EmuIDirect3DDevice8_BeginPush
// ******************************************************************
PDWORD WINAPI EMUPATCH(D3DDevice_BeginPush)(DWORD Count);

// ******************************************************************
// * func: EmuIDirect3DDevice8_EndPush
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_EndPush)(DWORD *pPush);

// ******************************************************************
// * func: EmuIDirect3DDevice8_BeginVisibilityTest
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_BeginVisibilityTest)();

// ******************************************************************
// * func: EmuIDirect3DDevice8_EndVisibilityTest
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_EndVisibilityTest)
(
    DWORD                       Index
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVisibilityTestResult
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_GetVisibilityTestResult)
(
    DWORD                       Index,
    UINT                       *pResult,
    ULONGLONG                  *pTimeStamp
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetBackBufferScale
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetBackBufferScale)(FLOAT x, FLOAT y);

// ******************************************************************
// * func: EmuIDirect3DDevice8_LoadVertexShader
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_LoadVertexShader)
(
    DWORD                       Handle,
    DWORD                       Address
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SelectVertexShader
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SelectVertexShader)
(
    DWORD                       Handle,
    DWORD                       Address
);

// ******************************************************************
// * func: EmuIDirect3D8_KickOffAndWaitForIdle
// ******************************************************************
VOID WINAPI EMUPATCH(D3D_KickOffAndWaitForIdle)();

// ******************************************************************
// * func: EmuIDirect3D8_KickOffAndWaitForIdle
// ******************************************************************
VOID WINAPI EMUPATCH(D3D_KickOffAndWaitForIdle2)(DWORD dwDummy1, DWORD dwDummy2);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetGammaRamp
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetGammaRamp)
(
    DWORD                   dwFlags,
    CONST X_D3DGAMMARAMP   *pRamp
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_AddRef
// ******************************************************************
ULONG WINAPI EMUPATCH(D3DDevice_AddRef)();

// ******************************************************************
// * func: EmuIDirect3DDevice8_BeginStateBlock
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_BeginStateBlock)();

// ******************************************************************
// * func: EmuIDirect3DDevice8_CaptureStateBlock
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_CaptureStateBlock)(DWORD Token);

// ******************************************************************
// * func: EmuIDirect3DDevice8_ApplyStateBlock
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_ApplyStateBlock)(DWORD Token);

// ******************************************************************
// * func: EmuIDirect3DDevice8_EndStateBlock
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_EndStateBlock)(DWORD *pToken);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CopyRects
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_CopyRects)
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
HRESULT WINAPI EMUPATCH(D3DDevice_CreateImageSurface)
(
    UINT                Width,
    UINT                Height,
    X_D3DFORMAT         Format,
    X_D3DSurface      **ppBackBuffer
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetGammaRamp
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_GetGammaRamp)
(
    X_D3DGAMMARAMP     *pRamp
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetBackBuffer2
// ******************************************************************
X_D3DSurface* WINAPI EMUPATCH(D3DDevice_GetBackBuffer2)
(
    INT                 BackBuffer
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetBackBuffer
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_GetBackBuffer)
(
    INT                 BackBuffer,
    D3DBACKBUFFER_TYPE  Type,
    X_D3DSurface      **ppBackBuffer
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetViewport
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetViewport)
(
    CONST D3DVIEWPORT8 *pViewport
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetViewport
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_GetViewport)
(
    D3DVIEWPORT8 *pViewport
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetViewportOffsetAndScale
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_GetViewportOffsetAndScale)
(
    D3DXVECTOR4 *pOffset,
    D3DXVECTOR4 *pScale
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetShaderConstantMode
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetShaderConstantMode)
(
    XTL::X_VERTEXSHADERCONSTANTMODE Mode
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_Reset
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_Reset)
(
    X_D3DPRESENT_PARAMETERS *pPresentationParameters
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetRenderTarget
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_GetRenderTarget)
(
    X_D3DSurface  **ppRenderTarget
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetRenderTarget
// ******************************************************************
X_D3DSurface * WINAPI EMUPATCH(D3DDevice_GetRenderTarget2)();

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetDepthStencilSurface
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_GetDepthStencilSurface)
(
    X_D3DSurface  **ppZStencilSurface
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetDepthStencilSurface
// ******************************************************************
X_D3DSurface * WINAPI EMUPATCH(D3DDevice_GetDepthStencilSurface2)();

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetTile
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_GetTile)
(
    DWORD           Index,
    X_D3DTILE      *pTile
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTileNoWait
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetTileNoWait)
(
    DWORD               Index,
    CONST X_D3DTILE    *pTile
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateVertexShader
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_CreateVertexShader)
(
    CONST DWORD    *pDeclaration,
    CONST DWORD    *pFunction,
    DWORD          *pHandle,
    DWORD           Usage
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetPixelShaderConstant
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetPixelShaderConstant)
(
    DWORD       Register,
    CONST PVOID pConstantData,
    DWORD       ConstantCount
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexShaderConstant
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetVertexShaderConstant)
(
    INT         Register,
    CONST PVOID pConstantData,
    DWORD       ConstantCount
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexShaderConstant1
// ******************************************************************
VOID __fastcall EMUPATCH(D3DDevice_SetVertexShaderConstant1)
(
    INT         Register,
    CONST PVOID pConstantData
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexShaderConstant4
// ******************************************************************
VOID __fastcall EMUPATCH(D3DDevice_SetVertexShaderConstant4)
(
    INT         Register,
    CONST PVOID pConstantData
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexShaderConstantNotInline
// ******************************************************************
VOID __fastcall EMUPATCH(D3DDevice_SetVertexShaderConstantNotInline)
(
    INT         Register,
    CONST PVOID pConstantData,
    DWORD       ConstantCount
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_DeletePixelShader
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_DeletePixelShader)
(
    DWORD          Handle
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreatePixelShader
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_CreatePixelShader)
(
    X_D3DPIXELSHADERDEF    *pPSDef,
    DWORD				   *pHandle
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetPixelShader
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetPixelShader)
(
    DWORD           Handle
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateTexture2
// ******************************************************************
X_D3DResource * WINAPI EMUPATCH(D3DDevice_CreateTexture2)
(
    UINT                Width,
    UINT                Height,
    UINT                Depth,
    UINT                Levels,
    DWORD               Usage,
    X_D3DFORMAT         Format,
    D3DRESOURCETYPE     D3DResource
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateTexture
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_CreateTexture)
(
    UINT                Width,
    UINT                Height,
    UINT                Levels,
    DWORD               Usage,
    X_D3DFORMAT         Format,
    D3DPOOL             Pool,
    X_D3DTexture      **ppTexture
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateVolumeTexture
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_CreateVolumeTexture)
(
    UINT                 Width,
    UINT                 Height,
    UINT                 Depth,
    UINT                 Levels,
    DWORD                Usage,
    X_D3DFORMAT          Format,
    D3DPOOL              Pool,
    X_D3DVolumeTexture **ppVolumeTexture
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateCubeTexture
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_CreateCubeTexture)
(
    UINT                 EdgeLength,
    UINT                 Levels,
    DWORD                Usage,
    X_D3DFORMAT          Format,
    D3DPOOL              Pool,
    X_D3DCubeTexture  **ppCubeTexture
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateIndexBuffer
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_CreateIndexBuffer)
(
    UINT                 Length,
    DWORD                Usage,
    X_D3DFORMAT          Format,
    D3DPOOL              Pool,
    X_D3DIndexBuffer   **ppIndexBuffer
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateIndexBuffer2
// ******************************************************************
X_D3DIndexBuffer * WINAPI EMUPATCH(D3DDevice_CreateIndexBuffer2)(UINT Length);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetIndices
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetIndices)
(
    X_D3DIndexBuffer   *pIndexData,
    UINT                BaseVertexIndex
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTexture
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetTexture)
(
    DWORD           Stage,
    X_D3DResource  *pTexture
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SwitchTexture
// ******************************************************************
VOID __fastcall EMUPATCH(D3DDevice_SwitchTexture)
(
    DWORD           Method,
    DWORD           Data,
    DWORD           Format
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetDisplayMode
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_GetDisplayMode)
(
    X_D3DDISPLAYMODE         *pMode
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_Begin
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_Begin)
(
    X_D3DPRIMITIVETYPE     PrimitiveType
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexData2f
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetVertexData2f)
(
    int     Register,
    FLOAT   a,
    FLOAT   b
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexData2s
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetVertexData2s)
(
    int     Register,
    SHORT   a,
    SHORT   b
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexData4f
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetVertexData4f)
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
HRESULT WINAPI EMUPATCH(D3DDevice_SetVertexData4ub)
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
HRESULT WINAPI EMUPATCH(D3DDevice_SetVertexData4s)
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
HRESULT WINAPI EMUPATCH(D3DDevice_SetVertexDataColor)
(
    int         Register,
    D3DCOLOR    Color
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_End
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_End)();

// ******************************************************************
// * func: EmuIDirect3DDevice8_RunPushBuffer
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_RunPushBuffer)
(
    X_D3DPushBuffer       *pPushBuffer,
    X_D3DFixup            *pFixup
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_Clear
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_Clear)
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
HRESULT WINAPI EMUPATCH(D3DDevice_Present)
(
    CONST RECT* pSourceRect,
    CONST RECT* pDestRect,
    PVOID       pDummy1,
    PVOID       pDummy2
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_Swap
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_Swap)
(
    DWORD Flags
);

// ******************************************************************
// * func: EmuIDirect3DResource8_Register
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DResource_Register)
(
    X_D3DResource      *pThis,
    PVOID               pBase
);

// ******************************************************************
// * func: EmuIDirect3DResource8_Release
// ******************************************************************
ULONG WINAPI EMUPATCH(D3DResource_Release)
(
    X_D3DResource      *pThis
);

// ******************************************************************
// * func: EmuIDirect3DResource8_GetType
// ******************************************************************
X_D3DRESOURCETYPE WINAPI EMUPATCH(D3DResource_GetType)
(
    X_D3DResource      *pThis
);

// ******************************************************************
// * func: EmuIDirect3DResource8_AddRef
// ******************************************************************
ULONG WINAPI EMUPATCH(D3DResource_AddRef)
(
    X_D3DResource      *pThis
);

// ******************************************************************
// * func: EmuIDirect3DResource8_IsBusy
// ******************************************************************
BOOL WINAPI EMUPATCH(D3DResource_IsBusy)
(
    X_D3DResource      *pThis
);

// ******************************************************************
// * func: EmuLock2DSurface
// ******************************************************************
VOID WINAPI EMUPATCH(Lock2DSurface)
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
VOID WINAPI EMUPATCH(Lock3DSurface)
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
VOID WINAPI EMUPATCH(Get2DSurfaceDesc)
(
    X_D3DPixelContainer *pPixelContainer,
    DWORD                dwLevel,
    X_D3DSURFACE_DESC   *pDesc
);

// ******************************************************************
// * func: EmuGet2DSurfaceDescD
// ******************************************************************
VOID WINAPI EMUPATCH(Get2DSurfaceDescD)
(
    X_D3DPixelContainer *pPixelContainer,
    X_D3DSURFACE_DESC   *pDesc
);

// ******************************************************************
// * func: EmuIDirect3DSurface8_GetDesc
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DSurface_GetDesc)
(
    X_D3DResource      *pThis,
    X_D3DSURFACE_DESC  *pDesc
);

// ******************************************************************
// * func: EmuIDirect3DSurface8_LockRect
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DSurface_LockRect)
(
    X_D3DResource  *pThis,
    D3DLOCKED_RECT *pLockedRect,
    CONST RECT     *pRect,
    DWORD           Flags
);

// ******************************************************************
// * func: EmuIDirect3DBaseTexture8_GetLevelCount
// ******************************************************************
DWORD WINAPI EMUPATCH(D3DBaseTexture_GetLevelCount)
(
    X_D3DBaseTexture   *pThis
);

// ******************************************************************
// * func: EmuIDirect3DTexture8_GetSurfaceLevel
// ******************************************************************
X_D3DResource * WINAPI EMUPATCH(D3DTexture_GetSurfaceLevel2)
(
    X_D3DTexture   *pThis,
    UINT            Level
);

// ******************************************************************
// * func: EmuIDirect3DTexture8_LockRect
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DTexture_LockRect)
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
HRESULT WINAPI EMUPATCH(D3DTexture_GetSurfaceLevel)
(
    X_D3DTexture   *pThis,
    UINT            Level,
    X_D3DSurface  **ppSurfaceLevel
);

// ******************************************************************
// * func: EmuIDirect3DVolumeTexture8_LockBox
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DVolumeTexture_LockBox)
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
HRESULT WINAPI EMUPATCH(D3DCubeTexture_LockRect)
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
HRESULT WINAPI EMUPATCH(D3DDevice_CreateVertexBuffer)
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
X_D3DVertexBuffer* WINAPI EMUPATCH(D3DDevice_CreateVertexBuffer2)
(
    UINT Length
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_EnableOverlay
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_EnableOverlay)
(
    BOOL Enable
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_UpdateOverlay
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_UpdateOverlay)
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
BOOL WINAPI EMUPATCH(D3DDevice_GetOverlayUpdateStatus)();

// ******************************************************************
// * func: EmuIDirect3DDevice8_BlockUntilVerticalBlank
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_BlockUntilVerticalBlank)();

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVerticalBlankCallback
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetVerticalBlankCallback)
(
    D3DVBLANKCALLBACK pCallback
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTextureState_TexCoordIndex
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetTextureState_TexCoordIndex)
(
    DWORD Stage,
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTextureState_TwoSidedLighting
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetTextureState_TwoSidedLighting)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_BackFillMode
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_BackFillMode)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTextureState_BorderColor
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetTextureState_BorderColor)
(
    DWORD Stage,
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTextureState_ColorKeyColor
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetTextureState_ColorKeyColor)
(
    DWORD Stage,
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTextureState_BumpEnv
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetTextureState_BumpEnv)
(
    DWORD                      Stage,
    X_D3DTEXTURESTAGESTATETYPE Type,
    DWORD                      Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_FrontFace
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_FrontFace)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_LogicOp
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_LogicOp)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_NormalizeNormals
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_NormalizeNormals)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_TextureFactor
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_TextureFactor)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_ZBias
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_ZBias)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_EdgeAntiAlias)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_FillMode
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_FillMode)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_FogColor
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_FogColor)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_Dxt1NoiseEnable)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_Simple
// ******************************************************************
VOID __fastcall EMUPATCH(D3DDevice_SetRenderState_Simple)
(
    DWORD Method,
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_VertexBlend
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_VertexBlend)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_PSTextureModes
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_PSTextureModes)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_CullMode
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_CullMode)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_LineWidth
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_LineWidth)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_StencilFail
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_StencilFail)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_OcclusionCullEnable
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_OcclusionCullEnable)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_StencilCullEnable
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_StencilCullEnable)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_RopZCmpAlwaysRead)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_RopZRead
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_RopZRead)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_DoNotCullUncompressed
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_DoNotCullUncompressed)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_ZEnable
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_ZEnable)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_StencilEnable
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_StencilEnable)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_MultiSampleMask
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_MultiSampleMask)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_MultiSampleMode
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_MultiSampleMode)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_MultiSampleRenderTargetMode)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_MultiSampleAntiAlias)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_ShadowFunc
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_ShadowFunc)
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_YuvEnable
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetRenderState_YuvEnable)
(
    BOOL Enable
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTransform
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetTransform)
(
    D3DTRANSFORMSTATETYPE State,
    CONST D3DMATRIX      *pMatrix
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetTransform
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_GetTransform)
(
    D3DTRANSFORMSTATETYPE State,
    D3DMATRIX            *pMatrix
);

// ******************************************************************
// * func: EmuIDirect3DVertexBuffer8_Lock
// ******************************************************************
VOID WINAPI EMUPATCH(D3DVertexBuffer_Lock)
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
BYTE* WINAPI EMUPATCH(D3DVertexBuffer_Lock2)
(
    X_D3DVertexBuffer  *ppVertexBuffer,
    DWORD               Flags
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetStreamSource2
// ******************************************************************
XTL::X_D3DVertexBuffer* WINAPI EMUPATCH(D3DDevice_GetStreamSource2)
(
    UINT  StreamNumber,
    UINT *pStride
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetStreamSource
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetStreamSource)
(
    UINT                StreamNumber,
    X_D3DVertexBuffer  *pStreamData,
    UINT                Stride
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexShader
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetVertexShader)
(
    DWORD            Handle
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_DrawVertices
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_DrawVertices)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    UINT                StartVertex,
    UINT                VertexCount
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_DrawVerticesUP
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_DrawVerticesUP)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    UINT                VertexCount,
    CONST PVOID         pVertexStreamZeroData,
    UINT                VertexStreamZeroStride
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_DrawIndexedVertices
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_DrawIndexedVertices)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    UINT                VertexCount,
    CONST PWORD         pIndexData
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_DrawIndexedVerticesUP
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_DrawIndexedVerticesUP)
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
HRESULT WINAPI EMUPATCH(D3DDevice_SetLight)
(
    DWORD            Index,
    CONST D3DLIGHT8 *pLight
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetMaterial
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetMaterial)
(
    CONST D3DMATERIAL8 *pMaterial
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_LightEnable
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_LightEnable)
(
    DWORD            Index,
    BOOL             bEnable
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_Release
// ******************************************************************
ULONG WINAPI EMUPATCH(D3DDevice_Release)();

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreatePalette
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_CreatePalette)
(
    X_D3DPALETTESIZE    Size,
    X_D3DPalette      **ppPalette
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreatePalette2
// ******************************************************************
X_D3DPalette * WINAPI EMUPATCH(D3DDevice_CreatePalette2)
(
    X_D3DPALETTESIZE    Size
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderTarget
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetRenderTarget)
(
    X_D3DSurface    *pRenderTarget,
    X_D3DSurface    *pNewZStencil
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetPalette
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetPalette)
(
    DWORD         Stage,
    X_D3DPalette *pPalette
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetFlickerFilter
// ******************************************************************
void WINAPI EMUPATCH(D3DDevice_SetFlickerFilter)
(
    DWORD         Filter
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetSoftDisplayFilter
// ******************************************************************
void WINAPI EMUPATCH(D3DDevice_SetSoftDisplayFilter)
(
    BOOL Enable
);

// ******************************************************************
// * func: EmuIDirect3DPalette8_Lock
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DPalette_Lock)
(
    X_D3DPalette   *pThis,
    D3DCOLOR      **ppColors,
    DWORD           Flags
);

// ******************************************************************
// * func: EmuIDirect3DPalette8_Lock2
// ******************************************************************
D3DCOLOR * WINAPI EMUPATCH(D3DPalette_Lock2)
(
    X_D3DPalette   *pThis,
    DWORD           Flags
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVertexShaderSize
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_GetVertexShaderSize)
(
    DWORD Handle,
    UINT* pSize
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_DeleteVertexShader
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_DeleteVertexShader)
(
    DWORD Handle
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SelectVertexShaderDirect
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SelectVertexShaderDirect)
(
    X_VERTEXATTRIBUTEFORMAT *pVAF,
    DWORD                    Address
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetShaderConstantMode
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_GetShaderConstantMode)
(
    DWORD *pMode
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVertexShader
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_GetVertexShader)
(
    DWORD *pHandle
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVertexShaderConstant
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_GetVertexShaderConstant)
(
    INT   Register,
    void  *pConstantData,
    DWORD ConstantCount
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexShaderInputDirect
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetVertexShaderInputDirect)
(
    X_VERTEXATTRIBUTEFORMAT *pVAF,
    UINT                     StreamCount,
    X_STREAMINPUT           *pStreamInputs
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVertexShaderInput
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_GetVertexShaderInput)
(
    DWORD              *pHandle,
    UINT               *pStreamCount,
    X_STREAMINPUT      *pStreamInputs
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVertexShaderInput
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetVertexShaderInput)
(
    DWORD              Handle,
    UINT               StreamCount,
    X_STREAMINPUT     *pStreamInputs
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_RunVertexStateShader
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_RunVertexStateShader)
(
    DWORD        Address,
    CONST FLOAT *pData
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_LoadVertexShaderProgram
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_LoadVertexShaderProgram)
(
    CONST DWORD *pFunction,
    DWORD        Address
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVertexShaderType
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_GetVertexShaderType)
(
    DWORD  Handle,
    DWORD *pType
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVertexShaderDeclaration
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_GetVertexShaderDeclaration)
(
    DWORD  Handle,
    PVOID  pData,
    DWORD *pSizeOfData
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVertexShaderFunction
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_GetVertexShaderFunction)
(
    DWORD  Handle,
    PVOID *pData,
    DWORD *pSizeOfData
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetDepthClipPlanes
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetDepthClipPlanes)
(
    FLOAT Near,
    FLOAT Far,
    DWORD Flags
);

// ******************************************************************
// * func: EmuIDirect3D8_AllocContiguousMemory
// ******************************************************************
PVOID WINAPI EMUPATCH(D3D_AllocContiguousMemory)
(
    SIZE_T dwSize,
    DWORD dwAllocAttributes
);

// ******************************************************************
// * func: IDirect3DTexture8_GetLevelDesc
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DTexture_GetLevelDesc)
(
    UINT Level,
    X_D3DSURFACE_DESC* pDesc
);

// ******************************************************************
// * func: EmuIDirect3D8_CheckDeviceMultiSampleType
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3D_CheckDeviceMultiSampleType)
(
    UINT                 Adapter,
    D3DDEVTYPE           DeviceType,
    X_D3DFORMAT          SurfaceFormat,
    BOOL                 Windowed,
    D3DMULTISAMPLE_TYPE  MultiSampleType
);

// ******************************************************************
// * func: EmuIDirect3D8_GetDeviceCaps
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3D_GetDeviceCaps)
(
    UINT        Adapter,
    D3DDEVTYPE  DeviceType,
    D3DCAPS8    *pCaps
);

// ******************************************************************
// * func: EmuIDirect3D8_SetPushBufferSize
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3D_SetPushBufferSize)
(
    DWORD PushBufferSize,
    DWORD KickOffSize
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_InsertFence
// ******************************************************************
DWORD WINAPI EMUPATCH(D3DDevice_InsertFence)();

// ******************************************************************
// * func: EmuIDirect3DDevice8_IsFencePending
// ******************************************************************
BOOL WINAPI EMUPATCH(D3DDevice_IsFencePending)
(
    DWORD Fence
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_BlockOnFence
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_BlockOnFence)
(
    DWORD Fence
);

// ******************************************************************
// * func: EmuIDirect3DResource8_BlockUntilNotBusy
// ******************************************************************
VOID WINAPI EMUPATCH(D3DResource_BlockUntilNotBusy)
(
    X_D3DResource *pThis
);

// ******************************************************************
// * func: EmuIDirect3DVertexBuffer8_GetDesc
// ******************************************************************
VOID WINAPI EMUPATCH(D3DVertexBuffer_GetDesc)
(
    X_D3DVertexBuffer    *pThis,
    D3DVERTEXBUFFER_DESC *pDesc
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetScissors
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetScissors)
(
    DWORD          Count,
    BOOL           Exclusive,
    CONST D3DRECT  *pRects
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetScreenSpaceOffset
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetScreenSpaceOffset)
(
    FLOAT x,
    FLOAT y
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_InsertFence
// ******************************************************************
DWORD WINAPI EMUPATCH(D3DDevice_InsertFence)();

// ******************************************************************
// * func: EmuIDirect3DDevice8_BlockOnFence
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_BlockOnFence)
(
	DWORD Fence
);

// ******************************************************************
// * func: EmuIDirect3DResource8_BlockUntilNotBusy
// ******************************************************************
VOID WINAPI EMUPATCH(D3DResource_BlockUntilNotBusy)
(
	X_D3DResource *pThis
);

// ******************************************************************
// * func: EmuIDirect3DVertexBuffer8_GetDesc
// ******************************************************************
VOID WINAPI EMUPATCH(D3DVertexBuffer_GetDesc)
(
	X_D3DVertexBuffer    *pThis,
	D3DVERTEXBUFFER_DESC *pDesc
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetScissors
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetScissors)
(
	DWORD          Count, 
	BOOL           Exclusive, 
	CONST D3DRECT  *pRects
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetPixelShaderProgram
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetPixelShaderProgram)
(
	X_D3DPIXELSHADERDEF *pPSDef
);

// ******************************************************************
// * func: EmuIDirect3DDevice_SetScreenSpaceOffset
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetScreenSpaceOffset)
(
	FLOAT x,
	FLOAT y
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateStateBlock
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_CreateStateBlock)
(
	D3DSTATEBLOCKTYPE Type,
	DWORD			  *pToken
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_InsertCallback
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_InsertCallback)
(
	X_D3DCALLBACKTYPE	Type,
	X_D3DCALLBACK		pCallback,
	DWORD				Context
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_DrawRectPatch
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_DrawRectPatch)
(
	UINT					Handle,
	CONST FLOAT				*pNumSegs,
	CONST D3DRECTPATCH_INFO *pRectPatchInfo
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetProjectionViewportMatrix
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_GetProjectionViewportMatrix)
(
	D3DXMATRIX *pProjectionViewport
);

// ******************************************************************
// * func: EmuD3DDevice_KickOff (D3D::CDevice::KickOff)
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_KickOff)();

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetTexture2
// ******************************************************************
X_D3DResource* WINAPI EMUPATCH(D3DDevice_GetTexture2)(DWORD Stage);

// ******************************************************************
// * func: EmuD3DDevice_SetStateVB (D3D::CDevice::SetStateVB)
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetStateVB)( ULONG Unknown1 );

// ******************************************************************
// * func: EmuD3DDevice_SetStateUP (D3D::CDevice::SetStateUP)
// ******************************************************************
VOID WINAPI EMUPATCH(D3DDevice_SetStateUP)();

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetStipple
// ******************************************************************
void WINAPI EMUPATCH(D3DDevice_SetStipple)( DWORD* pPattern );

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetSwapCallback
// ******************************************************************
void WINAPI EMUPATCH(D3DDevice_SetSwapCallback)
(
	D3DSWAPCALLBACK		pCallback
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_PersistDisplay
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_PersistDisplay)();

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetPersistedSurface
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_GetPersistedSurface)(X_D3DSurface **ppSurface);
X_D3DSurface* WINAPI EMUPATCH(D3DDevice_GetPersistedSurface2)();

// ******************************************************************
// * func: EmuIDirect3DDevice8_Unknown1
// ******************************************************************
void WINAPI EMUPATCH(D3DDevice_Unknown1)();

// ******************************************************************
// * func: EmuIDirect3DDevice8_PrimeVertexCache
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_PrimeVertexCache)
(
	UINT VertexCount,
	WORD *pIndexData
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_SampleAlpha
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetRenderState_SampleAlpha)
(
	DWORD dwSampleAlpha
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_Deferred
// ******************************************************************
VOID __fastcall EMUPATCH(D3DDevice_SetRenderState_Deferred)
(
	DWORD State,
	DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_DeleteStateBlock
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_DeleteStateBlock)
(
	DWORD Token
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetModelView
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetModelView)
(
	CONST D3DMATRIX *pModelView, 
	CONST D3DMATRIX *pInverseModelView, 
	CONST D3DMATRIX *pComposite
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_FlushVertexCache
// ******************************************************************
void WINAPI EMUPATCH(D3DDevice_FlushVertexCache)();

// ******************************************************************
// * func: EmuIDirect3DDevice8_BeginPushBuffer
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_BeginPushBuffer)
(
	X_D3DPushBuffer *pPushBuffer
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_EndPushBuffer
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_EndPushBuffer)();

// ******************************************************************
// * func: EmuXMETAL_StartPush
// ******************************************************************
void WINAPI EMUPATCH(XMETAL_StartPush)(void* Unknown);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetModelView
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_GetModelView)(D3DXMATRIX* pModelView);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetBackMaterial
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetBackMaterial)(D3DMATERIAL8* pMaterial);

// ******************************************************************
// * func: EmuIDirect3D8_GetAdapterIdentifier
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3D_GetAdapterIdentifier)
(
	UINT					Adapter,
	DWORD					Flags,
	D3DADAPTER_IDENTIFIER8* pIdentifier
);

// ******************************************************************
// * func: D3D::MakeRequestedSpace
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3D_MakeRequestedSpace)( DWORD Unknown1, DWORD Unknown2 );

// ******************************************************************
// * func: D3DDevice_MakeSpace
// ******************************************************************
void WINAPI EMUPATCH(D3DDevice_MakeSpace)();

// ******************************************************************
// * func: D3D::SetCommonDebugRegisters
// ******************************************************************
void WINAPI EMUPATCH(D3D_SetCommonDebugRegisters)();

// ******************************************************************
// * func: D3D::BlockOnTime
// ******************************************************************
void WINAPI EMUPATCH(D3D_BlockOnTime)( DWORD Unknown1, int Unknown2 );

// ******************************************************************
// * func: D3D::BlockOnResource
// ******************************************************************
void WINAPI EMUPATCH(D3D_BlockOnResource)( X_D3DResource* pResource );

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetPushBufferOffset
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_GetPushBufferOffset)
(
	DWORD *pOffset
);

// ******************************************************************
// * func: EmuIDirect3DCubeTexture8_GetCubeMapSurface
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DCubeTexture_GetCubeMapSurface)
(
	X_D3DCubeTexture*	pThis,
	D3DCUBEMAP_FACES	FaceType,
	UINT				Level,
	X_D3DSurface**		ppCubeMapSurface
);

// ******************************************************************
// * func: EmuIDirect3DCubeTexture8_GetCubeMapSurface2
// ******************************************************************
X_D3DSurface* WINAPI EMUPATCH(D3DCubeTexture_GetCubeMapSurface2)
(
	X_D3DCubeTexture*	pThis,
	D3DCUBEMAP_FACES	FaceType,
	UINT				Level
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetPixelShader
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_GetPixelShader)
(
	DWORD  Name,
	DWORD* pHandle
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderTargetFast
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_SetRenderTargetFast)
(
    X_D3DSurface	*pRenderTarget,
    X_D3DSurface	*pNewZStencil,
    DWORD			Flags
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetScissors
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_GetScissors)
(
	DWORD	*pCount, 
	BOOL	*pExclusive, 
	D3DRECT *pRects
);
// ******************************************************************
// * func: EmuIDirect3DDevice8_GetBackMaterial
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_GetBackMaterial)(D3DMATERIAL8* pMaterial);

// ******************************************************************
// * func: EmuD3D::LazySetPointParams
// ******************************************************************
void WINAPI EMUPATCH(D3D_LazySetPointParams)( void* Device );

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetMaterial
// ******************************************************************
HRESULT WINAPI EMUPATCH(D3DDevice_GetMaterial)(D3DMATERIAL8* pMaterial);

#endif
