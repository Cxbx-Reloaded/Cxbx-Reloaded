// ******************************************************************
// *  CPU-side tessellation of rect and tri patches for D3D11
// *  (D3D11 has no DrawRectPatch/DrawTriPatch API)
// ******************************************************************
#ifndef PATCHDRAW_H
#define PATCHDRAW_H

#ifdef CXBX_USE_D3D11

#include "core\hle\D3D8\XbD3D8Types.h"

// Tessellate a rectangular patch and draw it via D3D11
HRESULT CxbxDrawRectPatchD3D11(
	UINT Handle,
	const float *pNumSegs,
	const xbox::X_D3DRECTPATCH_INFO *pRectPatchInfo
);

// Tessellate a triangular patch and draw it via D3D11
HRESULT CxbxDrawTriPatchD3D11(
	UINT Handle,
	const float *pNumSegs,
	const xbox::X_D3DTRIPATCH_INFO *pTriPatchInfo
);

#endif // CXBX_USE_D3D11

#endif // PATCHDRAW_H
