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


xbox::X_D3DRESOURCETYPE GetXboxD3DResourceType(const xbox::X_D3DResource *pXboxResource)
{
	DWORD Type = GetXboxCommonResourceType(pXboxResource);
	switch (Type)
	{
	case X_D3DCOMMON_TYPE_VERTEXBUFFER:
		return xbox::X_D3DRTYPE_VERTEXBUFFER;
	case X_D3DCOMMON_TYPE_INDEXBUFFER:
		return xbox::X_D3DRTYPE_INDEXBUFFER;
	case X_D3DCOMMON_TYPE_PUSHBUFFER:
		return xbox::X_D3DRTYPE_PUSHBUFFER;
	case X_D3DCOMMON_TYPE_PALETTE:
		return xbox::X_D3DRTYPE_PALETTE;
	case X_D3DCOMMON_TYPE_TEXTURE:
	{
		DWORD Format = ((xbox::X_D3DPixelContainer *)pXboxResource)->Format;
		if (Format & X_D3DFORMAT_CUBEMAP)
			return xbox::X_D3DRTYPE_CUBETEXTURE;

		if (GetXboxPixelContainerDimensionCount((xbox::X_D3DPixelContainer *)pXboxResource) > 2)
			return xbox::X_D3DRTYPE_VOLUMETEXTURE;

		return xbox::X_D3DRTYPE_TEXTURE;
	}
	case X_D3DCOMMON_TYPE_SURFACE:
	{
		if (GetXboxPixelContainerDimensionCount((xbox::X_D3DPixelContainer *)pXboxResource) > 2)
			return xbox::X_D3DRTYPE_VOLUME;

		return xbox::X_D3DRTYPE_SURFACE;
	}
	case X_D3DCOMMON_TYPE_FIXUP:
		return xbox::X_D3DRTYPE_FIXUP;
	}

	return xbox::X_D3DRTYPE_NONE;
}

// This can be used to determine if resource Data adddresses
// need the PHYSICAL_MAP_BASE  bit set or cleared
inline bool IsResourceTypeGPUReadable(const DWORD ResourceType)
{
	switch (ResourceType) {
	case X_D3DCOMMON_TYPE_VERTEXBUFFER:
		return true;
	case X_D3DCOMMON_TYPE_INDEXBUFFER:
		/// assert(false); // Index buffers are not allowed to be registered
		break;
	case X_D3DCOMMON_TYPE_PUSHBUFFER:
		return false;
	case X_D3DCOMMON_TYPE_PALETTE:
		return true;
	case X_D3DCOMMON_TYPE_TEXTURE:
		return true;
	case X_D3DCOMMON_TYPE_SURFACE:
		return true;
	case X_D3DCOMMON_TYPE_FIXUP:
		// assert(false); // Fixup's are not allowed to be registered
		break;
	default:
		CxbxrAbort("Unhandled resource type");
	}

	return false;
}

inline bool IsPaletizedTexture(const xbox::dword_xt XboxPixelContainer_Format)
{
	return GetXboxPixelContainerFormat(XboxPixelContainer_Format) == xbox::X_D3DFMT_P8;
}

#if 0 // unused
inline bool IsYuvSurfaceOrTexture(const xbox::X_D3DResource* pXboxResource)
{
	if (GetXboxPixelContainerFormat((xbox::X_D3DPixelContainer *)pXboxResource) == xbox::X_D3DFMT_YUY2)
		return true;

	return false;
}
#endif

#if 0 // unused
inline bool IsXboxResourceLocked(const xbox::X_D3DResource *pXboxResource)
{
	bool result = !!(pXboxResource->Common & X_D3DCOMMON_ISLOCKED);
	return result;
}
#endif

#if 0 // unused
inline bool IsXboxResourceD3DCreated(const xbox::X_D3DResource *pXboxResource)
{
	bool result = !!(pXboxResource->Common & X_D3DCOMMON_D3DCREATED);
	return result;
}
#endif

void *GetDataFromXboxResource(xbox::X_D3DResource *pXboxResource)
{
	// Don't pass in unassigned Xbox resources
	if (pXboxResource == xbox::zeroptr)
		return nullptr;

	xbox::addr_xt pData = pXboxResource->Data;
	if (pData == xbox::zero)
		return nullptr;

	DWORD dwCommonType = GetXboxCommonResourceType(pXboxResource);
	if (IsResourceTypeGPUReadable(dwCommonType))
		pData |= PHYSICAL_MAP_BASE;

	return (uint8_t*)pData;
}

DWORD D3DUSAGE_INVALID = -1;

