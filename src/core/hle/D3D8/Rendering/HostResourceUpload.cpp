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
#include "Backend\Backend_D3D11.h"

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

// ---- Helper: Upload Xbox texture data into the host resource ----
// Iterates over cube faces and mip levels, copies data via Map/Lock or staging buffers
void UploadPixelContainerMips(
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

			// CS format conversion path: for single-mip 2D textures that need format conversion,
			// dispatch compute shader to unswizzle (if needed) + decode → R8G8B8A8_UNORM
			{
				UINT fmtType = CxbxGetFormatConvertType(X_Format);
				if (fmtType != 0 && bConvertTextureFormat && dwMipMapLevels == 1 && pxMipDepth == 1
					&& XboxResourceType == xbox::X_D3DRTYPE_TEXTURE) {
					uint8_t *pCsSrc = (uint8_t *)VirtualAddr + dwCubeFaceOffset + dwMipOffset;
					ID3D11Texture2D* pTexture2D = static_cast<ID3D11Texture2D*>(pNewHostResource.Get());
					if (CxbxD3D11FormatConvertTexture(pTexture2D, pCsSrc, pxMipWidth, pxMipHeight,
							dwBPP, fmtType, bSwizzled, dwMipRowPitch)) {
						continue; // CS handled it
					}
					EmuLog(LOG_LEVEL::WARNING, "CS format convert failed, falling back to CPU for %ux%u fmt=0x%02X",
						pxMipWidth, pxMipHeight, X_Format);
					// Fall through to CPU path
				}
			}

			if (bHostIsDynamic) {
				hRet = g_pD3DDeviceContext->Map(pNewHostResource.Get(), Subresource, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
			} else {
				// For DEFAULT textures, we can't Map; allocate a staging buffer and use UpdateSubresource later
				// When converting to ARGB, output is 4 bytes per pixel regardless of source BPP
				DWORD stagingBPP = bConvertTextureFormat ? 4u : dwBPP;
				DWORD stagingRowPitch = bCompressed ? dwMipRowPitch : pxMipWidth * stagingBPP;
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

