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
// *  All rights reserved
// *
// ******************************************************************
#pragma once

#include <atomic>
#include <string> // std::string
#include <d3dcompiler.h> // ID3DBlob (via d3d9.h > d3d11shader.h > d3dcommon.h)

extern HRESULT EmuCompileShader
(
	std::string hlsl_str,
	const char* shader_profile,
	ID3DBlob** ppHostShader,
	const char* pSourceName = nullptr
);

struct ShaderSources {
	// Pixel Shader
	std::string pixelShaderTemplateHlsl[3];
	std::string pixelShaderTemplatePath;

	std::string fixedFunctionPixelShaderHlsl;
	std::string fixedFunctionPixelShaderPath;

	// Vertex Shader
	std::string vertexShaderTemplateHlsl[2];

	std::string fixedFunctionVertexShaderHlsl;
	std::string fixedFunctionVertexShaderPath;

	std::string vertexShaderPassthroughHlsl;
	std::string vertexShaderPassthroughPath;

	// Load shaders from disk (if out-of-date)
	// and return the current loaded shader version
	int Update();

	// Start a thread to watch for changes in the shader folder
	void InitShaderHotloading();

private:
	void LoadShadersFromDisk();

	// counts upwards on every change detected to the shader source files at runtime
	std::atomic_int shaderVersionOnDisk = 0;
	// current loaded shader version
	// Initialized to < shaderVersionOnDisk
	int shaderVersionLoadedFromDisk = -1;
};

extern ShaderSources g_ShaderSources;
