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
#include "Backend_D3D11.h"


bool IsSupportedFormat(xbox::X_D3DFORMAT X_Format, xbox::X_D3DRESOURCETYPE XboxResourceType, DWORD D3DUsage) {
	// TODO : Nuance the following, because the Direct3D 8 docs states
	// CheckDeviceFormat is needed when D3DUSAGE_RENDERTARGET or
	// D3DUSAGE_DYNAMNIC is specified.
	// Otherwise, lookup resource type and accompanying 'SupportedFormat' array
	bool *pbSupportedFormats = g_bSupportsFormatTexture;

	switch (XboxResourceType) {
		case xbox::X_D3DRTYPE_SURFACE: {
			if (D3DUsage & D3DUSAGE_RENDERTARGET) {
				pbSupportedFormats = g_bSupportsFormatSurfaceRenderTarget;
			} else if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
				pbSupportedFormats = g_bSupportsFormatSurfaceDepthStencil;
			} else {
				pbSupportedFormats = g_bSupportsFormatSurface;
			}
			break;
		}
		case xbox::X_D3DRTYPE_VOLUME: {
			pbSupportedFormats = g_bSupportsFormatVolumeTexture;
			break;
		}
		case xbox::X_D3DRTYPE_TEXTURE: {
			if (D3DUsage & D3DUSAGE_RENDERTARGET) {
				pbSupportedFormats = g_bSupportsFormatTextureRenderTarget;
			} else if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
				pbSupportedFormats = g_bSupportsFormatTextureDepthStencil;
			} else {
				pbSupportedFormats = g_bSupportsFormatTexture;
			}
			break;
		}
		case xbox::X_D3DRTYPE_VOLUMETEXTURE: {
			pbSupportedFormats = g_bSupportsFormatVolumeTexture;
			break;
		}
		case xbox::X_D3DRTYPE_CUBETEXTURE: {
			pbSupportedFormats = g_bSupportsFormatCubeTexture;
			break;
		}
	} // switch XboxResourceType

	return pbSupportedFormats[X_Format];
}


// ---- Helper: Try to resolve a surface via its parent texture ----
// Returns true if the surface was successfully mapped to a parent host texture
static bool TryResolveParentSurface(
	xbox::X_D3DResource* pResource,
	DWORD D3DUsage,
	int iTextureStage)
{
	xbox::X_D3DSurface *pXboxSurface = (xbox::X_D3DSurface *)pResource;
	xbox::X_D3DBaseTexture *pParentXboxTexture = (pXboxSurface) ? (xbox::X_D3DBaseTexture*)pXboxSurface->Parent : xbox::zeroptr;

	// Don't init the Parent if the Surface and Surface Parent formats differ
	// Happens in some Outrun 2006 SetRenderTarget calls
	if (!pParentXboxTexture || (pXboxSurface->Format != pParentXboxTexture->Format))
		return false;

	// For surfaces with a parent texture, map these to a host texture first
	// TODO : Investigate how it's possible (and how we could fix) the case when
	// the following call to GetHostBaseTexture would reject non-texture resources,
	// which would seem to trigger a "CreateCubeTexture Failed!" regression.
#ifdef CXBX_USE_D3D11
	// In D3D11, child surfaces are the same texture as the parent - subresource
	// indices (computed from face + mip level) are used when creating views.
	IDirect3DBaseTexture *pParentHostBaseTexture = GetHostBaseTexture(pParentXboxTexture, D3DUsage, iTextureStage);
	if (pParentHostBaseTexture) {
		int CubeMapFace = 0;
		UINT SurfaceLevel = 0;
		GetSurfaceFaceAndLevelWithinTexture(pXboxSurface, pParentXboxTexture, SurfaceLevel, CubeMapFace);
		SetHostSurface(pXboxSurface, (IDirect3DSurface *)pParentHostBaseTexture, iTextureStage);
		EmuLog(LOG_LEVEL::DEBUG, "TryResolveParentSurface : D3D11 mapped child surface to parent texture (Face: %u, Level: %u, pResource: 0x%.08X)",
			CubeMapFace, SurfaceLevel, pResource);
		return true;
	}
	EmuLog(LOG_LEVEL::WARNING, "TryResolveParentSurface : D3D11 failed to get parent host texture - falling through");
#else
	IDirect3DBaseTexture *pParentHostBaseTexture = GetHostBaseTexture(pParentXboxTexture, D3DUsage, iTextureStage);
	ComPtr<IDirect3DSurface> pNewHostSurface;
	switch (pParentHostBaseTexture->GetType()) {
	case D3DRTYPE_VOLUMETEXTURE: {
		LOG_TEST_CASE("Using child surface of VolumeTexture");
		// TODO
		break;
	}
	case D3DRTYPE_CUBETEXTURE: {
		// test-case : Burnout
		auto pParentHostTexture = (IDirect3DCubeTexture*)pParentHostBaseTexture;

		_9_11(D3DCUBEMAP_FACES, int) CubeMapFace = _9_11(D3DCUBEMAP_FACE_POSITIVE_X, 0);
		UINT SurfaceLevel = 0;
		GetSurfaceFaceAndLevelWithinTexture(pXboxSurface, pParentXboxTexture, SurfaceLevel, CubeMapFace);

		HRESULT hRet = pParentHostTexture->GetCubeMapSurface(CubeMapFace, SurfaceLevel, pNewHostSurface.GetAddressOf());

		DEBUG_D3DRESULT(hRet, "pHostParentTexture->GetCubeMapSurface");
		if (hRet == D3D_OK) {
			SetHostResource(pXboxSurface, (IDirect3DResource*)pNewHostSurface.Get(), iTextureStage, D3DUsage);
			EmuLog(LOG_LEVEL::DEBUG, "TryResolveParentSurface : Successfully created CubeTexture surface level (Face: %u, Level: %u, pResource: 0x%.08X, pNewHostSurface: 0x%.08X)",
				CubeMapFace, SurfaceLevel, pResource, pNewHostSurface.Get());
			return true;
		}
		break;
	}
	case D3DRTYPE_TEXTURE: {
		IDirect3DTexture* pParentHostTexture = (IDirect3DTexture*)pParentHostBaseTexture;

		UINT SurfaceLevel = 0;
		GetSurfaceFaceAndLevelWithinTexture(pXboxSurface, pParentXboxTexture, SurfaceLevel);
		HRESULT hRet = pParentHostTexture->GetSurfaceLevel(SurfaceLevel, pNewHostSurface.GetAddressOf());

		DEBUG_D3DRESULT(hRet, "pHostParentTexture->GetSurfaceLevel");
		if (hRet == D3D_OK) {
			SetHostResource(pResource, (IDirect3DResource*)pNewHostSurface.Get(), iTextureStage, D3DUsage);
			EmuLog(LOG_LEVEL::DEBUG, "TryResolveParentSurface : Successfully created Texture surface level (Level: %u, pResource: 0x%.08X, pNewHostSurface: 0x%.08X)",
				SurfaceLevel, pResource, pNewHostSurface.Get());
			return true;
		}
		break;
	}
	}

	EmuLog(LOG_LEVEL::WARNING, "Failed getting host surface level - falling through to regular surface creation");
#endif // !CXBX_USE_D3D11
	return false;
}

