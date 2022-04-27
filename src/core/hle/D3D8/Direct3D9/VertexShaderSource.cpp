#define LOG_PREFIX CXBXR_MODULE::VSHCACHE

#include "VertexShaderSource.h"

#include "core/kernel/init/CxbxKrnl.h"
#include "util/hasher.h"
#include <CxbxVersion.h>

VertexShaderSource g_VertexShaderSource = VertexShaderSource();

// FIXME tidy up responsibility between this and VertexShader.cpp

// TODO The call pattern has changed.
// CreateShader is no longer be called potentially long before we need the shader.
// So there's no benefit in compiling shaders async?
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
		return false;
	}

	if (ppLazyShader) {
		*ppLazyShader = &it->second;
	}
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

void VertexShaderSource::RegisterShader(ShaderKey key, IDirect3DVertexShader9* pHostVertexShader) {
	LazyVertexShader s;
	s.isReady = true;
	s.pHostVertexShader = pHostVertexShader;

	LazyVertexShader* existing;
	if (_FindShader(key, &existing)) {
		EmuLog(LOG_LEVEL::WARNING, "Overwriting vertex shader %x", key);
		if (pHostVertexShader) {
			existing->pHostVertexShader->Release();
		}
	}
	cache[key] = std::move(s);
}

// Get a shader using the given key
IDirect3DVertexShader* VertexShaderSource::GetShader(IDirect3DDevice9& pD3DDevice, ShaderKey key)
{
	LazyVertexShader* pLazyShader = nullptr;

	// Look for the shader in the cache
	if (!_FindShader(key, &pLazyShader)) {
		// We didn't find anything! Was CreateShader called?
		EmuLog(LOG_LEVEL::DEBUG, "No vertex shader found for key %llx", key);
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
		auto hRet = pD3DDevice.CreateVertexShader
		(
			(DWORD*)pCompiledShader->GetBufferPointer(),
			&pLazyShader->pHostVertexShader
		);

		// TODO DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateVertexShader");
		if (SUCCEEDED(hRet)) {
			EmuLog(LOG_LEVEL::DEBUG, "Created new vertex shader instance for %llx", key);
		}
		else {
			EmuLog(LOG_LEVEL::ERROR2, "Failed creating new vertex shader instance for %llx", key);
		}
	}
	catch (const std::exception & e) {
		EmuLog(LOG_LEVEL::ERROR2, "Failed compiling shader %llx: %s", e.what());
	}

	if (pCompiledShader) {
		pCompiledShader->Release();
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


// Serialized shader cache
// A header followed by several entries
// Each entry field is followed by \n

struct Header {
	std::string fileType;
	std::string version;
};

struct EntryHeader {
	int64_t key;
	int32_t size;
	// D3D shader blob ('size' bytes)
};

Header CreateHeader() {
	Header h;
	h.fileType = "vscache-cxbx";
	h.version = GetGitVersionStr();
	return h;
}

void VertexShaderSource::Serialize(std::ostream& out) {
	Header h = CreateHeader();

	EmuLog(LOG_LEVEL::INFO, "Serializing VSH cache...");
	try {
		out << h.fileType << "\n";
		out << h.version << "\n";

		std::vector<char> shaderBuffer;
		for (auto& kv : cache) {
			if (!kv.second.isReady || kv.second.pHostVertexShader == nullptr) {
				continue; // skip incomplete or broken shaders
			}

			auto& s = kv.second.pHostVertexShader;

			EntryHeader c;
			// key
			c.key = kv.first;
			// size
			s->GetFunction(nullptr, (UINT*)&c.size);
			// shader
			shaderBuffer.clear();
			shaderBuffer.resize(c.size);
			s->GetFunction(shaderBuffer.data(), (UINT*)&c.size);

			EmuLog(LOG_LEVEL::INFO, "Serializing shader %x...", c.key);

			out << c.key << "\n";
			out << c.size << "\n";
			out.write(shaderBuffer.data(), c.size);
			out << "\n";
		}
	}
	catch (...) {
		EmuLog(LOG_LEVEL::ERROR2, "An error occurred serializing the VSH cache");
	}
}

void VertexShaderSource::DeserializeAndLoad(IDirect3DDevice9* pD3DDevice, std::istream& in) {
	EmuLog(LOG_LEVEL::INFO, "Loading VSH cache...");

	const Header hExpected = CreateHeader();
	Header hActual;
	std::string line;
	std::vector<char> shaderBuffer;

	try {
		std::getline(in, hActual.fileType);
		std::getline(in, hActual.version);

		if (hActual.fileType != hExpected.fileType) {
			EmuLog(LOG_LEVEL::ERROR2, "VSH cache did not have the expected header.");
			return;
		}
		if (hActual.version != hExpected.version) {
			EmuLog(LOG_LEVEL::INFO, "VSH cache version mismatch. The cache will not be loaded.");
			return;
		}

		while (in.peek() != EOF) {
			EntryHeader c;

			std::getline(in, line);
			c.key = std::stoll(line);

			std::getline(in, line);
			c.size = std::stoi(line);

			if (_FindShader(c.key, nullptr)) {
				EmuLog(LOG_LEVEL::INFO, "Skipping deserializing shader %x as it was already loaded.", c.key);
				in.ignore(c.size);
				in.ignore(1);
				continue;
			}

			EmuLog(LOG_LEVEL::INFO, "Loading shader %x from VSH cache...", c.key);

			// Read the shader
			ID3DBlob* pCompiledShader;
			D3DCreateBlob(c.size, &pCompiledShader);
			in.read((char*)pCompiledShader->GetBufferPointer(), c.size);
			in.ignore(1);

			// Save cache entry
			IDirect3DVertexShader9* pHostVertexShader;
			if (SUCCEEDED(pD3DDevice->CreateVertexShader((DWORD*)pCompiledShader->GetBufferPointer(), &pHostVertexShader))) {
				RegisterShader(c.key, pHostVertexShader);
			}
			else {
				EmuLog(LOG_LEVEL::ERROR2, "Failed to load shader %x!", c.key);
			}
		}

		EmuLog(LOG_LEVEL::INFO, "VSH cache loaded successfully.");
	}
	catch (...) {
		EmuLog(LOG_LEVEL::ERROR2, "An error occurred deserializing the VSH cache!");
	}
}