typedef struct _resource_info_t {
	
	ComPtr<IDirect3DResource> pHostResource;
	EMUFORMAT HostFormat = EMUFMT_UNKNOWN;
	DWORD HostUsage = D3DUSAGE_INVALID;
	DWORD dwXboxResourceType = 0;
	void* pXboxData = xbox::zeroptr;
	size_t szXboxDataSize = 0;
	uint64_t hash = 0;
	bool forceRehash = false;
	std::chrono::time_point<std::chrono::steady_clock> nextHashTime;
	std::chrono::milliseconds hashLifeTime = 1ms;
    std::chrono::time_point<std::chrono::steady_clock> lastUpdate;
} resource_info_t;

typedef std::unordered_map<resource_key_t, resource_info_t, resource_key_hash> resource_cache_t;
resource_cache_t g_Cxbx_Cached_Direct3DResources;
resource_cache_t g_Cxbx_Cached_PaletizedTextures;

bool IsResourceAPixelContainer(xbox::dword_xt XboxResource_Common)
{
	DWORD Type = GetXboxCommonResourceType(XboxResource_Common);
	switch (Type)
	{
	case X_D3DCOMMON_TYPE_TEXTURE:
	case X_D3DCOMMON_TYPE_SURFACE:
		return true;
	}

	return false;
}

bool IsResourceAPixelContainer(xbox::X_D3DResource* pXboxResource)
{
	// Don't pass in unassigned Xbox resources
	assert(pXboxResource != xbox::zeroptr);

	return IsResourceAPixelContainer(pXboxResource->Common);
}

resource_cache_t& GetResourceCache(resource_key_t& key)
{
	return IsResourceAPixelContainer(key.Common) && IsPaletizedTexture(key.Format)
		? g_Cxbx_Cached_PaletizedTextures : g_Cxbx_Cached_Direct3DResources;
}

resource_key_t GetHostResourceKey(xbox::X_D3DResource* pXboxResource, int iTextureStage = -1)
{
	resource_key_t key = {};
	if (pXboxResource != xbox::zeroptr) {
		// Initially, don't base the key on the address of the resource, but on it's uniquely identifying values
		key.Data = pXboxResource->Data;
		key.Common = pXboxResource->Common & CXBX_D3DCOMMON_IDENTIFYING_MASK;
		if (IsResourceAPixelContainer(pXboxResource)) {
			// Pixel containers have more values they must be identified by:
			auto pPixelContainer = (xbox::X_D3DPixelContainer*)pXboxResource;
			key.Format = pPixelContainer->Format;
			key.Size = pPixelContainer->Size;
			// For paletized textures, include the current palette hash as well
			if (IsPaletizedTexture(pPixelContainer->Format)) {
				if (iTextureStage < 0) {
					// ForceResourceRehash (called by Lock[23]DSurface) could hit this (not knowing the texture-stage)
					LOG_TEST_CASE("Unknown texture stage!");
				} else {
					assert(iTextureStage < xbox::X_D3DTS_STAGECOUNT);
					// Protect for when this gets hit before an actual palette is set
					if (g_Xbox_Palette_Size[iTextureStage] > 0) {
						// This caters for palette changes (only the active one will be used,
						// any intermediate changes have no effect). Obsolete palette texture
						// conversions will be pruned from g_Cxbx_Cached_PaletizedTextures
						key.PaletteHash = ComputeHash(g_pXbox_Palette_Data[iTextureStage], g_Xbox_Palette_Size[iTextureStage]);
					}
				}
			}
		} else {
			// For other resource types, do include their Xbox resource address (TODO : come up with something better)
			key.ResourceAddr = (xbox::addr_xt)pXboxResource;
		}
	}

	return key;
}

void FreeHostResource(resource_key_t key)
{
	// Release the host resource and remove it from the list
	auto& ResourceCache = GetResourceCache(key);
	ResourceCache.erase(key);
}

void ClearResourceCache(resource_cache_t& ResourceCache)
{
	ResourceCache.clear();
}

void PrunePaletizedTexturesCache()
{
	// TODO : Implement a better cache eviction algorithm (like least-recently used, or just at-random)
	// Poor mans cache eviction policy: just clear it once it overflows
	if (g_Cxbx_Cached_PaletizedTextures.size() >= 1500) {
		ClearResourceCache(g_Cxbx_Cached_PaletizedTextures);
	}
}

void ForceResourceRehash(xbox::X_D3DResource* pXboxResource)
{
	auto key = GetHostResourceKey(pXboxResource); // Note : iTextureStage is unknown here!
	auto& ResourceCache = GetResourceCache(key);
	auto it = ResourceCache.find(key);
	if (it != ResourceCache.end() && it->second.pHostResource) {
		it->second.forceRehash = true;
	}
}

