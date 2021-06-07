#include "FixedFunctionPixelShader.hlsli"

uniform FixedFunctionPixelShaderState state : register(c0);
sampler samplers[4] : register(s0);

struct PS_INPUT // Declared identical to vertex shader output (see VS_OUTPUT)
{
	float2 iPos : VPOS;   // Screen space x,y pixel location
	float4 iD0  : COLOR0; // Front-facing primary (diffuse) vertex color (clamped to 0..1)
	float4 iD1  : COLOR1; // Front-facing secondary (specular) vertex color (clamped to 0..1)
	float  iFog : FOG;
	float  iPts : PSIZE;
	float4 iB0  : TEXCOORD4; // Back-facing primary (diffuse) vertex color (clamped to 0..1)
	float4 iB1  : TEXCOORD5; // Back-facing secondary (specular) vertex color (clamped to 0..1)
	float4 iT[4]  : TEXCOORD0; // Texture Coord 0
	float  iFF : VFACE; // Front facing if > 0
};

// These 'D3DTA' texture argument values
// may be used during each texture stage
struct TextureArgs {
	float4 CURRENT;
	float4 TEXTURE;
	float4 DIFFUSE;
	float4 SPECULAR;
	float4 TEMP;
	float4 TFACTOR;
};

static float4 TexCoords[4];

// When creating an instance of the fixed function shader
// we string-replace the assignment below with a value
// The define keeps the shader compilable without the replacement
#define TEXTURE_SAMPLE_TYPE {SAMPLE_2D, SAMPLE_2D, SAMPLE_2D, SAMPLE_2D};
static int TextureSampleType[4] = TEXTURE_SAMPLE_TYPE;

bool HasFlag(float value, float flag) {
	// http://theinstructionlimit.com/encoding-boolean-flags-into-a-float-in-hlsl
	return fmod(value, flag) >= flag / 2;
}

float4 GetArg(float arg, TextureArgs ctx) {
	// https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dta
	bool alphaReplicate = HasFlag(arg, X_D3DTA_ALPHAREPLICATE);
	bool complement = HasFlag(arg, X_D3DTA_COMPLEMENT);
	arg = arg % 16;

	float4 o;

	if (arg == X_D3DTA_DIFFUSE)
		o = ctx.DIFFUSE;
	if (arg == X_D3DTA_CURRENT)
		o = ctx.CURRENT;
	if (arg == X_D3DTA_TEXTURE)
		o = ctx.TEXTURE;
	if (arg == X_D3DTA_TFACTOR)
		o = ctx.TFACTOR;
	if (arg == X_D3DTA_SPECULAR)
		o = ctx.SPECULAR;
	if (arg == X_D3DTA_TEMP)
		o = ctx.TEMP;

	if (alphaReplicate)
		return o.aaaa;
	else if (complement)
		return 1 - o;
	else
		return o;
}

