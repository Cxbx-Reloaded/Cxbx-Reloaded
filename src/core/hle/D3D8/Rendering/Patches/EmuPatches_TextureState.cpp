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
#include "../EmuD3D8_common.h"
#include "../IndexBufferConvert.h"

// Variables only used in EmuPatches_State.cpp
static xbox::X_D3DBaseTexture CxbxActiveTextureCopies[xbox::X_D3DTS_STAGECOUNT] = {}; // Set by D3DDevice_SwitchTexture. Cached active texture

// Forward declarations (defined later in this file)
thread_local uint32_t setTransformCount;
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

