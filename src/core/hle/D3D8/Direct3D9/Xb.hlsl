// This starts the raw string (comment to get syntax highlighting, UNCOMMENT to compile) :
R"DELIMITER(
// Xbox HLSL vertex shader (template populated at runtime)
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
	float  oPts : PSIZE;	 // Point size
	float4 oB0  : TEXCOORD4; // Back-facing primary color
	float4 oB1  : TEXCOORD5; // Back-facing secondary color
	float4 oT0  : TEXCOORD0; // Texture coordinate set 0
	float4 oT1  : TEXCOORD1; // Texture coordinate set 1
	float4 oT2  : TEXCOORD2; // Texture coordinate set 2
	float4 oT3  : TEXCOORD3; // Texture coordinate set 3
};

#define X_D3DSCM_CORRECTION                 96 // Add 96 to arrive at the range 0..191 (instead of -96..95)
#define X_D3DVS_CONSTREG_COUNT              192

// Xbox constant registers
uniform float4 C[X_D3DVS_CONSTREG_COUNT] : register(c0);

// Vertex input overrides for SetVertexData4f support
uniform float4 vOverrideValue[16]  : register(c192);
uniform float4 vOverridePacked[4]  : register(c208);

uniform float4 xboxViewportScale   : register(c212);
uniform float4 xboxViewportOffset  : register(c213);

// Overloaded casts, assuring all inputs are treated as float4
float4 _tof4(float  src) { return float4(src, src, src, src); }
float4 _tof4(float2 src) { return src.xyyy; }
float4 _tof4(float3 src) { return src.xyzz; }
float4 _tof4(float4 src) { return src; }
float4 _ssss(float s)    { return float4(s, s, s, s); } // a scalar output replicated across a 4-component vector
#define _scalar(src) _tof4(src).x /* a scalar input */

float4 c(int register_number)
{
	// Map Xbox [-96, 95] to Host [0, 191]
	// Account for Xbox's negative constant indexes
    register_number += X_D3DSCM_CORRECTION;
    if (register_number < 0)
        return 0;
    
    if (register_number >= X_D3DVS_CONSTREG_COUNT) // X_D3DVS_CONSTREG_COUNT
        return 0;

    return C[register_number];
}

// http://xboxdevwiki.net/NV2A/Vertex_Shader
// https://www.khronos.org/registry/OpenGL/extensions/NV/NV_vertex_program.txt
// https://www.khronos.org/registry/OpenGL/extensions/NV/NV_vertex_program1_1.txt

// Functions for MAC ('Multiply And Accumulate') opcodes

// 2.14.1.10.1  ARL: Address Register Load
// The address register should be floored
// Due to rounding differences with the Xbox (and increased precision on PC?)
// some titles produce values just below the threshold of the next integer.
// We can add a small bias to make sure it's bumped over the threshold
// Test Case: Azurik (divides indexes 755, then scales them back in the vertex shader)
#define x_arl(dest, mask, src0) dest.mask = floor(_tof4(src0).x + 0.0001).mask

// 2.14.1.10.2  MOV: Move
#define x_mov(dest, mask, src0) dest.mask = (_tof4(src0)).mask

// 2.14.1.10.3  MUL: Multiply
#define x_mul(dest, mask, src0, src1) dest.mask = (_tof4(src0) * _tof4(src1)).mask

// 2.14.1.10.4  ADD: Add
#define x_add(dest, mask, src0, src1) dest.mask = (_tof4(src0) + _tof4(src1)).mask

// 2.14.1.10.5  MAD: Multiply and Add
#define x_mad(dest, mask, src0, src1, src2) dest.mask = (_tof4(src0) * _tof4(src1) + _tof4(src2)).mask

// 2.14.1.10.8  DP3: Three-Component Dot Product
#define x_dp3(dest, mask, src0, src1) dest.mask = _ssss(dot(_tof4(src0).xyz, _tof4(src1).xyz)).mask

