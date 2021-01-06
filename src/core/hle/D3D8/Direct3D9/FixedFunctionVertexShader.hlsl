#include "FixedFunctionVertexShaderState.hlsli"

// Default values for vertex registers, and whether to use them
uniform float4 vRegisterDefaultValues[16] : register(c192);
uniform float4 vRegisterDefaultFlagsPacked[4] : register(c208);
static  bool  vRegisterDefaultFlags[16];

uniform FixedFunctionVertexShaderState state : register(c0);

uniform float4 xboxTextureScale[4] : register(c214);

#undef CXBX_ALL_TEXCOORD_INPUTS // Enable this to disable semantics in VS_INPUT (instead, we'll use an array of generic TEXCOORD's)

// Input registers
struct VS_INPUT
{
#ifdef CXBX_ALL_TEXCOORD_INPUTS
    float4 v[16] : TEXCOORD;
#else
    float4 pos : POSITION;
    float4 bw : BLENDWEIGHT;
    float4 color[2] : COLOR;
    float4 backColor[2] : TEXCOORD4;
    float4 normal : NORMAL;
    float4 texcoord[4] : TEXCOORD;
#endif
};

// Input register indices (also known as attributes, as given in VS_INPUT.v array)
// TODO : Convert FVF codes on CPU to a vertex declaration with these standardized register indices:
// NOTE : Converting FVF vertex indices must also consider NV2A vertex attribute 'slot mapping',
// as set in NV2A_VTXFMT/NV097_SET_VERTEX_DATA_ARRAY_FORMAT!
// TODO : Rename these into SLOT_POSITION, SLOT_WEIGHT, SLOT_TEXTURE0, SLOT_TEXTURE3, etc :
static const uint position = 0;     // See X_D3DFVF_XYZ      / X_D3DVSDE_POSITION    was float4 pos : POSITION;
static const uint weight = 1;       // See X_D3DFVF_XYZB1-4  / X_D3DVSDE_BLENDWEIGHT was float4 bw : BLENDWEIGHT;
static const uint normal = 2;       // See X_D3DFVF_NORMAL   / X_D3DVSDE_NORMAL      was float4 normal : NORMAL; // Note : Only normal.xyz is used. 
static const uint diffuse = 3;      // See X_D3DFVF_DIFFUSE  / X_D3DVSDE_DIFFUSE     was float4 color[2] : COLOR;
static const uint specular = 4;     // See X_D3DFVF_SPECULAR / X_D3DVSDE_SPECULAR
static const uint fogCoord = 5;     // Has no X_D3DFVF_* ! See X_D3DVSDE_FOG         Note : Only fog.x is used.
static const uint pointSize = 6;    // Has no X_D3DFVF_* ! See X_D3DVSDE_POINTSIZE
static const uint backDiffuse = 7;  // Has no X_D3DFVF_* ! See X_D3DVSDE_BACKDIFFUSE was float4 backColor[2] : TEXCOORD4;
static const uint backSpecular = 8; // Has no X_D3DFVF_* ! See X_D3DVSDE_BACKSPECULAR
static const uint texcoord0 = 9;    // See X_D3DFVF_TEX1     / X_D3DVSDE_TEXCOORD0   was float4 texcoord[4] : TEXCOORD;
static const uint texcoord1 = 10;   // See X_D3DFVF_TEX2     / X_D3DVSDE_TEXCOORD1
static const uint texcoord2 = 11;   // See X_D3DFVF_TEX3     / X_D3DVSDE_TEXCOORD2
static const uint texcoord3 = 12;   // See X_D3DFVF_TEX4     / X_D3DVSDE_TEXCOORD3
static const uint reserved0 = 13;   // Has no X_D3DFVF_*     / X_D3DVSDE_*
static const uint reserved1 = 14;   // Has no X_D3DFVF_*     / X_D3DVSDE_*
static const uint reserved2 = 15;   // Has no X_D3DFVF_*     / X_D3DVSDE_*