IDirect3DResource *GetHostResource(xbox::X_D3DResource *pXboxResource, DWORD D3DUsage = 0, int iTextureStage = -1)
{
	if (pXboxResource == xbox::zeroptr || pXboxResource->Data == xbox::zero)
		return nullptr;

	EmuVerifyResourceIsRegistered(pXboxResource, D3DUsage, iTextureStage, /*dwSize=*/0);

	auto key = GetHostResourceKey(pXboxResource, iTextureStage);
	auto& ResourceCache = GetResourceCache(key);
	auto it = ResourceCache.find(key);
	if (it == ResourceCache.end() || !it->second.pHostResource) {
		EmuLog(LOG_LEVEL::WARNING, "GetHostResource: Resource not registered or does not have a host counterpart!");
		return nullptr;
	}

	return it->second.pHostResource.Get();
}

// Forward declaration of CxbxGetPixelContainerMeasures to prevent
// polluting the diff too much by reshuffling functions around
void CxbxGetPixelContainerMeasures
(
	xbox::X_D3DPixelContainer *pPixelContainer,
	DWORD dwMipMapLevel,
	UINT *pWidth,
	UINT *pHeight,
	UINT *pDepth,
	UINT *pRowPitch,
	UINT *pSlicePitch
);

size_t GetXboxResourceSize(xbox::X_D3DResource* pXboxResource)
{
	// TODO: Smart size calculation based around format of resource
	if (IsResourceAPixelContainer(pXboxResource)) {
		unsigned int Width, Height, Depth, RowPitch, SlicePitch;
		// TODO : Accumulate all mipmap levels!!!
		CxbxGetPixelContainerMeasures(
			(xbox::X_D3DPixelContainer*)pXboxResource,
			0, // dwMipMapLevel
			&Width,
			&Height,
			&Depth,
			&RowPitch,
			&SlicePitch
		);

		return SlicePitch * Depth;
	} else {
		// Fallback to querying the allocation size, if no other calculation was present
		return xbox::MmQueryAllocationSize(GetDataFromXboxResource(pXboxResource));
	}
	
}

bool HostResourceRequiresUpdate(resource_key_t key, xbox::X_D3DResource* pXboxResource, DWORD dwSize)
{
	if (pXboxResource == nullptr) {
		return false;
	}

	// Currently, we only dynamically update Textures and Surfaces, so if our resource
	// isn't of these types, do nothing
	if (!IsResourceAPixelContainer(pXboxResource)) {
		return false;
	}

	auto& ResourceCache = GetResourceCache(key);
	auto it = ResourceCache.find(key);
	if (it == ResourceCache.end()) {
		return false;
	}

	// If the resource size got bigger, we need to re-create it
	// if it got smaller, just hashing will suffice
	if (dwSize > it->second.szXboxDataSize) {
		return true;
	}

	// If the resource type changed, we need to re-create it
	if (it->second.dwXboxResourceType != GetXboxCommonResourceType(pXboxResource)) {
		return true;
	}

	bool modified = false;

	auto now = std::chrono::steady_clock::now();
	if (now > it->second.nextHashTime || it->second.forceRehash) {
		uint64_t oldHash = it->second.hash;
		it->second.hash = ComputeHash(it->second.pXboxData, it->second.szXboxDataSize);

		if (it->second.hash != oldHash) {
			// The data changed, so reset the hash lifetime
			it->second.hashLifeTime = 1ms;
            it->second.lastUpdate = now;
			modified = true;
		} else if (it->second.lastUpdate + 1000ms < now) {
			// The data did not change, so increase the hash lifetime
			// TODO: choose a sensible upper limit
			if (it->second.hashLifeTime < 1000ms) {
				it->second.hashLifeTime += 10ms;
			}
		}
		
		it->second.forceRehash = false;
	}

	// Update the next hash time based on the hash lifetime
	it->second.nextHashTime = now + it->second.hashLifeTime;

	return modified;
}

