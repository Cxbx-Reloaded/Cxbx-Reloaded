// This starts the raw string (comment to get syntax highlighting, UNCOMMENT to compile) :
R"DELIMITER(

struct PS_INPUT // Declared identical to vertex shader output (see VS_OUTPUT)
{
	float2 iPos : VPOS;   // Screen space x,y pixel location
	float4 iD0  : COLOR0; // Front-facing primary (diffuse) vertex color (clamped to 0..1)
	float4 iD1  : COLOR1; // Front-facing secondary (specular) vertex color (clamped to 0..1)
	float  iFog : FOG;
	float  iPts : PSIZE;
	float4 iB0  : TEXCOORD4; // Back-facing primary (diffuse) vertex color (clamped to 0..1)
	float4 iB1  : TEXCOORD5; // Back-facing secondary (specular) vertex color (clamped to 0..1)
	float4 iT0  : TEXCOORD0; // Texture Coord 0
	float4 iT1  : TEXCOORD1; // Texture Coord 1
	float4 iT2  : TEXCOORD2; // Texture Coord 2
	float4 iT3  : TEXCOORD3; // Texture Coord 3
	float  iFF  : VFACE; // Front facing if > 0
};

struct PS_OUTPUT
{
	float4 oR0 : COLOR;
};

// Source register modifier macro's, based on enum PS_INPUTMAPPING :
// TODO : Should all these 'max(0, x)' actually be 'saturate(x)'? This, because the operation may actually clamp the register value to the range [0..1]
#define s_sat(x)         saturate(x)        // PS_INPUTMAPPING_UNSIGNED_IDENTITY= 0x00L, // OK for final combiner      // Clamps negative x to 0 // Was :        max(0, x)
#define s_comp(x)   (1 - saturate(x))       // PS_INPUTMAPPING_UNSIGNED_INVERT=   0x20L, // OK for final combiner      // Complements x (1-x)    // Was : 1- min(max(0, x), 1)
#define s_bx2(x)    (( 2 * max(0, x)) - 1)  // PS_INPUTMAPPING_EXPAND_NORMAL=     0x40L, // invalid for final combiner // Shifts range from [0..1] to [-1..1]
#define s_negbx2(x) ((-2 * max(0, x)) + 1)  // PS_INPUTMAPPING_EXPAND_NEGATE=     0x60L, // invalid for final combiner // Shifts range from [0..1] to [-1..1] and then negates
#define s_bias(x)         (max(0, x) - 0.5) // PS_INPUTMAPPING_HALFBIAS_NORMAL=   0x80L, // invalid for final combiner // Clamps negative x to 0 and then subtracts 0.5
#define s_negbias(x)     (-max(0, x) + 0.5) // PS_INPUTMAPPING_HALFBIAS_NEGATE=   0xa0L, // invalid for final combiner // Clamps negative x to 0, subtracts 0.5, and then negates
#define s_ident(x)                x         // PS_INPUTMAPPING_SIGNED_IDENTITY=   0xc0L, // invalid for final combiner // No modifier, x is passed without alteration
#define s_neg(x)                (-x)        // PS_INPUTMAPPING_SIGNED_NEGATE=     0xe0L, // invalid for final combiner // Negate

// Destination register modifier macro's, based on enum PS_COMBINEROUTPUT :
#define d_ident(x) x              // PS_COMBINEROUTPUT_IDENTITY=            0x00L, // 
#define d_bias(x) (x - 0.5)       // PS_COMBINEROUTPUT_BIAS=                0x08L, // Subtracts 0.5 from outputs
#define d_x2(x)  ( x        * 2)  // PS_COMBINEROUTPUT_SHIFTLEFT_1=         0x10L, // Scales outputs by 2
#define d_bx2(x) ((x - 0.5) * 2)  // PS_COMBINEROUTPUT_SHIFTLEFT_1_BIAS=    0x18L, // Subtracts 0.5 from outputs and scales by 2
#define d_x4(x)  ( x        * 4)  // PS_COMBINEROUTPUT_SHIFTLEFT_2=         0x20L, // Scales outputs by 4
#define d_bx4(x) ((x - 0.5) * 4)  // PS_COMBINEROUTPUT_SHIFTLEFT_2_BIAS=    0x28L, // Subtracts 0.5 from outputs and scales by 4
#define d_d2(x)  ( x        / 2)  // PS_COMBINEROUTPUT_SHIFTRIGHT_1=        0x30L, // Divides outputs by 2
#define d_bd2(x) ((x - 0.5) / 2)  // PS_COMBINEROUTPUT_SHIFTRIGHT_1_BIAS=   0x38L, // Subtracts 0.5 from outputs and divides by 2

