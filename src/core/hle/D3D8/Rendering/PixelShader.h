#pragma once

#include "Shader.h" // ID3DBlob (via d3dcompiler.h > d3d11shader.h > d3dcommon.h)
#include "core\hle\D3D8\XbPixelShader.h" // DecodedRegisterCombiner

static const char* ps_model_2_a = "ps_2_a";
static const char* ps_model_3_0 = "ps_3_0";
extern const char* g_ps_model;

extern HRESULT EmuCompilePixelShader
(
    DecodedRegisterCombiner* pIntermediateShader,
    ID3DBlob** ppHostShader
);