float4 Get(const VS_INPUT xIn, const uint index)
{
#ifdef CXBX_ALL_TEXCOORD_INPUTS
    return xIn.v[index];
#else
    // switch statements inexplicably don't work here
    if(index == position) return xIn.pos;
    if(index == weight) return xIn.bw;
    if(index == normal) return xIn.normal;
    if(index == diffuse) return xIn.color[0];
    if(index == specular) return xIn.color[1];
    if(index == backDiffuse) return xIn.backColor[0];
    if(index == backSpecular) return xIn.backColor[1];
    if(index == texcoord0) return xIn.texcoord[0];
    if(index == texcoord1) return xIn.texcoord[1];
    if(index == texcoord2) return xIn.texcoord[2];
    if(index == texcoord3) return xIn.texcoord[3];
    return 1;
#endif
}

// Output registers
struct VS_OUTPUT
{
    float4 oPos : POSITION;  // Homogeneous clip space position
    float4 oD0  : COLOR0;    // Primary color (front-facing)
    float4 oD1  : COLOR1;    // Secondary color (front-facing)
    float  oFog : FOG;       // Fog coordinate
    float  oPts : PSIZE;	 // Point size
    float4 oB0  : TEXCOORD4; // Back-facing primary color
    float4 oB1  : TEXCOORD5; // Back-facing secondary color
    float4 oT0  : TEXCOORD0; // Texture coordinate set 0
    float4 oT1  : TEXCOORD1; // Texture coordinate set 1
    float4 oT2  : TEXCOORD2; // Texture coordinate set 2
    float4 oT3  : TEXCOORD3; // Texture coordinate set 3
};

struct TransformInfo
{
    float4 Position;
    float3 Normal;
};

static TransformInfo View; // Vertex transformed to viewspace
static TransformInfo Projection; // Vertex transformed to projection space

// Vertex lighting
// Both frontface and backface lighting can be calculated
struct LightingInfo
{
    float3 Front;
    float3 Back;
};

// Final lighting output
struct LightingOutput
{
    LightingInfo Diffuse;
    LightingInfo Specular;
};

LightingInfo DoSpecular(const float3 toLightVN, const float3 toViewerVN, const float2 powers, const float4 lightSpecular)
{
    LightingInfo o;
    o.Front = o.Back = float3(0, 0, 0);

    // Specular
    if (state.Modes.SpecularEnable)
    {
        // Blinn-Phong
        // https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
        const float3 halfway = normalize(toViewerVN + toLightVN);
        const float NdotH = dot(View.Normal, halfway);

        const float3 frontSpecular = pow(abs(NdotH), powers[0]) * lightSpecular.rgb;
        const float3 backSpecular = pow(abs(NdotH), powers[1]) * lightSpecular.rgb;

        if (NdotH >= 0)
            o.Front = frontSpecular;
        else
            o.Back = backSpecular;
    }

    return o;
}

// useful reference https://drivers.amd.com/misc/samples/dx9/FixedFuncShader.pdf

LightingOutput DoPointLight(const Light l, const float3 toViewerVN, const float2 powers)
{
    LightingOutput o;
    o.Diffuse.Front = o.Diffuse.Back = float3(0, 0, 0);
    o.Specular.Front = o.Specular.Back = float3(0, 0, 0);

    // Diffuse
    const float3 toLightV = l.PositionV - View.Position.xyz;
    const float lightDist = length(toLightV);
    const float3 toLightVN = normalize(toLightV);

    // A(Constant) + A(Linear) * dist + A(Exp) * dist^2
    float attenuation =
        1 / (l.Attenuation[0]
        + l.Attenuation[1] * lightDist
        + l.Attenuation[2] * lightDist * lightDist);

    // Range cutoff
    if (lightDist > l.Range)
        attenuation = 0;

    const float NdotL = dot(View.Normal, toLightVN);
    const float3 lightDiffuse = abs(NdotL) * attenuation * l.Diffuse.rgb;

    if (NdotL >= 0)
        o.Diffuse.Front = lightDiffuse;
    else
        o.Diffuse.Back = lightDiffuse;

    // Specular
    o.Specular = DoSpecular(toLightVN, toViewerVN, powers, l.Specular);
    o.Specular.Front *= attenuation;
    o.Specular.Back *= attenuation;

    return o;
}

