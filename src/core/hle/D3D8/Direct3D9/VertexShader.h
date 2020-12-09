#pragma once

#include "core\hle\D3D8\XbVertexShader.h"
#include "FixedFunctionVertexShaderState.hlsli"

enum class ShaderType {
	Empty = 0,
	Compilable,
	Unsupported,
};

static const char* vs_model_2_a = "vs_2_a";
static const char* vs_model_3_0 = "vs_3_0";
extern const char* g_vs_model;

extern HRESULT EmuCompileVertexShader
(
    IntermediateVertexShader* pIntermediateShader,
    ID3DBlob** ppHostShader
);

extern void EmuCompileFixedFunction(ID3DBlob** ppHostShader);

extern HRESULT EmuCompileXboxPassthrough(ID3DBlob** ppHostShader);

