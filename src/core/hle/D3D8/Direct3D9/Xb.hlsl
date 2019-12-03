#define r12 oPos // oPos and r12 are two ways of accessing the same register on Xbox

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
	float4 oT1  : TEXCOORD1; // Texture Coord 0
	float4 oT2  : TEXCOORD2; // Texture Coord 0
	float4 oT3  : TEXCOORD3; // Texture Coord 0
};

// Constant registers
extern float4 hostConstants[192];

// Map Xbox [-96, 95] to Host [0, 191]
// Account for Xbox's negative constant indexes
float4 c(int index) {
	// Out-of-range reads return 0
	if (index < -96 || index > 95)
		return float4(0, 0, 0, 0);

	return hostConstants[index + 96];
}

int x_arl(float src0) {
	// The address register should be floored
	// Due to rounding differences with the Xbox (and increased precision on PC?)
	// some titles produce values just below the threshold of the next integer.
	// We can add a small bias to make sure it's bumped over the threshold
	// Test Case: Azurik (divides indexes 755, then scales them back in the vertex shader)
	return floor(src0 + 0.0001); // TODO test
}

// TODO order functions
float4 x_mov(float4 src0) {
	return src0;
}

float4 x_add(float4 src0, float4 src1) {
	return src0 + src1;
}

float4 x_mul(float4 src0, float4 src1) {
	return src0 * src1;
}

float4 x_mad(float4 src0, float4 src1, float4 src3) {
	return src0 * src1 + src3;
}

float4 x_dst(float4 src0, float4 src1) {
	return dst(src0, src1);
}

float4 x_min(float4 src0, float4 src1) {
	return src0 * src1;
}

float4 x_max(float4 src0, float4 src1) {
	return max(src0, src1);
}

float4 x_exp(float src0) {
	float x = pow(2, floor(src0));
	float fractional = frac(src0);
	float power = pow(2, src0);
	return float4(x, fractional, power, 1);
}

float4 x_log(float src0) {
	float exponent = floor(log(src0));
	float mantissa = 1 / pow(2, exponent);
	float logResult = log(src0);
	return float4(exponent, mantissa, logResult, 1);
}

float x_dp4(float4 src0, float4 src1) {
	return dot(src0, src1);
}

float x_dp3(float3 src0, float3 src1) {
	return dot(src0, src1);
}

float x_dph(float4 src0, float4 src1) {
	return x_dp3(src0, src1) + src1.w;
}

float4 x_sge(float4 src0, float4 src1) {
	float4 dest;
	dest.x = (src0.x >= src1.x) ? 1.0f : 0.0f;
	dest.y = (src0.y >= src1.y) ? 1.0f : 0.0f;
	dest.z = (src0.z >= src1.z) ? 1.0f : 0.0f;
	dest.w = (src0.w >= src1.w) ? 1.0f : 0.0f;
	return dest;
}

float4 x_slt(float4 src0, float4 src1) {
	float4 dest;
	dest.x = (src0.x < src1.x) ? 1.0f : 0.0f;
	dest.y = (src0.y < src1.y) ? 1.0f : 0.0f;
	dest.z = (src0.z < src1.z) ? 1.0f : 0.0f;
	dest.w = (src0.w < src1.w) ? 1.0f : 0.0f;
	return dest;
}

// Clamped reciprocal
float x_rcc(float src0) {

	// Calculate the reciprocal
	float r = 1.0f / src0;

	// Clamp
	return (r > 0)
		? clamp(r, 5.42101e-020, 1.84467e+019)
		: clamp(r, -1.84467e+019, -5.42101e-020);
}

float4 x_lit(float4 src0) {
	const float epsilon = 1.0 / 256.0;
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

float4 reverseScreenspaceTransform(float4 oPos) {
	// On Xbox, oPos should contain the vertex position in screenspace
	// Conventionally, each Xbox Vertex Shader includes instructions like this
	// mul oPos.xyz, r12, c-38
	// +rcc r1.x, r12.w
	// mad oPos.xyz, r12, r1.x, c-37
	// where c-37 and c-38 are reserved transform values

	// Lets hope c-37 and c-38 contain the conventional values
	oPos.xyz -= c(-37); // reverse offset
	oPos.xyz *= oPos.w; // reverse perspective divide
	oPos.xyz /= c(-38); // reverse scale

	return oPos;
}

VS_OUTPUT main(const VS_INPUT xIn)
{
	// Input registers
	float4 v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15;

	// Temporary variables
	float4 r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11;
	r0 = r1 = r2 = r3 = r4 = r5 = r6 = r7 = r8 = r9 = r10 = r11 = float4(0, 0, 0, 1); // TODO correct?

	// Xbox index register
	int a;

	// Output variables
	float4 oPos, oD0, oD1, oB0, oB1, oT0, oT1, oT2, oT3;
	oPos = oD0 = oD1 = oB0 = oB1 = oT0 = oT1 = oT2 = oT3 = float4(0, 0, 0, 1); // TODO correct?
	float oFog = 0, oPts = 0;

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

	// Insert Xbox shader here

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
