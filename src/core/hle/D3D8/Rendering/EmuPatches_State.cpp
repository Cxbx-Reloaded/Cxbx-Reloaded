// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
// *  All rights reserved
// *
// ******************************************************************
#include "EmuD3D8_common.h"
#include "IndexBufferConvert.h"

// Variables only used in EmuPatches_State.cpp
static xbox::X_D3DBaseTexture CxbxActiveTextureCopies[xbox::X_D3DTS_STAGECOUNT] = {}; // Set by D3DDevice_SwitchTexture. Cached active texture

// Forward declarations (defined later in this file)
static thread_local uint32_t setTransformCount;
void CxbxImpl_SetTransform(xbox::X_D3DTRANSFORMSTATETYPE State, CONST xbox::X_D3DMATRIX *pMatrix);

xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetBackBufferScale)(float_xt x, float_xt y)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_END;

	g_Xbox_BackbufferScaleX = x;
	g_Xbox_BackbufferScaleY = y;
}

// ******************************************************************
// * patch: D3DDevice_SetGammaRamp
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetGammaRamp)
(
   	dword_xt                   dwFlags,
   	CONST X_D3DGAMMARAMP      *pRamp
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(dwFlags)
		LOG_FUNC_ARG(pRamp)
		LOG_FUNC_END;

#ifdef CXBX_USE_D3D11
	// Use IDXGIOutput::SetGammaControl for D3D11
	if (g_pSwapChain) {
		IDXGIOutput* pOutput = nullptr;
		if (SUCCEEDED(g_pSwapChain->GetContainingOutput(&pOutput))) {
			DXGI_GAMMA_CONTROL gammaControl = {};
			gammaControl.Scale = { 1.0f, 1.0f, 1.0f };
			gammaControl.Offset = { 0.0f, 0.0f, 0.0f };
			for (int v = 0; v < 256; v++) {
				float idx = v / 255.0f * 1024.0f;
				int i = static_cast<int>(idx);
				if (i > 1024) i = 1024;
				gammaControl.GammaCurve[i] = {
					pRamp->red[v] / 255.0f,
					pRamp->green[v] / 255.0f,
					pRamp->blue[v] / 255.0f
				};
			}
			// Interpolate any gaps in the 1025-entry curve
			for (int i = 1; i < 1025; i++) {
				if (gammaControl.GammaCurve[i].Red == 0.0f &&
					gammaControl.GammaCurve[i].Green == 0.0f &&
					gammaControl.GammaCurve[i].Blue == 0.0f && i < 1024) {
					gammaControl.GammaCurve[i] = gammaControl.GammaCurve[i - 1];
				}
			}
			pOutput->SetGammaControl(&gammaControl);
			pOutput->Release();
		}
	}
#elif 0 // TODO : Why is this disabled?
   	// remove D3DSGR_IMMEDIATE
   	DWORD dwPCFlags = dwFlags & (~0x00000002);
   	D3DGAMMARAMP PCRamp;

   	for(int v=0;v<255;v++)
   	{
   	   	PCRamp.red[v]   = pRamp->red[v];
   	   	PCRamp.green[v] = pRamp->green[v];
   	   	PCRamp.blue[v]  = pRamp->blue[v];
   	}

	g_pD3DDevice->SetGammaRamp(
		0, // iSwapChain
		dwPCFlags, &PCRamp);
#endif
}

// ******************************************************************
// * patch: D3DDevice_GetGammaRamp
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_GetGammaRamp)
(
   	X_D3DGAMMARAMP     *pRamp
)
{
	LOG_FUNC_ONE_ARG(pRamp);

#ifdef CXBX_USE_D3D11
   	// Use IDXGIOutput::GetGammaControl to retrieve the current gamma ramp
   	bool gotGamma = false;
   	if (g_pSwapChain) {
   	   	IDXGIOutput* pOutput = nullptr;
   	   	if (SUCCEEDED(g_pSwapChain->GetContainingOutput(&pOutput))) {
   	   	   	DXGI_GAMMA_CONTROL gammaControl = {};
   	   	   	if (SUCCEEDED(pOutput->GetGammaControl(&gammaControl))) {
   	   	   	   	for (int v = 0; v < 256; v++) {
   	   	   	   	   	int i = static_cast<int>(v / 255.0f * 1024.0f);
   	   	   	   	   	if (i > 1024) i = 1024;
   	   	   	   	   	pRamp->red[v]   = static_cast<BYTE>(gammaControl.GammaCurve[i].Red * 255.0f);
   	   	   	   	   	pRamp->green[v] = static_cast<BYTE>(gammaControl.GammaCurve[i].Green * 255.0f);
   	   	   	   	   	pRamp->blue[v]  = static_cast<BYTE>(gammaControl.GammaCurve[i].Blue * 255.0f);
   	   	   	   	}
   	   	   	   	gotGamma = true;
   	   	   	}
   	   	   	pOutput->Release();
   	   	}
   	}
   	if (!gotGamma) {
   	   	// Fallback: return a linear ramp
   	   	for (int v = 0; v < 256; v++) {
   	   	   	pRamp->red[v]   = (BYTE)v;
   	   	   	pRamp->green[v] = (BYTE)v;
   	   	   	pRamp->blue[v]  = (BYTE)v;
   	   	}
   	}
#else
   	D3DGAMMARAMP *pGammaRamp = (D3DGAMMARAMP *)malloc(sizeof(D3DGAMMARAMP));

   	g_pD3DDevice->GetGammaRamp(
		0, // iSwapChain
		pGammaRamp);

   	for(int v=0;v<256;v++)
   	{
   	   	pRamp->red[v] = (BYTE)pGammaRamp->red[v];
   	   	pRamp->green[v] = (BYTE)pGammaRamp->green[v];
   	   	pRamp->blue[v] = (BYTE)pGammaRamp->blue[v];
   	}

	free(pGammaRamp);
#endif
}


