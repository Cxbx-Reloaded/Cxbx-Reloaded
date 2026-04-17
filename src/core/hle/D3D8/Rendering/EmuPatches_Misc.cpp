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

// Variables only used in EmuPatches_Misc.cpp
static DWORD g_OverlaySwap = 0; // Set in D3DDevice_UpdateOverlay
static std::stack<IDirect3DQuery*> g_HostQueryVisibilityTests;
static std::map<int, IDirect3DQuery*> g_HostVisibilityTestMap;

xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_BeginVisibilityTest)()
{
	LOG_FUNC();

	if (g_bEnableHostQueryVisibilityTest) {
#ifdef CXBX_USE_D3D11
		D3D11_QUERY_DESC QueryDesc;
		QueryDesc.Query = D3D11_QUERY_OCCLUSION;
		QueryDesc.MiscFlags = 0;
#endif
		// Create a D3D occlusion query to handle "visibility test" with
		IDirect3DQuery* pHostQueryVisibilityTest = nullptr;
		HRESULT hRet = g_pD3DDevice->CreateQuery(_9_11(D3DQUERYTYPE_OCCLUSION, &QueryDesc), &pHostQueryVisibilityTest);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateQuery (visibility test)");
		if (pHostQueryVisibilityTest != nullptr) {
			CxbxQueryIssueBegin(pHostQueryVisibilityTest);
			{
				g_HostQueryVisibilityTests.push(pHostQueryVisibilityTest);
			}

			pHostQueryVisibilityTest = nullptr;
		}
	}

	return D3D_OK;
}

// LTCG specific D3DDevice_EndVisibilityTest function...
// This uses a custom calling convention where parameter is passed in EAX
// Test-case: Test Drive: Eve of Destruction
__declspec(naked) xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_EndVisibilityTest_0__LTCG_eax1)
(
)
{
   	dword_xt Index;
	xbox::hresult_xt result;
   	__asm {
   	   	LTCG_PROLOGUE
   	   	mov  Index, eax
   	}

   	result = EMUPATCH(D3DDevice_EndVisibilityTest)(Index);

   	__asm {
   	   	mov  eax, result
   	   	LTCG_EPILOGUE
   	   	ret
   	}
}

// ******************************************************************
// * patch: D3DDevice_EndVisibilityTest
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_EndVisibilityTest)
(
   	dword_xt                       Index
)
{
	LOG_FUNC_ONE_ARG(Index);

	if (g_bEnableHostQueryVisibilityTest) {
		// Check that the dedicated storage for the given Index isn't in use
		if (g_HostVisibilityTestMap[Index] != nullptr) {
			return E_OUTOFMEMORY;
		}

		if (g_HostQueryVisibilityTests.empty()) {
			return 2088; // visibility test incomplete (a prior BeginVisibilityTest call is needed)
		}

		IDirect3DQuery* pHostQueryVisibilityTest = g_HostQueryVisibilityTests.top();
		g_HostQueryVisibilityTests.pop();
		assert(pHostQueryVisibilityTest != nullptr);

		CxbxQueryIssueEnd(pHostQueryVisibilityTest);
		{
			// Associate the result of this call with the given Index
			g_HostVisibilityTestMap[Index] = pHostQueryVisibilityTest;
		}
	}

   	return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_GetVisibilityTestResult
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_GetVisibilityTestResult)
(
   	dword_xt                       Index,
   	uint_xt                       *pResult,
   	ulonglong_xt                  *pTimeStamp
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Index)
		LOG_FUNC_ARG(pResult)
		LOG_FUNC_ARG(pTimeStamp)
		LOG_FUNC_END;

	if (g_bEnableHostQueryVisibilityTest) {
		IDirect3DQuery* pHostQueryVisibilityTest = g_HostVisibilityTestMap[Index];
		if (pHostQueryVisibilityTest == nullptr) {
			return E_OUTOFMEMORY;
		}

		// In order to prevent an endless loop if the D3D device becomes lost, we pass
		// the D3DGETDATA_FLUSH flag. This tells GetData to return D3DERR_DEVICELOST if
		// such a situation occurs, and break out of the loop as a result.
		// Note: By Cxbx's design, we cannot do drawing within this while loop in order
		// to further prevent any other endless loop situations.
#ifdef CXBX_USE_D3D11
		UINT64 occlusionData = 0;
		HRESULT hRet;
		while ((hRet = CxbxQueryGetData(pHostQueryVisibilityTest, &occlusionData, sizeof(occlusionData), 0)) == S_FALSE) {
			SwitchToThread(); // Yield CPU while waiting for GPU query result
		}
		if (FAILED(hRet)) {
			EmuLog(LOG_LEVEL::WARNING, "GetVisibilityTestResult: query failed (0x%08X)", hRet);
		}
		if (pResult != xbox::zeroptr)
			*pResult = (uint_xt)occlusionData;
#else
		while (S_FALSE == CxbxQueryGetData(pHostQueryVisibilityTest, pResult, sizeof(DWORD), D3DGETDATA_FLUSH));
#endif

		g_HostVisibilityTestMap[Index] = nullptr;
		pHostQueryVisibilityTest->Release();
	} else {
		// Fallback to old faked result when there's no host occlusion query :
		if (pResult != xbox::zeroptr) {
			*pResult = 640 * 480; // TODO : Use actual backbuffer dimensions
		}
	}

	if (pTimeStamp != xbox::zeroptr) {
		LOG_TEST_CASE("requested value for pTimeStamp");
		*pTimeStamp = sizeof(DWORD); // TODO : This should be an incrementing GPU-memory based DWORD-aligned memory address
	}

   	return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_EnableOverlay