void SetHostResource(xbox::X_D3DResource* pXboxResource, IDirect3DResource* pHostResource, int iTextureStage = -1, DWORD D3DUsage = D3DUSAGE_INVALID, EMUFORMAT PCFormat = EMUFMT_UNKNOWN)
{
	auto key = GetHostResourceKey(pXboxResource, iTextureStage);
	auto& ResourceCache = GetResourceCache(key);
	auto& resourceInfo = ResourceCache[key];	// Implicitely inserts a new entry if not already existing

	if (resourceInfo.pHostResource) {
		EmuLog(LOG_LEVEL::WARNING, "SetHostResource: Overwriting an existing host resource");
	}

	// Increments reference count
	resourceInfo.pHostResource = pHostResource;
	resourceInfo.dwXboxResourceType = GetXboxCommonResourceType(pXboxResource);
	resourceInfo.pXboxData = GetDataFromXboxResource(pXboxResource);
	resourceInfo.szXboxDataSize = GetXboxResourceSize(pXboxResource);
	resourceInfo.hash = ComputeHash(resourceInfo.pXboxData, resourceInfo.szXboxDataSize);
	resourceInfo.hashLifeTime = 1ms;
	resourceInfo.lastUpdate = std::chrono::steady_clock::now();
	resourceInfo.nextHashTime = resourceInfo.lastUpdate + resourceInfo.hashLifeTime;
	resourceInfo.forceRehash = false;
	if (PCFormat == EMUFMT_UNKNOWN) {
#ifdef CXBX_USE_D3D11
		D3D11_TEXTURE2D_DESC tex2dDesc = {};
		D3D11_TEXTURE3D_DESC tex3dDesc = {};
		switch (resourceInfo.dwXboxResourceType) {// TODO : Better check pHostResource class type
		case xbox::X_D3DRTYPE_SURFACE:
		case xbox::X_D3DRTYPE_TEXTURE:
		case xbox::X_D3DRTYPE_CUBETEXTURE:
			((ID3D11Texture2D*)pHostResource)->GetDesc(&tex2dDesc);
			PCFormat = tex2dDesc.Format;
			break;
		case xbox::X_D3DRTYPE_VOLUMETEXTURE:
			((ID3D11Texture3D*)pHostResource)->GetDesc(&tex3dDesc);
			PCFormat = tex3dDesc.Format;
			break;
		}
#else
		HRESULT hRet = STATUS_INVALID_PARAMETER; // Default to an error condition, so we can use D3D_OK to check for success
		D3DSURFACE_DESC surfaceDesc;
		D3DVOLUME_DESC volumeDesc;
		UINT Level = 0; // TODO : When should Level every be something other than zero, and if so : what other value?
		switch (resourceInfo.dwXboxResourceType) {// TODO : Better check pHostResource class type
		case xbox::X_D3DRTYPE_SURFACE:
			hRet = ((IDirect3DSurface9*)pHostResource)->GetDesc(&surfaceDesc);
			break;
		case xbox::X_D3DRTYPE_TEXTURE:
			hRet = ((IDirect3DTexture9*)pHostResource)->GetLevelDesc(Level, &surfaceDesc);
			break;
		case xbox::X_D3DRTYPE_VOLUMETEXTURE: {
			hRet = ((IDirect3DVolumeTexture9*)pHostResource)->GetLevelDesc(Level, &volumeDesc);
			break; }
		case xbox::X_D3DRTYPE_CUBETEXTURE:
			hRet = ((IDirect3DCubeTexture9*)pHostResource)->GetLevelDesc(Level, &surfaceDesc);
			break;
		}

		if (SUCCEEDED(hRet)) {
			PCFormat = (resourceInfo.dwXboxResourceType == xbox::X_D3DRTYPE_VOLUMETEXTURE) ? volumeDesc.Format : surfaceDesc.Format;
		}
#endif
	}

	resourceInfo.HostFormat = PCFormat;
	resourceInfo.HostUsage = D3DUsage;
}

// Type-safe wrappers for SetHostResource — cast specific D3D types to IDirect3DResource
inline void SetHostSurface(xbox::X_D3DResource* pXboxResource, IDirect3DSurface* pHostSurface, int iTextureStage = -1)
{
	SetHostResource(pXboxResource, (IDirect3DResource*)pHostSurface, iTextureStage);
}

inline void SetHostVolume(xbox::X_D3DResource* pXboxResource, IDirect3DVolume* pHostVolume, int iTextureStage = -1)
{
	SetHostResource(pXboxResource, (IDirect3DResource*)pHostVolume, iTextureStage);
}

inline void SetHostTexture(xbox::X_D3DResource* pXboxResource, IDirect3DTexture* pHostTexture, int iTextureStage = -1)
{
	SetHostResource(pXboxResource, (IDirect3DResource*)pHostTexture, iTextureStage);
}

inline void SetHostVolumeTexture(xbox::X_D3DResource* pXboxResource, IDirect3DVolumeTexture* pHostVolumeTexture, int iTextureStage = -1)
{
	SetHostResource(pXboxResource, (IDirect3DResource*)pHostVolumeTexture, iTextureStage);
}

inline void SetHostCubeTexture(xbox::X_D3DResource* pXboxResource, IDirect3DCubeTexture* pHostCubeTexture, int iTextureStage = -1)
{
	SetHostResource(pXboxResource, (IDirect3DResource*)pHostCubeTexture, iTextureStage);
}

