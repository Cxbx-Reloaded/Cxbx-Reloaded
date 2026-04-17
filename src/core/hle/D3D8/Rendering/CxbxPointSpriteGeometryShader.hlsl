// CxbxPointSpriteGeometryShader.hlsl
// Geometry shader that expands point primitives into screen-aligned quads
// for D3D11 point sprite emulation. D3D11 lacks D3D9's built-in point
// sprite rasterization, so we must expand points to quads here.
//
// The VS outputs clip-space position and point size (PSIZE).
// This GS reads those, computes a screen-space quad, and generates
// UV coordinates [0,1]^2 in TEXCOORD0 for the point sprite texture.
// All other VS outputs (colors, fog, tex coords 1-3) are passed through.

// Must match VS_OUTPUT / PS_INPUT
struct GS_INPUT
{
    float4 oPos : POSITION;
    float4 oD0  : COLOR0;
    float4 oD1  : COLOR1;
    float  oFog : FOG;
    float  oPts : PSIZE;
    float4 oB0  : TEXCOORD4;
    float4 oB1  : TEXCOORD5;
    float4 oT0  : TEXCOORD0;
    float4 oT1  : TEXCOORD1;
    float4 oT2  : TEXCOORD2;
    float4 oT3  : TEXCOORD3;
};

struct GS_OUTPUT
{
    float4 oPos : SV_Position;
    float4 oD0  : COLOR0;
    float4 oD1  : COLOR1;
    float  oFog : FOG;
    float  oPts : PSIZE;
    float4 oB0  : TEXCOORD4;
    float4 oB1  : TEXCOORD5;
    float4 oT0  : TEXCOORD0;
    float4 oT1  : TEXCOORD1;
    float4 oT2  : TEXCOORD2;
    float4 oT3  : TEXCOORD3;
};

// Viewport dimensions (set from C++ via GS constant buffer slot 0)
// x = 1.0 / ViewportWidth, y = 1.0 / ViewportHeight
cbuffer GSConstants : register(b0)
{
    float4 gsViewportInv; // (1/width, 1/height, 0, 0)
};

[maxvertexcount(4)]
void main(point GS_INPUT input[1], inout TriangleStream<GS_OUTPUT> stream)
{
    GS_OUTPUT output;

    // Pass through all attributes from the point vertex
    output.oD0  = input[0].oD0;
    output.oD1  = input[0].oD1;
    output.oFog = input[0].oFog;
    output.oPts = input[0].oPts;
    output.oB0  = input[0].oB0;
    output.oB1  = input[0].oB1;
    // Original texture coordinates passed through to stages 1-3
    output.oT1  = input[0].oT1;
    output.oT2  = input[0].oT2;
    output.oT3  = input[0].oT3;

    // Point size in pixels (from VS PSIZE output)
    float pointSize = max(input[0].oPts, 1.0);

    // Convert point size from pixels to clip-space half-extents
    // Position is in clip space (before perspective divide), so we work in NDC:
    // halfSize in NDC = pointSize * 0.5 * (2 / viewportDim) = pointSize / viewportDim
    float4 pos = input[0].oPos;
    float halfW = pointSize * gsViewportInv.x * pos.w;
    float halfH = pointSize * gsViewportInv.y * pos.w;

    // Emit 4 vertices as a triangle strip forming a quad
    // UV layout: (0,0) top-left, (1,0) top-right, (0,1) bottom-left, (1,1) bottom-right

    // Top-left
    output.oPos = pos + float4(-halfW, -halfH, 0, 0);
    output.oT0 = float4(0, 0, 0, 1);
    stream.Append(output);

    // Top-right
    output.oPos = pos + float4(+halfW, -halfH, 0, 0);
    output.oT0 = float4(1, 0, 0, 1);
    stream.Append(output);

    // Bottom-left
    output.oPos = pos + float4(-halfW, +halfH, 0, 0);
    output.oT0 = float4(0, 1, 0, 1);
    stream.Append(output);

    // Bottom-right
    output.oPos = pos + float4(+halfW, +halfH, 0, 0);
    output.oT0 = float4(1, 1, 0, 1);
    stream.Append(output);

    stream.RestartStrip();
}
