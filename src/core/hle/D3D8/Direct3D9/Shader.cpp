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
#include "common/FilePaths.hpp" // For szFilePath_CxbxReloaded_Exe
#include "core\kernel\init\CxbxKrnl.h" // LOG_TEST_CASE
#include "core\kernel\support\Emu.h" // EmuLog

#include <filesystem>
#include <fstream>
#include <array>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include "common\util\hasher.h" // For ComputeHash
//#include <sstream>

ShaderSources g_ShaderSources;

// ============================================================================
// Disk-based shader bytecode cache
// ============================================================================

// Shader bytecode magic validation
// D3D9 SM1-3: vertex shaders start with 0xFFFExxxx, pixel shaders with 0xFFFFxxxx
// D3D10+ SM4+: DXBC container starts with 0x43425844 ("DXBC")
static bool IsValidShaderBytecode(uint32_t magic)
{
	return (magic >> 16) == 0xFFFE  // D3D9 vertex shader
	    || (magic >> 16) == 0xFFFF  // D3D9 pixel shader
	    || magic == 0x43425844;     // DXBC container (SM4+)
}

static std::string g_ShaderCacheDir;
static std::atomic<int> g_CacheHits{0};
static std::atomic<int> g_CacheMisses{0};
static std::atomic<int> g_CacheSaves{0};
static std::atomic<int> g_CacheLoadErrors{0};

// Log file for shader cache (since emulation process may not have a console)
static FILE* g_ShaderCacheLogFile = nullptr;
static std::mutex g_LogMutex;

static void ShaderCacheLog(const char* fmt, ...)
{
	if (!g_ShaderCacheLogFile) return;
	std::lock_guard<std::mutex> lock(g_LogMutex);
	va_list args;
	va_start(args, fmt);
	vfprintf(g_ShaderCacheLogFile, fmt, args);
	va_end(args);
	fflush(g_ShaderCacheLogFile);
}

// Background save queue
static std::mutex g_SaveQueueMutex;
static std::queue<std::pair<std::string, std::vector<uint8_t>>> g_SaveQueue;
static std::thread g_SaveThread;
static std::atomic<bool> g_SaveThreadRunning{false};

static void ShaderCacheSaveWorker()
{
	while (g_SaveThreadRunning) {
		std::pair<std::string, std::vector<uint8_t>> item;
		bool hasItem = false;
		{
			std::lock_guard<std::mutex> lock(g_SaveQueueMutex);
			if (!g_SaveQueue.empty()) {
				item = std::move(g_SaveQueue.front());
				g_SaveQueue.pop();
				hasItem = true;
			}
		}

		if (!hasItem) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}

		FILE* fp = fopen(item.first.c_str(), "wb");
		if (fp) {
			fwrite(item.second.data(), 1, item.second.size(), fp);
			fclose(fp);
			g_CacheSaves++;
			ShaderCacheLog("SAVED %s (%zu bytes)\n", item.first.c_str(), item.second.size());
		} else {
			ShaderCacheLog("ERROR could not write %s (errno=%d)\n", item.first.c_str(), errno);
		}
	}
}