// ******************************************************************
static void CxbxrImpl_EnableOverlay()
{
	// The Xbox D3DDevice_EnableOverlay call merely resets the active
	// NV2A overlay state, it doesn't actually enable or disable anything.
	// Thus, we should just reset our overlay state here too. A title will
	// show new overlay data via D3DDevice_UpdateOverlay (see below).
	g_OverlayProxy = {};
}

xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_EnableOverlay)
(
   	bool_xt Enable
)
{
	LOG_FUNC_ONE_ARG(Enable);

	CxbxrImpl_EnableOverlay();
}

// ******************************************************************
// * patch: D3DDevice_EnableOverlay_0__LTCG
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_EnableOverlay_0__LTCG)()
{
	LOG_FUNC();

	CxbxrImpl_EnableOverlay();
}

static void CxbxrImpl_UpdateOverlay
(
	xbox::X_D3DSurface *pSurface,
	CONST xbox::X_RECT *SrcRect,
	CONST xbox::X_RECT *DstRect,
	xbox::bool_xt       EnableColorKey,
	xbox::X_D3DCOLOR    ColorKey
)
{
	using namespace xbox;

	// Reset and remember the overlay arguments, so our D3DDevice_Swap patch
	// can correctly show this overlay surface data.
	g_OverlayProxy = {};
	if (pSurface) {
		g_OverlayProxy.Surface = *pSurface;
		if (SrcRect)
			g_OverlayProxy.SrcRect = *SrcRect;

		if (DstRect)
			g_OverlayProxy.DstRect = *DstRect;

		g_OverlayProxy.EnableColorKey = EnableColorKey;
		g_OverlayProxy.ColorKey = ColorKey;
		// Update overlay if present was not called since the last call to
		// EmuD3DDevice_UpdateOverlay.
		if (g_OverlaySwap != g_Xbox_SwapData.Swap - 1) {
			EMUPATCH(D3DDevice_Swap)(CXBX_SWAP_PRESENT_FORWARD);
		}

		g_OverlaySwap = g_Xbox_SwapData.Swap;
	}
}

// ******************************************************************
// * patch: D3DDevice_UpdateOverlay
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_UpdateOverlay)
(
	X_D3DSurface *pSurface,
	CONST RECT   *SrcRect,
	CONST RECT   *DstRect,
	bool_xt       EnableColorKey,
	D3DCOLOR      ColorKey
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pSurface)
		LOG_FUNC_ARG(SrcRect)
		LOG_FUNC_ARG(DstRect)
		LOG_FUNC_ARG(EnableColorKey)
		LOG_FUNC_ARG(ColorKey)
		LOG_FUNC_END;

	CxbxrImpl_UpdateOverlay(pSurface, SrcRect, DstRect, EnableColorKey, ColorKey);
}

// ******************************************************************
// * patch: D3DDevice_UpdateOverlay_16__LTCG_eax2
// ******************************************************************
static void D3DDevice_UpdateOverlay_16__LTCG_eax2
(
	xbox::X_D3DSurface *pSurface,
	CONST RECT         *SrcRect,
	CONST RECT         *DstRect,
	xbox::bool_xt       EnableColorKey,
	D3DCOLOR            ColorKey
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pSurface)
		LOG_FUNC_ARG(SrcRect)
		LOG_FUNC_ARG(DstRect)
		LOG_FUNC_ARG(EnableColorKey)
		LOG_FUNC_ARG(ColorKey)
		LOG_FUNC_END;
}

// This uses a custom calling convention where parameter is passed in EAX
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_UpdateOverlay_16__LTCG_eax2)
(
	X_D3DSurface *pSurface,
	CONST RECT   *DstRect,
	bool_xt       EnableColorKey,
	D3DCOLOR      ColorKey
)
{
	RECT* SrcRect;
	__asm {
		LTCG_PROLOGUE
		mov  SrcRect, eax
	}

	// Log
	D3DDevice_UpdateOverlay_16__LTCG_eax2(pSurface, SrcRect, DstRect, EnableColorKey, ColorKey);

	CxbxrImpl_UpdateOverlay(pSurface, SrcRect, DstRect, EnableColorKey, ColorKey);

	__asm {
		LTCG_EPILOGUE
		ret  16
	}
}

