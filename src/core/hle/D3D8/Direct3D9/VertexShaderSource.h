
#ifndef DIRECT3D9SHADERCACHE_H
#define DIRECT3D9SHADERCACHE_H

#include "VertexShader.h"
#include <map>
#include <future>
#include <istream>
#include <ostream>

typedef uint64_t ShaderKey;

// Manages creation and caching of vertex shaders
class VertexShaderSource {

public:
	ShaderKey CreateShader(const xbox::dword_xt* pXboxFunction, DWORD* pXboxFunctionSize);
	IDirect3DVertexShader* GetShader(IDirect3DDevice9& pD3DDevice, ShaderKey key);
	void ReleaseShader(ShaderKey key);

	void Serialize(std::ostream& output);
	void DeserializeAndLoad(IDirect3DDevice9* pD3DDevice, std::istream& in);

private:
	struct LazyVertexShader {
		bool isReady = false;
		std::future<ID3DBlob*> compileResult;
		IDirect3DVertexShader* pHostVertexShader = nullptr;

		// TODO when is it a good idea to release shaders?
		int referenceCount = 0;
	};

	std::unordered_map<ShaderKey, LazyVertexShader> cache;

	bool _FindShader(ShaderKey key, LazyVertexShader** ppLazyShader);
};

// FIXME move this to a D3D9 global state file?
extern VertexShaderSource g_VertexShaderSource;

#endif
