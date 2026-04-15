#ifndef EMUPATCHES_STATE_H
#define EMUPATCHES_STATE_H

#include "core\hle\XAPI\Xapi.h" // For EMUPATCH
#include "core\hle\D3D8\XbD3D8Types.h"

namespace xbox {

// ******************************************************************
// * patch: D3DDevice_SetBackBufferScale
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetBackBufferScale)(float_xt x, float_xt y);

// ******************************************************************
// * patch: D3DDevice_SetGammaRamp
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetGammaRamp)
(
    dword_xt                   dwFlags,
    CONST X_D3DGAMMARAMP   *pRamp
);

// ******************************************************************
// * patch: D3DDevice_GetGammaRamp
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetGammaRamp)
(
    X_D3DGAMMARAMP     *pRamp
);

// ******************************************************************
// * patch: D3DDevice_SetViewport
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetViewport)
(
    X_D3DVIEWPORT8 *pViewport
);

xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetShaderConstantMode_0__LTCG_eax1)();

// ******************************************************************
// * patch: D3DDevice_SetTexture
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetTexture)
(
    dword_xt           Stage,
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
// * patch: IDirect3DResource8_AddRef
// ******************************************************************
xbox::ulong_xt WINAPI EMUPATCH(D3DResource_AddRef)
(
    X_D3DResource      *pThis
);

// ******************************************************************
// * patch: D3DDevice_SetTransform
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetTransform)
(
	X_D3DTRANSFORMSTATETYPE State,
    CONST X_D3DMATRIX      *pMatrix
);

// ******************************************************************
// * patch: D3DDevice_MultiplyTransform
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_MultiplyTransform)
(
	X_D3DTRANSFORMSTATETYPE State,
	CONST X_D3DMATRIX      *pMatrix
);

// ******************************************************************
// * patch: D3DDevice_MultiplyTransform_0__LTCG_ebx1_eax2
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_MultiplyTransform_0__LTCG_ebx1_eax2)();

// ******************************************************************
// * patch: D3DDevice_SetStreamSource
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetStreamSource)
(
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

xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetVertexShader_0__LTCG_ebx1)();

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
// * patch: D3DDevice_LightEnable_4__LTCG_eax1
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_LightEnable_4__LTCG_eax1)
(
    bool_xt bEnable
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
// * patch: D3DDevice_SetRenderTarget_0__LTCG_ecx1_eax2
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetRenderTarget_0__LTCG_ecx1_eax2)();

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

xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetPalette_4__LTCG_eax1)
(
    X_D3DPalette *pPalette
);

xbox::void_xt WINAPI EMUPATCH(D3DDevice_DeleteVertexShader_0__LTCG_eax1)();

// ******************************************************************
// * patch: D3DDevice_SetScreenSpaceOffset
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetScreenSpaceOffset)
(
    float_xt x,
    float_xt y
);

// ******************************************************************
// * patch: D3DDevice_SetBackMaterial
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetBackMaterial)
(
	CONST X_D3DMATERIAL8 *pMaterial
);

#if 0 // patch disabled
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
// * patch: D3D::LazySetPointParams
// ******************************************************************
void WINAPI EMUPATCH(D3D_LazySetPointParams)( void* Device );

} // end of namespace xbox

#endif

} // namespace xbox

#endif // EMUPATCHES_STATE_H