#define COPY_BACKBUFFER_TO_XBOX_SURFACE // Uncomment to enable writing Host Backbuffers back to Xbox surfaces
xbox::X_D3DSurface* CxbxrImpl_GetBackBuffer2
(
   	xbox::int_xt BackBuffer
)
{
	xbox::X_D3DSurface* pXboxBackBuffer = nullptr;


#ifndef COPY_BACKBUFFER_TO_XBOX_SURFACE
	/** unsafe, somehow
	HRESULT hRet = D3D_OK;

	X_D3DSurface *pXboxBackBuffer = EmuNewD3DSurface();

	if(BackBuffer == -1) {
		static IDirect3DSurface *pCachedPrimarySurface = nullptr;

		EMUFORMAT PCFormat = EMUFMT_A8R8G8B8;
		if(pCachedPrimarySurface == nullptr) {
			// create a buffer to return
			// TODO: Verify the surface is always 640x480
			hRet = g_pD3DDevice->CreateOffscreenPlainSurface(640, 480, EMUFMT_A8R8G8B8, /*D3DPool=* /0, &pCachedPrimarySurface, nullptr);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateOffscreenPlainSurface");
		}

		int iTextureStage = -1; // No iTextureStage!
		SetHostResource(pXboxBackBuffer, (IDirect3DResource*)pCachedPrimarySurface, iTextureStage, 0, PCFormat);


		hRet = g_pD3DDevice->GetFrontBuffer(pCachedPrimarySurface);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetFrontBuffer");

		if (FAILED(hRet)) {
			EmuLog(LOG_LEVEL::WARNING, "Could not retrieve primary surface, using backbuffer");
			SetHostResource(pXboxBackBuffer, nullptr); // No iTextureStage!
			pCachedPrimarySurface->Release();
			pCachedPrimarySurface = nullptr;
			BackBuffer = 0;
		}

		// Debug: Save this image temporarily
		//D3DXSaveSurfaceToFile("C:\\Aaron\\Textures\\FrontBuffer.bmp", D3DXIFF_BMP, GetHostSurface(pXboxBackBuffer), nullptr, nullptr);
	}

	if(BackBuffer != -1) {
		hRet = g_pD3DDevice->GetBackBuffer(
			0, // iSwapChain
			BackBuffer, D3DBACKBUFFER_TYPE_MONO, &pCachedPrimarySurface);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetBackBuffer");
	}
	//*/

	static X_D3DSurface *pXboxBackBuffer = EmuNewD3DSurface();
	IDirect3DSurface *pCurrentHostBackBuffer = nullptr;

	 if (BackBuffer == -1) {
		 BackBuffer = 0;
	 }

	HRESULT hRet = g_pD3DDevice->GetBackBuffer(
		0, // iSwapChain
		BackBuffer, D3DBACKBUFFER_TYPE_MONO, &pCurrentHostBackBuffer);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetBackBuffer");

	if (FAILED(hRet))
		CxbxrAbort("Unable to retrieve back buffer");

	SetHostResource(pXboxBackBuffer, (IDirect3DResource*)pCurrentHostBackBuffer); // No iTextureStage! TODO : Pass in D3DUsage, PCFormat

	// Increment reference count
	pXboxBackBuffer->Common++; // EMUPATCH(D3DResource_AddRef)(pXboxBackBuffer);

	return pXboxBackBuffer;
#else // COPY_BACKBUFFER_TO_XBOX_SURFACE
	// Rather than create a new surface, we should forward to the Xbox version of GetBackBuffer,
	// This gives us the correct Xbox surface to update.
	// We get signatures for both backbuffer functions as it changed in later XDKs

	// This also updates the reference count, so we don't need to do this ourselves
	if (XB_TRMP(D3DDevice_GetBackBuffer) != nullptr) {
		XB_TRMP(D3DDevice_GetBackBuffer)(BackBuffer, xbox::X_D3DBACKBUFFER_TYPE_MONO, &pXboxBackBuffer);
	}
	else if (XB_TRMP(D3DDevice_GetBackBuffer_8__LTCG_eax1) != nullptr) {
		__asm {
			lea  eax, pXboxBackBuffer
			push eax
			push D3DBACKBUFFER_TYPE_MONO
			mov  eax, BackBuffer
			call XB_TRMP(D3DDevice_GetBackBuffer_8__LTCG_eax1)
		}
	}
	else if (XB_TRMP(D3DDevice_GetBackBuffer2) != nullptr) {
		pXboxBackBuffer = XB_TRMP(D3DDevice_GetBackBuffer2)(BackBuffer);
	}
	else {
		__asm {
			mov  eax, BackBuffer
			call XB_TRMP(D3DDevice_GetBackBuffer2_0__LTCG_eax1)
			mov  pXboxBackBuffer, eax
		}
	}

	// Now pXboxBackbuffer points to the requested Xbox backbuffer
	if (pXboxBackBuffer == nullptr) {
		CxbxrAbort("D3DDevice_GetBackBuffer2: Could not get Xbox backbuffer");
	}


   	// HACK: Disabled: Enabling this breaks DOA3 at native res/without hacks+
   	// Also likely to effect Other games, but it has no known benefit at this point in time
   	// There are currently no known games that depend on backbuffer readback on the CPU!
#if 0
	// TODO: Downscale the host surface to the same size as the Xbox surface during copy
	// Otherwise, we will overflow memory and crash
	// HACK: For now, when using a non-zero scale factor, we can just skip the copy to prevent a crash
	if (g_RenderScaleFactor == 1) {
		auto pCopySrcSurface = GetHostSurface(pXboxBackBuffer, D3DUSAGE_RENDERTARGET);
		if (pCopySrcSurface == nullptr) {
			EmuLog(LOG_LEVEL::WARNING, "Failed to get Host Resource for Xbox Back Buffer");
			return pXboxBackBuffer;
		}

		D3DLOCKED_RECT copyLockedRect;
		HRESULT hRet = pCopySrcSurface->LockRect(&copyLockedRect, NULL, D3DLOCK_READONLY);
		if (hRet != D3D_OK) {
			EmuLog(LOG_LEVEL::WARNING, "Could not lock Host Resource for Xbox Back Buffer");
			return pXboxBackBuffer;
		}

		D3DSurfaceDesc copySurfaceDesc;
		hRet = pCopySrcSurface->GetDesc(&copySurfaceDesc);
		if (hRet != D3D_OK) {
			EmuLog(LOG_LEVEL::WARNING, "Could not get Xbox Back Buffer Host Surface Desc");
		}
		else {
			DWORD Size = copyLockedRect.Pitch * copySurfaceDesc.Height; // TODO : What about mipmap levels? (Backbuffer does not support mipmap)
			// Finally, do the copy from the converted host resource to the xbox resource
			memcpy((void*)GetDataFromXboxResource(pXboxBackBuffer), copyLockedRect.pBits, Size);
		}

		pCopySrcSurface->UnlockRect();
	}
#endif

	return pXboxBackBuffer;
#endif // COPY_BACKBUFFER_TO_XBOX_SURFACE
}