// Constant registers
uniform const float4 c0_[8] : register(c0);
uniform const float4 c1_[8] : register(c8);
uniform const float4 c_fog : register(c18); // Note : Maps to PSH_XBOX_CONSTANT_FOG, assigned to fog.rgb

// Constant registers used only in final combiner stage (xfc 'opcode') :
uniform const float4 FC0 : register(c16); // Note : Maps to PSH_XBOX_CONSTANT_FC0, must be generated as argument to xfc instead of C0
uniform const float4 FC1 : register(c17); // Note : Maps to PSH_XBOX_CONSTANT_FC1, must be generated as argument to xfc instead of C1
uniform const float4 BEM[4] : register(c19); // Note : PSH_XBOX_CONSTANT_BEM for 4 texture stages
uniform const float4 LUM[4] : register(c23); // Note : PSH_XBOX_CONSTANT_LUM for 4 texture stages
uniform const float  FRONTFACE_FACTOR : register(c27); // Note : PSH_XBOX_CONSTANT_LUM for 4 texture stages


#define CM_LT(c) if(c < 0) clip(-1); // = PS_COMPAREMODE_[RSTQ]_LT
#define CM_GE(c) if(c >= 0) clip(-1); // = PS_COMPAREMODE_[RSTQ]_GE

#if 0
   // Compiler-defines/symbols which must be defined when their bit/value is set in the corresponding register :

   // Bits from PSCombinerCount (a.k.a. PSCombinerCountFlags) :
   #define PS_COMBINERCOUNT 2
   #define PS_COMBINERCOUNT_UNIQUE_C0
   #define PS_COMBINERCOUNT_UNIQUE_C1
   #define PS_COMBINERCOUNT_MUX_MSB

   // Generate defines like this, based on actual values :
   #define PS_COMPAREMODE_0(in) CM_LT(in.x) CM_LT(in.y) CM_LT(in.z) CM_LT(in.w)
   #define PS_COMPAREMODE_1(in) CM_LT(in.x) CM_LT(in.y) CM_LT(in.z) CM_LT(in.w)
   #define PS_COMPAREMODE_2(in) CM_LT(in.x) CM_LT(in.y) CM_LT(in.z) CM_LT(in.w)
   #define PS_COMPAREMODE_3(in) CM_LT(in.x) CM_LT(in.y) CM_LT(in.z) CM_LT(in.w)

   // Input texture register mappings for stage 1, 2 and 3 (stage 0 has no input-texture)
   static const int PS_INPUTTEXTURE_[4] = { -1, 0, 0, 0 };

   // Dot mappings for stage 1, 2 and 3 (stage 0 performs no dot product)
   #define PS_DOTMAPPING_1 PS_DOTMAPPING_MINUS1_TO_1_D3D
   #define PS_DOTMAPPING_2 PS_DOTMAPPING_MINUS1_TO_1_D3D
   #define PS_DOTMAPPING_3 PS_DOTMAPPING_MINUS1_TO_1_D3D

   // Bits from FinalCombinerFlags (the 4th byte in PSFinalCombinerInputsEFG) :
   #define PS_FINALCOMBINERSETTING_COMPLEMENT_V1
   #define PS_FINALCOMBINERSETTING_COMPLEMENT_R0
   #define PS_FINALCOMBINERSETTING_CLAMP_SUM
