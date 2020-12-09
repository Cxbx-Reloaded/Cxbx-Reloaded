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

#define LOG_PREFIX CXBXR_MODULE::VTXSH // TODO : Introduce generic HLSL logging

#include <d3dcompiler.h>
#include "Shader.h"
#include "core\kernel\init\CxbxKrnl.h" // LOG_TEST_CASE
#include "core\kernel\support\Emu.h" // EmuLog
//#include <sstream>

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

extern HRESULT EmuCompileShader
(
	std::string hlsl_str,
	const char* shader_profile,
	ID3DBlob** ppHostShader,
	const char* pSourceName
)
{
	ID3DBlob* pErrors = nullptr;
	ID3DBlob* pErrorsCompatibility = nullptr;
	HRESULT             hRet = 0;

	EmuLog(LOG_LEVEL::DEBUG, "--- HLSL conversion ---");
	EmuLog(LOG_LEVEL::DEBUG, DebugPrependLineNumbers(hlsl_str).c_str());
	EmuLog(LOG_LEVEL::DEBUG, "-----------------------");


	UINT flags1 = D3DCOMPILE_OPTIMIZATION_LEVEL3;

	hRet = D3DCompile(
		hlsl_str.c_str(),
		hlsl_str.length(),
		pSourceName,
		nullptr, // pDefines
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // pInclude // TODO precompile x_* HLSL functions?
		"main", // shader entry poiint
		shader_profile,
		flags1, // flags1
		0, // flags2
		ppHostShader, // out
		&pErrors // ppErrorMsgs out
	);
	if (FAILED(hRet)) {
		EmuLog(LOG_LEVEL::WARNING, "Shader compile failed. Recompiling in compatibility mode");
		// Attempt to retry in compatibility mode, this allows some vertex-state shaders to compile
		// Test Case: Spy vs Spy
		flags1 |= D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY | D3DCOMPILE_AVOID_FLOW_CONTROL;
		hRet = D3DCompile(
			hlsl_str.c_str(),
			hlsl_str.length(),
			pSourceName,
			nullptr, // pDefines
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // pInclude // TODO precompile x_* HLSL functions?
			"main", // shader entry poiint
			shader_profile,
			flags1, // flags1
			0, // flags2
			ppHostShader, // out
			&pErrorsCompatibility // ppErrorMsgs out
		);

		if (FAILED(hRet)) {
			LOG_TEST_CASE("Couldn't assemble recompiled shader");
			//EmuLog(LOG_LEVEL::WARNING, "Couldn't assemble recompiled shader");
		}
	}

	// Determine the log level
	auto hlslErrorLogLevel = FAILED(hRet) ? LOG_LEVEL::ERROR2 : LOG_LEVEL::DEBUG;
	if (pErrors) {
		// Log errors from the initial compilation
		EmuLog(hlslErrorLogLevel, "%s", (char*)(pErrors->GetBufferPointer()));
		pErrors->Release();
		pErrors = nullptr;
	}

	// Failure to recompile in compatibility mode ignored for now
	if (pErrorsCompatibility != nullptr) {
		pErrorsCompatibility->Release();
		pErrorsCompatibility = nullptr;
	}

	LOG_CHECK_ENABLED(LOG_LEVEL::DEBUG) {
		if (g_bPrintfOn) {
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
		}
	}

	return hRet;
}
