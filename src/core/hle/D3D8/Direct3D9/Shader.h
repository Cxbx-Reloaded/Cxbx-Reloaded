#pragma once

#include <string> // std::string
#include <d3dcompiler.h> // ID3DBlob (via d3d9.h > d3d11shader.h > d3dcommon.h)

extern HRESULT EmuCompileShader
(
	std::string hlsl_str,
	const char* shader_profile,
	ID3DBlob** ppHostShader,
	const char* pSourceName = nullptr
);

struct ShaderHlsl {
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
	int UpdateShaders();

	// Start a thread to watch for changes in the shader folder
	void InitShaderHotloading();

private:
	void LoadShadersFromDisk();

	// counts upwards on every change detected to the shader source files at runtime
	volatile int shaderVersionOnDisk = 0;
	// current loaded shader version
	// Initialized to < shaderVersionOnDisk
	int shaderVersionLoadedFromDisk = -1;
};

extern ShaderHlsl g_ShaderHlsl;
