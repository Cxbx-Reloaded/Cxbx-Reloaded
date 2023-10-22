// This starts the raw string (comment to get syntax highlighting, UNCOMMENT to compile) :
R"DELIMITER(// Xbox HLSL vertex shader (template populated at runtime)

struct VS_INPUT
{
	float4 v[16] : TEXCOORD;
};

// Output registers
struct VS_OUTPUT // Declared identical to pixel shader input (see PS_INPUT)
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

#define X_D3DSCM_CORRECTION                 96 // Add 96 to arrive at the range 0..191 (instead of -96..95)
#define X_D3DVS_CONSTREG_COUNT              192

// Xbox constant registers
uniform float4 C[X_D3DVS_CONSTREG_COUNT] : register(c0);

// Default values for vertex registers, and whether to use them
uniform float4 vRegisterDefaultValues[16]  : register(c192);
uniform float4 vRegisterDefaultFlagsPacked[4]  : register(c208);

uniform float4 xboxScreenspaceScale : register(c212);
uniform float4 xboxScreenspaceOffset : register(c213);

uniform float4 xboxTextureScale[4] : register(c214);

// Parameters for mapping the shader's fog output value to a fog factor
uniform float4  CxbxFogInfo: register(c218); // = CXBX_D3DVS_CONSTREG_FOGINFO

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

    // Like Xbox, out-of-range indices are guaranteed to be zero in HLSL
    // so no need to bounds check negative numbers
    // if (register_number < 0)
    //    return 0;

    // If the index is too large, set it to -1 so 0 is returned
    // Note: returning 0 directly requires many more instructions
    if (register_number >= X_D3DVS_CONSTREG_COUNT) // X_D3DVS_CONSTREG_COUNT
        register_number = -1;

    return C[register_number];
}

// Due to rounding differences with the Xbox (and increased precision on PC?)
// some titles produce values just below the threshold of the next integer.
// We can add a small bias to make sure it's bumped over the threshold
// Test Case: Azurik (divides indexes 755, then scales them back in the vertex shader)
#define BIAS 0.001
// NOTE : Was 0.0001, unlike xqemu

// 2.14.1.11  Vertex Program Floating Point Requirements
// The floor operations used by the ARL and EXP instructions must
// operate identically.  Specifically, the EXP instruction's floor(t.x)
// intermediate result must exactly match the integer stored in the
// address register by the ARL instruction.
float x_floor(float src)
{
	return floor(src + BIAS);
}

// https://xboxdevwiki.net/NV2A/Vertex_Shader
// https://www.khronos.org/registry/OpenGL/extensions/NV/NV_vertex_program.txt
// https://www.khronos.org/registry/OpenGL/extensions/NV/NV_vertex_program1_1.txt

// Functions for MAC ('Multiply And Accumulate') opcodes

// 2.14.1.10.1  ARL: Address Register Load
// The address register should be floored
#define x_arl(dest, mask, src0) dest.mask = x_floor(_tof4(src0).x).mask

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
#define x_dph(dest, mask, src0, src1) dest.mask = _ssss(_dph(_tof4(src0), _tof4(src1))).mask
float _dph(float4 src0, float4 src1)
{
	return dot(src0.xyz, src1.xyz) + src1.w;
}

// Xbox ILU Functions

// 2.14.1.10.7  RSQ: Reciprocal Square Root
#define x_rsq(dest, mask, src0) dest.mask = _ssss(_rsq(_scalar(src0))).mask
float _rsq(float src)
{
	float a = abs(src);
#if 0 // TODO : Enable
	if (a == 1) return 1;
	if (a == 0) return 1.#INF;
#endif
	return rsqrt(a);
}

// 2.14.1.10.15  EXP: Exponential Base 2
#define x_expp(dest, mask, src0) dest.mask = _expp(_scalar(src0)).mask
float4 _expp(float src)
{
    float floor_src = x_floor(src);

    float4 dest;
    dest.x = exp2(floor_src);
    dest.y = src - floor_src;
    dest.z = exp2(src);
    dest.w = 1;

	return dest;
}

// 2.14.1.10.16  LOG: Logarithm Base 2
#define x_logp(dest, mask, src0) dest.mask = _logp(_scalar(src0)).mask
float4 _logp(float src)
{
    float4 dest;
#if 0 // TODO : Enable
	float t = abs(src);
	if (t != 0) {
		if (t == 1.#INF) {
			dest.x = 1.#INF;
			dest.y = 1;
			dest.z = 1.#INF;
		} else {
#endif
			float exponent;
			float mantissa = frexp(src/* + BIAS*/, /*out*/exponent);
			float z = log2(src);
			dest.x = exponent;
			dest.y = mantissa;
			dest.z = z;
#if 0
		}
	} else {
		dest.x = -1.#INF;
		dest.y = 1;
		dest.z = -1.#INF;
	}
#endif
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
	dest.z = (diffuse > 0) && (blinn > 0) ? pow(blinn, specPower) : 0;
	dest.w = 1;

	return dest;
}

