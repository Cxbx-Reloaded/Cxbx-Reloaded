// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *  This file is part of the Cxbx project.
// *
// *  Cxbx and Cxbe are free software; you can redistribute them
// *  and/or modify them under the terms of the GNU General Public
// *  License as published by the Free Software Foundation; either
// *  version 2 of the license, or (at your option) any later version.
// *
// *  This program is distributed in the hope that it will be useful,
// *  but WITHOUT ANY WARRANTY; without even the implied warranty of
// *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// *  GNU General Public License for more details.
// *
// *  You should have recieved a copy of the GNU General Public License
// *  along with this program; see the file COPYING.
// *  If not, write to the Free Software Foundation, Inc.,
// *  59 Temple Place - Suite 330, Bostom, MA 02111-1307, USA.
// *
// *  2020 PatrickvL
// *
// *  All rights reserved
// *
// ******************************************************************

#define LOG_PREFIX CXBXR_MODULE::PXSH

#include <sstream> // std::stringstream
#include "Shader.h" // EmuCompileShader
#include "PixelShader.h" // EmuCompilePixelShader
//#include "core\kernel\init\CxbxKrnl.h"
//#include "core\kernel\support\Emu.h"

extern const char* g_ps_model = ps_model_3_0;

// HLSL pixel shader generation

static const std::string register_str[16+2] = {
	"_discard", // PS_REGISTER_DISCARD =  0x00L, // w
	"C0",       // PS_REGISTER_C0 =       0x01L, // r
	"C1",       // PS_REGISTER_C1 =       0x02L, // r
	"fog",      // PS_REGISTER_FOG =      0x03L, // r
	"v0",       // PS_REGISTER_V0 =       0x04L, // r/w
	"v1",       // PS_REGISTER_V1 =       0x05L, // r/w
	"?r6?",
	"?r7?",
	"t0",       // PS_REGISTER_T0 =       0x08L, // r/w
	"t1",       // PS_REGISTER_T1 =       0x09L, // r/w
	"t2",       // PS_REGISTER_T2 =       0x0aL, // r/w
	"t3",       // PS_REGISTER_T3 =       0x0bL, // r/w
	"r0",       // PS_REGISTER_R0 =       0x0cL, // r/w
	"r1",       // PS_REGISTER_R1 =       0x0dL, // r/w
	"sum",      // PS_REGISTER_V1R0_SUM = 0x0eL, // r
	"prod",     // PS_REGISTER_EF_PROD =  0x0fL, // r

	// Cxbx extension; Separate final combiner constant registers :
	"FC0",      // PS_REGISTER_FC0 =      0x10L, // r
	"FC1",      // PS_REGISTER_FC1 =      0x11L, // r
};

static const unsigned channel_index_Alpha = 0;
static const unsigned channel_index_RGB = 1;
static const unsigned channel_index_BlueToAlpha = 2; // Note : RGB pipeline (sometimes referred to as "portion") can (besides reading .rgb) expand blue to alpha as well