// Returns true if cache dir is ready to use
static bool EnsureShaderCacheDir()
{
	if (!g_ShaderCacheDir.empty()) return true;

	// g_DataFilePath may not be set yet during early init
	if (g_DataFilePath.empty()) return false;

	// Need game certificate to create per-game directory
	if (!g_pCertificate) return false;

	// Build per-game cache dir: ShaderCache\<TitleID>-<GameName>
	// e.g. ShaderCache\4D530004-Halo
	char titleIdStr[16];
	snprintf(titleIdStr, sizeof(titleIdStr), "%08X", g_pCertificate->dwTitleId);

	// Get ASCII game title and sanitize for filesystem use
	std::string gameName;
	if (CxbxKrnl_Xbe && CxbxKrnl_Xbe->m_szAsciiTitle[0]) {
		gameName = CxbxKrnl_Xbe->m_szAsciiTitle;
		// Remove characters invalid in directory names
		for (char& c : gameName) {
			if (c == '\\' || c == '/' || c == ':' || c == '*' ||
				c == '?' || c == '"' || c == '<' || c == '>' || c == '|')
				c = '_';
		}
		// Trim trailing spaces
		while (!gameName.empty() && gameName.back() == ' ')
			gameName.pop_back();
	}

	std::string gameDir = std::string(titleIdStr);
	if (!gameName.empty()) {
		gameDir += "-" + gameName;
	}

	g_ShaderCacheDir = g_DataFilePath + "\\ShaderCache\\" + gameDir;
	std::error_code ec;
	if (!std::filesystem::exists(g_ShaderCacheDir)) {
		std::filesystem::create_directories(g_ShaderCacheDir, ec);
		if (ec) {
			// Failed to create — reset so we retry next time
			g_ShaderCacheDir.clear();
			return false;
		}
	}

	// Open log file in the per-game shader cache dir
	std::string logPath = g_ShaderCacheDir + "\\shader_cache.log";
	g_ShaderCacheLogFile = fopen(logPath.c_str(), "wt");
	ShaderCacheLog("ShaderCache initialized: %s\n", g_ShaderCacheDir.c_str());
	ShaderCacheLog("g_DataFilePath = %s\n", g_DataFilePath.c_str());
	ShaderCacheLog("TitleID = %s, GameName = %s\n", titleIdStr, gameName.c_str());

	// Start background save thread
	if (!g_SaveThreadRunning) {
		g_SaveThreadRunning = true;
		g_SaveThread = std::thread(ShaderCacheSaveWorker);
		g_SaveThread.detach();
	}

	return true;
}

static std::string GetShaderCachePath(uint64_t hash)
{
	char filename[32];
	snprintf(filename, sizeof(filename), "%016llx.cso", hash);
	return g_ShaderCacheDir + "\\" + filename;
}

static bool LoadCachedShader(uint64_t hash, ID3DBlob** ppBlob)
{
	std::string path = GetShaderCachePath(hash);
	FILE* fp = fopen(path.c_str(), "rb");
	if (!fp) return false;

	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (size < 8) {
		// Minimum: version token (4 bytes) + end token (4 bytes)
		ShaderCacheLog("REJECT %s (too small: %ld bytes)\n", path.c_str(), size);
		fclose(fp);
		g_CacheLoadErrors++;
		return false;
	}

	// Read the magic bytes first to validate
	uint32_t magic = 0;
	fread(&magic, 4, 1, fp);
	if (!IsValidShaderBytecode(magic)) {
		ShaderCacheLog("REJECT %s (bad magic: 0x%08X)\n", path.c_str(), magic);
		fclose(fp);
		g_CacheLoadErrors++;
		// Delete corrupt cache file
		std::error_code ec;
		std::filesystem::remove(path, ec);
		return false;
	}
	fseek(fp, 0, SEEK_SET);

	HRESULT hr = D3DCreateBlob(size, ppBlob);
	if (FAILED(hr)) {
		ShaderCacheLog("ERROR D3DCreateBlob failed for %s (size=%ld, hr=0x%08lX)\n", path.c_str(), size, hr);
		fclose(fp);
		g_CacheLoadErrors++;
		return false;
	}

	size_t readBytes = fread((*ppBlob)->GetBufferPointer(), 1, size, fp);
	fclose(fp);

	if ((long)readBytes != size) {
		ShaderCacheLog("ERROR partial read %s (%zu / %ld bytes)\n", path.c_str(), readBytes, size);
		(*ppBlob)->Release();
		*ppBlob = nullptr;
		g_CacheLoadErrors++;
		return false;
	}

	g_CacheHits++;
	ShaderCacheLog("HIT %016llx (%ld bytes) [hits=%d misses=%d]\n", hash, size, g_CacheHits.load(), g_CacheMisses.load());
	return true;
}

