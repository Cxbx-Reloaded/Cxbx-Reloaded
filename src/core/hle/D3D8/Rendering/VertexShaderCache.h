
#ifndef DIRECT3D9SHADERCACHE_H
#define DIRECT3D9SHADERCACHE_H

#include "VertexShader.h"
#include <map>

typedef uint64_t ShaderKey;

// Manages creation and caching of vertex shaders
class VertexShaderCache {

public:
	ShaderKey CreateShader(const xbox::dword_xt* pXboxFunction, DWORD* pXboxFunctionSize);
	IDirect3DVertexShader *GetShader(ShaderKey key);
#ifdef CXBX_USE_D3D11
	ID3DBlob* GetShaderBytecode(ShaderKey key);
#endif
	void ReleaseShader(ShaderKey key);

	void Clear();
	// TODO
	// WriteCacheToDisk
	// LoadCacheFromDisk

private:
	struct LazyVertexShader {
		bool isReady = false;
		std::future<ID3DBlob*> compileResult;
		IDirect3DVertexShader* pHostVertexShader = nullptr;
#ifdef CXBX_USE_D3D11
		ID3DBlob* pBytecode = nullptr; // Kept alive for input layout creation
#endif

		// TODO when is it a good idea to releas eshaders?
		int referenceCount = 0;

		// TODO persist shaders to disk
		// ShaderVersion?
		// OptimizationLevel?
	};

	std::mutex cacheMutex;
	std::map<ShaderKey, LazyVertexShader> cache;

	bool _FindShader(ShaderKey key, LazyVertexShader** ppLazyShader);
};

extern VertexShaderCache g_VertexShaderCache;

#endif