void InputRegisterHLSL(std::stringstream& hlsl, RPSInputRegister &input, unsigned channel_index, bool isLast = false, int isFinalCombiner = 0)
{
	static const std::string pipeline_channel_str[3][2] = {
		".b",    ".a",    // [0][*] dest Alpha           : [0] = PS_CHANNEL_BLUE, [1] = PS_CHANNEL_ALPHA >> 4
		".rgb",  ".aaa",  // [1][*] dest RGB             : [0] = PS_CHANNEL_RGB,  [1] = PS_CHANNEL_ALPHA >> 4
		".rgbb", ".aaaa", // [2][*] dest RGB+BlueToAlpha : [0] = PS_CHANNEL_RGB,  [1] = PS_CHANNEL_ALPHA >> 4 (test-case : TechCertGame) TODO : Verify .aaaa is indeed unreachable (BlueToAlpha being forbidden for Alpha channel
	};

	static const std::string input_mapping_str[8][3] = {
		// [*][0] = PS_REGISTER_ZERO-derived constants, based on enum PS_INPUTMAPPING :
		// [*][1] = Source register modifier macro's, based on enum PS_INPUTMAPPING :
		// [*][2] = Final combiner source register modifier macro's, based on enum PS_INPUTMAPPING :
		"zero",  "s_sat",     "s_sat",  //           saturate(x)        // PS_INPUTMAPPING_UNSIGNED_IDENTITY= 0x00L, OK for final combiner      // Clamps negative x to 0 
		"one",   "s_comp",    "s_comp", // ( 1.0   - saturate(x)      ) // PS_INPUTMAPPING_UNSIGNED_INVERT=   0x20L, OK for final combiner      // Complements x (1-x)
		"-one",  "s_bx2",     "N/A",    // ( 2.0 *   max(0.0, x) - 1.0) // PS_INPUTMAPPING_EXPAND_NORMAL=     0x40L, invalid for final combiner // Shifts range from [0..1] to [-1..1]
		"one",   "s_negbx2",  "N/A",    // (-2.0 *   max(0.0, x) + 1.0) // PS_INPUTMAPPING_EXPAND_NEGATE=     0x60L, invalid for final combiner // Shifts range from [0..1] to [-1..1] and then negates 
		"-half", "s_bias",    "N/A",    //          (max(0.0, x) - 0.5) // PS_INPUTMAPPING_HALFBIAS_NORMAL=   0x80L, invalid for final combiner // Clamps negative x to 0 and then subtracts 0.5 
		"half",  "s_negbias", "N/A",    //         (-max(0.0, x) + 0.5) // PS_INPUTMAPPING_HALFBIAS_NEGATE=   0xa0L, invalid for final combiner // Clamps negative x to 0, subtracts 0.5, and then negates
		"zero",  "s_ident",   "N/A",    //                    x         // PS_INPUTMAPPING_SIGNED_IDENTITY=   0xc0L, invalid for final combiner // No modifier, x is passed without alteration
		"zero",  "s_neg",     "N/A"     //                   -x         // PS_INPUTMAPPING_SIGNED_NEGATE=     0xe0L, invalid for final combiner // Negate
	};

	// Generate channel selector
	std::string channel_str = pipeline_channel_str[channel_index][input.Channel >> 4];

	unsigned input_mapping_index = (input.InputMapping >> 5) & 0x07; // Converts PS_INPUTMAPPING to an index into input_mapping_str
	if (input.Reg == PS_REGISTER_ZERO) { // = PS_REGISTER_DISCARD
		// Generate a constant per input mapping (instead of applying that on register_str[PS_REGISTER_DISCARD])
		hlsl << input_mapping_str[input_mapping_index][0] << channel_str;
	}
	else {
		// Or an actual register (with an input mapping function applied)
		switch (input_mapping_index) {
			case PS_INPUTMAPPING_SIGNED_IDENTITY >> 5:
				// Note : signed identity doesn't alter the argument, so avoid cluttering the output by leaving it out
				hlsl << register_str[input.Reg] << channel_str;
				break;
			case PS_INPUTMAPPING_SIGNED_NEGATE >> 5:
				// Note : signed negate can be written in short-hand using a minus sign
				hlsl << '-' << register_str[input.Reg] << channel_str;
				break;
			default:
				hlsl << input_mapping_str[input_mapping_index][1 + isFinalCombiner] << '(' << register_str[input.Reg] << channel_str << ')';
				break;
		}
	}

	if (!isLast)
		hlsl << ',';
}

static const std::string opcode_comment[6][2] = {
	"xdd",  "d0=s0 dot s1, d1=s2 dot s3",                        // dot/dot/discard > calculating AB=A.B and CD=C.D
	"xdm",  "d0=s0 dot s1, d1=s2*s3",                            // dot/mul/discard > calculating AB=A.B and CD=C*D
	"xmd",  "d0=s0*s1, d1=s2 dot s3",                            // mul/dot/discard > calculating AB=A*B and CD=C.D
	"xmma", "d0=s0*s1, d1=s2*s3, d2={s2*s3}+{s0*s1}",            // mul/mul/sum     > calculating AB=A*B and CD=C*D and Sum=CD+AB
	"xmmc", "d0=s0*s1, d1=s2*s3, d2={r0.a>0.5}?{s2*s3}:{s0*s1}", // mul/mul/mux     > calculating AB=A*B and CD=C*D and Mux=CD?AB
	"xfc",  "sum=r0+v1, prod=s4*s5, r0.rgb=s0*s1+{1-s0}*s2+s3, r0.a=s6.b"
};

