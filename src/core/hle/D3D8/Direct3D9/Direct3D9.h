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
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef DIRECT3D9_H
#define DIRECT3D9_H

#include "core\hle\XAPI\Xapi.h" // For EMUPATCH

// NOTE: this is necessary or else d3dx9mesh.h fails to compile because of undefined VOID macros
#ifndef VOID
#define VOID void
#endif

#include "core\hle\D3D8\XbD3D8Types.h"

#define DIRECTDRAW_VERSION 0x0700
#include <ddraw.h>

void LookupTrampolinesD3D();

// initialize render window
extern void CxbxInitWindow(bool bFullInit);

void CxbxUpdateNativeD3DResources();

void CxbxImpl_SetRenderTarget(xbox::X_D3DSurface* pRenderTarget, xbox::X_D3DSurface* pNewZStencil);
void CxbxImpl_SetViewport(xbox::X_D3DVIEWPORT8* pViewport);

// initialize direct3d
extern void EmuD3DInit();

// cleanup direct3d
extern void EmuD3DCleanup();

extern IDirect3DDevice *g_pD3DDevice;

extern xbox::dword_xt g_Xbox_VertexShader_Handle;

extern xbox::X_PixelShader *g_pXbox_PixelShader;

extern xbox::X_D3DBaseTexture *g_pXbox_SetTexture[xbox::X_D3DTS_STAGECOUNT];

namespace xbox {

// ******************************************************************
// * patch: Direct3D_CreateDevice
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(Direct3D_CreateDevice)
(
    uint_xt                     Adapter,
	X_D3DDEVTYPE                DeviceType,
    HWND                        hFocusWindow,
    dword_xt                    BehaviorFlags,
    X_D3DPRESENT_PARAMETERS    *pPresentationParameters,
    X_D3DDevice           **ppReturnedDeviceInterface
);

xbox::hresult_xt WINAPI EMUPATCH(Direct3D_CreateDevice_16__LTCG_eax_BehaviorFlags_ebx_ppReturnedDeviceInterface)
(
    uint_xt                     Adapter,
	X_D3DDEVTYPE                DeviceType,
    HWND                        hFocusWindow,
    X_D3DPRESENT_PARAMETERS    *pPresentationParameters
);

xbox::hresult_xt WINAPI EMUPATCH(Direct3D_CreateDevice_16__LTCG_eax_BehaviorFlags_ecx_ppReturnedDeviceInterface)
(
    uint_xt                     Adapter,
	X_D3DDEVTYPE                DeviceType,
    HWND                        hFocusWindow,
    X_D3DPRESENT_PARAMETERS    *pPresentationParameters
);

xbox::hresult_xt WINAPI EMUPATCH(Direct3D_CreateDevice_4)
(
    X_D3DPRESENT_PARAMETERS    *pPresentationParameters
);

// ******************************************************************
// * patch: IDirect3DResource8_IsBusy
// ******************************************************************
xbox::bool_xt WINAPI EMUPATCH(D3DDevice_IsBusy)();

// ******************************************************************
// * patch: D3DDevice_GetCreationParameters
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetCreationParameters)
(
	D3DDEVICE_CREATION_PARAMETERS *pParameters
);

#if 0 // patch disabled
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

#if 0 // patch disabled
// ******************************************************************
// * patch: D3DDevice_GetDeviceCaps
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetDeviceCaps)
(
    X_D3DCAPS                   *pCaps
);
#endif

// ******************************************************************
// * patch: D3DDevice_GetDisplayFieldStatus
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetDisplayFieldStatus)
(
	X_D3DFIELD_STATUS *pFieldStatus
);

// ******************************************************************
// * patch: D3DDevice_BeginPush
// ******************************************************************
xbox::PDWORD WINAPI EMUPATCH(D3DDevice_BeginPush)(dword_xt Count);