// 2.14.1.10.19  RCC: Reciprocal Clamped
#define x_rcc(dest, mask, src0) dest.mask = _ssss(_rcc(_scalar(src0))).mask
float _rcc(float src)
{
	// Calculate the reciprocal
	float r = 1 / src;

	// Clamp
	return (r >= 0)
		? clamp(r,  5.42101e-020f,  1.84467e+019f)  // the IEEE 32-bit binary values 0x1F800000 and 0x5F800000
		: clamp(r, -1.84467e+019f, -5.42101e-020f); // the IEEE 32-bit binary values 0xDF800000 and 0x9F800000
}

// 2.14.1.10.6  RCP: Reciprocal
#define x_rcp(dest, mask, src0) dest.mask = _ssss(_rcp(_scalar(src0))).mask
float _rcp(float src)
{
	// OpenGL/NVidia extension definition
#if 0 // TODO : Enable?
	if (src == 1) return 1;
	if (src == 0) return 1.#INF;
	return 1 / src;
#endif
	// Forward to Xbox clamped reciprocal
	// So we have defined behaviour with rcp(0)
	// This prevents issues with XYZRHW modes
	// where the w component may be 0
	return _rcc(src);
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

	// oPos.w and xboxViewportScale.z might be VERY big when a D24 depth buffer is used
	// and multiplying oPos.xyz by oPos.w may cause precision issues.
	// Test case: Burnout 3

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
	// Output variables
	float4 oPos, oD0, oD1, oB0, oB1, oT0, oT1, oT2, oT3;
	oPos = oD0 = oD1 = oB0 = oB1 = oT0 = oT1 = oT2 = oT3 = float4(0, 0, 0, 1); // Pre-initialize w component of outputs to 1

	// Single component outputs
	float4 oFog, oPts; // x is write-only on Xbox. Use float4 as some games use incorrect masks
	oFog = 1; // Default to no fog. Test case: Lego Star Wars II
	oPts = 0;

	// Address (index) register
	int1 a0 = 0;

	// Temporary registers
	float4 r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11;
	r0 = r1 = r2 = r3 = r4 = r5 = r6 = r7 = r8 = r9 = r10 = r11 = float4(0, 0, 0, 0);
	#define r12 oPos // oPos and r12 are two ways of accessing the same register on Xbox

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

	// Temp variable for paired VS instruction
	float4 temp;

	// Xbox shader program)DELIMITER", /* This terminates the header raw string" // */

R"DELIMITER(

	// Copy variables to output struct
    VS_OUTPUT xOut;

	// Fogging
	// TODO deduplicate
    float fogDepth            =   oFog.x;
	  if(oFog.x <= 0)fogDepth =  -oFog.x;	// Don't abs this value! Test-case : DolphinClassic xdk sample
	const float fogTableMode  =   CxbxFogInfo.x;
	const float fogDensity    =   CxbxFogInfo.y;
	const float fogStart      =   CxbxFogInfo.z;
	const float fogEnd        =   CxbxFogInfo.w;  

	const float FOG_TABLE_NONE    = 0;
	const float FOG_TABLE_EXP     = 1;
	const float FOG_TABLE_EXP2    = 2;
	const float FOG_TABLE_LINEAR  = 3;
 
    float fogFactor;
    if(fogTableMode == FOG_TABLE_NONE) 
       fogFactor = fogDepth;
    if(fogTableMode == FOG_TABLE_EXP) 
       fogFactor = 1 / exp(fogDepth * fogDensity); /* / 1 / e^(d * density)*/
    if(fogTableMode == FOG_TABLE_EXP2) 
       fogFactor = 1 / exp(pow(fogDepth * fogDensity, 2)); /* / 1 / e^((d * density)^2)*/
    if(fogTableMode == FOG_TABLE_LINEAR) 
       fogFactor = (fogEnd - fogDepth) / (fogEnd - fogStart);
       
	xOut.oPos = reverseScreenspaceTransform(oPos);
	xOut.oD0 = saturate(oD0);
	xOut.oD1 = saturate(oD1);
	xOut.oFog = fogFactor; // Note : Xbox clamps fog in pixel shader -> *NEEDS TESTING* /was oFog.x 
	xOut.oPts = oPts.x;
	xOut.oB0 = saturate(oB0);
	xOut.oB1 = saturate(oB1);
	// Scale textures (TODO : or should we apply this to the input register values?)
	xOut.oT0 = oT0 / xboxTextureScale[0];
	xOut.oT1 = oT1 / xboxTextureScale[1];
	xOut.oT2 = oT2 / xboxTextureScale[2];
	xOut.oT3 = oT3 / xboxTextureScale[3];

	return xOut;
}

// End of vertex shader footer)DELIMITER" /* This terminates the footer raw string" // */