// ******************************************************************
// * patch: D3DDevice_SetViewport
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetViewport)
(
	X_D3DVIEWPORT8 *pViewport
)
{
	LOG_FUNC_ONE_ARG(pViewport);

	// Always call the Xbox SetViewPort to update D3D Internal State
	XB_TRMP(D3DDevice_SetViewport)(pViewport);

	CxbxImpl_SetViewport(pViewport);
}

// Set the viewport
void CxbxImpl_SetViewport(xbox::X_D3DVIEWPORT8* pViewport)
{
	LOG_INIT;

	// Unclear what to do when no viewport is passed
	// Set the default viewport?
	// Clamp the current viewport to the current rendertarget?
	if (pViewport == nullptr) {
		LOG_TEST_CASE("pViewport = null");
		return;
	}

	// We need a rendertarget to clamp the viewport
	// Pretty much everything hits this on boot because of the way our SetRenderTarget patch works
	if (!g_pXbox_RenderTarget) {
		return;
	}

	float rendertargetBaseWidth;
	float rendertargetBaseHeight;
	GetRenderTargetBaseDimensions(rendertargetBaseWidth, rendertargetBaseHeight);

	// Update the current viewport
	g_Xbox_Viewport = *pViewport;

	// The SetRenderTarget trampoline calls SetViewport with
	// both Width and Height set to INT_MAX
	if ((pViewport->Width == INT_MAX) ^ (pViewport->Height == INT_MAX)) {
		LOG_TEST_CASE("SetViewport called with only one of width/height set to INT_MAX");
	}

	// Cap width and height to screen bounds
	// Test case: Need for Speed: HP2 (rear view mirror)
	g_Xbox_Viewport.Width = std::min(g_Xbox_Viewport.Width, (DWORD)rendertargetBaseWidth - g_Xbox_Viewport.X);
	g_Xbox_Viewport.Height = std::min(g_Xbox_Viewport.Height, (DWORD)rendertargetBaseHeight - g_Xbox_Viewport.Y);
}

// LTCG specific D3DDevice_SetShaderConstantMode function...
// This uses a custom calling convention where parameter is passed in EAX
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetShaderConstantMode_0__LTCG_eax1)
(
)
{
   	X_VERTEXSHADERCONSTANTMODE Mode;
   	__asm {
   	   	LTCG_PROLOGUE
   	   	mov  Mode, eax
   	}

   	EMUPATCH(D3DDevice_SetShaderConstantMode)(Mode);

   	__asm {
   	   	LTCG_EPILOGUE
   	   	ret
   	}
}

// ******************************************************************
// * patch: D3DDevice_SetTexture
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetTexture)
(
   	dword_xt           Stage,
	X_D3DBaseTexture  *pTexture
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Stage)
		LOG_FUNC_ARG(pTexture)
		LOG_FUNC_END;

	// Call the Xbox implementation of this function, to properly handle reference counting for us
	XB_TRMP(D3DDevice_SetTexture)(Stage, pTexture);

	g_pXbox_SetTexture[Stage] = pTexture;
}

// ******************************************************************
// * patch: D3DDevice_SwitchTexture
// ******************************************************************
xbox::void_xt __fastcall xbox::EMUPATCH(D3DDevice_SwitchTexture)
(
   	dword_xt           Method,
   	dword_xt           Data,
   	dword_xt           Format
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Method)
		LOG_FUNC_ARG(Data)
		LOG_FUNC_ARG(Format)
		LOG_FUNC_END;

   	DWORD Stage = -1;

	switch (Method) { // Detect which of the 4 (X_D3DTS_STAGECOUNT) texture stages is given by the (NV2A) Method argument
	// This code contains D3DPUSH_ENCODE(NV2A_TX_OFFSET(v), 2) = 2 DWORD's, shifted left PUSH_COUNT_SHIFT (18) left
	case 0x00081b00: Stage = 0; break;
	case 0x00081b40: Stage = 1; break;
	case 0x00081b80: Stage = 2; break;
	case 0x00081bc0: Stage = 3; break;
	default:
		LOG_TEST_CASE("D3DDevice_SwitchTexture Unknown Method");
   	   	EmuLog(LOG_LEVEL::WARNING, "Unknown Method (0x%.08X)", Method);
	}

   	if (Stage >= 0) {
		// Switch Texture updates the data pointer of an active texture using pushbuffer commands
		if (g_pXbox_SetTexture[Stage] == xbox::zeroptr) {
			LOG_TEST_CASE("D3DDevice_SwitchTexture without an active texture");
		}
		else {
			//LOG_TEST_CASE("Using CxbxActiveTextureCopies");
			// See https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/1159
			// Test-case : Arena Football
			// Test-case : Call of Duty 2: Big Red One
			// Test-case : Crimson Skies
			// Test-case : Freedom Fighters - see https://www.youtube.com/watch?v=_NDCoLY8V3I
			// Test-case : Freestyle MetalX
			// Test-case : GENMA ONIMUSHA
			// Test-case : Gun
			// Test-case : Harry Potter : Quidditch World Cup
			// Test-case : King Arthur
			// Test-case : Madden NFL 2002
			// Test-case : Madden NFL 2005
			// Test-case : Madden NFL 07
			// Test-case : Need For Speed Most Wanted
			// Test-case : Need For Speed Underground
			// Test-case : PocketBike Racer
			// Test-case : Project Gotham Racing 2
			// Test-case : Richard Burns Rally
			// Test-case : Spider - Man 2

			// Update data and format separately, instead of via GetDataFromXboxResource()
			CxbxActiveTextureCopies[Stage].Common = g_pXbox_SetTexture[Stage]->Common;
			CxbxActiveTextureCopies[Stage].Data = Data;
			CxbxActiveTextureCopies[Stage].Format = Format;
			CxbxActiveTextureCopies[Stage].Lock = 0;
			CxbxActiveTextureCopies[Stage].Size = g_pXbox_SetTexture[Stage]->Size;

			// Use the above modified copy, instead of altering the active Xbox texture
			g_pXbox_SetTexture[Stage] = &CxbxActiveTextureCopies[Stage];
			// Note : Since g_pXbox_SetTexture and CxbxActiveTextureCopies are host-managed,
			// Xbox code should never alter these members (so : no reference counting, etc).
			// As long as that's guaranteed, this is a safe way to emulate SwitchTexture.
			// (GetHostResourceKey also avoids using any Xbox texture resource memory address.)
		}
   	}
}

