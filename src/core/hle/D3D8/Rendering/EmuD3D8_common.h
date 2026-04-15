// Shared preamble for all files split from the original Direct3D9.cpp monolith.
// Each EmuPatches_*.cpp and Host*.cpp includes this to get the full
// set of headers, macros, and forward declarations they need.

#ifndef EMUD3D8_COMMON_H
#define EMUD3D8_COMMON_H

#define LOG_PREFIX CXBXR_MODULE::D3D8

#include "common\util\hasher.h" // For ComputeHash
#include <condition_variable>
#include <stack>
#include <algorithm>

#include <core\kernel\exports\xboxkrnl.h>
#include "common\util\CxbxUtil.h"
#include "CxbxVersion.h"
#include "core\kernel\init\CxbxKrnl.h"
#include "core\kernel\support\Emu.h"
#include "core\kernel\support\EmuFS.h"
#include "core\kernel\support\NativeHandle.h"
#include "core\kernel\exports\EmuKrnlKe.h"
#include "EmuShared.h"
#include "..\FixedFunctionState.h"
#include "core\hle\D3D8\ResourceTracker.h"
#include "core\hle\D3D8\Rendering\EmuD3D8.h"
#include "core\hle\D3D8\Rendering\Backend_D3D9.h"
#include "core\hle\D3D8\Rendering\Backend_D3D11.h"
#include "core\hle\D3D8\Rendering\Shader.h"
#include "core\hle\D3D8\XbVertexBuffer.h"
#include "core\hle\D3D8\XbVertexShader.h"
#include "core\hle\D3D8\XbPixelShader.h"
#include "core\hle\D3D8\XbPushBuffer.h"
#include "core\kernel\memory-manager\VMManager.h"
#include "core\hle\XAPI\Xapi.h" // For EMUPATCH
#include "core\hle\D3D8\XbConvert.h"
#include "Logging.h"
#include "..\XbD3D8Logging.h"
#include "core\hle\Intercept.hpp" // for bLLE_GPU
#include "devices\video\nv2a.h"
#include "gui/resource/ResCxbx.h"
#include "RenderStates.h"
#include "TextureStates.h"
#include "WalkIndexBuffer.h"
#include "core\kernel\common\strings.hpp"
#include "common\input\SdlJoystick.h"
#include "common\input\DInputKeyboardMouse.h"
#include "common\input\InputManager.h"
#include "common/util/strConverter.hpp"
#include "VertexShaderCache.h"
#include "PatchDraw.h"
#include "Timer.h"

#include <imgui.h>
#ifdef CXBX_USE_D3D11
#include <backends/imgui_impl_dx11.h>
#else
#include <backends/imgui_impl_dx9.h>
#endif
#include <backends/imgui_impl_win32.h>
#include "core/common/video/RenderBase.hpp"

#include <assert.h>
#include <process.h>
#include <chrono>
#include <clocale>
#include <functional>
#include <unordered_map>
#include <thread>

#include <wrl/client.h>

using namespace Microsoft::WRL;

// Allow use of time duration literals (making 16ms, etc possible)
using namespace std::literals::chrono_literals;

#endif // EMUD3D8_COMMON_H