// ******************************************************************
// * patch: D3DDevice_BeginPush2  //two arg version for xdk before 4531
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_BeginPush2)(dword_xt Count, dword_xt **ppPush);

// ******************************************************************
// * patch: D3DDevice_EndPush
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_EndPush)(dword_xt *pPush);

// ******************************************************************
// * patch: D3DDevice_BeginVisibilityTest
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_BeginVisibilityTest)();

// ******************************************************************
// * patch: D3DDevice_EndVisibilityTest
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_EndVisibilityTest)
(
    dword_xt                       Index
);

xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_EndVisibilityTest_0)();

// ******************************************************************
// * patch: D3DDevice_GetVisibilityTestResult
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_GetVisibilityTestResult)
(
    dword_xt                       Index,
    uint_xt                       *pResult,
    ulonglong_xt                  *pTimeStamp
);

// ******************************************************************
// * patch: D3DDevice_SetBackBufferScale
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetBackBufferScale)(float_xt x, float_xt y);

// ******************************************************************
// * patch: D3DDevice_LoadVertexShader
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_LoadVertexShader)
(
    dword_xt                       Handle,
    dword_xt                       Address
);

xbox::void_xt WINAPI EMUPATCH(D3DDevice_LoadVertexShader_0__LTCG_eax_Address_ecx_Handle)();
xbox::void_xt WINAPI EMUPATCH(D3DDevice_LoadVertexShader_0__LTCG_eax_Address_edx_Handle)();
xbox::void_xt WINAPI EMUPATCH(D3DDevice_LoadVertexShader_4)
(
    dword_xt                       Address
);

// ******************************************************************
// * patch: D3DDevice_SelectVertexShader
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SelectVertexShader)
(
    dword_xt                       Handle,
    dword_xt                       Address
);

xbox::void_xt WINAPI EMUPATCH(D3DDevice_SelectVertexShader_0)();
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SelectVertexShader_4)
(
    dword_xt                       Address
);

// ******************************************************************
// * patch: D3D_KickOffAndWaitForIdle
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3D_KickOffAndWaitForIdle)();

// ******************************************************************
// * patch: D3D_KickOffAndWaitForIdle
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3D_KickOffAndWaitForIdle2)(dword_xt dwDummy1, dword_xt dwDummy2);

// ******************************************************************
// * patch: D3DDevice_SetGammaRamp
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetGammaRamp)
(
    dword_xt                   dwFlags,
    CONST X_D3DGAMMARAMP   *pRamp
);

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
// * patch: D3DDevice_CopyRects
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_CopyRects)
(
    X_D3DSurface       *pSourceSurface,
    CONST RECT         *pSourceRectsArray,
    uint_xt                cRects,
    X_D3DSurface       *pDestinationSurface,
    CONST POINT        *pDestPointsArray
);

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
// * patch: D3DDevice_GetGammaRamp
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetGammaRamp)
(
    X_D3DGAMMARAMP     *pRamp
);

// ******************************************************************
// * patch: D3DDevice_GetBackBuffer2
// ******************************************************************
X_D3DSurface* WINAPI EMUPATCH(D3DDevice_GetBackBuffer2)
(
    int_xt                 BackBuffer
);

X_D3DSurface* WINAPI EMUPATCH(D3DDevice_GetBackBuffer2_0__LTCG_eax1)();

// ******************************************************************
// * patch: D3DDevice_GetBackBuffer
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetBackBuffer)
(
    int_xt                 BackBuffer,
    D3DBACKBUFFER_TYPE  Type,
    X_D3DSurface      **ppBackBuffer
);

// ******************************************************************
// * patch: D3DDevice_SetViewport
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetViewport)
(
    X_D3DVIEWPORT8 *pViewport
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
// * patch: D3DDevice_SetShaderConstantMode
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetShaderConstantMode)
(
    X_VERTEXSHADERCONSTANTMODE Mode
);

xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetShaderConstantMode_0__LTCG_eax1)();