#endif

)DELIMITER",  /* This terminates the 1st raw string within the 16380 single-byte characters limit. // */
// See https://docs.microsoft.com/en-us/cpp/error-messages/compiler-errors-1/compiler-error-c2026?f1url=%3FappId%3DDev15IDEF1%26l%3DEN-US%26k%3Dk(C2026)%26rd%3Dtrue&view=vs-2019
// Second raw string :
R"DELIMITER(

// Define defaults when their inverses are not defined (handy while compiler isn't yet providing these) :
#ifndef PS_COMBINERCOUNT_SAME_C0
	#define PS_COMBINERCOUNT_UNIQUE_C0
#endif
#ifndef PS_COMBINERCOUNT_SAME_C1
	#define PS_COMBINERCOUNT_UNIQUE_C1
#endif
#ifndef PS_COMBINERCOUNT_MUX_LSB
	#define PS_COMBINERCOUNT_MUX_MSB
#endif

// PS_COMBINERCOUNT_UNIQUE_C0 steers whether for C0 to use stage-specific constants c0_0 .. c0_7, or c0_0 for all stages
#ifdef PS_COMBINERCOUNT_UNIQUE_C0
	#define C0 c0_[stage] // concatenate stage to form c0_0 .. c0_7
#else // PS_COMBINERCOUNT_SAME_C0
	#define C0 c0_[0] // always resolve to c0_0
#endif

// PS_COMBINERCOUNT_UNIQUE_C1 steers whether for C1 to use stage-specific constants c1_0 .. c1_7, or c1_0 for all stages
#ifdef PS_COMBINERCOUNT_UNIQUE_C1
	#define C1 c1_[stage] // concatenate stage to form c1_0 .. c1_7
#else // PS_COMBINERCOUNT_SAME_C1
	#define C1 c1_[0] // always resolve to c1_0
#endif

// PS_COMBINERCOUNT_MUX_MSB steers the 'muxing' operation in the XMMC opcode,
// checking either the Most Significant Bit (MSB) or Least (LSB) of the r0 register.
// (In practice, LSB is seldom encountered, we have zero known test-cases.)
#ifdef PS_COMBINERCOUNT_MUX_MSB
	#define FCS_MUX (r0.a >= 0.5) // Check r0.a MSB; Having range upto 1 this should be equal to : (((r0.a * 255) /*mod 256*/) >= 128)
#else // PS_COMBINERCOUNT_MUX_LSB
	#define FCS_MUX (((r0.a * 255) mod 2) >= 1) // Check r0.b LSB; Get LSB by converting 1 into 255 (highest 8-bit value) and using modulo 2. TODO : Verify correctness
#endif

// PS_FINALCOMBINERSETTING_COMPLEMENT_V1, when defined, applies a modifier to the v1 input when calculating the sum register
#ifdef PS_FINALCOMBINERSETTING_COMPLEMENT_V1
	#define FCS_V1 s_comp // making it use 1-complement,
#else
	#define FCS_V1 s_ident // otherwise identity mapping.
#endif

// PS_FINALCOMBINERSETTING_COMPLEMENT_R0, when defined, applies a modifier to the r0 input when calculating the sum register
#ifdef PS_FINALCOMBINERSETTING_COMPLEMENT_R0
	#define FCS_R0 s_comp // making it use 1-complement,
#else
	#define FCS_R0 s_ident // otherwise identity mapping.
#endif

// PS_FINALCOMBINERSETTING_CLAMP_SUM, when defined, applies a modifier to the sum register
#ifdef PS_FINALCOMBINERSETTING_CLAMP_SUM
	#define FCS_SUM s_sat // making it clamp negative to zero,
#else
	#define FCS_SUM s_ident // otherwise identity mapping. TODO : Confirm correctness
#endif