// ******************************************************************
// * patch: D3DDevice_SetTransform
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetTransform)
(
	X_D3DTRANSFORMSTATETYPE State,
   	CONST X_D3DMATRIX      *pMatrix
)
{
   	LOG_FUNC_BEGIN
   	   	LOG_FUNC_ARG(State)
   	   	LOG_FUNC_ARG(pMatrix)
   	   	LOG_FUNC_END;

   	setTransformCount++;

   	// Trampoline to guest code to remove the need for a GetTransform patch
   	XB_TRMP(D3DDevice_SetTransform)(State, pMatrix);
   	CxbxImpl_SetTransform(State, pMatrix);
}

static void CxbxrImpl_MultiplyTransform(
	xbox::X_D3DTRANSFORMSTATETYPE State,
	CONST xbox::X_D3DMATRIX      *pMatrix
)
{
	setTransformCount = 0;

	// Trampoline to guest code, which we expect to call SetTransform
	// If we find a case where the trampoline doesn't call SetTransform
	// (or we can't detect the call) we will need to implement this
	if (XB_TRMP(D3DDevice_MultiplyTransform) != nullptr) {
		XB_TRMP(D3DDevice_MultiplyTransform)(State, pMatrix);
	}
	else {
		__asm {
			mov  eax, pMatrix
			mov  ebx, State
			call XB_TRMP(D3DDevice_MultiplyTransform_0__LTCG_ebx1_eax2)
		}
	}

	if (setTransformCount == 0) {
		LOG_TEST_CASE("MultiplyTransform did not appear to call SetTransform");
	}
}

// ******************************************************************
// * patch: D3DDevice_MultiplyTransform
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_MultiplyTransform)
(
   	X_D3DTRANSFORMSTATETYPE State,
   	CONST X_D3DMATRIX      *pMatrix
)
{
   	LOG_FUNC_BEGIN
   	   	LOG_FUNC_ARG(State)
   	   	LOG_FUNC_ARG(pMatrix)
   	   	LOG_FUNC_END;

	CxbxrImpl_MultiplyTransform(State, pMatrix);
}

// ******************************************************************
// * patch: D3DDevice_MultiplyTransform_0__LTCG_ebx1_eax2
// ******************************************************************
// Overload for logging
static void D3DDevice_MultiplyTransform_0__LTCG_ebx1_eax2(
   	xbox::X_D3DTRANSFORMSTATETYPE State,
   	CONST xbox::X_D3DMATRIX      *pMatrix
)
{
   	LOG_FUNC_BEGIN
   	   	LOG_FUNC_ARG(State)
   	   	LOG_FUNC_ARG(pMatrix)
   	   	LOG_FUNC_END;
}

// This uses a custom calling convention where parameter is passed in EBX, EAX
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_MultiplyTransform_0__LTCG_ebx1_eax2)()
{
	xbox::X_D3DTRANSFORMSTATETYPE State;
   	xbox::X_D3DMATRIX *pMatrix;
   	__asm {
   	   	LTCG_PROLOGUE
   	   	mov  State, eax
   	   	mov  pMatrix, ebx
   	}

   	// Log
   	D3DDevice_MultiplyTransform_0__LTCG_ebx1_eax2(State, pMatrix);

	CxbxrImpl_MultiplyTransform(State, pMatrix);

	__asm {
		LTCG_EPILOGUE
		ret
	}
}

// ******************************************************************
// * patch: D3DDevice_SetStreamSource
// ******************************************************************

// Overload for logging
static void D3DDevice_SetStreamSource_0__LTCG_eax1_edi2_ebx3
(
   	xbox::uint_xt            StreamNumber,
   	xbox::X_D3DVertexBuffer *pStreamData,
   	xbox::uint_xt            Stride
)
{
   	LOG_FUNC_BEGIN
   	   	LOG_FUNC_ARG(StreamNumber)
   	   	LOG_FUNC_ARG(pStreamData)
   	   	LOG_FUNC_ARG(Stride)
   	   	LOG_FUNC_END;
}

// LTCG specific D3DDevice_SetStreamSource function...
// This uses a custom calling convention where parameters are passed in EAX, EDI, EBX
// Test-case: Juiced
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetStreamSource_0__LTCG_eax1_edi2_ebx3)()
{
   	uint_xt StreamNumber;
   	X_D3DVertexBuffer *pStreamData;
   	uint_xt Stride;
   	__asm {
   	   	LTCG_PROLOGUE
   	   	mov  StreamNumber, eax
   	   	mov  pStreamData, edi
   	   	mov  Stride, ebx
   	}

   	// Log
   	D3DDevice_SetStreamSource_0__LTCG_eax1_edi2_ebx3(StreamNumber, pStreamData, Stride);

   	CxbxImpl_SetStreamSource(StreamNumber, pStreamData, Stride);

   	__asm {
   	   	mov  eax, StreamNumber
   	   	mov  edi, pStreamData
   	   	mov  ebx, Stride
   	   	call XB_TRMP(D3DDevice_SetStreamSource_0__LTCG_eax1_edi2_ebx3)

   	   	LTCG_EPILOGUE
   	   	ret
   	}
}

// Overload for logging
static void D3DDevice_SetStreamSource_4__LTCG_eax1_ebx2
(
   	xbox::uint_xt            StreamNumber,
   	xbox::X_D3DVertexBuffer *pStreamData,
   	xbox::uint_xt            Stride
)
{
   	LOG_FUNC_BEGIN
   	   	LOG_FUNC_ARG(StreamNumber)
   	   	LOG_FUNC_ARG(pStreamData)
   	   	LOG_FUNC_ARG(Stride)
   	   	LOG_FUNC_END;
}