LightingOutput DoSpotLight(const Light l, const float3 toViewerVN, const float2 powers)
{
    LightingOutput o;
    o.Diffuse.Front = o.Diffuse.Back = float3(0, 0, 0);
    o.Specular.Front = o.Specular.Back = float3(0, 0, 0);

    // Diffuse
    const float3 toLightV = l.PositionV - View.Position.xyz;
    const float lightDist = length(toLightV);
    const float3 toLightVN = normalize(toLightV);
    const float3 toVertexVN = -toLightVN;

    // https://docs.microsoft.com/en-us/windows/win32/direct3d9/light-types
    const float cosAlpha = dot(l.DirectionVN, toVertexVN);
    // I = ( cos(a) - cos(phi/2) ) / ( cos(theta/2) - cos(phi/2) )
    const float spotBase = saturate((cosAlpha - l.CosHalfPhi) / l.SpotIntensityDivisor);
    const float spotIntensity = pow(spotBase, l.Falloff);

    // A(Constant) + A(Linear) * dist + A(Exp) * dist^2
    float attenuation =
        1 / (l.Attenuation[0]
        + l.Attenuation[1] * lightDist
        + l.Attenuation[2] * lightDist * lightDist);

    // Range cutoff
    if (lightDist > l.Range)
        attenuation = 0;

    const float NdotL = dot(View.Normal, toLightVN);
    const float3 lightDiffuse = abs(NdotL) * attenuation * l.Diffuse.rgb * spotIntensity;

    if (NdotL >= 0)
        o.Diffuse.Front = lightDiffuse;
    else
        o.Diffuse.Back = lightDiffuse;

    // Specular
    o.Specular = DoSpecular(toLightVN, toViewerVN, powers, l.Specular);
    o.Specular.Front *= attenuation;
    o.Specular.Back *= attenuation;

    return o;
}

LightingOutput DoDirectionalLight(const Light l, const float3 toViewerVN, const float2 powers)
{
    LightingOutput o;
    o.Diffuse.Front = o.Diffuse.Back = float3(0, 0, 0);
    o.Specular.Front = o.Specular.Back = float3(0, 0, 0);

    // Diffuse

    // Intensity from N . L
    const float3 toLightVN = -l.DirectionVN;
    const float NdotL = dot(View.Normal, toLightVN);
    const float3 lightDiffuse = abs(NdotL * l.Diffuse.rgb);

    // Apply light contribution to front or back face
    // as the case may be
    if (NdotL >= 0)
        o.Diffuse.Front = lightDiffuse;
    else
        o.Diffuse.Back = lightDiffuse;

    // Specular
    o.Specular = DoSpecular(toLightVN, toViewerVN, powers, l.Specular);

    return o;
}


