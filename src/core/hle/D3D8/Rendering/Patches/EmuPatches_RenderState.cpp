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

// Shared with EmuPatches_TextureState.cpp
extern thread_local uint32_t setTransformCount;

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

	// Determine mip level for surfaces that are children of a texture
	UINT mipSlice = 0;
#ifdef CXBX_USE_D3D11
	if (pRenderTarget != xbox::zeroptr) {
		xbox::X_D3DBaseTexture* pParent = ((xbox::X_D3DSurface*)pRenderTarget)->Parent;
		if (pParent != xbox::zeroptr && pRenderTarget->Format == pParent->Format) {
			GetSurfaceFaceAndLevelWithinTexture((xbox::X_D3DSurface*)pRenderTarget, pParent, mipSlice);
		}
	}
#endif

	// The currenct depth stencil is always replaced by whats passed in here (even a null)
	g_pXbox_DepthStencil = pNewZStencil;
	g_ZScale = GetZScaleForPixelContainer(g_pXbox_DepthStencil); // TODO : Discern between Xbox and host and do this in UpdateDepthStencilFlags?
   	pHostDepthStencil = GetHostSurface(g_pXbox_DepthStencil, D3DUSAGE_DEPTHSTENCIL);

	HRESULT hRet;
	// Mimick Direct3D 8 SetRenderTarget by only setting render target if non-null
	if (pHostRenderTarget) {
		hRet = CxbxSetRenderTarget(pHostRenderTarget, mipSlice);
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
   	// Skip validation for mip-level surfaces: host reports full texture size,
   	// Xbox reports mip-level size, which is an expected mismatch.
   	DWORD HostRenderTarget_Width, HostRenderTarget_Height;
   	if (mipSlice == 0 && GetHostRenderTargetDimensions(&HostRenderTarget_Width, &HostRenderTarget_Height, pHostRenderTarget)) {
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
