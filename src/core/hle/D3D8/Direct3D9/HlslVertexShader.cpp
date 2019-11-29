
//#define LOG_PREFIX CXBXR_MODULE::VTXSH
//#include <string>
//
////#include "HlslVertexShader.h"
//
////#include "XbD3D8Types.h" // For X_D3DVSDE_*
//#include <sstream>
//#include <regex>
////#include <unordered_map>
//#include <array>
////#include <bitset>

//#pragma once
//#include "core\hle\D3D8\XbVertexShader.cpp"

// HLSL outputs
std::array<std::string, 16> outputs =
{
	"oPos",
	"oT0",
	"oT1",
	"oT2",
	"oT3",
	"oT4",
	"oD0",
	"oD1",
	"oB0",
	"oB1",
	"oFog",
	"oPts"
};

std::string ToHlsl(VSH_IMD_OUTPUT& dest) {
	auto hlsl = std::stringstream();
	switch (dest.Type)
	{
	case IMD_OUTPUT_O:
		hlsl << "out." << outputs[dest.Address];
		break;
	case IMD_OUTPUT_A0X:
		hlsl << "a";
		break;
	case IMD_OUTPUT_C:
		hlsl << "c[" << dest.Address << "]"; //todo we can output to constants...?
		break;
	case IMD_OUTPUT_R:
		hlsl << "r[" << dest.Address << "]";
		break;
	default:
		break;
	}

	return hlsl.str();
}

std::string ToHlsl(VSH_IMD_PARAMETER& parameter)
{
	auto hlsl = std::stringstream();

	hlsl << (parameter.Parameter.Neg ? "-" : "") << VshGetRegisterName(parameter.Parameter.ParameterType);

	if (parameter.Parameter.ParameterType == PARAM_C && parameter.IndexesWithA0_X)
	{
		// Only display the offset if it's not 0.
		parameter.Parameter.Address
		? hlsl << "[a+" << parameter.Parameter.Address << "]"
		: hlsl << "[a]";
	}
	else
	{
		hlsl << parameter.Parameter.Address;
	}

	// Only bother printing the swizzle if it is not .xyzw
	if (!(parameter.Parameter.Swizzle[0] == SWIZZLE_X &&
		parameter.Parameter.Swizzle[1] == SWIZZLE_Y &&
		parameter.Parameter.Swizzle[2] == SWIZZLE_Z &&
		parameter.Parameter.Swizzle[3] == SWIZZLE_W))
	{
		hlsl << ".";
		for (int i = 0; i < 4; i++)
		{
			char Swizzle = '?';
			switch (parameter.Parameter.Swizzle[i])
			{
			case SWIZZLE_X:
				Swizzle = 'x';
				break;
			case SWIZZLE_Y:
				Swizzle = 'y';
				break;
			case SWIZZLE_Z:
				Swizzle = 'z';
				break;
			case SWIZZLE_W:
				Swizzle = 'w';
				break;
			}
			hlsl << Swizzle;
		}
	}

	return hlsl.str();
}

std::string ToHlsl(std::string pattern, VSH_INTERMEDIATE_FORMAT& instruction) {
	auto static dest = std::regex("dest");
	const std::regex src[] = { std::regex("src0"), std::regex("src1"), std::regex("src2") };

	// TODO use simple string replace
	// Warn if we didn't replace anything etc.
	// Replace dest
	auto hlsl = std::regex_replace(pattern, std::regex("dest"), ToHlsl(instruction.Output));

	int srcNum = 0;
	for (int i = 0; i < 3; i++) { // TODO remove magic number
		if (instruction.Parameters[i].Active) {
			hlsl = std::regex_replace(hlsl, std::regex(src[srcNum]), ToHlsl(instruction.Parameters[i]));
			srcNum += 1;
		}
	}

	return hlsl;
}

std::string BuildShader(VSH_XBOX_SHADER* pShader) {

	auto hlsl = std::stringstream();


	for (int i = 0; i < pShader->IntermediateCount; i++) {

		VSH_INTERMEDIATE_FORMAT xboxInstruction = pShader->Intermediate[i];

		if (xboxInstruction.InstructionType == IMD_MAC)
		{
			switch (xboxInstruction.MAC)
			{
			case MAC_NOP:
				break;
			case MAC_MOV:
				hlsl << ToHlsl("dest = src0", xboxInstruction);
				break;
			case MAC_MUL:
				hlsl << ToHlsl("dest = src0 * src1", xboxInstruction);
				break;
			case MAC_ADD:
				hlsl << ToHlsl("dest = src0 + src1", xboxInstruction);
				break;
			case MAC_MAD:
				hlsl << ToHlsl("dest = mad(src0, src1, src2)", xboxInstruction);
				break;
			case MAC_DP3:
				hlsl << ToHlsl("dest = dot(src0, src1)", xboxInstruction);
				break;
			case MAC_DPH:
				hlsl << ToHlsl("dest = dot(src0, src1) + src1.w", xboxInstruction);
				break;
			case MAC_DP4:
				hlsl << ToHlsl("dest = dot(src0, src1)", xboxInstruction);
				break;
			case MAC_DST:
				hlsl << ToHlsl("dest = x_dst(src0, src1)", xboxInstruction);
				break;
			case MAC_MIN:
				hlsl << ToHlsl("dest = min(src0, src1)", xboxInstruction);
				break;
			case MAC_MAX:
				hlsl << ToHlsl("dest = max(src0, src1)", xboxInstruction);
				break;
			case MAC_SLT:
				hlsl << ToHlsl("dest = x_slt(src0, src1)", xboxInstruction);
				break;
			case MAC_SGE:
				hlsl << ToHlsl("dest = x_sge(src0, src1)", xboxInstruction);
				break;
			case MAC_ARL:
				hlsl << ToHlsl("a = src0", xboxInstruction);
				break;
			default:
				EmuLog(LOG_LEVEL::WARNING, "TODO message");
			}
		}
		else if (xboxInstruction.InstructionType == IMD_ILU)
		{
			switch (xboxInstruction.ILU)
			{
			case ILU_NOP:
				break;
			case ILU_MOV:
				hlsl << ToHlsl("dest = src0", xboxInstruction);
				break;
			case ILU_RCP:
				hlsl << ToHlsl("// rcp TODO", xboxInstruction);
				break;
			case ILU_RCC:
				hlsl << ToHlsl("// rcc TODO", xboxInstruction);
				break;
			case ILU_RSQ:
				hlsl << ToHlsl("// rsq TODO", xboxInstruction);
				break;
			case ILU_EXP:
				hlsl << ToHlsl("// exp TODO", xboxInstruction);
				break;
			case ILU_LOG:
				hlsl << ToHlsl("// log TODO", xboxInstruction);
				break;
			case ILU_LIT:
				hlsl << ToHlsl("// lit TODO", xboxInstruction);
				break;
			default:
				EmuLog(LOG_LEVEL::WARNING, "TODO message");
			}
		}
		else
		{
			EmuLog(LOG_LEVEL::WARNING, "TODO message");
		}

		// Finish the line
		hlsl << "\n";
	}
	return hlsl.str();
}