LightingOutput CalcLighting(const float2 powers)
{
    static const int LIGHT_TYPE_NONE        = 0;
    static const int LIGHT_TYPE_POINT       = 1;
    static const int LIGHT_TYPE_SPOT        = 2;
    static const int LIGHT_TYPE_DIRECTIONAL = 3;

    LightingOutput totalLightOutput;
    totalLightOutput.Diffuse.Front = float3(0, 0, 0);
    totalLightOutput.Diffuse.Back = float3(0, 0, 0);
    totalLightOutput.Specular.Front = float3(0, 0, 0);
    totalLightOutput.Specular.Back = float3(0, 0, 0);

    const float3 toViewerVN = state.Modes.LocalViewer
        ? float3(0, 0, 1)
        : normalize(-View.Position.xyz);

    for (uint i = 0; i < 8; i++)
    {
        const Light currentLight = state.Lights[i];
        LightingOutput currentLightOutput;

        if(currentLight.Type == LIGHT_TYPE_POINT)
            currentLightOutput = DoPointLight(currentLight, toViewerVN, powers);
        else if(currentLight.Type == LIGHT_TYPE_SPOT)
            currentLightOutput = DoSpotLight(currentLight, toViewerVN, powers);
        else if (currentLight.Type == LIGHT_TYPE_DIRECTIONAL)
            currentLightOutput = DoDirectionalLight(currentLight, toViewerVN, powers);
        else
            continue;

        totalLightOutput.Diffuse.Front += currentLightOutput.Diffuse.Front;
        totalLightOutput.Diffuse.Back += currentLightOutput.Diffuse.Back;
        totalLightOutput.Specular.Front += currentLightOutput.Specular.Front;
        totalLightOutput.Specular.Back += currentLightOutput.Specular.Back;
    }

    return totalLightOutput;
}

TransformInfo DoTransform(const float4 position, const float3 normal, const float4 blendWeights)
{
    TransformInfo output;
    output.Position = float4(0, 0, 0, 0);
    output.Normal = float3(0, 0, 0);

    // The number of matrices to blend (always in the range [1..4])
    const int matrices = state.Modes.VertexBlend_NrOfMatrices;

    // Initialize the final matrix its blend weight at 1, from which all preceding blend weights will be deducted :
    float lastBlend = 1;
    for (int i = 0; i < matrices; i++)
    {
        // Do we have to calculate the last blend value (never happens when there's already 4 matrices) ?
        const bool bCalcFinalWeight = (state.Modes.VertexBlend_CalcLastWeight > 0) && (i == (matrices - 1));
        // Note : In case of X_D3DVBF_DISABLE, no prior weights have been deducted from lastBlend, so it will still be 1.
        // The number of matrices will also be 1, which effectively turns this into non-weighted single-matrix multiplications :
        const float blendWeight = bCalcFinalWeight ? lastBlend : blendWeights[i];
        // Reduce the blend weight for the final matrix :
        lastBlend -= blendWeights[i];
        // Add this matrix (multiplied by its blend weight) to the output :
        output.Position += mul(position, state.Transforms.WorldView[i]) * blendWeight;
        output.Normal += mul(normal, (float3x3) state.Transforms.WorldViewInverseTranspose[i]) * blendWeight;
    }

    return output;
}

Material DoMaterial(const uint index, const uint diffuseReg, const uint specularReg, const VS_INPUT xIn)
{
    // Get the material from material state
    Material material = state.Materials[index];

    // Note : If X_D3DRS_COLORVERTEX is FALSE, UpdateFixedFunctionVertexShaderState has already changed all MaterialSource's into D3DMCS_MATERIAL

    // https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dmaterialcolorsource
    static const int D3DMCS_MATERIAL = 0;
    static const int D3DMCS_COLOR1 = 1;
    static const int D3DMCS_COLOR2 = 2;

    // If COLORVERTEX mode, AND the desired diffuse or specular colour is defined in the vertex declaration
    // Then use the vertex colour instead of the material

    if (!vRegisterDefaultFlags[diffuseReg]) {
        const float4 diffuseVertexColour = Get(xIn, diffuseReg);
        if (state.Modes.AmbientMaterialSource == D3DMCS_COLOR1) material.Ambient = diffuseVertexColour;
        if (state.Modes.DiffuseMaterialSource == D3DMCS_COLOR1) material.Diffuse = diffuseVertexColour;
        if (state.Modes.SpecularMaterialSource == D3DMCS_COLOR1) material.Specular = diffuseVertexColour;
        if (state.Modes.EmissiveMaterialSource == D3DMCS_COLOR1) material.Emissive = diffuseVertexColour;
    }

    if (!vRegisterDefaultFlags[specularReg]) {
        const float4 specularVertexColour = Get(xIn, specularReg);
        if (state.Modes.AmbientMaterialSource == D3DMCS_COLOR2) material.Ambient = specularVertexColour;
        if (state.Modes.DiffuseMaterialSource == D3DMCS_COLOR2) material.Diffuse = specularVertexColour;
        if (state.Modes.SpecularMaterialSource == D3DMCS_COLOR2) material.Specular = specularVertexColour;
        if (state.Modes.EmissiveMaterialSource == D3DMCS_COLOR2) material.Emissive = specularVertexColour;
    }

    return material;
}

