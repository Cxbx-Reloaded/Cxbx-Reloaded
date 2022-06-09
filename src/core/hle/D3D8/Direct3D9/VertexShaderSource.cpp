#define LOG_PREFIX CXBXR_MODULE::VSHCACHE

#include "VertexShaderSource.h"

#include "core/kernel/init/CxbxKrnl.h"
#include "util/hasher.h"
#include "core/kernel/support/Emu.h"
#include "core\hle\D3D8\Direct3D9\Direct3D9.h" // For g_pD3DDevice

VertexShaderSource g_VertexShaderSource = VertexShaderSource();
// FIXME : This should really be released and created in step with the D3D device lifecycle rather than being a thing on its own

ID3DBlob* AsyncCreateVertexShader(IntermediateVertexShader intermediateShader, ShaderKey key) {
	ID3DBlob* pCompiledShader;

	auto hRet = EmuCompileVertexShader(
		&intermediateShader,
		&pCompiledShader
	);

	EmuLog(LOG_LEVEL::DEBUG, "Finished compiling shader %llx", key);

	return pCompiledShader;
}

// Find a shader
// Return true if the shader was found
 bool VertexShaderSource::_FindShader(ShaderKey key, LazyVertexShader** ppLazyShader) {
	auto it = cache.find(key);
	if (it == cache.end()) {
		// We didn't find anything! Was CreateShader called?
		EmuLog(LOG_LEVEL::WARNING, "No vertex shader found for key %llx", key);
		return false;
	}

	*ppLazyShader = &it->second;
	return true;
}

 // Create a new shader
 // If the shader was already created, just increase its reference count
ShaderKey VertexShaderSource::CreateShader(const xbox::dword_xt* pXboxFunction, DWORD *pXboxFunctionSize) {
	IntermediateVertexShader intermediateShader;

	*pXboxFunctionSize = GetVshFunctionSize(pXboxFunction);

	ShaderKey key = ComputeHash((void*)pXboxFunction, *pXboxFunctionSize);

	// Check if we need to create the shader
	auto it = cache.find(key);
	if (it != cache.end()) {
		EmuLog(LOG_LEVEL::DEBUG, "Vertex shader %llx has been created already", key);
		// Increment reference count
		it->second.referenceCount++;
		EmuLog(LOG_LEVEL::DEBUG, "Incremented ref count for shader %llx (%d)", key, it->second.referenceCount);
		return key;
	}

	// Parse into intermediate format
	EmuParseVshFunction((DWORD*)pXboxFunction, &intermediateShader);

	// We're going to create a new shader
	auto newShader = LazyVertexShader();
	newShader.referenceCount = 1;

	if (!intermediateShader.Instructions.empty())
	{
		// Start compiling the shader in the background
		// TODO proper threading / threadpool.
		// We should have some control over the number and priority of threads
		EmuLog(LOG_LEVEL::DEBUG, "Creating vertex shader %llx size %d", key, *pXboxFunctionSize);
		newShader.compileResult = std::async(std::launch::async, AsyncCreateVertexShader, intermediateShader, key);
	}
	else {
		// We can't do anything with this shader
		// Test case: ???
		LOG_TEST_CASE("Empty vertex shader");
		newShader.isReady = true;
		newShader.pHostVertexShader = nullptr;
	}

	// Put the shader into the cache
	cache[key] = std::move(newShader);

	return key;
}

IDirect3DVertexShader* CxbxCreateVertexShader(ID3DBlob* pCompiledShader, const char *shader_category)
{
	IDirect3DVertexShader* pHostVertexShader = nullptr;

	// If there's no D3DDevice set, return nullptr
	if (g_pD3DDevice == nullptr) {
		EmuLog(LOG_LEVEL::WARNING, "Can't create %s vertex shader - no D3D device is set!", shader_category);
	}
	else {
		assert(pCompiledShader);

		HRESULT hRet;
#ifdef CXBX_USE_D3D11
		hRet = g_pD3DDevice->CreateVertexShader(
			(const void*)pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(), // BytecodeLength
			nullptr, // pClassLinkage
			&pHostVertexShader
		);
#else
		hRet = g_pD3DDevice->CreateVertexShader(
			(DWORD *)pCompiledShader->GetBufferPointer(),
			&pHostVertexShader
		);
#endif
		if (FAILED(hRet)) CxbxrAbort("Failed to create %s vertex shader", shader_category);

		// TODO DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateVertexShader");
		if (SUCCEEDED(hRet)) {
			EmuLog(LOG_LEVEL::DEBUG, "Created new %s vertex shader instance", shader_category);
		}
		else {
			EmuLog(LOG_LEVEL::ERROR2, "Failed creating new %s vertex shader instance", shader_category);
		}
	}

	return pHostVertexShader;
}

// Get a shader using the given key
IDirect3DVertexShader* VertexShaderSource::GetShader(ShaderKey key)
{
	LazyVertexShader* pLazyShader = nullptr;

	// Look for the shader in the cache
	if (!_FindShader(key, &pLazyShader)) {
		return nullptr; // we didn't find anything
	}

	// If the shader is ready, return it
	if (pLazyShader->isReady) {
		return pLazyShader->pHostVertexShader;
	}

	// We need to get the compiled HLSL and create a shader from it
	ID3DBlob* pCompiledShader = nullptr;
	try {
		// TODO one day, check is_ready before logging this (non-standard..?)
		EmuLog(LOG_LEVEL::DEBUG, "Waiting for shader %llx...", key);
		pCompiledShader = pLazyShader->compileResult.get();

		// Create the shader
		// TODO : incorporate key into "regular" shader category string
		pLazyShader->pHostVertexShader = CxbxCreateVertexShader(pCompiledShader, "regular");

	}
	catch (const std::exception & e) {
		EmuLog(LOG_LEVEL::ERROR2, "Failed compiling shader %llx: %s", e.what());
	}

	if (pCompiledShader) {
		pCompiledShader->Release();

		// TODO compile the shader at a higher optimization level in a background thread?
	}

	// The shader is ready
	pLazyShader->isReady = true;

	return pLazyShader->pHostVertexShader;
}

// Release a shader. Doesn't actually release any resources for now
void VertexShaderSource::ReleaseShader(ShaderKey key)
{
	// For now, don't bother releasing any shaders
	LazyVertexShader* pLazyShader;
	if (_FindShader(key, &pLazyShader)) {

		if (pLazyShader->referenceCount > 0) {
			pLazyShader->referenceCount--;
			EmuLog(LOG_LEVEL::DEBUG, "Decremented ref count for shader %llx (%d)", key, pLazyShader->referenceCount);
		}
		else
		{
			EmuLog(LOG_LEVEL::DEBUG, "Release called on unreferenced shader %llx", key);
		}
	}
	else {
		EmuLog(LOG_LEVEL::WARNING, "Release called on non-existent shader!");
	}
}