IDirect3DSurface *GetHostSurface(xbox::X_D3DResource *pXboxResource, DWORD D3DUsage = 0)
{
	if (pXboxResource == xbox::zeroptr)
		return nullptr;

	if (GetXboxCommonResourceType(pXboxResource) != X_D3DCOMMON_TYPE_SURFACE) // Allows breakpoint below
		assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_SURFACE);

	return (IDirect3DSurface*) GetHostResource(pXboxResource, D3DUsage);
}

IDirect3DBaseTexture *GetHostBaseTexture(xbox::X_D3DResource *pXboxResource, DWORD D3DUsage = 0, int iTextureStage = 0)
{
	if (pXboxResource == xbox::zeroptr)
		return nullptr;

	if (GetXboxCommonResourceType(pXboxResource) != X_D3DCOMMON_TYPE_TEXTURE) { // Allows breakpoint below
		// test-case : Burnout and Outrun 2006 hit this case (retrieving a surface instead of a texture)
		// TODO : Surfaces can be set in the texture stages, instead of textures - see preparations in CxbxConvertXboxSurfaceToHostTexture
		// We'll need to wrap the surface somehow before using it as a texture
		LOG_TEST_CASE("GetHostBaseTexture called on a non-texture object");
		return nullptr;
		// Note : We'd like to remove the above and do the following instead,
		// but we can't yet since that seems to cause a "CreateCubeTexture Failed!"
		// regression. The root cause for that seems to stem from the X_D3DRTYPE_SURFACE
		// handling in CreateHostResource.
		//assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_TEXTURE);
	}

	return (IDirect3DBaseTexture*)GetHostResource(pXboxResource, D3DUsage, iTextureStage);
}

#if 0 // unused
IDirect3DTexture *GetHostTexture(xbox::X_D3DResource *pXboxResource, int iTextureStage = 0)
{
	if (pXboxResource == xbox::zeroptr)
		return nullptr;

	return (IDirect3DTexture *)GetHostBaseTexture(pXboxResource, 0, iTextureStage);

	// TODO : Check for 1 face (and 2 dimensions)?
}
#endif

IDirect3DVolumeTexture *GetHostVolumeTexture(xbox::X_D3DResource *pXboxResource, int iTextureStage = 0)
{
	return (IDirect3DVolumeTexture *)GetHostBaseTexture(pXboxResource, 0, iTextureStage);

	// TODO : Check for 1 face (and 2 dimensions)?
}

#if 0 // unused
IDirect3DIndexBuffer *GetHostIndexBuffer(xbox::X_D3DResource *pXboxResource)
{
	if (pXboxResource == xbox::zeroptr)
		return nullptr;

	assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_INDEXBUFFER);

	return (IDirect3DIndexBuffer*)GetHostResource(pXboxResource);
}
#endif

int XboxD3DPaletteSizeToBytes(const xbox::X_D3DPALETTESIZE Size)
{
/*
	static int lk[4] =
	{
		256 * sizeof(D3DCOLOR),    // D3DPALETTE_256
		128 * sizeof(D3DCOLOR),    // D3DPALETTE_128
		64 * sizeof(D3DCOLOR),     // D3DPALETTE_64
		32 * sizeof(D3DCOLOR)      // D3DPALETTE_32
	};

	return lk[Size];
*/
	return (256 * sizeof(D3DCOLOR)) >> (unsigned)Size;
}

inline xbox::X_D3DPALETTESIZE GetXboxPaletteSize(const xbox::X_D3DPalette *pPalette)
{
	xbox::X_D3DPALETTESIZE PaletteSize = (xbox::X_D3DPALETTESIZE)
		((pPalette->Common & X_D3DPALETTE_COMMON_PALETTESIZE_MASK) >> X_D3DPALETTE_COMMON_PALETTESIZE_SHIFT);

	return PaletteSize;
}

int GetD3DResourceRefCount(IDirect3DResource *EmuResource)
{
	if (EmuResource != nullptr)
	{
		// Get actual reference count by increasing it using AddRef,
		// and relying on the return value of Release (which is
		// probably more reliable than AddRef)
		EmuResource->AddRef();
		return EmuResource->Release();
	}

	return 0;
}

/*
xbox::X_D3DSurface *EmuNewD3DSurface()
{
	xbox::X_D3DSurface *result = (xbox::X_D3DSurface *)xbox::ExAllocatePool(sizeof(xbox::X_D3DSurface));
	result->Common = X_D3DCOMMON_D3DCREATED | X_D3DCOMMON_TYPE_SURFACE | 1; // Set refcount to 1
	return result;
}
*/

unsigned int CxbxGetPixelContainerDepth
(
	xbox::X_D3DPixelContainer *pPixelContainer
)
{
	if (pPixelContainer->Size == 0) {
		DWORD l2d = (pPixelContainer->Format & X_D3DFORMAT_PSIZE_MASK) >> X_D3DFORMAT_PSIZE_SHIFT;
		return  1 << l2d;
	}

	return 1;
}

