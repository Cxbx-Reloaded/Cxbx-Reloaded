#ifndef EMUPATCHES_MISC_H
#define EMUPATCHES_MISC_H

#include "core\hle\XAPI\Xapi.h" // For EMUPATCH
#include "core\hle\D3D8\XbD3D8Types.h"

namespace xbox {

// ******************************************************************
// * patch: IDirect3DResource8_IsBusy
// ******************************************************************
xbox::bool_xt WINAPI EMUPATCH(D3DDevice_IsBusy)();

#if 0
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

xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_EndVisibilityTest_0__LTCG_eax1)();

// ******************************************************************
// * patch: D3DDevice_GetVisibilityTestResult
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_GetVisibilityTestResult)
(
    dword_xt                       Index,
    uint_xt                       *pResult,
    ulonglong_xt                  *pTimeStamp
);

xbox::void_xt WINAPI EMUPATCH(D3DDevice_LoadVertexShader_0__LTCG_ecx1_eax2)();
xbox::void_xt WINAPI EMUPATCH(D3DDevice_LoadVertexShader_0__LTCG_edx1_eax2)();
xbox::void_xt WINAPI EMUPATCH(D3DDevice_LoadVertexShader_4__LTCG_eax1)
(
    dword_xt                       Address
);

// ******************************************************************
// * patch: D3DDevice_Swap
// ******************************************************************
dword_xt WINAPI EMUPATCH(D3DDevice_Swap)
(
    dword_xt Flags
);

// ******************************************************************
// * patch: D3DDevice_EnableOverlay
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_EnableOverlay)
(
    bool_xt Enable
);

// ******************************************************************
// * patch: D3DDevice_EnableOverlay_0__LTCG
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_EnableOverlay_0__LTCG)();

// ******************************************************************
// * patch: D3DDevice_UpdateOverlay
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_UpdateOverlay)
(
    X_D3DSurface *pSurface,
    CONST RECT   *SrcRect,
    CONST RECT   *DstRect,
    bool_xt       EnableColorKey,
    D3DCOLOR      ColorKey
);

// ******************************************************************
// * patch: D3DDevice_UpdateOverlay
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_UpdateOverlay_16__LTCG_eax2)
(
    X_D3DSurface *pSurface,
    CONST RECT   *DstRect,
    bool_xt       EnableColorKey,
    X_D3DCOLOR    ColorKey
);

// ******************************************************************
// * patch: D3DDevice_GetOverlayUpdateStatus
// ******************************************************************
xbox::bool_xt WINAPI EMUPATCH(D3DDevice_GetOverlayUpdateStatus)();

// ******************************************************************
// * patch: D3DDevice_SetRenderState_Simple
// ******************************************************************
xbox::void_xt __fastcall EMUPATCH(D3DDevice_SetRenderState_Simple)
(
    dword_xt Method,
    dword_xt Value
);

xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetTransform_0__LTCG_eax1_edx2)();

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
	CONST X_D3DRECTPATCH_INFO *pRectPatchInfo
);

// ******************************************************************
// * patch: D3DDevice_DrawTriPatch
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_DrawTriPatch)
(
	uint_xt					Handle,
	CONST float_xt				*pNumSegs,
	CONST X_D3DTRIPATCH_INFO* pTriPatchInfo
);

// ******************************************************************
// * patch: D3DDevice_GetProjectionViewportMatrix
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetProjectionViewportMatrix)
(
	X_D3DMATRIX *pProjectionViewport
);

// ******************************************************************
// * patch: D3DDevice_SetModelView
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetModelView)
(
	CONST X_D3DMATRIX *pModelView,
	CONST X_D3DMATRIX *pInverseModelView,
	CONST X_D3DMATRIX *pComposite
);

// ******************************************************************
// * patch: D3DDevice_FlushVertexCache
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_FlushVertexCache)();

// ******************************************************************
// * patch: D3DDevice_GetModelView
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_GetModelView)
(
    X_D3DMATRIX* pModelView
);

// ******************************************************************
// * patch: D3D_SetCommonDebugRegisters
// ******************************************************************
void WINAPI EMUPATCH(D3D_SetCommonDebugRegisters)();

// ******************************************************************
// * patch: D3D_BlockOnTime
// ******************************************************************
void WINAPI EMUPATCH(D3D_BlockOnTime)(dword_xt Time, int MakeSpace);

// ******************************************************************
// * patch: D3D_BlockOnTime_4__LTCG_eax1
// ******************************************************************
void WINAPI EMUPATCH(D3D_BlockOnTime_4__LTCG_eax1)(int MakeSpace);

// ******************************************************************
// * patch: D3D_DestroyResource
// ******************************************************************
void WINAPI EMUPATCH(D3D_DestroyResource)( X_D3DResource* pResource );

// ******************************************************************
// * patch: D3D_DestroyResource_0__LTCG_edi1
// ******************************************************************
void WINAPI EMUPATCH(D3D_DestroyResource_0__LTCG_edi1)();



} // namespace xbox

#endif // EMUPATCHES_MISC_H