float DoFog(const VS_INPUT xIn)
{
    // TODO implement properly
    // Until we have pixel shader HLSL we are still leaning on D3D renderstates for fogging
    // So we are not doing any fog density calculations here
    // http://developer.download.nvidia.com/assets/gamedev/docs/Fog2.pdf

    float fogDepth;

    if (state.Fog.DepthMode == FixedFunctionVertexShader::FOG_DEPTH_NONE)
        fogDepth = Get(xIn, specular).a; // In fixed-function mode, fog is passed in the specular alpha
    if (state.Fog.DepthMode == FixedFunctionVertexShader::FOG_DEPTH_RANGE)
        fogDepth = length(View.Position.xyz);
    if (state.Fog.DepthMode == FixedFunctionVertexShader::FOG_DEPTH_Z)
        fogDepth = abs(Projection.Position.z);
    if (state.Fog.DepthMode == FixedFunctionVertexShader::FOG_DEPTH_W)
        fogDepth = Projection.Position.w;

    return fogDepth;
}

float4 DoTexCoord(const uint stage, const VS_INPUT xIn)
{
    // Texture transform flags
    // https://docs.microsoft.com/en-gb/windows/win32/direct3d9/d3dtexturetransformflags
    static const int D3DTTFF_DISABLE = 0;
    static const int D3DTTFF_COUNT1  = 1;
    static const int D3DTTFF_COUNT2  = 2;
    static const int D3DTTFF_COUNT3  = 3;
    static const int D3DTTFF_COUNT4  = 4;
    static const int D3DTTFF_PROJECTED = 256; // This is the only real flag

    // https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dtss-tci
    // Pre-shifted
    static const int TCI_PASSTHRU = 0;
    static const int TCI_CAMERASPACENORMAL = 1;
    static const int TCI_CAMERASPACEPOSITION = 2;
    static const int TCI_CAMERASPACEREFLECTIONVECTOR = 3;
    static const int TCI_OBJECT = 4; // Xbox
    static const int TCI_SPHERE = 5; // Xbox

    const TextureState tState = state.TextureStates[stage];

    // Extract transform flags
    const int countFlag = tState.TextureTransformFlagsCount;
    const bool projected = tState.TextureTransformFlagsProjected;

    // Get texture coordinates
    // Coordinates are either from the vertex texcoord data
    // Or generated
    float4 texCoord = float4(0, 0, 0, 0);
    if (tState.TexCoordIndexGen == TCI_PASSTHRU)
    {
        // Get from vertex data
        const uint texCoordIndex = abs(tState.TexCoordIndex); // Note : abs() avoids error X3548 : in vs_3_0 uints can only be used with known - positive values, use int if possible
        texCoord = Get(xIn, texcoord0+texCoordIndex);

        // Make coordinates homogenous
        // For example, if a title supplies (u, v)
        // We need to make transform (u, v, 1) to allow translation with a 3x3 matrix
        // We'll need to get this from the current FVF or VertexDeclaration
        // Test case: JSRF scrolling texture effect.
        // Test case: Madagascar shadows
        // Test case: Modify pixel shader sample

       // TODO move alongside the texture transformation when it stops angering the HLSL compiler
        const float componentCount = state.TexCoordComponentCount[texCoordIndex];
        if (componentCount == 1)
            texCoord.yzw = float3(1, 0, 0);
        if (componentCount == 2)
            texCoord.zw = float2(1, 0);
        if (componentCount == 3)
            texCoord.w = 1;
    }   // Generate texture coordinates
    else if (tState.TexCoordIndexGen == TCI_CAMERASPACENORMAL)
        texCoord = float4(View.Normal, 1);
    else if (tState.TexCoordIndexGen == TCI_CAMERASPACEPOSITION)
        texCoord = View.Position;
    else
    {
        const float3 reflected = reflect(normalize(View.Position.xyz), View.Normal);

        if (tState.TexCoordIndexGen == TCI_CAMERASPACEREFLECTIONVECTOR)
            texCoord.xyz = reflected;
        // else if TCI_OBJECT TODO is this just model position?
        else if (tState.TexCoordIndexGen == TCI_SPHERE)
        {
            // TODO verify
            // http://www.bluevoid.com/opengl/sig99/advanced99/notes/node177.html
            const float3 R = reflected;
            const float p = sqrt(pow(R.x, 2) + pow(R.y, 2) + pow(R.z + 1, 2));
            texCoord.x = R.x / 2 * p + 0.5f;
            texCoord.y = R.y / 2 * p + 0.5f;
        }
    }

    // Transform the texture coordinates if requested
    if (countFlag != D3DTTFF_DISABLE)
        texCoord = mul(texCoord, state.Transforms.Texture[stage]);

    // We always send four coordinates
    // If we are supposed to send less than four
    // then copy the last coordinate to the remaining coordinates
    // For D3DTTFF_PROJECTED, the value of the *last* coordinate is important
    // Test case: ProjectedTexture sample, which uses 3 coordinates
    // We'll need to implement the divide when D3D stops handling it for us?
    // https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dtexturetransformflags
    if (projected)
    {
        if (countFlag == 1)
            texCoord.yzw = texCoord.x;
        if (countFlag == 2)
            texCoord.zw = texCoord.y;
        if (countFlag == 3)
            texCoord.w = texCoord.z;
    }

    return texCoord;
}