// Xbox supports only one 'pixel shader' opcode, but bit flags tunes it's function;
// Here, effective all 5 Xbox opcodes, extended with a variable macro {xop_m(m,...)} for destination modifier :
// Note : Since both d0 AND d1 could be the same output register, calculation of d2 can re-use only one (d0 or d1)
#define xmma(d0, d1, d2,  s0, s1, s2, s3, m, tmp) tmp = d0 = m(s0 * s1); d1 = m(s2 * s3); d2 =           d1 + tmp // PS_COMBINEROUTPUT_AB_CD_SUM=           0x00L, // 3rd output is AB+CD
#define xmmc(d0, d1, d2,  s0, s1, s2, s3, m, tmp) tmp = d0 = m(s0 * s1); d1 = m(s2 * s3); d2 = FCS_MUX ? d1 : tmp // PS_COMBINEROUTPUT_AB_CD_MUX=           0x04L, // 3rd output is MUX(AB,CD) based on R0.a

#define xdm(d0, d1,  s0, s1, s2, s3, m) d0 = m(dot(s0 , s1)); d1 = m(    s2 * s3 )                                // PS_COMBINEROUTPUT_AB_DOT_PRODUCT=      0x02L, // RGB only // PS_COMBINEROUTPUT_CD_MULTIPLY=         0x00L,
#define xdd(d0, d1,  s0, s1, s2, s3, m) d0 = m(dot(s0 , s1)); d1 = m(dot(s2 , s3))                                // PS_COMBINEROUTPUT_CD_DOT_PRODUCT=      0x01L, // RGB only // PS_COMBINEROUTPUT_AB_MULTIPLY=         0x00L, 
#define xmd(d0, d1,  s0, s1, s2, s3, m) d0 = m(    s0 * s1 ); d1 = m(dot(s2 , s3))                                // PS_COMBINEROUTPUT_AB_DOT_PRODUCT=      0x02L, // RGB only // PS_COMBINEROUTPUT_CD_MULTIPLY=         0x01L,

// After the register combiner stages, there's one (optional) final combiner step, consisting of 4 parts;
// All the 7 final combiner inputs operate on rgb only and clamp negative input to zero:
#define fcin(r) saturate(r)
// Special purpose registers prod and sum operate on rgb only, and have alpha set to zero
#define xfc_sum sum = FCS_SUM(float4(FCS_V1(fcin(v1.rgb)) + FCS_R0(fcin(r0.rgb)), 0)) // Note : perform sum first, so prod can use its result
#define xfc_prod(e, f) prod = float4(fcin(e) * fcin(f), 0) // Note : prod can't have modifiers
// Color and Alpha calculations are performed, potentially using sum and/or prod and/or fog registers
#define xfc_rgb(a, b, c, d) r0.rgb = lerp(fcin(c), fcin(b), fcin(a)) + fcin(d) // Note : perform rgb and alpha last, so prod and sum can be used as inputs
#define xfc_alpha(g) r0.a = fcin(g)

// Glue them all together, so we can generate a one-liner closing off the stages :
#define xfc(a, b, c, d, e, f, g) xfc_sum; xfc_prod(e, f); xfc_rgb(a, b, c, d); xfc_alpha(g)
// Note : If xfc is not generated (when PSFinalCombinerInputsABCD and PSFinalCombinerEFG are both 0), r0.rgba is still returned as pixel shader output

// GLSL : https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/mix.xhtml
//  mix(x,  y,  a )  x*(1-a ) +  y*a
// 
// HLSL : https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-lerp
// lerp(x,  y,  s )  x*(1-s ) +  y*s == x + s(y-x)
// lerp(s2, s1, s0) s2*(1-s0) + s1*s0
)DELIMITER",  /* This terminates the 1st raw string within the 16380 single-byte characters limit. // */
// See https://docs.microsoft.com/en-us/cpp/error-messages/compiler-errors-1/compiler-error-c2026?f1url=%3FappId%3DDev15IDEF1%26l%3DEN-US%26k%3Dk(C2026)%26rd%3Dtrue&view=vs-2019
// Second raw string :
R"DELIMITER(