// LTCG specific D3DDevice_SetStreamSource function...
// This uses a custom calling convention where parameter is passed in EAX, EBX
// Test-case: Ninja Gaiden
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetStreamSource_4__LTCG_eax1_ebx2)
(
   	uint_xt                Stride
)
{
   	uint_xt StreamNumber;
   	X_D3DVertexBuffer *pStreamData;
   	__asm {
   	   	LTCG_PROLOGUE
   	   	mov  StreamNumber, eax
   	   	mov  pStreamData, ebx
   	}

   	// Log
   	D3DDevice_SetStreamSource_4__LTCG_eax1_ebx2(StreamNumber, pStreamData, Stride);

   	CxbxImpl_SetStreamSource(StreamNumber, pStreamData, Stride);

   	// Forward to Xbox implementation
   	// This should stop us having to patch GetStreamSource!
   	__asm {
   	   	push Stride
   	   	mov  ebx, pStreamData
   	   	mov  eax, StreamNumber
   	   	call XB_TRMP(D3DDevice_SetStreamSource_4__LTCG_eax1_ebx2)

   	   	LTCG_EPILOGUE
   	   	ret  4
   	}
}

// Overload for logging
static void D3DDevice_SetStreamSource_8__LTCG_eax1
(
   	xbox::uint_xt            StreamNumber,
   	xbox::X_D3DVertexBuffer *pStreamData,
   	xbox::uint_xt            Stride
)
{
   	LOG_FUNC_BEGIN
   	   	LOG_FUNC_ARG(StreamNumber)
   	   	LOG_FUNC_ARG(pStreamData)
   	   	LOG_FUNC_ARG(Stride)
   	   	LOG_FUNC_END;
}

// This uses a custom calling convention where parameter is passed in EAX
// Test-case: Superman - The Man Of Steel
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetStreamSource_8__LTCG_eax1)
(
   	X_D3DVertexBuffer  *pStreamData,
   	uint_xt             Stride
)
{
   	uint_xt StreamNumber;
   	__asm {
   	   	LTCG_PROLOGUE
   	   	mov  StreamNumber, eax
   	}

   	// Log
   	D3DDevice_SetStreamSource_8__LTCG_eax1(StreamNumber, pStreamData, Stride);

   	CxbxImpl_SetStreamSource(StreamNumber, pStreamData, Stride);

   	// Forward to Xbox implementation
   	// This should stop us having to patch GetStreamSource!
   	__asm {
   	   	push Stride
   	   	push pStreamData
   	   	mov  eax, StreamNumber
   	   	call XB_TRMP(D3DDevice_SetStreamSource_8__LTCG_eax1)

   	   	LTCG_EPILOGUE
   	   	ret  8
   	}
}

// This uses a custom calling convention where StreamNumber parameter is passed in EDX
// Test-case: NASCAR Heat 2002
xbox::void_xt __fastcall xbox::EMUPATCH(D3DDevice_SetStreamSource_8__LTCG_edx1)
(
   	void*,
   	uint_xt                StreamNumber,
   	X_D3DVertexBuffer  *pStreamData,
   	uint_xt                Stride
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(StreamNumber)
		LOG_FUNC_ARG(pStreamData)
		LOG_FUNC_ARG(Stride)
		LOG_FUNC_END;

	CxbxImpl_SetStreamSource(StreamNumber, pStreamData, Stride);

	// Forward to Xbox implementation
	// This should stop us having to patch GetStreamSource!
	XB_TRMP(D3DDevice_SetStreamSource_8__LTCG_edx1)(nullptr, StreamNumber, pStreamData, Stride);
}

// ******************************************************************
// * patch: D3DDevice_SetStreamSource
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetStreamSource)
(
   	uint_xt                StreamNumber,
   	X_D3DVertexBuffer  *pStreamData,
   	uint_xt                Stride
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(StreamNumber)
		LOG_FUNC_ARG(pStreamData)
		LOG_FUNC_ARG(Stride)
		LOG_FUNC_END;

	CxbxImpl_SetStreamSource(StreamNumber, pStreamData, Stride);

	// Forward to Xbox implementation
	// This should stop us having to patch GetStreamSource!
	XB_TRMP(D3DDevice_SetStreamSource)(StreamNumber, pStreamData, Stride);
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShader
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetVertexShader)
(
   	dword_xt Handle
)
{
	LOG_FUNC_ONE_ARG(Handle);

	// This trampoline leads to calling D3DDevice_LoadVertexShader and D3DDevice_SelectVertexShader
	// Please raise the alarm if this is ever not the case
	XB_TRMP(D3DDevice_SetVertexShader)(Handle);

	CxbxImpl_SetVertexShader(Handle);
}

// Overload for logging
static void D3DDevice_SetVertexShader_0__LTCG_ebx1
(
   	xbox::dword_xt Handle
)
{
	LOG_FUNC_ONE_ARG(Handle);
}

// This uses a custom calling convention where Handle is passed in EBX
// Test-case: NASCAR Heat 2002
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetVertexShader_0__LTCG_ebx1)()
{
	dword_xt Handle;
	__asm {
		LTCG_PROLOGUE
		mov  Handle, ebx
	}

	// Log
	D3DDevice_SetVertexShader_0__LTCG_ebx1(Handle);

	// This trampoline leads to calling D3DDevice_LoadVertexShader and D3DDevice_SelectVertexShader
	// Please raise the alarm if this is ever not the case
	__asm {
		mov  ebx, Handle
		call XB_TRMP(D3DDevice_SetVertexShader_0__LTCG_ebx1)
	}

	CxbxImpl_SetVertexShader(Handle);

	__asm {
		LTCG_EPILOGUE
		ret
	}
}

// ******************************************************************
// * patch: D3DDevice_SetLight
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_SetLight)
(
   	dword_xt            Index,
   	CONST X_D3DLIGHT8 *pLight
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Index)
		LOG_FUNC_ARG(pLight)
		LOG_FUNC_END;

	xbox::hresult_xt hRet = XB_TRMP(D3DDevice_SetLight)(Index, pLight);

	d3d8LightState.Lights[Index] = *pLight;

   	return hRet;
}

