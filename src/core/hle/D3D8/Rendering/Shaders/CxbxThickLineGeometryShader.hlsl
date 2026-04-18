// CxbxThickLineGeometryShader.hlsl
// Geometry shader that expands line segments into screen-aligned quads
// for D3D11 thick line emulation. D3D11 has no native line width control
// (all lines render at 1px), so we expand each line to a quad here.
//
// The VS outputs clip-space positions for two line endpoints.
// This GS computes a perpendicular offset in screen space based on the
// line width (from gsParams.z), then emits 4 vertices forming a quad.
// Vertex attributes (colors, fog, tex coords) are interpolated from
// the two endpoints — start attributes for the left pair, end for right.
//
// NOTE: This file is a reference copy. The actual shader is compiled from
// a string literal in CxbxD3D11InitBlit() in Backend_D3D11.cpp.

struct GS_INPUT {
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

struct GS_OUTPUT {
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

// xy = 1/viewportWidth, 1/viewportHeight; z = lineWidth
cbuffer GSConstants : register(b0) { float4 gsParams; };

[maxvertexcount(4)]
void main(line GS_INPUT input[2], inout TriangleStream<GS_OUTPUT> stream) {
    float4 p0 = input[0].oPos;
    float4 p1 = input[1].oPos;

    // Convert to NDC
    float2 ndc0 = p0.xy / p0.w;
    float2 ndc1 = p1.xy / p1.w;

    // Line direction in NDC, then to screen pixels
    float2 screenDir = (ndc1 - ndc0) / (gsParams.xy * 2.0);
    float len = length(screenDir);
    float2 dir = (len > 0.0001) ? screenDir / len : float2(1, 0);

    // Perpendicular in screen space, then back to clip space
    float2 perp = float2(-dir.y, dir.x);
    float halfWidth = gsParams.z * 0.5;
    float2 offset0 = perp * halfWidth * gsParams.xy * 2.0 * p0.w;
    float2 offset1 = perp * halfWidth * gsParams.xy * 2.0 * p1.w;

    GS_OUTPUT o;

    // Vertex 0 — start, left side
    o.oD0  = input[0].oD0; o.oD1  = input[0].oD1;
    o.oFog = input[0].oFog; o.oPts = input[0].oPts;
    o.oB0  = input[0].oB0; o.oB1  = input[0].oB1;
    o.oT0  = input[0].oT0; o.oT1  = input[0].oT1;
    o.oT2  = input[0].oT2; o.oT3  = input[0].oT3;
    o.oPos = p0 + float4(-offset0, 0, 0);
    stream.Append(o);

    // Vertex 1 — start, right side
    o.oPos = p0 + float4(+offset0, 0, 0);
    stream.Append(o);

    // Vertex 2 — end, left side (interpolate from input[1])
    o.oD0  = input[1].oD0; o.oD1  = input[1].oD1;
    o.oFog = input[1].oFog; o.oPts = input[1].oPts;
    o.oB0  = input[1].oB0; o.oB1  = input[1].oB1;
    o.oT0  = input[1].oT0; o.oT1  = input[1].oT1;
    o.oT2  = input[1].oT2; o.oT3  = input[1].oT3;
    o.oPos = p1 + float4(-offset1, 0, 0);
    stream.Append(o);

    // Vertex 3 — end, right side
    o.oPos = p1 + float4(+offset1, 0, 0);
    stream.Append(o);

    stream.RestartStrip();
}