//  2.14.1.10.9  DP4: Four-Component Dot Product
#define x_dp4(dest, mask, src0, src1) dest.mask = _ssss(dot(_tof4(src0), _tof4(src1))).mask

// 2.14.1.10.10  DST: Distance Vector
#define x_dst(dest, mask, src0, src1) dest.mask = dst(_tof4(src0), _tof4(src1)).mask /* equals { dest.x = 1; dest.y = src0.y * src1.y; dest.z = src0.z; dest.w = src1.w; } */

// 2.14.1.10.11  MIN: Minimum
#define x_min(dest, mask, src0, src1) dest.mask = min(_tof4(src0), _tof4(src1)).mask

// 2.14.1.10.12  MAX: Maximum
#define x_max(dest, mask, src0, src1) dest.mask = max(_tof4(src0), _tof4(src1)).mask

// 2.14.1.10.13  SLT: Set On Less Than
#define x_slt(dest, mask, src0, src1) dest.mask = _slt(_tof4(src0), _tof4(src1)).mask
float4 _slt(float4 src0, float4 src1)
{
	float4 dest;
	dest.x = (src0.x < src1.x) ? 1 : 0;
	dest.y = (src0.y < src1.y) ? 1 : 0;
	dest.z = (src0.z < src1.z) ? 1 : 0;
	dest.w = (src0.w < src1.w) ? 1 : 0;
	return dest;
}

// 2.14.1.10.14  SGE: Set On Greater or Equal Than
#define x_sge(dest, mask, src0, src1) dest.mask = _sge(_tof4(src0), _tof4(src1)).mask
float4 _sge(float4 src0, float4 src1)
{
	float4 dest;
	dest.x = (src0.x >= src1.x) ? 1 : 0;
	dest.y = (src0.y >= src1.y) ? 1 : 0;
	dest.z = (src0.z >= src1.z) ? 1 : 0;
	dest.w = (src0.w >= src1.w) ? 1 : 0;
	return dest;
}

// 2.14.1.10.18  DPH: Homogeneous Dot Product
#define x_dph(dest, mask, src0, src1) dest.mask = _ssss(dot(float4(_tof4(src0).xyz, 1), _tof4(src1)) + src1.w).mask

// Xbox ILU Functions

// 2.14.1.10.6  RCP: Reciprocal
#define x_rcp(dest, mask, src0) dest.mask = _ssss(1 / _scalar(src0)).mask
// TODO : #define x_rcp(dest, mask, src0) dest.mask = (_scalar(src0) == 0) ? 1.#INF : (1 / _scalar(src0))

// 2.14.1.10.7  RSQ: Reciprocal Square Root
#define x_rsq(dest, mask, src0) dest.mask = _ssss(rsqrt(abs(_scalar(src0)))).mask

// 2.14.1.10.15  EXP: Exponential Base 2
#define x_expp(dest, mask, src0) dest.mask = _expp(_scalar(src0)).mask
float4 _expp(float input)
{
    float base = floor(input);

    float4 dest;
    dest.x = exp2(base);
    dest.y = input - base; // Was : frac(input)
    dest.z = exp2(input);
    dest.w = 1;

	return dest;
}

// 2.14.1.10.16  LOG: Logarithm Base 2
#define x_logp(dest, mask, src0) dest.mask = _logp(_scalar(src0)).mask
float4 _logp(float input)
{
	float exponent = floor(log2(input));

    float4 dest;
    dest.x = exponent;
    dest.y = 1 / exp2(exponent); // mantissa
    dest.z = exponent + log2(input); // logResult
    dest.w = 1;
    
	return dest;
}

// 2.14.1.10.17  LIT: Light Coefficients
#define x_lit(dest, mask, src) dest.mask = _lit(_tof4(src)).mask
float4 _lit(float4 src0)
{
	const float epsilon = 1.0f / 256.0f;

	float diffuse = src0.x;
	float blinn = src0.y;
	float specPower = clamp(src0.w, -(128 - epsilon), (128 - epsilon));

	float4 dest;
	dest.x = 1;
	dest.y = max(0, diffuse);
	dest.z = diffuse > 0 ? exp2(specPower * log(blinn)) : 0;
	// TODO : Use dest.z = (diffuse > 0) && (blinn > 0) ? pow(blinn, specPower) : 0;
	dest.w = 1;

	return dest;
}