void CombinerStageHlsl(std::stringstream& hlsl, RPSCombinerStageChannel& stage, unsigned channel_index)
{
	// Determine which opcode we're dealing with (xdd, xdm, xmma or xmma)
	// Note : DotProduct can only be set for RGB (see RPSCombinerStageChannel::Decode)
	unsigned opcode;
	if (stage.OutputAB.DotProduct) {
		if (stage.OutputCD.DotProduct)
			opcode = 0; // xdd
		else
			opcode = 1; // xdm
	} else {
		if (stage.OutputCD.DotProduct)
			opcode = 2; // xmd  TODO : Verify
		else
			if (!stage.AB_CD_MUX)
				opcode = 3; // xmma
			else
				opcode = 4; // xmmc
	}

	// Early exit when all outputs are discarded
	if ((stage.OutputAB.Reg == PS_REGISTER_DISCARD) && (stage.OutputCD.Reg == PS_REGISTER_DISCARD)) {
		// xdd, xdm and xmd have just 2 outputs, but xmma and xmmc must also check their 3rd output
		if ((opcode <= 2) || (stage.OutputMUX_SUM.Reg == PS_REGISTER_DISCARD)) {
			hlsl << "// discarded";
			return;
		}
	}

	// Determine output channels (only channel_index_RGB can increase to channel_index_BlueToAlpha) :
	static const std::string dst_channels[3] = { ".a", ".rgb", ".rgba" };
	unsigned AB_channel_index = channel_index + stage.OutputAB.BlueToAlpha;
	unsigned CD_channel_index = channel_index + stage.OutputCD.BlueToAlpha;

	// Generate 2 (or 3 for xmma/xmmc) output arguments
	// Note : BlueToAlpha can only be set for RGB (see RPSCombinerStageChannel::Decode)
	std::stringstream arguments;
	arguments <<        register_str[stage.OutputAB.Reg] << dst_channels[AB_channel_index];
	arguments << ',' << register_str[stage.OutputCD.Reg] << dst_channels[CD_channel_index];
	// xmma and xmmc have a 3rd output (which doesn't support the BlueToAlpha flag)
	if (opcode >= 3) {
		// TODO : Figure out how to support BlueToAlpha source to MUX_SUM.rgb scenario
		// If the xmma_m and xmmc_m macro's can't handle this, we may need to drop
		// those macro's, and generate the HLSL here (alas, as we try to avoid that).
		arguments << ',' << register_str[stage.OutputMUX_SUM.Reg] << dst_channels[channel_index];
	}
	// Insert a visual separation between the output arguments, and the 4 input arguments
	arguments << ", ";
	// Generate 4 input arguments
	InputRegisterHLSL(arguments, stage.OutputAB.Input[0], AB_channel_index);
	InputRegisterHLSL(arguments, stage.OutputAB.Input[1], AB_channel_index);
	InputRegisterHLSL(arguments, stage.OutputCD.Input[0], CD_channel_index);
	InputRegisterHLSL(arguments, stage.OutputCD.Input[1], CD_channel_index);


	// Generate combiner output modifier
	static const std::string output_modifier_str[8] = {
		"d_ident", // y =  x             // PS_COMBINEROUTPUT_OUTPUTMAPPING_IDENTITY=            0x00L
		"d_bias",  // y = (x - 0.5)      // PS_COMBINEROUTPUT_OUTPUTMAPPING_BIAS=                0x08L // Subtracts 0.5 from outputs
		"d_x2",    // y =  x        * 2  // PS_COMBINEROUTPUT_OUTPUTMAPPING_SHIFTLEFT_1=         0x10L // Scales outputs by 2
		"d_bx2",   // y = (x - 0.5) * 2  // PS_COMBINEROUTPUT_OUTPUTMAPPING_SHIFTLEFT_1_BIAS=    0x18L // Subtracts 0.5 from outputs and scales by 2
		"d_x4",    // y =  x        * 4  // PS_COMBINEROUTPUT_OUTPUTMAPPING_SHIFTLEFT_2=         0x20L // Scales outputs by 4
		"d_bx4",   // y = (x - 0.5) * 4  // PS_COMBINEROUTPUT_OUTPUTMAPPING_SHIFTLEFT_2_BIAS=    0x28L // Subtracts 0.5 from outputs and scales by 4
		"d_d2",    // y =  x        / 2  // PS_COMBINEROUTPUT_OUTPUTMAPPING_SHIFTRIGHT_1=        0x30L // Divides outputs by 2
		"d_bd2"    // y = (x - 0.5) / 2  // PS_COMBINEROUTPUT_OUTPUTMAPPING_SHIFTRIGHT_1_BIAS=   0x38L // Subtracts 0.5 from outputs and divides by 2
	};

	std::string output_modifier = output_modifier_str[stage.CombinerOutputMapping >> 3];

	// Concatenate it all together into an opcode 'call' (which resolves into macro expressions)
	hlsl << opcode_comment[opcode][0] << '(' << arguments.str() << ' ' << output_modifier;

	// xmma and xmmc require a temporary register with channel designation
	if (opcode >= 3)
		hlsl << ",tmp" << dst_channels[AB_channel_index]; // TODO : 

	hlsl << "); // " << opcode_comment[opcode][1];
}