// ******************************************************************
// * patch: D3DDevice_Reset
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_Reset)
(
    X_D3DPRESENT_PARAMETERS *pPresentationParameters
);

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
// * patch: D3DDevice_SetVertexShaderConstant
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetVertexShaderConstant)
(
    int_xt         Register,
    CONST PVOID pConstantData,
    dword_xt       ConstantCount
);

xbox::void_xt __fastcall EMUPATCH(D3DDevice_SetVertexShaderConstant_8)
(
    void*,
    dword_xt    ConstantCount,
    int_xt      Register,
    CONST PVOID pConstantData
);


// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstant1
// ******************************************************************
xbox::void_xt __fastcall EMUPATCH(D3DDevice_SetVertexShaderConstant1)
(
    int_xt         Register,
    CONST PVOID pConstantData
);

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstant1Fast
// ******************************************************************
xbox::void_xt __fastcall EMUPATCH(D3DDevice_SetVertexShaderConstant1Fast)
(
    int_xt         Register,
    CONST PVOID pConstantData
);

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstant4
// ******************************************************************
xbox::void_xt __fastcall EMUPATCH(D3DDevice_SetVertexShaderConstant4)
(
    int_xt         Register,
    CONST PVOID pConstantData
);

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstantNotInline
// ******************************************************************
xbox::void_xt __fastcall EMUPATCH(D3DDevice_SetVertexShaderConstantNotInline)
(
    int_xt         Register,
    CONST PVOID pConstantData,
    dword_xt       ConstantCount
);

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstantNotInlineFast
// ******************************************************************
xbox::void_xt __fastcall EMUPATCH(D3DDevice_SetVertexShaderConstantNotInlineFast)
(
    int_xt         Register,
    CONST PVOID pConstantData,
    dword_xt       ConstantCount
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

// ******************************************************************
// * patch: D3DDevice_SetPixelShader
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetPixelShader)
(
    dword_xt           Handle
);

xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetPixelShader_0)();

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
    uint_xt                Width,
    uint_xt                Height,
    uint_xt                Levels,
    dword_xt               Usage,
    X_D3DFORMAT         Format,
    D3DPOOL             Pool,
    X_D3DTexture      **ppTexture
);

// ******************************************************************
// * patch: D3DDevice_CreateVolumeTexture
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_CreateVolumeTexture)
(
    uint_xt                 Width,
    uint_xt                 Height,
    uint_xt                 Depth,
    uint_xt                 Levels,
    dword_xt                Usage,
    X_D3DFORMAT          Format,
    D3DPOOL              Pool,
    X_D3DVolumeTexture **ppVolumeTexture
);

// ******************************************************************
// * patch: D3DDevice_CreateCubeTexture
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_CreateCubeTexture)
(
    uint_xt                 EdgeLength,
    uint_xt                 Levels,
    dword_xt                Usage,
    X_D3DFORMAT          Format,
    D3DPOOL              Pool,
    X_D3DCubeTexture  **ppCubeTexture
);

// ******************************************************************
// * patch: D3DDevice_CreateIndexBuffer
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_CreateIndexBuffer)
(
    uint_xt                 Length,
    dword_xt                Usage,
    X_D3DFORMAT          Format,
    D3DPOOL              Pool,
    X_D3DIndexBuffer   **ppIndexBuffer
);

// ******************************************************************
// * patch: D3DDevice_CreateIndexBuffer2
// ******************************************************************
X_D3DIndexBuffer * WINAPI EMUPATCH(D3DDevice_CreateIndexBuffer2)(uint_xt Length);

// ******************************************************************
// * patch: D3DDevice_SetIndices
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetIndices)
(
    X_D3DIndexBuffer   *pIndexData,
    uint_xt                BaseVertexIndex
);

// ******************************************************************
// * patch: D3DDevice_SetIndices_4
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetIndices_4)
(
    uint_xt                BaseVertexIndex
);

