#pragma once

#include <atomic>
#include <string> // std::string
#include <d3dcompiler.h> // ID3DBlob (via d3d9.h > d3d11shader.h > d3dcommon.h)

extern HRESULT EmuCompileShader
(
	std::string hlsl_str,
	const char* shader_profile,
	ID3DBlob** ppHostShader,
	const char* pSourceName = nullptr,
	bool asyncAllowed = false
);

// Flush the background shader save queue and stop the save thread.
// Must be called before process termination to ensure all compiled shaders
// are written to disk and persisted across sessions.
void ShaderCacheShutdown();

struct ShaderSources {
	// Pixel Shader
	std::string pixelShaderTemplateHlsl[3];

	std::string fixedFunctionPixelShaderHlsl;
	std::string fixedFunctionPixelShaderPath;

	// Vertex Shader
	std::string vertexShaderTemplateHlsl[2];

	std::string fixedFunctionVertexShaderHlsl;
	std::string fixedFunctionVertexShaderPath;

	std::string vertexShaderPassthroughHlsl;
	std::string vertexShaderPassthroughPath;

	// Load shaders from disk (if out-of-date)
	// and return the current loaded shader version
	int Update();

	// Start a thread to watch for changes in the shader folder
	void InitShaderHotloading();

private:
	void LoadShadersFromDisk();

	// counts upwards on every change detected to the shader source files at runtime
	std::atomic_int shaderVersionOnDisk = 0;
	// current loaded shader version
	// Initialized to < shaderVersionOnDisk
	int shaderVersionLoadedFromDisk = -1;
};

extern ShaderSources g_ShaderSources;

// Call this once after the D3D9 device is created. The adapter description and
// driver version are used to validate the on-disk shader bytecode cache: if the
// GPU or driver has changed since the cache was written, all stale .cso files
// are silently removed so they are recompiled rather than causing a crash.
void ShaderCacheSetAdapterFingerprint(const char* adapterDesc, uint32_t driverVersionHigh, uint32_t driverVersionLow);
