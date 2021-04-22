#define LOG_PREFIX CXBXR_MODULE::VTXSH

#include "Shader.h" // EmuCompileShader
#include "VertexShader.h" // EmuCompileVertexShader
#include "core\kernel\init\CxbxKrnl.h" // implicit CxbxKrnl_Xbe used in LOG_TEST_CASE
#include "core\kernel\support\Emu.h" // LOG_TEST_CASE (via Logging.h)

#include <fstream>
#include <sstream> // std::stringstream

extern const char* g_vs_model = vs_model_3_0;

// HLSL generation
void OutputHlsl(std::stringstream& hlsl, VSH_IMD_OUTPUT& dest)
{
	static const char* OReg_Name[/*VSH_OREG_NAME*/] = {
		"oPos",
		"???",
		"???",
		"oD0",
		"oD1",
		"oFog",
		"oPts",
		"oB0",
		"oB1",
		"oT0",
		"oT1",
		"oT2",
		"oT3",
		"???",
		"???",
		"a0.x"
	};

	switch (dest.Type) {
	case IMD_OUTPUT_C:
		// Access the HLSL capital C[] constants array, with the index bias applied :
		// TODO : Avoid out-of-bound writes (perhaps writing to a reserved index?)
		hlsl << "C[" << dest.Address + X_D3DSCM_CORRECTION << "]";
		LOG_TEST_CASE("Vertex shader writes to constant table");
		break;
	case IMD_OUTPUT_R:
		hlsl << "r" << dest.Address;
		break;
	case IMD_OUTPUT_O:
		assert(dest.Address < OREG_A0X);
		hlsl << OReg_Name[dest.Address];
		break;
	case IMD_OUTPUT_A0X:
		hlsl << "a0";
		break;
	default:
		assert(false);
		break;
	}

	// Write the mask as a separate argument to the opcode defines
	// (No space, so that "dest,mask, ..." looks close to "dest.mask, ...")
	hlsl << ",";

	// Detect oFog masks other than x
	// Test case: Lego Star Wars II (menu)
	if (dest.Type == IMD_OUTPUT_O &&
		dest.Address == OREG_OFOG &&
		dest.Mask != MASK_X)
	{
		LOG_TEST_CASE("Vertex shader uses oFog mask other than x");
		EmuLog(LOG_LEVEL::WARNING, "oFog mask was %#x", dest.Mask);
		hlsl << "x"; // write to x instead
		return;
	}

	// Write the mask
	if (dest.Mask & MASK_X) hlsl << "x";
	if (dest.Mask & MASK_Y) hlsl << "y";
	if (dest.Mask & MASK_Z) hlsl << "z";
	if (dest.Mask & MASK_W) hlsl << "w";
}

void ParameterHlsl(std::stringstream& hlsl, VSH_IMD_PARAMETER& param, bool IndexesWithA0_X)
{
	static char* RegisterName[/*VSH_PARAMETER_TYPE*/] = {
		"?", // PARAM_UNKNOWN = 0,
		"r", // PARAM_R,          // Temporary (scRatch) registers
		"v", // PARAM_V,          // Vertex registers
		"c", // PARAM_C,          // Constant registers, set by SetVertexShaderConstant
		"oPos" // PARAM_O // = 0??
	};

	if (param.Neg) {
		hlsl << "-";
	}

	if (param.ParameterType == PARAM_C) {
		// Access constant registers through our HLSL c() function,
		// which allows dumping negative indices (like Xbox shaders),
		// and which returns zero when out-of-bounds indices are passed in:
		if (IndexesWithA0_X) {
			if (param.Address == 0) {
				hlsl << "c(a0.x)"; // Hide the offset if it's 0
			}
			else if (param.Address < 0) {
				hlsl << "c(a0.x" << param.Address << ")"; // minus is part of the offset
			}
			else {
				hlsl << "c(a0.x+" << param.Address << ")"; // show addition character
			}
		}
		else {
			hlsl << "c(" << param.Address << ")";
		}
	}
	else {
		hlsl << RegisterName[param.ParameterType] << param.Address;
	}

	// Write the swizzle if we need to
	// Only bother printing the swizzle if it is not the default .xyzw
	if (!(param.Swizzle[0] == SWIZZLE_X &&
		param.Swizzle[1] == SWIZZLE_Y &&
		param.Swizzle[2] == SWIZZLE_Z &&
		param.Swizzle[3] == SWIZZLE_W)) {
		// We'll try to simplify swizzles if we can
		// If all swizzles are the same, we only need to write one out
		unsigned swizzles = 1;

		// Otherwise, we need to use the full swizzle
		if (param.Swizzle[0] != param.Swizzle[1] ||
			param.Swizzle[0] != param.Swizzle[2] ||
			param.Swizzle[0] != param.Swizzle[3]) {
			// Note, we can't remove trailing repeats, like in VS asm,
			// as it may change the type from float4 to float3, float2 or float1!
			swizzles = 4;
		}

		hlsl << ".";
		for (unsigned i = 0; i < swizzles; i++) {
			hlsl << "xyzw"[param.Swizzle[i]];
		}
	}
}