// ******************************************************************
// * patch: D3DDevice_SetTexture
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetTexture)
(
    dword_xt           Stage,
	X_D3DBaseTexture  *pTexture
);

xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetTexture_4__LTCG_eax_pTexture)
(
	dword_xt           Stage
);

xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetTexture_4)
(
	X_D3DBaseTexture  *pTexture
);

// ******************************************************************
// * patch: D3DDevice_SwitchTexture
// ******************************************************************
xbox::void_xt __fastcall EMUPATCH(D3DDevice_SwitchTexture)
(
    dword_xt           Method,
    dword_xt           Data,
    dword_xt           Format
);

// ******************************************************************
// * patch: D3DDevice_GetDisplayMode
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetDisplayMode)
(
    X_D3DDISPLAYMODE         *pModes
);

// ******************************************************************
// * patch: D3DDevice_Begin
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_Begin)
(
    X_D3DPRIMITIVETYPE     PrimitiveType
);

// ******************************************************************
// * patch: D3DDevice_SetVertexData2f
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetVertexData2f)
(
    int_xt     Register,
    float_xt   a,
    float_xt   b
);

// ******************************************************************
// * patch: D3DDevice_SetVertexData2s
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetVertexData2s)
(
    int_xt     Register,
    short_xt   a,
    short_xt   b
);

// ******************************************************************
// * patch: D3DDevice_SetVertexData4f
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetVertexData4f)
(
    int_xt     Register,
    float_xt   a,
    float_xt   b,
    float_xt   c,
    float_xt   d
);

// ******************************************************************
// * patch: D3DDevice_SetVertexData4f_16
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetVertexData4f_16)
(
	float_xt   a,
	float_xt   b,
	float_xt   c,
	float_xt   d
);

// ******************************************************************
// * patch: D3DDevice_SetVertexData4ub
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetVertexData4ub)
(
	int_xt	Register,
	byte_xt	a,
	byte_xt	b,
	byte_xt	c,
	byte_xt	d
);

// ******************************************************************
// * patch: D3DDevice_SetVertexData4s
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetVertexData4s)
(
	int_xt		Register,
	short_xt	a,
	short_xt	b,
	short_xt	c,
	short_xt	d
);

// ******************************************************************
// * patch: D3DDevice_SetVertexDataColor
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetVertexDataColor)
(
    int_xt      Register,
    D3DCOLOR    Color
);

// ******************************************************************
// * patch: D3DDevice_End
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_End)();

// ******************************************************************
// * patch: D3DDevice_RunPushBuffer
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_RunPushBuffer)
(
    X_D3DPushBuffer       *pPushBuffer,
    X_D3DFixup            *pFixup
);

// ******************************************************************
// * patch: D3DDevice_Clear
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_Clear)
(
    dword_xt                  Count,
    CONST D3DRECT         *pRects,
    dword_xt                  Flags,
    D3DCOLOR               Color,
    float                  Z,
    dword_xt                  Stencil
);

// ******************************************************************
// * patch: D3DDevice_Present
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_Present)
(
    CONST RECT* pSourceRect,
    CONST RECT* pDestRect,
    PVOID       pDummy1,
    PVOID       pDummy2
);

// ******************************************************************
// * patch: D3DDevice_Swap
// ******************************************************************
dword_xt WINAPI EMUPATCH(D3DDevice_Swap)
(
    dword_xt Flags
);

dword_xt WINAPI EMUPATCH(D3DDevice_Swap_0)();

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

#if 0 // patch disabled
// ******************************************************************
// * patch: IDirect3DResource8_GetType
// ******************************************************************
X_D3DRESOURCETYPE WINAPI EMUPATCH(D3DResource_GetType)
(
    X_D3DResource      *pThis
);
#endif

// ******************************************************************
// * patch: IDirect3DResource8_AddRef
// ******************************************************************
xbox::ulong_xt WINAPI EMUPATCH(D3DResource_AddRef)
(
    X_D3DResource      *pThis
);

