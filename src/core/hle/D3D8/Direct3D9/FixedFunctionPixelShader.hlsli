// C++ / HLSL shared state block for fixed function support
#ifdef  __cplusplus
#pragma once

#ifdef CXBX_USE_D3D11
#include <DirectXMath.h> // for XMFLOAT2, XMFLOAT3, XMFLOAT4, XMMATRIX
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

#else
// HLSL
#define arr(name, type, length) type name[length]
#define alignas(x)
#define const static
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

		alignas(16) float COLORKEYOP; // Unimplemented Xbox extension!
		alignas(16) float COLORSIGN; // Unimplemented Xbox extension!
#ifdef ENABLE_FF_ALPHAKILL
		alignas(16) float ALPHAKILL; // Xbox extension!
#else
		alignas(16) float ALPHAKILL; // Unimplemented Xbox extension!
#endif
		// TEXTURETRANSFORMFLAGS handled by the VS
		alignas(16) float BUMPENVMAT00;
		alignas(16) float BUMPENVMAT01;
		alignas(16) float BUMPENVMAT11;
		alignas(16) float BUMPENVMAT10;
		alignas(16) float BUMPENVLSCALE;
		alignas(16) float BUMPENVLOFFSET;
		// TEXCOORDINDEX handled by the VS
		// BORDERCOLOR set on sampler
		alignas(16) float COLORKEYCOLOR; // Unimplemented Xbox extension!
	};

	// This state is compiled into the shader
	// Values correspond to XD3D8 version of D3DTEXTURESTAGESTATETYPE
	// https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dtexturestagestatetype
	struct PsTextureHardcodedState {
		alignas(16) float COLOROP;
		alignas(16) float COLORARG0;
		alignas(16) float COLORARG1;
		alignas(16) float COLORARG2;
		alignas(16) float ALPHAOP;
		alignas(16) float ALPHAARG0;
		alignas(16) float ALPHAARG1;
		alignas(16) float ALPHAARG2;
		alignas(16) float RESULTARG;
	};

	struct FixedFunctionPixelShaderState {
		alignas(16) arr(stages, PsTextureStageState, 4);
		alignas(16) float4 TextureFactor;
		alignas(16) float SpecularEnable;
		alignas(16) float FogEnable;
		alignas(16) float3 FogColor;
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
