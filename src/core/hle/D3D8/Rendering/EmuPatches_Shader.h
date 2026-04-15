#ifndef EMUPATCHES_SHADER_H
#define EMUPATCHES_SHADER_H

#include "core\hle\XAPI\Xapi.h" // For EMUPATCH
#include "core\hle\D3D8\XbD3D8Types.h"

namespace xbox {

// ******************************************************************
// * patch: D3DDevice_LoadVertexShader
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_LoadVertexShader)
(
    dword_xt                       Handle,
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

xbox::void_xt WINAPI EMUPATCH(D3DDevice_SelectVertexShader_0__LTCG_eax1_ebx2)();
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SelectVertexShader_4__LTCG_eax1)
(
    dword_xt                       Address
);

// ******************************************************************
// * patch: D3DDevice_SetShaderConstantMode
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetShaderConstantMode)
(
    X_VERTEXSHADERCONSTANTMODE Mode
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

xbox::void_xt __fastcall EMUPATCH(D3DDevice_SetVertexShaderConstant_8__LTCG_edx3)
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
// * patch: D3DDevice_SetVertexShaderConstantNotInline_0__LTCG_ebx1_edx2_eax3
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetVertexShaderConstantNotInline_0__LTCG_ebx1_edx2_eax3)();

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
// * patch: D3DDevice_SetPixelShader
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetPixelShader)
(
    dword_xt           Handle
);

xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetTexture_4__LTCG_eax2)
(
	dword_xt           Stage
);

xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetTexture_4__LTCG_eax1)
(
	X_D3DBaseTexture  *pTexture
);

// ******************************************************************
// * patch: D3DDevice_DrawVertices
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_DrawVertices)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    uint_xt             StartVertex,
    uint_xt             VertexCount
);

// ******************************************************************
// * patch: D3DDevice_DrawVertices_4__LTCG_ecx2_eax3
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_DrawVertices_4__LTCG_ecx2_eax3)
(
    X_D3DPRIMITIVETYPE  PrimitiveType
);

// ******************************************************************
// * patch: D3DDevice_DrawVertices_8__LTCG_eax3
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_DrawVertices_8__LTCG_eax3)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    uint_xt             StartVertex
);

// ******************************************************************
// * patch: D3DDevice_DeleteVertexShader
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_DeleteVertexShader)
(
    dword_xt Handle
);

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
// * patch: D3DDevice_RunVertexStateShader_4__LTCG_esi2
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_RunVertexStateShader_4__LTCG_esi2)
(
    dword_xt Address
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
// * patch: D3DDevice_SetDepthClipPlanes
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_SetDepthClipPlanes)
(
    float_xt Near,
    float_xt Far,
    dword_xt Flags
);

#if 0 // DISABLED (Just calls MmAllocateContiguousMemory)

} // namespace xbox

#endif // EMUPATCHES_SHADER_H