// ******************************************************************
// * patch: D3DDevice_GetOverlayUpdateStatus
// ******************************************************************
xbox::bool_xt WINAPI xbox::EMUPATCH(D3DDevice_GetOverlayUpdateStatus)()
{
	LOG_FUNC();    

	LOG_UNIMPLEMENTED();

   	// TODO: Actually check for update status
   	return TRUE;
}

// ******************************************************************
// * patch: D3DDevice_InsertFence
// ******************************************************************
xbox::dword_xt WINAPI xbox::EMUPATCH(D3DDevice_InsertFence)()
{
	LOG_FUNC();

   	// TODO: Actually implement this
   	dword_xt dwRet = 0x8000BEEF;

	LOG_UNIMPLEMENTED();

   	return dwRet;
}

// ******************************************************************
// * patch: D3DDevice_IsFencePending
// ******************************************************************
xbox::bool_xt WINAPI xbox::EMUPATCH(D3DDevice_IsFencePending)
(
   	dword_xt Fence
)
{
	LOG_FUNC_ONE_ARG(Fence);

	// TODO: Implement
	LOG_UNIMPLEMENTED();

	return FALSE;
}

// ******************************************************************
// * patch: D3DDevice_BlockOnFence
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_BlockOnFence)
(
   	dword_xt Fence
)
{
	LOG_FUNC_ONE_ARG(Fence);

   	// TODO: Implement
	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * patch: D3DResource_BlockUntilNotBusy
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DResource_BlockUntilNotBusy)
(
   	X_D3DResource *pThis
)
{
	LOG_FUNC_ONE_ARG(pThis);

   	// TODO: Implement
	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * patch: D3DDevice_InsertCallback
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_InsertCallback)
(
	X_D3DCALLBACKTYPE	Type,
	X_D3DCALLBACK		pCallback,
	dword_xt				Context
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Type)
		LOG_FUNC_ARG(pCallback)
		LOG_FUNC_ARG(Context)
		LOG_FUNC_END;

	CxbxImpl_InsertCallback(Type, pCallback, Context);

	LOG_INCOMPLETE();
}

#pragma warning(disable:4244)
// ******************************************************************
// * patch: D3DDevice_GetProjectionViewportMatrix
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_GetProjectionViewportMatrix)
(
	X_D3DMATRIX *pProjectionViewport
)
{
	LOG_FUNC_ONE_ARG(pProjectionViewport);

	// blueshogun96 1/25/10
	// It's been almost 3 years, but I think this is a better 
	// implementation.  Still probably not right, but better
	// then before.

	HRESULT hRet;
	D3DXMATRIX Out, mtxProjection, mtxViewport;
	D3DVIEWPORT Viewport;

#ifdef CXBX_USE_D3D11
	// Get current viewport from D3D11 state
	UINT numViewports = 1;
	g_pD3DDeviceContext->RSGetViewports(&numViewports, &Viewport);
	hRet = S_OK;
#else
	// Get current viewport
	hRet = g_pD3DDevice->GetViewport(&Viewport);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetViewport - Unable to get viewport!");
#endif

#ifdef CXBX_USE_D3D11
	// D3D11 doesn't have GetTransform - use our tracked transform state
	mtxProjection = (D3DXMATRIX)d3d8TransformState.Transforms[xbox::X_D3DTS_PROJECTION];
	hRet = S_OK;
#else
	// Get current projection matrix
	hRet = g_pD3DDevice->GetTransform(D3DTS_PROJECTION, &mtxProjection);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetTransform - Unable to get projection matrix!");
#endif

	// Clear the destination matrix
	::ZeroMemory(&Out, sizeof(Out));

	// Create the Viewport matrix manually
	// Direct3D8 doesn't give me everything I need in a viewport structure
	// (one thing I REALLY HATE!) so some constants will have to be used
	// instead.

	float ClipWidth = 2.0f;
	float ClipHeight = 2.0f;
	float ClipX = -1.0f;
	float ClipY = 1.0f;
#ifdef CXBX_USE_D3D11
	float Width = Viewport.Width;
	float Height = Viewport.Height;
#else
	float Width = DWtoF(Viewport.Width);
	float Height = DWtoF(Viewport.Height);
#endif

	D3DXMatrixIdentity(&mtxViewport);
	mtxViewport._11 = Width / ClipWidth;
	mtxViewport._22 = -(Height / ClipHeight);
	mtxViewport._41 = -(ClipX * mtxViewport._11);
	mtxViewport._42 = -(ClipY * mtxViewport._22);

	// Multiply projection and viewport matrix together
	D3DXMatrixMultiply(&Out, &mtxProjection, &mtxViewport);

	*pProjectionViewport = Out;

//	__asm int 3;
}
#pragma warning(default:4244)