// Point size for Point Sprites
// https://docs.microsoft.com/en-us/windows/win32/direct3d9/point-sprites
// Test case: Point sprite sample
float DoPointSpriteSize()
{
    const PointSprite ps = state.PointSprite;
    float pointSize = ps.PointSize;

    if (ps.PointScaleEnable)
    {
        const float eyeDistance = length(View.Position);
        const float factor = ps.ScaleA + ps.ScaleB * eyeDistance + ps.ScaleC * (eyeDistance * eyeDistance);

        pointSize *= ps.RenderTargetHeight * sqrt(1 / factor);
    }

    return pointSize;
}

VS_INPUT InitializeInputRegisters(const VS_INPUT xInput)
{
    VS_INPUT xIn;

    // Initialize input registers from the vertex buffer data
    // Or use the register's default value (which can be changed by the title)
    for (uint i = 0; i < 16; i++)
    {
        const float4 value = lerp(Get(xInput, i), vRegisterDefaultValues[i], vRegisterDefaultFlags[i]);
        #ifdef CXBX_ALL_TEXCOORD_INPUTS
            xIn.v[i] = value;
        #else
            // switch statements inexplicably don't work here
            if(i == position) xIn.pos = value;
            if(i == weight) xIn.bw = value;
            if(i == normal) xIn.normal = value;
            if(i == diffuse) xIn.color[0] = value;
            if(i == specular) xIn.color[1] = value;
            if(i == backDiffuse) xIn.backColor[0] = value;
            if(i == backSpecular) xIn.backColor[1] = value;
            if(i == texcoord0) xIn.texcoord[0] = value;
            if(i == texcoord1) xIn.texcoord[1] = value;
            if(i == texcoord2) xIn.texcoord[2] = value;
            if(i == texcoord3) xIn.texcoord[3] = value;
        #endif
    }

    return xIn;
}

