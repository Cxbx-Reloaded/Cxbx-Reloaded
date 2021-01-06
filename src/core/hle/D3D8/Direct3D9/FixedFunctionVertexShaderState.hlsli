// C++ / HLSL shared state block for fixed function support
#ifdef  __cplusplus
#pragma once

#include <d3d9.h>
#include <d3d9types.h> // for D3DFORMAT, D3DLIGHT9, etc
#include <d3dx9math.h> // for D3DXVECTOR4, etc
#include <array>

#define float4x4 D3DMATRIX
#define float4 D3DXVECTOR4
#define float3 D3DVECTOR
#define float2 D3DXVECTOR2
#define arr(name, type, length) std::array<type, length> name

#else
// HLSL
#define arr(name, type, length) type name[length]
#define alignas(x)
#define const static
#endif //  __cplusplus

namespace FixedFunctionVertexShader {
	// Fog depth is taken from the vertex, rather than generated
	const float FOG_DEPTH_NONE = 0;
	// Fog depth is the output Z coordinate
    const float FOG_DEPTH_Z = 1;
	// Fog depth is based on the output W coordinate (1 / W)
    const float FOG_DEPTH_W = 2;
	// Fog depth is based distance of the vertex from the eye position
    const float FOG_DEPTH_RANGE = 3;
}

// Shared HLSL structures
// Taking care with packing rules
// In VS_3_0, packing works in mysterious ways
// * Structs inside arrays are not packed
// * Floats can't be packed at all (?)
// We don't get documented packing until vs_4_0

struct Transforms {
    float4x4 View; // 0
    float4x4 Projection; // 1
    arr(Texture, float4x4, 4); // 2, 3, 4, 5
	// World matrices are 6, 7, 8, 9
	// But we use combined WorldView matrices in the shader
    arr(WorldView, float4x4, 4); 
	arr(WorldViewInverseTranspose, float4x4, 4);
};

// See D3DLIGHT
struct Light {
    // TODO in vs_4_0+ when floats are packable
    // Change colour values to float3
    // And put something useful in the alpha slot
    float4 Diffuse;
    float4 Specular;

    // Viewspace light position
    alignas(16) float3 PositionV;
    alignas(16) float Range;

    // Viewspace light direction (normalized)
    alignas(16) float3 DirectionVN;
    alignas(16) float Type; // 1=Point, 2=Spot, 3=Directional

    alignas(16) float3 Attenuation;
    alignas(16) float Falloff;

    alignas(16) float CosHalfPhi;
    // cos(theta/2) - cos(phi/2)
    alignas(16) float SpotIntensityDivisor;
};

struct Material {
    float4 Diffuse;
    float4 Ambient;
    float4 Specular;
    float4 Emissive;

    alignas(16) float Power;
};

struct Modes {
    alignas(16) float AmbientMaterialSource;
    alignas(16) float DiffuseMaterialSource;
    alignas(16) float SpecularMaterialSource;
    alignas(16) float EmissiveMaterialSource;

    alignas(16) float BackAmbientMaterialSource;
    alignas(16) float BackDiffuseMaterialSource;
    alignas(16) float BackSpecularMaterialSource;
    alignas(16) float BackEmissiveMaterialSource;

    alignas(16) float Lighting;
    alignas(16) float TwoSidedLighting;
    alignas(16) float SpecularEnable;
    alignas(16) float LocalViewer;

    alignas(16) float VertexBlend_NrOfMatrices;
    alignas(16) float VertexBlend_CalcLastWeight; // Could be a bool in higer shader models
    alignas(16) float NormalizeNormals;
};

struct PointSprite {
    alignas(16) float PointSize;
    alignas(16) float PointScaleEnable;
    alignas(16) float RenderTargetHeight;
    alignas(16) float ScaleA;
    alignas(16) float ScaleB;
    alignas(16) float ScaleC;
};

struct TextureState {
    alignas(16) float TextureTransformFlagsCount;
    alignas(16) float TextureTransformFlagsProjected;
    alignas(16) float TexCoordIndex;
    alignas(16) float TexCoordIndexGen;
};

struct Fog {
    alignas(16) float DepthMode;
};

struct FixedFunctionVertexShaderState {
    alignas(16) Transforms Transforms;
    alignas(16) arr(Lights, Light, 8);
    alignas(16) float4 AmbientPlusLightAmbient;
    alignas(16) float4 BackAmbientPlusLightAmbient;
    alignas(16) arr(Materials, Material, 2);
    alignas(16) Modes Modes;
    alignas(16) Fog Fog;
    alignas(16) arr(TextureStates, TextureState, 4);
    alignas(16) PointSprite PointSprite;
    alignas(16) float4 TexCoordComponentCount;
};

#ifdef  __cplusplus
#undef float4x4
#undef float4
#undef float3
#undef float2
#undef arr
#else // HLSL
#undef arr
#undef alignas
#undef const
#endif //  __cplusplus
