// Xbox HLSL pretransformed vertex shader

// Default values for vertex registers, and whether to use them
uniform float4 vRegisterDefaultValues[16]  : register(c192);
uniform float4 vRegisterDefaultFlagsPacked[4]  : register(c208);

uniform float4 xboxScreenspaceScale : register(c212);
uniform float4 xboxScreenspaceOffset : register(c213);


uniform float4 xboxTextureScale[4] : register(c214);

// Parameters for mapping the shader's fog output value to a fog factor
uniform float4  CxbxFogInfo: register(c218); // = CXBX_D3DVS_CONSTREG_FOGINFO

struct VS_INPUT
{
    float4 v[16] : TEXCOORD;
};

// Output registers
struct VS_OUTPUT
{
    float4 oPos : POSITION;  // Homogeneous clip space position
    float4 oD0  : COLOR0;    // Primary color (front-facing)
    float4 oD1  : COLOR1;    // Secondary color (front-facing)
    float  oFog : FOG;       // Fog coordinate
    float  oPts : PSIZE;     // Point size
    float4 oB0  : TEXCOORD4; // Back-facing primary color
    float4 oB1  : TEXCOORD5; // Back-facing secondary color
    float4 oT0  : TEXCOORD0; // Texture coordinate set 0
    float4 oT1  : TEXCOORD1; // Texture coordinate set 1
    float4 oT2  : TEXCOORD2; // Texture coordinate set 2
    float4 oT3  : TEXCOORD3; // Texture coordinate set 3
};

float4 reverseScreenspaceTransform(float4 oPos)
{
    // Scale screenspace coordinates (0 to viewport width/height) to -1 to +1 range

    // On Xbox, oPos should contain the vertex position in screenspace
    // We need to reverse this transformation
    // Conventionally, each Xbox Vertex Shader includes instructions like this
    // mul oPos.xyz, r12, c-38
    // +rcc r1.x, r12.w
    // mad oPos.xyz, r12, r1.x, c-37
    // where c-37 and c-38 are reserved transform values

    // Reverse screenspace offset
    oPos -= xboxScreenspaceOffset;
    // Reverse screenspace scale
    oPos /= xboxScreenspaceScale;

    // Ensure w is nonzero
    if(oPos.w == 0) oPos.w = 1;
    // Reverse perspective divide
    oPos.xyz *= oPos.w;
    return oPos;
}

VS_OUTPUT main(const VS_INPUT xIn)
{
    // Input registers
    float4 v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15;

    // Unpack 16 flags from 4 float4 constant registers
    float vRegisterDefaultFlags[16] = (float[16])vRegisterDefaultFlagsPacked;

    // Initialize input registers from the vertex buffer data
    // Or use the register's default value (which can be changed by the title)
    #define init_v(i) v##i = lerp(xIn.v[i], vRegisterDefaultValues[i], vRegisterDefaultFlags[i]);
    // Note : unroll manually instead of for-loop, because of the ## concatenation
    init_v( 0); init_v( 1); init_v( 2); init_v( 3);
    init_v( 4); init_v( 5); init_v( 6); init_v( 7);
    init_v( 8); init_v( 9); init_v(10); init_v(11);
    init_v(12); init_v(13); init_v(14); init_v(15);

    // For passthrough, map output variables to their corresponding input registers
    float4 oPos = v0;
    float4 oD0 = v3;
    float4 oD1 = v4;
    float4 oFog = v5;
    float4 oPts = v6;
    float4 oB0 = v7;
    float4 oB1 = v8;
    float4 oT0 = v9;
    float4 oT1 = v10;
    float4 oT2 = v11;
    float4 oT3 = v12;

    // Copy variables to output struct
    VS_OUTPUT xOut;

    

    xOut.oPos = reverseScreenspaceTransform(oPos);
    xOut.oD0 = saturate(oD0);
    xOut.oD1 = saturate(oD1);
    xOut.oFog = oFog.x; // Note : Xbox clamps fog in pixel shader
    xOut.oPts = oPts.x;
    xOut.oB0 = saturate(oB0);
    xOut.oB1 = saturate(oB1);
    // Scale textures (TODO: or should we apply this to the input register values?)
    xOut.oT0 = oT0 / xboxTextureScale[0];
    xOut.oT1 = oT1 / xboxTextureScale[1];
    xOut.oT2 = oT2 / xboxTextureScale[2];
    xOut.oT3 = oT3 / xboxTextureScale[3];

    return xOut;
}