// ******************************************************************
// * patch: D3DDevice_SetMaterial
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetMaterial)
(
   	CONST X_D3DMATERIAL8 *pMaterial
)
{
	LOG_FUNC_ONE_ARG(pMaterial);

	XB_TRMP(D3DDevice_SetMaterial)(pMaterial);

	ffShaderState.Materials[0].Ambient = toVector(pMaterial->Ambient);
	ffShaderState.Materials[0].Diffuse = toVector(pMaterial->Diffuse);
	ffShaderState.Materials[0].Specular = toVector(pMaterial->Specular);
	ffShaderState.Materials[0].Emissive = toVector(pMaterial->Emissive);
	ffShaderState.Materials[0].Power = pMaterial->Power;
}

// ******************************************************************
// * patch: D3DDevice_SetBackMaterial
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetBackMaterial)
(
	CONST X_D3DMATERIAL8 *pMaterial
)
{
	LOG_FUNC_ONE_ARG(pMaterial);

	XB_TRMP(D3DDevice_SetBackMaterial)(pMaterial);

	ffShaderState.Materials[1].Ambient = toVector(pMaterial->Ambient);
	ffShaderState.Materials[1].Diffuse = toVector(pMaterial->Diffuse);
	ffShaderState.Materials[1].Specular = toVector(pMaterial->Specular);
	ffShaderState.Materials[1].Emissive = toVector(pMaterial->Emissive);
	ffShaderState.Materials[1].Power = pMaterial->Power;
}

static HRESULT CxbxrImpl_LightEnable(xbox::dword_xt Index, xbox::bool_xt bEnable)
{
	LOG_INIT;

	d3d8LightState.EnableLight(Index, bEnable);

#ifdef CXBX_USE_D3D11
	// Under D3D11, LightEnable relies on our fixed function shader
#else
	HRESULT hRet = g_pD3DDevice->LightEnable(Index, bEnable);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->LightEnable");
#endif

	return _9_11(hRet, S_OK);
}

// ******************************************************************
// * patch: D3DDevice_LightEnable
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_LightEnable)
(
   	dword_xt Index,
   	bool_xt  bEnable
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Index)
		LOG_FUNC_ARG(bEnable)
		LOG_FUNC_END;

	xbox::hresult_xt hRet = XB_TRMP(D3DDevice_LightEnable)(Index, bEnable);

	d3d8LightState.EnableLight(Index, bEnable);
	// Note : LightEnable is handled in our fixed function shader  - see UpdateFixedFunctionVertexShaderState()

   	return hRet;
}

// ******************************************************************
// * patch: D3DDevice_LightEnable_4__LTCG_eax1
// ******************************************************************
// Overload for logging
static void D3DDevice_LightEnable_4__LTCG_eax1
(
   	xbox::dword_xt Index,
   	xbox::bool_xt  bEnable
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Index)
		LOG_FUNC_ARG(bEnable)
		LOG_FUNC_END;
}

// This uses a custom calling convention where parameter is passed in EAX
__declspec(naked) xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_LightEnable_4__LTCG_eax1)
(
   	bool_xt bEnable
)
{
	xbox::dword_xt Index;
	HRESULT hRet;
	__asm {
		LTCG_PROLOGUE
		mov  Index, eax
	}

	// Log
	D3DDevice_LightEnable_4__LTCG_eax1(Index, bEnable);

	__asm {
		push bEnable
		mov  eax, Index
		call XB_TRMP(D3DDevice_LightEnable_4__LTCG_eax1)
	}

	hRet = CxbxrImpl_LightEnable(Index, bEnable);

	__asm {
		mov  eax, hRet
		LTCG_EPILOGUE
		ret  4
	}
}

// SetRenderTarget can call CommonSetRenderTarget, nested call detection is required
// Test case: Midtown Madness 3
static thread_local uint32_t setRenderTargetCount = 0;

void CxbxImpl_SetRenderTarget
(
   	xbox::X_D3DSurface    *pRenderTarget,
   	xbox::X_D3DSurface    *pNewZStencil
)
{
	LOG_INIT;

	IDirect3DSurface *pHostRenderTarget = nullptr;
	IDirect3DSurface *pHostDepthStencil = nullptr;
	// In Xbox titles, CreateDevice calls SetRenderTarget for the back buffer
	// We can use this to determine the Xbox backbuffer surface for later use!
	if (g_pXbox_BackBufferSurface == xbox::zeroptr) {
		g_pXbox_BackBufferSurface = pRenderTarget;
		// TODO : Some titles might render to another backbuffer later on,
		// if that happens, we might need to skip the first one or two calls?
	}

	// In Xbox titles, CreateDevice calls SetRenderTarget (our caller) for the depth stencil
	// We can use this to determine the Xbox depth stencil surface for later use!
   	if (g_pXbox_DefaultDepthStencilSurface == xbox::zeroptr) {
   	   	g_pXbox_DefaultDepthStencilSurface = pNewZStencil;
		// TODO : Some titles might set another depth stencil later on,
		// if that happens, we might need to skip the first one or two calls?
   	}

	// The current render target is only replaced if it's passed in here non-null
	if (pRenderTarget != xbox::zeroptr) {
		g_pXbox_RenderTarget = pRenderTarget;
	}
	else {
		// If non is given, use the current Xbox render target
		pRenderTarget = g_pXbox_RenderTarget;
		// If there's no Xbox render target yet, fallback to the Xbox back buffer
		if (pRenderTarget == xbox::zeroptr) {
			LOG_TEST_CASE("SetRenderTarget fallback to backbuffer");
			pRenderTarget = g_pXbox_BackBufferSurface;
		}
   	}

	// Set default viewport now we've updated the rendertarget
	// Note the Xbox does this, but before _our_ SetRenderTarget sets up the render target
	// Test case: Dashboard
	static xbox::X_D3DVIEWPORT8 defaultViewport;
	defaultViewport.X = 0;
	defaultViewport.Y = 0;
	defaultViewport.Width = INT_MAX;
	defaultViewport.Height = INT_MAX;
	defaultViewport.MinZ = 0.0f;
	defaultViewport.MaxZ = 1.0f;
	CxbxImpl_SetViewport(&defaultViewport);

	pHostRenderTarget = GetHostSurface(pRenderTarget, D3DUSAGE_RENDERTARGET);

	// The currenct depth stencil is always replaced by whats passed in here (even a null)
	g_pXbox_DepthStencil = pNewZStencil;
	g_ZScale = GetZScaleForPixelContainer(g_pXbox_DepthStencil); // TODO : Discern between Xbox and host and do this in UpdateDepthStencilFlags?
   	pHostDepthStencil = GetHostSurface(g_pXbox_DepthStencil, D3DUSAGE_DEPTHSTENCIL);

	HRESULT hRet;
	// Mimick Direct3D 8 SetRenderTarget by only setting render target if non-null
	if (pHostRenderTarget) {
		hRet = CxbxSetRenderTarget(pHostRenderTarget);
		if (FAILED(hRet)) {
			// If Direct3D 9 SetRenderTarget failed, skip setting depth stencil
			return;
		}
	}

	CxbxSetDepthStencilSurface(pHostDepthStencil);
	hRet = S_OK;

	if (SUCCEEDED(hRet)) {
		// Once we're sure the host depth-stencil is activated...
		UpdateDepthStencilFlags(pHostDepthStencil);
	}

   	// Validate that our host render target is still the correct size
   	DWORD HostRenderTarget_Width, HostRenderTarget_Height;
   	if (GetHostRenderTargetDimensions(&HostRenderTarget_Width, &HostRenderTarget_Height, pHostRenderTarget)) {
   	   	DWORD XboxRenderTarget_Width = GetPixelContainerWidth(g_pXbox_RenderTarget);
   	   	DWORD XboxRenderTarget_Height = GetPixelContainerHeight(g_pXbox_RenderTarget);
   	   	ValidateRenderTargetDimensions(HostRenderTarget_Width, HostRenderTarget_Height, XboxRenderTarget_Width, XboxRenderTarget_Height);
   	}
}