// ---- Helper: Try to resolve a volume via its parent volume texture ----
// Returns true if the volume was successfully mapped to a parent host volume texture
static bool TryResolveParentVolume(
	xbox::X_D3DResource* pResource,
	DWORD D3DUsage,
	int iTextureStage)
{
	xbox::X_D3DVolume *pXboxVolume = (xbox::X_D3DVolume *)pResource;
	xbox::X_D3DVolumeTexture *pParentXboxVolumeTexture = (pXboxVolume) ? (xbox::X_D3DVolumeTexture *)pXboxVolume->Parent : xbox::zeroptr;
	if (!pParentXboxVolumeTexture)
		return false;

#ifdef CXBX_USE_D3D11
	IDirect3DVolumeTexture *pParentHostVolumeTexture = GetHostVolumeTexture(pParentXboxVolumeTexture, iTextureStage);
	if (pParentHostVolumeTexture) {
		SetHostVolume(pXboxVolume, (IDirect3DVolume *)pParentHostVolumeTexture, iTextureStage);
		UINT VolumeLevel = 0; // TODO : Derive actual level based on pXboxVolume->Data delta to pParentXboxVolumeTexture->Data
		EmuLog(LOG_LEVEL::DEBUG, "TryResolveParentVolume : D3D11 mapped child volume to parent texture (Level: %u, pResource: 0x%.08X)",
			VolumeLevel, pResource);
		return true;
	}
	EmuLog(LOG_LEVEL::WARNING, "TryResolveParentVolume : D3D11 failed to get parent host volume texture - falling through");
#else
	IDirect3DVolumeTexture *pParentHostVolumeTexture = GetHostVolumeTexture(pParentXboxVolumeTexture, iTextureStage);
	UINT VolumeLevel = 0; // TODO : Derive actual level based on pXboxVolume->Data delta to pParentXboxVolumeTexture->Data
	ComPtr<IDirect3DVolume> pNewHostVolume;
	HRESULT hRet = pParentHostVolumeTexture->GetVolumeLevel(VolumeLevel, pNewHostVolume.GetAddressOf());
	DEBUG_D3DRESULT(hRet, "pParentHostVolumeTexture->GetVolumeLevel");
	if (hRet == D3D_OK) {
		SetHostResource(pResource, (IDirect3DResource*)pNewHostVolume.Get(), iTextureStage, D3DUsage);
		EmuLog(LOG_LEVEL::DEBUG, "TryResolveParentVolume : Successfully created volume level (%u, 0x%.08X, 0x%.08X)",
			VolumeLevel, pResource, pNewHostVolume.Get());
		return true;
	}

	EmuLog(LOG_LEVEL::WARNING, "Failed getting host volume level - falling through to regular volume creation");
#endif // !CXBX_USE_D3D11
	return false;
}

// ---- Helper: Determine the host format for an Xbox pixel format ----
// Sets bConvertTextureFormat and may clear D3DUSAGE_DEPTHSTENCIL from D3DUsage
static EMUFORMAT ResolveHostFormat(
	xbox::X_D3DFORMAT X_Format,
	xbox::X_D3DRESOURCETYPE XboxResourceType,
	DWORD& D3DUsage,
	const char* ResourceTypeName,
	bool& bConvertTextureFormat)
{
	EMUFORMAT PCFormat;
	bConvertTextureFormat = false;

	if (EmuXBFormatRequiresConversion(X_Format, /*&*/PCFormat)) {
		bConvertTextureFormat = true;

		// Unset D3DUSAGE_DEPTHSTENCIL: It's not possible for ARGB textures to be depth stencils
		// Fixes CreateTexture error in Virtua Cop 3 (Chihiro)
		D3DUsage &= ~D3DUSAGE_DEPTHSTENCIL;

#ifdef CXBX_USE_D3D11
		// D3D11: For 32-bit formats that differ only in channel order,
		// skip CPU conversion and upload raw bytes as R8G8B8A8_UNORM.
		// The pixel shader applies the correct channel swizzle via TEXFMTFIXUP.
		if (X_Format == xbox::X_D3DFMT_B8G8R8A8 || X_Format == xbox::X_D3DFMT_LIN_B8G8R8A8 ||
			X_Format == xbox::X_D3DFMT_R8G8B8A8 || X_Format == xbox::X_D3DFMT_LIN_R8G8B8A8) {
			bConvertTextureFormat = false;
			PCFormat = EMUFMT_A8B8G8R8; // = DXGI_FORMAT_R8G8B8A8_UNORM (raw byte order)
		}
#endif
	}
	else {
		if (IsSupportedFormat(X_Format, XboxResourceType, D3DUsage)) {
			PCFormat = EmuXB2PC_D3DFormat(X_Format);
		}
		else {
			if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
				EmuLog(LOG_LEVEL::WARNING, "Xbox %s Format %x will be converted to EMUFMT_D24S8", ResourceTypeName, X_Format);
				PCFormat = EMUFMT_D24S8;
			} else if (EmuXBFormatCanBeConverted(X_Format, /*&*/PCFormat)) {
				EmuLog(LOG_LEVEL::WARNING, "Xbox %s Format %x will be converted to ARGB", ResourceTypeName, X_Format);
				bConvertTextureFormat = true;
			} else {
				/*CxbxrAbort*/EmuLog(LOG_LEVEL::WARNING, "Encountered a completely incompatible %s format!", ResourceTypeName);
				PCFormat = EmuXB2PC_D3DFormat(X_Format);
			}
		}
	}

	return PCFormat;
}

// ---- Helper: Copy one mip level of Xbox texture data to the host buffer ----
// Handles format conversion, unswizzling, compressed copy, or linear copy
static void CopyMipDataToHost(
	uint8_t* pDst, DWORD dwDstRowPitch, DWORD dwDstSlicePitch,
	uint8_t* pSrc,
	xbox::X_D3DFORMAT X_Format, UINT dwBPP,
	DWORD pxMipWidth, DWORD pxMipHeight, DWORD pxMipDepth,
	DWORD dwMipRowPitch, DWORD mip2dSize,
	bool bConvertTextureFormat, bool bSwizzled, bool bCompressed,
	int iTextureStage)
{
	if (bConvertTextureFormat) {
		EmuLog(LOG_LEVEL::DEBUG, "Unsupported texture format, expanding to EMUFMT_A8R8G8B8");

		// In case where there is a palettized texture without a palette attached,
		// fill it with zeroes for now. This might not be correct, but it prevents a crash.
		// Test case: DRIV3R
		bool missingPalette = X_Format == xbox::X_D3DFMT_P8 && g_pXbox_Palette_Data[iTextureStage] == nullptr;
		if (missingPalette) {
			LOG_TEST_CASE("Palettized texture bound without a palette");
			memset(pDst, 0, dwDstRowPitch * pxMipHeight);
		}
		else {
			if (!ConvertD3DTextureToARGBBuffer(
				X_Format,
				pSrc, pxMipWidth, pxMipHeight, dwMipRowPitch, mip2dSize,
				pDst, dwDstRowPitch, dwDstSlicePitch,
				pxMipDepth,
				g_pXbox_Palette_Data[iTextureStage])) {
				CxbxrAbort("Unhandled conversion!");
			}
		}
	}
	else if (bSwizzled) {
		EmuUnswizzleBox(
			pSrc, pxMipWidth, pxMipHeight, pxMipDepth,
			dwBPP,
			pDst, dwDstRowPitch, dwDstSlicePitch
		);
	}
	else if (bCompressed) {
		memcpy(pDst, pSrc, mip2dSize);
	}
	else {
		if (dwDstRowPitch == dwMipRowPitch) {
			memcpy(pDst, pSrc, mip2dSize);
		}
		else {
			for (DWORD v = 0; v < pxMipHeight; v++) {
				memcpy(pDst, pSrc, pxMipWidth * dwBPP);
				pDst += dwDstRowPitch;
				pSrc += dwMipRowPitch;
			}
		}
	}
}


