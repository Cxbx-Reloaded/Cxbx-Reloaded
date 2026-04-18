// C++ / HLSL shared state block for fixed function support
#ifdef  __cplusplus
#pragma once

#ifdef CXBX_USE_D3D11
#include <DirectXMath.h> // for XMFLOAT2, XMFLOAT3, XMFLOAT4, XMMATRIX
using namespace DirectX;
#define float2 XMFLOAT2
#define float3 XMFLOAT3
#define float4 XMFLOAT4
#define float4x4 XMMATRIX
#else
#include <d3dx9math.h> // for D3DXVECTOR2, D3DVECTOR, D3DXVECTOR4, D3DMATRIX
#define float2 D3DXVECTOR2
#define float3 D3DVECTOR
#define float4 D3DXVECTOR4
#define float4x4 D3DMATRIX
#endif

#include <array> // for std::array<>
#define arr(name, type, length) std::array<type, length> name

// A float that occupies a full 16-byte constant register (matching Xbox register layout)
#define PADDED_FLOAT(name) alignas(16) float name
// A float3 that occupies a full 16-byte constant register (matching Xbox register layout)
#define PADDED_FLOAT3(name) alignas(16) float3 name

#else
// HLSL
#define arr(name, type, length) type name[length]
#define alignas(x)
#define const static

// Ensure each field occupies a full float4 constant register in SM5 cbuffer packing,
// matching the C++ alignas(16) layout for raw Xbox register data upload.
#define PADDED_FLOAT(name) float name; float3 _pad_##name
#define PADDED_FLOAT3(name) float3 name; float _pad_##name

#endif //  __cplusplus

#ifdef  __cplusplus
namespace FixedFunctionPixelShader {
#endif
	// From X_D3DTOP
	const float X_D3DTOP_DISABLE = 1;
	const float X_D3DTOP_SELECTARG1 = 2;
	const float X_D3DTOP_SELECTARG2 = 3;
	const float X_D3DTOP_MODULATE = 4;
	const float X_D3DTOP_MODULATE2X = 5;
	const float X_D3DTOP_MODULATE4X = 6;
	const float X_D3DTOP_ADD = 7;
	const float X_D3DTOP_ADDSIGNED = 8;
	const float X_D3DTOP_ADDSIGNED2X = 9;
	const float X_D3DTOP_SUBTRACT = 10;
	const float X_D3DTOP_ADDSMOOTH = 11;
	const float X_D3DTOP_BLENDDIFFUSEALPHA = 12;
	const float X_D3DTOP_BLENDCURRENTALPHA = 13;
	const float X_D3DTOP_BLENDTEXTUREALPHA = 14;
	const float X_D3DTOP_BLENDFACTORALPHA = 15;
	const float X_D3DTOP_BLENDTEXTUREALPHAPM = 16;
	const float X_D3DTOP_PREMODULATE = 17;
	const float X_D3DTOP_MODULATEALPHA_ADDCOLOR = 18;
	const float X_D3DTOP_MODULATECOLOR_ADDALPHA = 19;
	const float X_D3DTOP_MODULATEINVALPHA_ADDCOLOR = 20;
	const float X_D3DTOP_MODULATEINVCOLOR_ADDALPHA = 21;
	const float X_D3DTOP_DOTPRODUCT3 = 22;
	const float X_D3DTOP_MULTIPLYADD = 23;
	const float X_D3DTOP_LERP = 24;
	const float X_D3DTOP_BUMPENVMAP = 25;
	const float X_D3DTOP_BUMPENVMAPLUMINANCE = 26;

	// D3DTA taken from D3D9 - we don't have Xbox definitions
	// for these so I guess they're the same?
	const float X_D3DTA_DIFFUSE = 0x00000000;  // select diffuse color (read only)
	const float X_D3DTA_CURRENT = 0x00000001;  // select stage destination register (read/write)
	const float X_D3DTA_TEXTURE = 0x00000002;  // select texture color (read only)
	const float X_D3DTA_TFACTOR = 0x00000003;  // select D3DRS_TEXTUREFACTOR (read only)
	const float X_D3DTA_SPECULAR = 0x00000004;  // select specular color (read only)
	const float X_D3DTA_TEMP = 0x00000005;  // select temporary register color (read/write)
	const float X_D3DTA_CONSTANT = 0x00000006;  // select texture stage constant
	const float X_D3DTA_COMPLEMENT = 0x00000010;  // take 1.0 - x (read modifier)
	const float X_D3DTA_ALPHAREPLICATE = 0x00000020;  // replicate alpha to color components (read modifier)

    const int SAMPLE_NONE = 0;
    const int SAMPLE_2D = 1;
    const int SAMPLE_3D = 2;
    const int SAMPLE_CUBE = 3;

    // https://docs.microsoft.com/en-us/windows/win32/direct3d9/fog-formulas
    const float FOG_TABLE_NONE = 0;
    const float FOG_TABLE_EXP = 1;
    const float FOG_TABLE_EXP2 = 2;
    const float FOG_TABLE_LINEAR = 3;