// ******************************************************************
// * patch: D3DDevice_SetRenderTarget
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetRenderTarget)
(
   	X_D3DSurface    *pRenderTarget,
   	X_D3DSurface    *pNewZStencil
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pRenderTarget)
		LOG_FUNC_ARG(pNewZStencil)
		LOG_FUNC_END;

	NestedPatchCounter call(setRenderTargetCount);

	XB_TRMP(D3DDevice_SetRenderTarget)(pRenderTarget, pNewZStencil);

	CxbxImpl_SetRenderTarget(pRenderTarget, pNewZStencil);
}

// LTCG specific D3DDevice_SetRenderTarget function...
// Passes pRenderTarget in ecx and pNewZStencil in eax
static void D3DDevice_SetRenderTarget_0__LTCG_ecx1_eax2
(
   	xbox::X_D3DSurface    *pRenderTarget,
   	xbox::X_D3DSurface    *pNewZStencil
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pRenderTarget)
		LOG_FUNC_ARG(pNewZStencil)
		LOG_FUNC_END;

	NestedPatchCounter call(setRenderTargetCount);

	__asm {
		mov  ecx, pRenderTarget
		mov  eax, pNewZStencil
		call XB_TRMP(D3DDevice_SetRenderTarget_0__LTCG_ecx1_eax2)
	}

	CxbxImpl_SetRenderTarget(pRenderTarget, pNewZStencil);
}

__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetRenderTarget_0__LTCG_ecx1_eax2)
(
)
{
	X_D3DSurface *pRenderTarget;
   	X_D3DSurface *pNewZStencil;
	__asm {
		LTCG_PROLOGUE
		mov  pRenderTarget, ecx
		mov  pNewZStencil, eax
	}

	// Actual function body
	D3DDevice_SetRenderTarget_0__LTCG_ecx1_eax2(pRenderTarget, pNewZStencil);

	__asm {
		LTCG_EPILOGUE
		ret
	}
}

// ******************************************************************
// * patch: D3D_CommonSetRenderTarget
// ******************************************************************
// This is an internal function, but some LTCG games inline SetRenderTarget and call it directly
// Test-case: Midtown Madness 3
xbox::void_xt WINAPI xbox::EMUPATCH(D3D_CommonSetRenderTarget)
(
   	X_D3DSurface    *pRenderTarget,
   	X_D3DSurface    *pNewZStencil,
   	void            *unknown
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pRenderTarget)
		LOG_FUNC_ARG(pNewZStencil)
		LOG_FUNC_ARG(unknown)
		LOG_FUNC_END;

	NestedPatchCounter call(setRenderTargetCount);

	XB_TRMP(D3D_CommonSetRenderTarget)(pRenderTarget, pNewZStencil, unknown);

	if (call.GetLevel() == 0) {
		CxbxImpl_SetRenderTarget(pRenderTarget, pNewZStencil);
	}
}

static void CxbxImpl_SetPalette
(
   	xbox::dword_xt      Stage,
   	xbox::X_D3DPalette *pPalette
)
{
	if (Stage >= xbox::X_D3DTS_STAGECOUNT) {
		LOG_TEST_CASE("Stage out of bounds");
	} else {
		// Note : Actual update of paletized textures (X_D3DFMT_P8) happens in CxbxUpdateHostTextures!
		g_pXbox_Palette_Data[Stage] = GetDataFromXboxResource(pPalette);
		g_Xbox_Palette_Size[Stage] = pPalette ? XboxD3DPaletteSizeToBytes(GetXboxPaletteSize(pPalette)) : 0;
	}
}

// Overload for logging
static void D3DDevice_SetPalette_4__LTCG_eax1
(
	xbox::dword_xt      Stage,
	xbox::X_D3DPalette *pPalette
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Stage)
		LOG_FUNC_ARG(pPalette)
		LOG_FUNC_END;
}

// LTCG specific D3DDevice_SetPalette function...
// This uses a custom calling convention where Stage parameter is passed in EAX
// Test-case: Ninja Gaiden
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetPalette_4__LTCG_eax1)
(
	X_D3DPalette *pPalette
)
{
	dword_xt Stage;
	__asm {
		LTCG_PROLOGUE
		mov  Stage, eax
	}

	// Log
	D3DDevice_SetPalette_4__LTCG_eax1(Stage, pPalette);

	// Call the Xbox implementation of this function, to properly handle reference counting for us
	__asm {
		push pPalette
		mov  eax, Stage
		call XB_TRMP(D3DDevice_SetPalette_4__LTCG_eax1)
	}

	CxbxImpl_SetPalette(Stage, pPalette);

	__asm {
		LTCG_EPILOGUE
		ret  4
	}
}