// ---- Helper: Create the host GPU resource for a pixel container ----
// Creates the D3D surface/texture/volume/cube resource based on XboxResourceType
static HRESULT CreateGpuPixelContainerResource(
	xbox::X_D3DRESOURCETYPE XboxResourceType,
	UINT hostWidth, UINT hostHeight, DWORD dwDepth, UINT dwMipMapLevels,
	EMUFORMAT& PCFormat,
	DWORD& D3DUsage, D3DPOOL D3DPool,
	bool bSwizzled, bool& bConvertTextureFormat, xbox::X_D3DFORMAT X_Format,
	xbox::X_D3DResource* pResource, const char* ResourceTypeName, int iTextureStage,
#ifdef CXBX_USE_D3D11
	ComPtr<ID3D11Resource>& pNewHostResource, bool& bHostIsDynamic
#else
	ComPtr<IDirect3DSurface>& pNewHostSurface,
	ComPtr<IDirect3DVolume>& pNewHostVolume,
	ComPtr<IDirect3DTexture>& pNewHostTexture,
	ComPtr<IDirect3DTexture>& pIntermediateHostTexture,
	ComPtr<IDirect3DVolumeTexture>& pNewHostVolumeTexture,
	ComPtr<IDirect3DVolumeTexture>& pIntermediateHostVolumeTexture,
	ComPtr<IDirect3DCubeTexture>& pNewHostCubeTexture,
	ComPtr<IDirect3DCubeTexture>& pIntermediateHostCubeTexture
#endif
)
{
	HRESULT hRet = D3D_OK;

	// Create the surface/volume/(volume/cube/)texture
	switch (XboxResourceType) {
	case xbox::X_D3DRTYPE_SURFACE: {
#ifdef CXBX_USE_D3D11
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = hostWidth;
		desc.Height = hostHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = PCFormat;
		desc.SampleDesc.Count = 1; // No MSAA for now; enabling requires resolve pass infrastructure
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
			desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;
		} else if (D3DUsage & D3DUSAGE_RENDERTARGET) {
			desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;
		} else {
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
		desc.MiscFlags = 0;

		hRet = g_pD3DDevice->CreateTexture2D(&desc, NULL, reinterpret_cast<ID3D11Texture2D**>(pNewHostResource.ReleaseAndGetAddressOf()));
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateTexture2D");
#else
		if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
			hRet = g_pD3DDevice->CreateDepthStencilSurface(hostWidth, hostHeight, PCFormat,
				g_EmuCDPD.HostPresentationParameters.MultiSampleType,
				0, // MultisampleQuality
				false, // Discard
				pNewHostSurface.GetAddressOf(),
				nullptr // pSharedHandle
			);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateDepthStencilSurface");
		}
		else {
			// Note : This handles both (D3DUsage & D3DUSAGE_RENDERTARGET) and otherwise alike
			D3DUsage = D3DUSAGE_RENDERTARGET; // Usage always as render target
			hRet = g_pD3DDevice->CreateTexture(hostWidth, hostHeight,
				1, // Levels
				D3DUsage,
				PCFormat,
				D3DPool, // D3DPOOL_DEFAULT
				pNewHostTexture.GetAddressOf(),
				nullptr // pSharedHandle
			);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateTexture");

			if (hRet == D3D_OK) {
				HRESULT hRet2 = pNewHostTexture->GetSurfaceLevel(0, pNewHostSurface.GetAddressOf());
				DEBUG_D3DRESULT(hRet2, "pNewHostTexture->pNewHostSurface");
				pNewHostTexture.Reset();
   	   	   	}
		}

		// First fail, retry with a fallback format
		// If this succeeds, the surface may not render correctly, but it won't crash
		if (hRet != D3D_OK) {
			if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
				EmuLog(LOG_LEVEL::WARNING, "CreateDepthStencilSurface Failed\n\nError: %s\nDesc: %s",
					DXGetErrorString(hRet), DXGetErrorDescription(hRet));
			}
			else {
				EmuLog(LOG_LEVEL::WARNING, "CreateTexture Failed\n\nError: %s\nDesc: %s",
					DXGetErrorString(hRet), DXGetErrorDescription(hRet));
			}

			EmuLog(LOG_LEVEL::WARNING, "Trying Fallback");
			hRet = g_pD3DDevice->CreateOffscreenPlainSurface(hostWidth, hostHeight, PCFormat, D3DPool, pNewHostSurface.GetAddressOf(), nullptr);
		}
#endif
		// If the fallback failed, show an error and exit execution.
		if (hRet != D3D_OK) {
			// We cannot safely continue in this state.
			CxbxrAbort("CreateImageSurface Failed!\n\nError: %s\nDesc: %s",
				DXGetErrorString(hRet), DXGetErrorDescription(hRet));
		}

#ifdef CXBX_USE_D3D11
		SetHostResource(pResource, pNewHostResource.Get(), iTextureStage);
		EmuLog(LOG_LEVEL::DEBUG, "CreateGpuPixelContainerResource : Successfully created %s (0x%.08X, 0x%.08X)",
			ResourceTypeName, pResource, pNewHostResource.Get());
#else
		SetHostSurface(pResource, pNewHostSurface.Get(), iTextureStage);
		EmuLog(LOG_LEVEL::DEBUG, "CreateGpuPixelContainerResource : Successfully created %s (0x%.08X, 0x%.08X)",
			ResourceTypeName, pResource, pNewHostSurface.Get());
#endif
		EmuLog(LOG_LEVEL::DEBUG, "CreateGpuPixelContainerResource : Width : %d, Height : %d, Format : %d",
			hostWidth, hostHeight, PCFormat);
		break;
	}

	case xbox::X_D3DRTYPE_VOLUME: {
#ifdef CXBX_USE_D3D11
		// In D3D11, a standalone volume (no parent VolumeTexture) is backed by
		// a single-depth Texture3D. This is rare but can occur if a game creates
		// a volume resource directly.
		D3D11_TEXTURE3D_DESC desc;
		desc.Width = hostWidth;
		desc.Height = hostHeight;
		desc.Depth = dwDepth > 0 ? dwDepth : 1;
		desc.MipLevels = 1;
		desc.Format = PCFormat;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;

		hRet = g_pD3DDevice->CreateTexture3D(&desc, NULL, reinterpret_cast<ID3D11Texture3D**>(pNewHostResource.ReleaseAndGetAddressOf()));
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateTexture3D (standalone volume)");

		if (hRet != D3D_OK) {
			CxbxrAbort("CreateTexture3D (standalone volume) Failed!\n\n"
				"Error: 0x%X\nFormat: %d\nDimensions: %dx%dx%d", hRet, PCFormat, hostWidth, hostHeight, desc.Depth);
		}
		SetHostVolume(pResource, (IDirect3DVolume *)pNewHostResource.Get(), iTextureStage);
		EmuLog(LOG_LEVEL::DEBUG, "CreateGpuPixelContainerResource : Successfully created standalone Volume (0x%.08X, 0x%.08X)",
			pResource, pNewHostResource.Get());
#else
		LOG_UNIMPLEMENTED();
		// Note : Host D3D can only(?) retrieve a volume like this :
		// hRet = pNewHostVolumeTexture->GetVolumeLevel(level, pNewHostVolume.GetAddressOf());
		// So, we need to do this differently - we need to step up to the containing VolumeTexture,
		// and retrieve and convert all of it's GetVolumeLevel() slices.
#endif
		break;
	}

	case xbox::X_D3DRTYPE_TEXTURE: {
#ifdef CXBX_USE_D3D11
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = hostWidth;
		desc.Height = hostHeight;
		desc.MipLevels = dwMipMapLevels;
		desc.ArraySize = 1;
		desc.Format = PCFormat;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
			desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;
		} else if (D3DUsage & D3DUSAGE_RENDERTARGET) {
			desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;
		} else if (dwMipMapLevels == 1) {
			// Check if the format supports typed UAV access (needed for GPU unswizzle CS)
			UINT fmtSupport = 0;
			bool bCanUAV = SUCCEEDED(g_pD3DDevice->CheckFormatSupport(PCFormat, &fmtSupport))
				&& (fmtSupport & D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW);

			if (bSwizzled && !bConvertTextureFormat && bCanUAV) {
				// Swizzled textures use DEFAULT + UAV for GPU compute shader unswizzle
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
				desc.CPUAccessFlags = 0;
			} else if (bSwizzled && X_Format == xbox::X_D3DFMT_P8) {
				// P8 textures use DEFAULT + UAV for GPU palette expand CS
				// Use R8G8B8A8_UNORM (not B8G8R8A8) for R32_UINT UAV compatibility
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
				desc.CPUAccessFlags = 0;
			} else {
				// D3D11_USAGE_DYNAMIC requires MipLevels == 1
				desc.Usage = D3D11_USAGE_DYNAMIC;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				bHostIsDynamic = true;
			}
		} else {
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
		}
		desc.MiscFlags = 0;

		EmuLog(LOG_LEVEL::DEBUG, "CreateTexture2D: XboxFmt=0x%02X PCFmt=%u (%ux%u mips=%u) "
			"Usage=%u BindFlags=0x%X Swizzled=%d ConvertFmt=%d",
			X_Format, desc.Format, desc.Width, desc.Height, desc.MipLevels,
			desc.Usage, desc.BindFlags, bSwizzled, bConvertTextureFormat);

		hRet = g_pD3DDevice->CreateTexture2D(&desc, NULL, reinterpret_cast<ID3D11Texture2D**>(pNewHostResource.ReleaseAndGetAddressOf()));
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateTexture2D");
#else
		hRet = g_pD3DDevice->CreateTexture(hostWidth, hostHeight, dwMipMapLevels,
			D3DUsage, PCFormat, D3DPool, pNewHostTexture.GetAddressOf(),
			nullptr
		);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateTexture");