static void QueueSaveCachedShader(uint64_t hash, ID3DBlob* pBlob)
{
	// Validate the blob has DXBC magic before saving
	if (pBlob->GetBufferSize() < 4) {
		ShaderCacheLog("SKIP save %016llx (blob too small: %zu bytes)\n", hash, pBlob->GetBufferSize());
		return;
	}

	uint32_t magic = *reinterpret_cast<const uint32_t*>(pBlob->GetBufferPointer());
	if (!IsValidShaderBytecode(magic)) {
		ShaderCacheLog("SKIP save %016llx (bad magic: 0x%08X)\n", hash, magic);
		return;
	}

	std::string path = GetShaderCachePath(hash);
	std::vector<uint8_t> data(
		static_cast<uint8_t*>(pBlob->GetBufferPointer()),
		static_cast<uint8_t*>(pBlob->GetBufferPointer()) + pBlob->GetBufferSize()
	);

	{
		std::lock_guard<std::mutex> lock(g_SaveQueueMutex);
		g_SaveQueue.push({path, std::move(data)});
	}
	ShaderCacheLog("QUEUED save %016llx (%zu bytes)\n", hash, pBlob->GetBufferSize());
}

// ============================================================================
// Async shader compilation (Dolphin-style)
// ============================================================================

// In-memory cache of compiled shaders (from async completions)
static std::mutex g_AsyncMutex;
static std::unordered_map<uint64_t, ID3DBlob*> g_AsyncResults;
static std::unordered_set<uint64_t> g_AsyncInFlight;

// Precompiled fallback pixel shader (simple white output)
static ID3DBlob* g_FallbackPSBlob = nullptr;
static bool g_FallbacksInitialized = false;

static void EnsureFallbackShaders()
{
	if (g_FallbacksInitialized) return;
	g_FallbacksInitialized = true;

	// Minimal pixel shader: output white (only PS uses async fallback)
	const char* psSrc =
		"float4 main() : COLOR0 { return float4(1,1,1,1); }\n";
	D3DCompile(psSrc, strlen(psSrc), nullptr, nullptr, nullptr,
		"main", "ps_3_0", D3DCOMPILE_OPTIMIZATION_LEVEL0, 0, &g_FallbackPSBlob, nullptr);

	ShaderCacheLog("Fallback PS compiled: %p (%zu bytes)\n",
		g_FallbackPSBlob, g_FallbackPSBlob ? g_FallbackPSBlob->GetBufferSize() : 0);
}