// ******************************************************************
// * patch: D3DDevice_SetModelView
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetModelView)
(
	CONST X_D3DMATRIX *pModelView,
	CONST X_D3DMATRIX *pInverseModelView,
	CONST X_D3DMATRIX *pComposite
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pModelView)
		LOG_FUNC_ARG(pInverseModelView)
		LOG_FUNC_ARG(pComposite)
		LOG_FUNC_END;

	// TODO handle other matrices
	d3d8TransformState.SetWorldView(0, pModelView);
	LOG_TEST_CASE("SetModelView");
}

// ******************************************************************
// * patch: D3DDevice_FlushVertexCache
// ******************************************************************
void WINAPI xbox::EMUPATCH(D3DDevice_FlushVertexCache)()
{
	LOG_FUNC();

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * patch: D3DDevice_GetModelView
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_GetModelView)
(
	X_D3DMATRIX* pModelView
)
{
	LOG_FUNC_ONE_ARG(pModelView);

	*pModelView = *d3d8TransformState.GetWorldView(0);

	return D3D_OK;
}


DWORD PushBuffer[64 * 1024 / sizeof(DWORD)];

// ******************************************************************
// * patch: D3D_SetCommonDebugRegisters
// ******************************************************************
void WINAPI xbox::EMUPATCH(D3D_SetCommonDebugRegisters)()
{
	LOG_FUNC();

	// NOTE: I added this because I was too lazy to deal with emulating certain render
	// states that use it.  

	LOG_UNIMPLEMENTED();

}

// ******************************************************************
// * patch: D3DDevice_IsBusy
// ******************************************************************
xbox::bool_xt WINAPI xbox::EMUPATCH(D3DDevice_IsBusy)()
{
		LOG_FUNC();

	// NOTE: This function returns FALSE when the NV2A FIFO is empty/complete, or NV_PGRAPH_STATUS = 0
	// Otherwise, it returns true.

	return FALSE;
}

// ******************************************************************
// * patch: D3D_BlockOnTime
// ******************************************************************
void WINAPI xbox::EMUPATCH(D3D_BlockOnTime)(dword_xt Time, int MakeSpace)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Time)
		LOG_FUNC_ARG(MakeSpace)
		LOG_FUNC_END;

	// NOTE: This function is not meant to be emulated.  Just use it to find out
	// the function that is calling it, and emulate that instead!!!  If necessary,
	// create an XRef...

	//__asm int 3;

	LOG_UNIMPLEMENTED();
}

// LTCG specific D3D_BlockOnTime function
// This uses a custom calling convention where parameter is passed in EAX
// Test case: Burnout 3
__declspec(naked) void WINAPI xbox::EMUPATCH(D3D_BlockOnTime_4__LTCG_eax1)(int MakeSpace)
{
	xbox::dword_xt Time;
	__asm {
		LTCG_PROLOGUE
		mov  Time, eax
	}

	// LOG_FORWARD requires unwinding, so carry on without it
	EMUPATCH(D3D_BlockOnTime)(Time, MakeSpace);

	__asm {
		LTCG_EPILOGUE
		ret  4
	}
}

// ******************************************************************
// * patch: D3D_DestroyResource
// ******************************************************************
void WINAPI xbox::EMUPATCH(D3D_DestroyResource)(X_D3DResource* pResource)
{
   	LOG_FUNC_ONE_ARG(pResource);

   	// Release the host copy (if it exists!)
   	FreeHostResource(GetHostResourceKey(pResource));

   	// Call the Xbox version of DestroyResource
   	XB_TRMP(D3D_DestroyResource)(pResource);
}

// ******************************************************************
// * patch: D3D_DestroyResource_LTCG
// ******************************************************************
static void D3D_DestroyResource_0__LTCG_edi1(xbox::X_D3DResource* pResource)
{
	LOG_FUNC_ONE_ARG(pResource);
}

__declspec(naked) void WINAPI xbox::EMUPATCH(D3D_DestroyResource_0__LTCG_edi1)()
{
   	X_D3DResource* pResource;
   	__asm {
   	   	LTCG_PROLOGUE
   	   	mov  pResource, edi
   	}

   	// Log
   	D3D_DestroyResource_0__LTCG_edi1(pResource);

   	// Release the host copy (if it exists!)
   	FreeHostResource(GetHostResourceKey(pResource));

   	// Call the Xbox version of DestroyResource
   	__asm {
   	   	mov  edi, pResource
   	   	call XB_TRMP(D3D_DestroyResource_0__LTCG_edi1)

   	   	LTCG_EPILOGUE
   	   	ret
   	}
}

