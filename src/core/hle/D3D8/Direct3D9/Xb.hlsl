// This starts the raw string (comment to get syntax highlighting, UNCOMMENT to compile) :
R"DELIMITER(
// Xbox HLSL vertex shader (template populated at runtime)
struct VS_INPUT
{
	float4 v[16] : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 oPos : POSITION;
	float4 oD0  : COLOR0; // Colour 0
	float4 oD1  : COLOR1; // Colour 1
	float  oFog : FOG;
	float  oPts : PSIZE;
	float4 oB0  : TEXCOORD4; // Backface Colour 0
	float4 oB1  : TEXCOORD5; // Backface Colour 1
	float4 oT0  : TEXCOORD0; // Texture Coord 0
	float4 oT1  : TEXCOORD1; // Texture Coord 1
	float4 oT2  : TEXCOORD2; // Texture Coord 2
	float4 oT3  : TEXCOORD3; // Texture Coord 3
};

// Xbox constant registers
extern uniform float4 c[192] : register(c0);

// Functions for MAC ('Multiply And Accumulate') opcodes

float4 x_mov(float4 src0)
{
	return src0;
}

float4 x_mul(float4 src0, float4 src1)
{
	return src0 * src1;
}

float4 x_add(float4 src0, float4 src1)
{
	return src0 + src1;
}

float4 x_dst(float4 src0, float4 src1)
{
	return dst(src0, src1);
}

float4 x_min(float4 src0, float4 src1)
{
	return min(src0, src1);
}

float4 x_max(float4 src0, float4 src1)
{
	return max(src0, src1);
}

float4 x_mad(float4 src0, float4 src1, float4 src2)
{
	return (src0 * src1) + src2;
}

int x_arl(float src0)
{
	// The address register should be floored
	// Due to rounding differences with the Xbox (and increased precision on PC?)
	// some titles produce values just below the threshold of the next integer.
	// We can add a small bias to make sure it's bumped over the threshold
	// Test Case: Azurik (divides indexes 755, then scales them back in the vertex shader)
	return floor(src0 + 0.0001);
}

float x_dp3(float4 src0, float4 src1)
{
	return dot(src0.xyz, src1.xyz);
}

float x_dph(float4 src0, float4 src1)
{
	return x_dp3(src0, src1) + src1.w;
}

float x_dp4(float4 src0, float4 src1)
{
	return dot(src0, src1);
}

float4 x_sge(float4 src0, float4 src1)
{
	float4 dest;
	dest.x = (src0.x >= src1.x) ? 1 : 0;
	dest.y = (src0.y >= src1.y) ? 1 : 0;
	dest.z = (src0.z >= src1.z) ? 1 : 0;
	dest.w = (src0.w >= src1.w) ? 1 : 0;
	return dest;
}

float4 x_slt(float4 src0, float4 src1)
{
	float4 dest;
	dest.x = (src0.x < src1.x) ? 1 : 0;
	dest.y = (src0.y < src1.y) ? 1 : 0;
	dest.z = (src0.z < src1.z) ? 1 : 0;
	dest.w = (src0.w < src1.w) ? 1 : 0;
	return dest;
}

// Xbox ILU Functions

float scalar_component(float4 src0)
{
	return src0.w; // use w component by default
}

float x_rcp(float4 src0)
{
	return 1 / scalar_component(src0);
}

float x_rcc(float4 src0)
{
	float input = scalar_component(src0);

	// Calculate the reciprocal
	float r = 1 / input;

	// Clamp
	return (r > 0)
		? clamp(r, 5.42101e-020f, 1.84467e+019f)
		: clamp(r, -1.84467e+019f, -5.42101e-020f);
}

float x_rsq(float4 src0)
{
	return rsqrt(scalar_component(src0));
}

float4 x_exp(float4 src0)
{
	float input = scalar_component(src0);
	float x = exp2(floor(input));
	float fractional = frac(input);
	float power = exp2(input);
	return float4(x, fractional, power, 1);
}

float4 x_log(float4 src0)
{
	float input = scalar_component(src0);
	float exponent = floor(log2(input));
	float mantissa = 1 / exp2(exponent);
	float logResult = log2(input);
	return float4(exponent, mantissa, logResult, 1);
}

float4 x_lit(float4 src0)
{
	const float epsilon = 1.0f / 256.0f;
	float diffuse = src0.x;
	float blinn = src0.y;
	float specPower = clamp(src0.w, -(128 - epsilon), (128 - epsilon));

	float4 dest;
	dest.x = 1;
	dest.y = max(diffuse, 0);
	dest.z = diffuse > 0 ? pow(2, specPower * log(blinn)) : 0;
	dest.w = 1;

	return dest;
}

float4 reverseScreenspaceTransform(float4 oPos)
{
	// On Xbox, oPos should contain the vertex position in screenspace
	// Conventionally, each Xbox Vertex Shader includes instructions like this
	// mul oPos.xyz, r12, c-38
	// +rcc r1.x, r12.w
	// mad oPos.xyz, r12, r1.x, c-37
	// where c-37 and c-38 are reserved transform values

	// Lets hope c-37 and c-38 contain the conventional values
    oPos.xyz -= (float3)c[-37 + 96]; // reverse offset
	oPos.xyz *= oPos.w; // reverse perspective divide
    oPos.xyz /= (float3)c[-38 + 96]; // reverse scale

	return oPos;
}

VS_OUTPUT main(const VS_INPUT xIn)
{
	// Input registers
	float4 v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15;

	// Temporary registers
	float4 r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11;
	r0 = r1 = r2 = r3 = r4 = r5 = r6 = r7 = r8 = r9 = r10 = r11 = float4(0, 0, 0, 1); // TODO correct?
	#define r12 oPos // oPos and r12 are two ways of accessing the same register on Xbox

	// Address (index) register
	int a0_x;

	// Output variables
	float4 oPos, oD0, oD1, oB0, oB1, oT0, oT1, oT2, oT3;
	oPos = oD0 = oD1 = oB0 = oB1 = oT0 = oT1 = oT2 = oT3 = float4(0, 0, 0, 1); // Pre-initialize w component of outputs to 1
	// Single component outputs
	float4 oFog, oPts; // x is write-only on Xbox. Use float4 as some games use incorrect masks
	oFog = oPts = 0;

	// Initialize input variables
	v0 = xIn.v[0];
	v1 = xIn.v[1];
	v2 = xIn.v[2];
	v3 = xIn.v[3];
	v4 = xIn.v[4];
	v5 = xIn.v[5];
	v6 = xIn.v[6];
	v7 = xIn.v[7];
	v8 = xIn.v[8];
	v9 = xIn.v[9];
	v10 = xIn.v[10];
	v11 = xIn.v[11];
	v12 = xIn.v[12];
	v13 = xIn.v[13];
	v14 = xIn.v[14];
	v15 = xIn.v[15];

	// Xbox shader program
// <Xbox Shader>
	// Copy variables to output struct
	VS_OUTPUT xOut;

	xOut.oPos = reverseScreenspaceTransform(oPos);
	xOut.oD0 = oD0;
	xOut.oD1 = oD1;
	xOut.oFog = oFog;
	xOut.oPts = oPts;
	xOut.oB0 = oB0;
	xOut.oB1 = oB1;
	xOut.oT0 = oT0;
	xOut.oT1 = oT1;
	xOut.oT2 = oT2;
	xOut.oT3 = oT3;

	return xOut;
}

// End of vertex shader )DELIMITER" /* This terminates the raw string" // */