// ******************************************************************
// * patch: IDirect3DResource8_IsBusy
// ******************************************************************
xbox::bool_xt WINAPI EMUPATCH(D3DResource_IsBusy)
(
    X_D3DResource      *pThis
);

// ******************************************************************
// * patch: Lock2DSurface
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(Lock2DSurface)
(
    X_D3DPixelContainer *pPixelContainer,
    X_D3DCUBEMAP_FACES   FaceType,
    uint_xt              Level,
    D3DLOCKED_RECT      *pLockedRect,
    RECT                *pRect,
    dword_xt             Flags
);

// ******************************************************************
// * patch: Lock3DSurface
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(Lock3DSurface)
(
    X_D3DPixelContainer *pPixelContainer,
    uint_xt				Level,
	D3DLOCKED_BOX		*pLockedVolume,
	D3DBOX				*pBox,
	dword_xt				Flags
);

#if 0 // patch disabled
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
    X_D3DResource  *pThis,
    D3DLOCKED_RECT *pLockedRect,
    CONST RECT     *pRect,
    dword_xt           Flags
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
    X_D3DTexture   *pThis,
    uint_xt            Level,
    D3DLOCKED_RECT *pLockedRect,
    CONST RECT     *pRect,
    dword_xt           Flags
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
    D3DLOCKED_BOX      *pLockedVolume,
    CONST D3DBOX       *pBox,
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
    D3DLOCKED_RECT     *pLockedBox,
    CONST RECT         *pRect,
    dword_xt            Flags
);

// ******************************************************************
// * patch: D3DDevice_CreateVertexBuffer
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_CreateVertexBuffer)
(
    uint_xt                Length,
    dword_xt               Usage,
    dword_xt               FVF,
    D3DPOOL             Pool,
    X_D3DVertexBuffer **ppVertexBuffer
);
#endif

#if 0 // patch disabled
// ******************************************************************
// * patch: D3DDevice_CreateVertexBuffer2
// ******************************************************************
X_D3DVertexBuffer* WINAPI EMUPATCH(D3DDevice_CreateVertexBuffer2)
(
    uint_xt Length
);
#endif

// ******************************************************************
// * patch: D3DDevice_EnableOverlay
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_EnableOverlay)
(
    bool_xt Enable
);

// ******************************************************************
// * patch: D3DDevice_UpdateOverlay
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_UpdateOverlay)
(
    X_D3DSurface *pSurface,
    CONST RECT   *SrcRect,
    CONST RECT   *DstRect,
    bool_xt          EnableColorKey,
    D3DCOLOR      ColorKey
);

// ******************************************************************
// * patch: D3DDevice_GetOverlayUpdateStatus
// ******************************************************************
xbox::bool_xt WINAPI EMUPATCH(D3DDevice_GetOverlayUpdateStatus)();

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
// * patch: D3DDevice_SetRenderState_Simple
// ******************************************************************
xbox::void_xt __fastcall EMUPATCH(D3DDevice_SetRenderState_Simple)
(
    dword_xt Method,
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
// * patch: D3DDevice_SetTransform
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetTransform)
(
	xbox::X_D3DTRANSFORMSTATETYPE State,
    CONST D3DMATRIX      *pMatrix
);

xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetTransform_0__LTCG_eax1_edx2)();

// ******************************************************************
// * patch: D3DDevice_MultiplyTransform
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_MultiplyTransform)
(
	xbox::X_D3DTRANSFORMSTATETYPE State,
	CONST D3DMATRIX      *pMatrix
);

// ******************************************************************
// * patch: D3DDevice_GetTransform
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetTransform)
(
    D3DTRANSFORMSTATETYPE State,
    D3DMATRIX            *pMatrix
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
// * patch: D3DDevice_SetStreamSource
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetStreamSource)
(
    uint_xt                StreamNumber,
    X_D3DVertexBuffer  *pStreamData,
    uint_xt                Stride
);

xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetStreamSource_0__LTCG_eax_StreamNumber_edi_pStreamData_ebx_Stride)();

xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetStreamSource_4)
(
    uint_xt                Stride
);

xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetStreamSource_8)
(
    X_D3DVertexBuffer  *pStreamData,
    uint_xt                Stride
);

xbox::void_xt __fastcall EMUPATCH(D3DDevice_SetStreamSource_8__LTCG_edx_StreamNumber)
(
    void*,
    uint_xt                StreamNumber,
    X_D3DVertexBuffer  *pStreamData,
    uint_xt                Stride
);

// ******************************************************************
// * patch: D3DDevice_SetVertexShader
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetVertexShader)
(
    dword_xt            Handle
);

xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetVertexShader_0)();

// ******************************************************************
// * patch: D3DDevice_DrawVertices
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_DrawVertices)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    uint_xt                StartVertex,
    uint_xt                VertexCount
);

// ******************************************************************
// * patch: D3DDevice_DrawVertices_4
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_DrawVertices_4)
(
    X_D3DPRIMITIVETYPE  PrimitiveType
);

// ******************************************************************
// * patch: D3DDevice_DrawVerticesUP
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_DrawVerticesUP)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    uint_xt                VertexCount,
    CONST PVOID         pVertexStreamZeroData,
    uint_xt                VertexStreamZeroStride
);

xbox::void_xt WINAPI EMUPATCH(D3DDevice_DrawVerticesUP_12)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    uint_xt                VertexCount,
    uint_xt                VertexStreamZeroStride
);


// ******************************************************************
// * patch: D3DDevice_DrawIndexedVertices
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_DrawIndexedVertices)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    uint_xt                VertexCount,
    CONST PWORD         pIndexData
);

// ******************************************************************
// * patch: D3DDevice_DrawIndexedVerticesUP
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_DrawIndexedVerticesUP)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    uint_xt                VertexCount,
    CONST PVOID         pIndexData,
    CONST PVOID         pVertexStreamZeroData,
    uint_xt                VertexStreamZeroStride
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
// * patch: D3DDevice_SetLight
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_SetLight)
(
    dword_xt            Index,
    CONST X_D3DLIGHT8 *pLight
);

// ******************************************************************
// * patch: D3DDevice_SetMaterial
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetMaterial)
(
    CONST X_D3DMATERIAL8 *pMaterial
);

// ******************************************************************
// * patch: D3DDevice_LightEnable
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_LightEnable)
(
    dword_xt            Index,
    bool_xt             bEnable
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
// * patch: D3DDevice_SetRenderTarget
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderTarget)
(
    X_D3DSurface    *pRenderTarget,
    X_D3DSurface    *pNewZStencil
);

// ******************************************************************
// * patch: D3DDevice_SetRenderTarget_0
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderTarget_0)();

// ******************************************************************
// * patch: D3D_CommonSetRenderTarget
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3D_CommonSetRenderTarget)
(
    X_D3DSurface    *pRenderTarget,
    X_D3DSurface    *pNewZStencil,
    void            *unknown
);

// ******************************************************************
// * patch: D3DDevice_SetPalette
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetPalette)
(
    dword_xt         Stage,
    X_D3DPalette *pPalette
);

xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetPalette_4)
(
    X_D3DPalette *pPalette
);

// ******************************************************************
// * patch: D3DDevice_SetFlickerFilter
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetFlickerFilter)
(
    dword_xt         Filter
);

xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetFlickerFilter_0)();

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
    X_D3DPalette   *pThis,
    D3DCOLOR      **ppColors,
    dword_xt           Flags
);

// ******************************************************************
// * patch: IDirect3DPalette8_Lock2
// ******************************************************************
D3DCOLOR * WINAPI EMUPATCH(D3DPalette_Lock2)
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
// * patch: D3DDevice_DeleteVertexShader
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_DeleteVertexShader)
(
    dword_xt Handle
);