#endif

		// If the above failed, we might be able to use an ARGB texture instead
		EMUFORMAT TmpPCFormat;
		if ((hRet != D3D_OK) && (PCFormat != EMUFMT_A8R8G8B8) && EmuXBFormatCanBeConverted(X_Format, TmpPCFormat)) {
#ifdef CXBX_USE_D3D11
			desc.Format = TmpPCFormat;
			hRet = g_pD3DDevice->CreateTexture2D(&desc, NULL, reinterpret_cast<ID3D11Texture2D**>(pNewHostResource.ReleaseAndGetAddressOf()));
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateTexture2D");
#else
			hRet = g_pD3DDevice->CreateTexture(hostWidth, hostHeight, dwMipMapLevels,
				D3DUsage, TmpPCFormat, D3DPool, pNewHostTexture.GetAddressOf(),
				nullptr
			);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateTexture(EMUFMT_A8R8G8B8)");
#endif
			if (hRet == D3D_OK) {
				// Okay, now this works, make sure the texture gets converted
				bConvertTextureFormat = true;
				PCFormat = TmpPCFormat;
			}
		}

#ifdef CXBX_USE_D3D11
		if (hRet != D3D_OK) {
			CxbxrAbort("CreateTexture2D Failed!\n\n"
				"Error: 0x%X\nFormat: %d\nDimensions: %dx%d", hRet, PCFormat, hostWidth, hostHeight);
		}
		SetHostResource(pResource, pNewHostResource.Get(), iTextureStage);
		EmuLog(LOG_LEVEL::DEBUG, "CreateGpuPixelContainerResource : Successfully created %s (0x%.08X, 0x%.08X)",
			ResourceTypeName, pResource, pNewHostResource.Get());
#else
   	   	// Now create our intermediate texture for UpdateTexture, but not for render targets or depth stencils
   	   	if (hRet == D3D_OK && (D3DUsage & D3DUSAGE_RENDERTARGET) == 0 && (D3DUsage & D3DUSAGE_DEPTHSTENCIL) == 0) {
   	   	   	hRet = g_pD3DDevice->CreateTexture(hostWidth, hostHeight, dwMipMapLevels,
   	   	   	   	0, PCFormat, D3DPOOL_SYSTEMMEM, pIntermediateHostTexture.GetAddressOf(),
   	   	   	   	nullptr
   	   	   	);
   	   	}

		if (hRet != D3D_OK) {
			CxbxrAbort("CreateTexture Failed!\n\n"
				"Error: 0x%X\nFormat: %d\nDimensions: %dx%d", hRet, PCFormat, hostWidth, hostHeight);
		}
		SetHostTexture(pResource, pNewHostTexture.Get(), iTextureStage);
		EmuLog(LOG_LEVEL::DEBUG, "CreateGpuPixelContainerResource : Successfully created %s (0x%.08X, 0x%.08X)",
			ResourceTypeName, pResource, pNewHostTexture.Get());
#endif
		break;
	}

	case xbox::X_D3DRTYPE_VOLUMETEXTURE: {
#ifdef CXBX_USE_D3D11
		D3D11_TEXTURE3D_DESC desc;
		desc.Width = hostWidth;
		desc.Height = hostHeight;
		desc.Depth = dwDepth;
		desc.MipLevels = dwMipMapLevels;
		desc.Format = PCFormat;
		if (dwMipMapLevels == 1) {
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bHostIsDynamic = true;
		} else {
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
		}
		desc.MiscFlags = 0;

		hRet = g_pD3DDevice->CreateTexture3D(&desc, NULL, reinterpret_cast<ID3D11Texture3D**>(pNewHostResource.ReleaseAndGetAddressOf()));
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateTexture3D");

		if (hRet != D3D_OK) {
			CxbxrAbort("CreateTexture3D Failed!\n\n"
				"Error: 0x%X\nFormat: %d\nDimensions: %dx%dx%d", hRet, PCFormat, hostWidth, hostHeight, dwDepth);
		}
		SetHostResource(pResource, pNewHostResource.Get(), iTextureStage);
		EmuLog(LOG_LEVEL::DEBUG, "CreateGpuPixelContainerResource : Successfully created %s (0x%.08X, 0x%.08X)",
			ResourceTypeName, pResource, pNewHostResource.Get());
#else
		hRet = g_pD3DDevice->CreateVolumeTexture(hostWidth, hostHeight, dwDepth,
			dwMipMapLevels, D3DUsage, PCFormat, D3DPool, pNewHostVolumeTexture.GetAddressOf(),
			nullptr
		);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateVolumeTexture");

   	   	// Now create our intermediate texture for UpdateTexture, but not for render targets or depth stencils
   	   	if (hRet == D3D_OK && (D3DUsage & D3DUSAGE_RENDERTARGET) == 0 && (D3DUsage & D3DUSAGE_DEPTHSTENCIL) == 0) {
   	   	   	hRet = g_pD3DDevice->CreateVolumeTexture(hostWidth, hostHeight, dwDepth,
   	   	   	   	dwMipMapLevels, 0, PCFormat, D3DPOOL_SYSTEMMEM, pIntermediateHostVolumeTexture.GetAddressOf(),
   	   	   	   	nullptr
   	   	   	);
   	   	}

		if (hRet != D3D_OK) {
			CxbxrAbort("CreateVolumeTexture Failed!\n\nError: %s\nDesc: %s",
				DXGetErrorString(hRet), DXGetErrorDescription(hRet));
		}
		SetHostVolumeTexture(pResource, pNewHostVolumeTexture.Get(), iTextureStage);
		EmuLog(LOG_LEVEL::DEBUG, "CreateGpuPixelContainerResource : Successfully created %s (0x%.08X, 0x%.08X)",
			ResourceTypeName, pResource, pNewHostVolumeTexture.Get());
#endif
		break;
	}

	case xbox::X_D3DRTYPE_CUBETEXTURE: {
		EmuLog(LOG_LEVEL::DEBUG, "CreateCubeTexture(%d, %d, 0, %d, D3DPOOL_DEFAULT)", hostWidth,
			dwMipMapLevels, PCFormat);

#ifdef CXBX_USE_D3D11
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = hostWidth;
		desc.Height = hostHeight;
		desc.MipLevels = dwMipMapLevels;
		desc.ArraySize = 6;
		desc.Format = PCFormat;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		// D3D11_USAGE_DYNAMIC requires ArraySize==1, but cube textures need 6.
		// Always use DEFAULT and upload data via UpdateSubresource.
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		hRet = g_pD3DDevice->CreateTexture2D(&desc, NULL, reinterpret_cast<ID3D11Texture2D**>(pNewHostResource.ReleaseAndGetAddressOf()));
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateTexture2D");
#else
		hRet = g_pD3DDevice->CreateCubeTexture(hostWidth, dwMipMapLevels, D3DUsage,
			PCFormat, D3DPool, pNewHostCubeTexture.GetAddressOf(),
			nullptr
		);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateCubeTexture");

   	   	// Now create our intermediate texture for UpdateTexture, but not for render targets or depth stencils
   	   	if (hRet == D3D_OK && (D3DUsage & D3DUSAGE_RENDERTARGET) == 0 && (D3DUsage & D3DUSAGE_DEPTHSTENCIL) == 0) {
   	   	   	hRet = g_pD3DDevice->CreateCubeTexture(hostWidth, dwMipMapLevels, 0,
   	   	   	   	PCFormat, D3DPOOL_SYSTEMMEM, pIntermediateHostCubeTexture.GetAddressOf(),
   	   	   	   	nullptr
   	   	   	);
   	   	}
#endif

		if (hRet != D3D_OK) {
			CxbxrAbort("CreateCubeTexture Failed!\n\nError: \nDesc: "/*,
				DXGetErrorString(hRet), DXGetErrorDescription(hRet)*/);
		}

#ifdef CXBX_USE_D3D11
		SetHostResource(pResource, pNewHostResource.Get(), iTextureStage);
		EmuLog(LOG_LEVEL::DEBUG, "CreateGpuPixelContainerResource : Successfully created %s (0x%.08X, 0x%.08X)",
			ResourceTypeName, pResource, pNewHostResource.Get());
#else
		SetHostCubeTexture(pResource, pNewHostCubeTexture.Get(), iTextureStage);
		EmuLog(LOG_LEVEL::DEBUG, "CreateGpuPixelContainerResource : Successfully created %s (0x%.08X, 0x%.08X)",
			ResourceTypeName, pResource, pNewHostCubeTexture.Get());
#endif
		// TODO : Cube face surfaces can be used as a render-target,
		// so we need to associate host surfaces to each surface of this cube texture
   	   	// However, we can't do it here: On Xbox, a new Surface is created on every call to
   	   	// GetCubeMapSurface, so it needs to be done at surface conversion time by looking up
   	   	// the parent CubeTexture
		break;
	}
	} // switch XboxResourceType

	return hRet;
}