VS_OUTPUT main(const VS_INPUT xInput)
{
    VS_OUTPUT xOut;

    // Unpack 16 bool flags from 4 float4 constant registers
    vRegisterDefaultFlags = (bool[16]) vRegisterDefaultFlagsPacked;

    // TODO make sure this goes fast

    // Map default values
    VS_INPUT xIn = InitializeInputRegisters(xInput);

    // World + View transform with vertex blending
    View = DoTransform(Get(xIn, position), Get(xIn, normal).xyz, Get(xIn, weight));

    // Optionally normalize camera-space normals
    if (state.Modes.NormalizeNormals)
        View.Normal = normalize(View.Normal);

    // Projection transform
    Projection.Position = mul(View.Position, state.Transforms.Projection);
    // Normal unused...

    // Projection transform - final position
    xOut.oPos = Projection.Position;

    // Vertex lighting
    if (state.Modes.Lighting || state.Modes.TwoSidedLighting)
    {
        // Materials
        const Material material = DoMaterial(0, diffuse, specular, xIn);
        const Material backMaterial = DoMaterial(1, backDiffuse, backSpecular, xIn);

        const float2 powers = float2(material.Power, backMaterial.Power);

        LightingOutput lighting = CalcLighting(powers);

        // Compute each lighting component
        const float3 _ambient = material.Ambient.rgb * state.AmbientPlusLightAmbient.rgb;
        const float3 _backAmbient = backMaterial.Ambient.rgb * state.BackAmbientPlusLightAmbient.rgb;

        const float3 _diffuse = material.Diffuse.rgb * lighting.Diffuse.Front;
        const float3 _backDiffuse = backMaterial.Diffuse.rgb * lighting.Diffuse.Back;

        const float3 _specular = material.Specular.rgb * lighting.Specular.Front;
        const float3 _backSpecular = backMaterial.Specular.rgb * lighting.Specular.Back;

        const float3 _emissive = material.Emissive.rgb;
        const float3 _backEmissive = backMaterial.Emissive.rgb;

        // Frontface
        xOut.oD0 = float4(_ambient + _diffuse + _emissive, material.Diffuse.a);
        xOut.oD1 = float4(_specular, 0);
        // Backface
        xOut.oB0 = float4(_backAmbient + _backDiffuse + _backEmissive, backMaterial.Diffuse.a);
        xOut.oB1 = float4(_backSpecular, 0);
    }

    // TODO does TwoSidedLighting imply Lighting? Verify if TwoSidedLighting can be enabled independently of Lighting
    // Diffuse and specular for when lighting is disabled
    if (!state.Modes.Lighting)
    {
        xOut.oD0 = Get(xIn, diffuse);
        xOut.oD1 = Get(xIn, specular);
    }

    if(!state.Modes.TwoSidedLighting)
    {
        xOut.oB0 = Get(xIn, backDiffuse);
        xOut.oB1 = Get(xIn, backSpecular);
    }

    // Colour
    xOut.oD0 = saturate(xOut.oD0);
    xOut.oD1 = saturate(xOut.oD1);
    xOut.oB0 = saturate(xOut.oB0);
    xOut.oB1 = saturate(xOut.oB1);

    // Fog
    xOut.oFog = DoFog(xIn);

    // Point Sprite
    xOut.oPts = DoPointSpriteSize();

    // Texture coordinates
    xOut.oT0 = DoTexCoord(0, xIn) / xboxTextureScale[0];
    xOut.oT1 = DoTexCoord(1, xIn) / xboxTextureScale[1];
    xOut.oT2 = DoTexCoord(2, xIn) / xboxTextureScale[2];
    xOut.oT3 = DoTexCoord(3, xIn) / xboxTextureScale[3];

    return xOut;
}