float m21d(const float input)
{
	int tmp = (int)(input * 255); // Convert float 0..1 into byte 0..255
	tmp -= 128; // 0 lowers to -128, 128 lowers to 0, 255 lowers to 127
	return (float)tmp / 127; // -128 scales to -1.007874016, 0 scales to 0.0, 127 scales to 1.0
}

float m21g(const float input)
{
	int tmp = (int)(input * 255); // Convert float 0..1 into byte 0..255
	if (tmp >= 128) {
		tmp -= 256; // 128 lowers to -128, 255 lowers to -1
	} // 0 stays 0, 127 stays 127

	return ((float)tmp + 0.5) / 127.5;
}

float m21(const float input)
{
	int tmp = (int)(input * 255); // Convert float 0..1 into byte 0..255
	if (tmp >= 128) {
		tmp -= 256; // 128 lowers to -128, 255 lowers to -1
	} // 0 stays 0, 127 stays 127

	return (float)tmp / 127; // -128 scales to -1.007874016, 0 scales to 0.0, 127 scales to 1.0
}

// Note : each component seems already in range [0..1], but two must be combined into one
#define TwoIntoOne(a,b) (((a * 255) * 256) + (b * 255)) / 255               // TODO : Verify whether this works at all !
#define CalcHiLo(in) H = TwoIntoOne(in.x, in.y); L = TwoIntoOne(in.z, in.w) // TODO : Verify whether this works at all !

// Dot mappings over the output value of a (4 component 8 bit unsigned) texture stage register into a (3 component float) vector value, for use in a dot product calculation:
#define PS_DOTMAPPING_ZERO_TO_ONE(in)         dm = in.rgb                                          // :r8g8b8a8->(r,g,b):                                                   0x00=>0,                       0xff=>1 thus : output =                     (input / 0xff  )
#define PS_DOTMAPPING_MINUS1_TO_1_D3D(in)     dm = float3(m21d(in.x), m21d(in.y), m21d(in.z))      // :r8g8b8a8->(r,g,b):               0x00=>-128/127,         0x01=>-1,   0x80=>0,                       0xff=>1 thus : output =                                        ((input - 0x100  ) / 0x7f  )
#define PS_DOTMAPPING_MINUS1_TO_1_GL(in)      dm = float3(m21g(in.x), m21g(in.y), m21g(in.z))      // :r8g8b8a8->(r,g,b):                                       0x80=>-1,   0x00=>0,                       0x7f=>1 thus : output =  (input < 0x80  ) ? (input / 0x7f  ) : ((input - 0x100  ) / 0x80  ) (see https://en.wikipedia.org/wiki/Two's_complement)
#define PS_DOTMAPPING_MINUS1_TO_1(in)         dm = float3(m21(in.x),  m21(in.y),  m21(in.z))       // :r8g8b8a8->(r,g,b):               0x80=>-128/127,        ?0x81=>-1,   0x00=>0,                       0x7f=>1 thus : output =  (input < 0x80  ) ? (input / 0x7f  ) : ((input - 0x100  ) / 0x7f  ) (see https://en.wikipedia.org/wiki/Two's_complement)