void FinalCombinerStageHlsl(std::stringstream& hlsl, RPSFinalCombiner& fc, bool hasFinalCombiner)
{
	std::stringstream arguments;

	if (hasFinalCombiner) {
		hlsl << "\n    // Final combiner xfc was defined in the shader";
	}
	else {
		hlsl << "\n    // Final combiner xfc generated from XD3D8 renderstates";
	}

	for (unsigned i = 0; i < 7; i++) { // Generate A, B, C, D, E, F, G input arguments
		// Note : Most final combiner inputs are treated as RGB, but G is single-channel (.a or .b)
		bool isLast = (i == 6);
		unsigned channel_index = isLast ? channel_index_Alpha : channel_index_RGB;
		InputRegisterHLSL(arguments, fc.Input[i], channel_index, isLast, /*isFinalCombiner=*/1);
	}

	// Concatenate it all together into the xfc opcode 'call' (which resolves into macro expressions)
	// Note : The xfc opcode macro does not have an output modifier argument
	hlsl << "\n	" << opcode_comment[5][0] << "(" << arguments.str() << "); // " << opcode_comment[5][1];
}

void OutputDefineFlag(std::stringstream& hlsl, bool enabled, std::string_view define_enabled, std::string_view define_disabled = "")
{
	if (define_disabled.length() > 0) {
		if (enabled) {
			hlsl << "\n#define " << define_enabled << " // not " << define_disabled;
		}
		else {
			hlsl << "\n#define " << define_disabled << " // not " << define_enabled;
		}
	}
	else {
		if (enabled) {
			hlsl << "\n#define " << define_enabled;
		}
		else {
			hlsl << "\n#undef " << define_enabled;
		}
	}
}

/* Disabled, until BumpDemo is fixed (which with this code, inadvertedly skips stage 1 and 2 dotproducts) :
bool IsTextureSampled(DecodedRegisterCombiner* pShader, int reg)
{
	// TODO : Instead searching like this afterwards, simply set a boolean for each texture-read detected during decoding
	// TODO : Extend detection if textures can also be used indirectly thru PSInputTexture (without mention in actual combiner stages)
	for (unsigned i = 0; i < pShader->NumberOfCombiners; i++) {
		// Is an output calculated, and does any of the inputs read from the given (texture) register?
		if (pShader->Combiners[i].RGB.OutputAB.Reg != PS_REGISTER_DISCARD) {
			if (pShader->Combiners[i].RGB.OutputAB.Input[0].Reg == reg) return true;
			if (pShader->Combiners[i].RGB.OutputAB.Input[1].Reg == reg) return true;
		}
		if (pShader->Combiners[i].RGB.OutputCD.Reg != PS_REGISTER_DISCARD) {
			if (pShader->Combiners[i].RGB.OutputCD.Input[0].Reg == reg) return true;
			if (pShader->Combiners[i].RGB.OutputCD.Input[1].Reg == reg) return true;
		}
		if (pShader->Combiners[i].Alpha.OutputAB.Reg != PS_REGISTER_DISCARD) {
			if (pShader->Combiners[i].Alpha.OutputAB.Input[0].Reg == reg) return true;
			if (pShader->Combiners[i].Alpha.OutputAB.Input[1].Reg == reg) return true;
		}
		if (pShader->Combiners[i].Alpha.OutputCD.Reg != PS_REGISTER_DISCARD) {
			if (pShader->Combiners[i].Alpha.OutputCD.Input[0].Reg == reg) return true;
			if (pShader->Combiners[i].Alpha.OutputCD.Input[1].Reg == reg) return true;
		}
		// Is the given register writen to? Then no sampling took place
		if (pShader->Combiners[i].RGB.OutputAB.Reg == reg) return false;
		if (pShader->Combiners[i].RGB.OutputCD.Reg == reg) return false;
		if (pShader->Combiners[i].RGB.OutputMUX_SUM.Reg == reg) return false;
		if (pShader->Combiners[i].Alpha.OutputAB.Reg == reg) return false;
		if (pShader->Combiners[i].Alpha.OutputCD.Reg == reg) return false;
		if (pShader->Combiners[i].Alpha.OutputMUX_SUM.Reg == reg) return false;
	}

	if (pShader->hasFinalCombiner) {
		for (unsigned i = 0; i < 7; i++) {
			if (pShader->FinalCombiner.Input[i].Reg == reg) return true;
		}
	}

	return false;
} */