float4 ExecuteTextureOp(float op, float4 arg1, float4 arg2, float4 arg0, TextureArgs ctx, PsTextureStageState stage) {
	// https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dtextureop

	// Note if we use ifs here instead of else if
	// D3DCompile may stackoverflow at runtime
	if (op == X_D3DTOP_SELECTARG1)
		return arg1;
	else if (op == X_D3DTOP_SELECTARG2)
		return arg2;
	else if (op == X_D3DTOP_MODULATE)
		return arg1 * arg2;
	else if (op == X_D3DTOP_MODULATE2X)
		return 2 * (arg1 * arg2);
	else if (op == X_D3DTOP_MODULATE4X)
		return 4 * (arg1 * arg2);
	else if (op == X_D3DTOP_ADD)
		return arg1 + arg2;
	else if (op == X_D3DTOP_ADDSIGNED)
		return arg1 + arg2 - 0.5;
	else if (op == X_D3DTOP_ADDSIGNED2X)
		return 2 * (arg1 + arg2 - 0.5);
	else if (op == X_D3DTOP_SUBTRACT)
		return arg1 - arg2;
	else if (op == X_D3DTOP_ADDSMOOTH)
		return arg1 + arg2 * (1 - arg1);
	else if (op == X_D3DTOP_BLENDDIFFUSEALPHA)
		return arg1 * ctx.DIFFUSE.a + arg2 * (1 - ctx.DIFFUSE.a);
	else if (op == X_D3DTOP_BLENDCURRENTALPHA)
		return arg1 * ctx.CURRENT.a + arg2 * (1 - ctx.CURRENT.a);
	else if (op == X_D3DTOP_BLENDTEXTUREALPHA)
		return arg1 * ctx.TEXTURE.a + arg2 * (1 - ctx.TEXTURE.a);
	else if (op == X_D3DTOP_BLENDFACTORALPHA)
		return arg1 * ctx.TFACTOR.a + arg2 * (1 - ctx.TFACTOR.a);
	else if (op == X_D3DTOP_BLENDTEXTUREALPHAPM)
		return arg1 + arg2 * (1 - ctx.TEXTURE.a);
	else if (op == X_D3DTOP_PREMODULATE)
		return arg1; // Note this also multiplies the next stage's CURRENT by its texture
	else if (op == X_D3DTOP_MODULATEALPHA_ADDCOLOR)
		return float4(arg1.rgb + arg1.a * arg2.rgb, 1);
	else if (op == X_D3DTOP_MODULATECOLOR_ADDALPHA)
		return float4(arg1.rgb * arg2.rgb + arg1.a, 1);
	else if (op == X_D3DTOP_MODULATEINVALPHA_ADDCOLOR)
		return float4((1 - arg1.a) * arg2.rgb + arg1.rgb, 1);
	else if (op == X_D3DTOP_MODULATEINVCOLOR_ADDALPHA)
		return float4((1 - arg1.rgb) * arg2.rgb + arg1.a, 1);
	else if (op == X_D3DTOP_DOTPRODUCT3)
		// Test case: PerPixelLighting
		return saturate(dot(
			(arg1.rgb - 0.5) * 2,
			(arg2.rgb - 0.5) * 2
		));
	// Note arg0 below is arg1 in D3D docs
	// since it becomes the first argument for operations supporting 3 arguments...
	else if (op == X_D3DTOP_MULTIPLYADD)
		return arg0 + arg1 * arg2;
	else if (op == X_D3DTOP_LERP)
		return arg0 * arg1 + (1 - arg0) * arg2;
	else if (op == X_D3DTOP_BUMPENVMAP)
		return float4(
			arg1.x * stage.BUMPENVMAT00 + arg1.y * stage.BUMPENVMAT10,
			arg1.x * stage.BUMPENVMAT01 + arg1.y * stage.BUMPENVMAT11,
			1, 1);
	else if (op == X_D3DTOP_BUMPENVMAPLUMINANCE)
		return float4(
			arg1.x * stage.BUMPENVMAT00 + arg1.y * stage.BUMPENVMAT10,
			arg1.x * stage.BUMPENVMAT01 + arg1.y * stage.BUMPENVMAT11,
			arg1.z * stage.BUMPENVLSCALE + stage.BUMPENVLOFFSET,
			1);

	// Something is amiss... we should have returned by now!
	// Return a bright colour
	return float4(0, 1, 1, 1);
}