#define PS_DOTMAPPING_HILO_1(in)              CalcHiLo(in); dm = float3(H, L, 1)                   // :H16L16  ->(H,L,1):                                                 0x0000=>0,                     0xffff=>1 thus : output =                     (input / 0xffff)
#define PS_DOTMAPPING_HILO_HEMISPHERE_D3D(in) CalcHiLo(in); dm = float3(H, L, sqrt(1-(H*H)-(L*L))) // :H16L16  ->(H,L,sqrt(1-H^2-L^2)):?                      0x8000=>-1, 0x0000=>0, 0x7fff=32767/32768            thus : output =                                        ((input - 0x10000) / 0x7fff)
#define PS_DOTMAPPING_HILO_HEMISPHERE_GL(in)  CalcHiLo(in); dm = float3(H, L, sqrt(1-(H*H)-(L*L))) // :H16L16  ->(H,L,sqrt(1-H^2-L^2)):?                      0x8000=>-1, 0x0000=>0,                     0x7fff=>1 thus : output =  (input < 0x8000) ? (input / 0x7fff) : ((input - 0x10000) / 0x8000)
#define PS_DOTMAPPING_HILO_HEMISPHERE(in)     CalcHiLo(in); dm = float3(H, L, sqrt(1-(H*H)-(L*L))) // :H16L16  ->(H,L,sqrt(1-H^2-L^2)): 0x8000=>-32768/32767, 0x8001=>-1, 0x0000=>0,                     0x7fff=>1 thus : output =  (input < 0x8000) ? (input / 0x7fff) : ((input - 0x10000) / 0x7fff)

// Declare one sampler per each {Sampler Type, Texture Stage} combination
// TODO : Generate sampler status?
sampler samplers[4] : register(s0);

// Generated alphakill contents are based on X_D3DTSS_ALPHAKILL (we avoid using a constant, to allow false's to be optimized away)
// bool alphakill[4] = {false, false, false, false}; // Generated by PixelShader.cpp::BuildShader()

// Actual texture sampling per stage (always uses the s sampling vector variable as input)
// abstracting away the specifics of accessing above sampler declarations (usefull for future Direct3D 10+ sampler arrays)
float4 Sample2D(int st, float3 s)
{
	float4 result = tex2D(samplers[st], s.xy); // Ignores s.z (and whatever it's set to, will be optimized away by the compiler, see [1] below)
	if (alphakill[st])
		if (result.a == 0)
			discard;

	return result;
}

float4 Sample3D(int st, float3 s)
{
	float4 result = tex3D(samplers[st], s.xyz);
	if (alphakill[st])
		if (result.a == 0)
			discard;

	return result;
}

float4 Sample6F(int st, float3 s)
{
	float4 result = texCUBE(samplers[st], s.xyz);
	if (alphakill[st])
		if (result.a == 0)
			discard;

	return result;
}

// Map texture registers to their array elements. Having texture registers in an array allows indexed access to them
#define t0 t[0]
#define t1 t[1]
#define t2 t[2]
#define t3 t[3]

// Resolve a stage number via 'input texture (index) mapping' to it's corresponding output texture register (rgba?)
#define src(st) t[PS_INPUTTEXTURE_[st]]

// Calculate the dot result for a given stage. Since any given stage is input-mapped to always be less than or equal the stage it appears in, this won't cause read-ahead issues
#define CalcDot(st) PS_DOTMAPPING_ ## st(src(st)); dot_[st] = dot(iT[st].xyz, dm)

// Addressing operations
#define Passthru(st)  float4(saturate(iT[st].xyz), 1)                       // Clamps input texture coordinates to the range [0..1]
#define Brdf(st)      float3(t[st-2].y,  t[s1-1].y,  t[st-2].x - t[st-1].x) // TODO : Complete 16 bit phi/sigma retrieval from float4 texture register. Perhaps use CalcHiLo?
#define Normal2(st)   float3(dot_[st-1], dot_[st],   0)                     // Preceding and current stage dot result. Will be input for Sample2D.
#define Normal3(st)   float3(dot_[st-2], dot_[st-1], dot_[st])              // Two preceding and current stage dot result.
#define Eye           float3(iT[1].w,    iT[2].w,    iT[3].w)               // 4th (q) component of input texture coordinates 1, 2 and 3. Only used by texm3x3vspec/PS_TEXTUREMODES_DOT_RFLCT_SPEC, always at stage 3. TODO : Map iT[1/2/3] through PS_INPUTTEXTURE_[]?
#define Reflect(n, e) (2 * n * dot(n, e)) / dot(n, n)                       // TODO : Prevent division by zero when n == 0?
#define BumpEnv(st)   float3(iT[st].x + (BEM[st].x * src(st).r) + (BEM[st].y * src(st).g), iT[st].y + (BEM[st].z * src(st).r) + (BEM[st].w * src(st).g), 0) // Will be input for Sample2D. TODO : Compact into a regular 2x2 maxtrix multiplication.
#define LSO(st)       (LUM[st].x * src(st).b) + LUM[st].y                   // Uses PSH_XBOX_CONSTANT_LUM .x = D3DTSS_BUMPENVLSCALE .y = D3DTSS_BUMPENVLOFFSET

