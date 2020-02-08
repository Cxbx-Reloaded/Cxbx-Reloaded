#define LOG_PREFIX CXBXR_MODULE::VTXSH

#include "VertexShader.h"
#include "core\kernel\init\CxbxKrnl.h"
#include "core\kernel\support\Emu.h"

#include <sstream>

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

	for (int i = 0; i < pShader->Instructions.size(); i++) {
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

std::string DebugPrependLineNumbers(std::string shaderString) {
	std::stringstream shader(shaderString);
	auto debugShader = std::stringstream();

	int i = 1;
	for (std::string line; std::getline(shader, line); ) {
		auto lineNumber = std::to_string(i++);
		auto paddedLineNumber = lineNumber.insert(0, 3 - lineNumber.size(), ' ');
		debugShader << "/* " << paddedLineNumber << " */ " << line << "\n";
	}

	return debugShader.str();
}


extern ShaderType EmuGetShaderInfo(IntermediateVertexShader* pIntermediateShader) {

	if (pIntermediateShader->Instructions.size() == 0) {
		// Do not attempt to compile empty shaders
		// This is a declaration only shader, so there is no function to compile
		return ShaderType::Empty;
	}

	switch (pIntermediateShader->Header.Version) {
		case VERSION_XVS:
			break;
		case VERSION_XVSS:
			LOG_TEST_CASE("Might not support vertex state shaders?");
			break;
		case VERSION_XVSW:
			EmuLog(LOG_LEVEL::WARNING, "Might not support vertex read/write shaders?");
			return ShaderType::Unsupported;
		default:
			EmuLog(LOG_LEVEL::WARNING, "Unknown vertex shader version 0x%02X", pIntermediateShader->Header.Version);
			return ShaderType::Unsupported;
	}

	return ShaderType::Compilable;
}

// recompile xbox vertex shader function
extern HRESULT EmuCompileShader
(
	IntermediateVertexShader* pIntermediateShader,
	ID3DBlob** ppHostShader
)
{
	// TODO include header in vertex shader
	//XTL::X_VSH_SHADER_HEADER* pXboxVertexShaderHeader = (XTL::X_VSH_SHADER_HEADER*)pXboxFunction;
	ID3DBlob* pErrors = nullptr;
	HRESULT             hRet = 0;

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

	EmuLog(LOG_LEVEL::DEBUG, "--- HLSL conversion ---");
	EmuLog(LOG_LEVEL::DEBUG, DebugPrependLineNumbers(hlsl_str).c_str());
	EmuLog(LOG_LEVEL::DEBUG, "-----------------------");

	// Level 0 for fastest runtime compilation
	// TODO Can we recompile an optimized shader in the background?
	UINT flags1 = D3DCOMPILE_OPTIMIZATION_LEVEL0;

	hRet = D3DCompile(
		hlsl_str.c_str(),
		hlsl_str.length(),
		nullptr, // pSourceName
		nullptr, // pDefines
		nullptr, // pInclude // TODO precompile x_* HLSL functions?
		"main", // shader entry poiint
		"vs_3_0", // shader profile
		flags1, // flags1
		0, // flags2
		ppHostShader, // out
		&pErrors // ppErrorMsgs out
	);
	if (FAILED(hRet)) {
		// Attempt to retry in compatibility mode, this allows some vertex-state shaders to compile
		// Test Case: Spy vs Spy
		flags1 |= D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY;
		hRet = D3DCompile(
			hlsl_str.c_str(),
			hlsl_str.length(),
			nullptr, // pSourceName
			nullptr, // pDefines
			nullptr, // pInclude // TODO precompile x_* HLSL functions?
			"main", // shader entry poiint
			"vs_3_0", // shader profile
			flags1, // flags1
			0, // flags2
			ppHostShader, // out
			&pErrors // ppErrorMsgs out
		);

		if (FAILED(hRet)) {
			LOG_TEST_CASE("Couldn't assemble recompiled vertex shader");
			//EmuLog(LOG_LEVEL::WARNING, "Couldn't assemble recompiled vertex shader");
		}
	}

	// Determine the log level
	auto hlslErrorLogLevel = FAILED(hRet) ? LOG_LEVEL::ERROR2 : LOG_LEVEL::DEBUG;
	if (pErrors) {
		// Log HLSL compiler errors
		EmuLog(hlslErrorLogLevel, "%s", (char*)(pErrors->GetBufferPointer()));
		pErrors->Release();
		pErrors = nullptr;
	}

	LOG_CHECK_ENABLED(LOG_LEVEL::DEBUG)
		if (g_bPrintfOn)
			if (!FAILED(hRet)) {
				// Log disassembly
				hRet = D3DDisassemble(
					(*ppHostShader)->GetBufferPointer(),
					(*ppHostShader)->GetBufferSize(),
					D3D_DISASM_ENABLE_DEFAULT_VALUE_PRINTS | D3D_DISASM_ENABLE_INSTRUCTION_NUMBERING,
					NULL,
					&pErrors
				);
				if (pErrors) {
					EmuLog(hlslErrorLogLevel, "%s", (char*)(pErrors->GetBufferPointer()));
					pErrors->Release();
				}
			}

	return hRet;
}
