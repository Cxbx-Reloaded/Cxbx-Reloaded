#include "FixedFunctionPixelShader.hlsli"

#if 1 // TODO : Move these (and other) helper functions to a (potentially pre-compiled) hlsl(i) file, to be shared with CxbxPixelShaderTemplate.hlsl

static const float4 WarningColor = float4(0, 1, 1, 1); // Returned when unhandled scenario is encountered

#define unsigned_to_signed(x) (((x) * 2) - 1) // Shifts range from [0..1] to [-1..1] (just like s_bx2)
#define signed_to_unsigned(x) (((x) + 1) / 2) // Shifts range from [-1..1] to [0..1]

float4 PerformColorSign(const float4 ColorSign, float4 t)
{
	// Per color channel, based on the ColorSign setting :
	// either keep the value range as-is (when ColorSign is zero)
	// or convert from [0..1] to [-1..+1] (when ColorSign is more than zero, often used for bumpmaps),
	// or convert from [-1..1] to [0..1] (when ColorSign is less than zero):
	if (ColorSign.r > 0) t.r = unsigned_to_signed(t.r);
	if (ColorSign.g > 0) t.g = unsigned_to_signed(t.g);
	if (ColorSign.b > 0) t.b = unsigned_to_signed(t.b);
	if (ColorSign.a > 0) t.a = unsigned_to_signed(t.a);
	
	// TODO : Instead of the above, create a mirror texture with a host format that has identical component layout, but with all components signed.
	// Then, in here, when any component has to be read as signed, sample the signed texture (ouch : with what dimension and coordinate?!)
	// and replace the components that we read from the unsigned texture, but which have to be signed, with the signed components read from the signed mirror texture.
	// This way, texture filtering can still be allowed, as that would be performed separately over the unsigned vs unsigned textures (so no mixing between the two).

	return t;
}

float4 PerformColorKeyOp(const float ColorKeyOp, const float4 ColorKeyColor, float4 t)
{
	// Handle all D3DTCOLORKEYOP_ modes :
	if (ColorKeyOp == 0) // = _DISABLE
		return t; // No color-key checking

	if (any(t - ColorKeyColor))
		return t; // Cxbx assumption : On color mismatch, simply return the input. TODO : This might require a more elaborate operation? (Like "when any of the texels were filtered with a non-zero weight", whatever that means)

	if (ColorKeyOp == 1) // = _ALPHA
		return float4(t.rgb, 0);

	if (ColorKeyOp == 2) // = _RGBA
		return 0;

#ifdef ENABLE_FF_ALPHAKILL
	if (ColorKeyOp == 3) // = _KILL
		discard; // When compiled into FixedFunctionPixelShader.hlsl, compile errors arise

#endif
	// Undefined ColorKeyOp mode
	return WarningColor;
}

void PerformAlphaKill(const float AlphaKill, float4 t)
{
#ifdef ENABLE_FF_ALPHAKILL
	if (AlphaKill)
		if (t.a == 0)
			discard; // When compiled into FixedFunctionPixelShader.hlsl, compile errors arise
#endif
}
#endif

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

	// Note : When we use separate "if"'s here instead of below "else if"'s,
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
	else if (op == X_D3DTOP_DOTPRODUCT3) {
		// Test case: PerPixelLighting
		arg1.rgb = (arg1.rgb*2) -1; // TODO : These bias and scale operations should not be performed here, but when the input-texture is sampled (same as for X_D3DTOP_BUMPENVMAP*)
		arg2.rgb = (arg2.rgb*2)-1;
		return saturate(dot(arg1.rgb, arg2.rgb));
	}
	// Note arg0 below is arg1 in D3D docs
	// since it becomes the first argument for operations supporting 3 arguments...
	else if (op == X_D3DTOP_MULTIPLYADD)
		return arg0 + arg1 * arg2;
	else if (op == X_D3DTOP_LERP)
		return arg0 * arg1 + (1 - arg0) * arg2;
	else if (op >= X_D3DTOP_BUMPENVMAP) { // Also handles X_D3DTOP_BUMPENVMAPLUMINANCE
		arg1 = PerformColorSign(stage.COLORSIGN,ctx.CURRENT); // TODO : Verify bump mapping indeed uses previous pixel value (CURRENT, or possibly TEXTURE, but not COLORARG1)
		//arg1.xy = (arg1.xy - 0.5) * 2; // TODO : This bias and scale operation should not be performed here, but when the input-texture is sampled (same as for X_D3DTOP_DOTPRODUCT3)
		// Note : default component order .xyzw is identical to .rgba, and z (red) should not be scaled here, as it's used for luminance which is an unsigned input.
		return float4(
			arg1.x * stage.BUMPENVMAT00 + arg1.y * stage.BUMPENVMAT10,
			arg1.x * stage.BUMPENVMAT01 + arg1.y * stage.BUMPENVMAT11,
			(op == X_D3DTOP_BUMPENVMAPLUMINANCE) ? arg1.z * stage.BUMPENVLSCALE + stage.BUMPENVLOFFSET : 1,
			1);
	}
	// Something is amiss... we should have returned by now!
	return WarningColor;
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

	// Fetch the texture coordinates for this stage
	float3 TexCoord = TexCoords[i].xyz;

	// Bump environment mapping special case
	if (previousOp >= X_D3DTOP_BUMPENVMAP) { // Also handles X_D3DTOP_BUMPENVMAPLUMINANCE
		// Assume U, V, L is in CURRENT
		// Add U', V', to the texture coordinates
		// https://docs.microsoft.com/en-us/windows/win32/direct3d9/bump-mapping-formulas
		TexCoord.xy += ctx.CURRENT.xy;
	}

	// Sample the texture
	float4 t;
	int type = TextureSampleType[i];
	if (type == SAMPLE_NONE)
		t = 1; // Test case JSRF
	else if (type == SAMPLE_2D)
		t = tex2D(samplers[i], TexCoord.xy);
	else if (type == SAMPLE_3D)
		t = tex3D(samplers[i], TexCoord.xyz);
	else if (type == SAMPLE_CUBE)
		t = texCUBE(samplers[i], TexCoord.xyz);

	

	// TODO : Figure out in which order the following operations should be performed :
	//t = PerformColorSign(stage.COLORSIGN, t);
	//t = PerformColorKeyOp(stage.COLORKEYOP, stage.COLORKEYCOLOR, t);
	PerformAlphaKill(stage.ALPHAKILL, t);

	// Assign the final value for TEXTURE
	ctx.TEXTURE = t;

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
		ctx.TEMP = PerformColorKeyOp(stage.COLORKEYOP, stage.COLORKEYCOLOR, value);
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