xbox::void_xt WINAPI EMUPATCH(D3DDevice_DeleteVertexShader_0)();

// ******************************************************************
// * patch: D3DDevice_GetShaderConstantMode
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetShaderConstantMode)
(
    dword_xt *pMode
);

// ******************************************************************
// * patch: D3DDevice_GetVertexShader
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetVertexShader)
(
    dword_xt *pHandle
);

// ******************************************************************
// * patch: D3DDevice_GetVertexShaderConstant
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetVertexShaderConstant)
(
    int_xt   Register,
    void  *pConstantData,
    dword_xt ConstantCount
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
// * patch: D3DDevice_SetVertexShaderInput
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetVertexShaderInput)
(
    dword_xt              Handle,
    uint_xt               StreamCount,
    X_STREAMINPUT     *pStreamInputs
);

// ******************************************************************
// * patch: D3DDevice_RunVertexStateShader
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_RunVertexStateShader)
(
    dword_xt        Address,
    CONST float_xt *pData
);

// ******************************************************************
// * patch: D3DDevice_LoadVertexShaderProgram
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_LoadVertexShaderProgram)
(
    CONST dword_xt *pFunction,
    dword_xt        Address
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
// * patch: D3DDevice_SetDepthClipPlanes
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_SetDepthClipPlanes)
(
    float_xt Near,
    float_xt Far,
    dword_xt Flags
);

#if 0 // DISABLED (Just calls MmAllocateContiguousMemory)
// ******************************************************************
// * patch: D3D_AllocContiguousMemory
// ******************************************************************
PVOID WINAPI EMUPATCH(D3D_AllocContiguousMemory)
(
    size_xt dwSize,
    dword_xt dwAllocAttributes
);
#endif

#if 0 // DISABLED (Just calls Get2DSurfaceDesc)
// ******************************************************************
// * patch: D3DTexture_GetLevelDesc
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DTexture_GetLevelDesc)
(
    uint_xt Level,
    X_D3DSURFACE_DESC* pDesc
);
#endif

#if 0 // patch disabled
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

#if 0 // patch disabled
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

#if 0 // patch disabled
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
// * patch: D3DDevice_InsertFence
// ******************************************************************
xbox::dword_xt WINAPI EMUPATCH(D3DDevice_InsertFence)();

// ******************************************************************
// * patch: D3DDevice_IsFencePending
// ******************************************************************
xbox::bool_xt WINAPI EMUPATCH(D3DDevice_IsFencePending)
(
    dword_xt Fence
);

// ******************************************************************
// * patch: D3DDevice_BlockOnFence
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_BlockOnFence)
(
    dword_xt Fence
);

// ******************************************************************
// * patch: D3DResource_BlockUntilNotBusy
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DResource_BlockUntilNotBusy)
(
    X_D3DResource *pThis
);

#if 0 // patch DISABLED
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
    CONST D3DRECT  *pRects
);

// ******************************************************************
// * patch: D3DDevice_SetScreenSpaceOffset
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetScreenSpaceOffset)
(
    float_xt x,
    float_xt y
);

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
	D3DSTATEBLOCKTYPE Type,
	dword_xt			  *pToken
);

// ******************************************************************
// * patch: D3DDevice_InsertCallback
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_InsertCallback)
(
	X_D3DCALLBACKTYPE	Type,
	X_D3DCALLBACK		pCallback,
	dword_xt				Context
);

// ******************************************************************
// * patch: D3DDevice_DrawRectPatch
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_DrawRectPatch)
(
	uint_xt					Handle,
	CONST float_xt				*pNumSegs,
	CONST D3DRECTPATCH_INFO *pRectPatchInfo
);