void BuildShader(DecodedRegisterCombiner* pShader, std::stringstream& hlsl)
{
	// Include HLSL header and footer as raw strings :
	static const std::string hlsl_template[4] = {
		#include "core\hle\D3D8\Direct3D9\CxbxPixelShaderTemplate.hlsl"
	};

	hlsl << hlsl_template[0]; // Start with the HLSL template header

	hlsl << "\n#define ALPHAKILL {"
		<< (pShader->AlphaKill[0] ? "true, " : "false, ")
		<< (pShader->AlphaKill[1] ? "true, " : "false, ")
		<< (pShader->AlphaKill[2] ? "true, " : "false, ")
		<< (pShader->AlphaKill[3] ? "true}"  : "false}");

	hlsl << "\n#define PS_COMBINERCOUNT " << pShader->NumberOfCombiners;
	if (pShader->NumberOfCombiners > 0) {
		OutputDefineFlag(hlsl, pShader->CombinerHasUniqueC0, "PS_COMBINERCOUNT_UNIQUE_C0", "PS_COMBINERCOUNT_SAME_C0");
		OutputDefineFlag(hlsl, pShader->CombinerHasUniqueC1, "PS_COMBINERCOUNT_UNIQUE_C1", "PS_COMBINERCOUNT_SAME_C1");
		OutputDefineFlag(hlsl, pShader->CombinerMuxesOnMsb, "PS_COMBINERCOUNT_MUX_MSB", "PS_COMBINERCOUNT_MUX_LSB");
	}

	for (unsigned i = 0; i < PSH_XBOX_MAX_T_REGISTER_COUNT; i++) {
		hlsl << "\n#define PS_COMPAREMODE_" << i  << "(in)"
			 << (pShader->PSCompareMode[i][0] ? " CM_GE(in.x)" : " CM_LT(in.x)") // PS_COMPAREMODE_S_[GE|LT]
			 << (pShader->PSCompareMode[i][1] ? " CM_GE(in.y)" : " CM_LT(in.y)") // PS_COMPAREMODE_T_[GE|LT]
			 << (pShader->PSCompareMode[i][2] ? " CM_GE(in.z)" : " CM_LT(in.z)") // PS_COMPAREMODE_R_[GE|LT]
			 << (pShader->PSCompareMode[i][3] ? " CM_GE(in.w)" : " CM_LT(in.w)");// PS_COMPAREMODE_Q_[GE|LT]
	}

	hlsl << "\nstatic const int PS_INPUTTEXTURE_[4] = { -1, "
		<< pShader->PSInputTexture[1] << ", "
		<< pShader->PSInputTexture[2] << ", "
		<< pShader->PSInputTexture[3] << " };";

	// Generate #defines required by CxbxPixelShaderTemplate.hlsl :
	for (unsigned i = 1; i < PSH_XBOX_MAX_T_REGISTER_COUNT; i++) {
		static const std::string dotmapping_str[8] = {
			"PS_DOTMAPPING_ZERO_TO_ONE",		 // = 0x00L, // - * * *
			"PS_DOTMAPPING_MINUS1_TO_1_D3D",	 // = 0x01L, // - * * *
			"PS_DOTMAPPING_MINUS1_TO_1_GL",		 // = 0x02L, // - * * *
			"PS_DOTMAPPING_MINUS1_TO_1",		 // = 0x03L, // - * * *
			"PS_DOTMAPPING_HILO_1",				 // = 0x04L, // - * * *
			"PS_DOTMAPPING_HILO_HEMISPHERE_D3D", // = 0x05L, // - * * *
			"PS_DOTMAPPING_HILO_HEMISPHERE_GL",  // = 0x06L, // - * * *
			"PS_DOTMAPPING_HILO_HEMISPHERE"		 // = 0x07L, // - * * *
		};

		hlsl << "\n#define PS_DOTMAPPING_" << i << " " << dotmapping_str[(unsigned)pShader->PSDotMapping[i]];
	}

	OutputDefineFlag(hlsl, pShader->FinalCombiner.ComplementV1, "PS_FINALCOMBINERSETTING_COMPLEMENT_V1");
	OutputDefineFlag(hlsl, pShader->FinalCombiner.ComplementR0, "PS_FINALCOMBINERSETTING_COMPLEMENT_R0");
	OutputDefineFlag(hlsl, pShader->FinalCombiner.ClampSum, "PS_FINALCOMBINERSETTING_CLAMP_SUM");

	hlsl << hlsl_template[1];
	hlsl << hlsl_template[2];

	// Generate all four texture stages
	for (unsigned i = 0; i < PSH_XBOX_MAX_T_REGISTER_COUNT; i++) {
		static const std::string texturemode_str[19] = {
			"PS_TEXTUREMODES_NONE",                 // = 0x00L, // * * * *
			"PS_TEXTUREMODES_PROJECT2D",            // = 0x01L, // * * * *
			"PS_TEXTUREMODES_PROJECT3D",            // = 0x02L, // * * * *
			"PS_TEXTUREMODES_CUBEMAP",              // = 0x03L, // * * * *
			"PS_TEXTUREMODES_PASSTHRU",             // = 0x04L, // * * * *
			"PS_TEXTUREMODES_CLIPPLANE",            // = 0x05L, // * * * *
			"PS_TEXTUREMODES_BUMPENVMAP",           // = 0x06L, // - * * *
			"PS_TEXTUREMODES_BUMPENVMAP_LUM",       // = 0x07L, // - * * *
			"PS_TEXTUREMODES_BRDF",                 // = 0x08L, // - - * *
			"PS_TEXTUREMODES_DOT_ST",               // = 0x09L, // - - * *
			"PS_TEXTUREMODES_DOT_ZW",               // = 0x0aL, // - - * *
			"PS_TEXTUREMODES_DOT_RFLCT_DIFF",       // = 0x0bL, // - - * -
			"PS_TEXTUREMODES_DOT_RFLCT_SPEC",       // = 0x0cL, // - - - *
			"PS_TEXTUREMODES_DOT_STR_3D",           // = 0x0dL, // - - - *
			"PS_TEXTUREMODES_DOT_STR_CUBE",         // = 0x0eL, // - - - *
			"PS_TEXTUREMODES_DPNDNT_AR",            // = 0x0fL, // - * * *
			"PS_TEXTUREMODES_DPNDNT_GB",            // = 0x10L, // - * * *
			"PS_TEXTUREMODES_DOTPRODUCT",           // = 0x11L, // - * * -
			"PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST", // = 0x12L, // - - - *
		};

		/* Disabled, until BumpDemo is fixed (which with this code, inadvertedly skips stage 1 and 2 dotproducts) :
		// Skip stages never read, to avoid compilation overhead
		if (!IsTextureSampled(pShader, PS_REGISTER_T0 + i))
			continue; */

		hlsl << "\n	" << texturemode_str[pShader->PSTextureModes[i]] << "(" << i << ");";
		// On Xbox, r0.a is initialized to t0.a (and r1.a to t1.a ?) :
		if (i == 0) hlsl << " r0.a = t0.a;";
		if (i == 1) hlsl << " r1.a = t1.a;";
	}

	// Generate all combiners (rgb and alpha)
	for (unsigned i = 0; i < pShader->NumberOfCombiners; i++) {
		hlsl << "\n	stage = " << i << "; ";
		CombinerStageHlsl(hlsl, pShader->Combiners[i].RGB, channel_index_RGB);
		hlsl << "\n	/*    + */ ";
		CombinerStageHlsl(hlsl, pShader->Combiners[i].Alpha, channel_index_Alpha);
	}

	FinalCombinerStageHlsl(hlsl, pShader->FinalCombiner, pShader->hasFinalCombiner);

	hlsl << hlsl_template[3]; // Finish with the HLSL template footer
}

// recompile xbox pixel shader function
extern HRESULT EmuCompilePixelShader
(
	DecodedRegisterCombiner* pIntermediateShader,
	ID3DBlob** ppHostShader
)
{
	auto hlsl_stream = std::stringstream();
	BuildShader(pIntermediateShader, hlsl_stream);
	std::string hlsl_str = hlsl_stream.str();

	return EmuCompileShader(hlsl_str, g_ps_model, ppHostShader);
}