unsigned int CxbxGetPixelContainerMipMapLevels
(
	xbox::X_D3DPixelContainer *pPixelContainer
)
{
	if (pPixelContainer->Size == 0) {
		return (pPixelContainer->Format & X_D3DFORMAT_MIPMAP_MASK) >> X_D3DFORMAT_MIPMAP_SHIFT;
	}

	return 1;
}

uint32_t GetPixelContainerWidth(xbox::X_D3DPixelContainer *pPixelContainer)
{
	DWORD Size = pPixelContainer->Size;
	uint32_t Result;

	if (Size != 0) {
		Result = ((Size & X_D3DSIZE_WIDTH_MASK) /* >> X_D3DSIZE_WIDTH_SHIFT*/) + 1;
	}
	else {
		DWORD l2w = (pPixelContainer->Format & X_D3DFORMAT_USIZE_MASK) >> X_D3DFORMAT_USIZE_SHIFT;

		Result = 1 << l2w;
	}

	return Result;
}

uint32_t GetPixelContainerHeight(xbox::X_D3DPixelContainer *pPixelContainer)
{
	DWORD Size = pPixelContainer->Size;
	uint32_t Result;

	if (Size != 0) {
		Result = ((Size & X_D3DSIZE_HEIGHT_MASK) >> X_D3DSIZE_HEIGHT_SHIFT) + 1;
	}
	else {
		DWORD l2h = (pPixelContainer->Format & X_D3DFORMAT_VSIZE_MASK) >> X_D3DFORMAT_VSIZE_SHIFT;

		Result = 1 << l2h;
	}

	return Result;
}

void GetSurfaceFaceAndLevelWithinTexture(xbox::X_D3DSurface* pSurface, xbox::X_D3DBaseTexture* pTexture, UINT& Level, _9_11(D3DCUBEMAP_FACES, int)& Face)
{
    auto pSurfaceData = (uintptr_t)GetDataFromXboxResource(pSurface);
    auto pTextureData = (uintptr_t)GetDataFromXboxResource(pTexture);

    // Fast path: If the data pointers match, this must be the first surface within the texture
    if (pSurfaceData == pTextureData) {
        Level = 0;
        Face = _9_11(D3DCUBEMAP_FACE_POSITIVE_X, 0);
        return;
    }

    int numLevels = CxbxGetPixelContainerMipMapLevels(pTexture);
    int numFaces = pTexture->Format & X_D3DFORMAT_CUBEMAP ? 6 : 1;

    CxbxGetPixelContainerMipMapLevels(pTexture);

    // First, we need to fetch the dimensions of both the surface and the texture, for use within our calculations
    UINT textureWidth, textureHeight, textureDepth, textureRowPitch, textureSlicePitch;
    CxbxGetPixelContainerMeasures(pTexture, 0, &textureWidth, &textureHeight, &textureDepth, &textureRowPitch, &textureSlicePitch);

    UINT surfaceWidth, surfaceHeight, surfaceDepth, surfaceRowPitch, surfaceSlicePitch;
    CxbxGetPixelContainerMeasures(pSurface, 0, &surfaceWidth, &surfaceHeight, &surfaceDepth, &surfaceRowPitch, &surfaceSlicePitch);

    // Iterate through all faces and levels, until we find a matching pointer
    bool isCompressed = EmuXBFormatIsCompressed(GetXboxPixelContainerFormat(pTexture));
    int minSize = (isCompressed) ? 4 : 1;
    int cubeFaceOffset = 0; int cubeFaceSize = 0;
    auto pData = pTextureData;

    for (int face = _9_11(D3DCUBEMAP_FACE_POSITIVE_X, 0); face < numFaces; face++) {
        int mipWidth = textureWidth;
        int mipHeight = textureHeight;
        int mipDepth = textureDepth;
        int mipRowPitch = textureRowPitch;
        int mipDataOffset = 0;

        for (int level = 0; level < numLevels; level++) {
            if (pData == pSurfaceData) {
                Level = level;
                Face = (_9_11(D3DCUBEMAP_FACES, int))face;
                return;
            }

            // Calculate size of this mipmap level
            UINT dwMipSize = mipRowPitch * mipHeight;
            if (isCompressed) {
                dwMipSize /= 4;
            }

            // If this is the first face, set the cube face size
            if (face == _9_11(D3DCUBEMAP_FACE_POSITIVE_X, 0)) {
                cubeFaceSize = ROUND_UP(textureDepth * dwMipSize, X_D3DTEXTURE_CUBEFACE_ALIGNMENT);
            }

            // Move to the next mip-map and calculate dimensions for the next iteration
            mipDataOffset += dwMipSize;

            if (mipWidth > minSize) {
                mipWidth /= 2;
                mipRowPitch /= 2;
            }

            if (mipHeight > minSize) {
                mipHeight /= 2;
            }

            if (mipDepth > 1) {
                mipDepth /= 2;
            }
        }

        // Move to the next face
        pData += cubeFaceSize;
    }

    LOG_TEST_CASE("Could not find Surface within Texture, falling back to Level = 0, Face = D3DCUBEMAP_FACE_POSITIVE_X");
    Level = 0;
    Face = _9_11(D3DCUBEMAP_FACE_POSITIVE_X, 0);
}