// ******************************************************************
// * patch: D3DDevice_DrawTriPatch
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_DrawTriPatch)
(
	uint_xt					Handle,
	CONST float_xt				*pNumSegs,
	CONST D3DTRIPATCH_INFO* pTriPatchInfo
);

// ******************************************************************
// * patch: D3DDevice_GetProjectionViewportMatrix
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetProjectionViewportMatrix)
(
	D3DXMATRIX *pProjectionViewport
);

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
// * patch: CDevice_SetStateVB (D3D::CDevice::SetStateVB)
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(CDevice_SetStateVB)( xbox::ulong_xt Unknown1 );

// ******************************************************************
// * patch: CDevice_SetStateUP (D3D::CDevice::SetStateUP)
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(CDevice_SetStateUP)();

// ******************************************************************
// * patch: D3DDevice_SetStipple
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetStipple)( dword_xt* pPattern );

// ******************************************************************
// * patch: D3DDevice_SetSwapCallback
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetSwapCallback)
(
	X_D3DSWAPCALLBACK		pCallback
);

// ******************************************************************
// * patch: D3DDevice_PersistDisplay
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_PersistDisplay)();

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
// * patch: D3DDevice_PrimeVertexCache
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_PrimeVertexCache)
(
	uint_xt VertexCount,
	word_xt *pIndexData
);

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
// * patch: D3DDevice_SetModelView
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetModelView)
(
	CONST D3DMATRIX *pModelView, 
	CONST D3DMATRIX *pInverseModelView, 
	CONST D3DMATRIX *pComposite
);

// ******************************************************************
// * patch: D3DDevice_FlushVertexCache
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_FlushVertexCache)();

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
// * patch: D3DDevice_GetModelView
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_GetModelView)(D3DXMATRIX* pModelView);

// ******************************************************************
// * patch: D3DDevice_SetBackMaterial
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetBackMaterial)
(
	CONST X_D3DMATERIAL8 *pMaterial
);

#if 0 // patch disabled
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
// * patch: D3D_SetCommonDebugRegisters
// ******************************************************************
void WINAPI EMUPATCH(D3D_SetCommonDebugRegisters)();

// ******************************************************************
// * patch: D3D_BlockOnTime
// ******************************************************************
void WINAPI EMUPATCH(D3D_BlockOnTime)( dword_xt Unknown1, int Unknown2 );

// ******************************************************************
// * patch: D3D_BlockOnTime_4
//   One of the parameters (unknown which) passed in EAX
// ******************************************************************
void WINAPI EMUPATCH(D3D_BlockOnTime_4)( dword_xt Unknown1 );

// ******************************************************************
// * patch: D3D_BlockOnResource
// ******************************************************************
void WINAPI EMUPATCH(D3D_BlockOnResource)( X_D3DResource* pResource );

// ******************************************************************
// * patch: D3D_DestroyResource
// ******************************************************************
void WINAPI EMUPATCH(D3D_DestroyResource)( X_D3DResource* pResource );

// ******************************************************************
// * patch: D3D_DestroyResource__LTCG
// ******************************************************************
void WINAPI EMUPATCH(D3D_DestroyResource__LTCG)();


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
// * patch: D3DDevice_SetRenderTargetFast
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderTargetFast)
(
    X_D3DSurface	*pRenderTarget,
    X_D3DSurface	*pNewZStencil,
    dword_xt			Flags
);

// ******************************************************************
// * patch: D3DDevice_GetScissors
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetScissors)
(
	dword_xt	*pCount, 
	bool_xt	*pExclusive, 
	D3DRECT *pRects
);
// ******************************************************************
// * patch: D3DDevice_GetBackMaterial
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetBackMaterial)
(
	X_D3DMATERIAL8* pMaterial
);

// ******************************************************************
// * patch: D3D::LazySetPointParams
// ******************************************************************
void WINAPI EMUPATCH(D3D_LazySetPointParams)( void* Device );

} // end of namespace xbox

#endif