TextureArgs ExecuteTextureStage(
	int i,
	TextureArgs ctx,
	PsTextureHardcodedState s,
	int previousOp
)
{
	// Early exit if this stage is disabled (and therefore all further stages are too)
	if (s.COLOROP == X_D3DTOP_DISABLE)
		return ctx;

	PsTextureStageState stage = state.stages[i];

	// Determine the texture for this stage
	float3 offset = float3(0, 0, 0);
	float4 factor = float4(1, 1, 1, 1);

	// Bumpmap special case
	if (previousOp == X_D3DTOP_BUMPENVMAP ||
		previousOp == X_D3DTOP_BUMPENVMAPLUMINANCE) {
		// Assume U, V, L is in CURRENT
		// Add U', V', to the texture coordinates
		// And multiply by L'
		// https://docs.microsoft.com/en-us/windows/win32/direct3d9/bump-mapping-formulas
		offset.xy = ctx.CURRENT.xy;
		factor.rgb = ctx.CURRENT.z;
	}

	// Sample the texture
	float4 t;
	int type = TextureSampleType[i];
	if (type == SAMPLE_NONE)
		t = 1; // Test case JSRF
	else if (type == SAMPLE_2D)
		t = tex2D(samplers[i], TexCoords[i].xy + offset.xy);
	else if (type == SAMPLE_3D)
		t = tex3D(samplers[i], TexCoords[i].xyz + offset.xyz);
	else if (type == SAMPLE_CUBE)
		t = texCUBE(samplers[i], TexCoords[i].xyz + offset.xyz);

#ifdef ENABLE_FF_ALPHAKILL
	if (stage.ALPHAKILL)
		if (t.a == 0)
			discard;

#endif
	// Assign the final value for TEXTURE
	ctx.TEXTURE = t * factor;

	// Premodulate special case
	if (previousOp == X_D3DTOP_PREMODULATE) {
		ctx.CURRENT *= ctx.TEXTURE;
	}

	// Get arguments for the texture operation
	// Almost all operate on 2 arguments, Arg1 and Arg2
	// Arg0 is a third argument that seems to have been tacked on
	// for MULTIPLYADD and LERP

	// Colour operation arguments
	float4 cArg1 = GetArg(s.COLORARG1, ctx);
	float4 cArg2 = GetArg(s.COLORARG2, ctx);
	float4 cArg0 = GetArg(s.COLORARG0, ctx);

	// Alpha operation arguments
	float4 aArg1 = GetArg(s.ALPHAARG1, ctx);
	float4 aArg2 = GetArg(s.ALPHAARG2, ctx);
	float4 aArg0 = GetArg(s.ALPHAARG0, ctx);

	// Execute texture operation
	// ALPHAOP == X_D3DTOP_DISABLE is undefined behaviour
	// Using an intermediate value matches known cases...
	// Test case: DoA:Xtreme (menu water), GTA III (logos), Crash Wrath of Cortex (relics UI)
	static float4 value = 1;
	value.rgb = ExecuteTextureOp(s.COLOROP, cArg1, cArg2, cArg0, ctx, stage).rgb;
	if (s.ALPHAOP != X_D3DTOP_DISABLE) {
		value.a = ExecuteTextureOp(s.ALPHAOP, aArg1, aArg2, aArg0, ctx, stage).a;
	}

	// Save the result
	// Note RESULTARG should either be CURRENT or TEMP
	// But some titles seem to set it to DIFFUSE
	// Use CURRENT for anything other than TEMP
	// Test case: DoA 3
	if (s.RESULTARG == X_D3DTA_TEMP)
		ctx.TEMP = value;
	else
		ctx.CURRENT = value;

	return ctx;
}

float4 main(const PS_INPUT input) : COLOR {

	TexCoords = input.iT;

	// Each stage is passed and returns
	// a set of texture arguments
	// And will usually update the CURRENT value
	TextureArgs ctx;

	// The CURRENT register
	// Default to the diffuse value
	// TODO determine whether to use the front or back colours
	// and set them here
	ctx.CURRENT = input.iD0;
	ctx.DIFFUSE = input.iD0;
	ctx.SPECULAR = input.iD1;
	// The TEMP register
	// Default to 0
	ctx.TEMP = float4(0, 0, 0, 0);
	ctx.TFACTOR = state.TextureFactor;

	PsTextureHardcodedState stages[4];
	stages[0].COLOROP = X_D3DTOP_DISABLE;
	stages[1].COLOROP = X_D3DTOP_DISABLE;
	stages[2].COLOROP = X_D3DTOP_DISABLE;
	stages[3].COLOROP = X_D3DTOP_DISABLE;

	// Define stages
	// https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dtexturestagestatetype
	// We'll find comment below and insert the definitions after it
	// STAGE DEFINITIONS
	// END STAGE DEFINITIONS

	// Run each stage
	int previousOp = -1;
	for (int i = 0; i < 4; i++) {

		ctx = ExecuteTextureStage(
			i,
			ctx,
			stages[i],
			previousOp
		);

		previousOp = stages[i].COLOROP;
	}

	// Add fog if enabled
	if (state.FogEnable) {
		ctx.CURRENT.rgb = lerp(state.FogColor.rgb, ctx.CURRENT.rgb, saturate(input.iFog));
	}

	// Add specular if enabled
	if (state.SpecularEnable) {
		ctx.CURRENT.rgb += ctx.SPECULAR.rgb;
	}

	// Output whatever is in current at the end
	return ctx.CURRENT;
}
