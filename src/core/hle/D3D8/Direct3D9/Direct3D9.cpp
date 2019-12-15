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
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#define LOG_PREFIX CXBXR_MODULE::D3D8

#include "common\util\hasher.h"
#include <condition_variable>

// prevent name collisions
namespace xboxkrnl
{
    #include <xboxkrnl\xboxkrnl.h>
};

#include "common\util\CxbxUtil.h"
#include "CxbxVersion.h"
#include "core\kernel\init\CxbxKrnl.h"
#include "core\kernel\support\Emu.h"
#include "EmuShared.h"
#include "gui\DbgConsole.h"
#include "core\hle\D3D8\ResourceTracker.h"
#include "core\hle\D3D8\Direct3D9\Direct3D9.h" // For LPDIRECTDRAWSURFACE7
#include "core\hle\D3D8\XbVertexBuffer.h"
#include "core\hle\D3D8\XbVertexShader.h"
#include "core\hle\D3D8\XbPixelShader.h" // For DxbxUpdateActivePixelShader
#include "core\hle\D3D8\XbPushBuffer.h"
#include "core\kernel\memory-manager\VMManager.h" // for g_VMManager
#include "core\hle\XAPI\Xapi.h" // For EMUPATCH
#include "core\hle\D3D8\XbConvert.h"
#include "Logging.h"
#include "..\XbD3D8Logging.h"
#include "core\hle\Intercept.hpp" // for bLLE_GPU
#include "devices\video\nv2a.h" // For GET_MASK, NV_PGRAPH_CONTROL_0, PUSH_METHOD
#include "gui\ResCxbx.h"
#include "RenderStates.h"
#include "TextureStates.h"
#include "WalkIndexBuffer.h"
#include "core\kernel\common\strings.hpp" // For uem_str
#include "common\input\SdlJoystick.h"
#include "common/util/strConverter.hpp" // for utf8_to_utf16

#include <assert.h>
#include <process.h>
#include <clocale>
#include <unordered_map>
#include <thread>

XboxRenderStateConverter XboxRenderStates;
XboxTextureStateConverter XboxTextureStates;

// Allow use of time duration literals (making 16ms, etc possible)
using namespace std::literals::chrono_literals;

// Global(s)
HWND                                g_hEmuWindow   = NULL; // rendering window
IDirect3DDevice                    *g_pD3DDevice   = nullptr; // Direct3D Device

// Static Variable(s)
static IDirectDrawSurface7         *g_pDDSPrimary  = nullptr; // DirectDraw7 Primary Surface
static IDirectDrawClipper          *g_pDDClipper   = nullptr; // DirectDraw7 Clipper
static IDirectDraw7                *g_pDD7          = nullptr; // DirectDraw7
static HMONITOR                     g_hMonitor      = NULL; // Handle to DirectDraw monitor
static GUID                         g_ddguid = { 0 }; // DirectDraw driver GUID
static DDCAPS                       g_DriverCaps = { 0 };

static bool                         g_bSupportsFormatSurface[XTL::X_D3DFMT_LIN_R8G8B8A8 + 1] = { false };// Does device support surface format?
static bool                         g_bSupportsFormatSurfaceRenderTarget[XTL::X_D3DFMT_LIN_R8G8B8A8 + 1] = { false };// Does device support surface format?
static bool                         g_bSupportsFormatSurfaceDepthStencil[XTL::X_D3DFMT_LIN_R8G8B8A8 + 1] = { false };// Does device support surface format?
static bool                         g_bSupportsFormatTexture[XTL::X_D3DFMT_LIN_R8G8B8A8 + 1] = { false }; // Does device support texture format?
static bool                         g_bSupportsFormatTextureRenderTarget[XTL::X_D3DFMT_LIN_R8G8B8A8 + 1] = { false };// Does device support texture format?
static bool                         g_bSupportsFormatTextureDepthStencil[XTL::X_D3DFMT_LIN_R8G8B8A8 + 1] = { false };// Does device support texture format?
static bool                         g_bSupportsFormatVolumeTexture[XTL::X_D3DFMT_LIN_R8G8B8A8 + 1] = { false }; // Does device support surface format?
static bool                         g_bSupportsFormatCubeTexture[XTL::X_D3DFMT_LIN_R8G8B8A8 + 1] = { false }; // Does device support surface format?
static HBRUSH                       g_hBgBrush = NULL; // Background Brush
static volatile bool                g_bRenderWindowActive = false;
static BOOL                         g_bIsFauxFullscreen = FALSE;
static DWORD						g_OverlaySwap = 0; // Set in D3DDevice_UpdateOverlay
static int                          g_iWireframe = 0; // wireframe toggle
static bool                         g_bHack_UnlockFramerate = false; // ignore the xbox presentation interval
static bool                         g_bHasDepth = false;    // Does device have a Depth Buffer?
static bool                         g_bHasStencil = false;  // Does device have a Stencil Buffer?
static DWORD						g_dwPrimPerFrame = 0;	// Number of primitives within one frame

static Settings::s_video            g_XBVideo;

// D3D based variables
static IDirect3D                   *g_pDirect3D = nullptr;
       D3DCAPS						g_D3DCaps = {};         // Direct3D Caps
static IDirect3DVertexBuffer       *g_pDummyBuffer = nullptr;  // Dummy buffer, used to set unused stream sources with
static IDirect3DIndexBuffer        *g_pClosingLineLoopHostIndexBuffer = nullptr;
static IDirect3DIndexBuffer        *g_pQuadToTriangleHostIndexBuffer = nullptr;
static IDirect3DSurface            *g_pDefaultHostDepthBufferSurface = nullptr;

// cached Direct3D state variable(s)
static size_t                       g_QuadToTriangleHostIndexBuffer_Size = 0; // = NrOfQuadIndices
static INDEX16                     *g_pQuadToTriangleIndexData = nullptr;
static size_t                       g_QuadToTriangleIndexData_Size = 0; // = NrOfQuadIndices

static CxbxVertexBufferConverter VertexBufferConverter = {};

struct {
	XTL::X_D3DSurface Surface;
	RECT SrcRect;
	RECT DstRect;
	BOOL EnableColorKey;
	D3DCOLOR ColorKey;
} g_OverlayProxy;

typedef struct {
	// Arguments to D3DDevice_InsertCallback :
	XTL::X_D3DCALLBACK			    pCallback;
	XTL::X_D3DCALLBACKTYPE          Type;
	XTL::DWORD                      Context;
} s_Xbox_Callback;

static std::queue<s_Xbox_Callback>  g_Xbox_CallbackQueue;
static bool                         g_bHack_DisableHostGPUQueries = false; // TODO : Make configurable
static IDirect3DQuery              *g_pHostQueryWaitForIdle = nullptr;
static IDirect3DQuery              *g_pHostQueryCallbackEvent = nullptr;

// Vertex shader symbols, declared in XbVertexShader.cpp :
extern void CxbxImpl_SelectVertexShaderDirect(XTL::X_VERTEXATTRIBUTEFORMAT* pVAF, DWORD Address);
extern void CxbxImpl_SetVertexShaderInput(DWORD Handle, UINT StreamCount, XTL::X_STREAMINPUT* pStreamInputs);

// Vertex buffer symbols, declared in XbVertexBuffer.cpp
extern void CxbxImpl_SetStreamSource(UINT StreamNumber, XTL::X_D3DVertexBuffer* pStreamData, UINT Stride);

static std::condition_variable		g_VBConditionVariable;	// Used in BlockUntilVerticalBlank
static std::mutex					g_VBConditionMutex;		// Used in BlockUntilVerticalBlank
static DWORD                        g_VBLastSwap = 0;

static XTL::DWORD                   g_Xbox_PresentationInterval_Default = D3DPRESENT_INTERVAL_IMMEDIATE;
       XTL::DWORD                   g_Xbox_PresentationInterval_Override = 0;
static XTL::X_D3DSWAPDATA			g_Xbox_SwapData = {0}; // current swap information
static XTL::X_D3DSWAPCALLBACK		g_pXbox_SwapCallback = xbnullptr;	// Swap/Present callback routine
static XTL::X_D3DVBLANKDATA			g_Xbox_VBlankData = {0}; // current vertical blank information
static XTL::X_D3DVBLANKCALLBACK     g_pXbox_VerticalBlankCallback   = xbnullptr; // Vertical-Blank callback routine

       XTL::X_D3DSurface           *g_pXbox_BackBufferSurface = xbnullptr;
static XTL::X_D3DSurface           *g_pXbox_DefaultDepthStencilSurface = xbnullptr;
       XTL::X_D3DSurface           *g_pXbox_RenderTarget = xbnullptr;
static XTL::X_D3DSurface           *g_pXbox_DepthStencil = xbnullptr;

       XTL::X_VERTEXSHADERCONSTANTMODE g_Xbox_VertexShaderConstantMode = X_D3DSCM_192CONSTANTS; // Set by D3DDevice_SetShaderConstantMode, TODO : Move to XbVertexShader.cpp
static XTL::DWORD                   g_Xbox_BaseVertexIndex = 0; // Set by D3DDevice_SetIndices, read by D3DDevice_DrawIndexedVertices : a value that's effectively added to every vertex index (as stored in an index buffer) by multiplying this by vertex stride and added to the vertex buffer start (see BaseVertexIndex in CxbxDrawIndexed)
static XTL::DWORD                  *g_pXbox_BeginPush_Buffer = xbnullptr; // primary push buffer

       XTL::X_PixelShader*			g_pXbox_PixelShader = xbnullptr;
static XTL::PVOID                   g_pXbox_Palette_Data[XTL::X_D3DTS_STAGECOUNT] = { xbnullptr, xbnullptr, xbnullptr, xbnullptr }; // cached palette pointer
static unsigned                     g_Xbox_Palette_Size[XTL::X_D3DTS_STAGECOUNT] = { 0 }; // cached palette size


       XTL::X_D3DBaseTexture       *g_pXbox_SetTexture[XTL::X_D3DTS_STAGECOUNT] = {0,0,0,0}; // Set by our D3DDevice_SetTexture and D3DDevice_SwitchTexture patches
static XTL::X_D3DBaseTexture        CxbxActiveTextureCopies[XTL::X_D3DTS_STAGECOUNT] = {}; // Set by D3DDevice_SwitchTexture. Cached active texture

/* Unused :
static XTL::DWORD                  *g_Xbox_D3DDevice; // TODO: This should be a D3DDevice structure

static	DWORD						g_dwVertexShaderUsage = 0; // Unused. If needed, move to XbVertexShader.cpp
*/

static XTL::DWORD                   g_VertexShaderSlots[X_VSH_MAX_INSTRUCTION_COUNT];
       XTL::DWORD                   g_Xbox_VertexShader_Handle = 0;

// Static Function(s)
static BOOL WINAPI                  EmuEnumDisplayDevices(GUID FAR *lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext, HMONITOR hm);
static DWORD WINAPI                 EmuRenderWindow(LPVOID);
static DWORD WINAPI                 EmuCreateDeviceProxy(LPVOID);
static LRESULT WINAPI               EmuMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static DWORD WINAPI                 EmuUpdateTickCount(LPVOID);
static inline void                  EmuVerifyResourceIsRegistered(XTL::X_D3DResource *pResource, DWORD D3DUsage, int iTextureStage, DWORD dwSize);
static void							UpdateCurrentMSpFAndFPS(); // Used for benchmarking/fps count

extern void UpdateFPSCounter();

#define CXBX_D3DCOMMON_IDENTIFYING_MASK (X_D3DCOMMON_TYPE_MASK | X_D3DCOMMON_D3DCREATED)

typedef struct resource_key_hash {
	// All Xbox X_D3DResource structs have these fields :
	DWORD Common; // We set this to the CXBX_D3DCOMMON_IDENTIFYING_MASK bits of the source Common field
	DWORD Data; // We set this as-is to a copy of the source Data field
	// DWORD Lock; // We leave away the source Lock field, since it's entirely volatile (not deterministic)
	union {
		struct {
			// For non-pixel-containers, we set the Xbox resource address for now (TODO : Come up with something better) :
			xbaddr ResourceAddr; // We set this as-is
		};
		struct {
			// For XTL::X_D3DPixelContainer's we also set these fields :
			DWORD Format; // We set this as-is
			DWORD Size; // We set this as-is
			// For X_D3DFMT_P8 paletized pixel-containers, we also set this field :
			uint64_t PaletteHash;
		};
	};

	// These operator overloads are required to use resource_key_t in resource_cache_t :
	bool operator==(const struct resource_key_hash& other) const
	{
		return (Common == other.Common)
			&& (Data == other.Data)
			&& (Format == other.Format)
			&& (Size == other.Size)
			&& (PaletteHash == other.PaletteHash);
		// Note : ResourceAddr doesn't need comparison, since it's union'ed with Format,Size,PaletteHash already
	}

	// See https://marknelson.us/posts/2011/09/03/hash-functions-for-c-unordered-containers.html
	size_t operator()(const struct resource_key_hash& value) const
	{
		return (size_t)ComputeHash((void*)&value, sizeof(value));
	}
} resource_key_t;

// information passed to the create device proxy thread
struct EmuD3D8CreateDeviceProxyData
{
	// Set by EmuD3DInit()
    XTL::UINT                        Adapter;
    D3DDEVTYPE                       DeviceType;
    HWND                             hFocusWindow;
	// Set byt EMUPATCH(Direct3D_CreateDevice)
    XTL::X_D3DPRESENT_PARAMETERS     XboxPresentationParameters;
    volatile bool                    bReady;
    volatile bool                    bCreate;   // false : release
	// Set by EmuCreateDeviceProxy()
    XTL::DWORD                       BehaviorFlags;
	D3DPRESENT_PARAMETERS            HostPresentationParameters;
    volatile HRESULT                 hRet;
}
g_EmuCDPD = {0};

#define DEBUG_D3DRESULT(hRet, message) \
	do { \
		LOG_CHECK_ENABLED(LOG_LEVEL::DEBUG) { \
			if (FAILED(hRet)) \
				if(g_bPrintfOn) \
					printf("%s%s : %s D3D error (0x%.08X: %s)\n", _logThreadPrefix.c_str(), _logFuncPrefix.c_str(), message, hRet, D3DErrorString(hRet)); \
		} \
	} while (0)


const char *CxbxGetErrorDescription(HRESULT hResult)
{
	// TODO : For D3D9, Use DXGetErrorDescription9(hResult) (requires another DLL though)
	// See : https://www.fairyengine.com/articles/dxmultiviews.htm
	// and : https://web.archive.org/web/20101231193248/https://www.gamedev.net/community/forums/showfaq.asp?forum_id=10
	// and : https://www.gamedev.net/community/forums/topic.asp?topic_id=16157
	// But https://blogs.msdn.microsoft.com/chuckw/2012/04/24/wheres-dxerr-lib/
	// suggests to use FormatMessage with FORMAT_MESSAGE_FROM_SYSTEM for DirectX errors
	switch (hResult)
	{
	case D3DERR_INVALIDCALL: return "Invalid Call";
	case D3DERR_NOTAVAILABLE: return "Not Available";
	// case D3DERR_OUTOFVIDEOMEMORY: return "Out of Video Memory"; // duplicate of DDERR_OUTOFVIDEOMEMORY

	case D3D_OK: return "No error occurred.";
#if 0
	case D3DERR_BADMAJORVERSION: return "The service that you requested is unavailable in this major version of DirectX. (A major version denotes a primary release, such as DirectX 6.0.) ";
	case D3DERR_BADMINORVERSION: return "The service that you requested is available in this major version of DirectX, but not in this minor version. Get the latest version of the component run time from Microsoft. (A minor version denotes a secondary release, such as DirectX 6.1.) ";
	case D3DERR_COLORKEYATTACHED: return "The application attempted to create a texture with a surface that uses a color key for transparency. ";
#endif
	case D3DERR_CONFLICTINGTEXTUREFILTER: return "The current texture filters cannot be used together. ";
	case D3DERR_CONFLICTINGTEXTUREPALETTE: return "The current textures cannot be used simultaneously. This generally occurs when a multitexture device requires that all palettized textures simultaneously enabled also share the same palette. ";
	case D3DERR_CONFLICTINGRENDERSTATE: return "The currently set render states cannot be used together. ";
#if 0
	case D3DERR_DEVICEAGGREGATED: return "The IDirect3DDevice7::SetRenderTarget method was called on a device that was retrieved from the render target surface. ";
	case D3DERR_EXECUTE_CLIPPED_FAILED: return "The execute buffer could not be clipped during execution. ";
	case D3DERR_EXECUTE_CREATE_FAILED: return "The execute buffer could not be created. This typically occurs when no memory is available to allocate the execute buffer. ";
	case D3DERR_EXECUTE_DESTROY_FAILED: return "The memory for the execute buffer could not be deallocated. ";
	case D3DERR_EXECUTE_FAILED: return "The contents of the execute buffer are invalid and cannot be executed. ";
	case D3DERR_EXECUTE_LOCK_FAILED: return "The execute buffer could not be locked. ";
	case D3DERR_EXECUTE_LOCKED: return "The operation requested by the application could not be completed because the execute buffer is locked. ";
	case D3DERR_EXECUTE_NOT_LOCKED: return "The execute buffer could not be unlocked because it is not currently locked. ";
	case D3DERR_EXECUTE_UNLOCK_FAILED: return "The execute buffer could not be unlocked. ";
	case D3DERR_INBEGIN: return "The requested operation cannot be completed while scene rendering is taking place. Try again after the scene is completed and the IDirect3DDevice7::EndScene method is called. ";
	case D3DERR_INBEGINSTATEBLOCK: return "The operation cannot be completed while recording states for a state block. Complete recording by calling the IDirect3DDevice7::EndStateBlock method, and try again. ";
	case D3DERR_INITFAILED: return "A rendering device could not be created because the new device could not be initialized. ";
	case D3DERR_INVALID_DEVICE: return "The requested device type is not valid. ";
	case D3DERR_INVALIDCURRENTVIEWPORT: return "The currently selected viewport is not valid. ";
	case D3DERR_INVALIDMATRIX: return "The requested operation could not be completed because the combination of the currently set world, view, and projection matrices is invalid (the determinant of the combined matrix is 0). ";
	case D3DERR_INVALIDPALETTE: return "The palette associated with a surface is invalid. ";
	case D3DERR_INVALIDPRIMITIVETYPE: return "The primitive type specified by the application is invalid. ";
	case D3DERR_INVALIDRAMPTEXTURE: return "Ramp mode is being used, and the texture handle in the current material does not match the current texture handle that is set as a render state. ";
	case D3DERR_INVALIDSTATEBLOCK: return "The state block handle is invalid. ";
	case D3DERR_INVALIDVERTEXFORMAT: return "The combination of flexible vertex format flags specified by the application is not valid. ";
	case D3DERR_INVALIDVERTEXTYPE: return "The vertex type specified by the application is invalid. ";
	case D3DERR_LIGHT_SET_FAILED: return "The attempt to set lighting parameters for a light object failed. ";
	case D3DERR_LIGHTHASVIEWPORT: return "The requested operation failed because the light object is associated with another viewport. ";
	case D3DERR_LIGHTNOTINTHISVIEWPORT: return "The requested operation failed because the light object has not been associated with this viewport. ";
	case D3DERR_MATERIAL_CREATE_FAILED: return "The material could not be created. This typically occurs when no memory is available to allocate for the material. ";
	case D3DERR_MATERIAL_DESTROY_FAILED: return "The memory for the material could not be deallocated. ";
	case D3DERR_MATERIAL_GETDATA_FAILED: return "The material parameters could not be retrieved. ";
	case D3DERR_MATERIAL_SETDATA_FAILED: return "The material parameters could not be set. ";
	case D3DERR_MATRIX_CREATE_FAILED: return "The matrix could not be created. This can occur when no memory is available to allocate for the matrix. ";
	case D3DERR_MATRIX_DESTROY_FAILED: return "The memory for the matrix could not be deallocated. ";
	case D3DERR_MATRIX_GETDATA_FAILED: return "The matrix data could not be retrieved. This can occur when the matrix was not created by the current device. ";
	case D3DERR_MATRIX_SETDATA_FAILED: return "The matrix data could not be set. This can occur when the matrix was not created by the current device. ";
	case D3DERR_NOCURRENTVIEWPORT: return "The viewport parameters could not be retrieved because none have been set. ";
	case D3DERR_NOTINBEGIN: return "The requested rendering operation could not be completed because scene rendering has not begun. Call IDirect3DDevice7::BeginScene to begin rendering, and try again. ";
	case D3DERR_NOTINBEGINSTATEBLOCK: return "The requested operation could not be completed because it is only valid while recording a state block. Call the IDirect3DDevice7::BeginStateBlock method, and try again. ";
	case D3DERR_NOVIEWPORTS: return "The requested operation failed because the device currently has no viewports associated with it. ";
	case D3DERR_SCENE_BEGIN_FAILED: return "Scene rendering could not begin. ";
	case D3DERR_SCENE_END_FAILED: return "Scene rendering could not be completed. ";
	case D3DERR_SCENE_IN_SCENE: return "Scene rendering could not begin because a previous scene was not completed by a call to the IDirect3DDevice7::EndScene method. ";
	case D3DERR_SCENE_NOT_IN_SCENE: return "Scene rendering could not be completed because a scene was not started by a previous call to the IDirect3DDevice7::BeginScene method. ";
	case D3DERR_SETVIEWPORTDATA_FAILED: return "The viewport parameters could not be set. ";
	case D3DERR_STENCILBUFFER_NOTPRESENT: return "The requested stencil buffer operation could not be completed because there is no stencil buffer attached to the render target surface. ";
	case D3DERR_SURFACENOTINVIDMEM: return "The device could not be created because the render target surface is not located in video memory. (Hardware-accelerated devices require video-memory render target surfaces.) ";
	case D3DERR_TEXTURE_BADSIZE: return "The dimensions of a current texture are invalid. This can occur when an application attempts to use a texture that has dimensions that are not a power of 2 with a device that requires them. ";
	case D3DERR_TEXTURE_CREATE_FAILED: return "The texture handle for the texture could not be retrieved from the driver. ";
	case D3DERR_TEXTURE_DESTROY_FAILED: return "The device was unable to deallocate the texture memory. ";
	case D3DERR_TEXTURE_GETSURF_FAILED: return "The DirectDraw surface used to create the texture could not be retrieved. ";
	case D3DERR_TEXTURE_LOAD_FAILED: return "The texture could not be loaded. ";
	case D3DERR_TEXTURE_LOCK_FAILED: return "The texture could not be locked. ";
	case D3DERR_TEXTURE_LOCKED: return "The requested operation could not be completed because the texture surface is currently locked. ";
	case D3DERR_TEXTURE_NO_SUPPORT: return "The device does not support texture mapping. ";
	case D3DERR_TEXTURE_NOT_LOCKED: return "The requested operation could not be completed because the texture surface is not locked. ";
	case D3DERR_TEXTURE_SWAP_FAILED: return "The texture handles could not be swapped. ";
	case D3DERR_TEXTURE_UNLOCK_FAILED: return "The texture surface could not be unlocked. ";
#endif
	case D3DERR_TOOMANYOPERATIONS: return "The application is requesting more texture-filtering operations than the device supports. ";
#if 0
	case D3DERR_TOOMANYPRIMITIVES: return "The device is unable to render the provided number of primitives in a single pass. ";
#endif
	case D3DERR_UNSUPPORTEDALPHAARG: return "The device does not support one of the specified texture-blending arguments for the alpha channel. ";
	case D3DERR_UNSUPPORTEDALPHAOPERATION: return "The device does not support one of the specified texture-blending operations for the alpha channel. ";
	case D3DERR_UNSUPPORTEDCOLORARG: return "The device does not support one of the specified texture-blending arguments for color values. ";
	case D3DERR_UNSUPPORTEDCOLOROPERATION: return "The device does not support one of the specified texture-blending operations for color values. ";
	case D3DERR_UNSUPPORTEDFACTORVALUE: return "The specified texture factor value is not supported by the device. ";
	case D3DERR_UNSUPPORTEDTEXTUREFILTER: return "The specified texture filter is not supported by the device. ";
#if 0
	case D3DERR_VBUF_CREATE_FAILED: return "The vertex buffer could not be created. This can happen when there is insufficient memory to allocate a vertex buffer. ";
	case D3DERR_VERTEXBUFFERLOCKED: return "The requested operation could not be completed because the vertex buffer is locked. ";
	case D3DERR_VERTEXBUFFEROPTIMIZED: return "The requested operation could not be completed because the vertex buffer is optimized. (The contents of optimized vertex buffers are driver-specific and considered private.) ";
	case D3DERR_VERTEXBUFFERUNLOCKFAILED: return "The vertex buffer could not be unlocked because the vertex buffer memory was overrun. Be sure that your application does not write beyond the size of the vertex buffer. ";
	case D3DERR_VIEWPORTDATANOTSET: return "The requested operation could not be completed because viewport parameters have not yet been set. Set the viewport parameters by calling the IDirect3DDevice7::SetViewport method, and try again. ";
	case D3DERR_VIEWPORTHASNODEVICE: return "The requested operation could not be completed because the viewport has not yet been associated with a device. Associate the viewport with a rendering device by calling the IDirect3DDevice3::AddViewport method, and try again. ";
#endif
	case D3DERR_WRONGTEXTUREFORMAT: return "The pixel format of the texture surface is not valid. ";
#if 0
	case D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY: return "The requested operation could not be completed because the specified device requires system-memory depth-buffer surfaces. (Software rendering devices require system-memory depth buffers.) ";
	case D3DERR_ZBUFF_NEEDS_VIDEOMEMORY: return "The requested operation could not be completed because the specified device requires video-memory depth-buffer surfaces. (Hardware-accelerated devices require video-memory depth buffers.) ";
	case D3DERR_ZBUFFER_NOTPRESENT: return "The requested operation could not be completed because the render target surface does not have an attached depth buffer. ";
	case DD_OK: return "The request completed successfully.";
#endif
	case DDERR_ALREADYINITIALIZED: return "The object has already been initialized.";
	case DDERR_BLTFASTCANTCLIP: return "A DirectDrawClipper object is attached to a source surface that has passed into a call to the IDirectDrawSurface7::BltFast method.";
	case DDERR_CANNOTATTACHSURFACE: return "A surface cannot be attached to another requested surface.";
	case DDERR_CANNOTDETACHSURFACE: return "A surface cannot be detached from another requested surface.";
	case DDERR_CANTCREATEDC: return "Windows cannot create any more device contexts (DCs), or a DC has requested a palette-indexed surface when the surface had no palette and the display mode was not palette-indexed (in this case DirectDraw cannot select a proper palette into the DC).";
	case DDERR_CANTDUPLICATE: return "Primary and 3-D surfaces, or surfaces that are implicitly created, cannot be duplicated.";
	case DDERR_CANTLOCKSURFACE: return "Access to this surface is refused because an attempt was made to lock the primary surface without DCI support.";
	case DDERR_CANTPAGELOCK: return "An attempt to page-lock a surface failed. Page lock does not work on a display-memory surface or an emulated primary surface.";
	case DDERR_CANTPAGEUNLOCK: return "An attempt to page-unlock a surface failed. Page unlock does not work on a display-memory surface or an emulated primary surface.";
	case DDERR_CLIPPERISUSINGHWND: return "An attempt was made to set a clip list for a DirectDrawClipper object that is already monitoring a window handle.";
	case DDERR_COLORKEYNOTSET: return "No source color key is specified for this operation.";
	case DDERR_CURRENTLYNOTAVAIL: return "No support is currently available.";
	case DDERR_DDSCAPSCOMPLEXREQUIRED: return "New for DirectX 7.0. The surface requires the DDSCAPS_COMPLEX flag.";
	case DDERR_DCALREADYCREATED: return "A device context (DC) has already been returned for this surface. Only one DC can be retrieved for each surface.";
	case DDERR_DEVICEDOESNTOWNSURFACE: return "Surfaces created by one DirectDraw device cannot be used directly by another DirectDraw device.";
	case DDERR_DIRECTDRAWALREADYCREATED: return "A DirectDraw object representing this driver has already been created for this process.";
	case DDERR_EXCEPTION: return "An exception was encountered while performing the requested operation.";
	case DDERR_EXCLUSIVEMODEALREADYSET: return "An attempt was made to set the cooperative level when it was already set to exclusive.";
	case DDERR_EXPIRED: return "The data has expired and is therefore no longer valid.";
	case DDERR_GENERIC: return "There is an undefined error condition.";
	case DDERR_HEIGHTALIGN: return "The height of the provided rectangle is not a multiple of the required alignment.";
	case DDERR_HWNDALREADYSET: return "The DirectDraw cooperative-level window handle has already been set. It cannot be reset while the process has surfaces or palettes created.";
	case DDERR_HWNDSUBCLASSED: return "DirectDraw is prevented from restoring state because the DirectDraw cooperative-level window handle has been subclassed.";
	case DDERR_IMPLICITLYCREATED: return "The surface cannot be restored because it is an implicitly created surface.";
	case DDERR_INCOMPATIBLEPRIMARY: return "The primary surface creation request does not match the existing primary surface.";
	case DDERR_INVALIDCAPS: return "One or more of the capability bits passed to the callback function are incorrect.";
	case DDERR_INVALIDCLIPLIST: return "DirectDraw does not support the provided clip list.";
	case DDERR_INVALIDDIRECTDRAWGUID: return "The globally unique identifier (GUID) passed to the DirectDrawCreate function is not a valid DirectDraw driver identifier.";
	case DDERR_INVALIDMODE: return "DirectDraw does not support the requested mode.";
	case DDERR_INVALIDOBJECT: return "DirectDraw received a pointer that was an invalid DirectDraw object.";
	case DDERR_INVALIDPARAMS: return "One or more of the parameters passed to the method are incorrect.";
	case DDERR_INVALIDPIXELFORMAT: return "The pixel format was invalid as specified.";
	case DDERR_INVALIDPOSITION: return "The position of the overlay on the destination is no longer legal.";
	case DDERR_INVALIDRECT: return "The provided rectangle was invalid.";
	case DDERR_INVALIDSTREAM: return "The specified stream contains invalid data.";
	case DDERR_INVALIDSURFACETYPE: return "The surface was of the wrong type.";
	case DDERR_LOCKEDSURFACES: return "One or more surfaces are locked, causing the failure of the requested operation.";
	case DDERR_MOREDATA: return "There is more data available than the specified buffer size can hold.";
	case DDERR_NEWMODE: return "New for DirectX 7.0. When IDirectDraw7::StartModeTest is called with the DDSMT_ISTESTREQUIRED flag, it may return this value to denote that some or all of the resolutions can and should be tested. IDirectDraw7::EvaluateMode returns this value to indicate that the test has switched to a new display mode.";
	case DDERR_NO3D: return "No 3-D hardware or emulation is present.";
	case DDERR_NOALPHAHW: return "No alpha-acceleration hardware is present or available, causing the failure of the requested operation.";
	case DDERR_NOBLTHW: return "No blitter hardware is present.";
	case DDERR_NOCLIPLIST: return "No clip list is available.";
	case DDERR_NOCLIPPERATTACHED: return "No DirectDrawClipper object is attached to the surface object.";
	case DDERR_NOCOLORCONVHW: return "No color-conversion hardware is present or available.";
	case DDERR_NOCOLORKEY: return "The surface does not currently have a color key.";
	case DDERR_NOCOLORKEYHW: return "There is no hardware support for the destination color key.";
	case DDERR_NOCOOPERATIVELEVELSET: return "A create function was called without the IDirectDraw7::SetCooperativeLevel method.";
	case DDERR_NODC: return "No device context (DC) has ever been created for this surface.";
	case DDERR_NODDROPSHW: return "No DirectDraw raster-operation (ROP) hardware is available.";
	case DDERR_NODIRECTDRAWHW: return "Hardware-only DirectDraw object creation is not possible; the driver does not support any hardware.";
	case DDERR_NODIRECTDRAWSUPPORT: return "DirectDraw support is not possible with the current display driver.";
	case DDERR_NODRIVERSUPPORT: return "New for DirectX 7.0. Testing cannot proceed because the display adapter driver does not enumerate refresh rates.";
	case DDERR_NOEMULATION: return "Software emulation is not available.";
	case DDERR_NOEXCLUSIVEMODE: return "The operation requires the application to have exclusive mode, but the application does not have exclusive mode.";
	case DDERR_NOFLIPHW: return "Flipping visible surfaces is not supported.";
	case DDERR_NOFOCUSWINDOW: return "An attempt was made to create or set a device window without first setting the focus window.";
	case DDERR_NOGDI: return "No GDI is present.";
	case DDERR_NOHWND: return "Clipper notification requires a window handle, or no window handle has been previously set as the cooperative level window handle.";
	case DDERR_NOMIPMAPHW: return "No mipmap-capable texture mapping hardware is present or available.";
	case DDERR_NOMIRRORHW: return "No mirroring hardware is present or available.";
	case DDERR_NOMONITORINFORMATION: return "New for DirectX 7.0. Testing cannot proceed because the monitor has no associated EDID data.";
	case DDERR_NONONLOCALVIDMEM: return "An attempt was made to allocate nonlocal video memory from a device that does not support nonlocal video memory.";
	case DDERR_NOOPTIMIZEHW: return "The device does not support optimized surfaces.";
	case DDERR_NOOVERLAYDEST: return "The IDirectDrawSurface7::GetOverlayPosition method is called on an overlay that the IDirectDrawSurface7::UpdateOverlay method has not been called on to establish as a destination.";
	case DDERR_NOOVERLAYHW: return "No overlay hardware is present or available.";
	case DDERR_NOPALETTEATTACHED: return "No palette object is attached to this surface.";
	case DDERR_NOPALETTEHW: return "There is no hardware support for 16- or 256-color palettes.";
	case DDERR_NORASTEROPHW: return "No appropriate raster-operation hardware is present or available.";
	case DDERR_NOROTATIONHW: return "No rotation hardware is present or available.";
	case DDERR_NOSTEREOHARDWARE: return "There is no stereo hardware present or available.";
	case DDERR_NOSTRETCHHW: return "There is no hardware support for stretching.";
	case DDERR_NOSURFACELEFT: return "There is no hardware present that supports stereo surfaces.";
	case DDERR_NOT4BITCOLOR: return "The DirectDrawSurface object is not using a 4-bit color palette, and the requested operation requires a 4-bit color palette.";
	case DDERR_NOT4BITCOLORINDEX: return "The DirectDrawSurface object is not using a 4-bit color index palette, and the requested operation requires a 4-bit color index palette.";
	case DDERR_NOT8BITCOLOR: return "The DirectDrawSurface object is not using an 8-bit color palette, and the requested operation requires an 8-bit color palette.";
	case DDERR_NOTAOVERLAYSURFACE: return "An overlay component is called for a nonoverlay surface.";
	case DDERR_NOTEXTUREHW: return "The operation cannot be carried out because no texture-mapping hardware is present or available.";
	case DDERR_NOTFLIPPABLE: return "An attempt was made to flip a surface that cannot be flipped.";
	case DDERR_NOTFOUND: return "The requested item was not found.";
	case DDERR_NOTINITIALIZED: return "An attempt was made to call an interface method of a DirectDraw object created by CoCreateInstance before the object was initialized.";
	case DDERR_NOTLOADED: return "The surface is an optimized surface, but it has not yet been allocated any memory.";
	case DDERR_NOTLOCKED: return "An attempt was made to unlock a surface that was not locked.";
	case DDERR_NOTPAGELOCKED: return "An attempt was made to page-unlock a surface with no outstanding page locks.";
	case DDERR_NOTPALETTIZED: return "The surface being used is not a palette-based surface.";
	case DDERR_NOVSYNCHW: return "There is no hardware support for vertical blanksynchronized operations.";
	case DDERR_NOZBUFFERHW: return "The operation to create a z-buffer in display memory or to perform a blit, using a z-buffer cannot be carried out because there is no hardware support for z-buffers.";
	case DDERR_NOZOVERLAYHW: return "The overlay surfaces cannot be z-layered, based on the z-order because the hardware does not support z-ordering of overlays.";
	case DDERR_OUTOFCAPS: return "The hardware needed for the requested operation has already been allocated.";
	case DDERR_OUTOFMEMORY: return "DirectDraw does not have enough memory to perform the operation.";
	case DDERR_OUTOFVIDEOMEMORY: return "DirectDraw does not have enough display memory to perform the operation.";
	case DDERR_OVERLAPPINGRECTS: return "The source and destination rectangles are on the same surface and overlap each other.";
	case DDERR_OVERLAYCANTCLIP: return "The hardware does not support clipped overlays.";
	case DDERR_OVERLAYCOLORKEYONLYONEACTIVE: return "An attempt was made to have more than one color key active on an overlay.";
	case DDERR_OVERLAYNOTVISIBLE: return "The IDirectDrawSurface7::GetOverlayPosition method was called on a hidden overlay.";
	case DDERR_PALETTEBUSY: return "Access to this palette is refused because the palette is locked by another thread.";
	case DDERR_PRIMARYSURFACEALREADYEXISTS: return "This process has already created a primary surface.";
	case DDERR_REGIONTOOSMALL: return "The region passed to the IDirectDrawClipper::GetClipList method is too small.";
	case DDERR_SURFACEALREADYATTACHED: return "An attempt was made to attach a surface to another surface to which it is already attached.";
	case DDERR_SURFACEALREADYDEPENDENT: return "An attempt was made to make a surface a dependency of another surface on which it is already dependent.";
	case DDERR_SURFACEBUSY: return "Access to the surface is refused because the surface is locked by another thread.";
	case DDERR_SURFACEISOBSCURED: return "Access to the surface is refused because the surface is obscured.";
	case DDERR_SURFACELOST: return "Access to the surface is refused because the surface memory is gone. Call the IDirectDrawSurface7::Restore method on this surface to restore the memory associated with it.";
	case DDERR_SURFACENOTATTACHED: return "The requested surface is not attached.";
	case DDERR_TESTFINISHED: return "New for DirectX 7.0. When returned by the IDirectDraw7::StartModeTest method, this value means that no test could be initiated because all the resolutions chosen for testing already have refresh rate information in the registry. When returned by IDirectDraw7::EvaluateMode, the value means that DirectDraw has completed a refresh rate test.";
	case DDERR_TOOBIGHEIGHT: return "The height requested by DirectDraw is too large.";
	case DDERR_TOOBIGSIZE: return "The size requested by DirectDraw is too large. However, the individual height and width are valid sizes.";
	case DDERR_TOOBIGWIDTH: return "The width requested by DirectDraw is too large.";
	case DDERR_UNSUPPORTED: return "The operation is not supported.";
	case DDERR_UNSUPPORTEDFORMAT: return "The pixel format requested is not supported by DirectDraw.";
	case DDERR_UNSUPPORTEDMASK: return "The bitmask in the pixel format requested is not supported by DirectDraw.";
	case DDERR_UNSUPPORTEDMODE: return "The display is currently in an unsupported mode.";
	case DDERR_VERTICALBLANKINPROGRESS: return "A vertical blank is in progress.";
	case DDERR_VIDEONOTACTIVE: return "The video port is not active.";
	case DDERR_WASSTILLDRAWING: return "The previous blit operation that is transferring information to or from this surface is incomplete.";
	case DDERR_WRONGMODE: return "This surface cannot be restored because it was created in a different mode.";
	case DDERR_XALIGN: return "The provided rectangle was not horizontally aligned on a required boundary.";
	}

	return nullptr;
}

// TODO move to shader file. Needs to be called whenever a shader or declaration is set
void SetOverrideFlags(CxbxVertexShader* pCxbxVertexShader) {
	if (pCxbxVertexShader != nullptr && pCxbxVertexShader->pHostVertexShader != nullptr) {
		float overrideFlags[16];
		for (int i = 0; i < 16; i++) {
			overrideFlags[i] = !pCxbxVertexShader->VertexShaderInfo.vRegisterInDeclaration[i];
		}
		g_pD3DDevice->SetVertexShaderConstantF(X_D3DVS_CONSTREG_VERTEXDATA4F_FLAG_BASE, overrideFlags, 4);
	}
}

const char *D3DErrorString(HRESULT hResult)
{
	static char buffer[1024];
	buffer[0] = 0; // Reset static buffer!

	const char* errorCodeString = DXGetErrorString(hResult);
	if (errorCodeString)
	{
		strcat(buffer, errorCodeString);
		strcat(buffer, ": ");
	}

	const char* errorDescription = CxbxGetErrorDescription(hResult);
	if (errorDescription)
		strcat(buffer, errorDescription);
	else
		strcat(buffer, "Unknown D3D error.");

	return buffer;
}

VOID CxbxInitWindow(bool bFullInit)
{
    g_EmuShared->GetVideoSettings(&g_XBVideo);

    if(g_XBVideo.bFullScreen)
        CxbxKrnl_hEmuParent = NULL;

    // create timing thread
	if (bFullInit)
    {
        DWORD dwThreadId;

        HANDLE hThread = CreateThread(nullptr, 0, EmuUpdateTickCount, nullptr, 0, &dwThreadId);
        // We set the priority of this thread a bit higher, to assure reliable timing :
        SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);

        CxbxKrnlRegisterThread(hThread);
    }

/* TODO : Port this Dxbx code :
  // create vblank handling thread
    {
        dwThreadId = 0;
        {hThread :=} CreateThread(nullptr, 0, EmuThreadHandleVBlank, nullptr, 0, &dwThreadId);
    }
*/
    // create window message processing thread
    {
        DWORD dwThreadId;

        g_bRenderWindowActive = false;

        HANDLE hRenderWindowThread = CreateThread(nullptr, 0, EmuRenderWindow, nullptr, 0, &dwThreadId);

		if (hRenderWindowThread == NULL) {
			char szBuffer[1024] = { 0 };
			sprintf(szBuffer, "Creating EmuRenderWindowThread Failed: %08X", GetLastError());
			CxbxPopupMessage(LOG_LEVEL::FATAL, CxbxMsgDlgIcon_Error, szBuffer);
			EmuShared::Cleanup();
			ExitProcess(0);
		}

        while(!g_bRenderWindowActive)
            Sleep(0); 

		Sleep(0);
    }

	SetFocus(g_hEmuWindow);
}

void DrawUEM(HWND hWnd)
{
	// Draw the universal error message (UEM)
	// See https://xboxdevwiki.net/Fatal_Error
	// Only call this from WM_PAINT message!

	PAINTSTRUCT ps;

	BeginPaint(hWnd, &ps);

	HDC hDC = GetDC(hWnd);
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hUEMBmp = CreateCompatibleBitmap(hDC, 640, 480);
	HBITMAP hOriUEMBmp = (HBITMAP)SelectObject(hMemDC, hUEMBmp);


	int nHeight = -MulDiv(8, GetDeviceCaps(hMemDC, LOGPIXELSY), 72);

	HFONT hFont = CreateFont(nHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_ROMAN, "Verdana");

	HGDIOBJ tmpObj = SelectObject(hMemDC, hFont);

	SetBkColor(hMemDC, RGB(0, 0, 0));

	SetTextColor(hMemDC, RGB(0, 204, 0));

	std::wstring utf16str = utf8_to_utf16(uem_str.c_str());

	// Unfortunately, DrawTextW doesn't support vertical alignemnt, so we have to do the calculation
	// ourselves. See here: https://social.msdn.microsoft.com/Forums/vstudio/en-US/abd89aae-16a0-41c6-8db6-b119ea90b42a/win32-drawtext-how-center-in-vertical-with-new-lines-and-tabs?forum=vclanguage

	RECT rect = { 0, 0, 640, 480 };
	RECT textrect = { 0, 0, 640, 480 };
	DrawTextW(hMemDC, utf16str.c_str(), utf16str.length(), &textrect, DT_CALCRECT);
	rect.top = (rect.bottom - textrect.bottom) / 2;
	DrawTextW(hMemDC, utf16str.c_str(), utf16str.length(), &rect, DT_CENTER);


	// Draw the Xbox error code

	SetTextColor(hMemDC, RGB(255, 255, 255));
	std::string err_str(std::to_string(g_CxbxFatalErrorCode));
	rect.left = 20;
	DrawText(hMemDC, err_str.c_str(), err_str.length(), &rect, DT_LEFT);

	GetClientRect(hWnd, &rect);
	SetStretchBltMode(hDC, COLORONCOLOR);
	StretchBlt(hDC, rect.left, rect.top, rect.right, rect.bottom, hMemDC, 0, 0, 640, 480, SRCCOPY);

	SelectObject(hMemDC, hOriUEMBmp);
	SelectObject(hDC, tmpObj);

	DeleteObject(hUEMBmp);
	DeleteObject(hFont);
	DeleteObject(hMemDC);

	if (hDC != NULL)
		ReleaseDC(hWnd, hDC);

	EndPaint(hWnd, &ps);
}

inline DWORD GetXboxCommonResourceType(const XTL::DWORD XboxResource_Common)
{
	DWORD dwCommonType = XboxResource_Common & X_D3DCOMMON_TYPE_MASK;
	return dwCommonType;
}

inline DWORD GetXboxCommonResourceType(const XTL::X_D3DResource* pXboxResource)
{
	// Don't pass in unassigned Xbox resources
	assert(pXboxResource != xbnullptr);

	return GetXboxCommonResourceType(pXboxResource->Common);
}

XTL::X_D3DFORMAT GetXboxPixelContainerFormat(const XTL::DWORD XboxPixelContainer_Format)
{
	XTL::X_D3DFORMAT d3d_format = (XTL::X_D3DFORMAT)((XboxPixelContainer_Format & X_D3DFORMAT_FORMAT_MASK) >> X_D3DFORMAT_FORMAT_SHIFT);
	return d3d_format;
}

XTL::X_D3DFORMAT GetXboxPixelContainerFormat(const XTL::X_D3DPixelContainer *pXboxPixelContainer)
{
	// Don't pass in unassigned Xbox pixel container
	assert(pXboxPixelContainer != xbnullptr);

	return GetXboxPixelContainerFormat(pXboxPixelContainer->Format);
}

inline int GetXboxPixelContainerDimensionCount(const XTL::X_D3DPixelContainer *pXboxPixelContainer)
{
	// Don't pass in unassigned Xbox pixel container
	assert(pXboxPixelContainer != xbnullptr);

	return (XTL::X_D3DFORMAT)((pXboxPixelContainer->Format & X_D3DFORMAT_DIMENSION_MASK) >> X_D3DFORMAT_DIMENSION_SHIFT);
}

XTL::X_D3DRESOURCETYPE GetXboxD3DResourceType(const XTL::X_D3DResource *pXboxResource)
{
	DWORD Type = GetXboxCommonResourceType(pXboxResource);
	switch (Type)
	{
	case X_D3DCOMMON_TYPE_VERTEXBUFFER:
		return XTL::X_D3DRTYPE_VERTEXBUFFER;
	case X_D3DCOMMON_TYPE_INDEXBUFFER:
		return XTL::X_D3DRTYPE_INDEXBUFFER;
	case X_D3DCOMMON_TYPE_PUSHBUFFER:
		return XTL::X_D3DRTYPE_PUSHBUFFER;
	case X_D3DCOMMON_TYPE_PALETTE:
		return XTL::X_D3DRTYPE_PALETTE;
	case X_D3DCOMMON_TYPE_TEXTURE:
	{
		DWORD Format = ((XTL::X_D3DPixelContainer *)pXboxResource)->Format;
		if (Format & X_D3DFORMAT_CUBEMAP)
			return XTL::X_D3DRTYPE_CUBETEXTURE;

		if (GetXboxPixelContainerDimensionCount((XTL::X_D3DPixelContainer *)pXboxResource) > 2)
			return XTL::X_D3DRTYPE_VOLUMETEXTURE;

		return XTL::X_D3DRTYPE_TEXTURE;
	}
	case X_D3DCOMMON_TYPE_SURFACE:
	{
		if (GetXboxPixelContainerDimensionCount((XTL::X_D3DPixelContainer *)pXboxResource) > 2)
			return XTL::X_D3DRTYPE_VOLUME;

		return XTL::X_D3DRTYPE_SURFACE;
	}
	case X_D3DCOMMON_TYPE_FIXUP:
		return XTL::X_D3DRTYPE_FIXUP;
	}

	return XTL::X_D3DRTYPE_NONE;
}

// This can be used to determine if resource Data adddresses
// need the PHYSICAL_MAP_BASE  bit set or cleared
inline bool IsResourceTypeGPUReadable(const DWORD ResourceType)
{
	switch (ResourceType) {
	case X_D3DCOMMON_TYPE_VERTEXBUFFER:
		return true;
	case X_D3DCOMMON_TYPE_INDEXBUFFER:
		/// assert(false); // Index buffers are not allowed to be registered
		break;
	case X_D3DCOMMON_TYPE_PUSHBUFFER:
		return false;
	case X_D3DCOMMON_TYPE_PALETTE:
		return true;
	case X_D3DCOMMON_TYPE_TEXTURE:
		return true;
	case X_D3DCOMMON_TYPE_SURFACE:
		return true;
	case X_D3DCOMMON_TYPE_FIXUP:
		// assert(false); // Fixup's are not allowed to be registered
		break;
	default:
		CxbxKrnlCleanup("Unhandled resource type");
	}

	return false;
}

inline bool IsPaletizedTexture(const XTL::DWORD XboxPixelContainer_Format)
{
	return GetXboxPixelContainerFormat(XboxPixelContainer_Format) == XTL::X_D3DFMT_P8;
}

#if 0 // unused
inline bool IsYuvSurfaceOrTexture(const XTL::X_D3DResource* pXboxResource)
{
	if (GetXboxPixelContainerFormat((XTL::X_D3DPixelContainer *)pXboxResource) == XTL::X_D3DFMT_YUY2)
		return true;

	return false;
}
#endif

#if 0 // unused
inline bool IsXboxResourceLocked(const XTL::X_D3DResource *pXboxResource)
{
	bool result = !!(pXboxResource->Common & X_D3DCOMMON_ISLOCKED);
	return result;
}
#endif

#if 0 // unused
inline bool IsXboxResourceD3DCreated(const XTL::X_D3DResource *pXboxResource)
{
	bool result = !!(pXboxResource->Common & X_D3DCOMMON_D3DCREATED);
	return result;
}
#endif

void *GetDataFromXboxResource(XTL::X_D3DResource *pXboxResource)
{
	// Don't pass in unassigned Xbox resources
	if (pXboxResource == xbnullptr)
		return nullptr;

	xbaddr pData = pXboxResource->Data;
	if (pData == xbnull)
		return nullptr;

	DWORD dwCommonType = GetXboxCommonResourceType(pXboxResource);
	if (IsResourceTypeGPUReadable(dwCommonType))
		pData |= PHYSICAL_MAP_BASE;

	return (uint8_t*)pData;
}

typedef struct {
	IDirect3DResource* pHostResource = nullptr;
	XTL::X_D3DResource* pXboxResource = xbnullptr;
	DWORD dwXboxResourceType = 0;
	void* pXboxData = xbnullptr;
	size_t szXboxDataSize = 0;
	uint64_t hash = 0;
	bool forceRehash = false;
	std::chrono::time_point<std::chrono::high_resolution_clock> nextHashTime;
	std::chrono::milliseconds hashLifeTime = 1ms;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastUpdate;
} resource_info_t;

typedef std::unordered_map<resource_key_t, resource_info_t, resource_key_hash> resource_cache_t;
resource_cache_t g_Cxbx_Cached_Direct3DResources;
resource_cache_t g_Cxbx_Cached_PaletizedTextures;

bool IsResourceAPixelContainer(XTL::DWORD XboxResource_Common)
{
	DWORD Type = GetXboxCommonResourceType(XboxResource_Common);
	switch (Type)
	{
	case X_D3DCOMMON_TYPE_TEXTURE:
	case X_D3DCOMMON_TYPE_SURFACE:
		return true;
	}

	return false;
}

bool IsResourceAPixelContainer(XTL::X_D3DResource* pXboxResource)
{
	// Don't pass in unassigned Xbox resources
	assert(pXboxResource != xbnullptr);

	return IsResourceAPixelContainer(pXboxResource->Common);
}

resource_cache_t& GetResourceCache(resource_key_t& key)
{
	return IsResourceAPixelContainer(key.Common) && IsPaletizedTexture(key.Format)
		? g_Cxbx_Cached_PaletizedTextures : g_Cxbx_Cached_Direct3DResources;
}

resource_key_t GetHostResourceKey(XTL::X_D3DResource* pXboxResource, int iTextureStage = -1)
{
	resource_key_t key = {};
	if (pXboxResource != xbnullptr) {
		// Initially, don't base the key on the address of the resource, but on it's uniquely identifying values
		key.Data = pXboxResource->Data;
		key.Common = pXboxResource->Common & CXBX_D3DCOMMON_IDENTIFYING_MASK;
		if (IsResourceAPixelContainer(pXboxResource)) {
			// Pixel containers have more values they must be identified by:
			auto pPixelContainer = (XTL::X_D3DPixelContainer*)pXboxResource;
			key.Format = pPixelContainer->Format;
			key.Size = pPixelContainer->Size;
			// For paletized textures, include the current palette hash as well
			if (IsPaletizedTexture(pPixelContainer->Format)) {
				if (iTextureStage < 0) {
					// ForceResourceRehash (called by Lock[23]DSurface) could hit this (not knowing the texture-stage)
					LOG_TEST_CASE("Unknown texture stage!");
				} else {
					assert(iTextureStage < XTL::X_D3DTS_STAGECOUNT);
					// Protect for when this gets hit before an actual palette is set
					if (g_Xbox_Palette_Size[iTextureStage] > 0) {
						// This caters for palette changes (only the active one will be used,
						// any intermediate changes have no effect). Obsolete palette texture
						// conversions will be pruned from g_Cxbx_Cached_PaletizedTextures
						key.PaletteHash = ComputeHash(g_pXbox_Palette_Data[iTextureStage], g_Xbox_Palette_Size[iTextureStage]);
					}
				}
			}
		} else {
			// For other resource types, do include their Xbox resource address (TODO : come up with something better)
			key.ResourceAddr = (xbaddr)pXboxResource;
		}
	}

	return key;
}

void FreeHostResource(resource_key_t key)
{
	// Release the host resource and remove it from the list
	auto& ResourceCache = GetResourceCache(key);
	auto hostResourceIterator = ResourceCache.find(key);
	if (hostResourceIterator != ResourceCache.end()) {
		if (hostResourceIterator->second.pHostResource) {
			(hostResourceIterator->second.pHostResource)->Release();
		}

		ResourceCache.erase(hostResourceIterator);
	}
}

void ClearResourceCache(resource_cache_t& ResourceCache)
{
	for (auto& hostResourceIterator : ResourceCache) {
		if (hostResourceIterator.second.pHostResource) {
			(hostResourceIterator.second.pHostResource)->Release();
		}
	}

	ResourceCache.clear();
}

void PrunePaletizedTexturesCache()
{
	// TODO : Implement a better cache eviction algorithm (like least-recently used, or just at-random)
	// Poor mans cache eviction policy: just clear it once it overflows
	if (g_Cxbx_Cached_PaletizedTextures.size() >= 1500) {
		ClearResourceCache(g_Cxbx_Cached_PaletizedTextures);
	}
}

void ForceResourceRehash(XTL::X_D3DResource* pXboxResource)
{
	auto key = GetHostResourceKey(pXboxResource); // Note : iTextureStage is unknown here!
	auto& ResourceCache = GetResourceCache(key);
	auto it = ResourceCache.find(key);
	if (it != ResourceCache.end() && it->second.pHostResource) {
		it->second.forceRehash = true;
	}
}

IDirect3DResource *GetHostResource(XTL::X_D3DResource *pXboxResource, DWORD D3DUsage = 0, int iTextureStage = -1)
{
	if (pXboxResource == xbnullptr || pXboxResource->Data == xbnull)
		return nullptr;

	EmuVerifyResourceIsRegistered(pXboxResource, D3DUsage, iTextureStage, /*dwSize=*/0);

	auto key = GetHostResourceKey(pXboxResource, iTextureStage);
	auto& ResourceCache = GetResourceCache(key);
	auto it = ResourceCache.find(key);
	if (it == ResourceCache.end() || !it->second.pHostResource) {
		EmuLog(LOG_LEVEL::WARNING, "GetHostResource: Resource not registered or does not have a host counterpart!");
		return nullptr;
	}

	return it->second.pHostResource;
}

// Forward declaration of CxbxGetPixelContainerMeasures to prevent
// polluting the diff too much by reshuffling functions around
VOID CxbxGetPixelContainerMeasures
(
	XTL::X_D3DPixelContainer *pPixelContainer,
	DWORD dwMipMapLevel,
	UINT *pWidth,
	UINT *pHeight,
	UINT *pDepth,
	UINT *pRowPitch,
	UINT *pSlicePitch
);

size_t GetXboxResourceSize(XTL::X_D3DResource* pXboxResource)
{
	// TODO: Smart size calculation based around format of resource
	if (IsResourceAPixelContainer(pXboxResource)) {
		unsigned int Width, Height, Depth, RowPitch, SlicePitch;
		// TODO : Accumulate all mipmap levels!!!
		CxbxGetPixelContainerMeasures(
			(XTL::X_D3DPixelContainer*)pXboxResource,
			0, // dwMipMapLevel
			&Width,
			&Height,
			&Depth,
			&RowPitch,
			&SlicePitch
		);

		return SlicePitch * Depth;
	} else {
		// Fallback to querying the allocation size, if no other calculation was present
		return xboxkrnl::MmQueryAllocationSize(GetDataFromXboxResource(pXboxResource));
	}
	
}

bool HostResourceRequiresUpdate(resource_key_t key, DWORD dwSize)
{
	auto& ResourceCache = GetResourceCache(key);
	auto it = ResourceCache.find(key);
	if (it == ResourceCache.end() || !it->second.pXboxResource) {
		return false;
	}

	// Currently, we only dynamically update Textures and Surfaces, so if our resource
	// isn't of these types, do nothing
	if (!IsResourceAPixelContainer(it->second.pXboxResource)) {
		return false;
	}

	// If the resource size got bigger, we need to re-create it
	// if it got smaller, just hashing will suffice
	if (dwSize > it->second.szXboxDataSize) {
		return true;
	}

	// If the resource type changed, we need to re-create it
	if (it->second.dwXboxResourceType != GetXboxCommonResourceType(it->second.pXboxResource)) {
		return true;
	}

	bool modified = false;

	auto now = std::chrono::high_resolution_clock::now();
	if (now > it->second.nextHashTime || it->second.forceRehash) {
		uint64_t oldHash = it->second.hash;
		it->second.hash = ComputeHash(it->second.pXboxData, it->second.szXboxDataSize);

		if (it->second.hash != oldHash) {
			// The data changed, so reset the hash lifetime
			it->second.hashLifeTime = 1ms;
            it->second.lastUpdate = now;
			modified = true;
		} else if (it->second.lastUpdate + 1000ms < now) {
			// The data did not change, so increase the hash lifetime
			// TODO: choose a sensible upper limit
			if (it->second.hashLifeTime < 1000ms) {
				it->second.hashLifeTime += 10ms;
			}
		}
		
		it->second.forceRehash = false;
	}

	// Update the next hash time based on the hash lifetime
	it->second.nextHashTime = now + it->second.hashLifeTime;

	return modified;
}

void SetHostResource(XTL::X_D3DResource* pXboxResource, IDirect3DResource* pHostResource, int iTextureStage = -1, DWORD dwSize = 0)
{
	auto key = GetHostResourceKey(pXboxResource, iTextureStage);
	auto& ResourceCache = GetResourceCache(key);
	auto& resourceInfo = ResourceCache[key];	// Implicitely inserts a new entry if not already existing

	if (resourceInfo.pHostResource) {
		EmuLog(LOG_LEVEL::WARNING, "SetHostResource: Overwriting an existing host resource");
	}

	resourceInfo.pHostResource = pHostResource;
	resourceInfo.pXboxResource = pXboxResource;
	resourceInfo.dwXboxResourceType = GetXboxCommonResourceType(pXboxResource);
	resourceInfo.pXboxData = GetDataFromXboxResource(pXboxResource);
	resourceInfo.szXboxDataSize = dwSize > 0 ? dwSize : GetXboxResourceSize(pXboxResource);
	resourceInfo.hash = ComputeHash(resourceInfo.pXboxData, resourceInfo.szXboxDataSize);
	resourceInfo.hashLifeTime = 1ms;
	resourceInfo.lastUpdate = std::chrono::high_resolution_clock::now();
	resourceInfo.nextHashTime = resourceInfo.lastUpdate + resourceInfo.hashLifeTime;
	resourceInfo.forceRehash = false;
}

IDirect3DSurface *GetHostSurface(XTL::X_D3DResource *pXboxResource, DWORD D3DUsage = 0)
{
	if (pXboxResource == xbnullptr)
		return nullptr;

	if (GetXboxCommonResourceType(pXboxResource) != X_D3DCOMMON_TYPE_SURFACE) // Allows breakpoint below
		assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_SURFACE);

	return (IDirect3DSurface*) GetHostResource(pXboxResource, D3DUsage);
}

IDirect3DBaseTexture *GetHostBaseTexture(XTL::X_D3DResource *pXboxResource, DWORD D3DUsage = 0, int iTextureStage = 0)
{
	if (pXboxResource == xbnullptr)
		return nullptr;

	if (GetXboxCommonResourceType(pXboxResource) != X_D3DCOMMON_TYPE_TEXTURE) { // Allows breakpoint below
		// test-case : Burnout and Outrun 2006 hit this case (retrieving a surface instead of a texture)
		// TODO : Surfaces can be set in the texture stages, instead of textures - see preparations in CxbxConvertXboxSurfaceToHostTexture
		// We'll need to wrap the surface somehow before using it as a texture
		LOG_TEST_CASE("GetHostBaseTexture called on a non-texture object");
		return nullptr;
		// Note : We'd like to remove the above and do the following instead,
		// but we can't yet since that seems to cause a "CreateCubeTexture Failed!"
		// regression. The root cause for that seems to stem from the X_D3DRTYPE_SURFACE
		// handling in CreateHostResource.
		//assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_TEXTURE);
	}

	return (IDirect3DBaseTexture*)GetHostResource(pXboxResource, D3DUsage, iTextureStage);
}

#if 0 // unused
IDirect3DTexture *GetHostTexture(XTL::X_D3DResource *pXboxResource, int iTextureStage = 0)
{
	if (pXboxResource == xbnullptr)
		return nullptr;

	return (IDirect3DTexture *)GetHostBaseTexture(pXboxResource, 0, iTextureStage);

	// TODO : Check for 1 face (and 2 dimensions)?
}
#endif

IDirect3DVolumeTexture *GetHostVolumeTexture(XTL::X_D3DResource *pXboxResource, int iTextureStage = 0)
{
	return (IDirect3DVolumeTexture *)GetHostBaseTexture(pXboxResource, 0, iTextureStage);

	// TODO : Check for 1 face (and 2 dimensions)?
}

#if 0 // unused
IDirect3DIndexBuffer *GetHostIndexBuffer(XTL::X_D3DResource *pXboxResource)
{
	if (pXboxResource == xbnullptr)
		return nullptr;

	assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_INDEXBUFFER);

	return (IDirect3DIndexBuffer*)GetHostResource(pXboxResource);
}
#endif

void SetHostSurface(XTL::X_D3DResource *pXboxResource, IDirect3DSurface *pHostSurface, int iTextureStage)
{
	assert(pXboxResource != xbnullptr);
	assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_SURFACE);

	SetHostResource(pXboxResource, (IDirect3DResource*)pHostSurface, iTextureStage);
}

void SetHostTexture(XTL::X_D3DResource *pXboxResource, IDirect3DTexture *pHostTexture, int iTextureStage)
{
	assert(pXboxResource != xbnullptr);
	assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_TEXTURE);

	SetHostResource(pXboxResource, (IDirect3DResource*)pHostTexture, iTextureStage);
}

void SetHostCubeTexture(XTL::X_D3DResource *pXboxResource, IDirect3DCubeTexture *pHostCubeTexture, int iTextureStage)
{
	assert(pXboxResource != xbnullptr);
	assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_TEXTURE);

	SetHostResource(pXboxResource, (IDirect3DResource*)pHostCubeTexture, iTextureStage);
}

void SetHostVolumeTexture(XTL::X_D3DResource *pXboxResource, IDirect3DVolumeTexture *pHostVolumeTexture, int iTextureStage)
{
	assert(pXboxResource != xbnullptr);
	assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_TEXTURE);

	SetHostResource(pXboxResource, (IDirect3DResource*)pHostVolumeTexture, iTextureStage);
}

void SetHostVolume(XTL::X_D3DResource *pXboxResource, IDirect3DVolume *pHostVolume, int iTextureStage)
{
	assert(pXboxResource != xbnullptr);
	assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_TEXTURE);

	SetHostResource(pXboxResource, (IDirect3DResource*)pHostVolume, iTextureStage);
}

void SetHostIndexBuffer(XTL::X_D3DResource *pXboxResource, IDirect3DIndexBuffer *pHostIndexBuffer)
{
	assert(pXboxResource != xbnullptr);
	assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_INDEXBUFFER);

	SetHostResource(pXboxResource, (IDirect3DResource*)pHostIndexBuffer);
}

int XboxD3DPaletteSizeToBytes(const XTL::X_D3DPALETTESIZE Size)
{
/*
	static int lk[4] =
	{
		256 * sizeof(D3DCOLOR),    // D3DPALETTE_256
		128 * sizeof(D3DCOLOR),    // D3DPALETTE_128
		64 * sizeof(D3DCOLOR),     // D3DPALETTE_64
		32 * sizeof(D3DCOLOR)      // D3DPALETTE_32
	};

	return lk[Size];
*/
	return (256 * sizeof(D3DCOLOR)) >> (unsigned)Size;
}

inline XTL::X_D3DPALETTESIZE GetXboxPaletteSize(const XTL::X_D3DPalette *pPalette)
{
	XTL::X_D3DPALETTESIZE PaletteSize = (XTL::X_D3DPALETTESIZE)
		((pPalette->Common & X_D3DPALETTE_COMMON_PALETTESIZE_MASK) >> X_D3DPALETTE_COMMON_PALETTESIZE_SHIFT);

	return PaletteSize;
}

int GetD3DResourceRefCount(IDirect3DResource *EmuResource)
{
	if (EmuResource != nullptr)
	{
		// Get actual reference count by increasing it using AddRef,
		// and relying on the return value of Release (which is
		// probably more reliable than AddRef)
		EmuResource->AddRef();
		return EmuResource->Release();
	}

	return 0;
}

/*
XTL::X_D3DSurface *EmuNewD3DSurface()
{
	XTL::X_D3DSurface *result = (XTL::X_D3DSurface *)g_VMManager.AllocateZeroed(sizeof(XTL::X_D3DSurface));
	result->Common = X_D3DCOMMON_D3DCREATED | X_D3DCOMMON_TYPE_SURFACE | 1; // Set refcount to 1
	return result;
}
*/

VOID CxbxSetPixelContainerHeader
(
	XTL::X_D3DPixelContainer* pPixelContainer,
	DWORD				Common,
	UINT				Width,
	UINT				Height,
	UINT				Levels,
	XTL::X_D3DFORMAT	Format,
	UINT				Dimensions,
	UINT				Pitch
)
{
	// Set X_D3DResource field(s) :
	pPixelContainer->Common = Common;
	// DON'T SET pPixelContainer->Data
	// DON'T SET pPixelContainer->Lock

	// Are Width and Height both a power of two?
	DWORD l2w; _BitScanReverse(&l2w, Width); // MSVC intrinsic; GCC has __builtin_clz
	DWORD l2h; _BitScanReverse(&l2h, Height);
	DWORD l2d = 0; // TODO : Set this via input argument
	if (((1 << l2w) == Width) && ((1 << l2h) == Height)) {
		Width = Height = Pitch = 1; // When setting Format, clear Size field
	}
	else {
		l2w = l2h = l2d = 0; // When setting Size, clear D3DFORMAT_USIZE, VSIZE and PSIZE
	}

	// Set X_D3DPixelContainer field(s) :
	pPixelContainer->Format = 0
		| ((Dimensions << X_D3DFORMAT_DIMENSION_SHIFT) & X_D3DFORMAT_DIMENSION_MASK)
		| (((DWORD)Format << X_D3DFORMAT_FORMAT_SHIFT) & X_D3DFORMAT_FORMAT_MASK)
		| ((Levels << X_D3DFORMAT_MIPMAP_SHIFT) & X_D3DFORMAT_MIPMAP_MASK)
		| ((l2w << X_D3DFORMAT_USIZE_SHIFT) & X_D3DFORMAT_USIZE_MASK)
		| ((l2h << X_D3DFORMAT_VSIZE_SHIFT) & X_D3DFORMAT_VSIZE_MASK)
		| ((l2d << X_D3DFORMAT_PSIZE_SHIFT) & X_D3DFORMAT_PSIZE_MASK)
		;
	pPixelContainer->Size = 0
		| (((Width - 1) /*X_D3DSIZE_WIDTH_SHIFT*/) & X_D3DSIZE_WIDTH_MASK)
		| (((Height - 1) << X_D3DSIZE_HEIGHT_SHIFT) & X_D3DSIZE_HEIGHT_MASK)
		| (((Pitch - 1) << X_D3DSIZE_PITCH_SHIFT) & X_D3DSIZE_PITCH_MASK)
		;
}

unsigned int CxbxGetPixelContainerDepth
(
	XTL::X_D3DPixelContainer *pPixelContainer
)
{
	if (pPixelContainer->Size == 0) {
		DWORD l2d = (pPixelContainer->Format & X_D3DFORMAT_PSIZE_MASK) >> X_D3DFORMAT_PSIZE_SHIFT;
		return  1 << l2d;
	}

	return 1;
}

unsigned int CxbxGetPixelContainerMipMapLevels
(
	XTL::X_D3DPixelContainer *pPixelContainer
)
{
	if (pPixelContainer->Size == 0) {
		return (pPixelContainer->Format & X_D3DFORMAT_MIPMAP_MASK) >> X_D3DFORMAT_MIPMAP_SHIFT;
	}

	return 1;
}

uint32_t GetPixelContainerWidth(XTL::X_D3DPixelContainer *pPixelContainer)
{
	DWORD Size = pPixelContainer->Size;
	uint32_t Result;

	if (Size != 0) {
		Result = ((Size & X_D3DSIZE_WIDTH_MASK) /* >> X_D3DSIZE_WIDTH_SHIFT*/) + 1;
	}
	else {
		DWORD l2w = (pPixelContainer->Format & X_D3DFORMAT_USIZE_MASK) >> X_D3DFORMAT_USIZE_SHIFT;

		Result = 1 << l2w;
	}

	return Result;
}

uint32_t GetPixelContainerHeight(XTL::X_D3DPixelContainer *pPixelContainer)
{
	DWORD Size = pPixelContainer->Size;
	uint32_t Result;

	if (Size != 0) {
		Result = ((Size & X_D3DSIZE_HEIGHT_MASK) >> X_D3DSIZE_HEIGHT_SHIFT) + 1;
	}
	else {
		DWORD l2h = (pPixelContainer->Format & X_D3DFORMAT_VSIZE_MASK) >> X_D3DFORMAT_VSIZE_SHIFT;

		Result = 1 << l2h;
	}

	return Result;
}

VOID CxbxGetPixelContainerMeasures
(
	XTL::X_D3DPixelContainer *pPixelContainer,
	// TODO : Add X_D3DCUBEMAP_FACES argument
	DWORD dwMipMapLevel, // unused - TODO : Use
	UINT *pWidth,
	UINT *pHeight,
	UINT *pDepth,
	UINT *pRowPitch,
	UINT *pSlicePitch
)
{
	DWORD Size = pPixelContainer->Size;
	XTL::X_D3DFORMAT X_Format = GetXboxPixelContainerFormat(pPixelContainer);

	if (Size != 0)
	{
		*pDepth = 1;
		*pWidth = ((Size & X_D3DSIZE_WIDTH_MASK) /* >> X_D3DSIZE_WIDTH_SHIFT*/) + 1;
		*pHeight = ((Size & X_D3DSIZE_HEIGHT_MASK) >> X_D3DSIZE_HEIGHT_SHIFT) + 1;
		*pRowPitch = (((Size & X_D3DSIZE_PITCH_MASK) >> X_D3DSIZE_PITCH_SHIFT) + 1) * X_D3DTEXTURE_PITCH_ALIGNMENT;
	}
	else
	{
		DWORD l2w = (pPixelContainer->Format & X_D3DFORMAT_USIZE_MASK) >> X_D3DFORMAT_USIZE_SHIFT;
		DWORD l2h = (pPixelContainer->Format & X_D3DFORMAT_VSIZE_MASK) >> X_D3DFORMAT_VSIZE_SHIFT;
		DWORD l2d = (pPixelContainer->Format & X_D3DFORMAT_PSIZE_MASK) >> X_D3DFORMAT_PSIZE_SHIFT;
		DWORD dwBPP = EmuXBFormatBitsPerPixel(X_Format);

		*pDepth = 1 << l2d;
		*pHeight = 1 << l2h;
		*pWidth = 1 << l2w;
		*pRowPitch = (*pWidth) * dwBPP / 8;
	}

	*pSlicePitch = (*pRowPitch) * (*pHeight);

	if (EmuXBFormatIsCompressed(X_Format)) {
		*pRowPitch *= 4;
	}
}

void GetSurfaceFaceAndLevelWithinTexture(XTL::X_D3DSurface* pSurface, XTL::X_D3DBaseTexture* pTexture, UINT& Level, D3DCUBEMAP_FACES& Face)
{
    auto pSurfaceData = (uintptr_t)GetDataFromXboxResource(pSurface);
    auto pTextureData = (uintptr_t)GetDataFromXboxResource(pTexture);

    // Fast path: If the data pointers match, this must be the first surface within the texture
    if (pSurfaceData == pTextureData) {
        Level = 0;
        Face = D3DCUBEMAP_FACE_POSITIVE_X;
        return;
    }

    int numLevels = CxbxGetPixelContainerMipMapLevels(pTexture);
    int numFaces = pTexture->Format & X_D3DFORMAT_CUBEMAP ? 6 : 1;

    CxbxGetPixelContainerMipMapLevels(pTexture);

    // First, we need to fetch the dimensions of both the surface and the texture, for use within our calculations
    UINT textureWidth, textureHeight, textureDepth, textureRowPitch, textureSlicePitch;
    CxbxGetPixelContainerMeasures(pTexture, 0, &textureWidth, &textureHeight, &textureDepth, &textureRowPitch, &textureSlicePitch);

    UINT surfaceWidth, surfaceHeight, surfaceDepth, surfaceRowPitch, surfaceSlicePitch;
    CxbxGetPixelContainerMeasures(pSurface, 0, &surfaceWidth, &surfaceHeight, &surfaceDepth, &surfaceRowPitch, &surfaceSlicePitch);

    // Iterate through all faces and levels, until we find a matching pointer
    bool isCompressed = EmuXBFormatIsCompressed(GetXboxPixelContainerFormat(pTexture));
    int minSize = (isCompressed) ? 4 : 1;
    int cubeFaceOffset = 0; int cubeFaceSize = 0;
    auto pData = pTextureData;

    for (int face = D3DCUBEMAP_FACE_POSITIVE_X; face <= numFaces; face++) {
        int mipWidth = textureWidth;
        int mipHeight = textureHeight;
        int mipDepth = textureDepth;
        int mipRowPitch = textureRowPitch;
        int mipDataOffset = 0;

        for (int level = 0; level < numLevels; level++) {
            if (pData == pSurfaceData) {
                Level = level;
                Face = (D3DCUBEMAP_FACES)face;
                return;
            }

            // Calculate size of this mipmap level
            UINT dwMipSize = mipRowPitch * mipHeight;
            if (isCompressed) {
                dwMipSize /= 4;
            }

            // If this is the first face, set the cube face size
            if (face == D3DCUBEMAP_FACE_POSITIVE_X) {
                cubeFaceSize = ROUND_UP(textureDepth * dwMipSize, X_D3DTEXTURE_CUBEFACE_ALIGNMENT);
            }

            // Move to the next mip-map and calculate dimensions for the next iteration
            mipDataOffset += dwMipSize;

            if (mipWidth > minSize) {
                mipWidth /= 2;
                mipRowPitch /= 2;
            }

            if (mipHeight > minSize) {
                mipHeight /= 2;
            }

            if (mipDepth > 1) {
                mipDepth /= 2;
            }
        }

        // Move to the next face
        pData += cubeFaceSize;
    }

    LOG_TEST_CASE("Could not find Surface within Texture, falling back to Level = 0, Face = D3DCUBEMAP_FACE_POSITIVE_X");
    Level = 0;
    Face = D3DCUBEMAP_FACE_POSITIVE_X;
}

// Wrapper function to allow calling without passing a face
void GetSurfaceFaceAndLevelWithinTexture(XTL::X_D3DSurface* pSurface, XTL::X_D3DBaseTexture* pBaseTexture, UINT& Level)
{
    D3DCUBEMAP_FACES face;
    GetSurfaceFaceAndLevelWithinTexture(pSurface, pBaseTexture, Level, face);
}

bool ConvertD3DTextureToARGBBuffer(
	XTL::X_D3DFORMAT X_Format,
	uint8_t *pSrc,
	int SrcWidth, int SrcHeight, int SrcRowPitch, int SrcSlicePitch,
	uint8_t *pDst, int DstRowPitch, int DstSlicePitch,
	unsigned int uiDepth = 1,
	int iTextureStage = 0
)
{
	const FormatToARGBRow ConvertRowToARGB = EmuXBFormatComponentConverter(X_Format);
	if (ConvertRowToARGB == nullptr)
		return false; // Unhandled conversion

	uint8_t *unswizleBuffer = nullptr;
	if (EmuXBFormatIsSwizzled(X_Format)) {
		unswizleBuffer = (uint8_t*)malloc(SrcSlicePitch * uiDepth); // TODO : Reuse buffer when performance is important
		// First we need to unswizzle the texture data
		EmuUnswizzleBox(
			pSrc, SrcWidth, SrcHeight, uiDepth, 
			EmuXBFormatBytesPerPixel(X_Format),
			// Note : use src pitch on dest, because this is an intermediate step :
			unswizleBuffer, SrcRowPitch, SrcSlicePitch
		);
		// Convert colors from the unswizzled buffer
		pSrc = unswizleBuffer;
	}

	int AdditionalArgument;
	if (X_Format == XTL::X_D3DFMT_P8)
		AdditionalArgument = (int)g_pXbox_Palette_Data[iTextureStage];
	else
		AdditionalArgument = DstRowPitch;

	if (EmuXBFormatIsCompressed(X_Format)) {
		// All compressed formats (DXT1, DXT3 and DXT5) encode blocks of 4 pixels on 4 lines
		SrcHeight = (SrcHeight + 3) / 4;
		DstRowPitch *= 4;
	}

	uint8_t *pSrcSlice = pSrc;
	uint8_t *pDstSlice = pDst;
	for (unsigned int z = 0; z < uiDepth; z++) {
		uint8_t *pSrcRow = pSrcSlice;
		uint8_t *pDstRow = pDstSlice;
		for (int y = 0; y < SrcHeight; y++) {
			*(int*)pDstRow = AdditionalArgument; // Dirty hack, to avoid an extra parameter to all conversion callbacks
			ConvertRowToARGB(pSrcRow, pDstRow, SrcWidth);
			pSrcRow += SrcRowPitch;
			pDstRow += DstRowPitch;
		}

		pSrcSlice += SrcSlicePitch;
		pDstSlice += DstSlicePitch;
	}

	if (unswizleBuffer)
		free(unswizleBuffer);

	return true;
}

// Called by WndMain::LoadGameLogo() to load game logo bitmap
uint8_t *ConvertD3DTextureToARGB(
	XTL::X_D3DPixelContainer *pXboxPixelContainer,
    uint8_t *pSrc,
	int *pWidth, int *pHeight,
	int TextureStage // default = 0
)
{
	// Avoid allocating pDest when ConvertD3DTextureToARGBBuffer will fail anyway
	XTL::X_D3DFORMAT X_Format = GetXboxPixelContainerFormat(pXboxPixelContainer);
	const FormatToARGBRow ConvertRowToARGB = EmuXBFormatComponentConverter(X_Format);
	if (ConvertRowToARGB == nullptr)
		return nullptr; // Unhandled conversion

	unsigned int SrcDepth, SrcRowPitch, SrcSlicePitch;
	CxbxGetPixelContainerMeasures(
		pXboxPixelContainer,
		0, // dwMipMapLevel
		(UINT*)pWidth,
		(UINT*)pHeight,
		&SrcDepth,
		&SrcRowPitch,
		&SrcSlicePitch
	);

	// Now we know ConvertD3DTextureToARGBBuffer will do it's thing, allocate the resulting buffer
	int DstDepth = 1; // for now TODO : Use SrcDepth when supporting volume textures
	int DstRowPitch = (*pWidth) * sizeof(DWORD); // = sizeof ARGB pixel. TODO : Is this correct?
	int DstSlicePitch = DstRowPitch * (*pHeight); // TODO : Is this correct?
	int DstSize = DstSlicePitch * DstDepth;
	uint8_t *pDst = (uint8_t *)malloc(DstSize);

	// And convert the source towards that buffer
	/*ignore result*/ConvertD3DTextureToARGBBuffer(
		X_Format,
		pSrc, *pWidth, *pHeight, SrcRowPitch, SrcSlicePitch,
		pDst, DstRowPitch, DstSlicePitch,
		DstDepth,
		TextureStage);

	// NOTE : Caller must take ownership!
	return pDst;
}

// Direct3D initialization (called before emulation begins)
VOID EmuD3DInit()
{
	// create the create device proxy thread
	{
		DWORD dwThreadId;

		CreateThread(nullptr, 0, EmuCreateDeviceProxy, nullptr, 0, &dwThreadId);
		// Ported from Dxbx :
		// If possible, assign this thread to another core than the one that runs Xbox1 code :
		SetThreadAffinityMask(&dwThreadId, g_CPUOthers);
	}

	// Initialise CreateDevice Proxy Data struct
	{
		g_EmuCDPD = {0};
		g_EmuCDPD.Adapter = g_XBVideo.adapter;
		g_EmuCDPD.DeviceType = (g_XBVideo.direct3DDevice == 0) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF;
		g_EmuCDPD.hFocusWindow = g_hEmuWindow;
	}

	// create Direct3D8 and retrieve caps
    {
        // xbox Direct3DCreate8 returns "1" always, so we need our own ptr
        g_pDirect3D = Direct3DCreate(D3D_SDK_VERSION);
        if(g_pDirect3D == nullptr)
            CxbxKrnlCleanup("Could not initialize Direct3D8!");

        g_pDirect3D->GetDeviceCaps(g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType, &g_D3DCaps);

		// Dump Host D3DCaps to log unconditionally
		std::cout << "----------------------------------------\n";
		std::cout << "Host D3DCaps : " << g_D3DCaps << "\n";
		std::cout << "----------------------------------------\n";
	}
}

// cleanup Direct3D
VOID EmuD3DCleanup() {}

// enumeration procedure for locating display device GUIDs
static BOOL WINAPI EmuEnumDisplayDevices(GUID FAR *lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext, HMONITOR hm)
{
    static DWORD dwEnumCount = 0;

    if(dwEnumCount++ == g_EmuCDPD.Adapter + 1)
    {
        g_hMonitor = hm;
        dwEnumCount = 0;
        if(lpGUID != 0)
        {
            memcpy(&g_ddguid, lpGUID, sizeof(GUID));
        }
        else
        {
            memset(&g_ddguid, 0, sizeof(GUID));
        }

        return FALSE;
    }

    return TRUE;
}

// window message processing thread
static DWORD WINAPI EmuRenderWindow(LPVOID lpVoid)
{
	CxbxSetThreadName("Cxbx Render Window");
	SetThreadAffinityMask(GetCurrentThread(), g_CPUOthers);

    // register window class
    {
        LOGBRUSH logBrush = {BS_SOLID, RGB(0,0,0)};

        g_hBgBrush = CreateBrushIndirect(&logBrush);

        WNDCLASSEX wc =
        {
            sizeof(WNDCLASSEX),
            CS_CLASSDC,
            EmuMsgProc,
            0, 0, GetModuleHandle(nullptr),
			0, // TODO : LoadIcon(hmodule, ?)
            LoadCursor(NULL, IDC_ARROW),
            (HBRUSH)(g_hBgBrush), NULL,
            "CxbxRender",
			nullptr
        };

        RegisterClassEx(&wc);
    }

    // create the window
    {
		// Peform selection if running in GUI or kernel mode first.
		HWND hwndParent = (!CxbxKrnl_hEmuParent ? GetDesktopWindow() : CxbxKrnl_hEmuParent);
		DWORD dwStyle = WS_POPUP;
		RECT windowRect = { 0 };

		// Obtain the selected resolution from GUI or full desktop screen in kernel mode.
		if (!GetWindowRect(hwndParent, &windowRect)) {
			// Fall back resolution if failed
			windowRect = { 0, 0, 640, 480 };
		}

		// Then perform additional checks if not running in full screen.
		if (!g_XBVideo.bFullScreen) {

			// If running as kernel mode, force use the xbox's default resolution.
			if (!CxbxKrnl_hEmuParent) {
				// Xbox default resolution (standalone window is resizable by the way)
				windowRect.right = 640;
				windowRect.bottom = 480;
				dwStyle = WS_OVERLAPPEDWINDOW;
			}
			else {
				dwStyle = WS_CHILD;
			}
		}

        g_hEmuWindow = CreateWindow
        (
            "CxbxRender", "Cxbx-Reloaded",
            dwStyle, 
			windowRect.left,
			windowRect.top,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
            hwndParent, NULL, GetModuleHandle(nullptr), nullptr
        );
    }

    ShowWindow(g_hEmuWindow, ((CxbxKrnl_hEmuParent == 0) || g_XBVideo.bFullScreen) ? SW_SHOWDEFAULT : SW_SHOWMAXIMIZED);
    UpdateWindow(g_hEmuWindow);

    if(!g_XBVideo.bFullScreen && (CxbxKrnl_hEmuParent != NULL))
    {
        SetFocus(CxbxKrnl_hEmuParent);
    }

    EmuLog(LOG_LEVEL::DEBUG, "Message-Pump thread is running.");

    SetFocus(g_hEmuWindow);

    DbgConsole *dbgConsole = new DbgConsole();

    // message processing loop
    {
        MSG msg;

        ZeroMemory(&msg, sizeof(msg));

        bool lPrintfOn = g_bPrintfOn;

        g_bRenderWindowActive = true;

        while(msg.message != WM_QUIT)
        {
            if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
				Sleep(0);

                // if we've just switched back to display off, clear buffer & display prompt
                if(!g_bPrintfOn && lPrintfOn)
                {
                    dbgConsole->Reset();
                }

                lPrintfOn = g_bPrintfOn;

                dbgConsole->Process();
            }
        }

        g_bRenderWindowActive = false;

        delete dbgConsole;

        CxbxKrnlCleanup(nullptr);
    }

    return 0;
}

// simple helper function
void ToggleFauxFullscreen(HWND hWnd)
{
    if(g_XBVideo.bFullScreen)
        return;

    static LONG lRestore = 0, lRestoreEx = 0;
    static RECT lRect = {0};

    if(!g_bIsFauxFullscreen)
    {
        if(CxbxKrnl_hEmuParent != NULL)
        {
            SetParent(hWnd, NULL);
        }
        else
        {
            lRestore = GetWindowLong(hWnd, GWL_STYLE);
            lRestoreEx = GetWindowLong(hWnd, GWL_EXSTYLE);

            GetWindowRect(hWnd, &lRect);
        }

        SetWindowLong(hWnd, GWL_STYLE, WS_POPUP);
        ShowWindow(hWnd, SW_MAXIMIZE);
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    }
    else
    {
        if(CxbxKrnl_hEmuParent != NULL)
        {
            SetParent(hWnd, CxbxKrnl_hEmuParent);
            SetWindowLong(hWnd, GWL_STYLE, WS_CHILD);
            ShowWindow(hWnd, SW_MAXIMIZE);
            SetFocus(CxbxKrnl_hEmuParent);
        }
        else
        {
            SetWindowLong(hWnd, GWL_STYLE, lRestore);
            SetWindowLong(hWnd, GWL_EXSTYLE, lRestoreEx);
            ShowWindow(hWnd, SW_RESTORE);
            SetWindowPos(hWnd, HWND_NOTOPMOST, lRect.left, lRect.top, lRect.right - lRect.left, lRect.bottom - lRect.top, 0);
            SetFocus(hWnd);
        }
    }

    g_bIsFauxFullscreen = !g_bIsFauxFullscreen;
}

// rendering window message procedure
static LRESULT WINAPI EmuMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static bool bAutoPaused = false;

    switch(msg)
    {
        case WM_DESTROY:
        {
            DeleteObject(g_hBgBrush);
            PostQuitMessage(0);
            return D3D_OK; // = 0
        }
        break;

		case WM_PAINT:
		{
			if (g_CxbxPrintUEM)
			{
				DrawUEM(hWnd);
			}
		}
		break;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case ID_SYNC_CONFIG_LOGGING:
				{
					// Sync run-time config log settings from GUI process.
					log_sync_config();
					log_generate_active_filter_output(CXBXR_MODULE::CXBXR);
				}
				break;

				case ID_SYNC_CONFIG_INPUT:
				{
					SDL_Event UpdateInputEvent;
					SDL_memset(&UpdateInputEvent, 0, sizeof(SDL_Event));
					UpdateInputEvent.type = Sdl::UpdateInputEvent_t;
					UpdateInputEvent.user.data1 = new int(lParam);
					SDL_PushEvent(&UpdateInputEvent);
				}
				break;

				default:
					break;
			}
		}
		break;

        case WM_SYSKEYDOWN:
        {
            if(wParam == VK_RETURN)
            {
                ToggleFauxFullscreen(hWnd);
            }
            else if(wParam == VK_F4)
            {
                PostMessage(hWnd, WM_CLOSE, 0, 0);
            }
            // NOTE: Windows does not send F10 key message to WM_KEYDOWN.
            // Source: https://docs.microsoft.com/en-us/windows/desktop/inputdev/wm-syskeydown
            else if(wParam == VK_F10)
            {
                ToggleFauxFullscreen(hWnd);
            }
            else
            {
                return DefWindowProc(hWnd, msg, wParam, lParam);
            }
        }
        break;

        case WM_KEYDOWN:
        {
            /*! disable fullscreen if we are set to faux mode, and faux fullscreen is active */
            if(wParam == VK_ESCAPE)
            {
                if(g_XBVideo.bFullScreen)
                {
                    SendMessage(hWnd, WM_CLOSE, 0, 0);
                }
                else if(g_bIsFauxFullscreen)
                {
                    ToggleFauxFullscreen(hWnd);
                }
            }
            else if (wParam == VK_F1)
            {
                VertexBufferConverter.PrintStats();
            }
            else if (wParam == VK_F6)
            {
                // For some unknown reason, F6 isn't handled in WndMain::WndProc
                // sometimes, so detect it and stop emulation from here too :
                SendMessage(hWnd, WM_CLOSE, 0, 0); // See StopEmulation();
            }
            else if(wParam == VK_F8)
            {
                g_bPrintfOn = !g_bPrintfOn;
                g_EmuShared->SetIsKrnlLogEnabled(g_bPrintfOn);
                LOG_THREAD_INIT;
                std::cout << _logThreadPrefix << g_EnumModules2String[to_underlying(CXBXR_MODULE::CXBXR)] << "Enable log is " << g_bPrintfOn << std::endl;
                ipc_send_gui_update(IPC_UPDATE_GUI::LOG_ENABLED, static_cast<UINT>(g_bPrintfOn));
            }
			else if (wParam == VK_F9)
			{
				// Toggle frame-limiting
				g_bHack_UnlockFramerate = !g_bHack_UnlockFramerate;
			}
            else if(wParam == VK_F11)
            {
                if (g_iWireframe++ == 2) {
                    g_iWireframe = 0;
                }

                XboxRenderStates.SetWireFrameMode(g_iWireframe);
            }
            else
            {
                return DefWindowProc(hWnd, msg, wParam, lParam);
            }
        }
        break;

        case WM_SIZE:
        {
            switch(wParam)
            {
                case SIZE_RESTORED:
                case SIZE_MAXIMIZED:
                {
                    if(bAutoPaused)
                    {
                        bAutoPaused = false;
                        CxbxKrnlResume();
                    }
                }
                break;

                case SIZE_MINIMIZED:
                {
                    if(g_XBVideo.bFullScreen)
                        CxbxKrnlCleanup(nullptr);

                    if(!g_bEmuSuspended)
                    {
                        bAutoPaused = true;
                        CxbxKrnlSuspend();
                    }
                }
                break;
            }
        }
        break;

        case WM_CLOSE:
            DestroyWindow(hWnd);
			CxbxKrnlShutDown();
            break;

        case WM_SETFOCUS:
        {
            if(CxbxKrnl_hEmuParent != NULL)
            {
                SetFocus(CxbxKrnl_hEmuParent);
            }
        }
        break;

        case WM_SETCURSOR:
        {
            if(g_XBVideo.bFullScreen || g_bIsFauxFullscreen)
            {
                SetCursor(NULL);
                return S_OK; // = Is not part of D3D8 handling.
            }

            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
        break;

        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return S_OK; // = Is not part of D3D8 handling.
}

std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double, std::nano>> GetNextVBlankTime()
{
	// TODO: Read display frequency from Xbox Display Adapter
	// This is accessed by calling CMiniport::GetRefreshRate(); 
	// This reads from the structure located at CMinpPort::m_CurrentAvInfo
	// This will require at least Direct3D_CreateDevice being unpatched
	// otherwise, m_CurrentAvInfo will never be initialised!
	// 20ms should be used in the case of 50hz
	return std::chrono::steady_clock::now() + 16.6666666667ms;
}


// timing thread procedure
static DWORD WINAPI EmuUpdateTickCount(LPVOID)
{
	CxbxSetThreadName("Cxbx Timing Thread");

    // since callbacks come from here
	InitXboxThread(g_CPUOthers); // avoid Xbox1 core for lowest possible latency

    EmuLog(LOG_LEVEL::DEBUG, "Timing thread is running.");

    // current vertical blank count
    int curvb = 0;

	// Calculate Next VBlank time
	auto nextVBlankTime = GetNextVBlankTime();

    while(true)
    {
		SwitchToThread();

		// If VBlank Interval has passed, trigger VBlank callback
        // Note: This whole code block can be removed once NV2A interrupts are implemented
		// And Both Swap and Present can be ran unpatched
		// Once that is in place, MiniPort + Direct3D will handle this on it's own!
		// We check for LLE flag as NV2A handles it's own VBLANK if LLE is enabled!
		if (!(bLLE_GPU) && std::chrono::steady_clock::now() > nextVBlankTime)
        {
			nextVBlankTime = GetNextVBlankTime();

			// Increment the VBlank Counter and Wake all threads there were waiting for the VBlank to occur
			std::unique_lock<std::mutex> lk(g_VBConditionMutex);
			g_Xbox_VBlankData.VBlank++;
			g_VBConditionVariable.notify_all();

			// TODO: Fixme.  This may not be right...
			g_Xbox_SwapData.SwapVBlank = 1;

            if(g_pXbox_VerticalBlankCallback != xbnullptr)
            {
                    
                g_pXbox_VerticalBlankCallback(&g_Xbox_VBlankData);
                    
            }

            g_Xbox_VBlankData.Swap = 0;

			// TODO: This can't be accurate...
			g_Xbox_SwapData.TimeUntilSwapVBlank = 0;

			// TODO: Recalculate this for PAL version if necessary.
			// Also, we should check the D3DPRESENT_INTERVAL value for accurracy.
		//	g_Xbox_SwapData.TimeBetweenSwapVBlanks = 1/60;
			g_Xbox_SwapData.TimeBetweenSwapVBlanks = 0;
        }
    }
}

void UpdateDepthStencilFlags(IDirect3DSurface *pDepthStencilSurface)
{
	g_bHasDepth = false;
	g_bHasStencil = false;
	if (pDepthStencilSurface != nullptr) {
		D3DSURFACE_DESC Desc;
		pDepthStencilSurface->GetDesc(&Desc);

		switch (Desc.Format) {
		case D3DFMT_D16:
			g_bHasDepth = true;
			break;
		case D3DFMT_D15S1:
			g_bHasDepth = true;
			g_bHasStencil = true;
			break;
		case D3DFMT_D24X8:
			g_bHasDepth = true;
			break;
		case D3DFMT_D24S8:
			g_bHasDepth = true;
			g_bHasStencil = true;
			break;
		case D3DFMT_D24X4S4:
			g_bHasDepth = true;
			g_bHasStencil = true;
			break;
		case D3DFMT_D32:
			g_bHasDepth = true;
			break;
		}
	}
}
// thread dedicated to create devices
static DWORD WINAPI EmuCreateDeviceProxy(LPVOID)
{
	LOG_FUNC();

	CxbxSetThreadName("Cxbx CreateDevice Proxy");

    EmuLog(LOG_LEVEL::DEBUG, "CreateDevice proxy thread is running.");

    while(true)
    {
        // if we have been signalled, create the device with cached parameters
        if(g_EmuCDPD.bReady)
        {
            EmuLog(LOG_LEVEL::DEBUG, "CreateDevice proxy thread received request.");

            // only one device should be created at once
            if (g_pD3DDevice != nullptr) {
                EmuLog(LOG_LEVEL::DEBUG, "CreateDevice proxy thread releasing old Device.");

                g_pD3DDevice->EndScene();

				ClearResourceCache(g_Cxbx_Cached_PaletizedTextures);
				ClearResourceCache(g_Cxbx_Cached_Direct3DResources);

				// TODO: ensure all other resources are cleaned up too

				g_EmuCDPD.hRet = g_pD3DDevice->Release();
				g_pD3DDevice = nullptr;

				// cleanup overlay clipper
				if (g_pDDClipper != nullptr) {
					g_pDDClipper->Release();
					g_pDDClipper = nullptr;
				}

				// cleanup directdraw surface
				if (g_pDDSPrimary != nullptr) {
					g_pDDSPrimary->Release();
					g_pDDSPrimary = nullptr;
				}

				// cleanup directdraw
				if (g_pDD7 != nullptr) {
					g_pDD7->Release();
					g_pDD7 = nullptr;
				}
			}

            if (g_EmuCDPD.bCreate) {
				// Apply render scale factor for high-resolution rendering
				g_RenderScaleFactor = g_XBVideo.renderScaleFactor;

                if(g_EmuCDPD.XboxPresentationParameters.BufferSurfaces[0] != xbnullptr)
                    EmuLog(LOG_LEVEL::WARNING, "BufferSurfaces[0] : 0x%.08X", g_EmuCDPD.XboxPresentationParameters.BufferSurfaces[0]);

                if(g_EmuCDPD.XboxPresentationParameters.DepthStencilSurface != xbnullptr)
                    EmuLog(LOG_LEVEL::WARNING, "DepthStencilSurface : 0x%.08X", g_EmuCDPD.XboxPresentationParameters.DepthStencilSurface);

				// Make a binary copy of the Xbox D3DPRESENT_PARAMETERS
				memcpy(&g_EmuCDPD.HostPresentationParameters, &(g_EmuCDPD.XboxPresentationParameters), sizeof(D3DPRESENT_PARAMETERS));

				// make adjustments to parameters to make sense with windows Direct3D
                {
                    g_EmuCDPD.HostPresentationParameters.Windowed = !g_XBVideo.bFullScreen;

                    // TODO: Investigate the best option for this
                    g_EmuCDPD.HostPresentationParameters.SwapEffect = D3DSWAPEFFECT_COPY;

                    g_EmuCDPD.HostPresentationParameters.BackBufferFormat       = EmuXB2PC_D3DFormat(g_EmuCDPD.XboxPresentationParameters.BackBufferFormat);
					g_EmuCDPD.HostPresentationParameters.AutoDepthStencilFormat = EmuXB2PC_D3DFormat(g_EmuCDPD.XboxPresentationParameters.AutoDepthStencilFormat);

					g_EmuCDPD.HostPresentationParameters.PresentationInterval = g_XBVideo.bVSync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
					g_Xbox_PresentationInterval_Default = g_EmuCDPD.XboxPresentationParameters.PresentationInterval;

					// HACK: Disable Tripple Buffering for now...
					// TODO: Enumerate maximum BackBufferCount if possible.
					if(g_EmuCDPD.XboxPresentationParameters.BackBufferCount > 1)
					{
						EmuLog(LOG_LEVEL::WARNING, "Limiting BackBufferCount to 1...");
						g_EmuCDPD.HostPresentationParameters.BackBufferCount = 1;
					}

					// We disable multisampling on the host backbuffer completely for now
					// It causes issues with backbuffer locking.
					// NOTE: multisampling is could potentially be implemented by having the host backbuffer normal size
 					// the Xbox backbuffer being multisampled and scaled during blit
					g_EmuCDPD.HostPresentationParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
					g_EmuCDPD.HostPresentationParameters.MultiSampleQuality = 0;

					/*
                    if(g_EmuCDPD.XboxPresentationParameters.MultiSampleType != 0) {
                        // TODO: Check card for multisampling abilities
                        if(g_EmuCDPD.XboxPresentationParameters.MultiSampleType == XTL::X_D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX) // = 0x00001121 = 4385
							// Test-case : Galleon
                            g_EmuCDPD.HostPresentationParameters.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
						else {
							// CxbxKrnlCleanup("Unknown MultiSampleType (0x%.08X)", g_EmuCDPD.XboxPresentationParameters.MultiSampleType);
							EmuLog(LOG_LEVEL::WARNING, "MultiSampleType 0x%.08X is not supported!", g_EmuCDPD.XboxPresentationParameters.MultiSampleType);
							g_EmuCDPD.HostPresentationParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
						}
                    }*/

                    g_EmuCDPD.HostPresentationParameters.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

                    // retrieve resolution from configuration
                    if(g_EmuCDPD.HostPresentationParameters.Windowed)
                    {
						const char* resolution = g_XBVideo.szVideoResolution;
						if (2 != sscanf(resolution, "%u x %u", &g_EmuCDPD.HostPresentationParameters.BackBufferWidth, &g_EmuCDPD.HostPresentationParameters.BackBufferHeight)) {
							EmuLog(LOG_LEVEL::DEBUG, "EmuCreateDeviceProxy: Couldn't parse resolution : %s.", resolution);
						}
						else {
							if (g_EmuCDPD.HostPresentationParameters.BackBufferWidth == 640)
								EmuLog(LOG_LEVEL::DEBUG, "EmuCreateDeviceProxy: Default width wasn't updated.");
							if (g_EmuCDPD.HostPresentationParameters.BackBufferWidth == 480)
								EmuLog(LOG_LEVEL::DEBUG, "EmuCreateDeviceProxy: Default height wasn't updated.");
						}

                        D3DDISPLAYMODE D3DDisplayMode;

                        g_pDirect3D->GetAdapterDisplayMode(g_EmuCDPD.Adapter, &D3DDisplayMode);

                        g_EmuCDPD.HostPresentationParameters.BackBufferFormat = D3DDisplayMode.Format;
                        g_EmuCDPD.HostPresentationParameters.FullScreen_RefreshRateInHz = 0;
                    }
                    else
                    {
                        char szBackBufferFormat[16] = {};

						const char* resolution = g_XBVideo.szVideoResolution;
						if (4 != sscanf(resolution, "%u x %u %*dbit %s (%u hz)",
							&g_EmuCDPD.HostPresentationParameters.BackBufferWidth,
							&g_EmuCDPD.HostPresentationParameters.BackBufferHeight,
							szBackBufferFormat,
							&g_EmuCDPD.HostPresentationParameters.FullScreen_RefreshRateInHz)) {
							EmuLog(LOG_LEVEL::DEBUG, "EmuCreateDeviceProxy: Couldn't parse resolution : %s.", resolution);
						}
						else {
							if (g_EmuCDPD.HostPresentationParameters.BackBufferWidth == 640)
								EmuLog(LOG_LEVEL::DEBUG, "EmuCreateDeviceProxy: Default width wasn't updated.");
							if (g_EmuCDPD.HostPresentationParameters.BackBufferWidth == 480)
								EmuLog(LOG_LEVEL::DEBUG, "EmuCreateDeviceProxy: Default height wasn't updated.");
						}

                        if(strcmp(szBackBufferFormat, "x1r5g5b5") == 0)
							g_EmuCDPD.HostPresentationParameters.BackBufferFormat = D3DFMT_X1R5G5B5;
                        else if(strcmp(szBackBufferFormat, "r5g6r5") == 0)
							g_EmuCDPD.HostPresentationParameters.BackBufferFormat = D3DFMT_R5G6B5;
                        else if(strcmp(szBackBufferFormat, "x8r8g8b8") == 0)
							g_EmuCDPD.HostPresentationParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
                        else if(strcmp(szBackBufferFormat, "a8r8g8b8") == 0)
							g_EmuCDPD.HostPresentationParameters.BackBufferFormat = D3DFMT_A8R8G8B8;
                    }
                }

                // detect vertex processing capabilities
                if((g_D3DCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) && g_EmuCDPD.DeviceType == D3DDEVTYPE_HAL)
                {
                    EmuLog(LOG_LEVEL::DEBUG, "Using hardware vertex processing");

                    g_EmuCDPD.BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
                    // Unused : g_dwVertexShaderUsage = 0;
                }
                else
                {
                    EmuLog(LOG_LEVEL::DEBUG, "Using software vertex processing");

                    g_EmuCDPD.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
                    // Unused : g_dwVertexShaderUsage = D3DUSAGE_SOFTWAREPROCESSING;
                }

				// Dxbx addition : Prevent Direct3D from changing the FPU Control word :
				g_EmuCDPD.BehaviorFlags |= D3DCREATE_FPU_PRESERVE;

                // Direct3D8: (WARN) :Device that was created without D3DCREATE_MULTITHREADED is being used by a thread other than the creation thread.
                g_EmuCDPD.BehaviorFlags |= D3DCREATE_MULTITHREADED;

				// If a depth stencil format is set, enable AutoDepthStencil
				if (g_EmuCDPD.HostPresentationParameters.AutoDepthStencilFormat != 0) {
					g_EmuCDPD.HostPresentationParameters.EnableAutoDepthStencil = TRUE;
				}

				// If the depth stencil format is unsupported by the host, use a sensible fallback
				if (g_pDirect3D->CheckDeviceFormat(
					D3DADAPTER_DEFAULT,
					D3DDEVTYPE_HAL,
					g_EmuCDPD.HostPresentationParameters.BackBufferFormat,
					D3DUSAGE_DEPTHSTENCIL,
					D3DRTYPE_SURFACE,
					g_EmuCDPD.HostPresentationParameters.AutoDepthStencilFormat) != D3D_OK) {
					g_EmuCDPD.HostPresentationParameters.AutoDepthStencilFormat = D3DFMT_D24S8;
				}

				// For some reason, D3DFMT_D16_LOCKABLE as the AudoDepthStencil causes CreateDevice to fail...
				if (g_EmuCDPD.HostPresentationParameters.AutoDepthStencilFormat == D3DFMT_D16_LOCKABLE) {
					g_EmuCDPD.HostPresentationParameters.AutoDepthStencilFormat = D3DFMT_D16;
				}

				// DirectX9 doesn't support 0 as a swap effect
				if (g_EmuCDPD.HostPresentationParameters.SwapEffect == 0) {
					g_EmuCDPD.HostPresentationParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
				}

                // redirect to windows Direct3D
                g_EmuCDPD.hRet = g_pDirect3D->CreateDevice(
                    g_EmuCDPD.Adapter,
                    g_EmuCDPD.DeviceType,
                    g_EmuCDPD.hFocusWindow,
                    g_EmuCDPD.BehaviorFlags,
                    &g_EmuCDPD.HostPresentationParameters,
                    &g_pD3DDevice
                );
				DEBUG_D3DRESULT(g_EmuCDPD.hRet, "IDirect3D::CreateDevice");

                if(FAILED(g_EmuCDPD.hRet))
                    CxbxKrnlCleanup("IDirect3D::CreateDevice failed");

				// Which texture formats does this device support?
				memset(g_bSupportsFormatSurface, false, sizeof(g_bSupportsFormatSurface));
				memset(g_bSupportsFormatSurfaceRenderTarget, false, sizeof(g_bSupportsFormatSurfaceRenderTarget));
				memset(g_bSupportsFormatSurfaceDepthStencil, false, sizeof(g_bSupportsFormatSurfaceDepthStencil));
				memset(g_bSupportsFormatTexture, false, sizeof(g_bSupportsFormatTexture));
				memset(g_bSupportsFormatTextureRenderTarget, false, sizeof(g_bSupportsFormatTextureRenderTarget));
				memset(g_bSupportsFormatTextureDepthStencil, false, sizeof(g_bSupportsFormatTextureDepthStencil));
				memset(g_bSupportsFormatVolumeTexture, false, sizeof(g_bSupportsFormatVolumeTexture));
				memset(g_bSupportsFormatCubeTexture, false, sizeof(g_bSupportsFormatCubeTexture));
				for (int X_Format = XTL::X_D3DFMT_L8; X_Format <= XTL::X_D3DFMT_LIN_R8G8B8A8; X_Format++) {
					// Only process Xbox formats that are directly mappable to host
					if (!EmuXBFormatRequiresConversionToARGB((XTL::X_D3DFORMAT)X_Format)) {
						// Convert the Xbox format into host format (without warning, thanks to the above restriction)
						D3DFORMAT PCFormat = EmuXB2PC_D3DFormat((XTL::X_D3DFORMAT)X_Format);
						if (PCFormat != D3DFMT_UNKNOWN) {
							// Index with Xbox D3DFormat, because host FourCC codes are too big to be used as indices
							if (D3D_OK == g_pDirect3D->CheckDeviceFormat(
									g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
									g_EmuCDPD.HostPresentationParameters.BackBufferFormat, 0,
									D3DRTYPE_SURFACE, PCFormat))
								g_bSupportsFormatSurface[X_Format] = true;
							if (D3D_OK == g_pDirect3D->CheckDeviceFormat(
									g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
									g_EmuCDPD.HostPresentationParameters.BackBufferFormat, D3DUSAGE_RENDERTARGET,
									D3DRTYPE_SURFACE, PCFormat))
								g_bSupportsFormatSurfaceRenderTarget[X_Format] = true;
							if (D3D_OK == g_pDirect3D->CheckDeviceFormat(
									g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
									g_EmuCDPD.HostPresentationParameters.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
									D3DRTYPE_SURFACE, PCFormat))
								g_bSupportsFormatSurfaceDepthStencil[X_Format] = true;
							if (D3D_OK == g_pDirect3D->CheckDeviceFormat(
									g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
									g_EmuCDPD.HostPresentationParameters.BackBufferFormat, 0,
									D3DRTYPE_TEXTURE, PCFormat))
								g_bSupportsFormatTexture[X_Format] = true;
							if (D3D_OK == g_pDirect3D->CheckDeviceFormat(
									g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
									g_EmuCDPD.HostPresentationParameters.BackBufferFormat, D3DUSAGE_RENDERTARGET,
									D3DRTYPE_TEXTURE, PCFormat))
								g_bSupportsFormatTextureRenderTarget[X_Format] = true;
							if (D3D_OK == g_pDirect3D->CheckDeviceFormat(
									g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
									g_EmuCDPD.HostPresentationParameters.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
									D3DRTYPE_TEXTURE, PCFormat))
								g_bSupportsFormatTextureDepthStencil[X_Format] = true;
							if (D3D_OK == g_pDirect3D->CheckDeviceFormat(
									g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
									g_EmuCDPD.HostPresentationParameters.BackBufferFormat, 0,
									D3DRTYPE_VOLUMETEXTURE, PCFormat))
								g_bSupportsFormatVolumeTexture[X_Format] = true;
							if (D3D_OK == g_pDirect3D->CheckDeviceFormat(
									g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
									g_EmuCDPD.HostPresentationParameters.BackBufferFormat, 0,
									D3DRTYPE_CUBETEXTURE, PCFormat))
								g_bSupportsFormatCubeTexture[X_Format] = true;
						}
					}
				}

                // default NULL guid
                ZeroMemory(&g_ddguid, sizeof(GUID));

				HRESULT hRet;

                // enumerate device guid for this monitor, for directdraw
				hRet = DirectDrawEnumerateExA(EmuEnumDisplayDevices, nullptr, DDENUM_ATTACHEDSECONDARYDEVICES);
				DEBUG_D3DRESULT(hRet, "DirectDrawEnumerateExA");

                // create DirectDraw7
                {
                    if(FAILED(hRet)) {
                        hRet = DirectDrawCreateEx(nullptr, (void**)&g_pDD7, IID_IDirectDraw7, nullptr);
						DEBUG_D3DRESULT(hRet, "DirectDrawCreateEx(NULL)");
					} else {
						hRet = DirectDrawCreateEx(&g_ddguid, (void**)&g_pDD7, IID_IDirectDraw7, nullptr);
						DEBUG_D3DRESULT(hRet, "DirectDrawCreateEx(&g_ddguid)");
					}

					if(FAILED(hRet))
                        CxbxKrnlCleanup("Could not initialize DirectDraw7");

					hRet = g_pDD7->GetCaps(&g_DriverCaps, nullptr);
					DEBUG_D3DRESULT(hRet, "g_pDD7->GetCaps");

                    hRet = g_pDD7->SetCooperativeLevel(0, DDSCL_NORMAL);
					DEBUG_D3DRESULT(hRet, "g_pDD7->SetCooperativeLevel");

                    if(FAILED(hRet))
                        CxbxKrnlCleanup("Could not set cooperative level");
                }

				// Dump all supported DirectDraw FourCC format codes
				{
                    DWORD  dwCodes = 0;
                    DWORD *lpCodes = nullptr;

                    g_pDD7->GetFourCCCodes(&dwCodes, lpCodes);
                    lpCodes = (DWORD*)malloc(dwCodes*sizeof(DWORD));
                    g_pDD7->GetFourCCCodes(&dwCodes, lpCodes);
                    for(DWORD v=0;v<dwCodes;v++)
                    {
						EmuLog(LOG_LEVEL::DEBUG, "FourCC[%d] = %.4s", v, (char *)&(lpCodes[v]));
						// Map known FourCC codes to Xbox Format
						int X_Format;
						switch (lpCodes[v]) {
						case MAKEFOURCC('Y', 'U', 'Y', '2'):
							X_Format = XTL::X_D3DFMT_YUY2;
							break;
						case MAKEFOURCC('U', 'Y', 'V', 'Y'):
							X_Format = XTL::X_D3DFMT_UYVY;
							break;
						case MAKEFOURCC('D', 'X', 'T', '1'):
							X_Format = XTL::X_D3DFMT_DXT1;
							break;
						case MAKEFOURCC('D', 'X', 'T', '3'):
							X_Format = XTL::X_D3DFMT_DXT3;
							break;
						case MAKEFOURCC('D', 'X', 'T', '5'):
							X_Format = XTL::X_D3DFMT_DXT5;
							break;
						default:
							continue;
						}

						// Warn if CheckDeviceFormat didn't report this format
						if (!g_bSupportsFormatTexture[X_Format]) {
							EmuLog(LOG_LEVEL::WARNING, "FourCC format %.4s not previously detected via CheckDeviceFormat()! Enabling it.", (char *)&(lpCodes[v]));
							// TODO : If this warning never shows, detecting FourCC's could be removed entirely. For now, enable the format :
							g_bSupportsFormatTexture[X_Format] = true;
						}
                    }

                    free(lpCodes);						
				}

/* Disabled for now, as this has no other side-effect (other then adding a reference count that's never Released)
                // update render target cache
				IDirect3DSurface *pCurrentHostRenderTarget = nullptr;
                hRet = g_pD3DDevice->GetRenderTarget(
					0, // RenderTargetIndex
					&pCurrentHostRenderTarget);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetRenderTarget");
				// TODO : SetHostSurface(BackBuffer[0], pCurrentHostRenderTarget);
*/

				// update z-stencil surface cache
				g_pD3DDevice->GetDepthStencilSurface(&g_pDefaultHostDepthBufferSurface);
				UpdateDepthStencilFlags(g_pDefaultHostDepthBufferSurface);
		
				// Can host driver create event queries?
				if (SUCCEEDED(g_pD3DDevice->CreateQuery(D3DQUERYTYPE_EVENT, nullptr))) {
					// Is host GPU query creation enabled?
					if (!g_bHack_DisableHostGPUQueries) {
						// Create a D3D event query to handle "wait-for-idle" with
						hRet = g_pD3DDevice->CreateQuery(D3DQUERYTYPE_EVENT, &g_pHostQueryWaitForIdle);
						DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateQuery (wait for idle)");

						// Create a D3D event query to handle "callback events" with
						hRet = g_pD3DDevice->CreateQuery(D3DQUERYTYPE_EVENT, &g_pHostQueryCallbackEvent);
						DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateQuery (callback event)");
					}
				} else {
					LOG_TEST_CASE("Can't CreateQuery on host!");
				}

				hRet = g_pD3DDevice->CreateVertexBuffer
                (
                    1, 0, 0, D3DPOOL_MANAGED,
                    &g_pDummyBuffer
					, nullptr
                );
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateVertexBuffer");

                for(int Streams = 0; Streams < 16; Streams++)
                {
                    hRet = g_pD3DDevice->SetStreamSource(Streams, g_pDummyBuffer,
						0, // OffsetInBytes
						1);
					DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetStreamSource");
				}

                // initially, show a black screen
                // Only clear depth buffer and stencil if present
                //
                // Avoids following DirectX Debug Runtime error report
                //    [424] Direct3D8: (ERROR) :Invalid flag D3DCLEAR_ZBUFFER: no zbuffer is associated with device. Clear failed. 
                //
                hRet = g_pD3DDevice->Clear(
					/*Count=*/0, 
					/*pRects=*/nullptr, 
					D3DCLEAR_TARGET | (g_bHasDepth ? D3DCLEAR_ZBUFFER : 0) | (g_bHasStencil ? D3DCLEAR_STENCIL : 0),
					/*Color=*/0xFF000000, // TODO : Use constant for this
					/*Z=*/g_bHasDepth ? 1.0f : 0.0f,
					/*Stencil=*/0);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->Clear");

				hRet = g_pD3DDevice->BeginScene();
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->BeginScene");

				hRet = g_pD3DDevice->EndScene();
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->EndScene");

				hRet = g_pD3DDevice->Present(0, 0, 0, 0);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->Present");

                // begin scene
                hRet = g_pD3DDevice->BeginScene();
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->BeginScene(2nd)");
            }

            // signal completion
            g_EmuCDPD.bReady = false;
        }

		Sleep(1);
    }

    return 0;
}

// check if a resource has been registered yet (if not, register it)
void CreateHostResource(XTL::X_D3DResource *pResource, DWORD D3DUsage, int iTextureStage, DWORD dwSize); // Forward declartion to prevent restructure of code
static void EmuVerifyResourceIsRegistered(XTL::X_D3DResource *pResource, DWORD D3DUsage, int iTextureStage, DWORD dwSize)
{
	// Skip resources without data
	if (pResource->Data == xbnull)
		return;

	auto key = GetHostResourceKey(pResource, iTextureStage);
	auto& ResourceCache = GetResourceCache(key);
	auto it = ResourceCache.find(key);
	if (it != ResourceCache.end()) {
		if (D3DUsage == D3DUSAGE_RENDERTARGET && IsResourceAPixelContainer(pResource) && EmuXBFormatIsRenderTarget(GetXboxPixelContainerFormat((XTL::X_D3DPixelContainer*)pResource))) {
            // Render targets have special behavior: We can't trash them on guest modification
            // this fixes an issue where CubeMaps were broken because the surface Set in GetCubeMapSurface
            // would be overwritten by the surface created in SetRenderTarget
            // However, if a non-rendertarget surface is used here, we'll need to recreate it as a render target!
            auto hostResource = it->second.pHostResource;
            auto xboxSurface = (XTL::X_D3DSurface*)pResource;
            auto xboxTexture = (XTL::X_D3DTexture*)pResource;
            auto xboxResourceType = GetXboxD3DResourceType(pResource);

            // Determine if the associated host resource is a render target already, if so, do nothing
            HRESULT hRet = STATUS_INVALID_PARAMETER; // Default to an error condition, so we can use D3D_OK to check for success
            D3DSURFACE_DESC surfaceDesc;
            if (xboxResourceType == XTL::X_D3DRTYPE_SURFACE) {
                hRet = ((IDirect3DSurface*)hostResource)->GetDesc(&surfaceDesc);
            } else if (xboxResourceType == XTL::X_D3DRTYPE_TEXTURE) {
                hRet = ((IDirect3DTexture*)hostResource)->GetLevelDesc(0, &surfaceDesc);
            }

            // Only continue checking if we were able to get the surface desc, if it failed, we fall-through
            // to previous resource management behavior
            if (SUCCEEDED(hRet)) {
                // If this resource is already created as a render target on the host, simply return
                if (surfaceDesc.Usage & D3DUSAGE_RENDERTARGET) {
                    return;
                }

                // The host resource is not a render target, but the Xbox surface is
                // We need to re-create it as a render target
                switch (xboxResourceType) {
                    case XTL::X_D3DRTYPE_SURFACE: {
                        // Free the host surface
                        FreeHostResource(key);

                        // Free the parent texture, if present
                        XTL::X_D3DTexture* pParentXboxTexture = (pResource) ? (XTL::X_D3DTexture*)xboxSurface->Parent : xbnullptr;

                        if (pParentXboxTexture) {
                            // Re-create the texture with D3DUSAGE_RENDERTARGET, this will automatically create any child-surfaces
                            FreeHostResource(GetHostResourceKey(pParentXboxTexture, iTextureStage));
                            CreateHostResource(pParentXboxTexture, D3DUsage, iTextureStage, dwSize);
                        }

                        // Re-create the surface with D3DUSAGE_RENDERTARGET
                        CreateHostResource(pResource, D3DUsage, iTextureStage, dwSize);
                    } break;
                    case XTL::X_D3DRTYPE_TEXTURE: {
                        auto xboxTexture = (XTL::X_D3DTexture*)pResource;

                        // Free the host texture
                        FreeHostResource(key);

                        // And re-create the texture with D3DUSAGE_RENDERTARGET
                        CreateHostResource(pResource, D3DUsage, iTextureStage, dwSize);
                    } break;
                    default:
                        LOG_TEST_CASE("Unimplemented rendertarget type");
                }

                return;
            }
		}

        //check if the same key existed in the HostResource map already. if there is a old pXboxResource in the map with the same key but different resource address, it must be freed first.
        
        if (it->second.pXboxResource != pResource) {
            //printf("EmuVerifyResourceIsRegistered passed in XboxResource collides HostResource map!! key : %llX , map pXboxResource : %08X , passed in pResource : %08X \n", key, it->second.pXboxResource, pResource);
        }
        else {
			if (!HostResourceRequiresUpdate(key, dwSize)) {
				return;
			}
		}

		FreeHostResource(key);
	} else {
		resource_info_t newResource;
		ResourceCache[key] = newResource;
	}

	CreateHostResource(pResource, D3DUsage, iTextureStage, dwSize);
}

// TODO : Move to own file
constexpr UINT QuadToTriangleVertexCount(UINT NrOfQuadVertices)
{
	return (NrOfQuadVertices * VERTICES_PER_TRIANGLE * TRIANGLES_PER_QUAD) / VERTICES_PER_QUAD;
}

// TODO : Move to own file (or argument of all users)
bool bUseClockWiseWindingOrder = true; // TODO : Should this be fetched from X_D3DRS_FRONTFACE (or X_D3DRS_CULLMODE)?

// TODO : Move to own file
// This function convertes quad to triangle indices.
// When pXboxQuadIndexData is set, original quad indices are read from this buffer
// (this use-case is for when an indexed quad draw is to be emulated).
// When pXboxQuadIndexData is null, quad-emulating indices are generated
// (this use-case is for when a non-indexed quad draw is to be emulated).
// The number of indices to generate is specified through uNrOfTriangleIndices.
// Resulting triangle indices are written to pTriangleIndexData, which must
// be pre-allocated to fit the output data.
// (Note, this function is marked 'constexpr' to allow the compiler to optimize
// the case when pXboxQuadIndexData is null)
constexpr void CxbxConvertQuadListToTriangleListIndices(
	INDEX16* pXboxQuadIndexData,
	unsigned uNrOfTriangleIndices,
	INDEX16* pTriangleIndexData)
{
	assert(uNrOfTriangleIndices > 0);
	assert(pTriangleIndexData);

	unsigned i = 0;
	unsigned j = 0;
	while (i + (VERTICES_PER_TRIANGLE * TRIANGLES_PER_QUAD) <= uNrOfTriangleIndices) {
		if (bUseClockWiseWindingOrder) {
			// ABCD becomes ABC+CDA, so this is triangle 1 :
			pTriangleIndexData[i + 0] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 0] : j + 0; // A
			pTriangleIndexData[i + 1] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 1] : j + 1; // B
			pTriangleIndexData[i + 2] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 2] : j + 2; // C
			i += VERTICES_PER_TRIANGLE;
			// And this is triangle 2 :
			pTriangleIndexData[i + 0] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 2] : j + 2; // C
			pTriangleIndexData[i + 1] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 3] : j + 3; // D
			pTriangleIndexData[i + 2] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 0] : j + 0; // A
			i += VERTICES_PER_TRIANGLE;
		} else {
			// ABCD becomes ADC+CBA, so this is triangle 1 :
			pTriangleIndexData[i + 0] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 0] : j + 0; // A
			pTriangleIndexData[i + 1] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 3] : j + 3; // D
			pTriangleIndexData[i + 2] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 2] : j + 2; // C
			i += VERTICES_PER_TRIANGLE;
			// And this is triangle 2 :
			pTriangleIndexData[i + 0] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 2] : j + 2; // C
			pTriangleIndexData[i + 1] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 1] : j + 1; // B
			pTriangleIndexData[i + 2] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 0] : j + 0; // A
			i += VERTICES_PER_TRIANGLE;
		}

		// Next quad, please :
		j += VERTICES_PER_QUAD;
	}
}

// TODO : Move to own file
// Called from EMUPATCH(D3DDevice_DrawIndexedVerticesUP) when PrimitiveType == X_D3DPT_QUADLIST.
// This API receives the number of vertices to draw (VertexCount), the index data that references
// vertices and a single stream of vertex data. The number of vertices to draw indicates the number
// of indices that are going to be fetched. The vertex data is referenced up to the highest index
// number present in the index data.
// To emulate drawing indexed quads, g_pD3DDevice->DrawIndexedPrimitiveUP is called on host,
// whereby the quad indices are converted to triangle indices. This implies for every four
// quad indices, we have to generate (two times three is) six triangle indices. (Note, that
// vertex data undergoes it's own Xbox-to-host conversion, independent from these indices.)
INDEX16* CxbxCreateQuadListToTriangleListIndexData(INDEX16* pXboxQuadIndexData, unsigned QuadVertexCount)
{
	UINT NrOfTriangleIndices = QuadToTriangleVertexCount(QuadVertexCount);
	INDEX16* pQuadToTriangleIndexBuffer = (INDEX16*)malloc(NrOfTriangleIndices * sizeof(INDEX16));
	CxbxConvertQuadListToTriangleListIndices(pXboxQuadIndexData, NrOfTriangleIndices, pQuadToTriangleIndexBuffer);
	return pQuadToTriangleIndexBuffer;
}

// TODO : Move to own file
void CxbxReleaseQuadListToTriangleListIndexData(void* pHostIndexData)
{
	free(pHostIndexData);
}

class ConvertedIndexBuffer {
public:
	uint64_t Hash = 0;
	DWORD IndexCount = 0;
	IDirect3DIndexBuffer* pHostIndexBuffer = nullptr;
	INDEX16 LowIndex = 0;
	INDEX16 HighIndex = 0;

	~ConvertedIndexBuffer()
	{
		if (pHostIndexBuffer != nullptr) {
			pHostIndexBuffer->Release();
		}
	}
};

std::unordered_map<uint32_t, ConvertedIndexBuffer> g_IndexBufferCache;
	
void CxbxRemoveIndexBuffer(PWORD pData)
{
	// HACK: Never Free
}

IDirect3DIndexBuffer* CxbxCreateIndexBuffer(unsigned IndexCount)
{
	LOG_INIT; // Allows use of DEBUG_D3DRESULT

	IDirect3DIndexBuffer* Result = nullptr;

	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb147168(v=vs.85).aspx
	// "Managing Resources (Direct3D 9)"
	// suggests "for resources which change with high frequency" [...]
	// "D3DPOOL_DEFAULT along with D3DUSAGE_DYNAMIC should be used."
	const D3DPOOL D3DPool = D3DPOOL_DEFAULT; // Was D3DPOOL_MANAGED
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb172625(v=vs.85).aspx
	// "Buffers created with D3DPOOL_DEFAULT that do not specify D3DUSAGE_WRITEONLY may suffer a severe performance penalty."
	const DWORD D3DUsage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY; // Was D3DUSAGE_WRITEONLY

	// Create a new native index buffer of the requested size :
	UINT uiIndexBufferSize = IndexCount * sizeof(INDEX16);
	HRESULT hRet = g_pD3DDevice->CreateIndexBuffer(
		uiIndexBufferSize, // Size of the index buffer, in bytes.
		D3DUsage,
		/*Format=*/D3DFMT_INDEX16,
		D3DPool,
		&Result,
		nullptr // pSharedHandle
	);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateIndexBuffer");
	if (FAILED(hRet)) {
		assert(Result == nullptr);
	}

	return Result;
}

ConvertedIndexBuffer& CxbxUpdateActiveIndexBuffer
(
	INDEX16* pXboxIndexData,
	unsigned XboxIndexCount,
	bool bConvertQuadListToTriangleList
)
{
	LOG_INIT; // Allows use of DEBUG_D3DRESULT

	uint32_t LookupKey = (uint32_t)pXboxIndexData;
	unsigned RequiredIndexCount = XboxIndexCount;

	if (bConvertQuadListToTriangleList) {
		LOG_TEST_CASE("bConvertQuadListToTriangleList");
		RequiredIndexCount = QuadToTriangleVertexCount(XboxIndexCount);
		// For now, indicate the quad-to-triangles case using the otherwise
		// (due to alignment) always-zero least significant bit :
		LookupKey |= 1;
	}

	// Poor-mans eviction policy : when exceeding treshold, clear entire cache :
	if (g_IndexBufferCache.size() > 256) {
		g_IndexBufferCache.clear(); // Note : ConvertedIndexBuffer destructor will release any assigned pHostIndexBuffer
	}

	// Create a reference to the active buffer
	ConvertedIndexBuffer& CacheEntry = g_IndexBufferCache[LookupKey];

	// If the current index buffer size is too small, free it, so it'll get re-created
	bool bNeedRepopulation = CacheEntry.IndexCount < RequiredIndexCount;
	if (bNeedRepopulation) {
		if (CacheEntry.pHostIndexBuffer != nullptr)
			CacheEntry.pHostIndexBuffer->Release();

		CacheEntry = {};
	}

	// If we need to create an index buffer, do so.
	if (CacheEntry.pHostIndexBuffer == nullptr) {
		CacheEntry.pHostIndexBuffer = CxbxCreateIndexBuffer(RequiredIndexCount);
		if (!CacheEntry.pHostIndexBuffer)
			CxbxKrnlCleanup("CxbxUpdateActiveIndexBuffer: IndexBuffer Create Failed!");
	}

	// TODO : Speeds this up, perhaps by hashing less often, and/or by
	// doing two hashes : a small subset regularly, all data less frequently.
	uint64_t uiHash = ComputeHash(pXboxIndexData, XboxIndexCount * sizeof(INDEX16));

	// If the data needs updating, do so
	bNeedRepopulation |= (uiHash != CacheEntry.Hash);
	if (bNeedRepopulation)	{
		// Update the Index Count and the hash
		CacheEntry.IndexCount = RequiredIndexCount;
		CacheEntry.Hash = uiHash;

		// Update the host index buffer
		INDEX16* pHostIndexBufferData = nullptr;
		HRESULT hRet = CacheEntry.pHostIndexBuffer->Lock(0, /*entire SizeToLock=*/0, (D3DLockData **)&pHostIndexBufferData, D3DLOCK_DISCARD);
		DEBUG_D3DRESULT(hRet, "CacheEntry.pHostIndexBuffer->Lock");
		if (pHostIndexBufferData == nullptr) {
			CxbxKrnlCleanup("CxbxUpdateActiveIndexBuffer: Could not lock index buffer!");
		}

		// Determine highest and lowest index in use :
		WalkIndexBuffer(CacheEntry.LowIndex, CacheEntry.HighIndex, pXboxIndexData, XboxIndexCount);
		if (bConvertQuadListToTriangleList) {
			// Note, that LowIndex and HighIndex won't change due to any quad-to-triangle conversion,
			// so it's less work to WalkIndexBuffer over the input instead of the converted index buffer.
			EmuLog(LOG_LEVEL::DEBUG, "CxbxUpdateActiveIndexBuffer: Converting quads to %d triangle indices (D3DFMT_INDEX16)", RequiredIndexCount);
			CxbxConvertQuadListToTriangleListIndices(pXboxIndexData, RequiredIndexCount, pHostIndexBufferData);
		} else {
			EmuLog(LOG_LEVEL::DEBUG, "CxbxUpdateActiveIndexBuffer: Copying %d indices (D3DFMT_INDEX16)", XboxIndexCount);
			memcpy(pHostIndexBufferData, pXboxIndexData, XboxIndexCount * sizeof(INDEX16));
		}

		CacheEntry.pHostIndexBuffer->Unlock();
	}

	// Activate the new native index buffer :
	HRESULT hRet = g_pD3DDevice->SetIndices(CacheEntry.pHostIndexBuffer);
	// Note : Under Direct3D 9, the BaseVertexIndex argument is moved towards DrawIndexedPrimitive
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetIndices");

	if (FAILED(hRet))
		CxbxKrnlCleanup("CxbxUpdateActiveIndexBuffer: SetIndices Failed!");

	return CacheEntry;
}

void Direct3D_CreateDevice_Start
(
	XTL::X_D3DPRESENT_PARAMETERS     *pPresentationParameters
)
{
    if (!XboxRenderStates.Init()) {
        CxbxKrnlCleanup("Failed to init XboxRenderStates");
    }

    if (!XboxTextureStates.Init(&XboxRenderStates)) {
        CxbxKrnlCleanup("Failed to init XboxTextureStates");
    }

    // Disable multisampling for now, this fixes an issue where GTA3 only renders to half-screen
    // TODO: Find a better way of fixing this, we cannot just create larger backbuffers as it breaks
    // many games, despite working in the dashboard
	pPresentationParameters->MultiSampleType = XTL::X_D3DMULTISAMPLE_NONE;

	// create default device *before* calling Xbox Direct3D_CreateDevice trampline
	// to avoid hitting EMUPATCH'es that need a valid g_pD3DDevice
	{
		// Wait until proxy is done with an existing call (i highly doubt this situation will come up)
		while (g_EmuCDPD.bReady)
			Sleep(10);

		// Cache parameters
		memcpy(&(g_EmuCDPD.XboxPresentationParameters), pPresentationParameters, sizeof(XTL::X_D3DPRESENT_PARAMETERS));

		// Signal proxy thread (this will trigger EmuCreateDeviceProxy to call CreateDevice)
		g_EmuCDPD.bCreate = true;
		g_EmuCDPD.bReady = true;

		// Wait until host proxy is completed (otherwise, Xbox code could hit patches that need an assigned g_pD3DDevice)
		while (g_EmuCDPD.bReady)
			Sleep(10);
	}
}

void Direct3D_CreateDevice_End()
{
#if 0 // Unused :
    // Set g_Xbox_D3DDevice to point to the Xbox D3D Device
    auto it = g_SymbolAddresses.find("D3DDEVICE");
    if (it != g_SymbolAddresses.end()) {
        g_Xbox_D3DDevice = (DWORD*)it->second;
    }
#endif
    // If the Xbox version of CreateDevice didn't call SetRenderTarget, we must derive the default backbuffer ourselves
    // This works because CreateDevice always sets the current render target to the Xbox Backbuffer
    // In later XDKs, it does this inline rather than by calling D3DDevice_SetRenderTarget
    // meaning our patch doesn't always get called in these cases.
    // We fix the situation by calling the Xbox GetRenderTarget function, which immediately after CreateDevice
    // WILL always return the Backbuffer!
    // Test Case: Shin Megami Tensei: Nine
    if (g_pXbox_BackBufferSurface == xbnullptr || g_pXbox_DefaultDepthStencilSurface == xbnullptr) {
        // First, log the test case
        LOG_TEST_CASE("Xbox CreateDevice did not call SetRenderTarget");
    }

    if (g_pXbox_BackBufferSurface == xbnullptr) {
        // Setup trampolines for both versions of D3DDevice_GetRenderTarget
        XB_trampoline(HRESULT, WINAPI, D3DDevice_GetRenderTarget, (XTL::X_D3DSurface**));
        XB_trampoline(XTL::X_D3DSurface*, WINAPI, D3DDevice_GetRenderTarget2, (VOID));
        if (XB_D3DDevice_GetRenderTarget) {
            XB_D3DDevice_GetRenderTarget(&g_pXbox_BackBufferSurface);
        }
        else if (XB_D3DDevice_GetRenderTarget2) {
            g_pXbox_BackBufferSurface = XB_D3DDevice_GetRenderTarget2();
        }

        // At this point, pRenderTarget should now point to a valid render target
        // if it still doesn't, we cannot continue without crashing at draw time
        if (g_pXbox_BackBufferSurface == xbnullptr) {
            CxbxKrnlCleanup("Unable to determine default Xbox backbuffer");
        }

        // We must also call our SetRenderTarget patch to properly setup the host state
        // Update only the Back buffer
        XTL::EMUPATCH(D3DDevice_SetRenderTarget)(g_pXbox_BackBufferSurface, xbnullptr);
    }

    // Now do the same, but for the default depth stencil surface
    if (g_pXbox_DefaultDepthStencilSurface == xbnullptr) {
        // Setup trampolines for both versions of D3DDevice_GetDepthStencilSurface
        XB_trampoline(HRESULT, WINAPI, D3DDevice_GetDepthStencilSurface, (XTL::X_D3DSurface**));
        XB_trampoline(XTL::X_D3DSurface*, WINAPI, D3DDevice_GetDepthStencilSurface2, (VOID));
        if (XB_D3DDevice_GetDepthStencilSurface) {
            XB_D3DDevice_GetDepthStencilSurface(&g_pXbox_DefaultDepthStencilSurface);
        }
        else if (XB_D3DDevice_GetDepthStencilSurface2) {
            g_pXbox_DefaultDepthStencilSurface = XB_D3DDevice_GetDepthStencilSurface2();
        }

        // At this point, g_pXbox_DefaultDepthStencilSurface should now point to a valid depth stencil
        // If it doesn't, just log and carry on: Unlike RenderTarget, this situation is not fatal
        if (g_pXbox_DefaultDepthStencilSurface == xbnullptr) {
            LOG_TEST_CASE("Unable to determine default Xbox depth stencil");
        } else {
            // Update only the depth stencil
            XTL::EMUPATCH(D3DDevice_SetRenderTarget)(xbnullptr, g_pXbox_DefaultDepthStencilSurface);
        }
    }
}

// LTCG specific Direct3D_CreateDevice function...
// This uses a custom calling with parameters passed in eax, ecx and the stack
// Test-case: Ninja Gaiden, Halo 2
HRESULT WINAPI XTL::EMUPATCH(Direct3D_CreateDevice_4)
(
    X_D3DPRESENT_PARAMETERS     *pPresentationParameters
)
{
    DWORD BehaviorFlags;
    IDirect3DDevice **ppReturnedDeviceInterface;

    __asm {
        mov BehaviorFlags, eax
        mov ppReturnedDeviceInterface, ecx
    }

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pPresentationParameters)
		LOG_FUNC_END;

	Direct3D_CreateDevice_Start(pPresentationParameters);

    HRESULT hRet = 0;

	// Only then call Xbox CreateDevice function
	XB_trampoline(HRESULT, WINAPI, Direct3D_CreateDevice_4, (X_D3DPRESENT_PARAMETERS*));
    __asm {
        mov eax, BehaviorFlags
        mov ecx, ppReturnedDeviceInterface
        push pPresentationParameters
        call XB_Direct3D_CreateDevice_4
        mov hRet, eax
    }

	Direct3D_CreateDevice_End();

	return hRet;
}

// LTCG specific Direct3D_CreateDevice function...
// This uses a custom calling convention passed unknown parameters
// Test-case: Battle Engine Aquila
HRESULT WINAPI XTL::EMUPATCH(Direct3D_CreateDevice_16)
(
    UINT                        Adapter,
    D3DDEVTYPE                  DeviceType,
    HWND                        hFocusWindow,
    X_D3DPRESENT_PARAMETERS     *pPresentationParameters
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Adapter)
		LOG_FUNC_ARG(DeviceType)
		LOG_FUNC_ARG(hFocusWindow)
		LOG_FUNC_ARG(pPresentationParameters)
		LOG_FUNC_END;

	Direct3D_CreateDevice_Start(pPresentationParameters);

	// Only then call Xbox CreateDevice function
	XB_trampoline(HRESULT, WINAPI, Direct3D_CreateDevice_16, (UINT, D3DDEVTYPE, HWND, X_D3DPRESENT_PARAMETERS*));
	HRESULT hRet = XB_Direct3D_CreateDevice_16(Adapter, DeviceType, hFocusWindow, pPresentationParameters);

	Direct3D_CreateDevice_End();

	return hRet;
}

// ******************************************************************
// * patch: D3DDevice_SetIndices_4
// LTCG specific D3DDevice_SetIndices function...
// This uses a custom calling convention where parameter is passed in EBX and Stack
// Test Case: Conker
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetIndices_4)
(
    UINT                BaseVertexIndex
)
{
    X_D3DIndexBuffer   *pIndexData;

    __asm {
        mov pIndexData, ebx
    }
    // Cache the base vertex index
    g_Xbox_BaseVertexIndex = BaseVertexIndex;

    // Call LTCG-specific trampoline
    XB_trampoline(VOID, WINAPI, D3DDevice_SetIndices_4, (UINT));
    __asm {
        mov ebx, pIndexData
        push BaseVertexIndex
        call XB_D3DDevice_SetIndices_4;
    }
}


// ******************************************************************
// * patch: D3DDevice_SetIndices
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetIndices)
(
	X_D3DIndexBuffer   *pIndexData,
	UINT                BaseVertexIndex
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pIndexData)
		LOG_FUNC_ARG(BaseVertexIndex)
		LOG_FUNC_END;

	// Cache the base vertex index then call the Xbox function
	g_Xbox_BaseVertexIndex = BaseVertexIndex;

	XB_trampoline(VOID, WINAPI, D3DDevice_SetIndices, (X_D3DIndexBuffer*, UINT));
	XB_D3DDevice_SetIndices(pIndexData, BaseVertexIndex);
}

// ******************************************************************
// * patch: Direct3D_CreateDevice
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(Direct3D_CreateDevice)
(
    UINT                        Adapter,
    D3DDEVTYPE                  DeviceType,
    HWND                        hFocusWindow,
    DWORD                       BehaviorFlags,
    X_D3DPRESENT_PARAMETERS    *pPresentationParameters,
    IDirect3DDevice           **ppReturnedDeviceInterface
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Adapter)
		LOG_FUNC_ARG(DeviceType)
		LOG_FUNC_ARG(hFocusWindow)
		LOG_FUNC_ARG(BehaviorFlags) // Xbox ignores BehaviorFlags
		LOG_FUNC_ARG(pPresentationParameters)
		LOG_FUNC_ARG(ppReturnedDeviceInterface)
		LOG_FUNC_END;

	Direct3D_CreateDevice_Start(pPresentationParameters);

	// Only then call Xbox CreateDevice function
	XB_trampoline(HRESULT, WINAPI, Direct3D_CreateDevice, (UINT, D3DDEVTYPE, HWND, DWORD, X_D3DPRESENT_PARAMETERS*, IDirect3DDevice**));
	HRESULT hRet = XB_Direct3D_CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

	Direct3D_CreateDevice_End();

	return hRet;
}

// ******************************************************************
// * patch: D3DDevice_Reset
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_Reset)
(
	X_D3DPRESENT_PARAMETERS* pPresentationParameters
)
{
	LOG_FUNC_ONE_ARG(pPresentationParameters)

	// Unlike the host version of Reset, The Xbox version does not actually reset the entire device
	// Instead, it simply re-creates the backbuffer with a new configuration
	// Just like CreateDevice, we need to unset the MultiSample flag for now
	// This fixes the 'Render to half-screen' issue in some titles (Test Case: Max Payne, Gameplay)
	// See Direct3D_CreateDevice_Start for more info
	// We should leave the rest of the params as is, and let the Xbox code/surface cache take care of it for us
	pPresentationParameters->MultiSampleType = XTL::X_D3DMULTISAMPLE_NONE;

	// Since Reset will call create a new backbuffer surface, we can clear our current association
	// NOTE: We don't actually free the Xbox data, the Xbox side will do this for us when we call the trampoline below.
	// We must not reset the values to nullptr, since the XDK will re-use the same addresses for the data headers
	// (they are members of the Direct3DDevice object). if we overwrite then, the reference to the xbox backbuffer will be lost
    // and we'll get a black screen.
	FreeHostResource(GetHostResourceKey(g_pXbox_BackBufferSurface));
	FreeHostResource(GetHostResourceKey(g_pXbox_DefaultDepthStencilSurface));
	
	// Call the Xbox Reset function to do the rest of the work for us
	XB_trampoline(HRESULT, WINAPI, D3DDevice_Reset, (X_D3DPRESENT_PARAMETERS*));
	HRESULT hRet = XB_D3DDevice_Reset(pPresentationParameters);

	// Refresh the current render target and depth stencil, to apply changes made within D3DDevice_Reset
	// Some XDKs do this for us, but not all do!
	EMUPATCH(D3DDevice_SetRenderTarget)(g_pXbox_RenderTarget, g_pXbox_DepthStencil);

	return hRet;
}


// ******************************************************************
// * patch: D3DDevice_GetDisplayFieldStatus
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetDisplayFieldStatus)(X_D3DFIELD_STATUS *pFieldStatus)
{
	// NOTE: This can be unpatched only when NV2A does it's own VBlank and HLE _Swap function is unpatched


	LOG_FUNC_ONE_ARG(pFieldStatus);

	// Read AV Flags to determine if Progressive or Interlaced
	// The xbox does this by reading from pDevice->m_Miniport.m_CurrentAvInfo
	// but we don't have an OOVPA for that. Instead, we call the Xbox implementation of 
	// D3DDevice_GetDisplayMode and read the result

	// Get a function pointer to the unpatched xbox function D3DDevice_GetDisplayMode
	XB_trampoline(VOID, WINAPI, D3DDevice_GetDisplayMode, (X_D3DDISPLAYMODE*));
	
	X_D3DDISPLAYMODE displayMode;

	// If we can find the Xbox version of GetDisplayMode, use the real data returned, otherwise
	// use a sensible default
	if (XB_D3DDevice_GetDisplayMode != nullptr) {
		XB_D3DDevice_GetDisplayMode(&displayMode);
	} else {
		// We don't show a warning because doing so pollutes the kernel debug log as this function gets called every frame
		displayMode.Flags = X_D3DPRESENTFLAG_INTERLACED;
	}
	
	// Set the VBlank count
	pFieldStatus->VBlankCount = g_Xbox_VBlankData.VBlank;

	// If we are interlaced, return the current field, otherwise, return progressive scan
	if (displayMode.Flags & X_D3DPRESENTFLAG_INTERLACED) {
		pFieldStatus->Field = (g_Xbox_VBlankData.VBlank % 2 == 0) ? X_D3DFIELD_ODD : X_D3DFIELD_EVEN;
	} else {
		pFieldStatus->Field = X_D3DFIELD_PROGRESSIVE;
	}
}

// ******************************************************************
// * patch: D3DDevice_BeginPush
// TODO: Find a test case and verify this
// Starting from XDK 4531, this changed to 1 parameter only.
// Is this definition incorrect, or did it change at some point?
// ******************************************************************
PDWORD WINAPI XTL::EMUPATCH(D3DDevice_BeginPush)(DWORD Count)
{
	LOG_FUNC_ONE_ARG(Count);

	if (g_pXbox_BeginPush_Buffer != nullptr)
	{
		EmuLog(LOG_LEVEL::WARNING, "D3DDevice_BeginPush called without D3DDevice_EndPush in between?!");
		delete[] g_pXbox_BeginPush_Buffer; // prevent a memory leak
	}

    DWORD *pRet = new DWORD[Count];

    g_pXbox_BeginPush_Buffer = pRet;

    return pRet;
}

// ******************************************************************
// * patch: D3DDevice_BeginPush2
// TODO: Find a test case and verify this: RalliSport Challenge XDK 4134
// For XDK before 4531
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_BeginPush2)(DWORD Count, DWORD** ppPush)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Count)
		LOG_FUNC_ARG(ppPush)
		LOG_FUNC_END;

	if (g_pXbox_BeginPush_Buffer != nullptr)
	{
		EmuLog(LOG_LEVEL::WARNING, "D3DDevice_BeginPush2 called without D3DDevice_EndPush in between?!");
		delete[] g_pXbox_BeginPush_Buffer; // prevent a memory leak
	}

	DWORD *pRet = new DWORD[Count];

	g_pXbox_BeginPush_Buffer = pRet;

	*ppPush=pRet;
}

// ******************************************************************
// * patch: D3DDevice_EndPush
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_EndPush)(DWORD *pPush)
{
	LOG_FUNC_ONE_ARG(pPush);

	if (g_pXbox_BeginPush_Buffer == nullptr)
		EmuLog(LOG_LEVEL::WARNING, "D3DDevice_EndPush called without preceding D3DDevice_BeginPush?!");
	else
	{
        // Note: We don't use the count from BeginPush because that specifies the *maximum* count
        // rather than the count actually in the pushbuffer. 
		EmuExecutePushBufferRaw(g_pXbox_BeginPush_Buffer, (uintptr_t)pPush - (uintptr_t)g_pXbox_BeginPush_Buffer);

		delete[] g_pXbox_BeginPush_Buffer;
		g_pXbox_BeginPush_Buffer = nullptr;
	}
}

// ******************************************************************
// * patch: D3DDevice_BeginVisibilityTest
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_BeginVisibilityTest)()
{
	LOG_FUNC();

	LOG_UNIMPLEMENTED();
}

// LTCG specific D3DDevice_EndVisibilityTest function...
// This uses a custom calling convention where parameter is passed in EAX
// Test-case: Test Drive: Eve of Destruction
HRESULT __stdcall XTL::EMUPATCH(D3DDevice_EndVisibilityTest_0)
(
)
{
	DWORD                       Index;

	__asm {
		mov Index, eax
	}

	return EMUPATCH(D3DDevice_EndVisibilityTest)(Index);
}

// ******************************************************************
// * patch: D3DDevice_EndVisibilityTest
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_EndVisibilityTest)
(
    DWORD                       Index
)
{
	LOG_FUNC_ONE_ARG(Index);

	LOG_UNIMPLEMENTED();

    return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_SetBackBufferScale
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetBackBufferScale)(FLOAT x, FLOAT y)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_END;

	LOG_IGNORED();
}

// ******************************************************************
// * patch: D3DDevice_GetVisibilityTestResult
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetVisibilityTestResult)
(
    DWORD                       Index,
    UINT                       *pResult,
    ULONGLONG                  *pTimeStamp
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Index)
		LOG_FUNC_ARG(pResult)
		LOG_FUNC_ARG(pTimeStamp)
		LOG_FUNC_END;

    // TODO: actually emulate this!?

    if(pResult != 0)
        *pResult = 640*480;

    if(pTimeStamp != 0)
        *pTimeStamp = 0;

    return D3D_OK;
}

// LTCG specific D3DDevice_LoadVertexShader function...
// This uses a custom calling convention where parameter is passed in EAX, ECX
// Test-case: Aggressive Inline
VOID __stdcall XTL::EMUPATCH(D3DDevice_LoadVertexShader_0)
(
)
{
    DWORD                       Handle;
    DWORD                       Address;

	__asm {
		mov Address, eax
		mov Handle, ecx
	}

	return EMUPATCH(D3DDevice_LoadVertexShader)(Handle, Address);
}

// This uses a custom calling convention where parameter is passed in EAX
// Test-case: Ninja Gaiden
VOID WINAPI XTL::EMUPATCH(D3DDevice_LoadVertexShader_4)
(
    DWORD                       Address
)
{
	DWORD           Handle;
	__asm mov Handle, eax;

	LOG_FORWARD("D3DDevice_LoadVertexShader");
	return EMUPATCH(D3DDevice_LoadVertexShader)(Handle, Address);
}

// ******************************************************************
// * patch: D3DDevice_LoadVertexShader
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_LoadVertexShader)
(
    DWORD                       Handle,
    DWORD                       Address
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Handle)
		LOG_FUNC_ARG(Address)
	LOG_FUNC_END;

	// Handle is always address of an X_D3DVertexShader struct, thus always or-ed with 1 (X_D3DFVF_RESERVED0)
	// Address is the slot (offset) from which the program must be written onwards (as whole DWORDS)
	// D3DDevice_LoadVertexShader pushes the program contained in the Xbox VertexShader struct to the NV2A
    if(Address < 136) {
        CxbxVertexShader * pCxbxVertexShader = GetCxbxVertexShader(Handle);
		if (pCxbxVertexShader) {
			for (DWORD i = Address; i < pCxbxVertexShader->XboxNrAddressSlots; i++) {
				// TODO: This seems very fishy
				g_VertexShaderSlots[i] = Handle;
			}
		}
    }
}

// LTCG specific D3DDevice_SelectVertexShader function...
// This uses a custom calling convention where parameter is passed in EAX, EBX
// Test-case: Star Wars - Battlefront
VOID __stdcall XTL::EMUPATCH(D3DDevice_SelectVertexShader_0)
(
)
{
    DWORD                       Handle;
    DWORD                       Address;

	__asm {
		mov Handle, eax
		mov Address, ebx
	}

	return EMUPATCH(D3DDevice_SelectVertexShader)(Handle, Address);
}

// LTCG specific D3DDevice_SelectVertexShader function...
// This uses a custom calling convention where parameter is passed in EAX
// Test-case: Aggressive Inline
VOID __stdcall XTL::EMUPATCH(D3DDevice_SelectVertexShader_4)
(
    DWORD                       Address
)
{
	DWORD           Handle;
	__asm mov Handle, eax;

	return EMUPATCH(D3DDevice_SelectVertexShader)(Handle, Address);
}

// ******************************************************************
// * patch: D3DDevice_SelectVertexShader
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SelectVertexShader)
(
    DWORD                       Handle,
    DWORD                       Address
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Handle)
		LOG_FUNC_ARG(Address)
		LOG_FUNC_END;

	HRESULT hRet = D3D_OK;

	// Address always indicates a previously loaded vertex shader slot (from where the program is used).
	// Handle can be null if the current Xbox VertexShader is assigned
	// Handle can be an address of an Xbox VertexShader struct, or-ed with 1 (X_D3DFVF_RESERVED0)
	// If Handle is assigned, it becomes the new current Xbox VertexShader,
	// which resets a bit of state (nv2a execution mode, viewport, ?)
	// Either way, the given address slot is selected as the start of the current vertex shader program
	g_Xbox_VertexShader_Handle = Handle;

	CxbxVertexShader *pCxbxVertexShader = nullptr;
	DWORD HostFVF = 0;

    if(VshHandleIsVertexShader(Handle))
    {
        pCxbxVertexShader = GetCxbxVertexShader(Handle);
    }
    else if(Handle == xbnull)
    {
		HostFVF = D3DFVF_XYZ | D3DFVF_TEX0;
	}
    else if(Address < 136)
    {
        X_D3DVertexShader *pXboxVertexShader = (X_D3DVertexShader*)g_VertexShaderSlots[Address];

        if(pXboxVertexShader != nullptr)
        {
			LOG_TEST_CASE("Assigned g_VertexShaderSlots");
			// Was : pCxbxVertexShader = (CxbxVertexShader *)(pXboxVertexShader->CxbxVertexShaderHandle);
			// However, the CxbxVertexShaderHandle union is never set, so could lead to using undefined data!
		}
        else
        {
            EmuLog(LOG_LEVEL::WARNING, "g_VertexShaderSlots[%d] = 0", Address);
		}
    }

	IDirect3DVertexDeclaration* pHostVertexDeclaration = nullptr;
	IDirect3DVertexShader* pHostVertexShader = nullptr;

	if (pCxbxVertexShader)
	{
		pHostVertexDeclaration = pCxbxVertexShader->pHostVertexDeclaration;
		pHostVertexShader = pCxbxVertexShader->pHostVertexShader;
		HostFVF = pCxbxVertexShader->HostFVF;

		SetOverrideFlags(pCxbxVertexShader);
	}

	hRet = g_pD3DDevice->SetVertexDeclaration(pHostVertexDeclaration);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexDeclaration()");
	hRet = g_pD3DDevice->SetVertexShader(pHostVertexShader);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexShader()");
	if (HostFVF) // should equal (pHostVertexShader == nullptr)
	{
		hRet = g_pD3DDevice->SetFVF(HostFVF);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_TEX0)");
	}

	if (FAILED(hRet))
	{
		EmuLog(LOG_LEVEL::WARNING, "We're lying about setting a vertext shader!");

		hRet = D3D_OK;
	}
}

// ******************************************************************
// * patch: D3DDevice_SetGammaRamp
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetGammaRamp)
(
    DWORD                   dwFlags,
    CONST X_D3DGAMMARAMP   *pRamp
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(dwFlags)
		LOG_FUNC_ARG(pRamp)
		LOG_FUNC_END;

    // remove D3DSGR_IMMEDIATE
    DWORD dwPCFlags = dwFlags & (~0x00000002);
    D3DGAMMARAMP PCRamp;

    for(int v=0;v<255;v++)
    {
        PCRamp.red[v]   = pRamp->red[v];
        PCRamp.green[v] = pRamp->green[v];
        PCRamp.blue[v]  = pRamp->blue[v];
    }

#if 0 // TODO : Why is this disabled?
	g_pD3DDevice->SetGammaRamp(
		0, // iSwapChain
		dwPCFlags, &PCRamp);
#endif
}

// ******************************************************************
// * patch: D3DDevice_GetGammaRamp
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetGammaRamp)
(
    X_D3DGAMMARAMP     *pRamp
)
{
	LOG_FUNC_ONE_ARG(pRamp);

    D3DGAMMARAMP *pGammaRamp = (D3DGAMMARAMP *)malloc(sizeof(D3DGAMMARAMP));

    g_pD3DDevice->GetGammaRamp(
		0, // iSwapChain
		pGammaRamp);

    for(int v=0;v<256;v++)
    {
        pRamp->red[v] = (BYTE)pGammaRamp->red[v];
        pRamp->green[v] = (BYTE)pGammaRamp->green[v];
        pRamp->blue[v] = (BYTE)pGammaRamp->blue[v];
    }

	free(pGammaRamp);
}

// ******************************************************************
// * patch: D3DDevice_GetBackBuffer2
// ******************************************************************
#define COPY_BACKBUFFER_TO_XBOX_SURFACE // Uncomment to enable writing Host Backbuffers back to Xbox surfaces
XTL::X_D3DSurface* WINAPI XTL::EMUPATCH(D3DDevice_GetBackBuffer2)
(
    INT                 BackBuffer
)
{
	LOG_FUNC_ONE_ARG(BackBuffer);

	X_D3DSurface* pXboxBackBuffer = nullptr;


#ifndef COPY_BACKBUFFER_TO_XBOX_SURFACE
	/** unsafe, somehow
	HRESULT hRet = D3D_OK;

	X_D3DSurface *pXboxBackBuffer = EmuNewD3DSurface();

	if(BackBuffer == -1) {
		static IDirect3DSurface *pCachedPrimarySurface = nullptr;

		if(pCachedPrimarySurface == nullptr) {
			// create a buffer to return
			// TODO: Verify the surface is always 640x480
			hRet = g_pD3DDevice->CreateOffscreenPlainSurface(640, 480, D3DFMT_A8R8G8B8, /*D3DPool=* /0, &pCachedPrimarySurface, nullptr);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateOffscreenPlainSurface");
		}

		SetHostSurface(pXboxBackBuffer, pCachedPrimarySurface); // No iTextureStage!

		hRet = g_pD3DDevice->GetFrontBuffer(pCachedPrimarySurface);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetFrontBuffer");

		if (FAILED(hRet)) {
			EmuLog(LOG_LEVEL::WARNING, "Could not retrieve primary surface, using backbuffer");
			SetHostSurface(pXboxBackBuffer, nullptr); // No iTextureStage!
			pCachedPrimarySurface->Release();
			pCachedPrimarySurface = nullptr;
			BackBuffer = 0;
		}

		// Debug: Save this image temporarily
		//D3DXSaveSurfaceToFile("C:\\Aaron\\Textures\\FrontBuffer.bmp", D3DXIFF_BMP, GetHostSurface(pXboxBackBuffer), nullptr, nullptr);
	}

	if(BackBuffer != -1) {
		hRet = g_pD3DDevice->GetBackBuffer(
			0, // iSwapChain
			BackBuffer, D3DBACKBUFFER_TYPE_MONO, &pCachedPrimarySurface);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetBackBuffer");
	}
	//*/

	static X_D3DSurface *pXboxBackBuffer = EmuNewD3DSurface();
	IDirect3DSurface *pCurrentHostBackBuffer = nullptr;

	 STATUS_SUCCESS;

	 if (BackBuffer == -1) {
		 BackBuffer = 0;
	 }

	HRESULT hRet = g_pD3DDevice->GetBackBuffer(
		0, // iSwapChain
		BackBuffer, D3DBACKBUFFER_TYPE_MONO, &pCurrentHostBackBuffer);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetBackBuffer");

	if (FAILED(hRet))
		CxbxKrnlCleanup("Unable to retrieve back buffer");

	SetHostSurface(pXboxBackBuffer, pCurrentHostBackBuffer); // No iTextureStage!

	// Increment reference count
	pXboxBackBuffer->Common++; // EMUPATCH(D3DResource_AddRef)(pXboxBackBuffer);

	return pXboxBackBuffer;
#else // COPY_BACKBUFFER_TO_XBOX_SURFACE
	// Rather than create a new surface, we should forward to the Xbox version of GetBackBuffer,
	// This gives us the correct Xbox surface to update.
	// We get signatures for both backbuffer functions as it changed in later XDKs
	XB_trampoline(X_D3DSurface*, WINAPI, D3DDevice_GetBackBuffer2, (INT));

	XB_trampoline(VOID, WINAPI, D3DDevice_GetBackBuffer, (INT, D3DBACKBUFFER_TYPE, X_D3DSurface**));

	// This also updates the reference count, so we don't need to do this ourselves
	if (XB_D3DDevice_GetBackBuffer != nullptr) {
		XB_D3DDevice_GetBackBuffer(BackBuffer, D3DBACKBUFFER_TYPE_MONO, &pXboxBackBuffer);
	}
	else {
		pXboxBackBuffer = XB_D3DDevice_GetBackBuffer2(BackBuffer);
	}

	// Now pXboxBackbuffer points to the requested Xbox backbuffer
	if (pXboxBackBuffer == nullptr) {
		CxbxKrnlCleanup("D3DDevice_GetBackBuffer2: Could not get Xbox backbuffer");
	}


    // HACK: Disabled: Enabling this breaks DOA3 at native res/without hacks+
    // Also likely to effect Other games, but it has no known benefit at this point in time
    // There are currently no known games that depend on backbuffer readback on the CPU!
#if 0
	// TODO: Downscale the host surface to the same size as the Xbox surface during copy
	// Otherwise, we will overflow memory and crash
	// HACK: For now, when using a non-zero scale factor, we can just skip the copy to prevent a crash
	if (g_RenderScaleFactor == 1) {
		auto pCopySrcSurface = GetHostSurface(pXboxBackBuffer, D3DUSAGE_RENDERTARGET);
		if (pCopySrcSurface == nullptr) {
			EmuLog(LOG_LEVEL::WARNING, "Failed to get Host Resource for Xbox Back Buffer");
			return pXboxBackBuffer;
		}

		D3DLOCKED_RECT copyLockedRect;
		HRESULT hRet = pCopySrcSurface->LockRect(&copyLockedRect, NULL, D3DLOCK_READONLY);
		if (hRet != D3D_OK) {
			EmuLog(LOG_LEVEL::WARNING, "Could not lock Host Resource for Xbox Back Buffer");
			return pXboxBackBuffer;
		}

		D3DSURFACE_DESC copySurfaceDesc;
		hRet = pCopySrcSurface->GetDesc(&copySurfaceDesc);
		if (hRet != D3D_OK) {
			EmuLog(LOG_LEVEL::WARNING, "Could not get Xbox Back Buffer Host Surface Desc");
		}
		else {
			DWORD Size = copyLockedRect.Pitch * copySurfaceDesc.Height; // TODO : What about mipmap levels? (Backbuffer does not support mipmap)
			// Finally, do the copy from the converted host resource to the xbox resource
			memcpy((void*)GetDataFromXboxResource(pXboxBackBuffer), copyLockedRect.pBits, Size);
		}

		pCopySrcSurface->UnlockRect();
	}
#endif

	return pXboxBackBuffer;
#endif // COPY_BACKBUFFER_TO_XBOX_SURFACE
}

// ******************************************************************
// * patch: D3DDevice_GetBackBuffer
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetBackBuffer)
(
    INT                 BackBuffer,
    D3DBACKBUFFER_TYPE  Type,
    X_D3DSurface      **ppBackBuffer
)
{
	LOG_FORWARD("D3DDevice_GetBackBuffer2");

    *ppBackBuffer = EMUPATCH(D3DDevice_GetBackBuffer2)(BackBuffer);
}

bool GetHostRenderTargetDimensions(DWORD *pHostWidth, DWORD *pHostHeight, IDirect3DSurface* pHostRenderTarget = nullptr)
{
	bool shouldRelease = false;
	if (pHostRenderTarget == nullptr) {
		g_pD3DDevice->GetRenderTarget(
			0, // RenderTargetIndex
			&pHostRenderTarget);

		shouldRelease = true;
	}

	// The following can only work if we could retrieve a host render target
	if (!pHostRenderTarget) {
		return false;
	}

	// Get current host render target dimensions
	D3DSURFACE_DESC HostRenderTarget_Desc;
	pHostRenderTarget->GetDesc(&HostRenderTarget_Desc);

	if (shouldRelease) {
		pHostRenderTarget->Release();
	}

	*pHostWidth = HostRenderTarget_Desc.Width;
	*pHostHeight = HostRenderTarget_Desc.Height;

	return true;
}

DWORD ScaleDWORD(DWORD Value, DWORD FromMax, DWORD ToMax)
{
	uint64_t tmp = Value;
	tmp *= ToMax;
	tmp /= FromMax;
	return (DWORD)tmp;
}

void ValidateRenderTargetDimensions(DWORD HostRenderTarget_Width, DWORD HostRenderTarget_Height, DWORD XboxRenderTarget_Width, DWORD XboxRenderTarget_Height)
{
    // This operation is often used to change the display resolution without calling SetRenderTarget!
    // This works by updating the underlying Width & Height of the Xbox surface, without reallocating the data
    // Because of this, we need to validate that the associated host resource still matches the dimensions of the Xbox Render Target
    // If not, we must force them to be re-created
    // TEST CASE: Chihiro Factory Test Program
    DWORD XboxRenderTarget_Width_Scaled = XboxRenderTarget_Width * g_RenderScaleFactor;
    DWORD XboxRenderTarget_Height_Scaled = XboxRenderTarget_Height * g_RenderScaleFactor;
    if (HostRenderTarget_Width != XboxRenderTarget_Width_Scaled || HostRenderTarget_Height != XboxRenderTarget_Height_Scaled) {
        LOG_TEST_CASE("Existing RenderTarget width/height changed");

        if (g_pXbox_RenderTarget == g_pXbox_BackBufferSurface) {
            FreeHostResource(GetHostResourceKey(g_pXbox_RenderTarget)); g_pD3DDevice->SetRenderTarget(0, GetHostSurface(g_pXbox_RenderTarget, D3DUSAGE_RENDERTARGET));
            FreeHostResource(GetHostResourceKey(g_pXbox_DepthStencil)); g_pD3DDevice->SetDepthStencilSurface(GetHostSurface(g_pXbox_DepthStencil, D3DUSAGE_DEPTHSTENCIL));
        }
    }
}

float GetZScaleForSurface(XTL::X_D3DSurface* pSurface)
{
    // If no surface was present, fallback to 1
    if (pSurface == xbnullptr) {
        return 1;
    }

    auto format = GetXboxPixelContainerFormat(pSurface);
    switch (format) {
        case XTL::X_D3DFMT_D16:
        case XTL::X_D3DFMT_LIN_D16:
            return 65535.0f;

        case XTL::X_D3DFMT_D24S8:
        case XTL::X_D3DFMT_LIN_D24S8:
            return 16777215.0f;

        case XTL::X_D3DFMT_F16:
        case XTL::X_D3DFMT_LIN_F16:
            return 511.9375f;

        case XTL::X_D3DFMT_F24S8:
        case XTL::X_D3DFMT_LIN_F24S8:
            // 24bit floating point is close to precision maximum, so a lower value is used
            // We can't use a double here since the vertex shader is only at float precision
            return 1.0e30f; 
    }

    // Default to 1 if unknown depth format
    LOG_TEST_CASE("GetZScaleForSurface: Unknown Xbox Depth Format");
    return 1;
}

void GetViewPortOffsetAndScale(float (&vOffset)[4], float(&vScale)[4])
{
    // Store viewport offset and scale in constant registers
    // used in shaders to transform back from screen space (Xbox Shader Output) to Clip space (Host Shader Output)
    D3DVIEWPORT ViewPort;
    g_pD3DDevice->GetViewport(&ViewPort);

    // Calculate Width/Height scale & offset
    float scaleWidth = (2.0f / ViewPort.Width) * g_RenderScaleFactor;
    float scaleHeight = (2.0f / ViewPort.Height) * g_RenderScaleFactor;
    float offsetWidth = scaleWidth;
    float offsetHeight = scaleHeight;

    // Calculate Z scale & offset
    float zScale = GetZScaleForSurface(g_pXbox_DepthStencil);
    float scaleZ = zScale * (ViewPort.MaxZ - ViewPort.MinZ);
    float offsetZ = zScale * ViewPort.MinZ;

	// TODO will we need to do something here to support upscaling?
	// TODO remove the code above as required

	// Default scale and offset.
	// Multisample state will affect these
	float xScale = 1.0f;
	float yScale = 1.0f;
	float xOffset = 0.5f;
	float yOffset = 0.5f;

	// MULTISAMPLE options have offset of 0
	// Various sample sizes have various x and y scales
	switch (g_EmuCDPD.XboxPresentationParameters.MultiSampleType)
	{
		case XTL::X_D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR:
		case XTL::X_D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX:
		case XTL::X_D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR:
		case XTL::X_D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN:
			xOffset = yOffset = 0.0f;
			break;
		case XTL::X_D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR:
			xScale = 2.0f;
			break;
		case XTL::X_D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR:
			yScale = 2.0f;
			break;
		case XTL::X_D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR:
		case XTL::X_D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN:
			xScale = yScale = 2.0f;
			break;
		case XTL::X_D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN:
			xScale = yScale = 1.5f;
			xOffset = yOffset = 0.0f;
			break;
		case XTL::X_D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN:
			xScale = yScale = 3.0f;
			break;
	}

	// Xbox correct values?
	xOffset = xOffset + (1.0f / 32.0f);
	yOffset = yOffset + (1.0f / 32.0f);
	xScale = xScale * ViewPort.Width;
	yScale = yScale * ViewPort.Height;

	// HACK: Add a host correction factor to these values
	// So that after we reverse the screenspace transformation
	// Pre-transformed 2d geometry is in the same space as the 3d geometry...?

	// Offset with a host correction
	vOffset[0] = xOffset + (0.5f * (float)ViewPort.Width / (float)g_RenderScaleFactor);
	vOffset[1] = yOffset + (0.5f * (float)ViewPort.Height / (float)g_RenderScaleFactor);
	vOffset[2] = 0.0f; //offsetZ;
	vOffset[3] = 0.0f;

	// Scale with a host correction
	vScale[0] = xScale * (1.0f / ( 2.0f * (float)g_RenderScaleFactor));
	vScale[1] = yScale * (1.0f / (-2.0f * (float)g_RenderScaleFactor));
	vScale[2] = scaleZ; // ?
	vScale[3] = 1.0f; // ?
}

void UpdateViewPortOffsetAndScaleConstants()
{
    float vOffset[4], vScale[4];
    GetViewPortOffsetAndScale(vOffset, vScale);

	g_pD3DDevice->SetVertexShaderConstantF(X_D3DVS_VIEWPORT_SCALE_MIRROR, vScale, 1);
    g_pD3DDevice->SetVertexShaderConstantF(X_D3DVS_VIEWPORT_OFFSET_MIRROR, vOffset, 1);

	// Store viewport offset and scale in constant registers 58 (c-38) and
	// 59 (c-37) used for screen space transformation.
	// We only do this if X_D3DSCM_NORESERVEDCONSTANTS is not set, since enabling this flag frees up these registers for shader used
	if (g_Xbox_VertexShaderConstantMode != X_D3DSCM_NORESERVEDCONSTANTS)
	{
		g_pD3DDevice->SetVertexShaderConstantF(X_D3DSCM_RESERVED_CONSTANT_SCALE + X_D3DSCM_CORRECTION, vScale, 1);
		g_pD3DDevice->SetVertexShaderConstantF(X_D3DSCM_RESERVED_CONSTANT_OFFSET + X_D3DSCM_CORRECTION, vOffset, 1);
	}
}

// ******************************************************************
// * patch: D3DDevice_SetViewport
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetViewport)
(
    CONST X_D3DVIEWPORT8 *pViewport
)
{
	LOG_FUNC_ONE_ARG(pViewport);

	// Always call the Xbox SetViewPort to update D3D Internal State
	XB_trampoline(VOID, WINAPI, D3DDevice_SetViewport, (CONST X_D3DVIEWPORT8 *));
	XB_D3DDevice_SetViewport(pViewport);

	// Host does not support pViewPort = nullptr
	if (pViewport == nullptr) {
		LOG_TEST_CASE("pViewport = null");
		return;
	}

	D3DVIEWPORT XboxViewPort = *pViewport;
	D3DVIEWPORT HostViewPort = *pViewport;

	if (g_pXbox_RenderTarget) {
		// Clip the Xbox Viewport to the render target dimensions
		// This is required because during SetRenderTarget, Xbox calls SetViewPort with impossibly large values
		DWORD XboxRenderTarget_Width = GetPixelContainerWidth(g_pXbox_RenderTarget);
		DWORD XboxRenderTarget_Height = GetPixelContainerHeight(g_pXbox_RenderTarget);

		DWORD left = std::max((int)pViewport->X, 0);
		DWORD top = std::max((int)pViewport->Y, 0);
		DWORD right = std::min((int)pViewport->X + (int)pViewport->Width, (int)XboxRenderTarget_Width);
		DWORD bottom = std::min((int)pViewport->Y + (int)pViewport->Height, (int)XboxRenderTarget_Height);
		DWORD width = right - left;
		DWORD height = bottom - top;

		XboxViewPort.X = left;
		XboxViewPort.Y = top;
		XboxViewPort.Width = width;
		XboxViewPort.Height = height;
		XboxViewPort.MinZ = pViewport->MinZ;
		XboxViewPort.MaxZ = pViewport->MaxZ;


		// Store the updated viewport data ready to pass to host SetViewPort
		HostViewPort = XboxViewPort;

		DWORD HostRenderTarget_Width = 0, HostRenderTarget_Height = 0;
		if (GetHostRenderTargetDimensions(&HostRenderTarget_Width, &HostRenderTarget_Height)) {
            ValidateRenderTargetDimensions(HostRenderTarget_Width, HostRenderTarget_Height, XboxRenderTarget_Width, XboxRenderTarget_Height);

			// We *must* always scale the viewport to the associated host surface
			// Otherwise, we only get partial screen updates, or broken upscaling
			// Scale Xbox to host dimensions (avoiding hard-coding 640 x 480)
			HostViewPort.X = ScaleDWORD(XboxViewPort.X, XboxRenderTarget_Width, HostRenderTarget_Width);
			HostViewPort.Y = ScaleDWORD(XboxViewPort.Y, XboxRenderTarget_Height, HostRenderTarget_Height);
			HostViewPort.Width = ScaleDWORD(XboxViewPort.Width, XboxRenderTarget_Width, HostRenderTarget_Width);
			HostViewPort.Height = ScaleDWORD(XboxViewPort.Height, XboxRenderTarget_Height, HostRenderTarget_Height);
			// TODO : Fix test-case Shenmue 2 (which halves height, leaving the bottom half unused)
			HostViewPort.MinZ = XboxViewPort.MinZ; // No need scale Z for now
			HostViewPort.MaxZ = XboxViewPort.MaxZ;
		} else {
			LOG_TEST_CASE("SetViewPort: Unable to fetch host render target dimensions");
		}
	}

	HRESULT hRet = g_pD3DDevice->SetViewport(&HostViewPort);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetViewport");

	UpdateViewPortOffsetAndScaleConstants();
}

// LTCG specific D3DDevice_GetViewportOffsetAndScale function...
// This function is still not working so should be investigated...
// This uses a custom calling convention where parameter is passed in EDX, ECX
// Test-case: RalliSport Challenge 2
VOID __stdcall XTL::EMUPATCH(D3DDevice_GetViewportOffsetAndScale_0)
(
)
{
    D3DXVECTOR4 *pOffset;
    D3DXVECTOR4 *pScale;

	__asm {
		mov pScale, ecx
		mov pOffset, edx
	}

	return EMUPATCH(D3DDevice_GetViewportOffsetAndScale)(pOffset, pScale);
}

// ******************************************************************
// * patch: D3DDevice_GetViewportOffsetAndScale
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetViewportOffsetAndScale)
(
    X_D3DXVECTOR4 *pOffset,
	X_D3DXVECTOR4 *pScale
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pOffset)
		LOG_FUNC_ARG(pScale)
		LOG_FUNC_END;

	// LOG_TEST_CASE("D3DDevice_GetViewportOffsetAndScale"); // Get us some test-cases
	// Test case : 007: From Russia with Love
	// Test case : Army Men?: Sarge's War
	// Test case : BeatDown - Fists of Vengeance
	// Test case : Big Mutha Truckers
	// Test case : Big Mutha Truckers 2
	// Test case : Classified : The Sentinel Crisis
	// Test case : Cold Fear
	// Test case : Commandos SF
	// Test case : Darkwatch
	// Test case : Dr.Muto
	// Test case : GTA : Vice City
	// Test case : Jacked
	// Test case : Madagascar
	// Test case : Max Payne 2
	// Test case : NBA LIVE 2005
	// Test case : NFS : Underground
	// Test case : NFS : Underground 2
	// Test case : PoP : The Sands of Time
	// Test case : Prince of Persia : T2T - see #1109 (comment)
	// Test case : RPM Tuning
	// Test case : Street Racing Syndicate
	// Test case : Spongebob - Battle for Bikini Bottom
	// Test case : The Incredibles : Rise of the Underminer
	// Test case : The SpongeBob SquarePants Movie
	// Test case : TMNT(R)2
	// Test case : TMNT(R)3

    float vOffset[4], vScale[4];
    GetViewPortOffsetAndScale(vOffset, vScale);

    pOffset->x = vOffset[0];
    pOffset->y = vOffset[1];
    pOffset->z = vOffset[2];
    pOffset->w = vOffset[3];

    pScale->x = vScale[0];
    pScale->y = vScale[1];
    pScale->z = vScale[2];
    pScale->w = vScale[3];
}

// LTCG specific D3DDevice_SetShaderConstantMode function...
// This uses a custom calling convention where parameter is passed in EAX
VOID __stdcall XTL::EMUPATCH(D3DDevice_SetShaderConstantMode_0)
(
)
{
	XTL::X_VERTEXSHADERCONSTANTMODE param;
	__asm {
		mov param, eax;
	}

	return EMUPATCH(D3DDevice_SetShaderConstantMode)(param);
}

// ******************************************************************
// * patch: D3DDevice_SetShaderConstantMode
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetShaderConstantMode)
(
    XTL::X_VERTEXSHADERCONSTANTMODE Mode
)
{
	LOG_FUNC_ONE_ARG(Mode);

    g_Xbox_VertexShaderConstantMode = Mode;
}

// ******************************************************************
// * patch: D3DDevice_CreateVertexShader
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_CreateVertexShader)
(
    CONST DWORD    *pDeclaration,
    CONST DWORD    *pFunction,
    DWORD          *pHandle,
    DWORD           Usage
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pDeclaration)
		LOG_FUNC_ARG(pFunction)
		LOG_FUNC_ARG(pHandle)
		LOG_FUNC_ARG_TYPE(X_D3DUSAGE, Usage)
		LOG_FUNC_END;

	// First, we must call the Xbox CreateVertexShader function and check for success
	// This does the following:
	// Allocates an Xbox VertexShader struct
	// Sets reference count to 1
	// Puts Usage in VertexShader->Flags
	// If pFunction is not null, it points to DWORDS shader type, length and a binary compiled xbox vertex shader
	// If pDeclaration is not null, it's parsed, resulting in a number of constants
	// Parse results are pushed to the push buffer
	// Sets other fields
	// pHandle recieves the addres of the new shader, or-ed with 1 (D3DFVF_RESERVED0)
	XB_trampoline(HRESULT, WINAPI, D3DDevice_CreateVertexShader, (CONST DWORD*, CONST DWORD*, DWORD*, DWORD));

    HRESULT hRet = D3D_OK;

    if (XB_D3DDevice_CreateVertexShader) {
        HRESULT hRet = XB_D3DDevice_CreateVertexShader(pDeclaration, pFunction, pHandle, Usage);
        if (FAILED(hRet)) {
            LOG_TEST_CASE("XB_D3DDevice_CreateVertexShader failed");
            RETURN(hRet);
        }
    } else {
        // Due to how our LoadVertexShader patch is implemented, it may call this function without the Xbox version existing
        // As a result, we have to build our own vertex shader handle if the trampoline was not found
        // We don't do the full steps listed above intentionally so: If this situation is reached, the game
        // does not have a CreateVertexShader function, so those actions should not happen anyway!
        LOG_TEST_CASE("CreateVertexShader with no trampoline");
        *pHandle = ((DWORD)malloc(sizeof(X_D3DVertexShader)) & D3DFVF_RESERVED0);
    }

	if (g_pD3DDevice == nullptr) {
		LOG_TEST_CASE("D3DDevice_CreateVertexShader called before Direct3D_CreateDevice");
		// We lie to allow the game to continue for now, but it probably won't work well
		return STATUS_SUCCESS;
	}

    // HACK: TODO: support this situation
    if(pDeclaration == nullptr) {
		LOG_TEST_CASE("Vertex shader without declaration");
        *pHandle = xbnull;
        return D3D_OK;
    }

	// Now, we can create the host vertex shader
	DWORD             XboxDeclarationCount = 0;
	DWORD             HostDeclarationSize = 0;
	CxbxVertexShader* pCxbxVertexShader = (CxbxVertexShader*)calloc(1, sizeof(CxbxVertexShader));
	D3DVERTEXELEMENT *pRecompiledDeclaration = nullptr;

	pRecompiledDeclaration = EmuRecompileVshDeclaration((DWORD*)pDeclaration,
                                                   /*bIsFixedFunction=*/pFunction == xbnullptr,
                                                   &XboxDeclarationCount,
                                                   &HostDeclarationSize,
                                                   &pCxbxVertexShader->VertexShaderInfo);

	// Create the vertex declaration
	hRet = g_pD3DDevice->CreateVertexDeclaration(pRecompiledDeclaration, &pCxbxVertexShader->pHostVertexDeclaration);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateVertexDeclaration");

	if (FAILED(hRet)) {
		// NOTE: This is a fatal error because it ALWAYS triggers a crash within DrawVertices if not set
		CxbxKrnlCleanup("Failed to create Vertex Declaration");
	}
	g_pD3DDevice->SetVertexDeclaration(pCxbxVertexShader->pHostVertexDeclaration);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexDeclaration");

	ID3DBlob     *pRecompiledBuffer = nullptr;
	DWORD         XboxFunctionSize = 0;
	DWORD        *pRecompiledFunction = nullptr;
	if (SUCCEEDED(hRet) && pFunction)
	{
		bool bUseDeclarationOnly = false;

		hRet = EmuRecompileVshFunction((DWORD*)pFunction,
			/*bNoReservedConstants=*/g_Xbox_VertexShaderConstantMode == X_D3DSCM_NORESERVEDCONSTANTS,
			pRecompiledDeclaration,
			&bUseDeclarationOnly,
			&XboxFunctionSize,
			&pRecompiledBuffer);
		if (SUCCEEDED(hRet))
		{
			if (!bUseDeclarationOnly)
				pRecompiledFunction = (DWORD*)pRecompiledBuffer->GetBufferPointer();
			else
				pRecompiledFunction = nullptr;
		}
		else
		{
			pRecompiledFunction = nullptr;
			EmuLog(LOG_LEVEL::WARNING, "Couldn't recompile vertex shader function.");
		}
	}

    //EmuLog(LOG_LEVEL::DEBUG, "MaxVertexShaderConst = %d", g_D3DCaps.MaxVertexShaderConst);

	IDirect3DVertexShader* pHostVertexShader = nullptr;
	if (SUCCEEDED(hRet) && pRecompiledFunction != nullptr)
	{
		hRet = g_pD3DDevice->CreateVertexShader
		(
			pRecompiledFunction,
			&pHostVertexShader
		);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateVertexShader");
	}

	//* Fallback to dummy shader.
	//if (FAILED(hRet))
	//{
	//	static const char dummy[] =
	//		"vs.1.1\n"
	//		"dcl_position v0\n"
	//		"dp4 oPos.x, v0, c96\n"
	//		"dp4 oPos.y, v0, c97\n"
	//		"dp4 oPos.z, v0, c98\n"
	//		"dp4 oPos.w, v0, c99\n";

	//	EmuLog(LOG_LEVEL::WARNING, "Trying fallback:\n%s", dummy);

	//	hRet = D3DXAssembleShader(
	//		dummy,
	//		strlen(dummy),
	//		/*pDefines=*/nullptr,
	//		/*pInclude=*/nullptr,
	//		/*Flags=*/0, // Was D3DXASM_SKIPVALIDATION
	//		/*ppCompiledShader=*/&pRecompiledBuffer,
	//		/*ppCompilationErrors*/nullptr);

	//	DEBUG_D3DRESULT(hRet, "D3DXAssembleShader");

	//	hRet = g_pD3DDevice->CreateVertexShader
	//	(
	//		(DWORD*)pRecompiledBuffer->GetBufferPointer(),
	//		&pHostVertexShader
	//	);
	//	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateVertexShader(fallback)");
	//}

	if (pRecompiledBuffer != nullptr)
	{
		pRecompiledBuffer->Release();
		pRecompiledBuffer = nullptr;
	}

    free(pRecompiledDeclaration);

	pCxbxVertexShader->pXboxDeclarationCopy = (DWORD*)malloc(XboxDeclarationCount * sizeof(DWORD));
	memcpy(pCxbxVertexShader->pXboxDeclarationCopy, pDeclaration, XboxDeclarationCount * sizeof(DWORD));
	pCxbxVertexShader->XboxFunctionSize = 0;
	pCxbxVertexShader->pXboxFunctionCopy = nullptr;
	pCxbxVertexShader->XboxVertexShaderType = X_VST_NORMAL; // TODO : This can vary
	pCxbxVertexShader->XboxNrAddressSlots = (XboxFunctionSize - sizeof(X_VSH_SHADER_HEADER)) / X_VSH_INSTRUCTION_SIZE_BYTES;
	pCxbxVertexShader->HostFVF = 0;
	pCxbxVertexShader->pHostVertexShader = nullptr;
	pCxbxVertexShader->XboxDeclarationCount = XboxDeclarationCount;
	// Save the status, to remove things later
	// pCxbxVertexShader->XboxStatus = hRet; // Not even used by VshHandleIsValidShader()

    if(SUCCEEDED(hRet))
    {
        if(pFunction != xbnullptr)
        {
            pCxbxVertexShader->XboxFunctionSize = XboxFunctionSize;
            pCxbxVertexShader->pXboxFunctionCopy = (DWORD*)malloc(XboxFunctionSize);
            memcpy(pCxbxVertexShader->pXboxFunctionCopy, pFunction, XboxFunctionSize);
        }

		pCxbxVertexShader->pHostVertexShader = pHostVertexShader;
    }
    else
    {
		LOG_TEST_CASE("Falling back to FVF shader");
		pCxbxVertexShader->HostFVF = D3DFVF_XYZ | D3DFVF_TEX0;
    }

	// Register the host Vertex Shader
	SetCxbxVertexShader(*pHandle, pCxbxVertexShader);

    if(FAILED(hRet))
    {
#ifdef _DEBUG_TRACK_VS
        if (pFunction)
        {
            char pFileName[30];
            static int FailedShaderCount = 0;
            X_VSH_SHADER_HEADER *pHeader = (X_VSH_SHADER_HEADER*)pFunction;
            EmuLog(LOG_LEVEL::WARNING, "Couldn't create vertex shader!");
            sprintf(pFileName, "failed%05d.xvu", FailedShaderCount);
            FILE *f = fopen(pFileName, "wb");
            if(f)
            {
                fwrite(pFunction, sizeof(X_VSH_SHADER_HEADER) + pHeader->NumInst * 16, 1, f);
                fclose(f);
            }
            FailedShaderCount++;
        }
#endif // _DEBUG_TRACK_VS
        //hRet = D3D_OK;
    }

    return hRet;
}

// LTCG specific D3DDevice_SetVertexShaderConstant function...
// This uses a custom calling convention where parameter is passed in EDX
// Test-case: Murakumo
VOID __stdcall XTL::EMUPATCH(D3DDevice_SetVertexShaderConstant_8)
(
)
{
	static uint32_t returnAddr;

#ifdef _DEBUG_TRACE
		__asm add esp, 4
#endif

	__asm {
		pop returnAddr
		push edx
		call EmuPatch_D3DDevice_SetVertexShaderConstant
		mov eax, 0
		push returnAddr
		ret
	}
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstant
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetVertexShaderConstant)
(
    INT         Register,
    CONST PVOID pConstantData,
    DWORD       ConstantCount
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Register)
		LOG_FUNC_ARG(pConstantData)
		LOG_FUNC_ARG(ConstantCount)
		LOG_FUNC_END;

/*#ifdef _DEBUG_TRACK_VS_CONST
    for (uint32_t i = 0; i < ConstantCount; i++)
    {
        printf("SetVertexShaderConstant, c%d  = { %f, %f, %f, %f }\n",
               Register + i,
               *((float*)pConstantData + 4 * i),
               *((float*)pConstantData + 4 * i + 1),
               *((float*)pConstantData + 4 * i + 2),
               *((float*)pConstantData + 4 * i + 3));
    }
#endif*/ // _DEBUG_TRACK_VS_CONST

    // Xbox vertex shader constants range from -96 to 95
    // The host does not support negative, so we adjust to 0..191
	Register += X_D3DSCM_CORRECTION;

    HRESULT hRet;
	hRet = g_pD3DDevice->SetVertexShaderConstantF(
		Register,
		(float*)pConstantData,
		ConstantCount
	);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexShaderConstant");

    if(FAILED(hRet))
    {
        EmuLog(LOG_LEVEL::WARNING, "We're lying about setting a vertex shader constant!");
        hRet = D3D_OK;
    }   
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstant1
// ******************************************************************
VOID __fastcall XTL::EMUPATCH(D3DDevice_SetVertexShaderConstant1)
(
    INT         Register,
    CONST PVOID pConstantData
)
{
	LOG_FORWARD("D3DDevice_SetVertexShaderConstant");

    // The XDK uses a macro to automatically adjust to 0..191 range
    // but D3DDevice_SetVertexShaderConstant expects -96..95 range
    // so we adjust before forwarding
    EMUPATCH(D3DDevice_SetVertexShaderConstant)(Register - X_D3DSCM_CORRECTION, pConstantData, 1);
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstant1Fast
// ******************************************************************
VOID __fastcall XTL::EMUPATCH(D3DDevice_SetVertexShaderConstant1Fast)
(
    INT         Register,
    CONST PVOID pConstantData
)
{
	LOG_FORWARD("D3DDevice_SetVertexShaderConstant");

    // The XDK uses a macro to automatically adjust to 0..191 range
    // but D3DDevice_SetVertexShaderConstant expects -96..95 range
    // so we adjust before forwarding
    EMUPATCH(D3DDevice_SetVertexShaderConstant)(Register - X_D3DSCM_CORRECTION, pConstantData, 1);
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstant4
// ******************************************************************
VOID __fastcall XTL::EMUPATCH(D3DDevice_SetVertexShaderConstant4)
(
    INT         Register,
    CONST PVOID pConstantData
)
{
	LOG_FORWARD("D3DDevice_SetVertexShaderConstant");

    // The XDK uses a macro to automatically adjust to 0..191 range
    // but D3DDevice_SetVertexShaderConstant expects -96..95 range
    // so we adjust before forwarding
	EMUPATCH(D3DDevice_SetVertexShaderConstant)(Register - X_D3DSCM_CORRECTION, pConstantData, 4);
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstantNotInline
// ******************************************************************
VOID __fastcall XTL::EMUPATCH(D3DDevice_SetVertexShaderConstantNotInline)
(
    INT         Register,
    CONST PVOID pConstantData,
    DWORD       ConstantCount
)
{
	LOG_FORWARD("D3DDevice_SetVertexShaderConstant");

    // The XDK uses a macro to automatically adjust to 0..191 range
    // but D3DDevice_SetVertexShaderConstant expects -96..95 range
    // so we adjust before forwarding
	EMUPATCH(D3DDevice_SetVertexShaderConstant)(Register - X_D3DSCM_CORRECTION, pConstantData, ConstantCount / 4);
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstantNotInlineFast
// ******************************************************************
VOID __fastcall XTL::EMUPATCH(D3DDevice_SetVertexShaderConstantNotInlineFast)
(
    INT         Register,
    CONST PVOID pConstantData,
    DWORD       ConstantCount
)
{
	LOG_FORWARD("D3DDevice_SetVertexShaderConstant");

    // The XDK uses a macro to automatically adjust to 0..191 range
    // but D3DDevice_SetVertexShaderConstant expects -96..95 range
    // so we adjust before forwarding
	EMUPATCH(D3DDevice_SetVertexShaderConstant)(Register - X_D3DSCM_CORRECTION, pConstantData, ConstantCount / 4);
}

// LTCG specific D3DDevice_SetTexture function...
// This uses a custom calling convention where parameter is passed in EAX
// TODO: XB_trampoline plus Log function is not working due lost parameter in EAX.
// Test-case: Metal Wolf Chaos
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetTexture_4)
(
	X_D3DBaseTexture  *pTexture
)
{
	DWORD           Stage;
	__asm mov Stage, eax;

	//LOG_FUNC_BEGIN
	//	LOG_FUNC_ARG(Stage)
	//	LOG_FUNC_ARG(pTexture)
	//	LOG_FUNC_END;
	EmuLog(LOG_LEVEL::DEBUG, "D3DDevice_SetTexture_4(Stage : %d pTexture : %08x);", Stage, pTexture);

	// Call the Xbox implementation of this function, to properly handle reference counting for us
	//XB_trampoline(VOID, WINAPI, D3DDevice_SetTexture_4, (X_D3DBaseTexture*));
	//XB_D3DDevice_SetTexture_4(pTexture);

	g_pXbox_SetTexture[Stage] = pTexture;
}

// ******************************************************************
// * patch: D3DDevice_SetTexture
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetTexture)
(
    DWORD           Stage,
	X_D3DBaseTexture  *pTexture
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Stage)
		LOG_FUNC_ARG(pTexture)
		LOG_FUNC_END;

	// Call the Xbox implementation of this function, to properly handle reference counting for us
	XB_trampoline(VOID, WINAPI, D3DDevice_SetTexture, (DWORD, X_D3DBaseTexture*));
	XB_D3DDevice_SetTexture(Stage, pTexture);

	g_pXbox_SetTexture[Stage] = pTexture;
}

// ******************************************************************
// * patch: D3DDevice_SwitchTexture
// ******************************************************************
VOID __fastcall XTL::EMUPATCH(D3DDevice_SwitchTexture)
(
    DWORD           Method,
    DWORD           Data,
    DWORD           Format
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Method)
		LOG_FUNC_ARG(Data)
		LOG_FUNC_ARG(Format)
		LOG_FUNC_END;

    DWORD StageLookup[XTL::X_D3DTS_STAGECOUNT] = { 0x00081b00, 0x00081b40, 0x00081b80, 0x00081bc0 };
	// This array contains D3DPUSH_ENCODE(NV2A_TX_OFFSET(v), 2) = 2 DWORD's, shifted left PUSH_COUNT_SHIFT (18) left
    DWORD Stage = -1;

    for (int v = 0; v < XTL::X_D3DTS_STAGECOUNT; v++) {
        if (StageLookup[v] == Method) {
            Stage = v;
			break;
        }
    }

    if (Stage == -1) {
		LOG_TEST_CASE("D3DDevice_SwitchTexture Unknown Method");
        EmuLog(LOG_LEVEL::WARNING, "Unknown Method (0x%.08X)", Method);
    }
    else {
		// Switch Texture updates the data pointer of an active texture using pushbuffer commands
		if (g_pXbox_SetTexture[Stage] == xbnullptr) {
			LOG_TEST_CASE("D3DDevice_SwitchTexture without an active texture");
		}
		else {
			//LOG_TEST_CASE("Using CxbxActiveTextureCopies");
			// See https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/1159
			// Test-case : Arena Football
			// Test-case : Call of Duty 2: Big Red One
			// Test-case : Crimson Skies
			// Test-case : Freedom Fighters - see https://www.youtube.com/watch?v=_NDCoLY8V3I
			// Test-case : Freestyle MetalX
			// Test-case : GENMA ONIMUSHA
			// Test-case : Gun
			// Test-case : Harry Potter : Quidditch World Cup
			// Test-case : King Arthur
			// Test-case : Madden NFL 2002
			// Test-case : Madden NFL 2005
			// Test-case : Madden NFL 07
			// Test-case : Need For Speed Most Wanted
			// Test-case : Need For Speed Underground
			// Test-case : PocketBike Racer
			// Test-case : Project Gotham Racing 2
			// Test-case : Richard Burns Rally
			// Test-case : Spider - Man 2

			// Update data and format separately, instead of via GetDataFromXboxResource()
			CxbxActiveTextureCopies[Stage].Common = g_pXbox_SetTexture[Stage]->Common;
			CxbxActiveTextureCopies[Stage].Data = Data;
			CxbxActiveTextureCopies[Stage].Format = Format;
			CxbxActiveTextureCopies[Stage].Lock = 0;
			CxbxActiveTextureCopies[Stage].Size = g_pXbox_SetTexture[Stage]->Size;

			// Use the above modified copy, instead of altering the active Xbox texture
			g_pXbox_SetTexture[Stage] = &CxbxActiveTextureCopies[Stage];
			// Note : Since g_pXbox_SetTexture and CxbxActiveTextureCopies are host-managed,
			// Xbox code should never alter these members (so : no reference counting, etc).
			// As long as that's guaranteed, this is a safe way to emulate SwitchTexture.
			// (GetHostResourceKey also avoids using any Xbox texture resource memory address.)
		}
    }
}

// ******************************************************************
// * patch: D3DDevice_Begin
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_Begin)
(
    X_D3DPRIMITIVETYPE     PrimitiveType
)
{
	LOG_FUNC_ONE_ARG(PrimitiveType);

    g_InlineVertexBuffer_PrimitiveType = PrimitiveType;
    g_InlineVertexBuffer_TableOffset = 0;
    g_InlineVertexBuffer_FVF = 0;
}

// ******************************************************************
// * patch: D3DDevice_SetVertexData2f
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetVertexData2f)
(
    int     Register,
    FLOAT   a,
    FLOAT   b
)
{
	LOG_FORWARD("D3DDevice_SetVertexData4f");

    EMUPATCH(D3DDevice_SetVertexData4f)(Register, a, b, 0.0f, 1.0f);
}

static inline DWORD FtoDW(FLOAT f) { return *((DWORD*)&f); }
static inline FLOAT DWtoF(DWORD f) { return *((FLOAT*)&f); }

// ******************************************************************
// * patch: D3DDevice_SetVertexData2s
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetVertexData2s)
(
    int     Register,
    SHORT   a,
    SHORT   b
)
{
	LOG_FORWARD("D3DDevice_SetVertexData4f");

	float fa, fb;
	
	// Test case: Halo
	// Note : XQEMU verified that the int16_t arguments
	// must be mapped to floats in the range [-32768.0, 32767.0]
	// (See https://github.com/xqemu/xqemu/pull/176)
	fa = (float)a;
	fb = (float)b;

    EMUPATCH(D3DDevice_SetVertexData4f)(Register, fa, fb, 0.0f, 1.0f);
}

extern uint32_t HLE_read_NV2A_pgraph_register(const int reg); // Declared in PushBuffer.cpp
extern void HLE_write_NV2A_vertex_attribute_slot(unsigned slot, uint32_t parameter); // Declared in PushBuffer.cpp
extern uint32_t HLE_read_NV2A_vertex_attribute_slot(unsigned VertexSlot); // Declared in PushBuffer.cpp

extern NV2ADevice* g_NV2A;

// ******************************************************************
// * patch: D3DDevice_SetVertexData4f_16
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetVertexData4f_16)
(
	FLOAT   a,
	FLOAT   b,
	FLOAT   c,
	FLOAT   d
)
{
	// This is an LTCG specific version of SetVertexData4f where the first param is passed in edi
	int Register = 0;
	__asm{
		mov Register, edi
	}

	LOG_FORWARD("D3DDevice_SetVertexData4f");

	EMUPATCH(D3DDevice_SetVertexData4f)(Register, a, b, c, d);
}

// ******************************************************************
// * patch: D3DDevice_SetVertexData4f
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetVertexData4f)
(
    int     Register,
    FLOAT   a,
    FLOAT   b,
    FLOAT   c,
    FLOAT   d
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Register)
		LOG_FUNC_ARG(a)
		LOG_FUNC_ARG(b)
		LOG_FUNC_ARG(c)
		LOG_FUNC_ARG(d)
		LOG_FUNC_END;

	HRESULT hRet = D3D_OK;

	// Get the vertex shader flags (if any is active) :
	uint32_t ActiveVertexAttributeFlags = 0;
	if (VshHandleIsVertexShader(g_Xbox_VertexShader_Handle)) {
		LOG_TEST_CASE("D3DDevice_SetVertexData4f with active VertexShader");
		X_D3DVertexShader *pXboxVertexShader = VshHandleToXboxVertexShader(g_Xbox_VertexShader_Handle);
		if (!(pXboxVertexShader->Flags & 0x10/*=X_VERTEXSHADER_PROGRAM*/)) {
			ActiveVertexAttributeFlags = pXboxVertexShader->Flags;
		}

		// If we have an active vertex shader, we also write the input to a vertex shader constant
		// This allows us to implement Xbox functionality where SetVertexData4f can be used to specify attributes
		// not present in the vertex declaration.
		// We use range 193 and up to store these values, as Xbox shaders stop at c192!
		FLOAT values[] = {a,b,c,d};
		g_pD3DDevice->SetVertexShaderConstantF(CXBX_D3DVS_CONSTREG_VERTEXDATA4F_BASE + Register, values, 1);
	}

	// Grow g_InlineVertexBuffer_Table to contain at least current, and a potentially next vertex
	if (g_InlineVertexBuffer_TableLength <= g_InlineVertexBuffer_TableOffset + 1) {
		if (g_InlineVertexBuffer_TableLength == 0) {
			g_InlineVertexBuffer_TableLength = PAGE_SIZE / sizeof(struct _D3DIVB);
		} else {
			g_InlineVertexBuffer_TableLength *= 2;
		}

		g_InlineVertexBuffer_Table = (struct _D3DIVB*)realloc(g_InlineVertexBuffer_Table, sizeof(struct _D3DIVB) * g_InlineVertexBuffer_TableLength);
		EmuLog(LOG_LEVEL::DEBUG, "Reallocated g_InlineVertexBuffer_Table to %d entries", g_InlineVertexBuffer_TableLength);
	}

	// Is this the initial call after D3DDevice_Begin() ?
	if (g_InlineVertexBuffer_FVF == 0) {
		// Set first vertex to zero (preventing leaks from prior Begin/End calls)
		g_InlineVertexBuffer_Table[0] = {};

		// Handle persistent vertex attribute flags, by resetting non-persistent colors
		// to their default value (and leaving the persistent colors alone - see the
		// "Copy all attributes of the previous vertex" comment below) :
		static const uint32_t ColorBlack = D3DCOLOR_ARGB(0, 0, 0, 0);
		static const uint32_t ColorWhite = D3DCOLOR_ARGB(255, 255, 255, 255);

		// If needed, write default vertex colors to HLE NV2A pgraph :
		if (!(ActiveVertexAttributeFlags & X_D3DUSAGE_PERSISTENTDIFFUSE)) {
			HLE_write_NV2A_vertex_attribute_slot(X_D3DVSDE_DIFFUSE, ColorWhite);
		}

		if (!(ActiveVertexAttributeFlags & X_D3DUSAGE_PERSISTENTSPECULAR)) {
			HLE_write_NV2A_vertex_attribute_slot(X_D3DVSDE_SPECULAR, ColorBlack);
		}

		if (!(ActiveVertexAttributeFlags & X_D3DUSAGE_PERSISTENTBACKDIFFUSE)) {
			HLE_write_NV2A_vertex_attribute_slot(X_D3DVSDE_BACKDIFFUSE, ColorWhite);
		}

		if (!(ActiveVertexAttributeFlags & X_D3DUSAGE_PERSISTENTBACKSPECULAR)) {
			HLE_write_NV2A_vertex_attribute_slot(X_D3DVSDE_BACKSPECULAR, ColorBlack);
		}

		// Read starting vertex colors from HLE NV2A pgraph :
		g_InlineVertexBuffer_Table[0].Diffuse = HLE_read_NV2A_vertex_attribute_slot(X_D3DVSDE_DIFFUSE);
		g_InlineVertexBuffer_Table[0].Specular = HLE_read_NV2A_vertex_attribute_slot(X_D3DVSDE_SPECULAR);
		g_InlineVertexBuffer_Table[0].BackDiffuse = HLE_read_NV2A_vertex_attribute_slot(X_D3DVSDE_BACKDIFFUSE);
		g_InlineVertexBuffer_Table[0].BackSpecular = HLE_read_NV2A_vertex_attribute_slot(X_D3DVSDE_BACKSPECULAR);
	}

	int o = g_InlineVertexBuffer_TableOffset;
	unsigned int FVFPosType = g_InlineVertexBuffer_FVF & D3DFVF_POSITION_MASK;

	switch(Register)
	{
		case X_D3DVSDE_VERTEX:
		case X_D3DVSDE_POSITION:
		{
			// Note : Setting position signals completion of a vertex
			g_InlineVertexBuffer_Table[o].Position.x = a;
			g_InlineVertexBuffer_Table[o].Position.y = b;
			g_InlineVertexBuffer_Table[o].Position.z = c;
			g_InlineVertexBuffer_Table[o].Rhw = d; // Was : 1.0f; // Dxbx note : Why set Rhw to 1.0? And why ignore d?

			switch (g_InlineVertexBuffer_FVF & D3DFVF_POSITION_MASK) {
			case 0:
				// No position mask given yet, set it now :
				if (g_InlineVertexBuffer_FVF & D3DFVF_NORMAL) {
					// See https://msdn.microsoft.com/ru-ru/library/windows/desktop/bb172559(v=vs.85).aspx and DxbxFVFToVertexSizeInBytes 
					// D3DFVF_NORMAL cannot be combined with D3DFVF_XYZRHW :
					g_InlineVertexBuffer_FVF |= D3DFVF_XYZ;
					g_InlineVertexBuffer_Table[o].Rhw = 1.0f; // This, just to stay close to prior behaviour
				}
				else {
					// Without D3DFVF_NORMAL, assume D3DFVF_XYZRHW
					g_InlineVertexBuffer_FVF |= D3DFVF_XYZRHW;
				}
				break;
			case D3DFVF_XYZ:
			case D3DFVF_XYZRHW:
			case D3DFVF_XYZB1:
				// These are alright
				break;
			default:
				EmuLog(LOG_LEVEL::WARNING, "D3DDevice_SetVertexData4f unexpected FVF when selecting D3DFVF_XYZ(RHW) : %x", g_InlineVertexBuffer_FVF);
				// TODO : How to resolve this?
			}

			// Start a new vertex
			g_InlineVertexBuffer_TableOffset++;
			// Copy all attributes of the previous vertex (if any) to the new vertex
			g_InlineVertexBuffer_Table[g_InlineVertexBuffer_TableOffset] = g_InlineVertexBuffer_Table[o];
	
			break;
		}

		case X_D3DVSDE_BLENDWEIGHT:
		{
			g_InlineVertexBuffer_Table[o].Blend[0] = a;
			g_InlineVertexBuffer_Table[o].Blend[1] = b;
			g_InlineVertexBuffer_Table[o].Blend[2] = c;
			g_InlineVertexBuffer_Table[o].Blend[3] = d;
			// TODO: Test the above. 
			// Xbox supports up to 4 blendweights

			switch (g_InlineVertexBuffer_FVF & D3DFVF_POSITION_MASK) {
			case 0:
				// No position mask given yet, set it now :
				g_InlineVertexBuffer_FVF |= D3DFVF_XYZB1;
				// TODO: How to select blendweight D3DFVF_XYZB2 or up?
				break;
			case D3DFVF_XYZB1:
				// These are alright
				break;
			default:
				EmuLog(LOG_LEVEL::WARNING, "D3DDevice_SetVertexData4f unexpected FVF when processing X_D3DVSDE_BLENDWEIGHT : %x", g_InlineVertexBuffer_FVF);
				g_InlineVertexBuffer_FVF &= ~D3DFVF_POSITION_MASK; // for now, remove prior position mask, leading to blending below
				g_InlineVertexBuffer_FVF |= D3DFVF_XYZB1;
				// TODO: How to select blendweight D3DFVF_XYZB2 or up?
				// TODO : How to resolve this?
			}

			break;
		}

		case X_D3DVSDE_NORMAL:
		{
			g_InlineVertexBuffer_Table[o].Normal.x = a;
			g_InlineVertexBuffer_Table[o].Normal.y = b;
			g_InlineVertexBuffer_Table[o].Normal.z = c;
			g_InlineVertexBuffer_FVF |= D3DFVF_NORMAL;
			break;
		}

		case X_D3DVSDE_DIFFUSE:
		{
			g_InlineVertexBuffer_Table[o].Diffuse = D3DCOLOR_COLORVALUE(a, b, c, d);
			g_InlineVertexBuffer_FVF |= D3DFVF_DIFFUSE;
            HLE_write_NV2A_vertex_attribute_slot(X_D3DVSDE_DIFFUSE, g_InlineVertexBuffer_Table[o].Diffuse);
			break;
		}

		case X_D3DVSDE_SPECULAR:
		{
			g_InlineVertexBuffer_Table[o].Specular = D3DCOLOR_COLORVALUE(a, b, c, d);
			g_InlineVertexBuffer_FVF |= D3DFVF_SPECULAR;
            HLE_write_NV2A_vertex_attribute_slot(X_D3DVSDE_SPECULAR, g_InlineVertexBuffer_Table[o].Specular);
			break;
		}

		case X_D3DVSDE_FOG: // Xbox extension
		{
			g_InlineVertexBuffer_Table[o].Fog = a; // TODO : What about the other (b, c and d) arguments?
			//EmuLog(LOG_LEVEL::WARNING, "Host Direct3D8 doesn''t support FVF FOG");
			break;
		}

		// Note : X_D3DVSDE_POINTSIZE: Maps to D3DFVF_PSIZE, which is not available on Xbox FVF's

		case X_D3DVSDE_BACKDIFFUSE: // Xbox extension
		{
			g_InlineVertexBuffer_Table[o].BackDiffuse = D3DCOLOR_COLORVALUE(a, b, c, d);
			//EmuLog(LOG_LEVEL::WARNING, "Host Direct3D8 doesn''t support FVF BACKDIFFUSE");
            HLE_write_NV2A_vertex_attribute_slot(X_D3DVSDE_BACKDIFFUSE, g_InlineVertexBuffer_Table[o].BackDiffuse);
			break;
		}

		case X_D3DVSDE_BACKSPECULAR: // Xbox extension
		{
			g_InlineVertexBuffer_Table[o].BackSpecular = D3DCOLOR_COLORVALUE(a, b, c, d);
			//EmuLog(LOG_LEVEL::WARNING, "Host Direct3D8 doesn''t support FVF BACKSPECULAR");
            HLE_write_NV2A_vertex_attribute_slot(X_D3DVSDE_BACKSPECULAR, g_InlineVertexBuffer_Table[o].BackSpecular);
			break;
		}

		case X_D3DVSDE_TEXCOORD0:
		{
			g_InlineVertexBuffer_Table[o].TexCoord[0].x = a;
			g_InlineVertexBuffer_Table[o].TexCoord[0].y = b;
			g_InlineVertexBuffer_Table[o].TexCoord[0].z = c;
			g_InlineVertexBuffer_Table[o].TexCoord[0].w = d;
			if ((g_InlineVertexBuffer_FVF & D3DFVF_TEXCOUNT_MASK) < D3DFVF_TEX1) {
				// Dxbx fix : Use mask, else the format might get expanded incorrectly :
				g_InlineVertexBuffer_FVF &= ~D3DFVF_TEXCOUNT_MASK;
				g_InlineVertexBuffer_FVF |= D3DFVF_TEX1;
				// Dxbx note : Correct usage of D3DFVF_TEX1 (and the other cases below)
				// can be tested with "Daphne Xbox" (the Laserdisc Arcade Game Emulator).
			}

			break;
		}

		case X_D3DVSDE_TEXCOORD1:
		{
			g_InlineVertexBuffer_Table[o].TexCoord[1].x = a;
			g_InlineVertexBuffer_Table[o].TexCoord[1].y = b;
			g_InlineVertexBuffer_Table[o].TexCoord[1].z = c;
			g_InlineVertexBuffer_Table[o].TexCoord[1].w = d;
			if ((g_InlineVertexBuffer_FVF & D3DFVF_TEXCOUNT_MASK) < D3DFVF_TEX2) {
				g_InlineVertexBuffer_FVF &= ~D3DFVF_TEXCOUNT_MASK;
				g_InlineVertexBuffer_FVF |= D3DFVF_TEX2;
			}

			break;
		}

		case X_D3DVSDE_TEXCOORD2:
		{
			g_InlineVertexBuffer_Table[o].TexCoord[2].x = a;
			g_InlineVertexBuffer_Table[o].TexCoord[2].y = b;
			g_InlineVertexBuffer_Table[o].TexCoord[2].z = c;
			g_InlineVertexBuffer_Table[o].TexCoord[2].w = d;
			if ((g_InlineVertexBuffer_FVF & D3DFVF_TEXCOUNT_MASK) < D3DFVF_TEX3) {
				g_InlineVertexBuffer_FVF &= ~D3DFVF_TEXCOUNT_MASK;
				g_InlineVertexBuffer_FVF |= D3DFVF_TEX3;
			}

			break;
		}

		case X_D3DVSDE_TEXCOORD3:
		{
			g_InlineVertexBuffer_Table[o].TexCoord[3].x = a;
			g_InlineVertexBuffer_Table[o].TexCoord[3].y = b;
			g_InlineVertexBuffer_Table[o].TexCoord[3].z = c;
			g_InlineVertexBuffer_Table[o].TexCoord[3].w = d;
			if ((g_InlineVertexBuffer_FVF & D3DFVF_TEXCOUNT_MASK) < D3DFVF_TEX4) {
				g_InlineVertexBuffer_FVF &= ~D3DFVF_TEXCOUNT_MASK;
				g_InlineVertexBuffer_FVF |= D3DFVF_TEX4;
			}

			break;
		}

		default:
			EmuLog(LOG_LEVEL::WARNING, "Unknown IVB Register : %d", Register);
	}
}

// ******************************************************************
// * patch: D3DDevice_SetVertexData4ub
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetVertexData4ub)
(
	INT		Register,
	BYTE	a,
	BYTE	b,
	BYTE	c,
	BYTE	d
)
{
	LOG_FORWARD("D3DDevice_SetVertexData4f");

	float fa = a / 255.0f;
	float fb = b / 255.0f;
	float fc = c / 255.0f;
	float fd = d / 255.0f;

    EMUPATCH(D3DDevice_SetVertexData4f)(Register, fa, fb, fc, fd);
}

// ******************************************************************
// * patch: D3DDevice_SetVertexData4s
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetVertexData4s)
(
	INT		Register,
	SHORT	a,
	SHORT	b,
	SHORT	c,
	SHORT	d
)
{
	LOG_FORWARD("D3DDevice_SetVertexData4f");

	float fa, fb, fc, fd;

	// Test case: Halo
	// Note : XQEMU verified that the int16_t arguments
	// must be mapped to floats in the range [-32768.0, 32767.0]
	// (See https://github.com/xqemu/xqemu/pull/176)
	fa = (float)a;
	fb = (float)b;
	fc = (float)c;
	fd = (float)d;

    EMUPATCH(D3DDevice_SetVertexData4f)(Register, fa, fb, fc, fd);
}

// ******************************************************************
// * patch: D3DDevice_SetVertexDataColor
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetVertexDataColor)
(
    int         Register,
    D3DCOLOR    Color
)
{
	LOG_FORWARD("D3DDevice_SetVertexData4f");

    D3DXCOLOR XColor = Color;

    EMUPATCH(D3DDevice_SetVertexData4f)(Register, XColor.r, XColor.g, XColor.b, XColor.a);
}

// ******************************************************************
// * patch: D3DDevice_End
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_End)()
{
	LOG_FUNC();

    if(g_InlineVertexBuffer_TableOffset > 0)
        EmuFlushIVB();

    // TODO: Should technically clean this up at some point..but on XP doesnt matter much
//    g_VMManager.Deallocate((VAddr)g_InlineVertexBuffer_pData);
//    g_VMManager.Deallocate((VAddr)g_InlineVertexBuffer_Table);
}

// ******************************************************************
// * patch: D3DDevice_RunPushBuffer
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_RunPushBuffer)
(
    X_D3DPushBuffer       *pPushBuffer,
    X_D3DFixup            *pFixup
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pPushBuffer)
		LOG_FUNC_ARG(pFixup)
		LOG_FUNC_END;

	EmuExecutePushBuffer(pPushBuffer, pFixup);    
}

// ******************************************************************
// * patch: D3DDevice_Clear
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_Clear)
(
    DWORD           Count,
    CONST D3DRECT  *pRects,
    DWORD           Flags,
    D3DCOLOR        Color,
    float           Z,
    DWORD           Stencil
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Count)
		LOG_FUNC_ARG(pRects)
		LOG_FUNC_ARG(Flags)
		LOG_FUNC_ARG(Color)
		LOG_FUNC_ARG(Z)
		LOG_FUNC_ARG(Stencil)
		LOG_FUNC_END;

	DWORD HostFlags = 0;

    // make adjustments to parameters to make sense with windows d3d
    {
		if (Flags & X_D3DCLEAR_TARGET) {
			// TODO: D3DCLEAR_TARGET_A, *R, *G, *B don't exist on windows
			if ((Flags & X_D3DCLEAR_TARGET) != X_D3DCLEAR_TARGET)
				EmuLog(LOG_LEVEL::WARNING, "Unsupported : Partial D3DCLEAR_TARGET flag(s) for D3DDevice_Clear : 0x%.08X", Flags & X_D3DCLEAR_TARGET);
		
			HostFlags |= D3DCLEAR_TARGET;
		}

        // Do not needlessly clear Z Buffer
		if (Flags & X_D3DCLEAR_ZBUFFER) {
			if (g_bHasDepth)
				HostFlags |= D3DCLEAR_ZBUFFER;
			else
				EmuLog(LOG_LEVEL::WARNING, "Unsupported : D3DCLEAR_ZBUFFER flag for D3DDevice_Clear without ZBuffer");
		}

		// Only clear depth buffer and stencil if present
		//
		// Avoids following DirectX Debug Runtime error report
		//    [424] Direct3D8: (ERROR) :Invalid flag D3DCLEAR_ZBUFFER: no zbuffer is associated with device. Clear failed. 
		if (Flags & X_D3DCLEAR_STENCIL) {
			if (g_bHasStencil)
				HostFlags |= D3DCLEAR_STENCIL;
			else
				EmuLog(LOG_LEVEL::WARNING, "Unsupported : D3DCLEAR_STENCIL flag for D3DDevice_Clear without ZBuffer");
		}

        if(Flags & ~(X_D3DCLEAR_TARGET | X_D3DCLEAR_ZBUFFER | X_D3DCLEAR_STENCIL))
            EmuLog(LOG_LEVEL::WARNING, "Unsupported Flag(s) for D3DDevice_Clear : 0x%.08X", Flags & ~(X_D3DCLEAR_TARGET | X_D3DCLEAR_ZBUFFER | X_D3DCLEAR_STENCIL));
    }

    HRESULT hRet;

    if (pRects != nullptr) {
        // Scale the fill based on our scale factor
        D3DRECT rect = *pRects;
        rect.x1 *= g_RenderScaleFactor;
        rect.x2 *= g_RenderScaleFactor;
        rect.y1 *= g_RenderScaleFactor;
        rect.y2 *= g_RenderScaleFactor;
        hRet = g_pD3DDevice->Clear(Count, &rect, HostFlags, Color, Z, Stencil);
    } else {
        hRet = g_pD3DDevice->Clear(Count, pRects, HostFlags, Color, Z, Stencil);
    }

	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->Clear");
}


// ******************************************************************
// * patch: D3DDevice_CopyRects
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_CopyRects)
(
    X_D3DSurface* pSourceSurface,
    CONST RECT* pSourceRectsArray,
    UINT                cRects,
    X_D3DSurface* pDestinationSurface,
    CONST POINT* pDestPointsArray
)
{
    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pSourceSurface);
        LOG_FUNC_ARG(pSourceRectsArray);
        LOG_FUNC_ARG(cRects);
        LOG_FUNC_ARG(pDestinationSurface);
        LOG_FUNC_ARG(pDestPointsArray);
    LOG_FUNC_END;

    // We skip the trampoline to prevent unnecessary work
    // As our surfaces remain on the GPU, calling the trampoline would just
    // result in a memcpy from an empty Xbox surface to another empty Xbox Surface
    D3DSURFACE_DESC SourceDesc, DestinationDesc;
    auto pHostSourceSurface = GetHostSurface(pSourceSurface);
    auto pHostDestSurface = GetHostSurface(pDestinationSurface);

    if (pHostSourceSurface == nullptr || pHostDestSurface == nullptr) {
        // Test Case: DOA2 attempts to copy from an index buffer resource type
        // TODO: What should we do here?
        LOG_TEST_CASE("D3DDevice-CopyRects: Failed to fetch host surfaces");
        return;
    }

    pHostSourceSurface->GetDesc(&SourceDesc);
    pHostDestSurface->GetDesc(&DestinationDesc);

    // If the source is a render-target and the destination is not, we need force it to be re-created as one
    // This is because StrechRects cannot copy from a Render-Target to a Non-Render Target
    // Test Case: Crash Bandicoot: Wrath of Cortex attemps to copy the render-target to a texture
    // This fixes an issue on the pause screen where the screenshot of the current scene was not displayed correctly
    if ((SourceDesc.Usage & D3DUSAGE_RENDERTARGET) != 0 && (DestinationDesc.Usage & D3DUSAGE_RENDERTARGET) == 0) {
        pHostDestSurface = GetHostSurface(pDestinationSurface, D3DUSAGE_RENDERTARGET);
        pHostDestSurface->GetDesc(&DestinationDesc);
    }

    // If no rectangles were given, default to 1 (entire surface)
    if (cRects == 0) {
        cRects = 1;
    }

    for (UINT i = 0; i < cRects; i++) {
        RECT SourceRect, DestRect;

        if (pSourceRectsArray != nullptr) {
            SourceRect = pSourceRectsArray[i];
        } else {
            SourceRect.left = 0;
            SourceRect.right = SourceDesc.Width;
            SourceRect.top = 0;
            SourceRect.bottom = SourceDesc.Height;
        }

        if (pDestPointsArray != nullptr) {
            DestRect.left = pDestPointsArray[i].x;
            DestRect.right = DestRect.left + (SourceRect.right - SourceRect.left);
            DestRect.top = pDestPointsArray[i].y;
            DestRect.bottom = DestRect.top + (SourceRect.bottom - SourceRect.top);
        } else if (pSourceRectsArray) {
            DestRect = SourceRect;
        } else {
            DestRect.left = 0;
            DestRect.right = DestinationDesc.Width;
            DestRect.top = 0;
            DestRect.bottom = DestinationDesc.Height;
        }

        HRESULT hRet = g_pD3DDevice->StretchRect(pHostSourceSurface, &SourceRect, pHostDestSurface, &DestRect, D3DTEXF_NONE);
        if (FAILED(hRet)) {
            LOG_TEST_CASE("D3DDevice_CopyRects: Failed to copy surface");
        }
    }
}

#define CXBX_SWAP_PRESENT_FORWARD (256 + 4 + 1) // = CxbxPresentForwardMarker + D3DSWAP_FINISH + D3DSWAP_COPY

// ******************************************************************
// * patch: D3DDevice_Present
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_Present)
(
    CONST RECT* pSourceRect,
    CONST RECT* pDestRect,
    PVOID       pDummy1,
    PVOID       pDummy2
)
{
	// LOG_FORWARD("D3DDevice_Swap");
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pSourceRect)
		LOG_FUNC_ARG(pDestRect)
		LOG_FUNC_ARG(pDummy1)
		LOG_FUNC_ARG(pDummy2)
		LOG_FUNC_END;

	EMUPATCH(D3DDevice_Swap)(CXBX_SWAP_PRESENT_FORWARD); // Xbox present ignores
}

std::chrono::time_point<std::chrono::high_resolution_clock> frameStartTime;

// LTCG specific swap function...
// Massive hack, but could coax some more LTCG titles into booting with HLE
// This uses a custom calling convention where parameter is passed in EAX
DWORD XTL::EMUPATCH(D3DDevice_Swap_0)
(
)
{
	uint32_t param;
	__asm {
		mov param, eax;
	}

	return EMUPATCH(D3DDevice_Swap)(param);
}

// ******************************************************************
// * patch: D3DDevice_Swap
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(D3DDevice_Swap)
(
	DWORD Flags
)
{
	LOG_FUNC_ONE_ARG(Flags);

	// TODO: Ensure this flag is always the same across library versions
	if (Flags != 0 && Flags != CXBX_SWAP_PRESENT_FORWARD)
			EmuLog(LOG_LEVEL::WARNING, "XTL::EmuD3DDevice_Swap: Flags != 0");

	// Fetch the host backbuffer
	IDirect3DSurface *pCurrentHostBackBuffer = nullptr;
	HRESULT hRet = g_pD3DDevice->GetBackBuffer(
		0, // iSwapChain
		0, D3DBACKBUFFER_TYPE_MONO, &pCurrentHostBackBuffer);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetBackBuffer - Unable to get backbuffer surface!");
	if (hRet == D3D_OK) {
		assert(pCurrentHostBackBuffer != nullptr);

		// Get backbuffer dimensions; TODO : remember this once, at creation/resize time
		D3DSURFACE_DESC BackBufferDesc;
		pCurrentHostBackBuffer->GetDesc(&BackBufferDesc);

        // TODO: Implement a hot-key to change the filter?
        // Note: LoadSurfaceFilter Must be D3DTEXF_NONE, D3DTEXF_POINT or D3DTEXF_LINEAR
        // Before StretchRects we used D3DX_FILTER_POINT here, but that gave jagged edges in Dashboard.
        // Dxbx note : D3DX_FILTER_LINEAR gives a smoother image, but 'bleeds' across borders
        // LoadOverlayFilter must be a D3DX filter DWORD value
        const D3DTEXTUREFILTERTYPE LoadSurfaceFilter = D3DTEXF_LINEAR;
        const DWORD LoadOverlayFilter = D3DX_DEFAULT;

		auto pXboxBackBufferHostSurface = GetHostSurface(g_pXbox_BackBufferSurface, D3DUSAGE_RENDERTARGET);
		if (pXboxBackBufferHostSurface) {
			// Blit Xbox BackBuffer to host BackBuffer
			// TODO: Respect aspect ratio
            hRet = g_pD3DDevice->StretchRect(
                /* pSourceSurface = */ pXboxBackBufferHostSurface,
                /* pSourceRect = */ nullptr,
                /* pDestSurface = */ pCurrentHostBackBuffer,
                /* pDestRect = */ nullptr,
                /* Filter = */ LoadSurfaceFilter
            );
		
			if (hRet != D3D_OK) {
				EmuLog(LOG_LEVEL::WARNING, "Couldn't blit Xbox BackBuffer to host BackBuffer : %X", hRet);
			}
		}

		// Is there an overlay to be presented too?
		if (g_OverlayProxy.Surface.Common) {
			X_D3DFORMAT X_Format = GetXboxPixelContainerFormat(&g_OverlayProxy.Surface);
			if (X_Format != X_D3DFMT_YUY2) {
				LOG_TEST_CASE("Xbox overlay surface isn't using X_D3DFMT_YUY2");
			}

			// Interpret the Xbox overlay data (depending the color space conversion render state)
			// as either YUV or RGB format (note that either one must be a 3 bytes per pixel format)
			D3DFORMAT PCFormat;
			// TODO : Before reading from pgraph, flush all pending push-buffer commands
			switch (GET_MASK(HLE_read_NV2A_pgraph_register(NV_PGRAPH_CONTROL_0), NV_PGRAPH_CONTROL_0_CSCONVERT)) {
			case 0:  // = pass-through
				PCFormat = D3DFMT_YUY2;
				break;
			case 1: // = CRYCB_TO_RGB
				PCFormat = D3DFMT_YUY2; // Test-case : Turok (intro movie)
				break;
			case 2: // = SCRYSCB_TO_RGB
				LOG_TEST_CASE("SCRYSCB_TO_RGB");
				PCFormat = D3DFMT_YUY2;
				break;
			default:
				LOG_TEST_CASE("Unrecognized NV_PGRAPH_CONTROL_0_CSCONVERT");
				PCFormat = D3DFMT_YUY2;
				break;
			}

			// Blit Xbox overlay to host backbuffer
			uint8_t *pOverlayData = (uint8_t*)GetDataFromXboxResource(&g_OverlayProxy.Surface);
			UINT OverlayWidth, OverlayHeight, OverlayDepth, OverlayRowPitch, OverlaySlicePitch;
			CxbxGetPixelContainerMeasures(
				&g_OverlayProxy.Surface,
				0, // dwMipMapLevel
				&OverlayWidth, &OverlayHeight, &OverlayDepth, &OverlayRowPitch, &OverlaySlicePitch);

            RECT EmuSourRect = { 0 };
            RECT EmuDestRect = { 0 };

			if (g_OverlayProxy.SrcRect.right > 0) {
				EmuSourRect = g_OverlayProxy.SrcRect;
			}
			else {
				SetRect(&EmuSourRect, 0, 0, OverlayWidth, OverlayHeight);
			}

			if (g_OverlayProxy.DstRect.right > 0) {
				// If there's a destination rectangle given, copy that into our local variable :
				EmuDestRect = g_OverlayProxy.DstRect;

                // Make sure to scale the values based on the difference between the Xbox and Host backbuffer
                // We can't use the scale factor here because we are blitting directly to the host backbuffer
                // NOT an Xbox surface!
                DWORD XboxBackBufferWidth = GetPixelContainerWidth(g_pXbox_BackBufferSurface);
                DWORD XboxBackBufferHeight = GetPixelContainerHeight(g_pXbox_BackBufferSurface);

                float xScale = (float)BackBufferDesc.Width / (float)XboxBackBufferWidth;
                float yScale = (float)BackBufferDesc.Height / (float)XboxBackBufferHeight;

                EmuDestRect.top = (LONG)(EmuDestRect.top * yScale);
                EmuDestRect.left = (LONG)(EmuDestRect.left * xScale);
                EmuDestRect.bottom = (LONG)(EmuDestRect.bottom * yScale);
                EmuDestRect.right = (LONG)(EmuDestRect.right * xScale);
			} else {
				// Use backbuffer width/height since that may differ from the Window size
                EmuDestRect.right = BackBufferDesc.Width;
                EmuDestRect.bottom = BackBufferDesc.Height;
			}

			// load the YUY2 into the backbuffer

			// Limit the width and height of the output to the backbuffer dimensions.
			// This will (hopefully) prevent exceptions in Blinx - The Time Sweeper
			// (see https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/285)
			{
				// Use our (bounded) copy when bounds exceed :
				if (EmuDestRect.right > (LONG)BackBufferDesc.Width) {
					EmuDestRect.right = (LONG)BackBufferDesc.Width;
				}

				if (EmuDestRect.bottom > (LONG)BackBufferDesc.Height) {
					EmuDestRect.bottom = (LONG)BackBufferDesc.Height;
				}
			}

            // Create a temporary surface to hold the overlay
            // This is faster than loading directly into the backbuffer because it offloads scaling to the GPU
            // Without this, upscaling tanks the frame-rate!
            IDirect3DSurface* pTemporaryOverlaySurface;
            HRESULT hRet = g_pD3DDevice->CreateOffscreenPlainSurface(
                OverlayWidth,
                OverlayHeight,
                D3DFMT_A8R8G8B8,
                D3DPOOL_DEFAULT,
                &pTemporaryOverlaySurface,
                nullptr
            );

            if (FAILED(hRet)) {
                EmuLog(LOG_LEVEL::WARNING, "Couldn't create temporary overlay surface : %X", hRet);
            } else {
                RECT doNotScaleRect = { 0, 0, (LONG)OverlayWidth, (LONG)OverlayHeight };

                // Use D3DXLoadSurfaceFromMemory() to do conversion, we don't stretch at this moment in time
                // avoiding the need for YUY2toARGB() (might become relevant when porting to D3D9 or OpenGL)
                // see https://msdn.microsoft.com/en-us/library/windows/desktop/bb172902(v=vs.85).aspx
                hRet = D3DXLoadSurfaceFromMemory(
                    /* pDestSurface = */ pTemporaryOverlaySurface,
                    /* pDestPalette = */ nullptr,
                    /* pDestRect = */ &doNotScaleRect,
                    /* pSrcMemory = */ pOverlayData, // Source buffer
                    /* SrcFormat = */ PCFormat,
                    /* SrcPitch = */ OverlayRowPitch,
                    /* pSrcPalette = */ nullptr,
                    /* pSrcRect = */ &doNotScaleRect, // This parameter cannot be NULL
                    /* Filter = */ LoadOverlayFilter,
                    /* ColorKey = */ g_OverlayProxy.EnableColorKey ? g_OverlayProxy.ColorKey : 0);

                DEBUG_D3DRESULT(hRet, "D3DXLoadSurfaceFromMemory - UpdateOverlay could not convert buffer!\n");
                if (hRet != D3D_OK) {
                    EmuLog(LOG_LEVEL::WARNING, "Couldn't load Xbox overlay to host surface : %X", hRet);
                } else {
                    // TODO: Respect aspect ratio
                    hRet = g_pD3DDevice->StretchRect(
                        /* pSourceSurface = */ pTemporaryOverlaySurface,
                        /* pSourceRect = */ &EmuSourRect,
                        /* pDestSurface = */ pCurrentHostBackBuffer,
                        /* pDestRect = */ &EmuDestRect,
                        /* Filter = */ LoadSurfaceFilter
                    );

                    if (hRet != D3D_OK) {
                        EmuLog(LOG_LEVEL::WARNING, "Couldn't load Xbox overlay to host back buffer : %X", hRet);
                    }
                }

                pTemporaryOverlaySurface->Release();
            }
		}

		pCurrentHostBackBuffer->Release();
	}

	g_pD3DDevice->EndScene();

	hRet = g_pD3DDevice->Present(0, 0, 0, 0);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->Present");

	hRet = g_pD3DDevice->BeginScene();

    // RenderStates need reapplying each frame, but can be re-used between draw calls
    // This forces them to be reset
    XboxRenderStates.SetDirty();

    // Check if we need to enable our frame-limiter
    DWORD presentationInverval = g_Xbox_PresentationInterval_Override > 0 ? g_Xbox_PresentationInterval_Override : g_Xbox_PresentationInterval_Default;
    if ((presentationInverval != D3DPRESENT_INTERVAL_IMMEDIATE) && !g_bHack_UnlockFramerate) {
        // If the last frame completed faster than the Xbox target swap rate, wait for it

        auto targetRefreshRate = 60.0f; // TODO: Read from Xbox Display Mode

        // Determine how many 'frames' worth of time we need to wait for
        // This allows games that require a locked framerate (eg JSRF) to function correctly
        // While allowing titles with an unlocked frame-rate to not be limited
        auto multiplier = 1.0f;
        switch (presentationInverval) {
            case D3DPRESENT_INTERVAL_ONE:
            case 0x80000001: // D3DPRESENT_INTERVAL_ONE_OR_IMMEDIATE:
                multiplier = 1.0f;
                break;
            case D3DPRESENT_INTERVAL_TWO:
            case 0x80000002: // D3DPRESENT_INTERVAL_TWO_OR_IMMEDIATE:
                multiplier = 2.0f;
                break;
            case D3DPRESENT_INTERVAL_THREE:
                multiplier = 3.0f;
                break;
            case D3DPRESENT_INTERVAL_FOUR:
                multiplier = 4.0f;
                break;
        }

        auto targetDuration = std::chrono::duration<double, std::milli>(((1000.0f / targetRefreshRate) * multiplier));
        auto targetTimestamp = frameStartTime + targetDuration;
        auto actualDuration = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - frameStartTime);
        auto startTimeAjustment = actualDuration - targetDuration;

        // Only enter the wait loop if the frame took too long
        if (actualDuration < targetDuration) {
            // If we need to wait for a larger amount of time (>= 1 frame at 60FPS), we can just sleep
            if (std::chrono::duration_cast<std::chrono::milliseconds>(targetTimestamp - std::chrono::high_resolution_clock::now()).count() > 16) {
                std::this_thread::sleep_until(targetTimestamp);
            } else {
                // Otherwise, we fall-through and just keep polling
                // This prevents large waits from hogging CPU power, but allows small waits/ to remain precice.
                while (std::chrono::high_resolution_clock::now() < targetTimestamp) {
                    ;
                }
            }
        }
    }

    frameStartTime = std::chrono::high_resolution_clock::now();

	UpdateFPSCounter();

	if (Flags == CXBX_SWAP_PRESENT_FORWARD) // Only do this when forwarded from Present
	{
		// Put primitives per frame in the title
		/*{
			char szString[64];

			sprintf( szString, "Cxbx: PPF(%d)", g_dwPrimPerFrame );

			SetWindowText( CxbxKrnl_hEmuParent, szString );

			g_dwPrimPerFrame = 0;
		}*/

		// TODO : Check if this should be done at Swap-not-Present-time too :
		// not really accurate because you definately dont always present on every vblank
		g_Xbox_VBlankData.Swap = g_Xbox_VBlankData.VBlank;

		if (g_Xbox_VBlankData.VBlank == g_VBLastSwap + 1)
			g_Xbox_VBlankData.Flags = 1; // D3DVBLANK_SWAPDONE
		else
		{
			g_Xbox_VBlankData.Flags = 2; // D3DVBLANK_SWAPMISSED
			g_Xbox_SwapData.MissedVBlanks++;
		}
	}

	// Handle Swap Callback function
	{
		g_Xbox_SwapData.Swap++;

		if(g_pXbox_SwapCallback != xbnullptr) 
		{
				
			g_pXbox_SwapCallback(&g_Xbox_SwapData);
				
		}
	}

	DWORD result;
	if (Flags == CXBX_SWAP_PRESENT_FORWARD) // Only do this when forwarded from Present
		result = D3D_OK; // Present always returns success
	else
		result = g_Xbox_SwapData.Swap; // Swap returns number of swaps

    return result;
}

bool IsSupportedFormat(XTL::X_D3DFORMAT X_Format, XTL::X_D3DRESOURCETYPE XboxResourceType, DWORD D3DUsage) {
	// TODO : Nuance the following, because the Direct3D 8 docs states
	// CheckDeviceFormat is needed when D3DUSAGE_RENDERTARGET or
	// D3DUSAGE_DYNAMNIC is specified.
	// Otherwise, lookup resource type and accompanying 'SupportedFormat' array
	bool *pbSupportedFormats = g_bSupportsFormatTexture;

	switch (XboxResourceType) {
		case XTL::X_D3DRTYPE_SURFACE: {
			if (D3DUsage & D3DUSAGE_RENDERTARGET) {
				pbSupportedFormats = g_bSupportsFormatSurfaceRenderTarget;
			} else if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
				pbSupportedFormats = g_bSupportsFormatSurfaceDepthStencil;
			} else {
				pbSupportedFormats = g_bSupportsFormatSurface;
			}
			break;
		}
		case XTL::X_D3DRTYPE_VOLUME: {
			pbSupportedFormats = g_bSupportsFormatTexture; // TODO : Complete
			break;
		}
		case XTL::X_D3DRTYPE_TEXTURE: {
			if (D3DUsage & D3DUSAGE_RENDERTARGET) {
				pbSupportedFormats = g_bSupportsFormatTextureRenderTarget;
			} else if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
				pbSupportedFormats = g_bSupportsFormatTextureDepthStencil;
			} else {
				pbSupportedFormats = g_bSupportsFormatTexture;
			}
			break;
		}
		case XTL::X_D3DRTYPE_VOLUMETEXTURE: {
			pbSupportedFormats = g_bSupportsFormatVolumeTexture; // TODO : Complete
			break;
		}
		case XTL::X_D3DRTYPE_CUBETEXTURE: {
			pbSupportedFormats = g_bSupportsFormatCubeTexture; // TODO : Complete
			break;
		}
	} // switch XboxResourceType

	return pbSupportedFormats[X_Format];
}

// Was patch: IDirect3DResource8_Register
void CreateHostResource(XTL::X_D3DResource *pResource, DWORD D3DUsage, int iTextureStage, DWORD dwSize)
{
	if (pResource == xbnullptr)
		return;

	// Determine the resource type name
	const char *ResourceTypeName;
	XTL::X_D3DRESOURCETYPE XboxResourceType = GetXboxD3DResourceType(pResource);

	switch (XboxResourceType) {
	case XTL::X_D3DRTYPE_NONE: ResourceTypeName = "None"; break;
	case XTL::X_D3DRTYPE_SURFACE: ResourceTypeName = "Surface"; break;
	case XTL::X_D3DRTYPE_VOLUME: ResourceTypeName = "Volume"; break;
	case XTL::X_D3DRTYPE_TEXTURE: ResourceTypeName = "Texture"; break;
	case XTL::X_D3DRTYPE_VOLUMETEXTURE: ResourceTypeName = "VolumeTexture"; break;
	case XTL::X_D3DRTYPE_CUBETEXTURE: ResourceTypeName = "CubeTexture"; break;
	case XTL::X_D3DRTYPE_VERTEXBUFFER: ResourceTypeName = "VertexBuffer"; break;
	case XTL::X_D3DRTYPE_INDEXBUFFER: ResourceTypeName = "IndexBuffer"; break;
	case XTL::X_D3DRTYPE_PUSHBUFFER: ResourceTypeName = "PushBuffer"; break;
	case XTL::X_D3DRTYPE_PALETTE: ResourceTypeName = "Palette"; break;
	case XTL::X_D3DRTYPE_FIXUP: ResourceTypeName = "Fixup"; break;
	default:
		EmuLog(LOG_LEVEL::WARNING, "CreateHostResource :-> Unrecognized Xbox Resource Type 0x%.08X", XboxResourceType);
		return;
	}

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pResource)
		LOG_FUNC_ARG(iTextureStage)
		LOG_FUNC_ARG(dwSize)
		LOG_FUNC_ARG(ResourceTypeName)
		LOG_FUNC_END;

	// Retrieve and test the xbox resource buffer address
	VAddr VirtualAddr = (VAddr)GetDataFromXboxResource(pResource);
	if ((VirtualAddr & ~PHYSICAL_MAP_BASE) == 0) {
		// TODO: Fix or handle this situation..?
		LOG_TEST_CASE("CreateHostResource : VirtualAddr == 0");
		// This is probably an unallocated resource, mapped into contiguous memory (0x80000000 OR 0xF0000000)
		EmuLog(LOG_LEVEL::WARNING, "CreateHostResource :-> %s carries no data - skipping conversion", ResourceTypeName);
		return;
	}

    switch (XboxResourceType) {
	case XTL::X_D3DRTYPE_NONE: {
		break;
	}

	case XTL::X_D3DRTYPE_SURFACE: {
		XTL::X_D3DSurface *pXboxSurface = (XTL::X_D3DSurface *)pResource;
		XTL::X_D3DBaseTexture *pParentXboxTexture = (pXboxSurface) ? (XTL::X_D3DBaseTexture*)pXboxSurface->Parent : xbnullptr;

		// Don't init the Parent if the Surface and Surface Parent formats differ
		// Happens in some Outrun 2006 SetRenderTarget calls
		if (pParentXboxTexture && (pXboxSurface->Format == pParentXboxTexture->Format)) {
            // For surfaces with a parent texture, map these to a host texture first
			// TODO : Investigate how it's possible (and how we could fix) the case when
			// the following call to GetHostBaseTexture would reject non-texture resources,
			// which would seem to trigger a "CreateCubeTexture Failed!" regression.
			IDirect3DBaseTexture *pParentHostBaseTexture = GetHostBaseTexture(pParentXboxTexture, D3DUsage, iTextureStage);
            IDirect3DSurface* pNewHostSurface;
			switch (pParentHostBaseTexture->GetType()) {
			case D3DRTYPE_VOLUMETEXTURE: {
                LOG_TEST_CASE("Using child surface of VolumeTexture");
				// TODO
				break;
			}
			case D3DRTYPE_CUBETEXTURE: {
                // test-case : Burnout
                auto pParentHostTexture = (IDirect3DCubeTexture*)pParentHostBaseTexture;

                D3DCUBEMAP_FACES CubeMapFace = D3DCUBEMAP_FACE_POSITIVE_X;
                UINT SurfaceLevel = 0;
                GetSurfaceFaceAndLevelWithinTexture(pXboxSurface, pParentXboxTexture, SurfaceLevel, CubeMapFace);

                HRESULT hRet = pParentHostTexture->GetCubeMapSurface(CubeMapFace, SurfaceLevel, &pNewHostSurface);

                DEBUG_D3DRESULT(hRet, "pHostParentTexture->GetCubeMapSurface");
                if (hRet == D3D_OK) {
                    SetHostSurface(pXboxSurface, pNewHostSurface, iTextureStage);
                    EmuLog(LOG_LEVEL::DEBUG, "CreateHostResource : Successfully created CubeTexture surface level (Face: %u, Level: %u, pResource: 0x%.08X, pNewHostSurface: 0x%.08X)",
                        CubeMapFace, SurfaceLevel, pResource, pNewHostSurface);
                    return;
                }

				break;
			}
			case D3DRTYPE_TEXTURE: {
                IDirect3DTexture* pParentHostTexture = (IDirect3DTexture*)pParentHostBaseTexture;

                UINT SurfaceLevel = 0;
                GetSurfaceFaceAndLevelWithinTexture(pXboxSurface, pParentXboxTexture, SurfaceLevel);
				HRESULT hRet = pParentHostTexture->GetSurfaceLevel(SurfaceLevel, &pNewHostSurface);

				DEBUG_D3DRESULT(hRet, "pHostParentTexture->GetSurfaceLevel");
				if (hRet == D3D_OK) {
					SetHostSurface(pXboxSurface, pNewHostSurface, iTextureStage);
                    EmuLog(LOG_LEVEL::DEBUG, "CreateHostResource : Successfully created Texture surface level (Level: %u, pResource: 0x%.08X, pNewHostSurface: 0x%.08X)",
						SurfaceLevel, pResource, pNewHostSurface);
					return;
				}
				break;
			}
			}

			EmuLog(LOG_LEVEL::WARNING, "Failed getting host surface level - falling through to regular surface creation");
		}
		// fall through
	}
	case XTL::X_D3DRTYPE_VOLUME: {
		// Note : Use and check for null, since X_D3DRTYPE_SURFACE might fall through here (by design) 
		XTL::X_D3DVolume *pXboxVolume = (XboxResourceType == XTL::X_D3DRTYPE_VOLUME) ? (XTL::X_D3DVolume *)pResource : xbnullptr;
		XTL::X_D3DVolumeTexture *pParentXboxVolumeTexture = (pXboxVolume) ? (XTL::X_D3DVolumeTexture *)pXboxVolume->Parent : xbnullptr;
		if (pParentXboxVolumeTexture) {
			// For volumes with a parent volume texture, map these to a host volume texture first
			IDirect3DVolumeTexture *pParentHostVolumeTexture = GetHostVolumeTexture(pParentXboxVolumeTexture, iTextureStage);
			UINT VolumeLevel = 0; // TODO : Derive actual level based on pXboxVolume->Data delta to pParentXboxVolumeTexture->Data
			IDirect3DVolume *pNewHostVolume;
			HRESULT hRet = pParentHostVolumeTexture->GetVolumeLevel(VolumeLevel, &pNewHostVolume);
			DEBUG_D3DRESULT(hRet, "pParentHostVolumeTexture->GetVolumeLevel");
			if (hRet == D3D_OK) {
				SetHostVolume(pXboxVolume, pNewHostVolume, iTextureStage);
				EmuLog(LOG_LEVEL::DEBUG, "CreateHostResource : Successfully created volume level (%u, 0x%.08X, 0x%.08X)",
					VolumeLevel, pResource, pNewHostVolume);
				return;
			}

			EmuLog(LOG_LEVEL::WARNING, "Failed getting host volume level - falling through to regular volume creation");
		}
		// fall through
	}
	case XTL::X_D3DRTYPE_TEXTURE:
	case XTL::X_D3DRTYPE_VOLUMETEXTURE:
	case XTL::X_D3DRTYPE_CUBETEXTURE: {
		XTL::X_D3DPixelContainer *pPixelContainer = (XTL::X_D3DPixelContainer*)pResource;
		XTL::X_D3DFORMAT X_Format = GetXboxPixelContainerFormat(pPixelContainer);
		D3DPOOL D3DPool = D3DPOOL_DEFAULT; // Was: D3DPOOL_MANAGED  TODO : Nuance D3DPOOL where/when needed

		if (EmuXBFormatIsDepthBuffer(X_Format)) {
			D3DUsage |= D3DUSAGE_DEPTHSTENCIL;
		}
		else if (pPixelContainer == g_pXbox_RenderTarget) {
			if (EmuXBFormatIsRenderTarget(X_Format))
				D3DUsage |= D3DUSAGE_RENDERTARGET;
			else
				EmuLog(LOG_LEVEL::WARNING, "Updating RenderTarget %s with an incompatible format!", ResourceTypeName);
		}

		// Determine the format we'll be using on host D3D
		D3DFORMAT PCFormat;
		bool bConvertToARGB = false;

		if (EmuXBFormatRequiresConversionToARGB(X_Format)) {
			bConvertToARGB = true;
			PCFormat = D3DFMT_A8R8G8B8;

			// Unset D3DUSAGE_DEPTHSTENCIL: It's not possible for ARGB textures to be depth stencils
			// Fixes CreateTexture error in Virtua Cop 3 (Chihiro)
			D3DUsage &= ~D3DUSAGE_DEPTHSTENCIL;
		}
		else {
			// Does host CheckDeviceFormat() succeed on this format?
			if (IsSupportedFormat(X_Format, XboxResourceType, D3DUsage)) {
				// Then use matching host format
				PCFormat = EmuXB2PC_D3DFormat(X_Format);

				// If, and ONLY if this is the default backbuffer, make sure the format matches the host backbuffer
				if (pResource == g_pXbox_BackBufferSurface) {
					PCFormat = g_EmuCDPD.HostPresentationParameters.BackBufferFormat;
				}
			}
			else {
				if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
					// If it was a depth stencil, fall back to a known supported depth format
					EmuLog(LOG_LEVEL::WARNING, "Xbox %s Format %x will be converted to D3DFMT_D24S8", ResourceTypeName, X_Format);
					PCFormat = D3DFMT_D24S8;
				} else if (EmuXBFormatCanBeConvertedToARGB(X_Format)) {
					EmuLog(LOG_LEVEL::WARNING, "Xbox %s Format %x will be converted to ARGB", ResourceTypeName, X_Format);
					bConvertToARGB = true;
					PCFormat = D3DFMT_A8R8G8B8;
				} else {
					// Otherwise, use a best matching format
					/*CxbxKrnlCleanup*/EmuLog(LOG_LEVEL::WARNING, "Encountered a completely incompatible %s format!", ResourceTypeName);
					PCFormat = EmuXB2PC_D3DFormat(X_Format);
				}
			}
		}

		// Update D3DPool accordingly to the active D3DUsage flags
		if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
			D3DPool = D3DPOOL_DEFAULT;
		}
		if (D3DUsage & D3DUSAGE_RENDERTARGET) {
			D3DPool = D3DPOOL_DEFAULT;
		}
		if (D3DUsage & D3DUSAGE_DYNAMIC) {
			D3DPool = D3DPOOL_DEFAULT;
		}

		// Interpret Width/Height/BPP
		bool bCubemap = pPixelContainer->Format & X_D3DFORMAT_CUBEMAP;
		bool bSwizzled = EmuXBFormatIsSwizzled(X_Format);
		bool bCompressed = EmuXBFormatIsCompressed(X_Format);
		DWORD dwMinSize = (bCompressed) ? 4 : 1;
		UINT dwBPP = EmuXBFormatBytesPerPixel(X_Format);
		UINT dwMipMapLevels = CxbxGetPixelContainerMipMapLevels(pPixelContainer);
		UINT dwWidth, dwHeight, dwDepth, dwRowPitch, dwSlicePitch;

		// Interpret Width/Height/BPP
		CxbxGetPixelContainerMeasures(pPixelContainer, 0, &dwWidth, &dwHeight, &dwDepth, &dwRowPitch, &dwSlicePitch);

		// Each mip-map level is 1/2 the size of the previous level
		// D3D9 forbids creation of a texture with more mip-map levels than it is divisible
		// EG: A 256x256 texture cannot have more than 8 levels, since that would create a texture smaller than 1x1
		// Because of this, we need to cap dwMipMapLevels when required
		if (dwMipMapLevels > 0) {
			// Calculate how many mip-map levels it takes to get to a texture of 1 pixels in either dimension
			UINT highestMipMapLevel = 1;
			UINT width = dwWidth; UINT height = dwHeight;
			while (width > 1 || height > 1) {
				width /= 2;
				height /= 2;
				highestMipMapLevel++;
			}

			// If the desired mip-map level was higher than the maximum possible, cap it
			// Test case: Shin Megami Tensei: Nine
			if (dwMipMapLevels > highestMipMapLevel) {
				LOG_TEST_CASE("Too many mip-map levels");
				dwMipMapLevels = highestMipMapLevel;
			}
		}

		if (dwDepth != 1) {
			LOG_TEST_CASE("CreateHostResource : Depth != 1");
		}

		// The following is necessary for DXT* textures (4x4 blocks minimum)
		// TODO: Figure out if this is necessary under other circumstances?
		if (bCompressed) {
			if (dwWidth < dwMinSize) {
				LOG_TEST_CASE("CreateHostResource : dwWidth < dwMinSize");
				EmuLog(LOG_LEVEL::WARNING, "Expanding %s width (%d->%d)", ResourceTypeName, dwWidth, dwMinSize);
				dwWidth = dwMinSize;
			}

			if (dwHeight < dwMinSize) {
				LOG_TEST_CASE("CreateHostResource : dwHeight < dwMinSize");
				EmuLog(LOG_LEVEL::WARNING, "Expanding %s height (%d->%d)", ResourceTypeName, dwHeight, dwMinSize);
				dwHeight = dwMinSize;
			}
		}

		// One of these will be created : each also has an intermediate copy to allow UpdateTexture to work
        // This means we don't need to lock the GPU resource anymore, so we can use D3DPOOL_DEFAULT to allow Stretch/CopyRects to work!
		IDirect3DSurface *pNewHostSurface = nullptr; // for X_D3DRTYPE_SURFACE
		IDirect3DVolume *pNewHostVolume = nullptr; // for X_D3DRTYPE_VOLUME
		IDirect3DTexture *pNewHostTexture = nullptr; // for X_D3DRTYPE_TEXTURE
        IDirect3DTexture *pIntermediateHostTexture = nullptr;
		IDirect3DVolumeTexture *pNewHostVolumeTexture = nullptr; // for X_D3DRTYPE_VOLUMETEXTURE
        IDirect3DVolumeTexture *pIntermediateHostVolumeTexture = nullptr;
		IDirect3DCubeTexture *pNewHostCubeTexture = nullptr; // for X_D3DRTYPE_CUBETEXTURE
        IDirect3DCubeTexture *pIntermediateHostCubeTexture = nullptr;

		HRESULT hRet;

		// Create the surface/volume/(volume/cube/)texture
		switch (XboxResourceType) {
		case XTL::X_D3DRTYPE_SURFACE: {
			if (D3DUsage & D3DUSAGE_RENDERTARGET) {
				hRet = g_pD3DDevice->CreateRenderTarget(dwWidth * g_RenderScaleFactor, dwHeight * g_RenderScaleFactor, PCFormat,
					g_EmuCDPD.HostPresentationParameters.MultiSampleType,
					0, // MultisampleQuality
					true, // Lockable
					&pNewHostSurface,
					nullptr // pSharedHandle
				);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateRenderTarget");
			} else
			if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
				hRet = g_pD3DDevice->CreateDepthStencilSurface(dwWidth * g_RenderScaleFactor, dwHeight * g_RenderScaleFactor, PCFormat,
					g_EmuCDPD.HostPresentationParameters.MultiSampleType,
					0, // MultisampleQuality
					false, // Discard
					&pNewHostSurface,
					nullptr
				);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateDepthStencilSurface");
			}
			else {
				D3DPool = D3DPOOL_SYSTEMMEM;
				hRet = g_pD3DDevice->CreateOffscreenPlainSurface(dwWidth, dwHeight, PCFormat, D3DPool, &pNewHostSurface, nullptr);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateOffscreenPlainSurface");
			}

			// First fail, retry with a fallback format
			// If this succeeds, the surface may not render correctly, but it won't crash
			if (hRet != D3D_OK) {
				if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
					EmuLog(LOG_LEVEL::WARNING, "CreateDepthStencilSurface Failed\n\nError: %s\nDesc: %s",
						DXGetErrorString(hRet), DXGetErrorDescription(hRet));
				}
				else {
					EmuLog(LOG_LEVEL::WARNING, "CreateImageSurface Failed\n\nError: %s\nDesc: %s",
						DXGetErrorString(hRet), DXGetErrorDescription(hRet));
				}

				EmuLog(LOG_LEVEL::WARNING, "Trying Fallback");
				hRet = g_pD3DDevice->CreateOffscreenPlainSurface(dwWidth, dwHeight, PCFormat, D3DPool, &pNewHostSurface, nullptr);
			}

			// If the fallback failed, show an error and exit execution.
			if (hRet != D3D_OK) {
				// We cannot safely continue in this state.
				CxbxKrnlCleanup("CreateImageSurface Failed!\n\nError: %s\nDesc: %s",
					DXGetErrorString(hRet), DXGetErrorDescription(hRet));
			}

			SetHostSurface(pResource, pNewHostSurface, iTextureStage);
			EmuLog(LOG_LEVEL::DEBUG, "CreateHostResource : Successfully created %s (0x%.08X, 0x%.08X)",
				ResourceTypeName, pResource, pNewHostSurface);
			EmuLog(LOG_LEVEL::DEBUG, "CreateHostResource : Width : %d, Height : %d, Format : %d",
				dwWidth, dwHeight, PCFormat);
			break;
		}

		case XTL::X_D3DRTYPE_VOLUME: {
			LOG_UNIMPLEMENTED();
			// Note : Host D3D can only(?) retrieve a volume like this :
			// hRet = pNewHostVolumeTexture->GetVolumeLevel(level, &pNewHostVolume);
			// So, we need to do this differently - we need to step up to the containing VolumeTexture,
			// and retrieve and convert all of it's GetVolumeLevel() slices.
			pNewHostVolume = nullptr;
			// SetHostVolume(pResource, pNewHostVolume, iTextureStage);
			// EmuLog(LOG_LEVEL::DEBUG, "CreateHostResource : Successfully created %s (0x%.08X, 0x%.08X)",
			//	ResourceTypeName, pResource, pNewHostVolume);
			break;
		}

		case XTL::X_D3DRTYPE_TEXTURE: {
			hRet = g_pD3DDevice->CreateTexture(dwWidth, dwHeight, dwMipMapLevels,
				D3DUsage, PCFormat, D3DPool, &pNewHostTexture,
				nullptr
			);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateTexture");

			// If the above failed, we might be able to use an ARGB texture instead
			if ((hRet != D3D_OK) && (PCFormat != D3DFMT_A8R8G8B8) && EmuXBFormatCanBeConvertedToARGB(X_Format)) {
				hRet = g_pD3DDevice->CreateTexture(dwWidth, dwHeight, dwMipMapLevels,
					D3DUsage, D3DFMT_A8R8G8B8, D3DPool, &pNewHostTexture,
					nullptr
				);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateTexture(D3DFMT_A8R8G8B8)");

				if (hRet == D3D_OK) {
					// Okay, now this works, make sure the texture gets converted
					bConvertToARGB = true;
					PCFormat = D3DFMT_A8R8G8B8;
				}
			}

            // Now create our intermediate texture for UpdateTexture, but not for render targets or depth stencils
            if (hRet == D3D_OK && (D3DUsage & D3DUSAGE_RENDERTARGET) == 0 && (D3DUsage & D3DUSAGE_DEPTHSTENCIL) == 0) {
                hRet = g_pD3DDevice->CreateTexture(dwWidth, dwHeight, dwMipMapLevels,
                    0, PCFormat, D3DPOOL_SYSTEMMEM, &pIntermediateHostTexture,
                    nullptr
                );
            }

			/*if(FAILED(hRet))
			{
				hRet = g_pD3DDevice->CreateTexture
				(
					dwWidth, dwHeight, dwMipMapLevels, D3DUsage, PCFormat,
					D3DPOOL_SYSTEMMEM, &pNewHostTexture,
					nullptr
					);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateTexture(D3DPOOL_SYSTEMMEM)");
			}*/


			if (hRet != D3D_OK) {
				CxbxKrnlCleanup("CreateTexture Failed!\n\n"
					"Error: 0x%X\nFormat: %d\nDimensions: %dx%d", hRet, PCFormat, dwWidth, dwHeight);
			}

			SetHostTexture(pResource, pNewHostTexture, iTextureStage);
			EmuLog(LOG_LEVEL::DEBUG, "CreateHostResource : Successfully created %s (0x%.08X, 0x%.08X)",
				ResourceTypeName, pResource, pNewHostTexture);
			break;
		}

		case XTL::X_D3DRTYPE_VOLUMETEXTURE: {
			hRet = g_pD3DDevice->CreateVolumeTexture(dwWidth, dwHeight, dwDepth,
				dwMipMapLevels, D3DUsage, PCFormat, D3DPool, &pNewHostVolumeTexture,
				nullptr
			);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateVolumeTexture");

            // Now create our intermediate texture for UpdateTexture, but not for render targets or depth stencils
            if (hRet == D3D_OK && (D3DUsage & D3DUSAGE_RENDERTARGET) == 0 && (D3DUsage & D3DUSAGE_DEPTHSTENCIL) == 0) {
                hRet = g_pD3DDevice->CreateVolumeTexture(dwWidth, dwHeight, dwDepth,
                    dwMipMapLevels, 0, PCFormat, D3DPOOL_SYSTEMMEM, &pIntermediateHostVolumeTexture,
                    nullptr
                );
            }

			if (hRet != D3D_OK) {
				CxbxKrnlCleanup("CreateVolumeTexture Failed!\n\nError: %s\nDesc: %s",
					DXGetErrorString(hRet), DXGetErrorDescription(hRet));
			}

			SetHostVolumeTexture(pResource, pNewHostVolumeTexture, iTextureStage);
			EmuLog(LOG_LEVEL::DEBUG, "CreateHostResource : Successfully created %s (0x%.08X, 0x%.08X)",
				ResourceTypeName, pResource, pNewHostVolumeTexture);
			break;
		}

		case XTL::X_D3DRTYPE_CUBETEXTURE: {
			EmuLog(LOG_LEVEL::DEBUG, "CreateCubeTexture(%d, %d, 0, %d, D3DPOOL_MANAGED)", dwWidth,
				dwMipMapLevels, PCFormat);

			hRet = g_pD3DDevice->CreateCubeTexture(dwWidth, dwMipMapLevels, D3DUsage,
				PCFormat, D3DPool, &pNewHostCubeTexture,
				nullptr
			);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateCubeTexture");

            // Now create our intermediate texture for UpdateTexture, but not for render targets or depth stencils
            if (hRet == D3D_OK && (D3DUsage & D3DUSAGE_RENDERTARGET) == 0 && (D3DUsage & D3DUSAGE_DEPTHSTENCIL) == 0) {
                hRet = g_pD3DDevice->CreateCubeTexture(dwWidth, dwMipMapLevels, 0,
                    PCFormat, D3DPOOL_SYSTEMMEM, &pIntermediateHostCubeTexture,
                    nullptr
                );
            }

			if (hRet != D3D_OK) {
				CxbxKrnlCleanup("CreateCubeTexture Failed!\n\nError: \nDesc: "/*,
					DXGetErrorString(hRet), DXGetErrorDescription(hRet)*/);
			}

			SetHostCubeTexture(pResource, pNewHostCubeTexture, iTextureStage);
			// TODO : Cube face surfaces can be used as a render-target,
			// so we need to associate host surfaces to each surface of this cube texture
            // However, we can't do it here: On Xbox, a new Surface is created on every call to
            // GetCubeMapSurface, so it needs to be done at surface conversion time by looking up
            // the parent CubeTexture
			EmuLog(LOG_LEVEL::DEBUG, "CreateHostResource : Successfully created %s (0x%.08X, 0x%.08X)",
				ResourceTypeName, pResource, pNewHostCubeTexture);
			break;
		}
		} // switch XboxResourceType

        // If this resource is a render target or depth stencil, don't attempt to lock/copy it as it won't work anyway
        // In this case, we simply return
        if (D3DUsage & D3DUSAGE_RENDERTARGET || D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
            return;
        }

		DWORD D3DLockFlags = D3DLOCK_NOSYSLOCK;

		DWORD dwCubeFaceOffset = 0;
		DWORD dwCubeFaceSize = 0;
		D3DCUBEMAP_FACES last_face = (bCubemap) ? D3DCUBEMAP_FACE_NEGATIVE_Z : D3DCUBEMAP_FACE_POSITIVE_X;
		for (int face = D3DCUBEMAP_FACE_POSITIVE_X; face <= last_face; face++) {
			// As we iterate through mipmap levels, we'll adjust the source resource offset
			DWORD dwMipOffset = 0;
			DWORD dwMipWidth = dwWidth;
			DWORD dwMipHeight = dwHeight;
			DWORD dwMipDepth = dwDepth;
            DWORD dwMipRowPitch = dwRowPitch;
			DWORD dwSrcSlicePitch = dwMipRowPitch * dwMipHeight; // TODO

			for (unsigned int mipmap_level = 0; mipmap_level < dwMipMapLevels; mipmap_level++) {

				// Calculate size of this mipmap level
				DWORD dwMipSize = dwMipRowPitch * dwMipHeight;
				if (bCompressed) {
					dwMipSize /= 4;
				}

				// Lock the host resource
				D3DLOCKED_RECT LockedRect = {};
				D3DLOCKED_BOX LockedBox = {};

				switch (XboxResourceType) {
				case XTL::X_D3DRTYPE_SURFACE:
					hRet = pNewHostSurface->LockRect(&LockedRect, nullptr, D3DLockFlags);
					break;
				case XTL::X_D3DRTYPE_VOLUME:
					hRet = pNewHostVolume->LockBox(&LockedBox, nullptr, D3DLockFlags);
					break;
				case XTL::X_D3DRTYPE_TEXTURE:
					hRet = pIntermediateHostTexture->LockRect(mipmap_level, &LockedRect, nullptr, D3DLockFlags);
					break;
				case XTL::X_D3DRTYPE_VOLUMETEXTURE:
					hRet = pIntermediateHostVolumeTexture->LockBox(mipmap_level, &LockedBox, nullptr, D3DLockFlags);
					break;
				case XTL::X_D3DRTYPE_CUBETEXTURE:
					hRet = pIntermediateHostCubeTexture->LockRect((D3DCUBEMAP_FACES)face, mipmap_level, &LockedRect, nullptr, D3DLockFlags);
					break;
				default:
					assert(false);
				} // switch XboxResourceType

				if (hRet != D3D_OK) {
					EmuLog(LOG_LEVEL::WARNING, "Locking host %s failed!", ResourceTypeName);
					continue; // This often happens on depth-stencil surfaces - let's ignore their copies for now
				}

				// Determine destination buffer attributes
				uint8_t *pDst;
				DWORD dwDstRowPitch;
				DWORD dwDstSlicePitch;

				switch (XboxResourceType) {
				case XTL::X_D3DRTYPE_VOLUME:
				case XTL::X_D3DRTYPE_VOLUMETEXTURE:
					pDst = (uint8_t *)LockedBox.pBits;
					dwDstRowPitch = LockedBox.RowPitch;
					dwDstSlicePitch = LockedBox.SlicePitch;
					break;
				default:
					pDst = (uint8_t *)LockedRect.pBits;
					dwDstRowPitch = LockedRect.Pitch;
					dwDstSlicePitch = 0;
				}

				uint8_t *pSrc = (uint8_t *)VirtualAddr + dwMipOffset;

				// Do we need to convert to ARGB?
				if (bConvertToARGB) {
					EmuLog(LOG_LEVEL::DEBUG, "Unsupported texture format, expanding to D3DFMT_A8R8G8B8");

					// Convert a row at a time, using a libyuv-like callback approach :
					if (!ConvertD3DTextureToARGBBuffer(
						X_Format,
						pSrc, dwMipWidth, dwMipHeight, dwMipRowPitch, dwSrcSlicePitch,
						pDst, dwDstRowPitch, dwDstSlicePitch,
						dwDepth,
						iTextureStage)) {
						CxbxKrnlCleanup("Unhandled conversion!");
					}
				}
				else if (bSwizzled) {
					// First we need to unswizzle the texture data
					EmuUnswizzleBox(
						pSrc, dwMipWidth, dwMipHeight, dwMipDepth,
						dwBPP, 
						pDst, dwDstRowPitch, dwDstSlicePitch
					);
				}
				else if (bCompressed) {
					memcpy(pDst, pSrc, dwMipSize);
				}
				else {
					/* TODO : // Let DirectX convert the surface (including palette formats) :
					if(!EmuXBFormatRequiresConversionToARGB) {
						D3DXLoadSurfaceFromMemory(
							GetHostSurface(pResource),
							nullptr, // no destination palette
							&destRect,
							pSrc, // Source buffer
							dwMipPitch, // Source pitch
							g_pXbox_Palette_Data,
							&SrcRect,
							D3DX_DEFAULT, // D3DX_FILTER_NONE,
							0 // No ColorKey?
							);
					} else {
					*/
					if ((dwDstRowPitch == dwMipRowPitch) && (dwMipRowPitch == dwMipWidth * dwBPP)) {
						memcpy(pDst, pSrc, dwMipSize);
					}
					else {
						for (DWORD v = 0; v < dwMipHeight; v++) {
							memcpy(pDst, pSrc, dwMipWidth * dwBPP);
							pDst += dwDstRowPitch;
							pSrc += dwMipRowPitch;
						}
					}
				}

				// Unlock the host resource
				switch (XboxResourceType) {
				case XTL::X_D3DRTYPE_SURFACE:
					hRet = pNewHostSurface->UnlockRect();
					break;
				case XTL::X_D3DRTYPE_VOLUME:
					hRet = pNewHostVolume->UnlockBox();
					break;
				case XTL::X_D3DRTYPE_TEXTURE:
					hRet = pIntermediateHostTexture->UnlockRect(mipmap_level);
					break;
				case XTL::X_D3DRTYPE_VOLUMETEXTURE:
					hRet = pIntermediateHostVolumeTexture->UnlockBox(mipmap_level);
					break;
				case XTL::X_D3DRTYPE_CUBETEXTURE:
					hRet = pIntermediateHostCubeTexture->UnlockRect((D3DCUBEMAP_FACES)face, mipmap_level);
					break;
				default:
					assert(false);
				}

				if (hRet != D3D_OK) {
					EmuLog(LOG_LEVEL::WARNING, "Unlocking host %s failed!", ResourceTypeName);
				}

				if (face == D3DCUBEMAP_FACE_POSITIVE_X) {
					dwCubeFaceSize += dwDepth * dwMipSize;
				}

				// Calculate the next mipmap level dimensions
				dwMipOffset += dwMipSize;
				if (dwMipWidth > dwMinSize) {
					dwMipWidth /= 2;
					dwMipRowPitch /= 2;
				}

				if (dwMipHeight > dwMinSize) {
					dwMipHeight /= 2;
				}

				if (dwMipDepth > 1) {
					dwMipDepth /= 2;
				}
			} // for mipmap levels

			if (face == D3DCUBEMAP_FACE_POSITIVE_X) {
				dwCubeFaceSize = ROUND_UP(dwCubeFaceSize, X_D3DTEXTURE_CUBEFACE_ALIGNMENT);
			}

			dwCubeFaceOffset += dwCubeFaceSize;
		} // for cube faces


        // Copy from the intermediate resource to the final host resource
        // This is necessary because CopyRects/StretchRects only works on resources in the DEFAULT pool
        // But resources in this pool are not lockable: We must use UpdateSurface/UpdateTexture instead!
        switch (XboxResourceType) {
        case XTL::X_D3DRTYPE_SURFACE:
        case XTL::X_D3DRTYPE_VOLUME:
            // We didn't use a copy for Surfaces or Volumes
            break;
        case XTL::X_D3DRTYPE_TEXTURE:
            g_pD3DDevice->UpdateTexture(pIntermediateHostTexture, pNewHostTexture);
            pIntermediateHostTexture->Release();
            break;
        case XTL::X_D3DRTYPE_VOLUMETEXTURE:
            g_pD3DDevice->UpdateTexture(pIntermediateHostVolumeTexture, pNewHostVolumeTexture);
            pIntermediateHostVolumeTexture->Release();
            break;
        case XTL::X_D3DRTYPE_CUBETEXTURE:
            g_pD3DDevice->UpdateTexture(pIntermediateHostCubeTexture, pNewHostCubeTexture);
            pIntermediateHostCubeTexture->Release();
            break;
        default:
            assert(false);
        }

        if (hRet != D3D_OK) {
            EmuLog(LOG_LEVEL::WARNING, "Updating host %s failed!", ResourceTypeName);
        }

		// Debug resource dumping
//#define _DEBUG_DUMP_TEXTURE_REGISTER "D:\\"
#ifdef _DEBUG_DUMP_TEXTURE_REGISTER
		bool bDumpConvertedTextures = true;  // TODO : Make this a runtime changeable setting
		if (bDumpConvertedTextures) {
			char szFilePath[MAX_PATH];

			switch (XboxResourceType) {
			case XTL::X_D3DRTYPE_SURFACE: {
				static int dwDumpSurface = 0;
				sprintf(szFilePath, _DEBUG_DUMP_TEXTURE_REGISTER "%.03d-Surface%.03d.dds", X_Format, dwDumpSurface++);
				D3DXSaveSurfaceToFileA(szFilePath, D3DXIFF_DDS, pNewHostSurface, nullptr, nullptr);
				break;
			}
			case XTL::X_D3DRTYPE_VOLUME: {
				// TODO
				break;
			}
			case XTL::X_D3DRTYPE_TEXTURE: {
				static int dwDumpTexure = 0;
				sprintf(szFilePath, _DEBUG_DUMP_TEXTURE_REGISTER "%.03d-Texture%.03d.dds", X_Format, dwDumpTexure++);
				D3DXSaveTextureToFileA(szFilePath, D3DXIFF_DDS, pNewHostTexture, nullptr);
				break;
			}
			case XTL::X_D3DRTYPE_VOLUMETEXTURE: {
				// TODO
				break;
			}
			case XTL::X_D3DRTYPE_CUBETEXTURE: {
				static int dwDumpCubeTexture = 0;
				for (unsigned int face = D3DCUBEMAP_FACE_POSITIVE_X; face <= D3DCUBEMAP_FACE_NEGATIVE_Z; face++) {
					IDirect3DSurface *pSurface;
					if (D3D_OK == pNewHostCubeTexture->GetCubeMapSurface((D3DCUBEMAP_FACES)face, 0, &pSurface)) {
						sprintf(szFilePath, _DEBUG_DUMP_TEXTURE_REGISTER "%.03d-CubeTexure%.03d-%d.dds", X_Format, dwDumpCubeTexture, face);
						D3DXSaveSurfaceToFileA(szFilePath, D3DXIFF_DDS, pSurface, nullptr, nullptr);
						pSurface->Release();
					}
				}
				dwDumpCubeTexture++;
				break;
			}
			} // switch XboxResourceType
		}
#endif

		break;
	}

	// case X_D3DRTYPE_VERTEXBUFFER: { break; } // TODO

	// case X_D3DRTYPE_INDEXBUFFER: { break; } // TODO

	case XTL::X_D3DRTYPE_PUSHBUFFER: {
		XTL::X_D3DPushBuffer *pPushBuffer = (XTL::X_D3DPushBuffer*)pResource;

		// create push buffer
		dwSize = g_VMManager.QuerySize(VirtualAddr);
		if (dwSize == 0) {
			// TODO: once this is known to be working, remove the warning
			EmuLog(LOG_LEVEL::WARNING, "Push buffer allocation size unknown");
			pPushBuffer->Lock = X_D3DRESOURCE_LOCK_FLAG_NOSIZE;
			break;
		}

		EmuLog(LOG_LEVEL::DEBUG, "CreateHostResource : Successfully created %S (0x%.08X, 0x%.08X, 0x%.08X)", ResourceTypeName, pResource->Data, pPushBuffer->Size, pPushBuffer->AllocationSize);
		break;
	}

	//case X_D3DRTYPE_PALETTE: { break;	}

	case XTL::X_D3DRTYPE_FIXUP: {
		XTL::X_D3DFixup *pFixup = (XTL::X_D3DFixup*)pResource;

		EmuLog(LOG_LEVEL::WARNING, "X_D3DRTYPE_FIXUP is not yet supported\n"
			"0x%.08X (pFixup->Common) \n"
			"0x%.08X (pFixup->Data)   \n"
			"0x%.08X (pFixup->Lock)   \n"
			"0x%.08X (pFixup->Run)    \n"
			"0x%.08X (pFixup->Next)   \n"
			"0x%.08X (pFixup->Size)   \n", pFixup->Common, pFixup->Data, pFixup->Lock, pFixup->Run, pFixup->Next, pFixup->Size);
		break;
	}
    } // switch XboxResourceType
}

// ******************************************************************
// * patch: D3DDevice_EnableOverlay
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_EnableOverlay)
(
    BOOL Enable
)
{
	LOG_FUNC_ONE_ARG(Enable);
	
	// The Xbox D3DDevice_EnableOverlay call merely resets the active
	// NV2A overlay state, it doesn't actually enable or disable anything.
	// Thus, we should just reset our overlay state here too. A title will
	// show new overlay data via D3DDevice_UpdateOverlay (see below).
	g_OverlayProxy = {};
}

// ******************************************************************
// * patch: D3DDevice_UpdateOverlay
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_UpdateOverlay)
(
	X_D3DSurface *pSurface,
	CONST RECT   *SrcRect,
	CONST RECT   *DstRect,
	BOOL          EnableColorKey,
	D3DCOLOR      ColorKey
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pSurface)
		LOG_FUNC_ARG(SrcRect)
		LOG_FUNC_ARG(DstRect)
		LOG_FUNC_ARG(EnableColorKey)
		LOG_FUNC_ARG(ColorKey)
		LOG_FUNC_END;

	// Reset and remember the overlay arguments, so our D3DDevice_Swap patch
	// can correctly show this overlay surface data.
	g_OverlayProxy = {};
	if (pSurface) {
		g_OverlayProxy.Surface = *pSurface;
		if (SrcRect)
			g_OverlayProxy.SrcRect = *SrcRect;

		if (DstRect)
			g_OverlayProxy.DstRect = *DstRect;

		g_OverlayProxy.EnableColorKey = EnableColorKey;
		g_OverlayProxy.ColorKey = ColorKey;
		// Update overlay if present was not called since the last call to
		// EmuD3DDevice_UpdateOverlay.
		if (g_OverlaySwap != g_Xbox_SwapData.Swap - 1) {
			EMUPATCH(D3DDevice_Swap)(CXBX_SWAP_PRESENT_FORWARD);
		}

		g_OverlaySwap = g_Xbox_SwapData.Swap;
	}
}

// ******************************************************************
// * patch: D3DDevice_GetOverlayUpdateStatus
// ******************************************************************
BOOL WINAPI XTL::EMUPATCH(D3DDevice_GetOverlayUpdateStatus)()
{
	LOG_FUNC();    

	LOG_UNIMPLEMENTED();

    // TODO: Actually check for update status
    return TRUE;
}

// ******************************************************************
// * patch: D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_BlockUntilVerticalBlank)()
{
	LOG_FUNC();

	std::unique_lock<std::mutex> lk(g_VBConditionMutex);
	g_VBConditionVariable.wait(lk);
}

// ******************************************************************
// * patch: D3DDevice_SetVerticalBlankCallback
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetVerticalBlankCallback)
(
    X_D3DVBLANKCALLBACK pCallback
)
{
	LOG_FUNC_ONE_ARG(pCallback);

    g_pXbox_VerticalBlankCallback = pCallback;    
}


// ******************************************************************
// * patch: D3DDevice_SetRenderState_Simple
// ******************************************************************
VOID __fastcall XTL::EMUPATCH(D3DDevice_SetRenderState_Simple)
(
    DWORD Method,
    DWORD Value
)
{
    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(Method)
        LOG_FUNC_ARG(Value)
        LOG_FUNC_END;

    XB_trampoline(VOID, __fastcall, D3DDevice_SetRenderState_Simple, (DWORD, DWORD));
    XB_D3DDevice_SetRenderState_Simple(Method, Value);

    // Fetch the RenderState conversion info for the given input
    int XboxRenderStateIndex = -1;
    for (int i = X_D3DRS_FIRST; i <= X_D3DRS_LAST; i++) {
        if (GetDxbxRenderStateInfo(i).M == PUSH_METHOD(Method)) {
            XboxRenderStateIndex = i;
            break;
        }
    }

    // If we could not map it, log and return
    if (XboxRenderStateIndex == -1) {
        EmuLog(LOG_LEVEL::WARNING, "RenderState_Simple(0x%.08X (%s), 0x%.08X) could not be found in RenderState table", Method, GetDxbxRenderStateInfo(XboxRenderStateIndex).S, Value);
        return;
    }

	EmuLog(LOG_LEVEL::DEBUG, "RenderState_Simple: %s = 0x%08X", GetDxbxRenderStateInfo(XboxRenderStateIndex).S, Value);

    XboxRenderStates.SetXboxRenderState(XboxRenderStateIndex, Value);
}

// LTCG specific D3DDevice_SetTransform function...
// This uses a custom calling convention where parameter is passed in EAX, EDX
VOID __stdcall XTL::EMUPATCH(D3DDevice_SetTransform_0)
(
)
{
	D3DTRANSFORMSTATETYPE param1;
	CONST D3DMATRIX *param2;

	__asm {
		mov param1, eax
		mov param2, edx
	}

	return EMUPATCH(D3DDevice_SetTransform)(param1, param2);
}

// ******************************************************************
// * patch: D3DDevice_SetTransform
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetTransform)
(
    D3DTRANSFORMSTATETYPE State,
    CONST D3DMATRIX      *pMatrix
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(State)
		LOG_FUNC_ARG(pMatrix)
		LOG_FUNC_END;

    /*
    printf("pMatrix (%d)\n", State);
    printf("{\n");
    printf("    %.08f,%.08f,%.08f,%.08f\n", pMatrix->_11, pMatrix->_12, pMatrix->_13, pMatrix->_14);
    printf("    %.08f,%.08f,%.08f,%.08f\n", pMatrix->_21, pMatrix->_22, pMatrix->_23, pMatrix->_24);
    printf("    %.08f,%.08f,%.08f,%.08f\n", pMatrix->_31, pMatrix->_32, pMatrix->_33, pMatrix->_34);
    printf("    %.08f,%.08f,%.08f,%.08f\n", pMatrix->_41, pMatrix->_42, pMatrix->_43, pMatrix->_44);
    printf("}\n");

    if(State == 6 && (pMatrix->_11 == 1.0f) && (pMatrix->_22 == 1.0f) && (pMatrix->_33 == 1.0f) && (pMatrix->_44 == 1.0f))
    {
        g_bSkipPush = TRUE;
        printf("SkipPush ON\n");
    }
    else
    {
        g_bSkipPush = FALSE;
        printf("SkipPush OFF\n");
    }
    */

    State = EmuXB2PC_D3DTS(State);

    HRESULT hRet = g_pD3DDevice->SetTransform(State, pMatrix);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetTransform");    
}

// ******************************************************************
// * patch: D3DDevice_MultiplyTransform
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_MultiplyTransform)
(
	D3DTRANSFORMSTATETYPE State,
	CONST D3DMATRIX      *pMatrix
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(State)
		LOG_FUNC_ARG(pMatrix)
		LOG_FUNC_END;

	State = EmuXB2PC_D3DTS(State);

	HRESULT hRet = g_pD3DDevice->MultiplyTransform(State, pMatrix);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->MultiplyTransform");
}


// ******************************************************************
// * patch: D3DDevice_GetTransform
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetTransform)
(
    D3DTRANSFORMSTATETYPE State,
    D3DMATRIX            *pMatrix
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(State)
		LOG_FUNC_ARG(pMatrix)
		LOG_FUNC_END;

    State = EmuXB2PC_D3DTS(State);

    HRESULT hRet = g_pD3DDevice->GetTransform(State, pMatrix);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetTransform");    
}

// ******************************************************************
// * patch: Lock2DSurface
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(Lock2DSurface)
(
	X_D3DPixelContainer *pPixelContainer,
	D3DCUBEMAP_FACES     FaceType,
	UINT                 Level,
	D3DLOCKED_RECT      *pLockedRect,
	RECT                *pRect,
	DWORD                Flags
	)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pPixelContainer)
		LOG_FUNC_ARG(FaceType)
		LOG_FUNC_ARG(Level)
		LOG_FUNC_ARG(pLockedRect)
		LOG_FUNC_ARG(pRect)
		LOG_FUNC_ARG(Flags)
		LOG_FUNC_END;


	// Pass through to the Xbox implementation of this function
	XB_trampoline(VOID, WINAPI, Lock2DSurface, (X_D3DPixelContainer*, D3DCUBEMAP_FACES, UINT, D3DLOCKED_RECT*, RECT*, DWORD));

	XB_Lock2DSurface(pPixelContainer, FaceType, Level, pLockedRect, pRect, Flags);

	// Mark the resource as modified
	ForceResourceRehash(pPixelContainer);
}


// ******************************************************************
// * patch: Lock3DSurface
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(Lock3DSurface)
(
	X_D3DPixelContainer *pPixelContainer,
	UINT				Level,
	D3DLOCKED_BOX		*pLockedVolume,
	D3DBOX				*pBox,
	DWORD				Flags
	)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pPixelContainer)
		LOG_FUNC_ARG(Level)
		LOG_FUNC_ARG(pLockedVolume)
		LOG_FUNC_ARG(pBox)
		LOG_FUNC_ARG(Flags)
		LOG_FUNC_END;

	// Pass through to the Xbox implementation of this function
	XB_trampoline(VOID, WINAPI, Lock3DSurface, (X_D3DPixelContainer*, UINT, D3DLOCKED_BOX*, D3DBOX*, DWORD));

	XB_Lock3DSurface(pPixelContainer, Level, pLockedVolume, pBox, Flags);

	// Mark the resource as modified
	ForceResourceRehash(pPixelContainer);
}


// LTCG specific D3DDevice_SetStreamSource function...
// This uses a custom calling convention where parameter is passed in EBX, EAX
// TODO: XB_trampoline plus Log function is not working due lost parameter in EAX.
// Test-case: Ninja Gaiden
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetStreamSource_4)
(
    UINT                Stride
)
{
    UINT                StreamNumber;
    X_D3DVertexBuffer  *pStreamData;

	__asm {
		mov pStreamData, ebx
		mov StreamNumber, eax
	}

	//LOG_FUNC_BEGIN
	//	LOG_FUNC_ARG(StreamNumber)
	//	LOG_FUNC_ARG(pStreamData)
	//	LOG_FUNC_ARG(Stride)
	//	LOG_FUNC_END;
	EmuLog(LOG_LEVEL::DEBUG, "D3DDevice_SetStreamSource_4(StreamNumber : %08X pStreamData : %08X Stride : %08X);", StreamNumber, pStreamData, Stride);

	CxbxImpl_SetStreamSource(StreamNumber, pStreamData, Stride);

	// TODO : Forward to Xbox implementation
	// This should stop us having to patch GetStreamSource!
	//XB_trampoline(VOID, WINAPI, D3DDevice_SetStreamSource_4, (UINT, X_D3DVertexBuffer*, UINT));
	//XB_D3DDevice_SetStreamSource_4(StreamNumber, pStreamData, Stride);
}

// This uses a custom calling convention where parameter is passed in EAX
// TODO: XB_trampoline plus Log function is not working due lost parameter in EAX.
// Test-case: Superman - The Man Of Steel
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetStreamSource_8)
(
    X_D3DVertexBuffer  *pStreamData,
    UINT                Stride
)
{
    UINT                StreamNumber;

	__asm {
		mov StreamNumber, eax
	}

	//LOG_FUNC_BEGIN
	//	LOG_FUNC_ARG(StreamNumber)
	//	LOG_FUNC_ARG(pStreamData)
	//	LOG_FUNC_ARG(Stride)
	//	LOG_FUNC_END;
	EmuLog(LOG_LEVEL::DEBUG, "D3DDevice_SetStreamSource_8(StreamNumber : %08X pStreamData : %08X Stride : %08X);", StreamNumber, pStreamData, Stride);

	CxbxImpl_SetStreamSource(StreamNumber, pStreamData, Stride);

	// TODO : Forward to Xbox implementation
	// This should stop us having to patch GetStreamSource!
	//XB_trampoline(VOID, WINAPI, D3DDevice_SetStreamSource_8, (X_D3DVertexBuffer*, UINT));
	//XB_D3DDevice_SetStreamSource_8(pStreamData, Stride);
}

// ******************************************************************
// * patch: D3DDevice_SetStreamSource
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetStreamSource)
(
    UINT                StreamNumber,
    X_D3DVertexBuffer  *pStreamData,
    UINT                Stride
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(StreamNumber)
		LOG_FUNC_ARG(pStreamData)
		LOG_FUNC_ARG(Stride)
		LOG_FUNC_END;

	CxbxImpl_SetStreamSource(StreamNumber, pStreamData, Stride);

	// Forward to Xbox implementation
	// This should stop us having to patch GetStreamSource!
	XB_trampoline(VOID, WINAPI, D3DDevice_SetStreamSource, (UINT, X_D3DVertexBuffer*, UINT));
	XB_D3DDevice_SetStreamSource(StreamNumber, pStreamData, Stride);
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShader
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetVertexShader)
(
    DWORD Handle
)
{
	LOG_FUNC_ONE_ARG(Handle);

	// Checks if the Handle has bit 0 set - if not, it's a FVF
	// which is converted to a global Xbox Vertex Shader struct
	// Otherwise bit 0 is cleared and the resulting address is
	// validated to be a valid Xbox Vertex Shader
	// D3D state fields are updated.
	// If the shader contains a program, the handle is passed to
	// D3DDevice_LoadVertexShader and D3DDevice_SelectVertexShader.
	// Otherwise the shader is send using push buffer commands.

    HRESULT hRet = D3D_OK;

    g_Xbox_VertexShader_Handle = Handle;

	if (VshHandleIsVertexShader(Handle)) {
 		CxbxVertexShader *pCxbxVertexShader = GetCxbxVertexShader(Handle);
		hRet = g_pD3DDevice->SetVertexDeclaration(pCxbxVertexShader->pHostVertexDeclaration);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexDeclaration");
		if (pCxbxVertexShader->HostFVF)
		{
			hRet = g_pD3DDevice->SetFVF(pCxbxVertexShader->HostFVF);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetFVF(VshHandleIsVertexShader)");
		}
		else
		{
			SetOverrideFlags(pCxbxVertexShader);

			hRet = g_pD3DDevice->SetVertexShader(pCxbxVertexShader->pHostVertexShader);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexShader(VshHandleIsVertexShader)");
		}

	} else {
		hRet = g_pD3DDevice->SetVertexShader(nullptr);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexShader");
		hRet = g_pD3DDevice->SetFVF(Handle);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetFVF");
	}

	UpdateViewPortOffsetAndScaleConstants();
}

// TODO : Move to own file
constexpr unsigned int IndicesPerPage = PAGE_SIZE / sizeof(INDEX16);
constexpr unsigned int InputQuadsPerPage = ((IndicesPerPage * VERTICES_PER_QUAD) / VERTICES_PER_TRIANGLE) / TRIANGLES_PER_QUAD;

// TODO : Move to own file
// Called by CxbxDrawPrimitiveUP (indirectly by D3DDevice_DrawVerticesUP,
// EmuExecutePushBufferRaw and EmuFlushIVB) when PrimitiveType == X_D3DPT_QUADLIST.
// Emulated by calling g_pD3DDevice->DrawIndexedPrimitiveUP with index data that maps
// quads to triangles. This function creates the index buffer that is needed for this;
// For every quad that must be drawn, we generate indices for two triangles.
// Note, that the resulting index data can be re-used for later comparable draw calls
// and only needs to grow when  current length doesn't suffices for a larger draw.
INDEX16 *CxbxAssureQuadListIndexData(UINT NrOfQuadIndices)
{
	if (g_QuadToTriangleIndexData_Size < NrOfQuadIndices)
	{
		g_QuadToTriangleIndexData_Size = RoundUp(NrOfQuadIndices, InputQuadsPerPage);
		UINT NrOfTriangleIndices = QuadToTriangleVertexCount(g_QuadToTriangleIndexData_Size);
		if (g_pQuadToTriangleIndexData != nullptr) {
			free(g_pQuadToTriangleIndexData);
		}

		g_pQuadToTriangleIndexData = (INDEX16 *)malloc(NrOfTriangleIndices * sizeof(INDEX16));
		CxbxConvertQuadListToTriangleListIndices(nullptr, NrOfTriangleIndices, g_pQuadToTriangleIndexData);
	}

	return g_pQuadToTriangleIndexData;
}

// TODO : Move to own file
// Makes a D3D IndexBuffer active that contains quadlist-to-trianglelist indices.
// Uses CxbxAssureQuadListIndexData to populate the index buffer with.
// Note, that the resulting index buffer can be re-used for later comparable draw calls
// and only needs to grow when current length doesn't sufficesw for a larger draw.
void CxbxAssureQuadListD3DIndexBuffer(UINT NrOfQuadIndices)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

	HRESULT hRet;

	if (g_QuadToTriangleHostIndexBuffer_Size < NrOfQuadIndices)
	{
		// Round the number of indices up so we'll allocate whole pages
		g_QuadToTriangleHostIndexBuffer_Size = RoundUp(NrOfQuadIndices, InputQuadsPerPage);
		UINT NrOfTriangleIndices = QuadToTriangleVertexCount(g_QuadToTriangleHostIndexBuffer_Size); // 4 > 6

		// Create a new native index buffer of the above determined size :
		if (g_pQuadToTriangleHostIndexBuffer != nullptr) {
			g_pQuadToTriangleHostIndexBuffer->Release(); // test-case : XDK PointSprites
			g_pQuadToTriangleHostIndexBuffer = nullptr;
		}

		// Create a new native index buffer of the above determined size :
		g_pQuadToTriangleHostIndexBuffer = CxbxCreateIndexBuffer(NrOfTriangleIndices);
		if (g_pQuadToTriangleHostIndexBuffer == nullptr)
			CxbxKrnlCleanup("CxbxAssureQuadListD3DIndexBuffer : IndexBuffer Create Failed!");

		// Put quadlist-to-triangle-list index mappings into this buffer :
		INDEX16* pHostIndexBufferData = nullptr;
		hRet = g_pQuadToTriangleHostIndexBuffer->Lock(0, /*entire SizeToLock=*/0, (D3DLockData **)&pHostIndexBufferData, D3DLOCK_DISCARD);
		DEBUG_D3DRESULT(hRet, "g_pQuadToTriangleHostIndexBuffer->Lock");
		if (pHostIndexBufferData == nullptr)
			CxbxKrnlCleanup("CxbxAssureQuadListD3DIndexBuffer : Could not lock index buffer!");

		memcpy(pHostIndexBufferData, CxbxAssureQuadListIndexData(NrOfQuadIndices), NrOfTriangleIndices * sizeof(INDEX16));

		g_pQuadToTriangleHostIndexBuffer->Unlock();
	}

	// Activate the new native index buffer :
	hRet = g_pD3DDevice->SetIndices(g_pQuadToTriangleHostIndexBuffer);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetIndices");

	if (FAILED(hRet))
		CxbxKrnlCleanup("CxbxAssureQuadListD3DIndexBuffer : SetIndices Failed!"); // +DxbxD3DErrorString(hRet));
}

// TODO : Move to own file
// Calls SetIndices with a separate index-buffer, that's populated with the supplied indices.
void CxbxDrawIndexedClosingLine(INDEX16 LowIndex, INDEX16 HighIndex)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

	HRESULT hRet;

	if (g_pClosingLineLoopHostIndexBuffer == nullptr) {
		g_pClosingLineLoopHostIndexBuffer = CxbxCreateIndexBuffer(VERTICES_PER_LINE);
		if (g_pClosingLineLoopHostIndexBuffer == nullptr)
			CxbxKrnlCleanup("Unable to create g_pClosingLineLoopHostIndexBuffer for D3DPT_LINELOOP emulation");
	}

	INDEX16 *pCxbxClosingLineLoopIndexBufferData = nullptr;
	hRet = g_pClosingLineLoopHostIndexBuffer->Lock(0, /*entire SizeToLock=*/0, (D3DLockData **)&pCxbxClosingLineLoopIndexBufferData, D3DLOCK_DISCARD);
	DEBUG_D3DRESULT(hRet, "g_pClosingLineLoopHostIndexBuffer->Lock");

	// Set the indices for the two VERTICES_PER_LINE :
	pCxbxClosingLineLoopIndexBufferData[0] = LowIndex;
	pCxbxClosingLineLoopIndexBufferData[1] = HighIndex;

	hRet = g_pClosingLineLoopHostIndexBuffer->Unlock();
	DEBUG_D3DRESULT(hRet, "g_pClosingLineLoopHostIndexBuffer->Unlock");

	hRet = g_pD3DDevice->SetIndices(g_pClosingLineLoopHostIndexBuffer);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetIndices");

	hRet = g_pD3DDevice->DrawIndexedPrimitive(
		/*PrimitiveType=*/D3DPT_LINELIST,
		/*BaseVertexIndex=*/0, // Note : Callers must apply BaseVertexIndex to the LowIndex and HighIndex argument values
		/*MinVertexIndex=*/LowIndex,
		/*NumVertices=*/VERTICES_PER_LINE,
		/*startIndex=*/0,
		/*primCount=*/1
	);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitive(CxbxDrawIndexedClosingLine)");

	g_dwPrimPerFrame++;
}

// TODO : Move to own file
void CxbxDrawIndexedClosingLineUP(INDEX16 LowIndex, INDEX16 HighIndex, void *pHostVertexStreamZeroData, UINT uiHostVertexStreamZeroStride)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

#if 0
	// Since we can use pHostVertexStreamZeroData here, we can close the line simpler than
	// via CxbxDrawIndexedClosingLine, by drawing two indices via DrawIndexedPrimitiveUP.
	// (This is simpler because we use just indices and don't need to copy the vertices.)
	INDEX16 CxbxClosingLineIndices[2] = { LowIndex, HighIndex };

	HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitiveUP(
		/*PrimitiveType=*/D3DPT_LINELIST,
		/*MinVertexIndex=*/LowIndex,
		/*NumVertices=*/(HighIndex - LowIndex) + 1,
		/*PrimitiveCount=*/1,
		/*pIndexData=*/CxbxClosingLineIndices,
		/*IndexDataFormat=*/D3DFMT_INDEX16,
		pHostVertexStreamZeroData,
		uiHostVertexStreamZeroStride
	);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitiveUP(CxbxDrawIndexedClosingLineUP)");
#else // TODO : If NumVertices is high, performance might suffer - drawing a copy of the two vertices could be faster
	// Since we can use pHostVertexStreamZeroData here, we can close the line simpler than
	// via CxbxDrawIndexedClosingLine, by drawing two vertices via DrawPrimitiveUP.
	// (This is simpler because we just copy the vertices, and don't need a separate index buffer.)
	uint8_t VertexData[512]; assert(512 >= 2 * uiHostVertexStreamZeroStride);
	uint8_t *FirstVertex = (uint8_t *)pHostVertexStreamZeroData + (LowIndex * uiHostVertexStreamZeroStride);
	uint8_t *SecondVertex = (uint8_t *)pHostVertexStreamZeroData + (HighIndex * uiHostVertexStreamZeroStride);

	memcpy(VertexData, FirstVertex, uiHostVertexStreamZeroStride);
	memcpy(VertexData + uiHostVertexStreamZeroStride, SecondVertex, uiHostVertexStreamZeroStride);

	HRESULT hRet = g_pD3DDevice->DrawPrimitiveUP(
		/*PrimitiveType=*/D3DPT_LINELIST,
		/*PrimitiveCount=*/1,
		/*pVertexStreamZeroData=*/VertexData,
		uiHostVertexStreamZeroStride
	);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawPrimitiveUP(CxbxDrawIndexedClosingLineUP)");
#endif

	g_dwPrimPerFrame++;
}

// Requires assigned pXboxIndexData
// Called by D3DDevice_DrawIndexedVertices and EmuExecutePushBufferRaw (twice)
void CxbxDrawIndexed(CxbxDrawContext &DrawContext)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

	assert(DrawContext.dwStartVertex == 0);
	assert(DrawContext.pXboxIndexData != nullptr);
	assert(DrawContext.dwVertexCount > 0); // TODO : If this fails, make responsible callers do an early-exit
	assert(IsValidCurrentShader());

	bool bConvertQuadListToTriangleList = (DrawContext.XboxPrimitiveType == XTL::X_D3DPT_QUADLIST);
	ConvertedIndexBuffer& CacheEntry = CxbxUpdateActiveIndexBuffer(DrawContext.pXboxIndexData, DrawContext.dwVertexCount, bConvertQuadListToTriangleList);
	// Note : CxbxUpdateActiveIndexBuffer calls SetIndices

	// Set LowIndex and HighIndex *before* VerticesInBuffer gets derived
	DrawContext.LowIndex = CacheEntry.LowIndex;
	DrawContext.HighIndex = CacheEntry.HighIndex;

	VertexBufferConverter.Apply(&DrawContext); // Sets dwHostPrimitiveCount

	INT BaseVertexIndex = DrawContext.dwBaseVertexIndex;
	UINT primCount = DrawContext.dwHostPrimitiveCount;
	if (bConvertQuadListToTriangleList) {
		if (DrawContext.dwVertexCount == 4)
			LOG_TEST_CASE("X_D3DPT_QUADLIST (single quad)"); // breakpoint location
		else
			LOG_TEST_CASE("X_D3DPT_QUADLIST");

		if (BaseVertexIndex > 0)
			LOG_TEST_CASE("X_D3DPT_QUADLIST (BaseVertexIndex > 0)");

		// Convert draw arguments from quads to triangles :
		// Note : BaseVertexIndex must NOT be mapped through QuadToTriangleVertexCount(BaseVertexIndex)!
		primCount *= TRIANGLES_PER_QUAD;
	}

	// See https://docs.microsoft.com/en-us/windows/win32/direct3d9/rendering-from-vertex-and-index-buffers
	// for an explanation on the function of the BaseVertexIndex, MinVertexIndex, NumVertices and StartIndex arguments.
	HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitive(
		/* PrimitiveType = */EmuXB2PC_D3DPrimitiveType(DrawContext.XboxPrimitiveType),
		BaseVertexIndex,
		/* MinVertexIndex = */CacheEntry.LowIndex,
		/* NumVertices = */(CacheEntry.HighIndex - CacheEntry.LowIndex) + 1,
		/* startIndex = DrawContext.dwStartVertex = */0,
		primCount);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitive");

	g_dwPrimPerFrame += primCount;
	if (DrawContext.XboxPrimitiveType == XTL::X_D3DPT_LINELOOP) {
		// Close line-loops using a final single line, drawn from the end to the start vertex
		if (BaseVertexIndex == 0) {
			LOG_TEST_CASE("X_D3DPT_LINELOOP");
		} else {
			LOG_TEST_CASE("X_D3DPT_LINELOOP (BaseVertexIndex > 0)");
			// Note : This is to find test-cases for the BaseVertexIndex addition below:
		}
		// Read the end and start index from the supplied index data
		INDEX16 LowIndex = BaseVertexIndex + DrawContext.pXboxIndexData[0];
		INDEX16 HighIndex = BaseVertexIndex + DrawContext.pXboxIndexData[DrawContext.dwHostPrimitiveCount];
		// If needed, swap so highest index is higher than lowest (duh)
		if (HighIndex < LowIndex) {
			std::swap(HighIndex, LowIndex);
		}

		// Draw the closing line using a helper function (which will SetIndices)
		CxbxDrawIndexedClosingLine(LowIndex, HighIndex);
		// NOTE : We don't restore the previously active index buffer
	}
}

// TODO : Move to own file
// Drawing function specifically for rendering Xbox draw calls supplying a 'User Pointer'.
// Called by D3DDevice_DrawVerticesUP, EmuExecutePushBufferRaw and EmuFlushIVB
void CxbxDrawPrimitiveUP(CxbxDrawContext &DrawContext)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

	assert(DrawContext.dwStartVertex == 0);
	assert(DrawContext.pXboxVertexStreamZeroData != xbnullptr);
	assert(DrawContext.uiXboxVertexStreamZeroStride > 0);
	assert(DrawContext.dwBaseVertexIndex == 0); // No IndexBase under Draw*UP

	VertexBufferConverter.Apply(&DrawContext);
	if (DrawContext.XboxPrimitiveType == XTL::X_D3DPT_QUADLIST) {
		// LOG_TEST_CASE("X_D3DPT_QUADLIST"); // test-case : X-Marbles and XDK Sample PlayField
		// Draw quadlists using a single 'quad-to-triangle mapping' index buffer :
		INDEX16 *pIndexData = CxbxAssureQuadListIndexData(DrawContext.dwVertexCount);
		// Convert quad vertex-count to triangle vertex count :
		UINT PrimitiveCount = DrawContext.dwHostPrimitiveCount * TRIANGLES_PER_QUAD;

		// Instead of calling WalkIndexBuffer on pQuadToTriangleIndexBuffer,
		// we can derive the LowIndex and HighIndexes ourselves here
		INDEX16 LowIndex = 0;
		INDEX16 HighIndex = (INDEX16)(DrawContext.dwVertexCount - 1);

		// Draw indexed triangles instead of quads
		HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitiveUP(
			/*PrimitiveType=*/D3DPT_TRIANGLELIST,
			/*MinVertexIndex=*/LowIndex,
			/*NumVertexIndices=*/(HighIndex - LowIndex) + 1,
			PrimitiveCount,
			pIndexData,
			/*IndexDataFormat=*/D3DFMT_INDEX16,
			DrawContext.pHostVertexStreamZeroData,
			DrawContext.uiHostVertexStreamZeroStride
		);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitieUP(X_D3DPT_QUADLIST)");

		g_dwPrimPerFrame += PrimitiveCount;
	}
	else {
		// Primitives other than X_D3DPT_QUADLIST can be drawn using one DrawPrimitiveUP call :
		HRESULT hRet = g_pD3DDevice->DrawPrimitiveUP(
			EmuXB2PC_D3DPrimitiveType(DrawContext.XboxPrimitiveType),
			DrawContext.dwHostPrimitiveCount,
			DrawContext.pHostVertexStreamZeroData,
			DrawContext.uiHostVertexStreamZeroStride
		);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawPrimitiveUP");

		g_dwPrimPerFrame += DrawContext.dwHostPrimitiveCount;
		if (DrawContext.XboxPrimitiveType == XTL::X_D3DPT_LINELOOP) {
			// test-case : XDK samples reaching this case : DebugKeyboard, Gamepad, Tiling, ShadowBuffer
			// Close line-loops using a final single line, drawn from the end to the start vertex :
			CxbxDrawIndexedClosingLineUP(
				(INDEX16)0, // LowIndex
				(INDEX16)DrawContext.dwHostPrimitiveCount, // HighIndex,
				DrawContext.pHostVertexStreamZeroData,
				DrawContext.uiHostVertexStreamZeroStride
			);
		}
	}
}

IDirect3DBaseTexture* CxbxConvertXboxSurfaceToHostTexture(XTL::X_D3DBaseTexture* pBaseTexture)
{
	LOG_INIT;

	IDirect3DTexture* pNewHostTexture = nullptr;
#if 0 // TODO : Complete, debug and activate (and then cleanup GetHostBaseTexture)
	D3DFORMAT PCFormat = D3DFMT_A8B8G8R8; // TODO : Derive from pBaseTexture

	IDirect3DSurface* pHostSurface = GetHostSurface(pBaseTexture); // TODO : Extend this with a texture channel number too, if surfaces send to SetTexture can be paletized format?

	DWORD dwWidth = GetPixelContainerWidth(pBaseTexture);
	DWORD dwHeight = GetPixelContainerHeight(pBaseTexture);
	UINT dwMipMapLevels = CxbxGetPixelContainerMipMapLevels(pBaseTexture);

	HRESULT hRet = g_pD3DDevice->CreateTexture(dwWidth, dwHeight, dwMipMapLevels,
		/*Usage=*/0, PCFormat, D3DPOOL_SYSTEMMEM, &pNewHostTexture, nullptr);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateTexture (in CxbxConvertXboxSurfaceToHostTexture)");
	if (hRet != D3D_OK) {
		CxbxKrnlCleanup("CreateTexture Failed!\n\nError: \nDesc: "/*,
			DXGetErrorString(hRet), DXGetErrorDescription(hRet)*/);
	}

	IDirect3DSurface* pHostTextureSurface = nullptr;
	hRet = pNewHostTexture->GetSurfaceLevel(/*Level=*/0, &pHostTextureSurface);
	DEBUG_D3DRESULT(hRet, "pHostBaseTexture->GetSurfaceLevel");

	if (hRet == D3D_OK) {
		hRet = D3DXLoadSurfaceFromSurface(pHostTextureSurface, nullptr, nullptr, pHostSurface, nullptr, nullptr, D3DX_FILTER_NONE, 0x00000000);
		DEBUG_D3DRESULT(hRet, "D3DXLoadSurfaceFromSurface");
		pHostTextureSurface->Release();
	}
#endif
	return (IDirect3DBaseTexture*)pNewHostTexture; // return it as a base texture
}

void EmuUpdateActiveTextureStages()
{
	LOG_INIT;

	for (int i = 0; i < XTL::X_D3DTS_STAGECOUNT; i++)
	{
		XTL::X_D3DBaseTexture *pBaseTexture = g_pXbox_SetTexture[i];
		IDirect3DBaseTexture *pHostBaseTexture = nullptr;
		bool bNeedRelease = false;

		if (pBaseTexture != xbnullptr) {
			DWORD Type = GetXboxCommonResourceType(pBaseTexture);
			switch (Type) {
			case X_D3DCOMMON_TYPE_TEXTURE:
				pHostBaseTexture = GetHostBaseTexture(pBaseTexture, /*D3DUsage=*/0, i);
				break;
			case X_D3DCOMMON_TYPE_SURFACE:
				// Surfaces can be set in the texture stages, instead of textures
				LOG_TEST_CASE("ActiveTexture set to a surface (non-texture) resource"); // Test cases : Burnout, Outrun 2006
				// We must wrap the surface before using it as a texture
				pHostBaseTexture = CxbxConvertXboxSurfaceToHostTexture(pBaseTexture);
				// Release this texture (after SetTexture) when we succeeded in creating it :
				bNeedRelease = pHostBaseTexture != nullptr;
				break;
			default:
				LOG_TEST_CASE("ActiveTexture set to an unhandled resource type!");
				break;
			}
		}

		HRESULT hRet = g_pD3DDevice->SetTexture(i, pHostBaseTexture);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetTexture");
		if (bNeedRelease) {
			pHostBaseTexture->Release();
		}
	}
}

void CxbxUpdateNativeD3DResources()
{
	// Before we start, make sure our resource cache stays limited in size
	PrunePaletizedTexturesCache(); // TODO : Could we move this to Swap instead?

    EmuUpdateActiveTextureStages();

	// Some titles set Vertex Shader constants directly via pushbuffers rather than through D3D
	// We handle that case by updating any constants that have the dirty flag set on the nv2a.
	auto nv2a = g_NV2A->GetDeviceState();
	for(int i = 0; i < X_D3DVS_CONSTREG_COUNT; i++) {
        // Skip vOffset and vScale constants, we don't want our values to be overwritten by accident
        if (i == X_D3DSCM_RESERVED_CONSTANT_OFFSET_CORRECTED || i == X_D3DSCM_RESERVED_CONSTANT_SCALE_CORRECTED) {
            continue;
        }

		if (nv2a->pgraph.vsh_constants_dirty[i]) {
			g_pD3DDevice->SetVertexShaderConstantF(i, (float*)&nv2a->pgraph.vsh_constants[i][0], 1);
			nv2a->pgraph.vsh_constants_dirty[i] = false;
		}
	}

    // NOTE: Order is important here
    // Some Texture States depend on RenderState values (Point Sprites)
    // And some Pixel Shaders depend on Texture State values (BumpEnvMat, etc)
    XboxRenderStates.Apply();
    XboxTextureStates.Apply();

    // If Pixel Shaders are not disabled, process them
    if (!g_DisablePixelShaders) {
        DxbxUpdateActivePixelShader();
    }

/* TODO : Port these :
	DxbxUpdateActiveVertexShader();
	DxbxUpdateActiveTextures();
	DxbxUpdateDeferredStates(); // BeginPush sample shows us that this must come *after* texture update!
	DxbxUpdateActiveVertexBufferStreams();
	DxbxUpdateActiveRenderTarget();
*/
}

// This function should be called in thight idle-wait loops.
// It's purpose is to lower CPU cost in such a way that the
// caller will still repond quickly, without actually waiting
// or giving up it's time-slice.
// See https://docs.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-yieldprocessor
// and https://software.intel.com/en-us/cpp-compiler-developer-guide-and-reference-pause-intrinsic
inline void CxbxCPUIdleWait() // TODO : Apply wherever applicable
{
	YieldProcessor();
}

// This function indicates whether Cxbx can flush host GPU commands.
bool CxbxCanFlushHostGPU()
{
	return (g_pHostQueryWaitForIdle != nullptr);
}

// Wait until host GPU finished processing it's command queue
bool CxbxFlushHostGPU()
{
	// The following can only work when host GPU queries are available
	if (!CxbxCanFlushHostGPU()) {
		// If we can't query host GPU, return failure
		return false;
	}

	// See https://docs.microsoft.com/en-us/windows/win32/direct3d9/queries
	// Add an end marker to the command buffer queue.
	// This, so that the next GetData will always have at least one
	// final query event to flush out, after which GPU will be done.
	g_pHostQueryWaitForIdle->Issue(D3DISSUE_END);

	// Empty the command buffer and wait until host GPU is idle.
	while (S_FALSE == g_pHostQueryWaitForIdle->GetData(nullptr, 0, D3DGETDATA_FLUSH))
		CxbxCPUIdleWait();

	// Signal caller that host GPU has been flushed
	return true;
}

// This function mimicks NV2A software callback events.
// Normally, these would be handled by actual push-buffer
// command handling at the point where they where inserted.
// Since our HLE mostly circumvents the NV2A pushbuffer,
// this function has to be called after 'pushing' functions.
void CxbxHandleXboxCallbacks()
{
	// The following can only work when host GPU queries are available
	if (g_pHostQueryCallbackEvent != nullptr) {
		// Query whether host GPU encountered a callback event already
		if (S_FALSE == g_pHostQueryCallbackEvent->GetData(nullptr, 0, 0)) {
			// If not, don't handle callbacks
			return;
		}
	}

	// Process inserted callbacks
	while (!g_Xbox_CallbackQueue.empty()) {
		// Fetch a callback from the FIFO callback queue
		s_Xbox_Callback XboxCallback = g_Xbox_CallbackQueue.front();
		g_Xbox_CallbackQueue.pop();

		// Differentiate between write and read callbacks
		if (XboxCallback.Type == XTL::X_D3DCALLBACK_WRITE) {
			// Write callbacks should wait until GPU is idle
			if (!CxbxFlushHostGPU()) {
				// Host GPU can't be flushed. In the old behaviour, we made the callback anyway
				// TODO : Should we keep doing that?
			}
		} else {
			assert(XboxCallback.Type == XTL::X_D3DCALLBACK_READ);
			// Should we mimick Read callback old behaviour?
			if (g_bHack_DisableHostGPUQueries) {
				// Note : Previously, we only processed Write, and ignored Read callbacks
				continue;
			} else {
				// New behaviour does place Read callbacks too
			}
		}

		// Make the callback
		XboxCallback.pCallback(XboxCallback.Context);
	}
}

// On Xbox, this function inserts push-buffer commands that
// will trigger the software handler to perform the callback
// when the GPU processes these commands.
// The type X_D3DCALLBACK_WRITE callbacks are prefixed with an
// wait-for-idle command, but otherwise they're identical.
// (Software handlers are triggered on NV2A via NV097_NO_OPERATION) 
void CxbxImpl_InsertCallback
(
	XTL::X_D3DCALLBACKTYPE	Type,
	XTL::X_D3DCALLBACK		pCallback,
	XTL::DWORD				Context
)
{
	if (Type > XTL::X_D3DCALLBACK_WRITE) {
		LOG_TEST_CASE("Illegal callback type!");
		return;
	}

	if (pCallback == xbnullptr) {
		LOG_TEST_CASE("pCallback == xbnullptr!");
		return;
	}

	// Should we mimick old behaviour?
	if (g_bHack_DisableHostGPUQueries) {
		// Mimick old behaviour, in which only the final callback event
		// was remembered, by emptying the callback queue entirely :
		while (!g_Xbox_CallbackQueue.empty()) {
			g_Xbox_CallbackQueue.pop();
		}
	}

	// Push this callback's arguments into the callback queue :
	s_Xbox_Callback XboxCallback = { pCallback, Type, Context };
	g_Xbox_CallbackQueue.push(XboxCallback); // g_Xbox_CallbackQueue.emplace(pCallback, Type, Context); doesn't compile?

	// Does host supports GPU queries?
	if (g_pHostQueryCallbackEvent != nullptr) {
		// Insert a callback event on host GPU,
		// which will be handled by CxbxHandleXboxCallback
		g_pHostQueryCallbackEvent->Issue(D3DISSUE_END);
	}
}

VOID __declspec(noinline) D3DDevice_SetPixelShaderCommon(DWORD Handle)
{
    // Cache the active shader handle
    g_pXbox_PixelShader = (XTL::X_PixelShader*)Handle;

    // Copy the Pixel Shader data to our RenderState handler
    // This mirrors the fact that unpatched SetPixelShader does the same thing!
    // This shouldn't be necessary anymore, but shaders still break if we don't do this
    if (g_pXbox_PixelShader != nullptr) {
        memcpy(XboxRenderStates.GetPixelShaderRenderStatePointer(), g_pXbox_PixelShader->pPSDef, sizeof(XTL::X_D3DPIXELSHADERDEF) - 3 * sizeof(DWORD));
        XboxRenderStates.SetXboxRenderState(XTL::X_D3DRS_PSTEXTUREMODES, g_pXbox_PixelShader->pPSDef->PSTextureModes);
    }
}

// LTCG specific D3DDevice_SetPixelShader function...
// This uses a custom calling convention where parameter is passed in EAX
// Test-case: Metal Wolf Chaos
// Test-case: Lord of the Rings: The Third Age
VOID WINAPI D3DDevice_SetPixelShader_0_IMPL
(
    DWORD Handle
)
{
    LOG_FUNC_ONE_ARG(Handle);

    // Call the Xbox function to make sure D3D structures get set
    XB_trampoline(VOID, WINAPI, D3DDevice_SetPixelShader_0, ());
    __asm {
        mov eax, Handle
        call XB_D3DDevice_SetPixelShader_0
    }

    D3DDevice_SetPixelShaderCommon(Handle);
}

VOID WINAPI XTL::EMUPATCH(D3DDevice_SetPixelShader_0)()
{
    __asm {
        push eax
        call D3DDevice_SetPixelShader_0_IMPL
        ret
    }
}

// ******************************************************************
// * patch: D3DDevice_SetPixelShader
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetPixelShader)
(
	DWORD           Handle
)
{
	LOG_FUNC_ONE_ARG(Handle);

	// Call the Xbox function to make sure D3D structures get set
	XB_trampoline(VOID, WINAPI, D3DDevice_SetPixelShader, (DWORD));
	XB_D3DDevice_SetPixelShader(Handle);

	D3DDevice_SetPixelShaderCommon(Handle);
}

// ******************************************************************
// * patch: D3DDevice_DrawVertices_4
// LTCG specific D3DDevice_DrawVertices function...
// This uses a custom calling convention where parameter is passed in ECX, EAX and Stack
// Test Case: Conker
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_DrawVertices_4)
(
    X_D3DPRIMITIVETYPE  PrimitiveType
)
{
    UINT VertexCount;
    UINT StartVertex;
    
    _asm {
        mov VertexCount, eax
        mov StartVertex, ecx
    }

    EMUPATCH(D3DDevice_DrawVertices)(PrimitiveType, StartVertex, VertexCount);
}

// ******************************************************************
// * patch: D3DDevice_DrawVertices
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_DrawVertices)
(
    X_D3DPRIMITIVETYPE PrimitiveType,
    UINT               StartVertex,
    UINT               VertexCount
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(PrimitiveType)
		LOG_FUNC_ARG(StartVertex)
		LOG_FUNC_ARG(VertexCount)
		LOG_FUNC_END;

	// Dxbx Note : In DrawVertices and DrawIndexedVertices, PrimitiveType may not be D3DPT_POLYGON

	if (!IsValidXboxVertexCount(PrimitiveType, VertexCount)) {
		LOG_TEST_CASE("Invalid VertexCount");
		return;
	}

	// TODO : Call unpatched D3DDevice_SetStateVB(0);

	CxbxUpdateNativeD3DResources();
    if (IsValidCurrentShader()) {
		CxbxDrawContext DrawContext = {};

		DrawContext.XboxPrimitiveType = PrimitiveType;
		DrawContext.dwVertexCount = VertexCount;
		DrawContext.dwStartVertex = StartVertex;

		VertexBufferConverter.Apply(&DrawContext);
		if (DrawContext.XboxPrimitiveType == X_D3DPT_QUADLIST) {
			if (StartVertex == 0) {
				//LOG_TEST_CASE("X_D3DPT_QUADLIST (StartVertex == 0)"); // disabled, hit too often
				// test-case : ?X-Marbles
				// test-case XDK Samples : AlphaFog, AntiAlias, BackBufferScale, BeginPush, Cartoon, TrueTypeFont (?maybe PlayField?)
			} else {
				LOG_TEST_CASE("X_D3DPT_QUADLIST (StartVertex > 0)");
				// https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/1156
				// test-case : All - Star Baseball '03
				// test-case : Army Men Major Malfunction
				// test-case : Big Mutha Truckers
				// test-case : BLiNX: the time sweeper
				// test-case : Blood Wake
				// test-case : Call of Duty: Finest Hour
				// test-case : Flight academy
				// test-case : FIFA World Cup 2002
				// test-case : GENMA ONIMUSHA 
				// test-case : Halo - Combat Evolved
				// test-case : Harry Potter and the Sorcerer's Stone
				// test-case : Heroes of the Pacific
				// test-case : Hummer Badlands
				// test-case : Knights Of The Temple 2
				// test-case : LakeMasters Bass fishing
				// test-case : MetalDungeon
				// test-case : NFL Fever 2003 Demo - main menu
				// test-case : Night Caster 2
				// test-case : Pinball Hall of Fame
				// test-case : Robotech : Battlecry
				// test-case : SpiderMan 2
				// test-case : Splinter Cell Demo
				// test-case : Stubbs the Zombie
				// test-case : Tony Hawk's Pro Skater 2X (main menu entries)
				// test-case : Worms 3D Special Edition
				// test-case : XDK sample Lensflare (4, for 10 flare-out quads that use a linear texture; rendered incorrectly: https://youtu.be/idwlxHl9nAA?t=439)
				DrawContext.dwStartVertex = StartVertex; // Breakpoint location for testing.
			}

			// Draw quadlists using a single 'quad-to-triangle mapping' index buffer :
			// Assure & activate that special index buffer :
			CxbxAssureQuadListD3DIndexBuffer(/*NrOfQuadIndices=*/DrawContext.dwVertexCount);
			// This API's StartVertex argument is multiplied by vertex stride and added to the start of the vertex buffer;
			// BaseVertexIndex offers the same functionality on host :
			UINT BaseVertexIndex = DrawContext.dwStartVertex;
			// Convert quad vertex count to triangle vertex count :
			UINT NumVertices = QuadToTriangleVertexCount(DrawContext.dwVertexCount);
			// Convert quad primitive count to triangle primitive count :
			UINT primCount = DrawContext.dwHostPrimitiveCount * TRIANGLES_PER_QUAD;
			// See https://docs.microsoft.com/en-us/windows/win32/direct3d9/rendering-from-vertex-and-index-buffers
			// for an explanation on the function of the BaseVertexIndex, MinVertexIndex, NumVertices and StartIndex arguments.
			// Emulate drawing quads by drawing each quad with two indexed triangles :
			HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitive(
				/*PrimitiveType=*/D3DPT_TRIANGLELIST,
				BaseVertexIndex,
				/*MinVertexIndex=*/0,
				NumVertices,
				/*startIndex=*/0,
				primCount
			);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitive(X_D3DPT_QUADLIST)");

			g_dwPrimPerFrame += primCount;
		}
		else {
			// if (StartVertex > 0) LOG_TEST_CASE("StartVertex > 0 (non-quad)"); // Verified test case : XDK Sample (PlayField)
			HRESULT hRet = g_pD3DDevice->DrawPrimitive(
				EmuXB2PC_D3DPrimitiveType(DrawContext.XboxPrimitiveType),
				DrawContext.dwStartVertex,
				DrawContext.dwHostPrimitiveCount
			);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawPrimitive");

			g_dwPrimPerFrame += DrawContext.dwHostPrimitiveCount;
			if (DrawContext.XboxPrimitiveType == X_D3DPT_LINELOOP) {
				// Close line-loops using a final single line, drawn from the end to the start vertex
				LOG_TEST_CASE("X_D3DPT_LINELOOP"); // TODO : Text-cases needed

				assert(DrawContext.dwBaseVertexIndex == 0); // if this fails, it needs to be added to LowIndex and HighIndex :
				INDEX16 LowIndex = (INDEX16)DrawContext.dwStartVertex;
				INDEX16 HighIndex = (INDEX16)(DrawContext.dwStartVertex + DrawContext.dwHostPrimitiveCount);
				// Draw the closing line using a helper function (which will SetIndices)
				CxbxDrawIndexedClosingLine(LowIndex, HighIndex);
				// NOTE : We don't restore the previously active index buffer
			}
		}
    }

	CxbxHandleXboxCallbacks();
}

// ******************************************************************
// * patch: D3DDevice_DrawVerticesUP
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_DrawVerticesUP)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    UINT                VertexCount,
    CONST PVOID         pVertexStreamZeroData,
    UINT                VertexStreamZeroStride
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(PrimitiveType)
		LOG_FUNC_ARG(VertexCount)
		LOG_FUNC_ARG(pVertexStreamZeroData)
		LOG_FUNC_ARG(VertexStreamZeroStride)
		LOG_FUNC_END;

	if (!IsValidXboxVertexCount(PrimitiveType, VertexCount)) {
		LOG_TEST_CASE("Invalid VertexCount");
		return;
	}

	// TODO : Call unpatched D3DDevice_SetStateUP();

	CxbxUpdateNativeD3DResources();

    if (IsValidCurrentShader()) {
		CxbxDrawContext DrawContext = {};

		DrawContext.XboxPrimitiveType = PrimitiveType;
		DrawContext.dwVertexCount = VertexCount;
		DrawContext.pXboxVertexStreamZeroData = pVertexStreamZeroData;
		DrawContext.uiXboxVertexStreamZeroStride = VertexStreamZeroStride;

		CxbxDrawPrimitiveUP(DrawContext);
    }

	CxbxHandleXboxCallbacks();
}

// ******************************************************************
// * patch: D3DDevice_DrawIndexedVertices
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_DrawIndexedVertices)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    UINT                VertexCount,
    CONST PWORD         pIndexData
)
{
	// Test-cases : XDK samples (Cartoon, Gamepad)
	// Note : In gamepad.xbe, the gamepad is drawn by D3DDevice_DrawIndexedVertices
	// Dxbx Note : In DrawVertices and DrawIndexedVertices, PrimitiveType may not be D3DPT_POLYGON

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(PrimitiveType)
		LOG_FUNC_ARG(VertexCount)
		LOG_FUNC_ARG(pIndexData)
		LOG_FUNC_END;

	if (!IsValidXboxVertexCount(PrimitiveType, VertexCount)) {
		LOG_TEST_CASE("Invalid VertexCount");
		return;
	}

	// TODO : Call unpatched D3DDevice_SetStateVB(g_Xbox_BaseVertexIndex);

	CxbxUpdateNativeD3DResources();

	if (IsValidCurrentShader()) {
		CxbxDrawContext DrawContext = {};

		DrawContext.XboxPrimitiveType = PrimitiveType;
		DrawContext.dwVertexCount = VertexCount;
		DrawContext.dwBaseVertexIndex = g_Xbox_BaseVertexIndex; // Multiplied by vertex stride and added to the vertex buffer start
		DrawContext.pXboxIndexData = pIndexData; // Used to derive VerticesInBuffer

		// Test case JSRF draws all geometry through this function (only sparks are drawn via another method)
		// using X_D3DPT_TRIANGLELIST and X_D3DPT_TRIANGLESTRIP PrimitiveType
		CxbxDrawIndexed(DrawContext);
	}

	CxbxHandleXboxCallbacks();
}

// ******************************************************************
// * patch: D3DDevice_DrawIndexedVerticesUP
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_DrawIndexedVerticesUP)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    UINT                VertexCount,
    CONST PVOID         pIndexData,
    CONST PVOID         pVertexStreamZeroData,
    UINT                VertexStreamZeroStride
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(PrimitiveType)
		LOG_FUNC_ARG(VertexCount)
		LOG_FUNC_ARG(pIndexData)
		LOG_FUNC_ARG(pVertexStreamZeroData)
		LOG_FUNC_ARG(VertexStreamZeroStride)
		LOG_FUNC_END;

	if (!IsValidXboxVertexCount(PrimitiveType, VertexCount)) {
		LOG_TEST_CASE("Invalid VertexCount");
		return;
	}

	// TODO : Call unpatched D3DDevice_SetStateUP();

	CxbxUpdateNativeD3DResources();

	if (IsValidCurrentShader()) {
		CxbxDrawContext DrawContext = {};
		INDEX16* pXboxIndexData = (INDEX16*)pIndexData;

		DrawContext.XboxPrimitiveType = PrimitiveType;
		DrawContext.dwVertexCount = VertexCount;
		DrawContext.pXboxIndexData = pXboxIndexData; // Used to derive VerticesInBuffer
		// Note : D3DDevice_DrawIndexedVerticesUP does NOT use g_Xbox_BaseVertexIndex, so keep DrawContext.dwBaseVertexIndex at 0!
		DrawContext.pXboxVertexStreamZeroData = pVertexStreamZeroData;
		DrawContext.uiXboxVertexStreamZeroStride = VertexStreamZeroStride;

		// Determine LowIndex and HighIndex *before* VerticesInBuffer gets derived
		WalkIndexBuffer(DrawContext.LowIndex, DrawContext.HighIndex, pXboxIndexData, VertexCount);

		VertexBufferConverter.Apply(&DrawContext);

		INDEX16* pHostIndexData;
		UINT PrimitiveCount = DrawContext.dwHostPrimitiveCount;

		bool bConvertQuadListToTriangleList = (DrawContext.XboxPrimitiveType == X_D3DPT_QUADLIST);
		if (bConvertQuadListToTriangleList) {
			LOG_TEST_CASE("X_D3DPT_QUADLIST");
			// Test-case : Buffy: The Vampire Slayer
			// Test-case : XDK samples : FastLoad, BackBufferScale, DisplacementMap, Donuts3D, VolumeLight, PersistDisplay, PolynomialTextureMaps, SwapCallback, Tiling, VolumeFog, DebugKeyboard, Gamepad
			// Convert draw arguments from quads to triangles :
			pHostIndexData = CxbxCreateQuadListToTriangleListIndexData(pXboxIndexData, VertexCount);
			PrimitiveCount *= TRIANGLES_PER_QUAD;
			// Note, that LowIndex and HighIndex won't change due to this quad-to-triangle conversion,
			// so it's less work to WalkIndexBuffer over the input instead of the converted index buffer.
		} else {
			// LOG_TEST_CASE("DrawIndexedPrimitiveUP"); // Test-case : Burnout, Namco Museum 50th Anniversary
			pHostIndexData = pXboxIndexData;
		}

		HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitiveUP(
			/*PrimitiveType=*/EmuXB2PC_D3DPrimitiveType(DrawContext.XboxPrimitiveType),
			/*MinVertexIndex=*/DrawContext.LowIndex,
			/*NumVertexIndices=*/(DrawContext.HighIndex - DrawContext.LowIndex) + 1,
			PrimitiveCount,
			pHostIndexData,
			/*IndexDataFormat=*/D3DFMT_INDEX16,
			DrawContext.pHostVertexStreamZeroData,
			DrawContext.uiHostVertexStreamZeroStride
		);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitiveUP");

		if (bConvertQuadListToTriangleList) {
			CxbxReleaseQuadListToTriangleListIndexData(pHostIndexData);
		}

		g_dwPrimPerFrame += PrimitiveCount;
		if (DrawContext.XboxPrimitiveType == X_D3DPT_LINELOOP) {
			// Close line-loops using a final single line, drawn from the end to the start vertex
			LOG_TEST_CASE("X_D3DPT_LINELOOP"); // TODO : Which titles reach this test-case?
			// Read the end and start index from the supplied index data
			INDEX16 LowIndex = pXboxIndexData[0];
			INDEX16 HighIndex = pXboxIndexData[DrawContext.dwHostPrimitiveCount];
			// If needed, swap so highest index is higher than lowest (duh)
			if (HighIndex < LowIndex) {
				std::swap(HighIndex, LowIndex);
			}

			// Close line-loops using a final single line, drawn from the end to the start vertex :
			CxbxDrawIndexedClosingLineUP(
				LowIndex,
				HighIndex,
				DrawContext.pHostVertexStreamZeroData,
				DrawContext.uiHostVertexStreamZeroStride
			);
		}
    }

	CxbxHandleXboxCallbacks();
}

// ******************************************************************
// * patch: D3DDevice_SetLight
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetLight)
(
    DWORD            Index,
    CONST X_D3DLIGHT8 *pLight
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Index)
		LOG_FUNC_ARG(pLight)
		LOG_FUNC_END;

	XB_trampoline(HRESULT, WINAPI, D3DDevice_SetLight, (DWORD, CONST X_D3DLIGHT8*));
	XB_D3DDevice_SetLight(Index, pLight);

    HRESULT hRet = g_pD3DDevice->SetLight(Index, pLight);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetLight");    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_SetMaterial
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetMaterial)
(
    CONST X_D3DMATERIAL8 *pMaterial
)
{
	LOG_FUNC_ONE_ARG(pMaterial);

    HRESULT hRet = g_pD3DDevice->SetMaterial(pMaterial);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetMaterial");
}

// ******************************************************************
// * patch: D3DDevice_LightEnable
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_LightEnable)
(
    DWORD            Index,
    BOOL             bEnable
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Index)
		LOG_FUNC_ARG(bEnable)
		LOG_FUNC_END;

	XB_trampoline(HRESULT, WINAPI, D3DDevice_LightEnable, (DWORD, BOOL));
	XB_D3DDevice_LightEnable(Index, bEnable);

    HRESULT hRet = g_pD3DDevice->LightEnable(Index, bEnable);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->LightEnable");    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderTarget
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderTarget)
(
    X_D3DSurface    *pRenderTarget,
    X_D3DSurface    *pNewZStencil
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pRenderTarget)
		LOG_FUNC_ARG(pNewZStencil)
		LOG_FUNC_END;

	IDirect3DSurface *pHostRenderTarget = nullptr;
	IDirect3DSurface *pHostDepthStencil = nullptr;

	XB_trampoline(void, WINAPI, D3DDevice_SetRenderTarget, (X_D3DSurface*, X_D3DSurface*));
	XB_D3DDevice_SetRenderTarget(pRenderTarget, pNewZStencil);

	// In Xbox titles, CreateDevice calls SetRenderTarget for the back buffer
	// We can use this to determine the Xbox backbuffer surface for later use!
	if (g_pXbox_BackBufferSurface == xbnullptr) {
		g_pXbox_BackBufferSurface = pRenderTarget;
		// TODO : Some titles might render to another backbuffer later on,
		// if that happens, we might need to skip the first one or two calls?
	}

    if (g_pXbox_DefaultDepthStencilSurface == xbnullptr) {
        g_pXbox_DefaultDepthStencilSurface = pNewZStencil;
    }

	// The current render target is only replaced if it's passed in here non-null
	if (pRenderTarget != xbnullptr) {
		g_pXbox_RenderTarget = pRenderTarget;
	}
	else {
		// If non is given, use the current Xbox render target
		pRenderTarget = g_pXbox_RenderTarget;
		// If there's no Xbox render target yet, fallback to the Xbox back buffer
		if (pRenderTarget == xbnullptr) {
			LOG_TEST_CASE("SetRenderTarget fallback to backbuffer");
			pRenderTarget = g_pXbox_BackBufferSurface;
		}
    }

	pHostRenderTarget = GetHostSurface(pRenderTarget, D3DUSAGE_RENDERTARGET);

	// The currenct depth stencil is always replaced by whats passed in here (even a null)
	g_pXbox_DepthStencil = pNewZStencil;
    pHostDepthStencil = GetHostSurface(g_pXbox_DepthStencil, D3DUSAGE_DEPTHSTENCIL);

	HRESULT hRet;
	// Mimick Direct3D 8 SetRenderTarget by only setting render target if non-null
	if (pHostRenderTarget) {
		hRet = g_pD3DDevice->SetRenderTarget(/*RenderTargetIndex=*/0, pHostRenderTarget);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetRenderTarget");
		if (FAILED(hRet)) {
			// If Direct3D 9 SetRenderTarget failed, skip setting depth stencil
			return;
		}
	}

	hRet = g_pD3DDevice->SetDepthStencilSurface(pHostDepthStencil);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetDepthStencilSurface");

	if (SUCCEEDED(hRet)) {
		// Once we're sure the host depth-stencil is activated...
		UpdateDepthStencilFlags(pHostDepthStencil);
	}

    // Validate that our host render target is still the correct size
    DWORD HostRenderTarget_Width, HostRenderTarget_Height;
    if (GetHostRenderTargetDimensions(&HostRenderTarget_Width, &HostRenderTarget_Height)) {
        DWORD XboxRenderTarget_Width = GetPixelContainerWidth(g_pXbox_RenderTarget);
        DWORD XboxRenderTarget_Height = GetPixelContainerHeight(g_pXbox_RenderTarget);
        ValidateRenderTargetDimensions(HostRenderTarget_Width, HostRenderTarget_Height, XboxRenderTarget_Width, XboxRenderTarget_Height);
    }

    UpdateViewPortOffsetAndScaleConstants();
}

// LTCG specific D3DDevice_SetPalette function...
// This uses a custom calling convention where parameter is passed in EAX
// Test-case: Ninja Gaiden
VOID __stdcall XTL::EMUPATCH(D3DDevice_SetPalette_4)
(
)
{
	static uint32_t returnAddr;

#ifdef _DEBUG_TRACE
		__asm add esp, 4
#endif

	__asm {
		pop returnAddr
		push eax
		call EmuPatch_D3DDevice_SetPalette
		mov eax, 0
		push returnAddr
		ret
	}
}

// ******************************************************************
// * patch: D3DDevice_SetPalette
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetPalette)
(
    DWORD         Stage,
    X_D3DPalette *pPalette
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Stage)
		LOG_FUNC_ARG(pPalette)
		LOG_FUNC_END;

	//    g_pD3DDevice9->SetPaletteEntries(Stage?, (PALETTEENTRY*)pPalette->Data);
	//    g_pD3DDevice9->SetCurrentTexturePalette(Stage, Stage);

	if (Stage >= XTL::X_D3DTS_STAGECOUNT) {
		LOG_TEST_CASE("Stage out of bounds");
	} else {
		// Note : Actual update of paletized textures (X_D3DFMT_P8) happens in EmuUpdateActiveTextureStages!
		g_pXbox_Palette_Data[Stage] = GetDataFromXboxResource(pPalette);
		g_Xbox_Palette_Size[Stage] = pPalette ? XboxD3DPaletteSizeToBytes(GetXboxPaletteSize(pPalette)) : 0;
	}
}

// LTCG specific D3DDevice_SetFlickerFilter function...
// This uses a custom calling convention where parameter is passed in ESI
// Test-case: Metal Wolf Chaos
VOID __stdcall XTL::EMUPATCH(D3DDevice_SetFlickerFilter_0)
(
)
{
	DWORD         Filter;

	__asm {
		mov Filter, esi
	}

	return EMUPATCH(D3DDevice_SetFlickerFilter)(Filter);
}

// ******************************************************************
// * patch: D3DDevice_SetFlickerFilter
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3DDevice_SetFlickerFilter)
(
    DWORD         Filter
)
{
	LOG_FUNC_ONE_ARG(Filter);

	LOG_IGNORED();
}

// ******************************************************************
// * patch: D3DDevice_SetSoftDisplayFilter
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3DDevice_SetSoftDisplayFilter)
(
    BOOL Enable
)
{
	LOG_FUNC_ONE_ARG(Enable);

	LOG_IGNORED();
}

// ******************************************************************
// * patch: D3DDevice_GetVertexShaderSize
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetVertexShaderSize)
(
    DWORD Handle,
    UINT* pSize
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Handle)
		LOG_FUNC_ARG(pSize)
		LOG_FUNC_END;

	// Handle is always address of an Xbox VertexShader struct, or-ed with 1 (X_D3DFVF_RESERVED0)

    if (pSize) {
        CxbxVertexShader *pVertexShader = GetCxbxVertexShader(Handle);
        *pSize = pVertexShader ? pVertexShader->XboxNrAddressSlots : 0;
    }
}

// LTCG specific D3DDevice_DeleteVertexShader function...
// This uses a custom calling convention where parameter is passed in EAX
// UNTESTED - Need test-case!
VOID __stdcall XTL::EMUPATCH(D3DDevice_DeleteVertexShader_0)
(
)
{
	DWORD Handle;

	__asm {
		mov Handle, eax
	}

	LOG_TEST_CASE("Validate this function!");
	return EMUPATCH(D3DDevice_DeleteVertexShader)(Handle);
}

// ******************************************************************
// * patch: D3DDevice_DeleteVertexShader
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_DeleteVertexShader)
(
	DWORD Handle
)
{
	LOG_FUNC_ONE_ARG(Handle);

	XB_trampoline(void, WINAPI, D3DDevice_DeleteVertexShader, (DWORD));
	XB_D3DDevice_DeleteVertexShader(Handle);

	// Handle is always address of an Xbox VertexShader struct, or-ed with 1 (X_D3DFVF_RESERVED0)
	// It's reference count is lowered. If it reaches zero (0), the struct is freed.

	if (VshHandleIsVertexShader(Handle))
	{
		CxbxVertexShader *pCxbxVertexShader = GetCxbxVertexShader(Handle);
		SetCxbxVertexShader(Handle, nullptr);

		if (pCxbxVertexShader->pHostVertexDeclaration) {
			HRESULT hRet = pCxbxVertexShader->pHostVertexDeclaration->Release();
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DeleteVertexShader(pHostVertexDeclaration)");
		}

		if (pCxbxVertexShader->pHostVertexShader) {
			HRESULT hRet = pCxbxVertexShader->pHostVertexShader->Release();
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DeleteVertexShader(pHostVertexShader)");
		}

		if (pCxbxVertexShader->pXboxDeclarationCopy)
		{
			free(pCxbxVertexShader->pXboxDeclarationCopy);
		}

		if (pCxbxVertexShader->pXboxFunctionCopy)
		{
			free(pCxbxVertexShader->pXboxFunctionCopy);
		}

		FreeVertexDynamicPatch(pCxbxVertexShader);

		free(pCxbxVertexShader);
	}
}

// ******************************************************************
// * patch: D3DDevice_SelectVertexShaderDirect
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SelectVertexShaderDirect)
(
    X_VERTEXATTRIBUTEFORMAT *pVAF,
    DWORD                    Address
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pVAF)
		LOG_FUNC_ARG(Address)
		LOG_FUNC_END;

	CxbxImpl_SelectVertexShaderDirect(pVAF, Address);
}

// ******************************************************************
// * patch: D3DDevice_GetShaderConstantMode
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetShaderConstantMode)
(
    DWORD *pMode
)
{
	LOG_FUNC_ONE_ARG(pMode);
        
    if(pMode)
    {
        *pMode = g_Xbox_VertexShaderConstantMode;
    }
}

// ******************************************************************
// * patch: D3DDevice_GetVertexShader
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetVertexShader)
(
    DWORD *pHandle
)
{
	LOG_FUNC_ONE_ARG(pHandle);

    if(pHandle)
    {
        (*pHandle) = g_Xbox_VertexShader_Handle;
    }
}

// ******************************************************************
// * patch: D3DDevice_GetVertexShaderConstant
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetVertexShaderConstant)
(
    INT   Register,
    void  *pConstantData,
    DWORD ConstantCount
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Register)
		LOG_FUNC_ARG(pConstantData)
		LOG_FUNC_ARG(ConstantCount)
		LOG_FUNC_END;

	// Xbox vertex shader constants range from -96 to 95
	// The host does not support negative, so we adjust to 0..191
	Register += X_D3DSCM_CORRECTION;

	HRESULT hRet = g_pD3DDevice->GetVertexShaderConstantF
    (
        Register,
        (float*)pConstantData, // TODO : Validate this work correctly under D3D9
        ConstantCount
    );

	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetVertexShaderConstant");
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderInputDirect
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetVertexShaderInputDirect)
(
    X_VERTEXATTRIBUTEFORMAT *pVAF,
    UINT                     StreamCount,
    X_STREAMINPUT           *pStreamInputs
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pVAF)
		LOG_FUNC_ARG(StreamCount)
		LOG_FUNC_ARG(pStreamInputs)
		LOG_FUNC_END;

	// If pVAF is given, it's copied into a global Xbox VertexBuffer struct and
	// D3DDevice_SetVertexShaderInput is called with Handle set to that address, or-ed with 1 (X_D3DFVF_RESERVED0)
	// Otherwise, D3DDevice_SetVertexShaderInput is called with Handle 0.

    LOG_UNIMPLEMENTED(); 
}

// ******************************************************************
// * patch: D3DDevice_GetVertexShaderInput
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetVertexShaderInput)
(
    DWORD              *pHandle,
    UINT               *pStreamCount,
    X_STREAMINPUT      *pStreamInputs
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHandle)
		LOG_FUNC_ARG(pStreamCount)
		LOG_FUNC_ARG(pStreamInputs)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED(); 

    return 0;
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderInput
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetVertexShaderInput)
(
    DWORD              Handle,
    UINT               StreamCount,
    X_STREAMINPUT     *pStreamInputs
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Handle)
		LOG_FUNC_ARG(StreamCount)
		LOG_FUNC_ARG(pStreamInputs)
		LOG_FUNC_END;

	// When this API is in effect, VertexBuffers as set by Xbox SetStreamSource are disregarded,
	// instead, the pStreamInputs[].VertexBuffer streams are used.

	// If Handle is NULL, all VertexShader input state is cleared (after which the VertexBuffers as set by SetStreamSource are used once again).

	// Otherwise, Handle is the address of an Xbox VertexShader struct, or-ed with 1 (X_D3DFVF_RESERVED0)
	// The given pStreamInputs are stored in a global array, and the NV2A is programmed to read
	// each vertex attribute (as defined in the given VertexShader.VertexAttribute.Slots[]) to read
	// the attribute data from the pStreamInputs[slot].VertexBuffer + pStreamInputs[slot].Offset + VertexShader.VertexAttribute.Slots[slot].Offset

	CxbxImpl_SetVertexShaderInput(Handle, StreamCount, pStreamInputs);
}

// ******************************************************************
// * patch: D3DDevice_RunVertexStateShader
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_RunVertexStateShader)
(
    DWORD Address,
    CONST FLOAT *pData
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Address)
		LOG_FUNC_ARG(pData)
		LOG_FUNC_END;

	// If pData is assigned, pData[0..3] is pushed towards nv2a transform data registers
	// then sends the nv2a a command to launch the vertex shader function located at Address

    LOG_UNIMPLEMENTED(); 
}

// Maps pFunction defintions to pre-compiled shaders
// to reduce the speed impact of LoadVertexShaderProgram
typedef uint64_t load_shader_program_key_t;
std::unordered_map<load_shader_program_key_t, DWORD> g_LoadVertexShaderProgramCache;

// ******************************************************************
// * patch: D3DDevice_LoadVertexShaderProgram
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_LoadVertexShaderProgram)
(
    CONST DWORD *pFunction,
    DWORD        Address
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pFunction)
		LOG_FUNC_ARG(Address)
		LOG_FUNC_END;

	// D3DDevice_LoadVertexShaderProgram splits the given function buffer into batch-wise pushes to the NV2A

	load_shader_program_key_t shaderCacheKey = ((load_shader_program_key_t)g_Xbox_VertexShader_Handle << 32) | (DWORD)pFunction;
    
	// If the shader key was located in the cache, use the cached shader
	// TODO: When do we clear the cache? In this approach, shaders are
	// never freed...
	auto it = g_LoadVertexShaderProgramCache.find(shaderCacheKey);
	if (it != g_LoadVertexShaderProgramCache.end()) {
		EMUPATCH(D3DDevice_LoadVertexShader)(it->second, Address);
		EMUPATCH(D3DDevice_SelectVertexShader)(it->second, Address);
		return;
	}

    DWORD *pXboxVertexDeclaration = nullptr;

    if (VshHandleIsVertexShader(g_Xbox_VertexShader_Handle)) {
        CxbxVertexShader *pCxbxVertexShader = GetCxbxVertexShader(g_Xbox_VertexShader_Handle);

        // If we failed to fetch an active pixel shader, log and do nothing
        if (pCxbxVertexShader == nullptr) {
            LOG_TEST_CASE("D3DDevice_LoadVertexShaderProgram: Failed to locate original shader");
            return;
        }

        // Simply retrieve the contents of the existing vertex shader program
		pXboxVertexDeclaration = pCxbxVertexShader->pXboxDeclarationCopy;
    } else {
        // This is an unusual scenario in which an FVF-based shader is being replaced with an actual shader
        // But without calling CreateVertexShader: This means we need to parse the current FVF and generate
        // our own Xbox-like declaration to use when converting/setting this new shader

        // Define a large enough definition to contain all possible FVF types
        // 20 is maximum possible size
		DWORD CxbxXboxVertexDeclaration[20] = { 0 };
        int index = 0;

        // Write the Stream Number (always 0 for FVF)
        CxbxXboxVertexDeclaration[index++] = X_D3DVSD_STREAM(0);

        // Write Position
        DWORD position = (g_Xbox_VertexShader_Handle & X_D3DFVF_POSITION_MASK);
        if (position == X_D3DFVF_XYZRHW) {
            CxbxXboxVertexDeclaration[index++] = X_D3DVSD_REG(X_D3DVSDE_POSITION, X_D3DVSDT_FLOAT4);
        } else {
            CxbxXboxVertexDeclaration[index++] = X_D3DVSD_REG(X_D3DVSDE_POSITION, X_D3DVSDT_FLOAT3);
        }

        // Write Blend Weights
        if (position == X_D3DFVF_XYZB1) {
            CxbxXboxVertexDeclaration[index++] = X_D3DVSD_REG(X_D3DVSDE_BLENDWEIGHT, X_D3DVSDT_FLOAT1);
        }
        if (position == X_D3DFVF_XYZB2) {
            CxbxXboxVertexDeclaration[index++] = X_D3DVSD_REG(X_D3DVSDE_BLENDWEIGHT, X_D3DVSDT_FLOAT2);
        }
        if (position == X_D3DFVF_XYZB3) {
            CxbxXboxVertexDeclaration[index++] = X_D3DVSD_REG(X_D3DVSDE_BLENDWEIGHT, X_D3DVSDT_FLOAT3);
        }
        if (position == X_D3DFVF_XYZB4) {
            CxbxXboxVertexDeclaration[index++] = X_D3DVSD_REG(X_D3DVSDE_BLENDWEIGHT, X_D3DVSDT_FLOAT4);
        }

        // Write Normal, Diffuse, and Specular
        if (g_Xbox_VertexShader_Handle & X_D3DFVF_NORMAL) {
            CxbxXboxVertexDeclaration[index++] = X_D3DVSD_REG(X_D3DVSDE_NORMAL, X_D3DVSDT_FLOAT3);
        }
        if (g_Xbox_VertexShader_Handle & X_D3DFVF_DIFFUSE) {
            CxbxXboxVertexDeclaration[index++] = X_D3DVSD_REG(X_D3DVSDE_DIFFUSE, X_D3DVSDT_D3DCOLOR);
        }
        if (g_Xbox_VertexShader_Handle & X_D3DFVF_SPECULAR) {
            CxbxXboxVertexDeclaration[index++] = X_D3DVSD_REG(X_D3DVSDE_SPECULAR, X_D3DVSDT_D3DCOLOR);
        }

        // Write Texture Coordinates
        int textureCount = (g_Xbox_VertexShader_Handle & X_D3DFVF_TEXCOUNT_MASK) >> X_D3DFVF_TEXCOUNT_SHIFT;
        assert(textureCount <= 4); // Safeguard, since the X_D3DFVF_TEXCOUNT bitfield could contain invalid values (5 up to 15)
        for (int i = 0; i < textureCount; i++) {
            int numberOfCoordinates = 0;

            if ((g_Xbox_VertexShader_Handle & X_D3DFVF_TEXCOORDSIZE1(i)) == (DWORD)X_D3DFVF_TEXCOORDSIZE1(i)) {
                numberOfCoordinates = X_D3DVSDT_FLOAT1;
            }
            if ((g_Xbox_VertexShader_Handle & X_D3DFVF_TEXCOORDSIZE2(i)) == (DWORD)X_D3DFVF_TEXCOORDSIZE2(i)) {
                numberOfCoordinates = X_D3DVSDT_FLOAT2;
            }
            if ((g_Xbox_VertexShader_Handle & X_D3DFVF_TEXCOORDSIZE3(i)) == (DWORD)X_D3DFVF_TEXCOORDSIZE3(i)) {
                numberOfCoordinates = X_D3DVSDT_FLOAT3;
            }
            if ((g_Xbox_VertexShader_Handle & X_D3DFVF_TEXCOORDSIZE4(i)) == (DWORD)X_D3DFVF_TEXCOORDSIZE4(i)) {
                numberOfCoordinates = X_D3DVSDT_FLOAT4;
            }

            CxbxXboxVertexDeclaration[index++] = X_D3DVSD_REG(X_D3DVSDE_TEXCOORD0 + i, numberOfCoordinates);
        }

        // Write Declaration End
        CxbxXboxVertexDeclaration[index++] = X_D3DVSD_END();

        pXboxVertexDeclaration = CxbxXboxVertexDeclaration;
        // Now we can fall through and create a new vertex shader
    }

	// Create a vertex shader with the new vertex program data
	DWORD hNewXboxShader = 0;
	HRESULT hr = EMUPATCH(D3DDevice_CreateVertexShader)(pXboxVertexDeclaration, pFunction, &hNewXboxShader, 0);

	if( FAILED( hr ) )
		CxbxKrnlCleanup("Error creating new vertex shader!" );

	EMUPATCH(D3DDevice_LoadVertexShader)(hNewXboxShader, Address);
	EMUPATCH(D3DDevice_SelectVertexShader)(hNewXboxShader, Address);

	g_LoadVertexShaderProgramCache[shaderCacheKey] = hNewXboxShader;

	EmuLog(LOG_LEVEL::WARNING, "Vertex Shader Cache Size: %d", g_LoadVertexShaderProgramCache.size());
}

// ******************************************************************
// * patch: D3DDevice_GetVertexShaderType
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetVertexShaderType)
(
    DWORD  Handle,
    DWORD *pType
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Handle)
		LOG_FUNC_ARG(pType)
		LOG_FUNC_END;

	// Handle is always address of an Xbox VertexShader struct, or-ed with 1 (X_D3DFVF_RESERVED0)
	// *pType is set according to flags in the VertexShader struct

	if (pType) {
		CxbxVertexShader *pCxbxVertexShader = GetCxbxVertexShader(Handle);
		if (pCxbxVertexShader) {
			*pType = pCxbxVertexShader->XboxVertexShaderType;
		}
    }
}

// ******************************************************************
// * patch: D3DDevice_GetVertexShaderDeclaration
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetVertexShaderDeclaration)
(
    DWORD  Handle,
    PVOID  pData,
    DWORD *pSizeOfData
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Handle)
		LOG_FUNC_ARG(pData)
		LOG_FUNC_ARG(pSizeOfData)
		LOG_FUNC_END;

	// Handle is always address of an Xbox VertexShader struct, or-ed with 1 (X_D3DFVF_RESERVED0)
	// If the pData buffer pointer is given, pSizeOfData is the address of it's size (in bytes)
	// If pData is null, pSizeOfData is still given (to receive the required data size)


	// The VertexShader is converted back into the contained program and it's size.
	// In any case, *pSizeOfData will be set to the program size.
	// If the pData is null, no further action it taken.
	// If the pData buffer pointer is given, but the given *pSizeOfData is smaller than the program size, an error is returned.
	// Otherwise, the program is unbatched and copied into the pData buffer.

	HRESULT hRet = D3DERR_INVALIDCALL;

    if (pSizeOfData) {
        CxbxVertexShader *pCxbxVertexShader = GetCxbxVertexShader(Handle);
		if (pCxbxVertexShader) {
			DWORD sizeOfData = pCxbxVertexShader->XboxDeclarationCount * sizeof(DWORD);
			if (*pSizeOfData < sizeOfData || !pData) {
				*pSizeOfData = sizeOfData;
				hRet = !pData ? D3D_OK : D3DERR_MOREDATA;
			}
			else {
				memcpy(pData, pCxbxVertexShader->pXboxDeclarationCopy, pCxbxVertexShader->XboxDeclarationCount * sizeof(DWORD));
				hRet = D3D_OK;
			}
		}
    }
    
    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_GetVertexShaderFunction
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetVertexShaderFunction)
(
    DWORD  Handle,
    PVOID *pData,
    DWORD *pSizeOfData
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Handle)
		LOG_FUNC_ARG(pData)
		LOG_FUNC_ARG(pSizeOfData)
		LOG_FUNC_END;

	// Handle is always address of an Xbox VertexShader struct, or-ed with 1 (X_D3DFVF_RESERVED0)
	// If the pData buffer pointer is given, pSizeOfData is the address of it's size (in bytes)
	// If pData is null, pSizeOfData is still given (to receive the required data size)

	// The VertexShader is parsed and converted back into the underlying declaration and it's size.
	// In any case, *pSizeOfData will be set to the declaration size.
	// If the pData is null, no further action it taken.
	// If the pData buffer pointer is given, but the given *pSizeOfData is smaller than the declaration size, an error is returned.
	// Otherwise, the declaration is copied into the pData buffer.

	HRESULT hRet = D3DERR_INVALIDCALL;

    if(pSizeOfData) {
        CxbxVertexShader *pCxbxVertexShader = GetCxbxVertexShader(Handle);
		if (pCxbxVertexShader) {
			if (*pSizeOfData < pCxbxVertexShader->XboxFunctionSize || !pData) {
				*pSizeOfData = pCxbxVertexShader->XboxFunctionSize;
				hRet = !pData ? D3D_OK : D3DERR_MOREDATA;
			}
			else {
				memcpy(pData, pCxbxVertexShader->pXboxFunctionCopy, pCxbxVertexShader->XboxFunctionSize);
				hRet = D3D_OK;
			}
		}
    }
    
    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_SetDepthClipPlanes
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetDepthClipPlanes)
(
    FLOAT Near,
    FLOAT Far,
    DWORD Flags
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Near)
		LOG_FUNC_ARG(Far)
		LOG_FUNC_ARG(Flags)
		LOG_FUNC_END;

    HRESULT hRet = D3D_OK;

    switch(Flags) // Member of X_D3DSET_DEPTH_CLIP_PLANES_FLAGS enum
    {
        case X_D3DSDCP_SET_VERTEXPROGRAM_PLANES:
        {
            // Sets the depth-clipping planes used whenever vertex shader programs are active
            // TODO

            // pDevice->fNear = Near
            // pDevice->fFar  = Far
        }
        break;

        case X_D3DSDCP_SET_FIXEDFUNCTION_PLANES:
        {
            // Sets the depth-clipping planes used whenever the fixed-function pipeline is in use. 
            // TODO

            // pDevice->fNear = Near
            // pDevice->fFar  = Far
        }
        break;

        case X_D3DSDCP_USE_DEFAULT_VERTEXPROGRAM_PLANES:
        {
            // Causes Direct3D to disregard the depth-clipping planes set when using X_D3DSDCP_SET_VERTEXPROGRAM_PLANE. 
            // Direct3D will resume using its own internally calculated clip planes when vertex shader programs are active. 
            // TODO
        }
        break;

        case X_D3DSDCP_USE_DEFAULT_FIXEDFUNCTION_PLANES:
        {
            // Causes Direct3D to disregard the depth-clipping planes set when using X_D3DSDCP_SET_FIXEDFUNCTION_PLANES. 
            // Direct3D will resume using its own internally calculated clip planes when the fixed-function pipeline is active.
            // TODO
        }
        break;

        default:
            EmuLog(LOG_LEVEL::WARNING, "Unknown SetDepthClipPlanes Flags provided");
    }

    // TODO

    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_InsertFence
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(D3DDevice_InsertFence)()
{
	LOG_FUNC();

    // TODO: Actually implement this
    DWORD dwRet = 0x8000BEEF;

	LOG_UNIMPLEMENTED();

    return dwRet;
}

// ******************************************************************
// * patch: D3DDevice_IsFencePending
// ******************************************************************
BOOL WINAPI XTL::EMUPATCH(D3DDevice_IsFencePending)
(
    DWORD Fence
)
{
	LOG_FUNC_ONE_ARG(Fence);

	// TODO: Implement
	LOG_UNIMPLEMENTED();

	return FALSE;
}

// ******************************************************************
// * patch: D3DDevice_BlockOnFence
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_BlockOnFence)
(
    DWORD Fence
)
{
	LOG_FUNC_ONE_ARG(Fence);

    // TODO: Implement
	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * patch: IDirect3DResource8_BlockUntilNotBusy
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DResource_BlockUntilNotBusy)
(
    X_D3DResource *pThis
)
{
	LOG_FUNC_ONE_ARG(pThis);

    // TODO: Implement
	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * patch: D3DDevice_SetScreenSpaceOffset
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetScreenSpaceOffset)
(
    FLOAT x,
    FLOAT y
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_END;

    // No need to log this, it's safe to ignore.
    //EmuLog(LOG_LEVEL::WARNING, "EmuD3DDevice_SetScreenSpaceOffset ignored");
}

// ******************************************************************
// * patch: D3DDevice_InsertCallback
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_InsertCallback)
(
	X_D3DCALLBACKTYPE	Type,
	X_D3DCALLBACK		pCallback,
	DWORD				Context
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Type)
		LOG_FUNC_ARG(pCallback)
		LOG_FUNC_ARG(Context)
		LOG_FUNC_END;

	CxbxImpl_InsertCallback(Type, pCallback, Context);

	LOG_INCOMPLETE();
}

// ******************************************************************
// * patch: D3DDevice_DrawRectPatch
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_DrawRectPatch)
(
	UINT					Handle,
	CONST FLOAT				*pNumSegs,
	CONST D3DRECTPATCH_INFO *pRectPatchInfo
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Handle)
		LOG_FUNC_ARG(pNumSegs)
		LOG_FUNC_ARG(pRectPatchInfo)
		LOG_FUNC_END;

	CxbxUpdateNativeD3DResources();

	HRESULT hRet = g_pD3DDevice->DrawRectPatch( Handle, pNumSegs, pRectPatchInfo );
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawRectPatch");

	return hRet;
}

// ******************************************************************
// * patch: D3DDevice_DrawTriPatch
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_DrawTriPatch)
(
	UINT					Handle,
	CONST FLOAT				*pNumSegs,
	CONST D3DTRIPATCH_INFO* pTriPatchInfo
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Handle)
		LOG_FUNC_ARG(pNumSegs)
		LOG_FUNC_ARG(pTriPatchInfo)
		LOG_FUNC_END;

	CxbxUpdateNativeD3DResources();

	HRESULT hRet = g_pD3DDevice->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawTriPatch");

	return hRet;
}

#pragma warning(disable:4244)
// ******************************************************************
// * patch: D3DDevice_GetProjectionViewportMatrix
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetProjectionViewportMatrix)
(
	D3DXMATRIX *pProjectionViewport
)
{
	LOG_FUNC_ONE_ARG(pProjectionViewport);

	// blueshogun96 1/25/10
	// It's been almost 3 years, but I think this is a better 
	// implementation.  Still probably not right, but better
	// then before.

	HRESULT hRet;
	D3DXMATRIX Out, mtxProjection, mtxViewport;
	D3DVIEWPORT Viewport;

	// Get current viewport
	hRet = g_pD3DDevice->GetViewport(&Viewport);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetViewport - Unable to get viewport!");

	// Get current projection matrix
	hRet = g_pD3DDevice->GetTransform(D3DTS_PROJECTION, &mtxProjection);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetTransform - Unable to get projection matrix!");

	// Clear the destination matrix
	::ZeroMemory(&Out, sizeof(D3DMATRIX));

	// Create the Viewport matrix manually
	// Direct3D8 doesn't give me everything I need in a viewport structure
	// (one thing I REALLY HATE!) so some constants will have to be used
	// instead.

	float ClipWidth = 2.0f;
	float ClipHeight = 2.0f;
	float ClipX = -1.0f;
	float ClipY = 1.0f;
	float Width = DWtoF(Viewport.Width);
	float Height = DWtoF(Viewport.Height);

	D3DXMatrixIdentity(&mtxViewport);
	mtxViewport._11 = Width / ClipWidth;
	mtxViewport._22 = -(Height / ClipHeight);
	mtxViewport._41 = -(ClipX * mtxViewport._11);
	mtxViewport._42 = -(ClipY * mtxViewport._22);

	// Multiply projection and viewport matrix together
	Out = mtxProjection * mtxViewport;

	*pProjectionViewport = Out;

//	__asm int 3;
}
#pragma warning(default:4244)

// ******************************************************************
// * patch: D3DDevice_SetStateVB (D3D::CDevice::SetStateVB)
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetStateVB)( ULONG Unknown1 )
{
	LOG_FUNC_ONE_ARG(Unknown1);

	// TODO: Anything?
//	__asm int 3;

	LOG_UNIMPLEMENTED();	
}

// ******************************************************************
// * patch: D3DDevice_SetStateUP (D3D::CDevice::SetStateUP)
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetStateUP)()
{
	LOG_FUNC();

	LOG_UNIMPLEMENTED();

	// TODO: Anything?
//	__asm int 3;
	
}

// ******************************************************************
// * patch: D3DDevice_SetStipple
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3DDevice_SetStipple)( DWORD* pPattern )
{
	LOG_FUNC_ONE_ARG(pPattern);

	// We need an OpenGL port... badly

	LOG_IGNORED();
}

// ******************************************************************
// * patch: D3DDevice_SetSwapCallback
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3DDevice_SetSwapCallback)
(
	X_D3DSWAPCALLBACK		pCallback
)
{
	LOG_FUNC_ONE_ARG(pCallback);

    g_pXbox_SwapCallback = pCallback;
}

// ******************************************************************
// * patch: D3DDevice_PersistDisplay
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_PersistDisplay)()
{
	LOG_FUNC();

	HRESULT hRet = D3D_OK;

	LOG_UNIMPLEMENTED();

	// TODO: This function simply saves a copy of the display to a surface and persists it in contiguous memory
	// This function, if ever required, can be implemented as the following
	// 1. Check for an existing persisted surface via AvGetSavedDataAddress, free it if necessary
	// 2. Create an Xbox format surface with the same size and format as active display
	// 3. Copy the host framebuffer to the xbox surface, converting format if necessary
	// 4. Set the display mode via AvSetDisplayMode to the same format as the persisted surface,
	//    passing the ->Data pointer of the xbox surface as the framebuffer pointer.
	// 5. Use MmPersistContigousMemory to persist the surface data across reboot
	// 6. Call AvSetSavedDataAddress, passing the xbox surface data pointer


	return hRet;
}

// ******************************************************************
// * patch: D3DDevice_PrimeVertexCache
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_PrimeVertexCache)
(
	UINT  VertexCount,
	WORD *pIndexData
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(VertexCount)
		LOG_FUNC_ARG(pIndexData)
		LOG_FUNC_END;

	// TODO: Implement
	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * patch: D3DDevice_SetModelView
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetModelView)
(
	CONST D3DMATRIX *pModelView, 
	CONST D3DMATRIX *pInverseModelView, 
	CONST D3DMATRIX *pComposite
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pModelView)
		LOG_FUNC_ARG(pInverseModelView)
		LOG_FUNC_ARG(pComposite)
		LOG_FUNC_END;

	// TODO: Implement
	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * patch: D3DDevice_FlushVertexCache
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3DDevice_FlushVertexCache)()
{
	LOG_FUNC();

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * patch: D3DDevice_GetModelView
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetModelView)(D3DXMATRIX* pModelView)
{
	LOG_FUNC_ONE_ARG(pModelView);

	D3DXMATRIX mtxWorld, mtxView;

	// I hope this is right
	g_pD3DDevice->GetTransform( D3DTS_WORLD, &mtxWorld );
	g_pD3DDevice->GetTransform( D3DTS_VIEW, &mtxView );

	*pModelView = mtxWorld * mtxView;

	return D3D_OK;
}


DWORD PushBuffer[64 * 1024 / sizeof(DWORD)];

// ******************************************************************
// * patch: D3D_SetCommonDebugRegisters
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3D_SetCommonDebugRegisters)()
{
	LOG_FUNC();

	// NOTE: I added this because I was too lazy to deal with emulating certain render
	// states that use it.  

	LOG_UNIMPLEMENTED();

}

// ******************************************************************
// * patch: D3DDevice_IsBusy
// ******************************************************************
BOOL WINAPI XTL::EMUPATCH(D3DDevice_IsBusy)()
{
		LOG_FUNC();

	// NOTE: This function returns FALSE when the NV2A FIFO is empty/complete, or NV_PGRAPH_STATUS = 0
	// Otherwise, it returns true.

	return FALSE;
}

// ******************************************************************
// * patch: D3D_BlockOnTime
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3D_BlockOnTime)( DWORD Unknown1, int Unknown2 )
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Unknown1)
		LOG_FUNC_ARG(Unknown2)
		LOG_FUNC_END;

	// NOTE: This function is not meant to be emulated.  Just use it to find out
	// the function that is calling it, and emulate that instead!!!  If necessary,
	// create an XRef...

	//__asm int 3;

	LOG_UNIMPLEMENTED();
}

bool DestroyResource_Common(XTL::X_D3DResource* pResource)
{
    if (pResource == g_pXbox_RenderTarget) {
        LOG_TEST_CASE("Skipping Release of active Xbox Render Target");
        return false;
    }

    if (pResource == g_pXbox_DepthStencil) {
        LOG_TEST_CASE("Skipping Release of active Xbox Depth Stencil");
        return false;
    }

    if (pResource == g_pXbox_BackBufferSurface) {
        LOG_TEST_CASE("Skipping Release of active Xbox BackBuffer");
        return false;
    }

    if (pResource == g_pXbox_DefaultDepthStencilSurface) {
        LOG_TEST_CASE("Skipping Release of default Xbox Depth Stencil");
        return false;
    }

    for (int i = 0; i < XTL::X_D3DTS_STAGECOUNT; i++) {
        if (pResource == g_pXbox_SetTexture[i]) {
            // This shouldn't happen, since texture resources that get destroyed,
            // shouldn't be set to any stage anymore.
            LOG_TEST_CASE("Skipping Release of active Xbox Texture");
            return false;
        }
    }

    // Release the host copy (if it exists!)
    FreeHostResource(GetHostResourceKey(pResource));

    return true;
}

// ******************************************************************
// * patch: D3D_DestroyResource
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3D_DestroyResource)(X_D3DResource* pResource)
{
    LOG_FUNC_ONE_ARG(pResource);

    if (DestroyResource_Common(pResource)) {
        // Call the Xbox version of DestroyResource
        XB_trampoline(void, WINAPI, D3D_DestroyResource, (X_D3DResource*));
        XB_D3D_DestroyResource(pResource);
    }
}

// ******************************************************************
// * patch: D3D_DestroyResource_LTCG
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3D_DestroyResource__LTCG)()
{
    X_D3DResource* pResource;
    __asm {
        mov pResource, edi
    }

    if (DestroyResource_Common(pResource)) {
        // Call the Xbox version of DestroyResource
        XB_trampoline(void, WINAPI, D3D_DestroyResource__LTCG, (VOID));
        __asm {
            mov edi, pResource
            call XB_D3D_DestroyResource__LTCG
        }
    }
}


// ******************************************************************
// * patch: D3DDevice_SetRenderTargetFast
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderTargetFast)
(
    X_D3DSurface	*pRenderTarget,
    X_D3DSurface	*pNewZStencil,
    DWORD			Flags
)
{
	LOG_FORWARD("D3DDevice_SetRenderTarget");

	// Redirect to the standard version.
	
	EMUPATCH(D3DDevice_SetRenderTarget)(pRenderTarget, pNewZStencil);
}

// ******************************************************************
// * patch: D3D::LazySetPointParams
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3D_LazySetPointParams)
(
	void* Device
)
{
	LOG_FUNC_ONE_ARG(Device);

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * patch: D3DDevice_GetMaterial
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetMaterial)
(
	X_D3DMATERIAL8* pMaterial
)
{
	LOG_FUNC_ONE_ARG(pMaterial);

	HRESULT hRet = D3D_OK;

	if (pMaterial)
	{
		hRet = g_pD3DDevice->GetMaterial(pMaterial);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetMaterial");
	}

	if(FAILED(hRet))
    {
		EmuLog(LOG_LEVEL::WARNING, "We're lying about getting a material!");
        hRet = D3D_OK;
    }
}

// LTCG specific D3DDevice_SetPixelShaderConstant function...
// This uses a custom calling convention where parameter is passed in ECX, EAX
// TODO: Log function is not working due lost parameter in EAX.
// Test-case: Otogi 2, Ninja Gaiden: Black
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetPixelShaderConstant_4)
(
    CONST PVOID pConstantData
)
{
    DWORD       Register;
    DWORD       ConstantCount;

    __asm {
        mov Register, ecx
        mov ConstantCount, eax
    }

    //LOG_FUNC_BEGIN
    //    LOG_FUNC_ARG(Register)
    //    LOG_FUNC_ARG(pConstantData)
    //    LOG_FUNC_ARG(ConstantCount)
    //    LOG_FUNC_END;
    EmuLog(LOG_LEVEL::DEBUG, "D3DDevice_SetPixelShaderConstant_4(Register : %d pConstantData : %08X ConstantCount : %d);", Register, pConstantData, ConstantCount);

	HRESULT hRet = g_pD3DDevice->SetPixelShaderConstantF
    (
        Register,
		(PixelShaderConstantType*)pConstantData,
        ConstantCount
    );
    //DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetPixelShaderConstant");

    if(FAILED(hRet))
    {
        EmuLog(LOG_LEVEL::WARNING, "We're lying about setting a pixel shader constant!");

        hRet = D3D_OK;
    }
}