// ---- Helper: Upload Xbox texture data into the host resource ----
// Iterates over cube faces and mip levels, copies data via Map/Lock or staging buffers
static void UploadPixelContainerMips(
	xbox::X_D3DRESOURCETYPE XboxResourceType,
	const char* ResourceTypeName,
	VAddr VirtualAddr,
	int iTextureStage,
	xbox::X_D3DFORMAT X_Format, EMUFORMAT PCFormat, UINT dwBPP,
	UINT xboxWidth, UINT xboxHeight,
	DWORD dwDepth, DWORD dwRowPitch, DWORD dwSlicePitch,
	UINT dwMipMapLevels,
	bool bCubemap, bool bSwizzled, bool bCompressed, bool bConvertTextureFormat,
#ifdef CXBX_USE_D3D11
	ComPtr<ID3D11Resource>& pNewHostResource,
	bool bHostIsDynamic
#else
	ComPtr<IDirect3DSurface>& pNewHostSurface,
	ComPtr<IDirect3DVolume>& pNewHostVolume,
	ComPtr<IDirect3DTexture>& pNewHostTexture,
	ComPtr<IDirect3DTexture>& pIntermediateHostTexture,
	ComPtr<IDirect3DVolumeTexture>& pNewHostVolumeTexture,
	ComPtr<IDirect3DVolumeTexture>& pIntermediateHostVolumeTexture,
	ComPtr<IDirect3DCubeTexture>& pNewHostCubeTexture,
	ComPtr<IDirect3DCubeTexture>& pIntermediateHostCubeTexture
#endif
)
{
	HRESULT hRet = D3D_OK;

	DWORD dwCubeFaceOffset = 0;
	_9_11(D3DCUBEMAP_FACES, int) last_face = (bCubemap) ? _9_11(D3DCUBEMAP_FACE_NEGATIVE_Z, 5) : _9_11(D3DCUBEMAP_FACE_POSITIVE_X, 0);

	// Block size only applies to compressed DXT formats
	// DXT1 block size is 8 bytes
	// Other Xbox DXT formats are 16 bytes
	DWORD blockSize = 0;
	if (bCompressed) {
		blockSize = X_Format == xbox::X_D3DFMT_DXT1 ? 8 : 16;
	}

	DWORD actualSlicePitch = dwSlicePitch;
	for (int face = _9_11(D3DCUBEMAP_FACE_POSITIVE_X, 0); face <= last_face; face++) {
		// As we iterate through mipmap levels, we'll adjust the source resource offset
		DWORD dwMipOffset = 0;
		DWORD pxMipWidth = xboxWidth; // the current mip width in pixels
		DWORD pxMipHeight = xboxHeight; // the current mip height in pixels
		DWORD pxMipDepth = dwDepth; // the current mip depth in pixels
		DWORD dwMipRowPitch = dwRowPitch; // bytes to the next row of pixels (or DXT blocks)

		for (unsigned int mipmap_level = 0; mipmap_level < dwMipMapLevels; mipmap_level++) {

			// Calculate size of this mipmap level
			DWORD numRows = pxMipHeight;

			if (bCompressed) {
				// Each row contains a 4x4 pixel DXT blocks, instead of single pixels
				// So divide by 4 to get the number of rows
				numRows = (numRows + 3) / 4;

				if (dwDepth > 1) {
					LOG_TEST_CASE("Unsupported compressed volume texture");
				}
			}

			DWORD mip2dSize = dwMipRowPitch * numRows; // the size of one layer of the mip slice
			DWORD mipSlicePitch = mip2dSize * pxMipDepth; // the total size of the mip slice (depth is only > 1 for volume textures)

#ifdef CXBX_USE_D3D11
			// Map the host resource (for DYNAMIC textures) or prepare a staging buffer (for DEFAULT textures)
			UINT Subresource = (face * dwMipMapLevels) + mipmap_level;
			// See https://docs.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-resources-subresources
			D3D11_MAPPED_SUBRESOURCE MappedResource;
			uint8_t* pStagingBuffer = nullptr; // Used for DEFAULT textures only

			// CS unswizzle path: for swizzled single-mip 2D textures (DEFAULT+UAV),
			// dispatch compute shader instead of Map/CPU-unswizzle/Unmap
			if (bSwizzled && !bConvertTextureFormat && dwMipMapLevels == 1 && pxMipDepth == 1
				&& XboxResourceType == xbox::X_D3DRTYPE_TEXTURE) {
				uint8_t *pCsSrc = (uint8_t *)VirtualAddr + dwCubeFaceOffset + dwMipOffset;
				ID3D11Texture2D* pTexture2D = static_cast<ID3D11Texture2D*>(pNewHostResource.Get());
				if (CxbxD3D11UnswizzleTexture(pTexture2D, pCsSrc, pxMipWidth, pxMipHeight, dwBPP, PCFormat)) {
					continue; // CS handled it, skip to next mip/face
				}
				// CS failed (unsupported format?) — fall back to CPU unswizzle via staging buffer + UpdateSubresource
				EmuLog(LOG_LEVEL::WARNING, "CS unswizzle failed, falling back to CPU for %ux%u bpp=%u", pxMipWidth, pxMipHeight, dwBPP);
				DWORD fallbackRowPitch = pxMipWidth * dwBPP;
				DWORD fallbackSize = fallbackRowPitch * pxMipHeight;
				uint8_t* pFallbackBuf = (uint8_t*)malloc(fallbackSize);
				EmuUnswizzleBox(pCsSrc, pxMipWidth, pxMipHeight, 1, dwBPP, pFallbackBuf, fallbackRowPitch, 0);
				g_pD3DDeviceContext->UpdateSubresource(pNewHostResource.Get(), Subresource, nullptr, pFallbackBuf, fallbackRowPitch, 0);
				free(pFallbackBuf);
				continue;
			}

			// CS palette expand path: for swizzled P8 single-mip 2D textures (DEFAULT+UAV),
			// combines unswizzle + palette lookup in a single GPU dispatch
			if (bSwizzled && X_Format == xbox::X_D3DFMT_P8 && dwMipMapLevels == 1 && pxMipDepth == 1
				&& XboxResourceType == xbox::X_D3DRTYPE_TEXTURE) {
				if (g_pXbox_Palette_Data[iTextureStage] == nullptr) {
					// Missing palette — zero-fill via UpdateSubresource (texture is DEFAULT, can't Map)
					LOG_TEST_CASE("Palettized texture bound without a palette");
					DWORD zeroRowPitch = pxMipWidth * 4;
					DWORD zeroSize = zeroRowPitch * pxMipHeight;
					uint8_t* pZeroBuf = (uint8_t*)calloc(1, zeroSize);
					g_pD3DDeviceContext->UpdateSubresource(pNewHostResource.Get(), Subresource, nullptr, pZeroBuf, zeroRowPitch, 0);
					free(pZeroBuf);
					continue;
				}
				uint8_t *pCsSrc = (uint8_t *)VirtualAddr + dwCubeFaceOffset + dwMipOffset;
				ID3D11Texture2D* pTexture2D = static_cast<ID3D11Texture2D*>(pNewHostResource.Get());
				if (CxbxD3D11ExpandPaletteTexture(pTexture2D, pCsSrc, pxMipWidth, pxMipHeight, g_pXbox_Palette_Data[iTextureStage])) {
					continue; // CS handled it, skip to next mip/face
				}
				// CS failed — fall back to CPU conversion via staging buffer
				EmuLog(LOG_LEVEL::WARNING, "CS palette expand failed, falling back to CPU for %ux%u P8", pxMipWidth, pxMipHeight);
				DWORD fallbackRowPitch = pxMipWidth * 4; // ARGB = 4 bpp output
				DWORD fallbackSize = fallbackRowPitch * pxMipHeight;
				uint8_t* pFallbackBuf = (uint8_t*)malloc(fallbackSize);
				if (ConvertD3DTextureToARGBBuffer(
					X_Format, (uint8_t *)VirtualAddr + dwCubeFaceOffset + dwMipOffset,
					pxMipWidth, pxMipHeight, dwMipRowPitch, mip2dSize,
					pFallbackBuf, fallbackRowPitch, fallbackSize,
					pxMipDepth, g_pXbox_Palette_Data[iTextureStage])) {
					// ConvertD3DTextureToARGBBuffer writes BGRA byte order,
					// but texture is R8G8B8A8_UNORM (expects RGBA bytes) — swap R↔B
					for (DWORD i = 0; i < fallbackSize; i += 4) {
						uint8_t tmp = pFallbackBuf[i];
						pFallbackBuf[i] = pFallbackBuf[i + 2];
						pFallbackBuf[i + 2] = tmp;
					}
					g_pD3DDeviceContext->UpdateSubresource(pNewHostResource.Get(), Subresource, nullptr, pFallbackBuf, fallbackRowPitch, 0);
				}
				free(pFallbackBuf);
				continue;
			}

			if (bHostIsDynamic) {
				hRet = g_pD3DDeviceContext->Map(pNewHostResource.Get(), Subresource, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
			} else {
				// For DEFAULT textures, we can't Map; allocate a staging buffer and use UpdateSubresource later
				DWORD stagingRowPitch = bCompressed ? dwMipRowPitch : pxMipWidth * dwBPP;
				DWORD stagingSize = stagingRowPitch * numRows * pxMipDepth;
				pStagingBuffer = (uint8_t*)malloc(stagingSize);
				MappedResource.pData = pStagingBuffer;
				MappedResource.RowPitch = stagingRowPitch;
				MappedResource.DepthPitch = stagingRowPitch * numRows;
				hRet = D3D_OK;
			}
#else
			// Lock the host resource
			D3DLOCKED_RECT LockedRect = {};
			D3DLOCKED_BOX LockedBox = {};
			DWORD D3DLockFlags = D3DLOCK_NOSYSLOCK;

			switch (XboxResourceType) {
			case xbox::X_D3DRTYPE_SURFACE:
				hRet = pNewHostSurface->LockRect(&LockedRect, nullptr, D3DLockFlags);
				break;
			case xbox::X_D3DRTYPE_VOLUME:
				hRet = pNewHostVolume->LockBox(&LockedBox, nullptr, D3DLockFlags);
				break;
			case xbox::X_D3DRTYPE_TEXTURE:
				hRet = pIntermediateHostTexture->LockRect(mipmap_level, &LockedRect, nullptr, D3DLockFlags);
				break;
			case xbox::X_D3DRTYPE_VOLUMETEXTURE:
				hRet = pIntermediateHostVolumeTexture->LockBox(mipmap_level, &LockedBox, nullptr, D3DLockFlags);
				break;
			case xbox::X_D3DRTYPE_CUBETEXTURE:
				hRet = pIntermediateHostCubeTexture->LockRect((D3DCUBEMAP_FACES)face, mipmap_level, &LockedRect, nullptr, D3DLockFlags);
				break;
			default:
				assert(false);
			} // switch XboxResourceType
#endif

			if (hRet != D3D_OK) {
				EmuLog(LOG_LEVEL::WARNING, "Locking host %s failed!", ResourceTypeName);
				continue; // This often happens on depth-stencil surfaces - let's ignore their copies for now
			}

			// Determine destination buffer attributes
			uint8_t *pDst;
			DWORD dwDstRowPitch;
			DWORD dwDstSlicePitch;

#ifdef CXBX_USE_D3D11
			pDst = (uint8_t *)MappedResource.pData;
			dwDstRowPitch = MappedResource.RowPitch;
			dwDstSlicePitch = MappedResource.DepthPitch;
#else
			switch (XboxResourceType) {
			case xbox::X_D3DRTYPE_VOLUME:
			case xbox::X_D3DRTYPE_VOLUMETEXTURE:
				pDst = (uint8_t *)LockedBox.pBits;
				dwDstRowPitch = LockedBox.RowPitch;
				dwDstSlicePitch = LockedBox.SlicePitch;
				break;
			default:
				pDst = (uint8_t *)LockedRect.pBits;
				dwDstRowPitch = LockedRect.Pitch;
				dwDstSlicePitch = 0;
			}
#endif
			uint8_t *pSrc = (uint8_t *)VirtualAddr + dwCubeFaceOffset + dwMipOffset;

			// If this is the final mip of the first cube face, set the cube face size
			if (face == D3DCUBEMAP_FACE_POSITIVE_X && mipmap_level >= dwMipMapLevels - 1) {
				actualSlicePitch = ROUND_UP(((UINT)pSrc + mipSlicePitch) - (UINT)VirtualAddr, X_D3DTEXTURE_CUBEFACE_ALIGNMENT);
			}

			// Copy texture data to the host resource
			CopyMipDataToHost(pDst, dwDstRowPitch, dwDstSlicePitch, pSrc,
				X_Format, dwBPP, pxMipWidth, pxMipHeight, pxMipDepth,
				dwMipRowPitch, mip2dSize,
				bConvertTextureFormat, bSwizzled, bCompressed, iTextureStage);


#ifdef CXBX_USE_D3D11
			if (bHostIsDynamic) {
				// Unmap the host resource (DYNAMIC textures)
				g_pD3DDeviceContext->Unmap(pNewHostResource.Get(), Subresource);
			} else {
				// Upload the staging buffer to the DEFAULT texture
				g_pD3DDeviceContext->UpdateSubresource(
					pNewHostResource.Get(), Subresource, nullptr,
					pStagingBuffer, MappedResource.RowPitch, MappedResource.DepthPitch);
				free(pStagingBuffer);
			}
#else
			// Unlock the host resource
			switch (XboxResourceType) {
			case xbox::X_D3DRTYPE_SURFACE:
				hRet = pNewHostSurface->UnlockRect();
				break;
			case xbox::X_D3DRTYPE_VOLUME:
				hRet = pNewHostVolume->UnlockBox();
				break;
			case xbox::X_D3DRTYPE_TEXTURE:
				hRet = pIntermediateHostTexture->UnlockRect(mipmap_level);
				break;
			case xbox::X_D3DRTYPE_VOLUMETEXTURE:
				hRet = pIntermediateHostVolumeTexture->UnlockBox(mipmap_level);
				break;
			case xbox::X_D3DRTYPE_CUBETEXTURE:
				hRet = pIntermediateHostCubeTexture->UnlockRect((D3DCUBEMAP_FACES)face, mipmap_level);
				break;
			default:
				assert(false);
			}

			if (hRet != D3D_OK) {
				EmuLog(LOG_LEVEL::WARNING, "Unlocking host %s failed!", ResourceTypeName);
			}
#endif

			// Calculate the next mipmap level dimensions
			dwMipOffset += mipSlicePitch;
			if (pxMipWidth > 1) {
				pxMipWidth /= 2;

				// Update the row pitch
				dwMipRowPitch /= 2;

				// The pitch can't be less than a block
				if (dwMipRowPitch < blockSize) {
					dwMipRowPitch = blockSize;
				}
			}

			if (pxMipHeight > 1) {
				pxMipHeight /= 2;
			}

			if (pxMipDepth > 1) {
				pxMipDepth /= 2;//this is for 3D volumeTexture mip-map, it shrinked down to 1/2 in 3 dimensions. this variable should be used.
			}
		} // for mipmap levels

		dwCubeFaceOffset += actualSlicePitch;
	} // for cube faces

#ifndef CXBX_USE_D3D11
   	// Copy from the intermediate resource to the final host resource
   	// This is necessary because CopyRects/StretchRects only works on resources in the DEFAULT pool
   	// But resources in this pool are not lockable: We must use UpdateSurface/UpdateTexture instead!
   	switch (XboxResourceType) {
   	case xbox::X_D3DRTYPE_SURFACE:
   	case xbox::X_D3DRTYPE_VOLUME:
   	   	// We didn't use a copy for Surfaces or Volumes
   	   	break;
   	case xbox::X_D3DRTYPE_TEXTURE:
   	   	g_pD3DDevice->UpdateTexture(pIntermediateHostTexture.Get(), pNewHostTexture.Get());
   	   	break;
   	case xbox::X_D3DRTYPE_VOLUMETEXTURE:
   	   	g_pD3DDevice->UpdateTexture(pIntermediateHostVolumeTexture.Get(), pNewHostVolumeTexture.Get());
   	   	break;
   	case xbox::X_D3DRTYPE_CUBETEXTURE:
   	   	g_pD3DDevice->UpdateTexture(pIntermediateHostCubeTexture.Get(), pNewHostCubeTexture.Get());
   	   	break;
   	default:
   	   	assert(false);
   	}

   	if (hRet != D3D_OK) {
   	   	EmuLog(LOG_LEVEL::WARNING, "Updating host %s failed!", ResourceTypeName);
   	}
#endif
}

// ---- Helper: Create and upload a pixel container (surface/texture/volume) ----
static void CreateHostPixelContainer(
	xbox::X_D3DResource* pResource,
	DWORD D3DUsage,
	int iTextureStage,
	VAddr VirtualAddr,
	const char* ResourceTypeName,
	xbox::X_D3DRESOURCETYPE XboxResourceType)
{
	xbox::X_D3DPixelContainer *pPixelContainer = (xbox::X_D3DPixelContainer*)pResource;
	xbox::X_D3DFORMAT X_Format = GetXboxPixelContainerFormat(pPixelContainer);
	D3DPOOL D3DPool = D3DPOOL_DEFAULT; // TODO : Nuance D3DPOOL where/when needed

	if (EmuXBFormatIsDepthBuffer(X_Format)) {
		D3DUsage |= D3DUSAGE_DEPTHSTENCIL;
	}
	else if (pPixelContainer == g_pXbox_RenderTarget) {
		if (EmuXBFormatIsRenderTarget(X_Format))
			D3DUsage |= D3DUSAGE_RENDERTARGET;
		else
			EmuLog(LOG_LEVEL::WARNING, "Updating RenderTarget %s with an incompatible format!", ResourceTypeName);
	}
	// Determine the host format
	bool bConvertTextureFormat;
	EMUFORMAT PCFormat = ResolveHostFormat(X_Format, XboxResourceType, D3DUsage, ResourceTypeName, bConvertTextureFormat);

	// Update D3DPool accordingly to the active D3DUsage flags
	if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
		D3DPool = D3DPOOL_DEFAULT;
	}
	if (D3DUsage & D3DUSAGE_RENDERTARGET) {
		D3DPool = D3DPOOL_DEFAULT;
	}
	if (D3DUsage & D3DUSAGE_DYNAMIC) {
		D3DPool = D3DPOOL_DEFAULT;
	}

	// Interpret Width/Height/BPP
	bool bCubemap = pPixelContainer->Format & X_D3DFORMAT_CUBEMAP;
	bool bSwizzled = EmuXBFormatIsSwizzled(X_Format);
	bool bCompressed = EmuXBFormatIsCompressed(X_Format);
	UINT dwBPP = EmuXBFormatBytesPerPixel(X_Format);
	UINT dwMipMapLevels = CxbxGetPixelContainerMipMapLevels(pPixelContainer);
	UINT xboxWidth, xboxHeight, dwDepth, dwRowPitch, dwSlicePitch;

	// Interpret Width/Height/BPP
	CxbxGetPixelContainerMeasures(pPixelContainer, 0, &xboxWidth, &xboxHeight, &dwDepth, &dwRowPitch, &dwSlicePitch);

	// Host width and height dimensions
	UINT hostWidth = xboxWidth;
	UINT hostHeight = xboxHeight;
	// Upscale rendertargets and depth surfaces
	if (D3DUsage & (X_D3DUSAGE_DEPTHSTENCIL | X_D3DUSAGE_RENDERTARGET)) {
		hostWidth *= g_RenderUpscaleFactor;
		hostHeight *= g_RenderUpscaleFactor;
	}

	// Each mip-map level is 1/2 the size of the previous level
	// D3D9 forbids creation of a texture with more mip-map levels than it is divisible
	// EG: A 256x256 texture cannot have more than 8 levels, since that would create a texture smaller than 1x1
	// Because of this, we need to cap dwMipMapLevels when required
	if (dwMipMapLevels > 0) {
		// Calculate how many mip-map levels it takes to get to a texture of 1 pixels in either dimension
		UINT highestMipMapLevel = 1;
		UINT width = xboxWidth; UINT height = xboxHeight;
		while (width > 1 || height > 1) {
			width /= 2;
			height /= 2;
			highestMipMapLevel++;
		}

		// If the desired mip-map level was higher than the maximum possible, cap it
		// Test case: Shin Megami Tensei: Nine
		if (dwMipMapLevels > highestMipMapLevel) {
			LOG_TEST_CASE("Too many mip-map levels");
			dwMipMapLevels = highestMipMapLevel;
		}
	}

	if (dwDepth != 1) {
		LOG_TEST_CASE("CreateHostPixelContainer : Depth != 1");
	}


#ifdef CXBX_USE_D3D11
	ComPtr<ID3D11Resource> pNewHostResource;
	bool bHostIsDynamic = false;
#else
	// One of these will be created : each also has an intermediate copy to allow UpdateTexture to work
   	// This means we don't need to lock the GPU resource anymore, so we can use D3DPOOL_DEFAULT to allow Stretch/CopyRects to work!
	ComPtr<IDirect3DSurface> pNewHostSurface; // for X_D3DRTYPE_SURFACE
	ComPtr<IDirect3DVolume> pNewHostVolume; // for X_D3DRTYPE_VOLUME
	ComPtr<IDirect3DTexture> pNewHostTexture; // for X_D3DRTYPE_TEXTURE
	ComPtr<IDirect3DTexture> pIntermediateHostTexture;
	ComPtr<IDirect3DVolumeTexture> pNewHostVolumeTexture; // for X_D3DRTYPE_VOLUMETEXTURE
	ComPtr<IDirect3DVolumeTexture> pIntermediateHostVolumeTexture;
	ComPtr<IDirect3DCubeTexture> pNewHostCubeTexture; // for X_D3DRTYPE_CUBETEXTURE
	ComPtr<IDirect3DCubeTexture> pIntermediateHostCubeTexture;
#endif

	CreateGpuPixelContainerResource(
		XboxResourceType,
		hostWidth, hostHeight, dwDepth, dwMipMapLevels,
		PCFormat,
		D3DUsage, D3DPool,
		bSwizzled, bConvertTextureFormat, X_Format,
		pResource, ResourceTypeName, iTextureStage,
#ifdef CXBX_USE_D3D11
		pNewHostResource, bHostIsDynamic
#else
		pNewHostSurface, pNewHostVolume,
		pNewHostTexture, pIntermediateHostTexture,
		pNewHostVolumeTexture, pIntermediateHostVolumeTexture,
		pNewHostCubeTexture, pIntermediateHostCubeTexture
#endif
	);

   	// If this resource is a render target or depth stencil, don't attempt to lock/copy it as it won't work anyway
   	// In this case, we simply return
   	if (D3DUsage & D3DUSAGE_RENDERTARGET || D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
   	   	return;
   	}

	UploadPixelContainerMips(
		XboxResourceType, ResourceTypeName, VirtualAddr, iTextureStage,
		X_Format, PCFormat, dwBPP,
		xboxWidth, xboxHeight,
		dwDepth, dwRowPitch, dwSlicePitch,
		dwMipMapLevels,
		bCubemap, bSwizzled, bCompressed, bConvertTextureFormat,
#ifdef CXBX_USE_D3D11
		pNewHostResource, bHostIsDynamic
#else
		pNewHostSurface, pNewHostVolume,
		pNewHostTexture, pIntermediateHostTexture,
		pNewHostVolumeTexture, pIntermediateHostVolumeTexture,
		pNewHostCubeTexture, pIntermediateHostCubeTexture
#endif
	);


	// Debug resource dumping
//#define _DEBUG_DUMP_TEXTURE_REGISTER "D:\\"
#ifdef _DEBUG_DUMP_TEXTURE_REGISTER
	bool bDumpConvertedTextures = true;  // TODO : Make this a runtime changeable setting
	if (bDumpConvertedTextures) {
		char szFilePath[MAX_PATH];

		switch (XboxResourceType) {
		case xbox::X_D3DRTYPE_SURFACE: {
			static int dwDumpSurface = 0;
			sprintf(szFilePath, _DEBUG_DUMP_TEXTURE_REGISTER "%.03d-Surface%.03d.dds", X_Format, dwDumpSurface++);
			D3DXSaveSurfaceToFileA(szFilePath, D3DXIFF_DDS, pNewHostSurface, nullptr, nullptr);
			break;
		}
		case xbox::X_D3DRTYPE_VOLUME: {
			// TODO
			break;
		}
		case xbox::X_D3DRTYPE_TEXTURE: {
			static int dwDumpTexure = 0;
			sprintf(szFilePath, _DEBUG_DUMP_TEXTURE_REGISTER "%.03d-Texture%.03d.dds", X_Format, dwDumpTexure++);
			D3DXSaveTextureToFileA(szFilePath, D3DXIFF_DDS, pNewHostTexture, nullptr);
			break;
		}
		case xbox::X_D3DRTYPE_VOLUMETEXTURE: {
			// TODO
			break;
		}
		case xbox::X_D3DRTYPE_CUBETEXTURE: {
			static int dwDumpCubeTexture = 0;
			for (unsigned int face = _9_11(D3DCUBEMAP_FACE_POSITIVE_X, 0); face <= _9_11(D3DCUBEMAP_FACE_NEGATIVE_Z, 5); face++) {
				IDirect3DSurface *pSurface;
				if (D3D_OK == pNewHostCubeTexture->GetCubeMapSurface((_9_11(D3DCUBEMAP_FACES, int))face, 0, &pSurface)) {
					sprintf(szFilePath, _DEBUG_DUMP_TEXTURE_REGISTER "%.03d-CubeTexure%.03d-%d.dds", X_Format, dwDumpCubeTexture, face);
					D3DXSaveSurfaceToFileA(szFilePath, D3DXIFF_DDS, pSurface, nullptr, nullptr);
					pSurface->Release();
				}
			}
			dwDumpCubeTexture++;
			break;
		}
		} // switch XboxResourceType
	}
#endif

}
// Was patch: IDirect3DResource8_Register
void CreateHostResource(xbox::X_D3DResource *pResource, DWORD D3DUsage, int iTextureStage, DWORD dwSize)
{
	if (pResource == xbox::zeroptr)
		return;

	// Determine the resource type name
	const char *ResourceTypeName;
	xbox::X_D3DRESOURCETYPE XboxResourceType = GetXboxD3DResourceType(pResource);

	switch (XboxResourceType) {
	case xbox::X_D3DRTYPE_NONE: ResourceTypeName = "None"; break;
	case xbox::X_D3DRTYPE_SURFACE: ResourceTypeName = "Surface"; break;
	case xbox::X_D3DRTYPE_VOLUME: ResourceTypeName = "Volume"; break;
	case xbox::X_D3DRTYPE_TEXTURE: ResourceTypeName = "Texture"; break;
	case xbox::X_D3DRTYPE_VOLUMETEXTURE: ResourceTypeName = "VolumeTexture"; break;
	case xbox::X_D3DRTYPE_CUBETEXTURE: ResourceTypeName = "CubeTexture"; break;
	case xbox::X_D3DRTYPE_VERTEXBUFFER: ResourceTypeName = "VertexBuffer"; break;
	case xbox::X_D3DRTYPE_INDEXBUFFER: ResourceTypeName = "IndexBuffer"; break;
	case xbox::X_D3DRTYPE_PUSHBUFFER: ResourceTypeName = "PushBuffer"; break;
	case xbox::X_D3DRTYPE_PALETTE: ResourceTypeName = "Palette"; break;
	case xbox::X_D3DRTYPE_FIXUP: ResourceTypeName = "Fixup"; break;
	default:
		EmuLog(LOG_LEVEL::WARNING, "CreateHostResource :-> Unrecognized Xbox Resource Type 0x%.08X", XboxResourceType);
		return;
	}

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pResource)
		LOG_FUNC_ARG(D3DUsage)
		LOG_FUNC_ARG(iTextureStage)
		LOG_FUNC_ARG(dwSize)
		LOG_FUNC_ARG(ResourceTypeName)
		LOG_FUNC_END;

	// Retrieve and test the xbox resource buffer address
	VAddr VirtualAddr = (VAddr)GetDataFromXboxResource(pResource);
	if ((VirtualAddr & ~PHYSICAL_MAP_BASE) == 0) {
		// TODO: Fix or handle this situation..?
		LOG_TEST_CASE("CreateHostResource : VirtualAddr == 0");
		// This is probably an unallocated resource, mapped into contiguous memory (0x80000000 OR 0xF0000000)
		EmuLog(LOG_LEVEL::WARNING, "CreateHostResource :-> %s carries no data - skipping conversion", ResourceTypeName);
		return;
	}

	switch (XboxResourceType) {
	case xbox::X_D3DRTYPE_NONE: {
		break;
	}

	case xbox::X_D3DRTYPE_SURFACE:
		if (TryResolveParentSurface(pResource, D3DUsage, iTextureStage))
			return;
		[[fallthrough]];
	case xbox::X_D3DRTYPE_VOLUME:
		if (XboxResourceType == xbox::X_D3DRTYPE_VOLUME) {
			if (TryResolveParentVolume(pResource, D3DUsage, iTextureStage))
				return;
		}
		[[fallthrough]];
	case xbox::X_D3DRTYPE_TEXTURE:
	case xbox::X_D3DRTYPE_VOLUMETEXTURE:
	case xbox::X_D3DRTYPE_CUBETEXTURE: {
		CreateHostPixelContainer(pResource, D3DUsage, iTextureStage, VirtualAddr, ResourceTypeName, XboxResourceType);
		break;
	}

	// case X_D3DRTYPE_VERTEXBUFFER: { break; } // TODO

	// case X_D3DRTYPE_INDEXBUFFER: { break; } // TODO

	case xbox::X_D3DRTYPE_PUSHBUFFER: {
		xbox::X_D3DPushBuffer *pPushBuffer = (xbox::X_D3DPushBuffer*)pResource;

		// create push buffer
		dwSize = g_VMManager.QuerySize(VirtualAddr);
		if (dwSize == 0) {
			// TODO: once this is known to be working, remove the warning
			EmuLog(LOG_LEVEL::WARNING, "Push buffer allocation size unknown");
			pPushBuffer->Lock = X_D3DRESOURCE_LOCK_FLAG_NOSIZE;
			break;
		}

		EmuLog(LOG_LEVEL::DEBUG, "CreateHostResource : Successfully created %S (0x%.08X, 0x%.08X, 0x%.08X)", ResourceTypeName, pResource->Data, pPushBuffer->Size, pPushBuffer->AllocationSize);
		break;
	}

	//case X_D3DRTYPE_PALETTE: { break;	}

	case xbox::X_D3DRTYPE_FIXUP: {
		xbox::X_D3DFixup *pFixup = (xbox::X_D3DFixup*)pResource;

		EmuLog(LOG_LEVEL::WARNING, "X_D3DRTYPE_FIXUP is not yet supported\n"
			"0x%.08X (pFixup->Common) \n"
			"0x%.08X (pFixup->Data)   \n"
			"0x%.08X (pFixup->Lock)   \n"
			"0x%.08X (pFixup->Run)    \n"
			"0x%.08X (pFixup->Next)   \n"
			"0x%.08X (pFixup->Size)   \n", pFixup->Common, pFixup->Data, pFixup->Lock, pFixup->Run, pFixup->Next, pFixup->Size);
		break;
	}
   	} // switch XboxResourceType
}

D3DXVECTOR4 toVector(D3DCOLOR color) {
	D3DXVECTOR4 v;
	// ARGB to XYZW
	v.w = (color >> 24 & 0xFF) / 255.f;
	v.x = (color >> 16 & 0xFF) / 255.f;
	v.y = (color >> 8 & 0xFF) / 255.f;
	v.z = (color >> 0 & 0xFF) / 255.f;
	return v;
}

D3DXVECTOR4 toVector(xbox::X_D3DCOLORVALUE val) {
	return D3DXVECTOR4(val.r, val.g, val.b, val.a);
}