// Background compile worker — runs real D3DCompile and stores result
static void AsyncCompileWorker(std::string hlsl_str, std::string profile,
	std::string sourceName, uint64_t hash, bool cacheReady)
{
	auto tStart = std::chrono::high_resolution_clock::now();

	ID3DBlob* pResult = nullptr;
	ID3DBlob* pErrors = nullptr;
	UINT flags1 = D3DCOMPILE_OPTIMIZATION_LEVEL3;

	HRESULT hr = D3DCompile(
		hlsl_str.c_str(), hlsl_str.length(),
		sourceName.empty() ? nullptr : sourceName.c_str(),
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", profile.c_str(), flags1, 0, &pResult, &pErrors);

	if (FAILED(hr)) {
		if (pErrors) { pErrors->Release(); pErrors = nullptr; }
		flags1 |= D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY | D3DCOMPILE_AVOID_FLOW_CONTROL;
		hr = D3DCompile(
			hlsl_str.c_str(), hlsl_str.length(),
			sourceName.empty() ? nullptr : sourceName.c_str(),
			nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main", profile.c_str(), flags1, 0, &pResult, &pErrors);
	}

	if (pErrors) { pErrors->Release(); pErrors = nullptr; }

	auto tEnd = std::chrono::high_resolution_clock::now();
	double ms = std::chrono::duration<double, std::milli>(tEnd - tStart).count();

	{
		std::lock_guard<std::mutex> lock(g_AsyncMutex);
		g_AsyncInFlight.erase(hash);
		if (!FAILED(hr) && pResult) {
			g_AsyncResults[hash] = pResult; // takes ownership
			if (cacheReady) {
				QueueSaveCachedShader(hash, pResult);
			}
			ShaderCacheLog("ASYNC COMPILED %016llx in %.2f ms (profile=%s, blob=%zu bytes)\n",
				hash, ms, profile.c_str(), pResult->GetBufferSize());
		} else {
			ShaderCacheLog("ASYNC COMPILE FAILED %016llx in %.2f ms (profile=%s, hr=0x%08lX)\n",
				hash, ms, profile.c_str(), hr);
		}
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

extern HRESULT EmuCompileShader
(
	std::string hlsl_str,
	const char* shader_profile,
	ID3DBlob** ppHostShader,
	const char* pSourceName,
	bool asyncAllowed
)
{
	// Compute a cache key from the HLSL source + shader profile
	bool cacheReady = EnsureShaderCacheDir();
	std::string cacheInput = hlsl_str + "|" + shader_profile;
	uint64_t cacheHash = ComputeHash(cacheInput.c_str(), cacheInput.size());

	// 1. Try loading from disk cache first (fast — ~0.08ms)
	if (cacheReady) {
		auto t0 = std::chrono::high_resolution_clock::now();
		if (LoadCachedShader(cacheHash, ppHostShader)) {
			auto t1 = std::chrono::high_resolution_clock::now();
			double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
			ShaderCacheLog("LOAD took %.2f ms (profile=%s)\n", ms, shader_profile);
			return S_OK;
		}
	}

	// 2. Async pixel shader path (only for PS, not VS)
	if (asyncAllowed && shader_profile[0] == 'p') {
		// Ensure fallback shader is ready (outside mutex — D3DCompile is slow first time)
		EnsureFallbackShaders();

		std::lock_guard<std::mutex> lock(g_AsyncMutex);

		// Check in-memory async results (completed background compiles)
		auto it = g_AsyncResults.find(cacheHash);
		if (it != g_AsyncResults.end()) {
			// Async compilation finished — return the real shader
			it->second->AddRef();
			*ppHostShader = it->second;
			ShaderCacheLog("ASYNC HIT %016llx (profile=%s)\n", cacheHash, shader_profile);
			return S_OK;
		}

		if (g_AsyncInFlight.count(cacheHash)) {
			// Already compiling in background — return fallback
			if (g_FallbackPSBlob) {
				g_FallbackPSBlob->AddRef();
				*ppHostShader = g_FallbackPSBlob;
				ShaderCacheLog("ASYNC PENDING %016llx -> fallback (profile=%s)\n", cacheHash, shader_profile);
				return S_FALSE;
			}
			// If no fallback available, fall through to synchronous compile
		} else {
			// Start background compile
			g_AsyncInFlight.insert(cacheHash);

			std::string profileStr = shader_profile;
			std::string sourceStr = pSourceName ? pSourceName : "";
			std::thread(AsyncCompileWorker, hlsl_str, profileStr, sourceStr, cacheHash, cacheReady).detach();

			// Return fallback shader
			if (g_FallbackPSBlob) {
				g_FallbackPSBlob->AddRef();
				*ppHostShader = g_FallbackPSBlob;
				ShaderCacheLog("ASYNC START %016llx -> fallback (profile=%s)\n", cacheHash, shader_profile);
				return S_FALSE;
			}
			// If no fallback (shouldn't happen), fall through to synchronous
			g_AsyncInFlight.erase(cacheHash);
		}
	} else if (asyncAllowed) {
		// VS path: check async results in case a previous async compile for this hash finished
		std::lock_guard<std::mutex> lock(g_AsyncMutex);
		auto it = g_AsyncResults.find(cacheHash);
		if (it != g_AsyncResults.end()) {
			it->second->AddRef();
			*ppHostShader = it->second;
			ShaderCacheLog("ASYNC HIT %016llx (profile=%s)\n", cacheHash, shader_profile);
			return S_OK;
		}
	}

	// 3. Synchronous compilation (for vertex shaders or when async fallback isn't available)
	ID3DBlob* pErrors = nullptr;
	ID3DBlob* pErrorsCompatibility = nullptr;
	HRESULT             hRet = 0;

	EmuLog(LOG_LEVEL::DEBUG, "--- HLSL conversion ---");
	EmuLog(LOG_LEVEL::DEBUG, DebugPrependLineNumbers(hlsl_str).c_str());
	EmuLog(LOG_LEVEL::DEBUG, "-----------------------");


	auto tCompileStart = std::chrono::high_resolution_clock::now();
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

	auto tCompileEnd = std::chrono::high_resolution_clock::now();
	double compileMs = std::chrono::duration<double, std::milli>(tCompileEnd - tCompileStart).count();
	HRESULT compileResult = hRet; // Preserve the actual compile result

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
			if (!FAILED(compileResult)) {
				// Log disassembly — use a separate HRESULT so we don't clobber compileResult
				HRESULT hDisasm = D3DDisassemble(
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

	// Save successfully compiled shader to disk cache (async — queued to background thread)
	if (!FAILED(compileResult) && *ppHostShader && cacheReady) {
		g_CacheMisses++;
		ShaderCacheLog("SYNC MISS %016llx compile took %.2f ms (profile=%s, blob=%zu bytes) [hits=%d misses=%d]\n",
			cacheHash, compileMs, shader_profile,
			(*ppHostShader)->GetBufferSize(),
			g_CacheHits.load(), g_CacheMisses.load());
		QueueSaveCachedShader(cacheHash, *ppHostShader);
	} else if (FAILED(compileResult)) {
		ShaderCacheLog("COMPILE FAILED hash=%016llx profile=%s hr=0x%08lX\n",
			cacheHash, shader_profile, compileResult);
	} else if (!cacheReady) {
		ShaderCacheLog("SKIP (cache not ready, g_DataFilePath='%s') hash=%016llx\n",
			g_DataFilePath.c_str(), cacheHash);
	}

	return compileResult;
}

std::ifstream OpenWithRetry(const std::string& path) {
	auto fstream = std::ifstream(path);
	int failures = 0;
	while (fstream.fail()) {
		Sleep(50);
		fstream = std::ifstream(path);

		if (failures++ > 10) {
			// crash?
			CxbxrAbort("Error opening shader file: %s", path);
			break;
		}
	}

	return fstream;
}

int ShaderSources::Update() {
	int versionOnDisk = shaderVersionOnDisk;
	if (shaderVersionLoadedFromDisk != versionOnDisk) {
		LoadShadersFromDisk();
		shaderVersionLoadedFromDisk = versionOnDisk;

		// Invalidate disk shader cache when HLSL templates change
		if (!g_ShaderCacheDir.empty() && std::filesystem::exists(g_ShaderCacheDir)) {
			std::error_code ec;
			std::filesystem::remove_all(g_ShaderCacheDir, ec);
			std::filesystem::create_directories(g_ShaderCacheDir, ec);
		}
	}

	return shaderVersionLoadedFromDisk;
}

void ShaderSources::LoadShadersFromDisk() {
	const auto hlslDir = std::filesystem::path(szFilePath_CxbxReloaded_Exe)
		.parent_path()
		.append("hlsl");

	// Pixel Shader Template
	{
		std::stringstream tmp;
		auto dir = hlslDir;
		dir.append("CxbxPixelShaderTemplate.hlsl");
		tmp << OpenWithRetry(dir.string()).rdbuf();
		std::string hlsl = tmp.str();

		// Split the HLSL file on insertion points
		std::array<std::string, 2> insertionPoints = {
			"// <HARDCODED STATE GOES HERE>\n",
			"// <XBOX SHADER PROGRAM GOES HERE>\n",
		};
		int pos = 0;
		for (int i = 0; i < insertionPoints.size(); i++) {
			auto insertionPoint = insertionPoints[i];
			auto index = hlsl.find(insertionPoint, pos);

			if (index == std::string::npos) {
				// Handle broken shaders
				this->pixelShaderTemplateHlsl[i] = "";
			}
			else {
				this->pixelShaderTemplateHlsl[i] = hlsl.substr(pos, index - pos);
				pos = index + insertionPoint.length();
			}
		}
		this->pixelShaderTemplateHlsl[insertionPoints.size()] = hlsl.substr(pos);
	}

	// Fixed Function Pixel Shader
	{
		auto dir = hlslDir;
		this->fixedFunctionPixelShaderPath = dir.append("FixedFunctionPixelShader.hlsl").string();
		std::stringstream tmp;
		tmp << OpenWithRetry(this->fixedFunctionPixelShaderPath).rdbuf();
		this->fixedFunctionPixelShaderHlsl = tmp.str();
	}

	// Vertex Shader Template
	{
		std::stringstream tmp;
		auto dir = hlslDir;
		dir.append("CxbxVertexShaderTemplate.hlsl");
		tmp << OpenWithRetry(dir.string()).rdbuf();
		std::string hlsl = tmp.str();

		const std::string insertionPoint = "// <XBOX SHADER PROGRAM GOES HERE>\n";
		auto index = hlsl.find(insertionPoint);

		if (index == std::string::npos) {
			// Handle broken shaders
			this->vertexShaderTemplateHlsl[0] = hlsl;
			this->vertexShaderTemplateHlsl[1] = "";
		}
		else
		{
			this->vertexShaderTemplateHlsl[0] = hlsl.substr(0, index);
			this->vertexShaderTemplateHlsl[1] = hlsl.substr(index + insertionPoint.length());
		}
	}

	// Fixed Function Vertex Shader
	{
		auto dir = hlslDir;
		this->fixedFunctionVertexShaderPath = dir.append("FixedFunctionVertexShader.hlsl").string();
		std::stringstream tmp;
		tmp << OpenWithRetry(this->fixedFunctionVertexShaderPath).rdbuf();
		this->fixedFunctionVertexShaderHlsl = tmp.str();
	}

	// Passthrough Vertex Shader
	{
		auto dir = hlslDir;
		this->vertexShaderPassthroughPath = dir.append("CxbxVertexShaderPassthrough.hlsl").string();
		std::stringstream tmp;
		tmp << OpenWithRetry(this->vertexShaderPassthroughPath).rdbuf();
		this->vertexShaderPassthroughHlsl = tmp.str();
	}
}

void ShaderSources::InitShaderHotloading() {
	static std::jthread fsWatcherThread;

	if (fsWatcherThread.joinable()) {
		EmuLog(LOG_LEVEL::ERROR2, "Ignoring request to start shader file watcher - it has already been started.");
		return;
	}

	EmuLog(LOG_LEVEL::DEBUG, "Starting shader file watcher...");

	fsWatcherThread = std::jthread([]{
		// Determine the filename and directory for the fixed function shader
		char cxbxExePath[MAX_PATH];
		GetModuleFileName(GetModuleHandle(nullptr), cxbxExePath, MAX_PATH);
		auto hlslDir = std::filesystem::path(cxbxExePath).parent_path().append("hlsl/");

		HANDLE changeHandle = FindFirstChangeNotification(hlslDir.string().c_str(), false, FILE_NOTIFY_CHANGE_LAST_WRITE);

		if (changeHandle == INVALID_HANDLE_VALUE) {
			DWORD errorCode = GetLastError();
			EmuLog(LOG_LEVEL::ERROR2, "Error initializing shader file watcher: %d", errorCode);

			return 1;
		}

		while (true) {
			if (FindNextChangeNotification(changeHandle)) {
				WaitForSingleObject(changeHandle, INFINITE);

				// Wait for changes to stop..
				// Will usually be at least two - one for the file and one for the directory
				while (true) {
					FindNextChangeNotification(changeHandle);
					if (WaitForSingleObject(changeHandle, 100) == WAIT_TIMEOUT) {
						break;
					}
				}

				EmuLog(LOG_LEVEL::DEBUG, "Change detected in shader folder");

				g_ShaderSources.shaderVersionOnDisk++;
			}
			else {
				EmuLog(LOG_LEVEL::ERROR2, "Shader filewatcher failed to get the next notification");
				break;
			}
		}

		EmuLog(LOG_LEVEL::DEBUG, "Shader file watcher exiting...");

		// until there is a way to disable hotloading
		// this is always an error
		FindCloseChangeNotification(changeHandle);
		return 1;
	});
}
