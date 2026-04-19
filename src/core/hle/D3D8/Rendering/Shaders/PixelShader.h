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

#include "Shader.h" // ID3DBlob (via d3dcompiler.h > d3d11shader.h > d3dcommon.h)
#include "core\hle\D3D8\XbPixelShader.h" // DecodedRegisterCombiner

static const char* ps_model_2_a = "ps_2_a";
static const char* ps_model_3_0 = "ps_3_0";
static const char* ps_model_4_0 = "ps_4_0";  // For D3D11
static const char* ps_model_5_0 = "ps_5_0";  // For D3D11 with feature level 11.0+
extern const char* g_ps_model;

extern HRESULT EmuCompilePixelShader
(
   	DecodedRegisterCombiner* pIntermediateShader,
   	ID3DBlob** ppHostShader
);
