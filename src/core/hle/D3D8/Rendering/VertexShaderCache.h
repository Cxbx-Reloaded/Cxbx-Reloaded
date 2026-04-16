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

#ifndef DIRECT3D9SHADERCACHE_H
#define DIRECT3D9SHADERCACHE_H

#include "VertexShader.h"
#include <map>

typedef uint64_t ShaderKey;

// Manages creation and caching of vertex shaders
class VertexShaderCache {

public:
	ShaderKey CreateShader(const xbox::dword_xt* pXboxFunction, DWORD* pXboxFunctionSize);
	IDirect3DVertexShader *GetShader(ShaderKey key);
#ifdef CXBX_USE_D3D11
	ID3DBlob* GetShaderBytecode(ShaderKey key);
#endif
	void ReleaseShader(ShaderKey key);

	void Clear();
	// TODO
	// WriteCacheToDisk
	// LoadCacheFromDisk

private:
	struct LazyVertexShader {
		bool isReady = false;
		std::future<ID3DBlob*> compileResult;
		IDirect3DVertexShader* pHostVertexShader = nullptr;
#ifdef CXBX_USE_D3D11
		ID3DBlob* pBytecode = nullptr; // Kept alive for input layout creation
#endif

		// TODO when is it a good idea to releas eshaders?
		int referenceCount = 0;

		// TODO persist shaders to disk
		// ShaderVersion?
		// OptimizationLevel?
	};

	std::mutex cacheMutex;
	std::map<ShaderKey, LazyVertexShader> cache;

	bool _FindShader(ShaderKey key, LazyVertexShader** ppLazyShader);
};

extern VertexShaderCache g_VertexShaderCache;

#endif