// Implementations for all possible texture modes, with stage as argument (prefixed with valid stages and corresponding pixel shader 1.3 assembly texture addressing instructions)
// For ease of understanding, all follow this plan : Optional specifics, or dot calculation (some with normal selection) and sampling vector determination. All end by deriving a value and assigning this to the stage's texture register.
/*0123 tex          */ #define PS_TEXTUREMODES_NONE(st)                                                                    v = black;           t[st] = v // Seems to work
/*0123 tex          */ #define PS_TEXTUREMODES_PROJECT2D(st)                                          s = iT[st].xyz;      v = Sample2D(st, s); t[st] = v // Seems to work (are x/w and y/w implicit?) [1]
/*0123 tex          */ #define PS_TEXTUREMODES_PROJECT3D(st)                                          s = iT[st].xyz;      v = Sample3D(st, s); t[st] = v // Seems to work (is z/w implicit?)
/*0123 tex          */ #define PS_TEXTUREMODES_CUBEMAP(st)                                            s = iT[st].xyz;      v = Sample6F(st, s); t[st] = v // TODO : Test
/*0123 texcoord     */ #define PS_TEXTUREMODES_PASSTHRU(st)                                                                v = Passthru(st);    t[st] = v // Seems to work
/*0123 texkill      */ #define PS_TEXTUREMODES_CLIPPLANE(st)            PS_COMPAREMODE_ ## st(iT[st]);                     v = black;           t[st] = v // Seems to work (setting black to texture register, in case it gets read)
/*-123 texbem       */ #define PS_TEXTUREMODES_BUMPENVMAP(st)                                         s = BumpEnv(st);     v = Sample2D(st, s); t[st] = v // Seems to work
/*-123 texbeml      */ #define PS_TEXTUREMODES_BUMPENVMAP_LUM(st)       PS_TEXTUREMODES_BUMPENVMAP(st);                    v.rgb *= LSO(st);    t[st] = v // TODO : Test
/*--23 texbrdf      */ #define PS_TEXTUREMODES_BRDF(st)                                               s = Brdf(st);        v = Sample3D(st, s); t[st] = v // TODO : Test (t[st-2] is 16 bit eyePhi,eyeSigma; t[st-1] is lightPhi,lightSigma)
/*--23 texm3x2tex   */ #define PS_TEXTUREMODES_DOT_ST(st)               CalcDot(st); n = Normal2(st); s = n;               v = Sample2D(st, s); t[st] = v // TODO : Test
/*--23 texm3x2depth */ #define PS_TEXTUREMODES_DOT_ZW(st)               CalcDot(st); n = Normal2(st); if (n.y==0) v=1;else v = n.x / n.y;       t[st] = v // TODO : Make depth-check use result of division, but how?
/*--2- texm3x3diff  */ #define PS_TEXTUREMODES_DOT_RFLCT_DIFF(st)       CalcDot(st); n = Normal3(st); s = n;               v = Sample6F(st, s); t[st] = v // TODO : Test
/*---3 texm3x3vspec */ #define PS_TEXTUREMODES_DOT_RFLCT_SPEC(st)       CalcDot(st); n = Normal3(st); s = Reflect(n, Eye); v = Sample6F(st, s); t[st] = v // TODO : Test
/*---3 texm3x3tex   */ #define PS_TEXTUREMODES_DOT_STR_3D(st)           CalcDot(st); n = Normal3(st); s = n;               v = Sample3D(st, s); t[st] = v // TODO : Test
/*---3 texm3x3tex   */ #define PS_TEXTUREMODES_DOT_STR_CUBE(st)         CalcDot(st); n = Normal3(st); s = n;               v = Sample6F(st, s); t[st] = v // TODO : Test
/*-123 texreg2ar    */ #define PS_TEXTUREMODES_DPNDNT_AR(st)                                          s = src(st).arg;     v = Sample2D(st, s); t[st] = v // TODO : Test [1]
/*-123 texreg2bg    */ #define PS_TEXTUREMODES_DPNDNT_GB(st)                                          s = src(st).gba;     v = Sample2D(st, s); t[st] = v // TODO : Test [1]
/*-12- texm3x2pad   */ #define PS_TEXTUREMODES_DOTPRODUCT(st)           CalcDot(st);                                       v = float4(dm, 0);   t[st] = v // TODO : Test all dot mapping (setting texture register, in case it gets read - test-case : BumpDemo) 
/*---3 texm3x3spec  */ #define PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST(st) CalcDot(st); n = Normal3(st); s = Reflect(n, c0);  v = Sample6F(st, s); t[st] = v // TODO : Test
// [1] Note : 3rd component set to s.z is just an (ignored) placeholder to produce a float3 (made unique, to avoid the potential complexity of repeated components)