// ******************************************************************
// * patch: D3DDevice_SetPalette
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetPalette)
(
	dword_xt      Stage,
	X_D3DPalette *pPalette
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Stage)
		LOG_FUNC_ARG(pPalette)
		LOG_FUNC_END;

	// Call the Xbox implementation of this function, to properly handle reference counting for us
	XB_TRMP(D3DDevice_SetPalette)(Stage, pPalette);

	CxbxImpl_SetPalette(Stage, pPalette);
}

// Overload for logging
static void D3DDevice_DeleteVertexShader_0__LTCG_eax1
(
	xbox::dword_xt Handle
)
{
	LOG_FUNC_ONE_ARG(Handle);
}

// LTCG specific D3DDevice_DeleteVertexShader function...
// This uses a custom calling convention where parameter is passed in EAX
// Test-case: Midtown Madness 3
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_DeleteVertexShader_0__LTCG_eax1)
(
)
{
	dword_xt Handle;
	__asm {
		LTCG_PROLOGUE
		mov  Handle, eax
	}

	// Log
	D3DDevice_DeleteVertexShader_0__LTCG_eax1(Handle);

	CxbxImpl_DeleteVertexShader(Handle);

	// When deleting, call trampoline *after* our implementation,
	// so that we can still access it's fields before it gets deleted!
	__asm {
		mov  eax, Handle
		call XB_TRMP(D3DDevice_DeleteVertexShader_0__LTCG_eax1)

		LTCG_EPILOGUE
		ret
	}
}

// ******************************************************************
// * patch: D3DDevice_SetScreenSpaceOffset
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetScreenSpaceOffset)
(
   	float_xt x,
   	float_xt y
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_END;

	CxbxImpl_SetScreenSpaceOffset(x, y);
}

// ******************************************************************
// * patch: D3DDevice_SetRenderTargetFast
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetRenderTargetFast)
(
   	X_D3DSurface	*pRenderTarget,
   	X_D3DSurface	*pNewZStencil,
   	dword_xt			Flags
)
{
	LOG_FORWARD("D3DDevice_SetRenderTarget");

	// Redirect to the standard version.
	
	EMUPATCH(D3DDevice_SetRenderTarget)(pRenderTarget, pNewZStencil);
}

// ******************************************************************
// * patch: D3D_LazySetPointParams
// ******************************************************************
void WINAPI xbox::EMUPATCH(D3D_LazySetPointParams)
(
	void* Device
)
{
	LOG_FUNC_ONE_ARG(Device);

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_Simple
// ******************************************************************
xbox::void_xt __fastcall xbox::EMUPATCH(D3DDevice_SetRenderState_Simple)
(
   	dword_xt Method,
   	dword_xt Value
)
{
   	LOG_FUNC_BEGIN
   	   	LOG_FUNC_ARG(Method)
   	   	LOG_FUNC_ARG(Value)
   	   	LOG_FUNC_END;

   	XB_TRMP(D3DDevice_SetRenderState_Simple)(Method, Value);

   	// Fetch the RenderState conversion info for the given input
   	int XboxRenderStateIndex = -1;
   	for (int i = X_D3DRS_FIRST; i <= X_D3DRS_LAST; i++) {
   	   	if (GetDxbxRenderStateInfo(i).M == PUSH_METHOD(Method)) {
   	   	   	XboxRenderStateIndex = i;
   	   	   	break;
   	   	}
   	}

   	// If we could not map it, log and return
   	if (XboxRenderStateIndex == -1) {
   	   	EmuLog(LOG_LEVEL::WARNING, "RenderState_Simple(0x%.08X (%s), 0x%.08X) could not be found in RenderState table", Method, GetDxbxRenderStateInfo(XboxRenderStateIndex).S, Value);
   	   	return;
   	}

	EmuLog(LOG_LEVEL::DEBUG, "RenderState_Simple: %s = 0x%08X", GetDxbxRenderStateInfo(XboxRenderStateIndex).S, Value);

   	XboxRenderStates.SetXboxRenderState(XboxRenderStateIndex, Value);
}

// ******************************************************************
// * patch: D3DDevice_SetTransform
// ******************************************************************
void CxbxImpl_SetTransform
(
   	xbox::X_D3DTRANSFORMSTATETYPE State,
   	CONST xbox::X_D3DMATRIX *pMatrix
)
{
   	LOG_INIT

	d3d8TransformState.SetTransform(State, pMatrix);
	// Note : SetTransform is handled in our fixed function shader  - see UpdateFixedFunctionVertexShaderState()
}

// MultiplyTransform should call SetTransform, we'd like to know if it didn't
// Test case: 25 to Life

// LTCG specific D3DDevice_SetTransform function...
// This uses a custom calling convention where parameter is passed in EAX, EDX

// Naked functions must not contain objects that would require unwinding
// so we cheat a bit by stashing the function body in a separate function
static void D3DDevice_SetTransform_0__LTCG_eax1_edx2
(
	xbox::X_D3DTRANSFORMSTATETYPE State,
   	CONST xbox::X_D3DMATRIX *pMatrix
)
{
   	LOG_FUNC_BEGIN
   	   	LOG_FUNC_ARG(State)
   	   	LOG_FUNC_ARG(pMatrix)
   	   	LOG_FUNC_END;

   	setTransformCount++;

   	__asm {
   	   	// Trampoline to guest code to remove the need for a GetTransform patch
   	   	mov  eax, State
   	   	mov  edx, pMatrix
   	   	call XB_TRMP(D3DDevice_SetTransform_0__LTCG_eax1_edx2)
   	}

	CxbxImpl_SetTransform(State, pMatrix);
}

__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetTransform_0__LTCG_eax1_edx2)
(
)
{
	X_D3DTRANSFORMSTATETYPE State;
   	CONST X_D3DMATRIX *pMatrix;
   	__asm {
   	   	LTCG_PROLOGUE
   	   	mov  State, eax
   	   	mov  pMatrix, edx   
   	}

	// Log + implementation
	D3DDevice_SetTransform_0__LTCG_eax1_edx2(State, pMatrix);

   	__asm {
   	   	LTCG_EPILOGUE
   	   	ret
   	}
}
