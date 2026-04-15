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

extern IDirect3DDevice *g_pD3DDevice;

#ifdef CXBX_USE_D3D11
extern ID3D11DeviceContext *g_pD3DDeviceContext;
#endif

void LookupTrampolinesD3D();

// initialize render window
extern void CxbxInitWindow();

void CxbxUpdateNativeD3DResources();

// Unified shader constant helpers (dispatch to D3D9 or D3D11 internally)
void CxbxSetVertexShaderConstantF(UINT startRegister, const float* pConstantData, UINT Vector4fCount);
void CxbxSetPixelShaderConstantF(UINT startRegister, const float* pConstantData, UINT Vector4fCount);

// BeginScene/EndScene: no-ops in D3D11
inline void CxbxBeginScene()
{
#ifndef CXBX_USE_D3D11
	g_pD3DDevice->BeginScene();
#endif
}

inline void CxbxEndScene()
{
#ifndef CXBX_USE_D3D11
	g_pD3DDevice->EndScene();
#endif
}

// Query helpers: dispatch Issue/GetData calls to D3D9 or D3D11 patterns
inline void CxbxQueryIssueBegin(IDirect3DQuery* pQuery)
{
#ifdef CXBX_USE_D3D11
	g_pD3DDeviceContext->Begin(pQuery);
#else
	pQuery->Issue(D3DISSUE_BEGIN);
#endif
}

inline void CxbxQueryIssueEnd(IDirect3DQuery* pQuery)
{
#ifdef CXBX_USE_D3D11
	g_pD3DDeviceContext->End(pQuery);
#else
	pQuery->Issue(D3DISSUE_END);
#endif
}

inline HRESULT CxbxQueryGetData(IDirect3DQuery* pQuery, void* pData, DWORD dwSize, DWORD dwGetDataFlags)
{
#ifdef CXBX_USE_D3D11
	return g_pD3DDeviceContext->GetData(pQuery, pData, dwSize, dwGetDataFlags);
#else
	return pQuery->GetData(pData, dwSize, dwGetDataFlags);
#endif
}

void CxbxImpl_SetRenderTarget(xbox::X_D3DSurface* pRenderTarget, xbox::X_D3DSurface* pNewZStencil);
void CxbxImpl_SetViewport(xbox::X_D3DVIEWPORT8* pViewport);

// initialize direct3d
extern void EmuD3DInit();

// cleanup direct3d
extern void EmuD3DCleanup();

extern xbox::dword_xt g_Xbox_VertexShader_Handle;

extern xbox::X_PixelShader *g_pXbox_PixelShader;

extern EMUFORMAT g_HostTextureFormats[xbox::X_D3DTS_STAGECOUNT];

extern xbox::X_D3DBaseTexture *g_pXbox_SetTexture[xbox::X_D3DTS_STAGECOUNT];


// EMUPATCH declarations split into topic-specific headers:
#include "EmuPatches_Device.h"
#include "EmuPatches_Draw.h"
#include "EmuPatches_Misc.h"
#include "EmuPatches_Shader.h"
#include "EmuPatches_State.h"
#include "EmuPatches_Surface.h"
#include "EmuPatches_Unused.h"

#endif // DIRECT3D9_H
