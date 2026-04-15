#ifndef EMUPATCHES_DEVICE_H
#define EMUPATCHES_DEVICE_H

#include "core\hle\XAPI\Xapi.h" // For EMUPATCH
#include "core\hle\D3D8\XbD3D8Types.h"

namespace xbox {

// ******************************************************************
// * patch: Direct3D_CreateDevice
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(Direct3D_CreateDevice)
(
    uint_xt                     Adapter,
	X_D3DDEVTYPE                DeviceType,
	X_HWND                      hFocusWindow,
    dword_xt                    BehaviorFlags,
    X_D3DPRESENT_PARAMETERS    *pPresentationParameters,
    X_D3DDevice           **ppReturnedDeviceInterface
);

// ******************************************************************
// * patch: D3DDevice_GetDisplayFieldStatus
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetDisplayFieldStatus)
(
	X_D3DFIELD_STATUS *pFieldStatus
);

// ******************************************************************
// * patch: D3DDevice_BeginPush_4
// ******************************************************************
xbox::PDWORD WINAPI EMUPATCH(D3DDevice_BeginPush_4)(dword_xt Count);

// ******************************************************************
// * patch: D3DDevice_BeginPush_8
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_BeginPush_8)(dword_xt Count, dword_xt **ppPush);

// ******************************************************************
// * patch: D3DDevice_EndPush
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_EndPush)(dword_xt *pPush);

// ******************************************************************
// * patch: D3DDevice_Reset
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_Reset)
(
    X_D3DPRESENT_PARAMETERS *pPresentationParameters
);

xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_Reset_0__LTCG_edi1)();

xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_Reset_0__LTCG_ebx1)();

// ******************************************************************
// * patch: D3DDevice_SetIndices
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_SetIndices)
(
    X_D3DIndexBuffer   *pIndexData,
    uint_xt                BaseVertexIndex
);


} // namespace xbox

#endif // EMUPATCHES_DEVICE_H