PS_OUTPUT main(const PS_INPUT xIn)
{
	// Local constants
	const float4 zero = 0;
	const float4 half = 0.5; // = s_negbias(zero)
	const float4 one = 1; // = s_comp(zero)
	const float4 black = float4(0, 0, 0, 1); // opaque black
	const float4 iT[4] = { xIn.iT0, xIn.iT1, xIn.iT2, xIn.iT3 }; // Map input texture coordinates to an array, for indexing purposes

	// Xbox register variables
	float4 r0, r1;         // Temporary registers
	float4 t[4];           // Texture coordinate registers
	float4 v0, v1;         // Vertex color registers
	float4 _discard;       // Write-only discard 'register' (we assume the HLSL compilers' optimization pass will remove assignments to this)
	float4 fog;            // Read-only fog register, reading alpha is only allowed in final combiner
	float4 sum, prod;      // Special purpose registers for xfc (final combiner) operation

	// Helper variables
	int stage;             // Write-only variable, generated prefixing each 'opcode', for use in C0 and C1 macro's (and should thus get optimized away)
	float4 tmp;
	float H, L;            // HILO (high/low) temps
	float dot_[4];
	float3 dm;             // Dot mapping temporary
	float3 n;              // Normal vector (based on preceding dot_[] values)
	float3 s;              // Actual texture coordinate sampling coordinates (temporary)
	float4 v;              // Texture value (temporary)

	// Determine if this is a front face or backface
	bool isFrontFace = (xIn.iFF * FRONTFACE_FACTOR) >= 0;

	// Initialize variables
	r0 = r1 = black; // Note : r0.a/r1.a will be overwritten by t0.a/t1.a (opaque_black will be retained for PS_TEXTUREMODES_NONE)
	// Note : VFACE/FrontFace has been unreliable, investigate again if some test-case shows bland colors
	v0 = isFrontFace ? xIn.iD0 : xIn.iB0; // Diffuse front/back
	v1 = isFrontFace ? xIn.iD1 : xIn.iB1; // Specular front/back
	fog = float4(c_fog.rgb, xIn.iFog); // color from PSH_XBOX_CONSTANT_FOG, alpha from vertex shader output / pixel shader input

	// Xbox shader program
)DELIMITER",  /* This terminates the 2nd raw string within the 16380 single-byte characters limit. // */
// Third and last raw string, the footer :
R"DELIMITER(

	// Copy r0.rgba to output
	PS_OUTPUT xOut;

	xOut.oR0 = r0;

	return xOut;
}

// End of pixel shader footer)DELIMITER" /* This terminates the footer raw string" // */