// Wrapper function to allow calling without passing a face
void GetSurfaceFaceAndLevelWithinTexture(xbox::X_D3DSurface* pSurface, xbox::X_D3DBaseTexture* pBaseTexture, UINT& Level)
{
    _9_11(D3DCUBEMAP_FACES, int) face;
    GetSurfaceFaceAndLevelWithinTexture(pSurface, pBaseTexture, Level, face);
}

extern void HLE_init_pgraph_plugins(); // implemented in XbPushBuffer.cpp

// Direct3D initialization (called before emulation begins)
void CreateHostResource(xbox::X_D3DResource *pResource, DWORD D3DUsage, int iTextureStage, DWORD dwSize); // Forward declartion to prevent restructure of code
static void EmuVerifyResourceIsRegistered(xbox::X_D3DResource *pResource, DWORD D3DUsage, int iTextureStage, DWORD dwSize)
{
	// Skip resources without data
	if (pResource->Data == xbox::zero)
		return;

	auto key = GetHostResourceKey(pResource, iTextureStage);
	auto& ResourceCache = GetResourceCache(key);
	auto it = ResourceCache.find(key);
	if (it != ResourceCache.end()) {
		// TODO : Should this check be (D3DUsage & D3DUSAGE_RENDERTARGET) instead?
		if (D3DUsage == D3DUSAGE_RENDERTARGET && IsResourceAPixelContainer(pResource) && EmuXBFormatIsRenderTarget(GetXboxPixelContainerFormat((xbox::X_D3DPixelContainer*)pResource))) {
            // Render targets have special behavior: We can't trash them on guest modification
            // this fixes an issue where CubeMaps were broken because the surface Set in GetCubeMapSurface
            // would be overwritten by the surface created in SetRenderTarget
            // However, if a non-rendertarget surface is used here, we'll need to recreate it as a render target!
            auto hostResource = it->second.pHostResource;
			auto xboxSurface = (xbox::X_D3DSurface*)pResource;
			auto xboxTexture = (xbox::X_D3DTexture*)pResource;
			auto xboxResourceType = GetXboxD3DResourceType(pResource);
            

            // Only continue checking if we were able to get the surface desc, if it failed, we fall-through
            // to previous resource management behavior
            if (it->second.HostUsage != D3DUSAGE_INVALID) {
                // If this resource is already created as a render target on the host, simply return
                if (it->second.HostUsage & D3DUSAGE_RENDERTARGET) {
                    return;
                }

                // The host resource is not a render target, but the Xbox surface is
                // We need to re-create it as a render target
                switch (xboxResourceType) {
                    case xbox::X_D3DRTYPE_SURFACE: {
                        auto xboxSurface = (xbox::X_D3DSurface*)pResource;

                        // Free the host surface
                        FreeHostResource(key);

                        // Free the parent texture, if present
                        xbox::X_D3DTexture* pParentXboxTexture = (pResource) ? (xbox::X_D3DTexture*)xboxSurface->Parent : xbox::zeroptr;

                        if (pParentXboxTexture) {
                            // Re-create the texture with D3DUSAGE_RENDERTARGET, this will automatically create any child-surfaces
                            FreeHostResource(GetHostResourceKey(pParentXboxTexture, iTextureStage));
                            CreateHostResource(pParentXboxTexture, D3DUsage, iTextureStage, dwSize);
                        }

                        // Re-create the surface with D3DUSAGE_RENDERTARGET
                        CreateHostResource(pResource, D3DUsage, iTextureStage, dwSize);
                    } break;
                    case xbox::X_D3DRTYPE_TEXTURE: {
                        auto xboxTexture = (xbox::X_D3DTexture*)pResource;

                        // Free the host texture
                        FreeHostResource(key);

                        // And re-create the texture with D3DUSAGE_RENDERTARGET
                        CreateHostResource(pResource, D3DUsage, iTextureStage, dwSize);
                    } break;
                    default:
                        LOG_TEST_CASE("Unimplemented rendertarget type");
                }

                return;
            }
		}

		if (!HostResourceRequiresUpdate(key, pResource, dwSize)) {
			return;
		}

		FreeHostResource(key);
	} else {
		resource_info_t newResource;
		ResourceCache[key] = newResource;
	}

	CreateHostResource(pResource, D3DUsage, iTextureStage, dwSize);
}