// 2.14.1.10.19  RCC: Reciprocal Clamped
#define x_rcc(dest, mask, src0) dest.mask = _ssss(_rcc(_scalar(src0))).mask
float _rcc(float input)
{
	// Calculate the reciprocal
	float r = 1 / input;

	// Clamp
	return (r >= 0)
		? clamp(r,  5.42101e-020f,  1.84467e+019f)  // the IEEE 32-bit binary values 0x1F800000 and 0x5F800000
		: clamp(r, -1.84467e+019f, -5.42101e-020f); // the IEEE 32-bit binary values 0xDF800000 and 0x9F800000
}

float4 reverseScreenspaceTransform(float4 oPos)
{
	// On Xbox, oPos should contain the vertex position in screenspace
	// We need to reverse this transformation
	// Conventionally, each Xbox Vertex Shader includes instructions like this
	// mul oPos.xyz, r12, c-38
	// +rcc r1.x, r12.w
	// mad oPos.xyz, r12, r1.x, c-37
	// where c-37 and c-38 are reserved transform values

	oPos.xyz -= xboxViewportOffset.xyz; // reverse offset
	oPos.xyz *= oPos.w; // reverse perspective divide
	oPos.xyz /= xboxViewportScale.xyz; // reverse scale

	return oPos;
}

VS_OUTPUT main(const VS_INPUT xIn)
{
	// Output variables
	float4 oPos, oD0, oD1, oB0, oB1, oT0, oT1, oT2, oT3;
	oPos = oD0 = oD1 = oB0 = oB1 = oT0 = oT1 = oT2 = oT3 = float4(0, 0, 0, 1); // Pre-initialize w component of outputs to 1

	// Single component outputs
	float4 oFog, oPts; // x is write-only on Xbox. Use float4 as some games use incorrect masks
	oFog = oPts = 0;

	// Address (index) register
	int1 a0 = 0;

	// Temporary registers
	float4 r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11;
	r0 = r1 = r2 = r3 = r4 = r5 = r6 = r7 = r8 = r9 = r10 = r11 = float4(0, 0, 0, 0);
	#define r12 oPos // oPos and r12 are two ways of accessing the same register on Xbox

	// Input registerss
	float4 v[16];
	# define v0  v[0]
	# define v1  v[1]
	# define v2  v[2]
	# define v3  v[3]
	# define v4  v[4]
	# define v5  v[5]
	# define v6  v[6]
	# define v7  v[7]
	# define v8  v[8]
	# define v9  v[9]
	# define v10 v[10]
	# define v11 v[11]
	# define v12 v[12]
	# define v13 v[13]
	# define v14 v[14]
	# define v15 v[15]

	// View 4 packed overrides as an array of 16 floats
	float vOverride[16] = (float[16])vOverridePacked;

	// Initialize input registers from the vertex buffer
	// Or use an override value set with SetVertexData4f
	for(int i = 0; i < 16; i++){
		v[i] = vOverride[i] ? vOverrideValue[i] : xIn.v[i];
	}

	// Xbox shader program
// <Xbox Shader>

	// Copy variables to output struct
	VS_OUTPUT xOut;

	xOut.oPos = reverseScreenspaceTransform(oPos);
	xOut.oD0 = oD0;
	xOut.oD1 = oD1;
	xOut.oFog = oFog.x;
	xOut.oPts = oPts.x;
	xOut.oB0 = oB0;
	xOut.oB1 = oB1;
	xOut.oT0 = oT0;
	xOut.oT1 = oT1;
	xOut.oT2 = oT2;
	xOut.oT3 = oT3;

	return xOut;
}

// End of vertex shader )DELIMITER" /* This terminates the raw string" // */