void BuildShader(IntermediateVertexShader* pShader, std::stringstream& hlsl)
{
	// HLSL strings for all MAC opcodes, indexed with VSH_MAC
	static std::string VSH_MAC_HLSL[/*VSH_MAC*/] = {
		/*MAC_NOP:*/"",
		/*MAC_MOV:*/"x_mov",
		/*MAC_MUL:*/"x_mul",
		/*MAC_ADD:*/"x_add",
		/*MAC_MAD:*/"x_mad",
		/*MAC_DP3:*/"x_dp3",
		/*MAC_DPH:*/"x_dph",
		/*MAC_DP4:*/"x_dp4",
		/*MAC_DST:*/"x_dst",
		/*MAC_MIN:*/"x_min",
		/*MAC_MAX:*/"x_max",
		/*MAC_SLT:*/"x_slt",
		/*MAC_SGE:*/"x_sge",
		/*MAC_ARL:*/"x_arl",
					"",
					"" // VSH_MAC 2 final values of the 4 bits are undefined/unknown  TODO : Investigate their effect (if any) and emulate that as well
	};

	// HLSL strings for all ILU opcodes, indexed with VSH_ILU
	static std::string VSH_ILU_HLSL[/*VSH_ILU*/] = {
		/*ILU_NOP:*/"",
		/*ILU_MOV:*/"x_mov",
		/*ILU_RCP:*/"x_rcp",
		/*ILU_RCC:*/"x_rcc",
		/*ILU_RSQ:*/"x_rsq",
		/*ILU_EXP:*/"x_expp",
		/*ILU_LOG:*/"x_logp",
		/*ILU_LIT:*/"x_lit" // = 7 - all values of the 3 bits are used
	};

	for (size_t i = 0; i < pShader->Instructions.size(); i++) {
		VSH_INTERMEDIATE_FORMAT& IntermediateInstruction = pShader->Instructions[i];

		std::string str;
		if (IntermediateInstruction.MAC > MAC_NOP) {
			str = VSH_MAC_HLSL[IntermediateInstruction.MAC];
		}
		else {
			str = VSH_ILU_HLSL[IntermediateInstruction.ILU];
		}

		hlsl << "\n  " << str << "("; // opcode
		OutputHlsl(hlsl, IntermediateInstruction.Output);
		for (unsigned i = 0; i < IntermediateInstruction.ParamCount; i++) {
			hlsl << ", ";
			ParameterHlsl(hlsl, IntermediateInstruction.Parameters[i], IntermediateInstruction.IndexesWithA0_X);
		}

		hlsl << ");";
	}
}

// recompile xbox vertex shader function
extern HRESULT EmuCompileVertexShader
(
	IntermediateVertexShader* pIntermediateShader,
	ID3DBlob** ppHostShader
)
{
	// Include HLSL header and footer as raw strings :
	static std::string hlsl_template[2] = {
		#include "core\hle\D3D8\Direct3D9\CxbxVertexShaderTemplate.hlsl"
	};

	auto hlsl_stream = std::stringstream();
	hlsl_stream << hlsl_template[0]; // Start with the HLSL template header
	assert(pIntermediateShader->Instructions.size() > 0);
	BuildShader(pIntermediateShader, hlsl_stream);

	hlsl_stream << hlsl_template[1]; // Finish with the HLSL template footer
	std::string hlsl_str = hlsl_stream.str();

	HRESULT hRet = EmuCompileShader(hlsl_str, g_vs_model, ppHostShader, "CxbxVertexShaderTemplate.hlsl");
	
	if (FAILED(hRet) && (g_vs_model != vs_model_3_0)) {
		// If the shader failed in the default vertex shader model, retry in vs_model_3_0
		// This allows shaders too large for 2_a to be compiled (Test Case: Shenmue 2)
		EmuLog(LOG_LEVEL::WARNING, "Shader compile failed. Retrying with shader model 3.0");
		hRet = EmuCompileShader(hlsl_str, vs_model_3_0, ppHostShader, "CxbxVertexShaderTemplate.hlsl");
	}
		
	return hRet;
}

extern void EmuCompileFixedFunction(ID3DBlob** ppHostShader)
{
	static ID3DBlob* pShader = nullptr;

	// TODO does this need to be thread safe?
	if (pShader == nullptr) {
		// Determine the filename and directory for the fixed function shader
		auto hlslDir = std::filesystem::path(szFilePath_CxbxReloaded_Exe)
			.parent_path()
			.append("hlsl");

		auto sourceFile = hlslDir.append("FixedFunctionVertexShader.hlsl").string();

		// Load the shader into a string
		std::ifstream hlslStream(sourceFile);
		std::stringstream hlsl;
		hlsl << hlslStream.rdbuf();

		// Compile the shader
		EmuCompileShader(hlsl.str(), g_vs_model, &pShader, sourceFile.c_str());
	}

	*ppHostShader = pShader;
};

static ID3DBlob* pPassthroughShader = nullptr;

extern HRESULT EmuCompileXboxPassthrough(ID3DBlob** ppHostShader)
{
	// TODO does this need to be thread safe?
	if (pPassthroughShader == nullptr) {
		auto hlsl =
R"(
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
	float  oPts : PSIZE;	 // Point size
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

	// Fogging
	// TODO deduplicate
	const float fogDepth      =   abs(oFog.x); 
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
	xOut.oFog = fogFactor; // Note : Xbox clamps fog in pixel shader
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
)";

		EmuCompileShader(hlsl, g_vs_model, &pPassthroughShader, "passthrough.hlsl");
	}

	*ppHostShader = pPassthroughShader;

	return 0;
}
