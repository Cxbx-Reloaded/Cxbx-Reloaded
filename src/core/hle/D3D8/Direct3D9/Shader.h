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