// TODO : Move to own file
constexpr UINT QuadToTriangleVertexCount(UINT NrOfQuadVertices)
{
	return (NrOfQuadVertices * VERTICES_PER_TRIANGLE * TRIANGLES_PER_QUAD) / VERTICES_PER_QUAD;
}

// TODO : Move to own file (or argument of all users)
bool bUseClockWiseWindingOrder = true; // TODO : Should this be fetched from X_D3DRS_FRONTFACE (or X_D3DRS_CULLMODE)?

// TODO : Move to own file
// This function convertes quad to triangle indices.
// When pXboxQuadIndexData is set, original quad indices are read from this buffer
// (this use-case is for when an indexed quad draw is to be emulated).
// When pXboxQuadIndexData is null, quad-emulating indices are generated
// (this use-case is for when a non-indexed quad draw is to be emulated).
// The number of indices to generate is specified through uNrOfTriangleIndices.
// Resulting triangle indices are written to pTriangleIndexData, which must
// be pre-allocated to fit the output data.
// (Note, this function is marked 'constexpr' to allow the compiler to optimize
// the case when pXboxQuadIndexData is null)
constexpr void CxbxConvertQuadListToTriangleListIndices(
	INDEX16* pXboxQuadIndexData,
	unsigned uNrOfTriangleIndices,
	INDEX16* pTriangleIndexData)
{
	assert(uNrOfTriangleIndices > 0);
	assert(pTriangleIndexData);

	unsigned i = 0;
	unsigned j = 0;
	while (i + (VERTICES_PER_TRIANGLE * TRIANGLES_PER_QUAD) <= uNrOfTriangleIndices) {
		if (bUseClockWiseWindingOrder) {
			// ABCD becomes ABC+CDA, so this is triangle 1 :
			pTriangleIndexData[i + 0] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 0] : j + 0; // A
			pTriangleIndexData[i + 1] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 1] : j + 1; // B
			pTriangleIndexData[i + 2] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 2] : j + 2; // C
			i += VERTICES_PER_TRIANGLE;
			// And this is triangle 2 :
			pTriangleIndexData[i + 0] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 2] : j + 2; // C
			pTriangleIndexData[i + 1] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 3] : j + 3; // D
			pTriangleIndexData[i + 2] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 0] : j + 0; // A
			i += VERTICES_PER_TRIANGLE;
		} else {
			// ABCD becomes ADC+CBA, so this is triangle 1 :
			pTriangleIndexData[i + 0] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 0] : j + 0; // A
			pTriangleIndexData[i + 1] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 3] : j + 3; // D
			pTriangleIndexData[i + 2] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 2] : j + 2; // C
			i += VERTICES_PER_TRIANGLE;
			// And this is triangle 2 :
			pTriangleIndexData[i + 0] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 2] : j + 2; // C
			pTriangleIndexData[i + 1] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 1] : j + 1; // B
			pTriangleIndexData[i + 2] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 0] : j + 0; // A
			i += VERTICES_PER_TRIANGLE;
		}

		// Next quad, please :
		j += VERTICES_PER_QUAD;
	}
}

// TODO : Move to own file
// Called from EMUPATCH(D3DDevice_DrawIndexedVerticesUP) when PrimitiveType == X_D3DPT_QUADLIST.
// This API receives the number of vertices to draw (VertexCount), the index data that references
// vertices and a single stream of vertex data. The number of vertices to draw indicates the number
// of indices that are going to be fetched. The vertex data is referenced up to the highest index
// number present in the index data.
// To emulate drawing indexed quads, g_pD3DDevice->DrawIndexedPrimitiveUP is called on host,
// whereby the quad indices are converted to triangle indices. This implies for every four
// quad indices, we have to generate (two times three is) six triangle indices. (Note, that
// vertex data undergoes it's own Xbox-to-host conversion, independent from these indices.)
INDEX16* CxbxCreateQuadListToTriangleListIndexData(INDEX16* pXboxQuadIndexData, unsigned QuadVertexCount)
{
	UINT NrOfTriangleIndices = QuadToTriangleVertexCount(QuadVertexCount);
	INDEX16* pQuadToTriangleIndexBuffer = (INDEX16*)malloc(NrOfTriangleIndices * sizeof(INDEX16));
	CxbxConvertQuadListToTriangleListIndices(pXboxQuadIndexData, NrOfTriangleIndices, pQuadToTriangleIndexBuffer);
	return pQuadToTriangleIndexBuffer;
}