	// This state is passed to the shader
	struct PsTextureStageState {
		// Values correspond to XD3D8 version of D3DTEXTURESTAGESTATETYPE
		// https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dtexturestagestatetype

		/* Samplers for now are configured elsewhere already
		constexpr DWORD X_D3DTSS_ADDRESSU = 0;
		constexpr DWORD X_D3DTSS_ADDRESSV = 1;
		constexpr DWORD X_D3DTSS_ADDRESSW = 2;
		constexpr DWORD X_D3DTSS_MAGFILTER = 3;
		constexpr DWORD X_D3DTSS_MINFILTER = 4;
		constexpr DWORD X_D3DTSS_MIPFILTER = 5;
		constexpr DWORD X_D3DTSS_MIPMAPLODBIAS = 6;
		constexpr DWORD X_D3DTSS_MAXMIPLEVEL = 7;
		constexpr DWORD X_D3DTSS_MAXANISOTROPY = 8;
		*/

		PADDED_FLOAT(COLORKEYOP); // = 9; Xbox extension!
		alignas(16) float4 COLORSIGN; // = 10; Xbox extension!
		PADDED_FLOAT(ALPHAKILL); // = 11; Xbox extension!

		// 12 .. 20 are moved into PsTextureHardcodedState, which are compiled into the shader

		// TEXTURETRANSFORMFLAGS // = 21; handled by the VS
		PADDED_FLOAT(BUMPENVMAT00); // = 22;
		PADDED_FLOAT(BUMPENVMAT01); // = 23;
		PADDED_FLOAT(BUMPENVMAT11); // = 24;
		PADDED_FLOAT(BUMPENVMAT10); // = 25;
		PADDED_FLOAT(BUMPENVLSCALE); // = 26;
		PADDED_FLOAT(BUMPENVLOFFSET); // = 27;
		// TEXCOORDINDEX // = 28; handled by the VS
		// BORDERCOLOR // = 29; set on sampler
		alignas(16) float4 COLORKEYCOLOR; // = 30; Xbox extension!
		// UNSUPPORTED // = 31; // Note : Somehow, this one comes through D3DDevice_SetTextureStageStateNotInline sometimes
		PADDED_FLOAT(TEXFMTFIXUP); // D3D11: texture format channel fixup (0=identity, 1=.gbar, 2=.abgr, 3=luminance .rrra, 4=alpha-luminance .rrrg)
	};

	// Texture format fixup constants (shared between C++ and HLSL)
	const float TEXFMTFIXUP_IDENTITY = 0;
	const float TEXFMTFIXUP_GBAR     = 1; // B8G8R8A8 uploaded as R8G8B8A8
	const float TEXFMTFIXUP_ABGR     = 2; // R8G8B8A8 uploaded as R8G8B8A8
	const float TEXFMTFIXUP_LUM      = 3; // Luminance: R8→(R,R,R,1)
	const float TEXFMTFIXUP_ALUM     = 4; // Alpha-luminance: R8G8→(R,R,R,G);

	// This state is compiled into the shader
	// Values correspond to XD3D8 version of D3DTEXTURESTAGESTATETYPE
	// https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dtexturestagestatetype
	struct PsTextureHardcodedState {
		PADDED_FLOAT(COLOROP); // = 12;
		PADDED_FLOAT(COLORARG0); // = 13;
		PADDED_FLOAT(COLORARG1); // = 14;
		PADDED_FLOAT(COLORARG2); // = 15;
		PADDED_FLOAT(ALPHAOP); // = 16;
		PADDED_FLOAT(ALPHAARG0); // = 17;
		PADDED_FLOAT(ALPHAARG1); // = 18;
		PADDED_FLOAT(ALPHAARG2); // = 19;
		PADDED_FLOAT(RESULTARG); // = 20;
	};

	struct FixedFunctionPixelShaderState {
		alignas(16) arr(stages, PsTextureStageState, 4);
		alignas(16) float4 TextureFactor;
		PADDED_FLOAT(SpecularEnable);
		PADDED_FLOAT(FogEnable);
		PADDED_FLOAT3(FogColor);
		PADDED_FLOAT(FogTableMode);
		PADDED_FLOAT(FogDensity);
		PADDED_FLOAT(FogStart);
		PADDED_FLOAT(FogEnd);
		alignas(16) float4 AlphaTest; // x=enable, y=ref [0..1], z=func [D3DCMPFUNC], w=unused
    };
#ifdef  __cplusplus
} // FixedFunctionPixelShader namespace
#endif

#ifdef  __cplusplus
#undef float2
#undef float3
#undef float4
#undef float4x4
#else // HLSL
#undef alignas
#undef const
#endif //  __cplusplus

#undef arr
#undef PADDED_FLOAT
#undef PADDED_FLOAT3
