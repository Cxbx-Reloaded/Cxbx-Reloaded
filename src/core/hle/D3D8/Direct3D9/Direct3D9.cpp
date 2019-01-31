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
#define _XBOXKRNL_DEFEXTRN_
#define LOG_PREFIX CXBXR_MODULE::D3D8

#include "common\util\xxhash32.h"
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
#include "core\kernel\support\EmuFS.h"
#include "EmuShared.h"
#include "gui\DbgConsole.h"
#include "core\hle\D3D8\ResourceTracker.h"
#include "core\kernel\memory-manager\VMManager.h" // for g_VMManager
#include "core\kernel\support\EmuXTL.h"
#include "Logging.h"
#include "..\XbD3D8Logging.h"
#include "core\hle\Intercept.hpp" // for bLLE_GPU
#include "devices\video\nv2a.h" // For GET_MASK, NV_PGRAPH_CONTROL_0
#include "gui\ResCxbx.h"
#include "WalkIndexBuffer.h"

#include <assert.h>
#include <process.h>
#include <clocale>
#include <unordered_map>

// Allow use of time duration literals (making 16ms, etc possible)
using namespace std::literals::chrono_literals;

// Global(s)
HWND                                g_hEmuWindow   = NULL; // rendering window
XTL::IDirect3DDevice               *g_pD3DDevice   = nullptr; // Direct3D Device
XTL::LPDIRECTDRAWSURFACE7           g_pDDSPrimary  = NULL; // DirectDraw7 Primary Surface
XTL::LPDIRECTDRAWCLIPPER            g_pDDClipper   = nullptr; // DirectDraw7 Clipper
DWORD                               g_CurrentXboxVertexShaderHandle = 0;
XTL::X_PixelShader*					g_D3DActivePixelShader = nullptr;
BOOL                                g_bIsFauxFullscreen = FALSE;
DWORD								g_OverlaySwap = 0;
DWORD								g_CurrentFillMode = XTL::D3DFILL_SOLID;	// Used to backup/restore the fill mode when WireFrame is enabled

// Static Function(s)
static BOOL WINAPI                  EmuEnumDisplayDevices(GUID FAR *lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext, HMONITOR hm);
static DWORD WINAPI                 EmuRenderWindow(LPVOID);
static DWORD WINAPI                 EmuCreateDeviceProxy(LPVOID);
static LRESULT WINAPI               EmuMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static DWORD WINAPI                 EmuUpdateTickCount(LPVOID);
static inline void                  EmuVerifyResourceIsRegistered(XTL::X_D3DResource *pResource, DWORD D3DUsage, int iTextureStage, DWORD dwSize);
static void							UpdateCurrentMSpFAndFPS(); // Used for benchmarking/fps count

// Static Variable(s)
static HMONITOR                     g_hMonitor      = NULL; // Handle to DirectDraw monitor
static bool                         g_bSupportsFormatSurface[XTL::X_D3DFMT_LIN_R8G8B8A8 + 1] = { false };// Does device support surface format?
static bool                         g_bSupportsFormatSurfaceRenderTarget[XTL::X_D3DFMT_LIN_R8G8B8A8 + 1] = { false };// Does device support surface format?
static bool                         g_bSupportsFormatSurfaceDepthStencil[XTL::X_D3DFMT_LIN_R8G8B8A8 + 1] = { false };// Does device support surface format?
static bool                         g_bSupportsFormatTexture[XTL::X_D3DFMT_LIN_R8G8B8A8 + 1] = { false }; // Does device support texture format?
static bool                         g_bSupportsFormatTextureRenderTarget[XTL::X_D3DFMT_LIN_R8G8B8A8 + 1] = { false };// Does device support texture format?
static bool                         g_bSupportsFormatTextureDepthStencil[XTL::X_D3DFMT_LIN_R8G8B8A8 + 1] = { false };// Does device support texture format?
static bool                         g_bSupportsFormatVolumeTexture[XTL::X_D3DFMT_LIN_R8G8B8A8 + 1] = { false }; // Does device support surface format?
static bool                         g_bSupportsFormatCubeTexture[XTL::X_D3DFMT_LIN_R8G8B8A8 + 1] = { false }; // Does device support surface format?
static XTL::LPDIRECTDRAW7           g_pDD7          = NULL; // DirectDraw7
static XTL::DDCAPS                  g_DriverCaps          = { 0 };
static HBRUSH                       g_hBgBrush      = NULL; // Background Brush
static volatile bool                g_bRenderWindowActive = false;
static Settings::s_video            g_XBVideo;
static XTL::D3DVBLANKCALLBACK       g_pVBCallback   = NULL; // Vertical-Blank callback routine
static std::condition_variable		g_VBConditionVariable;	// Used in BlockUntilVerticalBlank
static std::mutex					g_VBConditionMutex;		// Used in BlockUntilVerticalBlank
static XTL::D3DSWAPCALLBACK			g_pSwapCallback = NULL;	// Swap/Present callback routine
static XTL::D3DCALLBACK				g_pCallback		= NULL;	// D3DDevice::InsertCallback routine
static XTL::X_D3DCALLBACKTYPE		g_CallbackType;			// Callback type
static DWORD						g_CallbackParam;		// Callback param
static bool                         g_bHasDepth = false;    // Does device have a Depth Buffer?
static bool                         g_bHasStencil = false;  // Does device have a Stencil Buffer?
static DWORD						g_dwPrimPerFrame = 0;	// Number of primitives within one frame

// primary push buffer
static uint32  g_dwPrimaryPBCount = 0;
static uint32 *g_pPrimaryPB = nullptr;


struct {
	XTL::X_D3DSurface Surface;
	RECT SrcRect;
	RECT DstRect;
	BOOL EnableColorKey;
	XTL::D3DCOLOR ColorKey;
} g_OverlayProxy;

// D3D based variables
static GUID                         g_ddguid;               // DirectDraw driver GUID
static XTL::IDirect3D              *g_pDirect3D = nullptr;
XTL::D3DCAPS					    g_D3DCaps = {};         // Direct3D Caps

// wireframe toggle
static int                          g_iWireframe    = 0;

// build version
extern uint32						g_BuildVersion;

typedef uint64_t resource_key_t;

extern void UpdateFPSCounter();

// current active index buffer
static DWORD                        g_dwBaseVertexIndex = 0;// current active index buffer base index

// current active vertex stream
static XTL::X_D3DVertexBuffer      *g_pVertexBuffer = NULL; // current active vertex buffer
static XTL::IDirect3DVertexBuffer  *g_pDummyBuffer = NULL;  // Dummy buffer, used to set unused stream sources with
static DWORD						g_dwLastSetStream = 0;	// The last stream set by D3DDevice::SetStreamSource

// current vertical blank information
static XTL::D3DVBLANKDATA			g_VBData = {0};
static DWORD                        g_VBLastSwap = 0;

// current swap information
static XTL::D3DSWAPDATA				g_SwapData = {0};
static DWORD						g_SwapLast = 0;

// cached Direct3D state variable(s)
static XTL::IDirect3DIndexBuffer   *pClosingLineLoopIndexBuffer = nullptr;

static XTL::IDirect3DIndexBuffer   *pQuadToTriangleD3DIndexBuffer = nullptr;
static UINT                         QuadToTriangleD3DIndexBuffer_Size = 0; // = NrOfQuadVertices

static XTL::INDEX16                *pQuadToTriangleIndexBuffer = nullptr;
static UINT                         QuadToTriangleIndexBuffer_Size = 0; // = NrOfQuadVertices

static XTL::IDirect3DSurface	   *g_DefaultHostDepthBufferSuface = NULL;
static XTL::X_D3DSurface		   *g_XboxBackBufferSurface = NULL;
static XTL::X_D3DSurface           *g_pXboxRenderTarget = NULL;
static XTL::X_D3DSurface           *g_pXboxDepthStencil = NULL;
static DWORD                        g_dwVertexShaderUsage = 0;
static DWORD                        g_VertexShaderSlots[136];

DWORD g_XboxBaseVertexIndex = 0;


// Active D3D Vertex Streams (and strides)
XTL::X_D3DVertexBuffer*g_D3DStreams[16];
UINT g_D3DStreamStrides[16];

// cached palette pointer
static PVOID g_pCurrentPalette[TEXTURE_STAGES] = { nullptr, nullptr, nullptr, nullptr };

static XTL::X_VERTEXSHADERCONSTANTMODE g_VertexShaderConstantMode = X_D3DSCM_192CONSTANTS;

// cached Direct3D tiles
XTL::X_D3DTILE XTL::EmuD3DTileCache[0x08] = {0};

// cached active texture
XTL::X_D3DBaseTexture *XTL::EmuD3DActiveTexture[TEXTURE_STAGES] = {0,0,0,0};
XTL::X_D3DBaseTexture CxbxActiveTextureCopies[TEXTURE_STAGES] = {};

// information passed to the create device proxy thread
struct EmuD3D8CreateDeviceProxyData
{
	// Set by EmuD3DInit()
    XTL::UINT                        Adapter;
    XTL::D3DDEVTYPE                  DeviceType;
    HWND                             hFocusWindow;
	// Set byt EMUPATCH(Direct3D_CreateDevice)
    XTL::X_D3DPRESENT_PARAMETERS     XboxPresentationParameters;
    volatile bool                    bReady;
    volatile bool                    bCreate;   // false : release
	// Set by EmuCreateDeviceProxy()
    XTL::DWORD                       BehaviorFlags;
	XTL::D3DPRESENT_PARAMETERS       HostPresentationParameters;
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


// TODO: This should be a D3DDevice structure
DWORD* g_XboxD3DDevice;

const char *CxbxGetErrorDescription(HRESULT hResult)
{
	// TODO : For D3D9, Use DXGetErrorDescription9(hResult) (requires another DLL though)
	// See : http://www.fairyengine.com/articles/dxmultiviews.htm
	// and : http://www.gamedev.net/community/forums/showfaq.asp?forum_id=10
	// and : http://www.gamedev.net/community/forums/topic.asp?topic_id=16157
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

const char *D3DErrorString(HRESULT hResult)
{
	using namespace XTL;

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

VOID XTL::CxbxInitWindow(bool bFullInit)
{
    g_EmuShared->GetVideoSettings(&g_XBVideo);

    if(g_XBVideo.bFullScreen)
        CxbxKrnl_hEmuParent = NULL;

    // create timing thread
	if (bFullInit)
    {
        DWORD dwThreadId;

        HANDLE hThread = CreateThread(NULL, NULL, EmuUpdateTickCount, NULL, NULL, &dwThreadId);
        // We set the priority of this thread a bit higher, to assure reliable timing :
        SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);

        CxbxKrnlRegisterThread(hThread);
    }

/* TODO : Port this Dxbx code :
  // create vblank handling thread
    {
        dwThreadId = 0;
        {hThread :=} CreateThread(NULL, 0, EmuThreadHandleVBlank, NULL, 0, &dwThreadId);
    }
*/
    // create window message processing thread
    {
        DWORD dwThreadId;

        g_bRenderWindowActive = false;

        HANDLE hRenderWindowThread = CreateThread(NULL, NULL, EmuRenderWindow, NULL, NULL, &dwThreadId);

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
	// See http://xboxdevwiki.net/Fatal_Error
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

	wchar_t buff[500];
	LoadStringW(GetModuleHandle(NULL), IDS_UEM, buff, sizeof(buff) / sizeof(wchar_t));
	std::wstring wstr(buff);

	// Unfortunately, DrawTextW doesn't support vertical alignemnt, so we have to do the calculation
	// ourselves. See here: https://social.msdn.microsoft.com/Forums/vstudio/en-US/abd89aae-16a0-41c6-8db6-b119ea90b42a/win32-drawtext-how-center-in-vertical-with-new-lines-and-tabs?forum=vclanguage

	RECT rect = { 0, 0, 640, 480 };
	RECT textrect = { 0, 0, 640, 480 };
	DrawTextW(hMemDC, wstr.c_str(), wstr.length(), &textrect, DT_CALCRECT);
	rect.top = (rect.bottom - textrect.bottom) / 2;
	DrawTextW(hMemDC, wstr.c_str(), wstr.length(), &rect, DT_CENTER);


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

inline DWORD GetXboxCommonResourceType(const XTL::X_D3DResource *pXboxResource)
{
	// Don't pass in unassigned Xbox resources
	assert(pXboxResource != NULL);

	DWORD dwCommonType = pXboxResource->Common & X_D3DCOMMON_TYPE_MASK;
	return dwCommonType;
}

inline XTL::X_D3DFORMAT GetXboxPixelContainerFormat(const XTL::X_D3DPixelContainer *pXboxPixelContainer)
{
	// Don't pass in unassigned Xbox pixel container
	assert(pXboxPixelContainer != NULL);

	return (XTL::X_D3DFORMAT)((pXboxPixelContainer->Format & X_D3DFORMAT_FORMAT_MASK) >> X_D3DFORMAT_FORMAT_SHIFT);
}

inline int GetXboxPixelContainerDimensionCount(const XTL::X_D3DPixelContainer *pXboxPixelContainer)
{
	// Don't pass in unassigned Xbox pixel container
	assert(pXboxPixelContainer != NULL);

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

inline bool IsYuvSurfaceOrTexture(const XTL::X_D3DResource *pXboxResource)
{
	if (GetXboxPixelContainerFormat((XTL::X_D3DPixelContainer *)pXboxResource) == XTL::X_D3DFMT_YUY2)
		return true;

	return false;
}

inline bool IsXboxResourceLocked(const XTL::X_D3DResource *pXboxResource)
{
	bool result = !!(pXboxResource->Common & X_D3DCOMMON_ISLOCKED);
	return result;
}

inline bool IsXboxResourceD3DCreated(const XTL::X_D3DResource *pXboxResource)
{
	bool result = !!(pXboxResource->Common & X_D3DCOMMON_D3DCREATED);
	return result;
}

void *GetDataFromXboxResource(XTL::X_D3DResource *pXboxResource)
{
	// Don't pass in unassigned Xbox resources
	if (pXboxResource == NULL)
		return nullptr;

	xbaddr pData = pXboxResource->Data;
	if (pData == NULL)
		return nullptr;

	DWORD dwCommonType = GetXboxCommonResourceType(pXboxResource);
	if (IsResourceTypeGPUReadable(dwCommonType))
		pData |= PHYSICAL_MAP_BASE;

	return (uint08*)pData;
}

typedef struct {
	XTL::IDirect3DResource* pHostResource = nullptr;
	XTL::X_D3DResource* pXboxResource = nullptr;
	DWORD dwXboxResourceType = 0;
	void* pXboxData = nullptr;
	size_t szXboxDataSize = 0;
	uint32_t hash = 0;
	bool forceRehash = false;
	std::chrono::time_point<std::chrono::high_resolution_clock> nextHashTime;
	std::chrono::milliseconds hashLifeTime = 1ms;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastUpdate;
} resource_info_t;

std::unordered_map <resource_key_t, resource_info_t> g_XboxDirect3DResources;

bool IsResourceAPixelContainer(XTL::X_D3DResource* pXboxResource)
{
	// assert(pXboxResource);
	DWORD Type = GetXboxCommonResourceType(pXboxResource);
	switch (Type)
	{
	case X_D3DCOMMON_TYPE_TEXTURE:
	case X_D3DCOMMON_TYPE_SURFACE:
		return true;
	}

	return false;
}

resource_key_t GetHostResourceKey(XTL::X_D3DResource* pXboxResource)
{
	resource_key_t key = 0;
	if (pXboxResource != xbnullptr) {
		// Initially, don't base the key on the address of the resource, but on it's uniquely identifying values
		key ^= pXboxResource->Data;
		key ^= (pXboxResource->Common & X_D3DCOMMON_TYPE_MASK) >> X_D3DCOMMON_TYPE_SHIFT;
		if (IsResourceAPixelContainer(pXboxResource)) {
			// Pixel containers have more values they are be identified by:
			key ^= ((uint64_t)((XTL::X_D3DPixelContainer *)pXboxResource)->Format) << 24;
			key ^= ((uint64_t)((XTL::X_D3DPixelContainer *)pXboxResource)->Size) << 32;
		}
		else {
			// For other resource types, do include their Xbox resource address (TODO : come up with something better)
			key ^= ((uint64_t)pXboxResource) << 32;
		}
	}

	return key;
}

void FreeHostResource(resource_key_t key)
{
	// Release the host resource and remove it from the list
	auto hostResourceIterator = g_XboxDirect3DResources.find(key);
	if (hostResourceIterator != g_XboxDirect3DResources.end()) {
		if (hostResourceIterator->second.pHostResource) {
			(hostResourceIterator->second.pHostResource)->Release();
		}

		g_XboxDirect3DResources.erase(hostResourceIterator);
	}
}

void ForceResourceRehash(XTL::X_D3DResource* pXboxResource)
{
	auto key = GetHostResourceKey(pXboxResource);
	auto it = g_XboxDirect3DResources.find(key);
	if (it != g_XboxDirect3DResources.end() && it->second.pHostResource) {
		it->second.forceRehash = true;
	}
}

XTL::IDirect3DResource *GetHostResource(XTL::X_D3DResource *pXboxResource, DWORD D3DUsage = 0, int iTextureStage = 0)
{
	if (pXboxResource == NULL || pXboxResource->Data == NULL)
		return nullptr;

	EmuVerifyResourceIsRegistered(pXboxResource, D3DUsage, iTextureStage, /*dwSize=*/0);

	auto key = GetHostResourceKey(pXboxResource);
	auto it = g_XboxDirect3DResources.find(key);
	if (it == g_XboxDirect3DResources.end() || !it->second.pHostResource) {
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
	switch (GetXboxCommonResourceType(pXboxResource)) {
	case X_D3DCOMMON_TYPE_SURFACE:
	case X_D3DCOMMON_TYPE_TEXTURE:
		uint Width, Height, Depth, RowPitch, SlicePitch;
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
	default:
		// Fallback to querying the allocation size, if no other calculation was present
		return xboxkrnl::MmQueryAllocationSize(GetDataFromXboxResource(pXboxResource));
	}
	
}

bool HostResourceRequiresUpdate(resource_key_t key, DWORD dwSize)
{
	auto it = g_XboxDirect3DResources.find(key);
	if (it == g_XboxDirect3DResources.end() || !it->second.pXboxResource) {
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
		uint32_t oldHash = it->second.hash;
		it->second.hash = XXHash32::hash(it->second.pXboxData, it->second.szXboxDataSize, 0);

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

void SetHostResource(XTL::X_D3DResource* pXboxResource, XTL::IDirect3DResource* pHostResource, DWORD dwSize = 0)
{
	auto key = GetHostResourceKey(pXboxResource);
	auto& resourceInfo = g_XboxDirect3DResources[key];	// Implicitely inserts a new entry if not already existing

	if (resourceInfo.pHostResource) {
		EmuLog(LOG_LEVEL::WARNING, "SetHostResource: Overwriting an existing host resource");
	}

	resourceInfo.pHostResource = pHostResource;
	resourceInfo.pXboxResource = pXboxResource;
	resourceInfo.dwXboxResourceType = GetXboxCommonResourceType(pXboxResource);
	resourceInfo.pXboxData = GetDataFromXboxResource(pXboxResource);
	resourceInfo.szXboxDataSize = dwSize > 0 ? dwSize : GetXboxResourceSize(pXboxResource);
	resourceInfo.hash = XXHash32::hash(resourceInfo.pXboxData, resourceInfo.szXboxDataSize, 0);
	resourceInfo.hashLifeTime = 1ms;
	resourceInfo.lastUpdate = std::chrono::high_resolution_clock::now();
	resourceInfo.nextHashTime = resourceInfo.lastUpdate + resourceInfo.hashLifeTime;
	resourceInfo.forceRehash = false;
}

XTL::IDirect3DSurface *GetHostSurface(XTL::X_D3DResource *pXboxResource, DWORD D3DUsage = 0)
{
	if (pXboxResource == NULL)
		return nullptr;

	if (GetXboxCommonResourceType(pXboxResource) != X_D3DCOMMON_TYPE_SURFACE) // Allows breakpoint below
		assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_SURFACE);

	return (XTL::IDirect3DSurface*) GetHostResource(pXboxResource, D3DUsage);
}

XTL::IDirect3DBaseTexture *GetHostBaseTexture(XTL::X_D3DResource *pXboxResource, DWORD D3DUsage = 0, int iTextureStage = 0)
{
	if (pXboxResource == NULL)
		return nullptr;

	if (GetXboxCommonResourceType(pXboxResource) != X_D3DCOMMON_TYPE_TEXTURE) { // Allows breakpoint below
		; // TODO : Log instead of assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_TEXTURE);
		// Burnout hits this case (retreiving a surface instead of a texture)
	}

	return (XTL::IDirect3DBaseTexture*)GetHostResource(pXboxResource, D3DUsage, iTextureStage);
}

XTL::IDirect3DTexture *GetHostTexture(XTL::X_D3DResource *pXboxResource, int iTextureStage = 0)
{
	return (XTL::IDirect3DTexture *)GetHostBaseTexture(pXboxResource, 0, iTextureStage);

	// TODO : Check for 1 face (and 2 dimensions)?
}

XTL::IDirect3DVolumeTexture *GetHostVolumeTexture(XTL::X_D3DResource *pXboxResource, int iTextureStage = 0)
{
	return (XTL::IDirect3DVolumeTexture *)GetHostBaseTexture(pXboxResource, 0, iTextureStage);

	// TODO : Check for 1 face (and 2 dimensions)?
}

XTL::IDirect3DIndexBuffer *GetHostIndexBuffer(XTL::X_D3DResource *pXboxResource)
{
	if (pXboxResource == NULL)
		return nullptr;

	assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_INDEXBUFFER);

	return (XTL::IDirect3DIndexBuffer*)GetHostResource(pXboxResource);
}

void SetHostSurface(XTL::X_D3DResource *pXboxResource, XTL::IDirect3DSurface *pHostSurface)
{
	assert(pXboxResource != NULL);
	assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_SURFACE);

	SetHostResource(pXboxResource, (XTL::IDirect3DResource*)pHostSurface);
}

void SetHostTexture(XTL::X_D3DResource *pXboxResource, XTL::IDirect3DTexture *pHostTexture)
{
	assert(pXboxResource != NULL);
	assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_TEXTURE);

	SetHostResource(pXboxResource, (XTL::IDirect3DResource*)pHostTexture);
}

void SetHostCubeTexture(XTL::X_D3DResource *pXboxResource, XTL::IDirect3DCubeTexture *pHostCubeTexture)
{
	assert(pXboxResource != NULL);
	assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_TEXTURE);

	SetHostResource(pXboxResource, (XTL::IDirect3DResource*)pHostCubeTexture);
}

void SetHostVolumeTexture(XTL::X_D3DResource *pXboxResource, XTL::IDirect3DVolumeTexture *pHostVolumeTexture)
{
	assert(pXboxResource != NULL);
	assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_TEXTURE);

	SetHostResource(pXboxResource, (XTL::IDirect3DResource*)pHostVolumeTexture);
}

void SetHostVolume(XTL::X_D3DResource *pXboxResource, XTL::IDirect3DVolume *pHostVolume)
{
	assert(pXboxResource != NULL);
	assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_TEXTURE);

	SetHostResource(pXboxResource, (XTL::IDirect3DResource*)pHostVolume);
}

void SetHostIndexBuffer(XTL::X_D3DResource *pXboxResource, XTL::IDirect3DIndexBuffer *pHostIndexBuffer)
{
	assert(pXboxResource != NULL);
	assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_INDEXBUFFER);

	SetHostResource(pXboxResource, (XTL::IDirect3DResource*)pHostIndexBuffer);
}

int XboxD3DPaletteSizeToBytes(const XTL::X_D3DPALETTESIZE Size)
{
	static int lk[4] =
	{
		256 * sizeof(XTL::D3DCOLOR),    // D3DPALETTE_256
		128 * sizeof(XTL::D3DCOLOR),    // D3DPALETTE_128
		64 * sizeof(XTL::D3DCOLOR),     // D3DPALETTE_64
		32 * sizeof(XTL::D3DCOLOR)      // D3DPALETTE_32
	};

	return lk[Size];
}

inline XTL::X_D3DPALETTESIZE GetXboxPaletteSize(const XTL::X_D3DPalette *pPalette)
{
	XTL::X_D3DPALETTESIZE PaletteSize = (XTL::X_D3DPALETTESIZE)
		((pPalette->Common & X_D3DPALETTE_COMMON_PALETTESIZE_MASK) >> X_D3DPALETTE_COMMON_PALETTESIZE_SHIFT);

	return PaletteSize;
}

int GetD3DResourceRefCount(XTL::IDirect3DResource *EmuResource)
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

VOID XTL::CxbxSetPixelContainerHeader
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

uint CxbxGetPixelContainerDepth
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

uint CxbxGetPixelContainerMipMapLevels
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

uint32_t GetPixelContainerHeigth(XTL::X_D3DPixelContainer *pPixelContainer)
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

bool ConvertD3DTextureToARGBBuffer(
	XTL::X_D3DFORMAT X_Format,
	uint8_t *pSrc,
	int SrcWidth, int SrcHeight, int SrcRowPitch, int SrcSlicePitch,
	uint8_t *pDst, int DstRowPitch, int DstSlicePitch,
	uint uiDepth = 1,
	int iTextureStage = 0
)
{
	const XTL::FormatToARGBRow ConvertRowToARGB = EmuXBFormatComponentConverter(X_Format);
	if (ConvertRowToARGB == nullptr)
		return false; // Unhandled conversion

	uint8_t *unswizleBuffer = nullptr;
	if (XTL::EmuXBFormatIsSwizzled(X_Format)) {
		unswizleBuffer = (uint8_t*)malloc(SrcSlicePitch * uiDepth); // TODO : Reuse buffer when performance is important
		// First we need to unswizzle the texture data
		XTL::EmuUnswizzleBox(
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
		AdditionalArgument = (int)g_pCurrentPalette[iTextureStage];
	else
		AdditionalArgument = DstRowPitch;

	if (EmuXBFormatIsCompressed(X_Format)) {
		// All compressed formats (DXT1, DXT3 and DXT5) encode blocks of 4 pixels on 4 lines
		SrcHeight = (SrcHeight + 3) / 4;
		DstRowPitch *= 4;
	}

	uint8_t *pSrcSlice = pSrc;
	uint8_t *pDstSlice = pDst;
	for (uint z = 0; z < uiDepth; z++) {
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
uint8 *XTL::ConvertD3DTextureToARGB(
	XTL::X_D3DPixelContainer *pXboxPixelContainer,
	uint8 *pSrc,
	int *pWidth, int *pHeight,
	int TextureStage // default = 0
)
{
	// Avoid allocating pDest when ConvertD3DTextureToARGBBuffer will fail anyway
	XTL::X_D3DFORMAT X_Format = GetXboxPixelContainerFormat(pXboxPixelContainer);
	const XTL::FormatToARGBRow ConvertRowToARGB = EmuXBFormatComponentConverter(X_Format);
	if (ConvertRowToARGB == nullptr)
		return nullptr; // Unhandled conversion

	uint SrcDepth, SrcRowPitch, SrcSlicePitch;
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
	uint8 *pDst = (uint8 *)malloc(DstSize);

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
VOID XTL::EmuD3DInit()
{
	// create the create device proxy thread
	{
		DWORD dwThreadId;

		CreateThread(NULL, NULL, EmuCreateDeviceProxy, NULL, NULL, &dwThreadId);
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
        if(g_pDirect3D == NULL)
            CxbxKrnlCleanup("Could not initialize Direct3D8!");

        g_pDirect3D->GetDeviceCaps(g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType, &g_D3DCaps);

		// Dump Host D3DCaps to log unconditionally
		std::cout << "----------------------------------------\n";
		std::cout << "Host D3DCaps : " << g_D3DCaps << "\n";
		std::cout << "----------------------------------------\n";
	}
}

// cleanup Direct3D
VOID XTL::EmuD3DCleanup()
{
    XTL::EmuDInputCleanup();
}

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
            0, 0, hActiveModule, // Was GetModuleHandle(NULL),
			0, // TODO : LoadIcon(hmodule, ?)
            LoadCursor(NULL, IDC_ARROW),
            (HBRUSH)(g_hBgBrush), NULL,
            "CxbxRender",
            NULL
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
			hwndParent, NULL, hActiveModule, // Was GetModuleHandle(NULL),
			NULL
        );
    }

    ShowWindow(g_hEmuWindow, ((CxbxKrnl_hEmuParent == 0) || g_XBVideo.bFullScreen) ? SW_SHOWDEFAULT : SW_SHOWMAXIMIZED);
    UpdateWindow(g_hEmuWindow);

    if(!g_XBVideo.bFullScreen && (CxbxKrnl_hEmuParent != NULL))
    {
        SetFocus(CxbxKrnl_hEmuParent);
    }

    // initialize direct input only if LLE USB is off
	if (!bLLE_USB) {
		if (!XTL::EmuDInputInit()) {
			CxbxKrnlCleanup("Could not initialize DirectInput!");
		}
	}

    DBG_PRINTF("Message-Pump thread is running.\n");

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

        CxbxKrnlCleanup(NULL);
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
            else if(wParam == VK_F11)
            {
                if(g_iWireframe++ == 2)
                    g_iWireframe = 0;
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
                        CxbxKrnlCleanup(NULL);

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

    DBG_PRINTF("Timing thread is running.\n");

    // current vertical blank count
    int curvb = 0;

	// Calculate Next VBlank time
	auto nextVBlankTime = GetNextVBlankTime();

    while(true)
    {
		SwitchToThread();
        //
        // Poll input
        //
        int port;
        for (port = 0; port < 4;port++) {
            extern XTL::X_CONTROLLER_HOST_BRIDGE g_XboxControllerHostBridge[4];
            if (g_XboxControllerHostBridge[port].hXboxDevice == 0)
                continue;
            if (g_XboxControllerHostBridge[port].pXboxFeedbackHeader == 0)
                continue;
            DWORD dwLatency = g_XboxControllerHostBridge[port].dwLatency++;

            if (dwLatency < XINPUT_SETSTATE_LATENCY)
                continue;

            g_XboxControllerHostBridge[port].dwLatency = 0;

            if (g_XboxControllerHostBridge[port].pXboxFeedbackHeader->dwStatus != ERROR_SUCCESS)
            {
                if (g_XboxControllerHostBridge[port].pXboxFeedbackHeader->hEvent != 0)
                {
                    SetEvent(g_XboxControllerHostBridge[port].pXboxFeedbackHeader->hEvent);
                }

                g_XboxControllerHostBridge[port].pXboxFeedbackHeader->dwStatus = ERROR_SUCCESS;
            }

        }

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
			g_VBData.VBlank++;
			g_VBConditionVariable.notify_all();

			// TODO: Fixme.  This may not be right...
			g_SwapData.SwapVBlank = 1;

            if(g_pVBCallback != NULL)
            {
                    
                g_pVBCallback(&g_VBData);
                    
            }

            g_VBData.Swap = 0;

			// TODO: This can't be accurate...
			g_SwapData.TimeUntilSwapVBlank = 0;

			// TODO: Recalculate this for PAL version if necessary.
			// Also, we should check the D3DPRESENT_INTERVAL value for accurracy.
		//	g_SwapData.TimeBetweenSwapVBlanks = 1/60;
			g_SwapData.TimeBetweenSwapVBlanks = 0;
        }
    }
}

void UpdateDepthStencilFlags(XTL::IDirect3DSurface *pDepthStencilSurface)
{
	g_bHasDepth = false;
	g_bHasStencil = false;
	if (pDepthStencilSurface != nullptr) {
		using namespace XTL; // Shouldn't be nescessary

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

    DBG_PRINTF("CreateDevice proxy thread is running.\n");

    while(true)
    {
        // if we have been signalled, create the device with cached parameters
        if(g_EmuCDPD.bReady)
        {
            DBG_PRINTF("CreateDevice proxy thread received request.\n");

            // only one device should be created at once
            if (g_pD3DDevice != nullptr) {
                DBG_PRINTF("CreateDevice proxy thread releasing old Device.\n");

                g_pD3DDevice->EndScene();

				for (auto &hostResourceIterator : g_XboxDirect3DResources) {
					if (hostResourceIterator.second.pHostResource) {
						(hostResourceIterator.second.pHostResource)->Release();
					}
				}
				g_XboxDirect3DResources.clear();

				// TODO: ensure all other resources are cleaned up too

				g_EmuCDPD.hRet = g_pD3DDevice->Release();

				// Address DirectX Debug Runtime reported error in _DEBUG builds
                // Direct3D8: (ERROR) :Not all objects were freed: the following indicate the types of unfreed objects.
                #ifndef _DEBUG
                    while(g_pD3DDevice->Release() != 0);
                #endif

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
                if(g_EmuCDPD.XboxPresentationParameters.BufferSurfaces[0] != NULL)
                    EmuLog(LOG_LEVEL::WARNING, "BufferSurfaces[0] : 0x%.08X", g_EmuCDPD.XboxPresentationParameters.BufferSurfaces[0]);

                if(g_EmuCDPD.XboxPresentationParameters.DepthStencilSurface != NULL)
                    EmuLog(LOG_LEVEL::WARNING, "DepthStencilSurface : 0x%.08X", g_EmuCDPD.XboxPresentationParameters.DepthStencilSurface);

				// Make a binary copy of the Xbox D3DPRESENT_PARAMETERS
				memcpy(&g_EmuCDPD.HostPresentationParameters, &(g_EmuCDPD.XboxPresentationParameters), sizeof(XTL::D3DPRESENT_PARAMETERS));

				// make adjustments to parameters to make sense with windows Direct3D
                {
                    g_EmuCDPD.HostPresentationParameters.Windowed = !g_XBVideo.bFullScreen;

                    if(g_XBVideo.bVSync)
                        g_EmuCDPD.HostPresentationParameters.SwapEffect = XTL::D3DSWAPEFFECT_COPY; // Was D3DSWAPEFFECT_COPY_VSYNC;

                    g_EmuCDPD.HostPresentationParameters.BackBufferFormat       = XTL::EmuXB2PC_D3DFormat(g_EmuCDPD.XboxPresentationParameters.BackBufferFormat);
					g_EmuCDPD.HostPresentationParameters.AutoDepthStencilFormat = XTL::EmuXB2PC_D3DFormat(g_EmuCDPD.XboxPresentationParameters.AutoDepthStencilFormat);

                    if(!g_XBVideo.bVSync && (g_D3DCaps.PresentationIntervals & D3DPRESENT_INTERVAL_IMMEDIATE) && g_XBVideo.bFullScreen)
                        g_EmuCDPD.HostPresentationParameters.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
                    else
                    {
                        if(g_D3DCaps.PresentationIntervals & D3DPRESENT_INTERVAL_ONE && g_XBVideo.bFullScreen)
                            g_EmuCDPD.HostPresentationParameters.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
                        else
                            g_EmuCDPD.HostPresentationParameters.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
                    }

					// HACK: Disable Tripple Buffering for now...
					// TODO: Enumerate maximum BackBufferCount if possible.
					if(g_EmuCDPD.XboxPresentationParameters.BackBufferCount > 1)
					{
						EmuLog(LOG_LEVEL::WARNING, "Limiting BackBufferCount to 1...");
						g_EmuCDPD.HostPresentationParameters.BackBufferCount = 1;
					}

                    // We ignore multisampling completely for now
					// It causes issues with backbuffer locking.
					// NOTE: It is possible to fix multisampling by having the host backbuffer normal size, the Xbox backbuffer being multisamples
					// and scaling that way, but that can be done as a future PR
					g_EmuCDPD.HostPresentationParameters.MultiSampleType = XTL::D3DMULTISAMPLE_NONE;
					g_EmuCDPD.HostPresentationParameters.MultiSampleQuality = 0;

					/*
                    if(g_EmuCDPD.XboxPresentationParameters.MultiSampleType != 0) {
                        // TODO: Check card for multisampling abilities
                        if(g_EmuCDPD.XboxPresentationParameters.MultiSampleType == XTL::X_D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX) // = 0x00001121 = 4385
							// Test-case : Galleon
                            g_EmuCDPD.HostPresentationParameters.MultiSampleType = XTL::D3DMULTISAMPLE_2_SAMPLES;
						else {
							// CxbxKrnlCleanup("Unknown MultiSampleType (0x%.08X)", g_EmuCDPD.XboxPresentationParameters.MultiSampleType);
							EmuLog(LOG_LEVEL::WARNING, "MultiSampleType 0x%.08X is not supported!", g_EmuCDPD.XboxPresentationParameters.MultiSampleType);
							g_EmuCDPD.HostPresentationParameters.MultiSampleType = XTL::D3DMULTISAMPLE_NONE;
						}
                    }*/

                    g_EmuCDPD.HostPresentationParameters.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

                    // retrieve resolution from configuration
                    if(g_EmuCDPD.HostPresentationParameters.Windowed)
                    {
						const char* resolution = g_XBVideo.szVideoResolution;
						if (2 != sscanf(resolution, "%u x %u", &g_EmuCDPD.HostPresentationParameters.BackBufferWidth, &g_EmuCDPD.HostPresentationParameters.BackBufferHeight)) {
							DBG_PRINTF("EmuCreateDeviceProxy: Couldn't parse resolution : %s.\n", resolution);
						}
						else {
							if (g_EmuCDPD.HostPresentationParameters.BackBufferWidth == 640)
								DBG_PRINTF("EmuCreateDeviceProxy: Default width wasn't updated.\n");
							if (g_EmuCDPD.HostPresentationParameters.BackBufferWidth == 480)
								DBG_PRINTF("EmuCreateDeviceProxy: Default height wasn't updated.\n");
						}

                        XTL::D3DDISPLAYMODE D3DDisplayMode;

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
							DBG_PRINTF("EmuCreateDeviceProxy: Couldn't parse resolution : %s.\n", resolution);
						}
						else {
							if (g_EmuCDPD.HostPresentationParameters.BackBufferWidth == 640)
								DBG_PRINTF("EmuCreateDeviceProxy: Default width wasn't updated.\n");
							if (g_EmuCDPD.HostPresentationParameters.BackBufferWidth == 480)
								DBG_PRINTF("EmuCreateDeviceProxy: Default height wasn't updated.\n");
						}

                        if(strcmp(szBackBufferFormat, "x1r5g5b5") == 0)
							g_EmuCDPD.HostPresentationParameters.BackBufferFormat = XTL::D3DFMT_X1R5G5B5;
                        else if(strcmp(szBackBufferFormat, "r5g6r5") == 0)
							g_EmuCDPD.HostPresentationParameters.BackBufferFormat = XTL::D3DFMT_R5G6B5;
                        else if(strcmp(szBackBufferFormat, "x8r8g8b8") == 0)
							g_EmuCDPD.HostPresentationParameters.BackBufferFormat = XTL::D3DFMT_X8R8G8B8;
                        else if(strcmp(szBackBufferFormat, "a8r8g8b8") == 0)
							g_EmuCDPD.HostPresentationParameters.BackBufferFormat = XTL::D3DFMT_A8R8G8B8;
                    }
                }

                // detect vertex processing capabilities
                if((g_D3DCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) && g_EmuCDPD.DeviceType == XTL::D3DDEVTYPE_HAL)
                {
                    DBG_PRINTF("Using hardware vertex processing\n");

                    g_EmuCDPD.BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
                    g_dwVertexShaderUsage = 0;
                }
                else
                {
                    DBG_PRINTF("Using software vertex processing\n");

                    g_EmuCDPD.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
                    g_dwVertexShaderUsage = D3DUSAGE_SOFTWAREPROCESSING;
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
					XTL::D3DDEVTYPE_HAL,
					g_EmuCDPD.HostPresentationParameters.BackBufferFormat,
					D3DUSAGE_DEPTHSTENCIL,
					XTL::D3DRTYPE_SURFACE,
					g_EmuCDPD.HostPresentationParameters.AutoDepthStencilFormat) != D3D_OK) {
					g_EmuCDPD.HostPresentationParameters.AutoDepthStencilFormat = XTL::D3DFMT_D24S8;
				}

				// For some reason, D3DFMT_D16_LOCKABLE as the AudoDepthStencil causes CreateDevice to fail...
				if (g_EmuCDPD.HostPresentationParameters.AutoDepthStencilFormat == XTL::D3DFMT_D16_LOCKABLE) {
					g_EmuCDPD.HostPresentationParameters.AutoDepthStencilFormat = XTL::D3DFMT_D16;
				}

				// DirectX9 doesn't support 0 as a swap effect
				if (g_EmuCDPD.HostPresentationParameters.SwapEffect == 0) {
					g_EmuCDPD.HostPresentationParameters.SwapEffect = XTL::D3DSWAPEFFECT_DISCARD;
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
					if (!XTL::EmuXBFormatRequiresConversionToARGB((XTL::X_D3DFORMAT)X_Format)) {
						// Convert the Xbox format into host format (without warning, thanks to the above restriction)
						XTL::D3DFORMAT PCFormat = XTL::EmuXB2PC_D3DFormat((XTL::X_D3DFORMAT)X_Format);
						if (PCFormat != XTL::D3DFMT_UNKNOWN) {
							// Index with Xbox D3DFormat, because host FourCC codes are too big to be used as indices
							if (D3D_OK == g_pDirect3D->CheckDeviceFormat(
									g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
									g_EmuCDPD.HostPresentationParameters.BackBufferFormat, 0,
									XTL::D3DRTYPE_SURFACE, PCFormat))
								g_bSupportsFormatSurface[X_Format] = true;
							if (D3D_OK == g_pDirect3D->CheckDeviceFormat(
									g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
									g_EmuCDPD.HostPresentationParameters.BackBufferFormat, D3DUSAGE_RENDERTARGET,
									XTL::D3DRTYPE_SURFACE, PCFormat))
								g_bSupportsFormatSurfaceRenderTarget[X_Format] = true;
							if (D3D_OK == g_pDirect3D->CheckDeviceFormat(
									g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
									g_EmuCDPD.HostPresentationParameters.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
									XTL::D3DRTYPE_SURFACE, PCFormat))
								g_bSupportsFormatSurfaceDepthStencil[X_Format] = true;
							if (D3D_OK == g_pDirect3D->CheckDeviceFormat(
									g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
									g_EmuCDPD.HostPresentationParameters.BackBufferFormat, 0,
									XTL::D3DRTYPE_TEXTURE, PCFormat))
								g_bSupportsFormatTexture[X_Format] = true;
							if (D3D_OK == g_pDirect3D->CheckDeviceFormat(
									g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
									g_EmuCDPD.HostPresentationParameters.BackBufferFormat, D3DUSAGE_RENDERTARGET,
									XTL::D3DRTYPE_TEXTURE, PCFormat))
								g_bSupportsFormatTextureRenderTarget[X_Format] = true;
							if (D3D_OK == g_pDirect3D->CheckDeviceFormat(
									g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
									g_EmuCDPD.HostPresentationParameters.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
									XTL::D3DRTYPE_TEXTURE, PCFormat))
								g_bSupportsFormatTextureDepthStencil[X_Format] = true;
							if (D3D_OK == g_pDirect3D->CheckDeviceFormat(
									g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
									g_EmuCDPD.HostPresentationParameters.BackBufferFormat, 0,
									XTL::D3DRTYPE_VOLUMETEXTURE, PCFormat))
								g_bSupportsFormatVolumeTexture[X_Format] = true;
							if (D3D_OK == g_pDirect3D->CheckDeviceFormat(
									g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
									g_EmuCDPD.HostPresentationParameters.BackBufferFormat, 0,
									XTL::D3DRTYPE_CUBETEXTURE, PCFormat))
								g_bSupportsFormatCubeTexture[X_Format] = true;
						}
					}
				}

                // default NULL guid
                ZeroMemory(&g_ddguid, sizeof(GUID));

				HRESULT hRet;

                // enumerate device guid for this monitor, for directdraw
				hRet = XTL::DirectDrawEnumerateExA(EmuEnumDisplayDevices, NULL, DDENUM_ATTACHEDSECONDARYDEVICES);
				DEBUG_D3DRESULT(hRet, "DirectDrawEnumerateExA");

                // create DirectDraw7
                {
                    if(FAILED(hRet)) {
                        hRet = XTL::DirectDrawCreateEx(NULL, (void**)&g_pDD7, XTL::IID_IDirectDraw7, NULL);
						DEBUG_D3DRESULT(hRet, "XTL::DirectDrawCreateEx(NULL)");
					} else {
						hRet = XTL::DirectDrawCreateEx(&g_ddguid, (void**)&g_pDD7, XTL::IID_IDirectDraw7, NULL);
						DEBUG_D3DRESULT(hRet, "XTL::DirectDrawCreateEx(&g_ddguid)");
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
						DBG_PRINTF("FourCC[%d] = %.4s\n", v, (char *)&(lpCodes[v]));
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
				XTL::IDirect3DSurface *pCurrentHostRenderTarget = nullptr;
                hRet = g_pD3DDevice->GetRenderTarget(
					0, // RenderTargetIndex
					&pCurrentHostRenderTarget);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetRenderTarget");
				// TODO : SetHostResource(BackBuffer[0], pCurrentHostRenderTarget);
*/

				// update z-stencil surface cache
				g_pD3DDevice->GetDepthStencilSurface(&g_DefaultHostDepthBufferSuface);
				UpdateDepthStencilFlags(g_DefaultHostDepthBufferSuface);
		
				hRet = g_pD3DDevice->CreateVertexBuffer
                (
                    1, 0, 0, XTL::D3DPOOL_MANAGED,
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
static void EmuVerifyResourceIsRegistered(XTL::X_D3DResource *pResource, DWORD D3DUsage = 0, int iTextureStage = 0, DWORD dwSize = 0)
{
	// Skip resources without data
	if (pResource->Data == NULL)
		return;

	auto key = GetHostResourceKey(pResource);
	auto it = g_XboxDirect3DResources.find(key);
	if (it != g_XboxDirect3DResources.end()) {
		// Don't trash RenderTargets
		// this fixes an issue where CubeMaps were broken because the surface Set in GetCubeMapSurface
		// would be overwritten by the surface created in SetRenderTarget
		if (D3DUsage == D3DUSAGE_RENDERTARGET) {
			return;
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
		g_XboxDirect3DResources[key] = newResource;
	}

	CreateHostResource(pResource, D3DUsage, iTextureStage, dwSize);
}

typedef struct {
	DWORD Hash = 0;
	DWORD IndexCount = 0;
	XTL::IDirect3DIndexBuffer* pHostIndexBuffer = nullptr;
} ConvertedIndexBuffer;

std::unordered_map<PWORD, ConvertedIndexBuffer> g_ConvertedIndexBuffers;
	
void CxbxRemoveIndexBuffer(PWORD pData)
{
	// HACK: Never Free
}

void CxbxUpdateActiveIndexBuffer
(
	PWORD         pIndexData,
	UINT          IndexCount
)
{
	LOG_INIT; // Allows use of DEBUG_D3DRESULT

	// Create a reference to the active buffer
	ConvertedIndexBuffer& indexBuffer = g_ConvertedIndexBuffers[pIndexData];

	// If the size has changed, free the buffer so it will be re-created
	if (indexBuffer.pHostIndexBuffer != nullptr &&
		indexBuffer.IndexCount < IndexCount) {
		indexBuffer.pHostIndexBuffer->Release();
		indexBuffer = {};
	}

	// If we need to create an index buffer, do so.
	if (indexBuffer.pHostIndexBuffer == nullptr) {
		// https://msdn.microsoft.com/en-us/library/windows/desktop/bb147168(v=vs.85).aspx
		// "Managing Resources (Direct3D 9)"
		// suggests "for resources which change with high frequency" [...]
		// "D3DPOOL_DEFAULT along with D3DUSAGE_DYNAMIC should be used."
		const XTL::D3DPOOL D3DPool = XTL::D3DPOOL_DEFAULT; // Was XTL::D3DPOOL_MANAGED
		// https://msdn.microsoft.com/en-us/library/windows/desktop/bb172625(v=vs.85).aspx
		// "Buffers created with D3DPOOL_DEFAULT that do not specify D3DUSAGE_WRITEONLY may suffer a severe performance penalty."
		const DWORD D3DUsage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY; // Was D3DUSAGE_WRITEONLY
		// Create a new native index buffer of the above determined size :
		HRESULT hRet = g_pD3DDevice->CreateIndexBuffer(
			IndexCount * 2,
			D3DUsage,
			XTL::D3DFMT_INDEX16,
			D3DPool,
			&indexBuffer.pHostIndexBuffer
			, nullptr // pSharedHandle
		);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateIndexBuffer");

		if (FAILED(hRet))
			CxbxKrnlCleanup("CxbxUpdateActiveIndexBuffer: IndexBuffer Create Failed!");
	}

	// If the data needs updating, do so
	uint32_t uiHash = XXHash32::hash(pIndexData, IndexCount * 2, 0);
	if (uiHash != indexBuffer.Hash)	{
		// Update the Index Count and the hash
		indexBuffer.IndexCount = IndexCount;
		indexBuffer.Hash = uiHash;

		// Update the host index buffer
		D3DLockData* pData = nullptr;
		indexBuffer.pHostIndexBuffer->Lock(0, 0, &pData, D3DLOCK_DISCARD);
		if (pData == nullptr) {
			CxbxKrnlCleanup("CxbxUpdateActiveIndexBuffer: Could not lock index buffer!");
		}

		DBG_PRINTF("CxbxUpdateActiveIndexBuffer: Copying %d indices (D3DFMT_INDEX16)\n", IndexCount);
		memcpy(pData, pIndexData, IndexCount * sizeof(XTL::INDEX16));

		indexBuffer.pHostIndexBuffer->Unlock();
	}

	// Activate the new native index buffer :
	HRESULT hRet = g_pD3DDevice->SetIndices(indexBuffer.pHostIndexBuffer);
	// Note : Under Direct3D 9, the BaseVertexIndex argument is moved towards DrawIndexedPrimitive
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetIndices");

	if (FAILED(hRet))
		CxbxKrnlCleanup("CxbxUpdateActiveIndexBuffer: SetIndices Failed!");
}

void Direct3D_CreateDevice_Start
(
	XTL::X_D3DPRESENT_PARAMETERS     *pPresentationParameters
)
{
	// HACK: Disable multisampling... See comment in CreateDevice proxy for more info
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
	// Set g_XboxD3DDevice to point to the Xbox D3D Device
	auto it = g_SymbolAddresses.find("D3DDEVICE");
	if (it != g_SymbolAddresses.end()) {
		xbaddr dwD3DDevice = it->second;
		if (dwD3DDevice != xbnull) {
			g_XboxD3DDevice = *((DWORD**)dwD3DDevice);
		}
    }
}

// LTCG specific Direct3D_CreateDevice function...
// This uses a custom calling convention passed unknown parameters
// Test-case: Ninja Gaiden
HRESULT WINAPI XTL::EMUPATCH(Direct3D_CreateDevice_4)
(
    X_D3DPRESENT_PARAMETERS     *pPresentationParameters
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pPresentationParameters)
		LOG_FUNC_END;

	Direct3D_CreateDevice_Start(pPresentationParameters);

	// Only then call Xbox CreateDevice function
	XB_trampoline(HRESULT, WINAPI, Direct3D_CreateDevice_4, (X_D3DPRESENT_PARAMETERS*));
	HRESULT hRet = XB_Direct3D_CreateDevice_4(pPresentationParameters);

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
	g_XboxBaseVertexIndex = BaseVertexIndex;

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
	pFieldStatus->VBlankCount = g_VBData.VBlank;

	// If we are interlaced, return the current field, otherwise, return progressive scan
	if (displayMode.Flags & X_D3DPRESENTFLAG_INTERLACED) {
		pFieldStatus->Field = (g_VBData.VBlank % 2 == 0) ? X_D3DFIELD_ODD : X_D3DFIELD_EVEN;
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

	if (g_pPrimaryPB != nullptr)
	{
		EmuLog(LOG_LEVEL::WARNING, "D3DDevice_BeginPush called without D3DDevice_EndPush in between?!");
		delete[] g_pPrimaryPB; // prevent a memory leak
	}

    DWORD *pRet = new DWORD[Count];

    g_dwPrimaryPBCount = Count;
    g_pPrimaryPB = pRet;

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

	if (g_pPrimaryPB != nullptr)
	{
		EmuLog(LOG_LEVEL::WARNING, "D3DDevice_BeginPush2 called without D3DDevice_EndPush in between?!");
		delete[] g_pPrimaryPB; // prevent a memory leak
	}

	DWORD *pRet = new DWORD[Count];

	g_dwPrimaryPBCount = Count;
	g_pPrimaryPB = pRet;

	*ppPush=pRet;
}

// ******************************************************************
// * patch: D3DDevice_EndPush
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_EndPush)(DWORD *pPush)
{
	LOG_FUNC_ONE_ARG(pPush);

	if (g_pPrimaryPB == nullptr)
		EmuLog(LOG_LEVEL::WARNING, "D3DDevice_EndPush called without preceding D3DDevice_BeginPush?!");
	else
	{
		EmuExecutePushBufferRaw(g_pPrimaryPB, g_dwPrimaryPBCount * sizeof(DWORD));

		delete[] g_pPrimaryPB;
		g_pPrimaryPB = nullptr;
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

	//LOG_FUNC_BEGIN
	//	LOG_FUNC_ARG(Handle)
	//	LOG_FUNC_ARG(Address)
	//	LOG_FUNC_END;
	DBG_PRINTF("D3DDevice_LoadVertexShader_4(Handle : %d Address : %08x);\n", Handle, Address);

	// Handle is always address of an Xbox VertexShader struct, or-ed with 1 (X_D3DFVF_RESERVED0)
	// An Xbox VertexShader contains : a 'Vshd' signature, flags, a size, a program (and constants)
	// Address is the slot (offset) from which the program must be written onwards (as whole DWORDS)
	// D3DDevice_LoadVertexShader pushes the program contained in the Xbox VertexShader struct to the NV2A
    if(Address < 136 && VshHandleIsVertexShader(Handle))
    {
		CxbxVertexShader *pVertexShader = GetCxbxVertexShader(Handle);
        for (DWORD i = Address; i < pVertexShader->Size; i++)
        {
            // TODO: This seems very fishy
            g_VertexShaderSlots[i] = Handle;
        }
	}
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

	// Handle is always address of an Xbox VertexShader struct, or-ed with 1 (X_D3DFVF_RESERVED0)
	// An Xbox VertexShader contains : a 'Vshd' signature, flags, a size, a program (and constants)
	// Address is the slot (offset) from which the program must be written onwards (as whole DWORDS)
	// D3DDevice_LoadVertexShader pushes the program contained in the Xbox VertexShader struct to the NV2A
    if(Address < 136) {
        CxbxVertexShader *pVertexShader = GetCxbxVertexShader(Handle);
		if (pVertexShader) {
			for (DWORD i = Address; i < pVertexShader->Size; i++) {
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
	g_CurrentXboxVertexShaderHandle = Handle;

    if(VshHandleIsVertexShader(Handle))
    {
        CxbxVertexShader *pVertexShader = GetCxbxVertexShader(Handle);
		hRet = g_pD3DDevice->SetVertexDeclaration(pVertexShader->pHostDeclaration);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexDeclaration(VshHandleIsVertexShader)");
		hRet = g_pD3DDevice->SetVertexShader((XTL::IDirect3DVertexShader9*)pVertexShader->Handle);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexShader(VshHandleIsVertexShader)");
    }
    else if(Handle == NULL)
    {
		hRet = g_pD3DDevice->SetVertexDeclaration(nullptr);
		hRet = g_pD3DDevice->SetVertexShader(nullptr);
		hRet = g_pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_TEX0);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_TEX0)");
	}
    else if(Address < 136)
    {
        X_D3DVertexShader *pVertexShader = (X_D3DVertexShader*)g_VertexShaderSlots[Address];

        if(pVertexShader != NULL)
        {
			hRet = g_pD3DDevice->SetVertexDeclaration((XTL::IDirect3DVertexDeclaration9*)((CxbxVertexShader *)((X_D3DVertexShader *)g_VertexShaderSlots[Address])->Handle)->pDeclaration);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexDeclaration(pVertexShader)");
			hRet = g_pD3DDevice->SetVertexShader((XTL::IDirect3DVertexShader9*)((CxbxVertexShader *)((X_D3DVertexShader *)g_VertexShaderSlots[Address])->Handle)->Handle);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexShader(pVertexShader)");
		}
        else
        {
            EmuLog(LOG_LEVEL::WARNING, "g_VertexShaderSlots[%d] = 0", Address);
		}
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

	X_D3DSurface *pBackBuffer = EmuNewD3DSurface();

	if(BackBuffer == -1) {
		static IDirect3DSurface *pCachedPrimarySurface = nullptr;

		if(pCachedPrimarySurface == nullptr) {
			// create a buffer to return
			// TODO: Verify the surface is always 640x480
			hRet = g_pD3DDevice->CreateOffscreenPlainSurface(640, 480, D3DFMT_A8R8G8B8, /*D3DPool=* /0, &pCachedPrimarySurface, nullptr);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateOffscreenPlainSurface");
		}

		SetHostSurface(pBackBuffer, pCachedPrimarySurface);

		hRet = g_pD3DDevice->GetFrontBuffer(pCachedPrimarySurface);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetFrontBuffer");

		if (FAILED(hRet)) {
			EmuLog(LOG_LEVEL::WARNING, "Could not retrieve primary surface, using backbuffer");
			SetHostSurface(pBackBuffer, nullptr);
			pCachedPrimarySurface->Release();
			pCachedPrimarySurface = nullptr;
			BackBuffer = 0;
		}

		// Debug: Save this image temporarily
		//D3DXSaveSurfaceToFile("C:\\Aaron\\Textures\\FrontBuffer.bmp", D3DXIFF_BMP, GetHostSurface(pBackBuffer), NULL, NULL);
	}

	if(BackBuffer != -1) {
		hRet = g_pD3DDevice->GetBackBuffer(
			0, // iSwapChain
			BackBuffer, D3DBACKBUFFER_TYPE_MONO, &pCachedPrimarySurface);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetBackBuffer");
	}
	//*/

	static X_D3DSurface *pBackBuffer = EmuNewD3DSurface();
	XTL::IDirect3DSurface *pCurrentHostBackBuffer = nullptr;

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

	SetHostSurface(pBackBuffer, pCurrentHostBackBuffer);

	// Increment reference count
	pBackBuffer->Common++; // EMUPATCH(D3DResource_AddRef)(pBackBuffer);

	return pBackBuffer;
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
	} else {
		DWORD Size = copyLockedRect.Pitch * copySurfaceDesc.Height; // TODO : What about mipmap levels?
		// Finally, do the copy from the converted host resource to the xbox resource
		memcpy((void*)GetDataFromXboxResource(pXboxBackBuffer), copyLockedRect.pBits, Size);
	}

	pCopySrcSurface->UnlockRect();

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

bool GetHostRenderTargetDimensions(DWORD *pHostWidth, DWORD *pHostHeight)
{
	XTL::IDirect3DSurface* pHostRenderTarget = nullptr;

	g_pD3DDevice->GetRenderTarget(
		0, // RenderTargetIndex
		&pHostRenderTarget);

	// The following can only work if we could retrieve a host render target
	if (!pHostRenderTarget)
		return false;

	// Get current host render target dimensions
	XTL::D3DSURFACE_DESC HostRenderTarget_Desc;
	pHostRenderTarget->GetDesc(&HostRenderTarget_Desc);
	pHostRenderTarget->Release();

	// Emulate field-rendering not by halving the host backbuffer, but by faking
	// the host backbuffer to half-height, which results in a correct viewport scale :
	if (g_pXboxRenderTarget == g_XboxBackBufferSurface) {
		if (g_EmuCDPD.XboxPresentationParameters.Flags & X_D3DPRESENTFLAG_FIELD) {
			// Test case : XDK Sample FieldRender
			HostRenderTarget_Desc.Height /= 2;
		}
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

	if (g_pXboxRenderTarget) {
		// Clip the Xbox Viewport to the render target dimensions
		// This is required because during SetRenderTarget, Xbox calls SetViewPort with impossibly large values
		DWORD XboxRenderTarget_Width = GetPixelContainerWidth(g_pXboxRenderTarget);
		DWORD XboxRenderTarget_Height = GetPixelContainerHeigth(g_pXboxRenderTarget);

		DWORD left = std::max((int)pViewport->X, 0);
		DWORD top = std::max((int)pViewport->Y, 0);
		DWORD right = std::min((int)pViewport->X + (int)pViewport->Width, (int)XboxRenderTarget_Width);
		DWORD bottom = std::min((int)pViewport->Y + (int)pViewport->Height, (int)XboxRenderTarget_Height);

		XboxViewPort.X = left;
		XboxViewPort.Y = top;
		XboxViewPort.Width = right - left;
		XboxViewPort.Height = bottom - top;
		XboxViewPort.MinZ = pViewport->MinZ;
		XboxViewPort.MaxZ = pViewport->MaxZ;

		if (g_ScaleViewport) {
			// Get current host render target dimensions
			DWORD HostRenderTarget_Width;
			DWORD HostRenderTarget_Height;

			if (GetHostRenderTargetDimensions(&HostRenderTarget_Width, &HostRenderTarget_Height)) {
				// Scale Xbox to host dimensions (avoiding hard-coding 640 x 480)
				HostViewPort.X = ScaleDWORD(XboxViewPort.X, XboxRenderTarget_Width, HostRenderTarget_Width);
				HostViewPort.Y = ScaleDWORD(XboxViewPort.Y, XboxRenderTarget_Height, HostRenderTarget_Height);
				HostViewPort.Width = ScaleDWORD(XboxViewPort.Width, XboxRenderTarget_Width, HostRenderTarget_Width);
				HostViewPort.Height = ScaleDWORD(XboxViewPort.Height, XboxRenderTarget_Height, HostRenderTarget_Height);
				// TODO : Fix test-case Shenmue 2 (which halves height, leaving the bottom half unused)
				HostViewPort.MinZ = XboxViewPort.MinZ; // No need scale Z for now
				HostViewPort.MaxZ = XboxViewPort.MaxZ;
			}
			else {
				EmuLog(LOG_LEVEL::WARNING, "GetHostRenderTargetDimensions failed - SetViewport sets Xbox viewport instead!");
			}
		}
	}

	HRESULT hRet = g_pD3DDevice->SetViewport(&HostViewPort);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetViewport");
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

#if 0
    float fScaleX = 1.0f;
    float fScaleY = 1.0f;
    float fScaleZ = 1.0f;
    float fOffsetX = 0.5 + 1.0/32;
    float fOffsetY = 0.5 + 1.0/32;
	X_D3DVIEWPORT8 Viewport;

	EMUPATCH(D3DDevice_GetViewport)(&Viewport);

    pScale->x = (float)Viewport.Width * 0.5f * fScaleX;
    pScale->y = (float)Viewport.Height * -0.5f * fScaleY;
    pScale->z = (Viewport.MaxZ - Viewport.MinZ) * fScaleZ;
    pScale->w = 0;

    pOffset->x = (float)Viewport.Width * fScaleX * 0.5f + (float)Viewport.X * fScaleX + fOffsetX;
    pOffset->y = (float)Viewport.Height * fScaleY * 0.5f + (float)Viewport.Y * fScaleY + fOffsetY;
    pOffset->z = Viewport.MinZ * fScaleZ;
    pOffset->w = 0;
#else
    pScale->x = 1.0f;
    pScale->y = 1.0f;
    pScale->z = 1.0f;
    pScale->w = 1.0f;

    pOffset->x = 0.0f;
    pOffset->y = 0.0f;
    pOffset->z = 0.0f;
    pOffset->w = 0.0f;
#endif
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

    g_VertexShaderConstantMode = Mode;
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
	HRESULT hRet = XB_D3DDevice_CreateVertexShader(pDeclaration, pFunction, pHandle, Usage);
	if (FAILED(hRet)) {
		LOG_TEST_CASE("XB_D3DDevice_CreateVertexShader failed");
		RETURN(hRet);
	}

	if (g_pD3DDevice == nullptr) {
		LOG_TEST_CASE("D3DDevice_CreateVertexShader called before Direct3D_CreateDevice");
		// We lie to allow the game to continue for now, but it probably won't work well
		return STATUS_SUCCESS;
	}

    // HACK: TODO: support this situation
    if(pDeclaration == NULL) {
		LOG_TEST_CASE("Vertex shader without declaration");
        *pHandle = NULL;
        return D3D_OK;
    }

	// Now, we can create the host vertex shader
	CxbxVertexShader* hostVertexShader = (CxbxVertexShader*)malloc(sizeof(CxbxVertexShader));
	memset(hostVertexShader, 0, sizeof(CxbxVertexShader));

    LPD3DXBUFFER pRecompiledBuffer = NULL;
    DWORD        *pRecompiledDeclaration = NULL;
    DWORD        *pRecompiledFunction = NULL;
    DWORD        VertexShaderSize = 0;
	DWORD        OriginalDeclarationCount = 0;
    DWORD        HostDeclarationSize = 0;
    DWORD        Handle = 0;

    hRet = XTL::EmuRecompileVshDeclaration((DWORD*)pDeclaration,
                                                   (XTL::D3DVERTEXELEMENT9**)&pRecompiledDeclaration,
												   &OriginalDeclarationCount,
                                                   &HostDeclarationSize,
                                                   pFunction == NULL,
                                                   &hostVertexShader->VertexShaderInfo);


	// Create the vertex declaration
	hRet = g_pD3DDevice->CreateVertexDeclaration((XTL::D3DVERTEXELEMENT9*)pRecompiledDeclaration, &hostVertexShader->pHostDeclaration);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateVertexDeclaration");

	if (FAILED(hRet)) {
		// NOTE: This is a fatal error because it ALWAYS triggers a crash within DrawVertices if not set
		CxbxKrnlCleanup("Failed to create Vertex Declaration");
	}
	g_pD3DDevice->SetVertexDeclaration(hostVertexShader->pHostDeclaration);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexDeclaration");

	if (SUCCEEDED(hRet) && pFunction)
	{
		boolean bUseDeclarationOnly = 0;

		hRet = XTL::EmuRecompileVshFunction((DWORD*)pFunction,
			&pRecompiledBuffer,
			&VertexShaderSize,
			g_VertexShaderConstantMode == X_D3DSCM_NORESERVEDCONSTANTS,
			&bUseDeclarationOnly,
			pRecompiledDeclaration,
            HostDeclarationSize);
		if (SUCCEEDED(hRet))
		{
			if (!bUseDeclarationOnly)
				pRecompiledFunction = (DWORD*)pRecompiledBuffer->GetBufferPointer();
			else
				pRecompiledFunction = NULL;
		}
		else
		{
			pRecompiledFunction = NULL;
			EmuLog(LOG_LEVEL::WARNING, "Couldn't recompile vertex shader function.");
		}
	}

    //DBG_PRINTF("MaxVertexShaderConst = %d\n", g_D3DCaps.MaxVertexShaderConst);

	if (SUCCEEDED(hRet) && pRecompiledFunction != nullptr)
	{
		hRet = g_pD3DDevice->CreateVertexShader
		(
			pRecompiledFunction,
			(IDirect3DVertexShader9**)&Handle
		);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateVertexShader");
	}

	//* Fallback to dummy shader.
	if (FAILED(hRet))
	{
		static const char dummy[] =
			"vs.1.1\n"
			"dcl_position v0\n"
			"dp4 oPos.x, v0, c96\n"
			"dp4 oPos.y, v0, c97\n"
			"dp4 oPos.z, v0, c98\n"
			"dp4 oPos.w, v0, c99\n";

		EmuLog(LOG_LEVEL::WARNING, "Trying fallback:\n%s", dummy);

		hRet = D3DXAssembleShader(
			dummy,
			strlen(dummy),
			/*pDefines=*/nullptr,
			/*pInclude=*/nullptr,
			/*Flags=*/0, // Was D3DXASM_SKIPVALIDATION
			/*ppCompiledShader=*/&pRecompiledBuffer,
			/*ppCompilationErrors*/nullptr);

		DEBUG_D3DRESULT(hRet, "D3DXAssembleShader");

		hRet = g_pD3DDevice->CreateVertexShader
		(
			(DWORD*)pRecompiledBuffer->GetBufferPointer(),
			(IDirect3DVertexShader9**)&Handle
		);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateVertexShader(fallback)");
	}

	if (pRecompiledBuffer != nullptr)
	{
		pRecompiledBuffer->Release();
		pRecompiledBuffer = nullptr;
	}

    // Save the status, to remove things later
	hostVertexShader->Status = hRet;

    free(pRecompiledDeclaration);

	hostVertexShader->pDeclaration = (DWORD*)malloc(OriginalDeclarationCount * sizeof(DWORD));
    memcpy(hostVertexShader->pDeclaration, pDeclaration, OriginalDeclarationCount * sizeof(DWORD));

	hostVertexShader->FunctionSize = 0;
	hostVertexShader->pFunction = NULL;
	hostVertexShader->Type = X_VST_NORMAL;
	hostVertexShader->Size = (VertexShaderSize - sizeof(VSH_SHADER_HEADER)) / VSH_INSTRUCTION_SIZE_BYTES;
	hostVertexShader->OriginalDeclarationCount = OriginalDeclarationCount; 
    hostVertexShader->HostDeclarationSize = HostDeclarationSize;

    if(SUCCEEDED(hRet))
    {
        if(pFunction != NULL)
        {
			hostVertexShader->pFunction = (DWORD*)malloc(VertexShaderSize);
            memcpy(hostVertexShader->pFunction, pFunction, VertexShaderSize);
			hostVertexShader->FunctionSize = VertexShaderSize;
        }
        else
        {
			hostVertexShader->pFunction = NULL;
			hostVertexShader->FunctionSize = 0;
        }
		hostVertexShader->Handle = Handle;
    }
    else
    {
		LOG_TEST_CASE("Falling back to FVF shader");
        hostVertexShader->Handle = D3DFVF_XYZ | D3DFVF_TEX0;
    }

	// Register the host Vertex Shader
	SetCxbxVertexShader(*pHandle, hostVertexShader);

    if(FAILED(hRet))
    {
#ifdef _DEBUG_TRACK_VS
        if (pFunction)
        {
            char pFileName[30];
            static int FailedShaderCount = 0;
            VSH_SHADER_HEADER *pHeader = (VSH_SHADER_HEADER*)pFunction;
            EmuLog(LOG_LEVEL::WARNING, "Couldn't create vertex shader!");
            sprintf(pFileName, "failed%05d.xvu", FailedShaderCount);
            FILE *f = fopen(pFileName, "wb");
            if(f)
            {
                fwrite(pFunction, sizeof(VSH_SHADER_HEADER) + pHeader->NumInst * 16, 1, f);
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
	static uint32 returnAddr;

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
    for (uint32 i = 0; i < ConstantCount; i++)
    {
        printf("SetVertexShaderConstant, c%d (c%d) = { %f, %f, %f, %f }\n",
               Register - 96 + i, Register + i,
               *((float*)pConstantData + 4 * i),
               *((float*)pConstantData + 4 * i + 1),
               *((float*)pConstantData + 4 * i + 2),
               *((float*)pConstantData + 4 * i + 3));
    }
#endif*/ // _DEBUG_TRACK_VS_CONST

	// TODO: HACK: Since Xbox vertex shader constants range from -96 to 96, during conversion
	// some shaders need to add 96 to use ranges 0 to 192.  This fixes 3911 - 4361 games and XDK
	// samples, but breaks Turok.
	// See D3DDevice_GetVertexShaderConstant
	if(g_BuildVersion <= 4361)
		Register += 96;

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

    EMUPATCH(D3DDevice_SetVertexShaderConstant)(Register, pConstantData, 1);
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

	// Redirect to the standard version.

    EMUPATCH(D3DDevice_SetVertexShaderConstant)(Register, pConstantData, 1);
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

	EMUPATCH(D3DDevice_SetVertexShaderConstant)(Register, pConstantData, 4);
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

	EMUPATCH(D3DDevice_SetVertexShaderConstant)(Register, pConstantData, ConstantCount / 4);
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

	// Redirect to the standard version.

	EMUPATCH(D3DDevice_SetVertexShaderConstant)(Register, pConstantData, ConstantCount / 4);
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
	DBG_PRINTF("D3DDevice_SetTexture_4(Stage : %d pTexture : %08x);\n", Stage, pTexture);

	// Call the Xbox implementation of this function, to properly handle reference counting for us
	//XB_trampoline(VOID, WINAPI, D3DDevice_SetTexture_4, (X_D3DBaseTexture*));
	//XB_D3DDevice_SetTexture_4(pTexture);

	EmuD3DActiveTexture[Stage] = pTexture;
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

	EmuD3DActiveTexture[Stage] = pTexture;
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

    DWORD StageLookup[TEXTURE_STAGES] = { 0x00081b00, 0x00081b40, 0x00081b80, 0x00081bc0 };
	// This array contains D3DPUSH_ENCODE(NV2A_TX_OFFSET(v), 2) = 2 DWORD's, shifted left PUSH_COUNT_SHIFT (18) left
    DWORD Stage = -1;

    for (int v = 0; v < TEXTURE_STAGES; v++) {
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
		if (EmuD3DActiveTexture[Stage] == xbnullptr) {
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
			CxbxActiveTextureCopies[Stage].Common = EmuD3DActiveTexture[Stage]->Common;
			CxbxActiveTextureCopies[Stage].Data = Data;
			CxbxActiveTextureCopies[Stage].Format = Format;
			CxbxActiveTextureCopies[Stage].Lock = 0;
			CxbxActiveTextureCopies[Stage].Size = EmuD3DActiveTexture[Stage]->Size;

			// Use the above modified copy, instead of altering the active Xbox texture
			EmuD3DActiveTexture[Stage] = &CxbxActiveTextureCopies[Stage];
			// Note : Since EmuD3DActiveTexture and CxbxActiveTextureCopies are host-managed,
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
	// Note : XQEMU recently verified that the int16_t arguments
	// must be mapped to floats in the range [-32768.0, 32767.0]
	// (See https://github.com/xqemu/xqemu/pull/176)
	fa = (float)a;
	fb = (float)b;

    EMUPATCH(D3DDevice_SetVertexData4f)(Register, fa, fb, 0.0f, 1.0f);
}

DWORD FloatsToDWORD(FLOAT d, FLOAT a, FLOAT b, FLOAT c)
{
	DWORD ca = (FtoDW(d) << 24);
	DWORD cr = (FtoDW(a) << 16) & 0x00FF0000;
	DWORD cg = (FtoDW(b) << 8) & 0x0000FF00;
	DWORD cb = (FtoDW(c) << 0) & 0x000000FF;

	return ca | cr | cg | cb;
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

	// Grow g_InlineVertexBuffer_Table to contain at least current, and a potentially next vertex
	if (g_InlineVertexBuffer_TableLength <= g_InlineVertexBuffer_TableOffset + 1) {
		if (g_InlineVertexBuffer_TableLength == 0) {
			g_InlineVertexBuffer_TableLength = PAGE_SIZE / sizeof(struct _D3DIVB);
		} else {
			g_InlineVertexBuffer_TableLength *= 2;
		}

		g_InlineVertexBuffer_Table = (struct _D3DIVB*)realloc(g_InlineVertexBuffer_Table, sizeof(struct _D3DIVB) * g_InlineVertexBuffer_TableLength);
		DBG_PRINTF("Reallocated g_InlineVertexBuffer_Table to %d entries\n", g_InlineVertexBuffer_TableLength);
	}

	// Is this the initial call after D3DDevice_Begin() ?
	if (g_InlineVertexBuffer_FVF == 0) {
		// Set first vertex to zero (preventing leaks from prior Begin/End calls)
		g_InlineVertexBuffer_Table[0] = {};

		// Get the vertex shader flags (if any is active) :
		uint32_t ActiveVertexAttributeFlags = 0;
		if (VshHandleIsVertexShader(g_CurrentXboxVertexShaderHandle)) {
			LOG_TEST_CASE("D3DDevice_SetVertexData4f with active VertexShader");
			X_D3DVertexShader *pXboxVertexShader = VshHandleToXboxVertexShader(g_CurrentXboxVertexShaderHandle);
			if (!(pXboxVertexShader->Flags & 0x10/*=X_VERTEXSHADER_PROGRAM*/)) {
				ActiveVertexAttributeFlags = pXboxVertexShader->Flags;
			}
		}

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
	uint FVFPosType = g_InlineVertexBuffer_FVF & D3DFVF_POSITION_MASK;

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
			break;
		}

		case X_D3DVSDE_SPECULAR:
		{
			g_InlineVertexBuffer_Table[o].Specular = D3DCOLOR_COLORVALUE(a, b, c, d);
			g_InlineVertexBuffer_FVF |= D3DFVF_SPECULAR;
			break;
		}

		case X_D3DVSDE_FOG: // Xbox extension
		{
			g_InlineVertexBuffer_Table[o].Fog = a; // TODO : What about the other (b, c and d) arguments?
			EmuLog(LOG_LEVEL::WARNING, "Host Direct3D8 doesn''t support FVF FOG");
			break;
		}

		// Note : X_D3DVSDE_POINTSIZE: Maps to D3DFVF_PSIZE, which is not available on Xbox FVF's

		case X_D3DVSDE_BACKDIFFUSE: // Xbox extension
		{
			g_InlineVertexBuffer_Table[o].BackDiffuse = D3DCOLOR_COLORVALUE(a, b, c, d);
			EmuLog(LOG_LEVEL::WARNING, "Host Direct3D8 doesn''t support FVF BACKDIFFUSE");
			break;
		}

		case X_D3DVSDE_BACKSPECULAR: // Xbox extension
		{
			g_InlineVertexBuffer_Table[o].BackSpecular = D3DCOLOR_COLORVALUE(a, b, c, d);
			EmuLog(LOG_LEVEL::WARNING, "Host Direct3D8 doesn''t support FVF BACKSPECULAR");
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
	// See comment note in D3DDevice_SetVertexData2s
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

    FLOAT a = ((Color & 0xFF000000) >> 24) / 255.0f;
    FLOAT r = ((Color & 0x00FF0000) >> 16) / 255.0f;
    FLOAT g = ((Color & 0x0000FF00) >> 8) / 255.0f;
    FLOAT b = ((Color & 0x000000FF) >> 0) / 255.0f;

    EMUPATCH(D3DDevice_SetVertexData4f)(Register, r, g, b, a);
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

	DWORD dwFillMode;

	if(g_iWireframe == 0)
        dwFillMode = g_CurrentFillMode;
    else if(g_iWireframe == 1)
        dwFillMode = D3DFILL_WIREFRAME;
    else
        dwFillMode = D3DFILL_POINT;

	HRESULT hRet;

    hRet = g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, dwFillMode);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetRenderState");

    hRet = g_pD3DDevice->Clear(Count, pRects, HostFlags, Color, Z, Stencil);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->Clear");
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
	XTL::IDirect3DSurface *pCurrentHostBackBuffer = nullptr;
	HRESULT hRet = g_pD3DDevice->GetBackBuffer(
		0, // iSwapChain
		0, XTL::D3DBACKBUFFER_TYPE_MONO, &pCurrentHostBackBuffer);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetBackBuffer - Unable to get backbuffer surface!");
	if (hRet == D3D_OK) {
		assert(pCurrentHostBackBuffer != nullptr);

		pCurrentHostBackBuffer->UnlockRect(); // remove any old lock

		// Get backbuffer dimenions; TODO : remember this once, at creation/resize time
		D3DSURFACE_DESC BackBufferDesc;
		pCurrentHostBackBuffer->GetDesc(&BackBufferDesc);
		
		const DWORD LoadSurfaceFilter = D3DX_DEFAULT; // == D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER
		// Previously we used D3DX_FILTER_POINT here, but that gave jagged edges in Dashboard.
		// Dxbx note : D3DX_FILTER_LINEAR gives a smoother image, but 'bleeds' across borders

		if (!g_DirectHostBackBufferAccess) {
			auto pXboxBackBufferHostSurface = GetHostSurface(g_XboxBackBufferSurface);
			if (pXboxBackBufferHostSurface) {
				// Blit Xbox BackBuffer to host BackBuffer
				// TODO: This could be much faster if we used the XboxBackBufferSurface as a texture and blitted with a fullscreen quad
				// This way, the scaling/format conversion would be handled by the GPU instead
				// If we were using native D3D9, we could just use StretchRects instead, but D3D8 doesn't have that feature!
				hRet = D3DXLoadSurfaceFromSurface(
					/* pDestSurface = */ pCurrentHostBackBuffer,
					/* pDestPalette = */ nullptr,
					/* pDestRect = */ nullptr,
					/* pSrcSurface = */ pXboxBackBufferHostSurface,
					/* pSrcPalette = */ nullptr,
					/* pSrcRect = */ nullptr,
					/* Filter = */ LoadSurfaceFilter,
					/* ColorKey = */ 0);

				if (hRet != D3D_OK) {
					EmuLog(LOG_LEVEL::WARNING, "Couldn't blit Xbox BackBuffer to host BackBuffer : %X", hRet);
				}
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
			uint08 *pOverlayData = (uint08*)GetDataFromXboxResource(&g_OverlayProxy.Surface);
			UINT OverlayWidth, OverlayHeight, OverlayDepth, OverlayRowPitch, OverlaySlicePitch;
			CxbxGetPixelContainerMeasures(
				&g_OverlayProxy.Surface,
				0, // dwMipMapLevel
				&OverlayWidth, &OverlayHeight, &OverlayDepth, &OverlayRowPitch, &OverlaySlicePitch);

			RECT EmuSourRect;
			RECT EmuDestRect;

			if (g_OverlayProxy.SrcRect.right > 0) {
				EmuSourRect = g_OverlayProxy.SrcRect;
			}
			else {
				SetRect(&EmuSourRect, 0, 0, OverlayWidth, OverlayHeight);
			}

			if (g_OverlayProxy.DstRect.right > 0) {
				// If there's a destination rectangle given, copy that into our local variable :
				EmuDestRect = g_OverlayProxy.DstRect;
			} else {
				// Should this be our backbuffer size rather than the actual window size?
				GetClientRect(g_hEmuWindow, &EmuDestRect);
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

			// Use D3DXLoadSurfaceFromMemory() to do conversion, stretching and filtering
			// avoiding the need for YUY2toARGB() (might become relevant when porting to D3D9 or OpenGL)
			// see https://msdn.microsoft.com/en-us/library/windows/desktop/bb172902(v=vs.85).aspx
			hRet = D3DXLoadSurfaceFromMemory(
				/* pDestSurface = */ pCurrentHostBackBuffer,
				/* pDestPalette = */ nullptr,
				/* pDestRect = */ &EmuDestRect,
				/* pSrcMemory = */ pOverlayData, // Source buffer
				/* SrcFormat = */ PCFormat,
				/* SrcPitch = */ OverlayRowPitch,
				/* pSrcPalette = */ nullptr,
				/* pSrcRect = */ &EmuSourRect, // This parameter cannot be NULL
				/* Filter = */ LoadSurfaceFilter,
				/* ColorKey = */ g_OverlayProxy.EnableColorKey ? g_OverlayProxy.ColorKey : 0);

			DEBUG_D3DRESULT(hRet, "D3DXLoadSurfaceFromMemory - UpdateOverlay could not convert buffer!\n");
			if (hRet != D3D_OK) {
				EmuLog(LOG_LEVEL::WARNING, "Couldn't blit Xbox overlay to host BackBuffer : %X", hRet);
			}
		}

		pCurrentHostBackBuffer->Release();
	}

	g_pD3DDevice->EndScene();

	hRet = g_pD3DDevice->Present(0, 0, 0, 0);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->Present");

	hRet = g_pD3DDevice->BeginScene();

	if (!g_UncapFramerate) {
		// If the last frame completed faster than the Xbox VBlank period, wait for it
		// TODO: Read the frame rate target from the Xbox display mode
		// See comments in GetNextVblankTime();
		auto targetDuration = 16.6666666667ms;
		while (std::chrono::high_resolution_clock::now() - frameStartTime < targetDuration) {
			// We use an empty while loop because actually sleeping is too unstable
			// Sleeping causes the frame duration to jitter...
			;
		}

		frameStartTime = std::chrono::high_resolution_clock::now();
	}

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
		g_VBData.Swap = g_VBData.VBlank;

		if (g_VBData.VBlank == g_VBLastSwap + 1)
			g_VBData.Flags = 1; // D3DVBLANK_SWAPDONE
		else
		{
			g_VBData.Flags = 2; // D3DVBLANK_SWAPMISSED
			g_SwapData.MissedVBlanks++;
		}
	}

	// Handle Swap Callback function
	{
		g_SwapData.Swap++;

		if(g_pSwapCallback != NULL) 
		{
				
			g_pSwapCallback(&g_SwapData);
				
		}
	}

	DWORD result;
	if (Flags == CXBX_SWAP_PRESENT_FORWARD) // Only do this when forwarded from Present
		result = D3D_OK; // Present always returns success
	else
		result = g_SwapData.Swap; // Swap returns number of swaps

    return result;
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
		XTL::X_D3DTexture *pParentXboxTexture = (pXboxSurface) ? (XTL::X_D3DTexture *)pXboxSurface->Parent : xbnullptr;

		// Don't init the Parent if the Surface and Surface Parent formats differ
		// Happens in some Outrun 2006 SetRenderTarget calls
		if (pParentXboxTexture && (pXboxSurface->Format == pParentXboxTexture->Format)) {
			XTL::IDirect3DBaseTexture *pParentHostBaseTexture = GetHostBaseTexture(pParentXboxTexture, D3DUsage, iTextureStage);
			switch (pParentHostBaseTexture->GetType()) {
			case XTL::D3DRTYPE_VOLUMETEXTURE: {
				// TODO
				break;
			}
			case XTL::D3DRTYPE_CUBETEXTURE: {
				// TODO
				// test-case : Burnout
				break;
			}
			case XTL::D3DRTYPE_TEXTURE: {
				// For surfaces with a parent texture, map these to a host texture first
				XTL::IDirect3DTexture *pParentHostTexture = (XTL::IDirect3DTexture *)pParentHostBaseTexture;
				UINT SurfaceLevel = 0; // TODO : Derive actual level based on pXboxSurface->Data delta to pParentXboxTexture->Data
				XTL::IDirect3DSurface *pNewHostSurface;
				HRESULT hRet = pParentHostTexture->GetSurfaceLevel(SurfaceLevel, &pNewHostSurface);
				DEBUG_D3DRESULT(hRet, "pHostParentTexture->GetSurfaceLevel");
				if (hRet == D3D_OK) {
					SetHostSurface(pXboxSurface, pNewHostSurface);
					DBG_PRINTF("CreateHostResource : Successfully created surface level (%u, 0x%.08X, 0x%.08X)\n",
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
			XTL::IDirect3DVolumeTexture *pParentHostVolumeTexture = GetHostVolumeTexture(pParentXboxVolumeTexture, iTextureStage);
			UINT VolumeLevel = 0; // TODO : Derive actual level based on pXboxVolume->Data delta to pParentXboxVolumeTexture->Data
			XTL::IDirect3DVolume *pNewHostVolume;
			HRESULT hRet = pParentHostVolumeTexture->GetVolumeLevel(VolumeLevel, &pNewHostVolume);
			DEBUG_D3DRESULT(hRet, "pParentHostVolumeTexture->GetVolumeLevel");
			if (hRet == D3D_OK) {
				SetHostVolume(pXboxVolume, pNewHostVolume);
				DBG_PRINTF("CreateHostResource : Successfully created volume level (%u, 0x%.08X, 0x%.08X)\n",
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
		XTL::D3DPOOL D3DPool = XTL::D3DPOOL_MANAGED; // TODO : Nuance D3DPOOL where/when needed

#if 0 // jackchen : this must be marked out to fix a fading black regression.
		// PatrickvL : The following tries to switch over to using dynamic textures,
		// since these can always be locked (which is quite helpfull for doing updates).
		// Addendum : Alas, this prevents locking anything else but the first mipmap level,
		// turning all others black. (Spotted by Cakelancelot and disabled by jackchen)
		if (g_D3DCaps.Caps2 & D3DCAPS2_DYNAMICTEXTURES) {
			// jackchen : leave either one single line of code will introduce further regresson.
			D3DUsage |= D3DUSAGE_DYNAMIC;
		}
#endif

		if (EmuXBFormatIsDepthBuffer(X_Format)) {
			D3DUsage |= D3DUSAGE_DEPTHSTENCIL;
		}
		else if (pPixelContainer == g_pXboxRenderTarget) {
			if (EmuXBFormatIsRenderTarget(X_Format))
				D3DUsage |= D3DUSAGE_RENDERTARGET;
			else
				EmuLog(LOG_LEVEL::WARNING, "Updating RenderTarget %s with an incompatible format!", ResourceTypeName);
		}

		// Determine the format we'll be using on host D3D
		XTL::D3DFORMAT PCFormat;
		bool bConvertToARGB = false;

		if (EmuXBFormatRequiresConversionToARGB(X_Format)) {
			bConvertToARGB = true;
			PCFormat = XTL::D3DFMT_A8R8G8B8;

			// Unset D3DUSAGE_DEPTHSTENCIL: It's not possible for ARGB textures to be depth stencils
			// Fixes CreateTexture error in Virtua Cop 3 (Chihiro)
			D3DUsage &= ~D3DUSAGE_DEPTHSTENCIL;
		}
		else {
			// TODO : Nuance the following, because the Direct3D 8 docs states
			// CheckDeviceFormat is needed when D3DUSAGE_RENDERTARGET or
			// D3DUSAGE_DYNAMNIC is specified.

			// Otherwise, lookup resource type and accompanying 'SupportedFormat' array
			bool *pbSupportedFormats = g_bSupportsFormatTexture;

			switch (XboxResourceType) {
			case XTL::X_D3DRTYPE_SURFACE: {
				if (D3DUsage & D3DUSAGE_RENDERTARGET) {
					pbSupportedFormats = g_bSupportsFormatSurfaceRenderTarget;
				}
				else if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
					pbSupportedFormats = g_bSupportsFormatSurfaceDepthStencil;
				}
				else {
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
				}
				else if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
					pbSupportedFormats = g_bSupportsFormatTextureDepthStencil;
				}
				else {
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

			// Does host CheckDeviceFormat() succeed on this format?
			if (pbSupportedFormats[X_Format]) {
				// Then use matching host format
				PCFormat = EmuXB2PC_D3DFormat(X_Format);

				// If, and ONLY if this is the default backbuffer, make sure the format matches the host backbuffer
				if (pResource == g_XboxBackBufferSurface) {
					PCFormat = g_EmuCDPD.HostPresentationParameters.BackBufferFormat;
				}
			}
			else {
				if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
					// If it was a depth stencil, fall back to a known supported depth format
					EmuLog(LOG_LEVEL::WARNING, "Xbox %s Format %x will be converted to D3DFMT_D24S8", ResourceTypeName, X_Format);
					PCFormat = XTL::D3DFMT_D24S8;
				} else if (EmuXBFormatCanBeConvertedToARGB(X_Format)) {
					EmuLog(LOG_LEVEL::WARNING, "Xbox %s Format %x will be converted to ARGB", ResourceTypeName, X_Format);
					bConvertToARGB = true;
					PCFormat = XTL::D3DFMT_A8R8G8B8;
				} else {
					// Otherwise, use a best matching format
					/*CxbxKrnlCleanup*/EmuLog(LOG_LEVEL::WARNING, "Encountered a completely incompatible %s format!", ResourceTypeName);
					PCFormat = EmuXB2PC_D3DFormat(X_Format);
				}
			}
		}

		// Update D3DPool accordingly to the active D3DUsage flags
		if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
			D3DPool = XTL::D3DPOOL_DEFAULT;
		}
		if (D3DUsage & D3DUSAGE_RENDERTARGET) {
			D3DPool = XTL::D3DPOOL_DEFAULT;
		}
		if (D3DUsage & D3DUSAGE_DYNAMIC) {
			D3DPool = XTL::D3DPOOL_DEFAULT;
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
			UINT highestMipMapLevel = 0;
			UINT width = dwWidth; UINT height = dwHeight;
			while (width > 1 && height > 1) {
				width /= 2;
				height /= 2;
				highestMipMapLevel++;
			}

			// If the desired mip-map level was higher than the maximum possible, cap it
			// Test case: Shin Megami Tensei: Nine
			if (dwMipMapLevels > highestMipMapLevel + 1) {
				LOG_TEST_CASE("Too many mip-map levels");
				dwMipMapLevels = highestMipMapLevel + 1;
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

		// One of these will be created :
		XTL::IDirect3DSurface *pNewHostSurface = nullptr; // for X_D3DRTYPE_SURFACE
		XTL::IDirect3DVolume *pNewHostVolume = nullptr; // for X_D3DRTYPE_VOLUME
		XTL::IDirect3DTexture *pNewHostTexture = nullptr; // for X_D3DRTYPE_TEXTURE
		XTL::IDirect3DVolumeTexture *pNewHostVolumeTexture = nullptr; // for X_D3DRTYPE_VOLUMETEXTURE
		XTL::IDirect3DCubeTexture *pNewHostCubeTexture = nullptr; // for X_D3DRTYPE_CUBETEXTURE
		HRESULT hRet;

		// Create the surface/volume/(volume/cube/)texture
		switch (XboxResourceType) {
		case XTL::X_D3DRTYPE_SURFACE: {
			if (D3DUsage & D3DUSAGE_RENDERTARGET) {
				hRet = g_pD3DDevice->CreateRenderTarget(dwWidth, dwHeight, PCFormat,
					g_EmuCDPD.HostPresentationParameters.MultiSampleType,
					0, // MultisampleQuality
					true, // Lockable
					&pNewHostSurface,
					nullptr // pSharedHandle
				);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateRenderTarget");
			} else
			if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
				hRet = g_pD3DDevice->CreateDepthStencilSurface(dwWidth, dwHeight, PCFormat,
					g_EmuCDPD.HostPresentationParameters.MultiSampleType,
					0, // MultisampleQuality
					false, // Discard
					&pNewHostSurface,
					nullptr
				);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateDepthStencilSurface");
			}
			else {
				D3DPool = XTL::D3DPOOL_SYSTEMMEM;
				hRet = g_pD3DDevice->CreateOffscreenPlainSurface(dwWidth, dwHeight, PCFormat, D3DPool, &pNewHostSurface, nullptr);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateOffscreenPlainSurface");
			}

			// First fail, retry with a fallback format
			// If this succeeds, the surface may not render correctly, but it won't crash
			if (hRet != D3D_OK) {
				if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
					EmuLog(LOG_LEVEL::WARNING, "CreateDepthStencilSurface Failed\n\nError: %s\nDesc: %s",
						XTL::DXGetErrorString(hRet), XTL::DXGetErrorDescription(hRet));
				}
				else {
					EmuLog(LOG_LEVEL::WARNING, "CreateImageSurface Failed\n\nError: %s\nDesc: %s",
						XTL::DXGetErrorString(hRet), XTL::DXGetErrorDescription(hRet));
				}

				EmuLog(LOG_LEVEL::WARNING, "Trying Fallback");
				hRet = g_pD3DDevice->CreateOffscreenPlainSurface(dwWidth, dwHeight, PCFormat, D3DPool, &pNewHostSurface, nullptr);
			}

			// If the fallback failed, show an error and exit execution.
			if (hRet != D3D_OK) {
				// We cannot safely continue in this state.
				CxbxKrnlCleanup("CreateImageSurface Failed!\n\nError: %s\nDesc: %s",
					XTL::DXGetErrorString(hRet), XTL::DXGetErrorDescription(hRet));
			}

			SetHostSurface(pResource, pNewHostSurface);
			DBG_PRINTF("CreateHostResource : Successfully created %s (0x%.08X, 0x%.08X)\n",
				ResourceTypeName, pResource, pNewHostSurface);
			DBG_PRINTF("CreateHostResource : Width : %d, Height : %d, Format : %d\n",
				dwWidth, dwHeight, PCFormat);
			break;
		}

		case XTL::X_D3DRTYPE_VOLUME: {
			LOG_UNIMPLEMENTED();
			// Note : Host D3D can only(?) retrieve a volue like this : 
			// hRet = pNewHostVolumeTexture->GetVolumeLevel(level, &pNewHostVolume);
			// So, we need to do this differently - we need to step up to the containing VolumeTexture,
			// and retrieve and convert all of it's GetVolumeLevel() slices.
			pNewHostVolume = nullptr;
			// SetHostVolume(pResource, pNewHostVolume);
			// DBG_PRINTF("CreateHostResource : Successfully created %s (0x%.08X, 0x%.08X)\n",
			//	ResourceTypeName, pResource, pNewHostVolume);
			break;
		}

		case XTL::X_D3DRTYPE_TEXTURE: {
/* TODO : Enabled this if D3DPool is ever anything else but D3DPOOL_MANAGED :
			if (D3DPool == D3DPOOL_DEFAULT) {
				if ((D3DUsage & D3DUSAGE_DYNAMIC) == 0) {
					if ((D3DUsage & D3DUSAGE_RENDERTARGET) == 0) {
						D3DUsage |= D3DUSAGE_DYNAMIC;
					}
				}
			}
*/
			hRet = g_pD3DDevice->CreateTexture(dwWidth, dwHeight, dwMipMapLevels,
				D3DUsage, PCFormat, D3DPool, &pNewHostTexture,
				nullptr
			);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateTexture");

			// If the above failed, we might be able to use an ARGB texture instead
			if ((hRet != D3D_OK) && (PCFormat != XTL::D3DFMT_A8R8G8B8) && EmuXBFormatCanBeConvertedToARGB(X_Format)) {
				hRet = g_pD3DDevice->CreateTexture(dwWidth, dwHeight, dwMipMapLevels,
					D3DUsage, XTL::D3DFMT_A8R8G8B8, D3DPool, &pNewHostTexture,
					nullptr
				);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateTexture(D3DFMT_A8R8G8B8)");

				if (hRet == D3D_OK) {
					// Okay, now this works, make sure the texture gets converted
					bConvertToARGB = true;
					PCFormat = XTL::D3DFMT_A8R8G8B8;
				}
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

			SetHostTexture(pResource, pNewHostTexture);
			DBG_PRINTF("CreateHostResource : Successfully created %s (0x%.08X, 0x%.08X)\n",
				ResourceTypeName, pResource, pNewHostTexture);
			break;
		}

		case XTL::X_D3DRTYPE_VOLUMETEXTURE: {
			hRet = g_pD3DDevice->CreateVolumeTexture(dwWidth, dwHeight, dwDepth,
				dwMipMapLevels, D3DUsage, PCFormat, D3DPool, &pNewHostVolumeTexture,
				nullptr
			);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateVolumeTexture");

			if (hRet != D3D_OK) {
				CxbxKrnlCleanup("CreateVolumeTexture Failed!\n\nError: %s\nDesc: %s",
					XTL::DXGetErrorString(hRet), XTL::DXGetErrorDescription(hRet));
			}

			SetHostVolumeTexture(pResource, pNewHostVolumeTexture);
			DBG_PRINTF("CreateHostResource : Successfully created %s (0x%.08X, 0x%.08X)\n",
				ResourceTypeName, pResource, pNewHostVolumeTexture);
			break;
		}

		case XTL::X_D3DRTYPE_CUBETEXTURE: {
			DBG_PRINTF("CreateCubeTexture(%d, %d, 0, %d, D3DPOOL_MANAGED)\n", dwWidth,
				dwMipMapLevels, PCFormat);

			hRet = g_pD3DDevice->CreateCubeTexture(dwWidth, dwMipMapLevels, D3DUsage,
				PCFormat, D3DPool, &pNewHostCubeTexture,
				nullptr
			);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateCubeTexture");

			if (hRet != D3D_OK) {
				CxbxKrnlCleanup("CreateCubeTexture Failed!\n\nError: \nDesc: "/*,
					DXGetErrorString(hRet), DXGetErrorDescription(hRet)*/);
			}

			SetHostCubeTexture(pResource, pNewHostCubeTexture);
			// TODO : Because cube face surfaces can be used as a render-target,
			// we should call SetHostSurface() on all 6 faces, so that when Xbox
			// code accesses a face, the host counterpart is already registered!
			DBG_PRINTF("CreateHostResource : Successfully created %s (0x%.08X, 0x%.08X)\n",
				ResourceTypeName, pResource, pNewHostCubeTexture);
			break;
		}
		} // switch XboxResourceType

		DWORD D3DLockFlags = D3DLOCK_NOSYSLOCK;

		// D3DLOCK_DISCARD is only valid for D3DUSAGE_DYNAMIC
		if (g_D3DCaps.Caps2 & D3DCAPS2_DYNAMICTEXTURES) {
			if (D3DUsage & D3DUSAGE_DYNAMIC) {
				D3DLockFlags |= D3DLOCK_DISCARD;
			}
		}

		DWORD dwCubeFaceOffset = 0;
		DWORD dwCubeFaceSize = 0;
		XTL::D3DCUBEMAP_FACES last_face = (bCubemap) ? XTL::D3DCUBEMAP_FACE_NEGATIVE_Z : XTL::D3DCUBEMAP_FACE_POSITIVE_X;
		for (int face = XTL::D3DCUBEMAP_FACE_POSITIVE_X; face <= last_face; face++) {
			// As we iterate through mipmap levels, we'll adjust the source resource offset
			DWORD dwMipOffset = 0;
			DWORD dwMipWidth = dwWidth;
			DWORD dwMipHeight = dwHeight;
			DWORD dwMipDepth = dwDepth;
            DWORD dwMipRowPitch = dwRowPitch;
			DWORD dwSrcSlicePitch = dwMipRowPitch * dwMipHeight; // TODO

			for (uint mipmap_level = 0; mipmap_level < dwMipMapLevels; mipmap_level++) {

				// Calculate size of this mipmap level
				DWORD dwMipSize = dwMipRowPitch * dwMipHeight;
				if (bCompressed) {
					dwMipSize /= 4;
				}

				// Lock the host resource
				XTL::D3DLOCKED_RECT LockedRect = {};
				XTL::D3DLOCKED_BOX LockedBox = {};

				switch (XboxResourceType) {
				case XTL::X_D3DRTYPE_SURFACE:
					hRet = pNewHostSurface->LockRect(&LockedRect, nullptr, D3DLockFlags);
					break;
				case XTL::X_D3DRTYPE_VOLUME:
					hRet = pNewHostVolume->LockBox(&LockedBox, nullptr, D3DLockFlags);
					break;
				case XTL::X_D3DRTYPE_TEXTURE:
					hRet = pNewHostTexture->LockRect(mipmap_level, &LockedRect, nullptr, D3DLockFlags);
					break;
				case XTL::X_D3DRTYPE_VOLUMETEXTURE:
					hRet = pNewHostVolumeTexture->LockBox(mipmap_level, &LockedBox, nullptr, D3DLockFlags);
					break;
				case XTL::X_D3DRTYPE_CUBETEXTURE:
					hRet = pNewHostCubeTexture->LockRect((XTL::D3DCUBEMAP_FACES)face, mipmap_level, &LockedRect, nullptr, D3DLockFlags);
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
					DBG_PRINTF("Unsupported texture format, expanding to D3DFMT_A8R8G8B8\n");

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
					XTL::EmuUnswizzleBox(
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
							g_pCurrentPalette,
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
					hRet = pNewHostTexture->UnlockRect(mipmap_level);
					break;
				case XTL::X_D3DRTYPE_VOLUMETEXTURE:
					hRet = pNewHostVolumeTexture->UnlockBox(mipmap_level);
					break;
				case XTL::X_D3DRTYPE_CUBETEXTURE:
					hRet = pNewHostCubeTexture->UnlockRect((XTL::D3DCUBEMAP_FACES)face, mipmap_level);
					break;
				default:
					assert(false);
				}

				if (hRet != D3D_OK) {
					EmuLog(LOG_LEVEL::WARNING, "Unlocking host %s failed!", ResourceTypeName);
				}

				if (face == XTL::D3DCUBEMAP_FACE_POSITIVE_X) {
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

			if (face == XTL::D3DCUBEMAP_FACE_POSITIVE_X) {
				dwCubeFaceSize = ROUND_UP(dwCubeFaceSize, X_D3DTEXTURE_CUBEFACE_ALIGNMENT);
			}

			dwCubeFaceOffset += dwCubeFaceSize;
		} // for cube faces

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
				D3DXSaveSurfaceToFileA(szFilePath, XTL::D3DXIFF_DDS, pNewHostSurface, NULL, NULL);
				break;
			}
			case XTL::X_D3DRTYPE_VOLUME: {
				// TODO
				break;
			}
			case XTL::X_D3DRTYPE_TEXTURE: {
				static int dwDumpTexure = 0;
				sprintf(szFilePath, _DEBUG_DUMP_TEXTURE_REGISTER "%.03d-Texture%.03d.dds", X_Format, dwDumpTexure++);
				D3DXSaveTextureToFileA(szFilePath, XTL::D3DXIFF_DDS, pNewHostTexture, NULL);
				break;
			}
			case XTL::X_D3DRTYPE_VOLUMETEXTURE: {
				// TODO
				break;
			}
			case XTL::X_D3DRTYPE_CUBETEXTURE: {
				static int dwDumpCubeTexture = 0;
				for (uint face = XTL::D3DCUBEMAP_FACE_POSITIVE_X; face <= XTL::D3DCUBEMAP_FACE_NEGATIVE_Z; face++) {
					XTL::IDirect3DSurface8 *pSurface;
					if (D3D_OK == pNewHostCubeTexture->GetCubeMapSurface((XTL::D3DCUBEMAP_FACES)face, 0, &pSurface)) {
						sprintf(szFilePath, _DEBUG_DUMP_TEXTURE_REGISTER "%.03d-CubeTexure%.03d-%d.dds", X_Format, dwDumpCubeTexture, face);
						D3DXSaveSurfaceToFileA(szFilePath, XTL::D3DXIFF_DDS, pSurface, NULL, NULL);
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

		DBG_PRINTF("CreateHostResource : Successfully created %S (0x%.08X, 0x%.08X, 0x%.08X)\n", ResourceTypeName, pResource->Data, pPushBuffer->Size, pPushBuffer->AllocationSize);
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
// * patch: IDirect3DResource8_Release
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(D3DResource_Release)
(
	X_D3DResource      *pThis
)
{
	LOG_FUNC_ONE_ARG(pThis);

	// Backup the key now, as the Xbox resource may be wiped out by the following release call!
	auto key = GetHostResourceKey(pThis);

	// Call the Xbox version of D3DResource_Release and store the result
	XB_trampoline(ULONG, WINAPI, D3DResource_Release, (X_D3DResource*));

	ULONG uRet = XB_D3DResource_Release(pThis);

	// Was the Xbox resource freed?
	if (uRet == 0) {

		// If this was a cached render target or depth surface, clear the cache variable too!
		if (pThis == g_pXboxRenderTarget) {
			g_pXboxRenderTarget = nullptr;
		}

		if (pThis == g_pXboxDepthStencil) {
			g_pXboxDepthStencil = nullptr;
		}

		if (pThis == g_XboxBackBufferSurface) {
			g_XboxBackBufferSurface = nullptr;
		}

		// Also release the host copy (if it exists!)
		FreeHostResource(key); 
	}

    return uRet;
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
		if (g_OverlaySwap != g_SwapData.Swap - 1) {
			EMUPATCH(D3DDevice_Swap)(CXBX_SWAP_PRESENT_FORWARD);
		}

		g_OverlaySwap = g_SwapData.Swap;
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
    D3DVBLANKCALLBACK pCallback
)
{
	LOG_FUNC_ONE_ARG(pCallback);

    g_pVBCallback = pCallback;    
}

// LTCG specific D3DDevice_SetTextureState_TexCoordIndex function...
// This uses a custom calling convention where parameter is passed in EDI, EAX
// Test-case: Ski Racing 2006
VOID __stdcall XTL::EMUPATCH(D3DDevice_SetTextureState_TexCoordIndex_0)
(
)
{
	DWORD Stage;
	DWORD Value;

	__asm {
		mov Stage, edi
		mov Value, eax
	}

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Stage)
		LOG_FUNC_ARG(Value)
		LOG_FUNC_END;

	if (Value >= 0x00040000) {
		EmuLog(LOG_LEVEL::WARNING, "EmuD3DDevice_SetTextureState_TexCoordIndex: Unknown TexCoordIndex Value (0x%.08X)", Value);
		return;
	}

	HRESULT hRet = g_pD3DDevice->SetTextureStageState(Stage, D3DTSS_TEXCOORDINDEX, Value);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetTextureStageState");
}

// This uses a custom calling convention where parameter is passed in ESI
// Test-case: Metal Wolf Chaos
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetTextureState_TexCoordIndex_4)
(
    DWORD Value
)
{
	DWORD           Stage;
	__asm mov Stage, esi;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Stage)
		LOG_FUNC_ARG(Value)
		LOG_FUNC_END;

	// TODO: Xbox Direct3D supports sphere mapping OpenGL style.

	// BUG FIX: The lower 16 bits were causing false Unknown TexCoordIndex errors.
	// Check for 0x00040000 instead.

	if (Value >= 0x00040000) {
		EmuLog(LOG_LEVEL::WARNING, "EmuD3DDevice_SetTextureState_TexCoordIndex: Unknown TexCoordIndex Value (0x%.08X)", Value);
		return;
	}

    HRESULT hRet = g_pD3DDevice->SetTextureStageState(Stage, D3DTSS_TEXCOORDINDEX, Value);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetTextureStageState");
}

// ******************************************************************
// * patch: D3DDevice_SetTextureState_TexCoordIndex
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetTextureState_TexCoordIndex)
(
    DWORD Stage,
    DWORD Value
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Stage)
		LOG_FUNC_ARG(Value)
		LOG_FUNC_END;

	XB_trampoline(VOID, WINAPI, D3DDevice_SetTextureState_TexCoordIndex, (DWORD, DWORD));
	XB_D3DDevice_SetTextureState_TexCoordIndex(Stage, Value);

	// TODO: Xbox Direct3D supports sphere mapping OpenGL style.

	// BUG FIX: The lower 16 bits were causing false Unknown TexCoordIndex errors.
	// Check for 0x00040000 instead.

	if (Value >= 0x00040000) {
		EmuLog(LOG_LEVEL::WARNING, "EmuD3DDevice_SetTextureState_TexCoordIndex: Unknown TexCoordIndex Value (0x%.08X)", Value);
		return;
	}

    HRESULT hRet = g_pD3DDevice->SetTextureStageState(Stage, D3DTSS_TEXCOORDINDEX, Value);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetTextureStageState");
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_TwoSidedLighting
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_TwoSidedLighting)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_TwoSidedLighting, (DWORD));
	XB_D3DDevice_SetRenderState_TwoSidedLighting(Value);

	LOG_NOT_SUPPORTED();
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_BackFillMode
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_BackFillMode)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	// blueshogun96 12/4/07
	// I haven't had access to Cxbx sources in a few months, great to be back :)
	//
	// Anyway, since standard Direct3D doesn't support the back fill mode
	// operation, this function will be ignored.  Things like this make me
	// think even more that an OpenGL port wouldn't hurt since OpenGL supports
	// nearly all of the missing features that Direct3D lacks.  The Xbox's version
	// of Direct3D was specifically created to take advantage of certain NVIDIA
	// GPU registers and provide more OpenGL-like features IHMO.
	LOG_NOT_SUPPORTED();

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_BackFillMode, (DWORD));
	XB_D3DDevice_SetRenderState_BackFillMode(Value);
}

// LTCG specific D3DDevice_SetTextureState_BorderColor function...
// This uses a custom calling convention where parameter is passed in EAX, EBX
// Test-case: Metal Wolf Chaos
VOID XTL::EMUPATCH(D3DDevice_SetTextureState_BorderColor_0)
(
)
{
	DWORD Stage;
	DWORD Value;

	__asm {
		mov Stage, eax
		mov Value, ebx
	}


	HRESULT hRet;
	hRet = g_pD3DDevice->SetSamplerState(Stage, D3DSAMP_BORDERCOLOR, Value);
	//DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetTextureStageState");
}

// This uses a custom calling convention where parameter is passed in EAX
// TODO: Log function is not working due lost parameter in EAX.
// Test-case: Murakumo
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetTextureState_BorderColor_4)
(
    DWORD Value
)
{
    DWORD Stage;
	__asm mov Stage, eax;

	//LOG_FUNC_BEGIN
	//	LOG_FUNC_ARG(Stage)
	//	LOG_FUNC_ARG(Value)
	//	LOG_FUNC_END;
	DBG_PRINTF("D3DDevice_SetTextureState_BorderColor_4(Stage : %d Value : %d);\n", Stage, Value);

    HRESULT hRet;
	hRet = g_pD3DDevice->SetSamplerState(Stage, D3DSAMP_BORDERCOLOR, Value);
	//DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetTextureStageState");
}

// ******************************************************************
// * patch: D3DDevice_SetTextureState_BorderColor
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetTextureState_BorderColor)
(
    DWORD Stage,
    DWORD Value
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Stage)
		LOG_FUNC_ARG(Value)
		LOG_FUNC_END;

	XB_trampoline(VOID, WINAPI, D3DDevice_SetTextureState_BorderColor, (DWORD, DWORD));
	XB_D3DDevice_SetTextureState_BorderColor(Stage, Value);

    HRESULT hRet;
	hRet = g_pD3DDevice->SetSamplerState(Stage, D3DSAMP_BORDERCOLOR, Value);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetTextureStageState");
}

// LTCG specific D3DDevice_SetTextureState_ColorKeyColor function...
// This uses a custom calling convention where parameter is passed in ESI, EBX
// Test-case: Metal Wolf Chaos
VOID XTL::EMUPATCH(D3DDevice_SetTextureState_ColorKeyColor_0)
(
)
{
	DWORD Stage;
	DWORD Value;

	__asm {
		mov Stage, esi
		mov Value, ebx
	}

	LOG_NOT_SUPPORTED();
}

// This uses a custom calling convention where parameter is passed in EAX
// Test-case: Murakumo
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetTextureState_ColorKeyColor_4)
(
    DWORD Value
)
{
    DWORD Stage;
	__asm mov Stage, eax;

	LOG_NOT_SUPPORTED();
}

// ******************************************************************
// * patch: D3DDevice_SetTextureState_ColorKeyColor
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetTextureState_ColorKeyColor)
(
    DWORD Stage,
    DWORD Value
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Stage)
		LOG_FUNC_ARG(Value)
		LOG_FUNC_END;

	XB_trampoline(VOID, WINAPI, D3DDevice_SetTextureState_ColorKeyColor, (DWORD, DWORD));
	XB_D3DDevice_SetTextureState_ColorKeyColor(Stage, Value);

	LOG_NOT_SUPPORTED();
}

// LTCG specific D3DDevice_SetTextureState_BumpEnv function...
// This uses a custom calling convention where parameter is passed in EAX
// TODO: Log function is not working due lost parameter in EAX.
// Test-case: Metal Wolf Chaos
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetTextureState_BumpEnv_8)
(
    X_D3DTEXTURESTAGESTATETYPE Type,
    DWORD                      Value
)
{
	DWORD           Stage;
	__asm mov Stage, eax;

	//LOG_FUNC_BEGIN
	//	LOG_FUNC_ARG(Stage)
	//	LOG_FUNC_ARG(Type)
	//	LOG_FUNC_ARG(Value)
	//	LOG_FUNC_END;
	DBG_PRINTF("D3DDevice_SetTextureState_BumpEnv_8(Stage : %d Type : %d Value : %d);\n", Stage, Type, Value);

	HRESULT hRet = D3D_OK;

    switch(Type)
    {
        case 22:    // X_D3DTSS_BUMPENVMAT00
            hRet = g_pD3DDevice->SetTextureStageState(Stage, D3DTSS_BUMPENVMAT00, Value);
            break;
        case 23:    // X_D3DTSS_BUMPENVMAT01
			hRet = g_pD3DDevice->SetTextureStageState(Stage, D3DTSS_BUMPENVMAT01, Value);
			break;
        case 24:    // X_D3DTSS_BUMPENVMAT11
			hRet = g_pD3DDevice->SetTextureStageState(Stage, D3DTSS_BUMPENVMAT11, Value);
            break;
        case 25:    // X_D3DTSS_BUMPENVMAT10
			hRet = g_pD3DDevice->SetTextureStageState(Stage, D3DTSS_BUMPENVMAT10, Value);
            break;
        case 26:    // X_D3DTSS_BUMPENVLSCALE
			hRet = g_pD3DDevice->SetTextureStageState(Stage, D3DTSS_BUMPENVLSCALE, Value);
            break;
        case 27:    // X_D3DTSS_BUMPENVLOFFSET
            hRet = g_pD3DDevice->SetTextureStageState(Stage, D3DTSS_BUMPENVLOFFSET, Value);
            break;
    }

	//DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetTextureStageState");
}

// ******************************************************************
// * patch: D3DDevice_SetTextureState_BumpEnv
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetTextureState_BumpEnv)
(
    DWORD                      Stage,
    X_D3DTEXTURESTAGESTATETYPE Type,
    DWORD                      Value
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Stage)
		LOG_FUNC_ARG(Type)
		LOG_FUNC_ARG(Value)
		LOG_FUNC_END;

	XB_trampoline(VOID, WINAPI, D3DDevice_SetTextureState_BumpEnv, (DWORD, X_D3DTEXTURESTAGESTATETYPE, DWORD));
	XB_D3DDevice_SetTextureState_BumpEnv(Stage, Type, Value);

	HRESULT hRet = D3D_OK;

    switch(Type)
    {
        case 22:    // X_D3DTSS_BUMPENVMAT00
            hRet = g_pD3DDevice->SetTextureStageState(Stage, D3DTSS_BUMPENVMAT00, Value);
            break;
        case 23:    // X_D3DTSS_BUMPENVMAT01
			hRet = g_pD3DDevice->SetTextureStageState(Stage, D3DTSS_BUMPENVMAT01, Value);
			break;
        case 24:    // X_D3DTSS_BUMPENVMAT11
			hRet = g_pD3DDevice->SetTextureStageState(Stage, D3DTSS_BUMPENVMAT11, Value);
            break;
        case 25:    // X_D3DTSS_BUMPENVMAT10
			hRet = g_pD3DDevice->SetTextureStageState(Stage, D3DTSS_BUMPENVMAT10, Value);
            break;
        case 26:    // X_D3DTSS_BUMPENVLSCALE
			hRet = g_pD3DDevice->SetTextureStageState(Stage, D3DTSS_BUMPENVLSCALE, Value);
            break;
        case 27:    // X_D3DTSS_BUMPENVLOFFSET
			hRet = g_pD3DDevice->SetTextureStageState(Stage, D3DTSS_BUMPENVLOFFSET, Value);
            break;
    }

	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetTextureStageState");
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_FrontFace
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_FrontFace)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_FrontFace, (DWORD));
	XB_D3DDevice_SetRenderState_FrontFace(Value);

	LOG_NOT_SUPPORTED();
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_LogicOp
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_LogicOp)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_LogicOp, (DWORD));
	XB_D3DDevice_SetRenderState_LogicOp(Value);

	LOG_NOT_SUPPORTED();
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_NormalizeNormals
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_NormalizeNormals)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_NormalizeNormals, (DWORD));
	XB_D3DDevice_SetRenderState_NormalizeNormals(Value);

    HRESULT hRet = g_pD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, Value);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetRenderState");
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_TextureFactor
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_TextureFactor)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_TextureFactor, (DWORD));
	XB_D3DDevice_SetRenderState_TextureFactor(Value);

	HRESULT hRet = g_pD3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, Value);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetRenderState");
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_ZBias
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_ZBias)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	HRESULT hRet;

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_ZBias, (DWORD));
	XB_D3DDevice_SetRenderState_ZBias(Value);

	FLOAT Biased = static_cast<FLOAT>(Value) * -0.000005f;
	Value = *reinterpret_cast<const DWORD *>(&Biased);

	hRet = g_pD3DDevice->SetRenderState(D3DRS_DEPTHBIAS, Value);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetRenderState");
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_EdgeAntiAlias
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_EdgeAntiAlias)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_EdgeAntiAlias, (DWORD));
	XB_D3DDevice_SetRenderState_EdgeAntiAlias(Value);

	//  TODO: Analyze performance and compatibility (undefined behavior on PC with triangles or points)
	HRESULT hRet = g_pD3DDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, Value); // Was D3DRS_EDGEANTIALIAS (no longer exists)
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetRenderState");
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_FillMode
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_FillMode)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_FillMode, (DWORD));
	XB_D3DDevice_SetRenderState_FillMode(Value);

    DWORD dwFillMode;

	if(Value != 0)
		g_CurrentFillMode = EmuXB2PC_D3DFILLMODE(Value);

    if(g_iWireframe == 0)
        dwFillMode = g_CurrentFillMode;
    else if(g_iWireframe == 1)
        dwFillMode = D3DFILL_WIREFRAME;
    else
        dwFillMode = D3DFILL_POINT;

	HRESULT hRet = g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, dwFillMode);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetRenderState");
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_FogColor
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_FogColor)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_FogColor, (DWORD));
	XB_D3DDevice_SetRenderState_FogColor(Value);

	HRESULT hRet = g_pD3DDevice->SetRenderState(D3DRS_FOGCOLOR, Value);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetRenderState");
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_Dxt1NoiseEnable)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_Dxt1NoiseEnable, (DWORD));
	XB_D3DDevice_SetRenderState_Dxt1NoiseEnable(Value);

	LOG_UNIMPLEMENTED();
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

	// Special Case: Handle PixelShader related Render States
	// TODO: Port over EmuMappedD3DRenderState and related code from Dxbx or Wip_LessVertexPatching
	// After this, we don't need to do this part anymore
	switch (Method & 0x00001FFC) {
		case NV2A_RC_IN_ALPHA(0): TemporaryPixelShaderRenderStates[X_D3DRS_PSALPHAINPUTS0] = Value; return;
		case NV2A_RC_IN_ALPHA(1): TemporaryPixelShaderRenderStates[X_D3DRS_PSALPHAINPUTS1] = Value; return;
		case NV2A_RC_IN_ALPHA(2): TemporaryPixelShaderRenderStates[X_D3DRS_PSALPHAINPUTS2] = Value; return;
		case NV2A_RC_IN_ALPHA(3): TemporaryPixelShaderRenderStates[X_D3DRS_PSALPHAINPUTS3] = Value; return;
		case NV2A_RC_IN_ALPHA(4): TemporaryPixelShaderRenderStates[X_D3DRS_PSALPHAINPUTS4] = Value; return;
		case NV2A_RC_IN_ALPHA(5): TemporaryPixelShaderRenderStates[X_D3DRS_PSALPHAINPUTS5] = Value; return;
		case NV2A_RC_IN_ALPHA(6): TemporaryPixelShaderRenderStates[X_D3DRS_PSALPHAINPUTS6] = Value; return;
		case NV2A_RC_IN_ALPHA(7): TemporaryPixelShaderRenderStates[X_D3DRS_PSALPHAINPUTS7] = Value; return;
		case NV2A_RC_FINAL0: TemporaryPixelShaderRenderStates[X_D3DRS_PSFINALCOMBINERINPUTSABCD] = Value; return;
		case NV2A_RC_FINAL1: TemporaryPixelShaderRenderStates[X_D3DRS_PSFINALCOMBINERINPUTSEFG] = Value; return;
		case NV2A_RC_CONSTANT_COLOR0(0): TemporaryPixelShaderRenderStates[X_D3DRS_PSCONSTANT0_0] = Value; return;
		case NV2A_RC_CONSTANT_COLOR0(1): TemporaryPixelShaderRenderStates[X_D3DRS_PSCONSTANT0_1] = Value; return;
		case NV2A_RC_CONSTANT_COLOR0(2): TemporaryPixelShaderRenderStates[X_D3DRS_PSCONSTANT0_2] = Value; return;
		case NV2A_RC_CONSTANT_COLOR0(3): TemporaryPixelShaderRenderStates[X_D3DRS_PSCONSTANT0_3] = Value; return;
		case NV2A_RC_CONSTANT_COLOR0(4): TemporaryPixelShaderRenderStates[X_D3DRS_PSCONSTANT0_4] = Value; return;
		case NV2A_RC_CONSTANT_COLOR0(5): TemporaryPixelShaderRenderStates[X_D3DRS_PSCONSTANT0_5] = Value; return;
		case NV2A_RC_CONSTANT_COLOR0(6): TemporaryPixelShaderRenderStates[X_D3DRS_PSCONSTANT0_6] = Value; return;
		case NV2A_RC_CONSTANT_COLOR0(7): TemporaryPixelShaderRenderStates[X_D3DRS_PSCONSTANT0_7] = Value; return;
		case NV2A_RC_CONSTANT_COLOR1(0): TemporaryPixelShaderRenderStates[X_D3DRS_PSCONSTANT1_0] = Value; return;
		case NV2A_RC_CONSTANT_COLOR1(1): TemporaryPixelShaderRenderStates[X_D3DRS_PSCONSTANT1_1] = Value; return;
		case NV2A_RC_CONSTANT_COLOR1(2): TemporaryPixelShaderRenderStates[X_D3DRS_PSCONSTANT1_2] = Value; return;
		case NV2A_RC_CONSTANT_COLOR1(3): TemporaryPixelShaderRenderStates[X_D3DRS_PSCONSTANT1_3] = Value; return;
		case NV2A_RC_CONSTANT_COLOR1(4): TemporaryPixelShaderRenderStates[X_D3DRS_PSCONSTANT1_4] = Value; return;
		case NV2A_RC_CONSTANT_COLOR1(5): TemporaryPixelShaderRenderStates[X_D3DRS_PSCONSTANT1_5] = Value; return;
		case NV2A_RC_CONSTANT_COLOR1(6): TemporaryPixelShaderRenderStates[X_D3DRS_PSCONSTANT1_6] = Value; return;
		case NV2A_RC_CONSTANT_COLOR1(7): TemporaryPixelShaderRenderStates[X_D3DRS_PSCONSTANT1_7] = Value; return;
		case NV2A_RC_OUT_ALPHA(0): TemporaryPixelShaderRenderStates[X_D3DRS_PSALPHAOUTPUTS0] = Value; return;
		case NV2A_RC_OUT_ALPHA(1): TemporaryPixelShaderRenderStates[X_D3DRS_PSALPHAOUTPUTS1] = Value; return;
		case NV2A_RC_OUT_ALPHA(2): TemporaryPixelShaderRenderStates[X_D3DRS_PSALPHAOUTPUTS2] = Value; return;
		case NV2A_RC_OUT_ALPHA(3): TemporaryPixelShaderRenderStates[X_D3DRS_PSALPHAOUTPUTS3] = Value; return;
		case NV2A_RC_OUT_ALPHA(4): TemporaryPixelShaderRenderStates[X_D3DRS_PSALPHAOUTPUTS4] = Value; return;
		case NV2A_RC_OUT_ALPHA(5): TemporaryPixelShaderRenderStates[X_D3DRS_PSALPHAOUTPUTS5] = Value; return;
		case NV2A_RC_OUT_ALPHA(6): TemporaryPixelShaderRenderStates[X_D3DRS_PSALPHAOUTPUTS6] = Value; return;
		case NV2A_RC_OUT_ALPHA(7): TemporaryPixelShaderRenderStates[X_D3DRS_PSALPHAOUTPUTS7] = Value; return;
		case NV2A_RC_IN_RGB(0): TemporaryPixelShaderRenderStates[X_D3DRS_PSRGBINPUTS0] = Value; return;
		case NV2A_RC_IN_RGB(1): TemporaryPixelShaderRenderStates[X_D3DRS_PSRGBINPUTS1] = Value; return;
		case NV2A_RC_IN_RGB(2): TemporaryPixelShaderRenderStates[X_D3DRS_PSRGBINPUTS2] = Value; return;
		case NV2A_RC_IN_RGB(3): TemporaryPixelShaderRenderStates[X_D3DRS_PSRGBINPUTS3] = Value; return;
		case NV2A_RC_IN_RGB(4): TemporaryPixelShaderRenderStates[X_D3DRS_PSRGBINPUTS4] = Value; return;
		case NV2A_RC_IN_RGB(5): TemporaryPixelShaderRenderStates[X_D3DRS_PSRGBINPUTS5] = Value; return;
		case NV2A_RC_IN_RGB(6): TemporaryPixelShaderRenderStates[X_D3DRS_PSRGBINPUTS6] = Value; return;
		case NV2A_RC_IN_RGB(7): TemporaryPixelShaderRenderStates[X_D3DRS_PSRGBINPUTS7] = Value; return;
		case NV2A_TX_SHADER_CULL_MODE: TemporaryPixelShaderRenderStates[X_D3DRS_PSCOMPAREMODE] = Value; return;
		case NV2A_RC_COLOR0: TemporaryPixelShaderRenderStates[X_D3DRS_PSFINALCOMBINERCONSTANT0] = Value; return;
		case NV2A_RC_COLOR1: TemporaryPixelShaderRenderStates[X_D3DRS_PSFINALCOMBINERCONSTANT1] = Value; return;
		case NV2A_RC_OUT_RGB(0): TemporaryPixelShaderRenderStates[X_D3DRS_PSRGBOUTPUTS0] = Value; return;
		case NV2A_RC_OUT_RGB(1): TemporaryPixelShaderRenderStates[X_D3DRS_PSRGBOUTPUTS1] = Value; return;
		case NV2A_RC_OUT_RGB(2): TemporaryPixelShaderRenderStates[X_D3DRS_PSRGBOUTPUTS2] = Value; return;
		case NV2A_RC_OUT_RGB(3): TemporaryPixelShaderRenderStates[X_D3DRS_PSRGBOUTPUTS3] = Value; return;
		case NV2A_RC_OUT_RGB(4): TemporaryPixelShaderRenderStates[X_D3DRS_PSRGBOUTPUTS4] = Value; return;
		case NV2A_RC_OUT_RGB(5): TemporaryPixelShaderRenderStates[X_D3DRS_PSRGBOUTPUTS5] = Value; return;
		case NV2A_RC_OUT_RGB(6): TemporaryPixelShaderRenderStates[X_D3DRS_PSRGBOUTPUTS6] = Value; return;
		case NV2A_RC_OUT_RGB(7): TemporaryPixelShaderRenderStates[X_D3DRS_PSRGBOUTPUTS7] = Value; return;
		case NV2A_RC_ENABLE: TemporaryPixelShaderRenderStates[X_D3DRS_PSCOMBINERCOUNT] = Value; return;
		case NV2A_NOP: TemporaryPixelShaderRenderStates[X_D3DRS_PS_RESERVED] = Value; return; // Dxbx note : This takes the slot of X_D3DPIXELSHADERDEF.PSTextureModes, set by D3DDevice_SetRenderState_LogicOp?
		case NV2A_TX_SHADER_DOTMAPPING: TemporaryPixelShaderRenderStates[X_D3DRS_PSDOTMAPPING] = Value; return;
		case NV2A_TX_SHADER_PREVIOUS: TemporaryPixelShaderRenderStates[X_D3DRS_PSINPUTTEXTURE] = Value; return;	
	}

	// Fallback to non-shader render state handling
	int State = -1;

	// Todo: make this faster and more elegant
	for (int v = 0; v<174; v++)
	{
		if (EmuD3DRenderStateSimpleEncoded[v] == Method)
		{
			State = v;
			break;
		}
	}

	if (State == -1) {
		// Attempt to determine renderstate name for unsupported types
		std::string name = "Unknown";
		for (int i = 0; i <= X_D3DRS_DONOTCULLUNCOMPRESSED; i++) {
			if (DxbxRenderStateInfo[i].M == (Method & 0x00001FFC)) {
				name = DxbxRenderStateInfo[i].S;
				break;
			}
		}

		EmuLog(LOG_LEVEL::WARNING, "RenderState_Simple(0x%.08X (%s), 0x%.08X) is unsupported!", Method, name.c_str(), Value);
	}
    else
    {
        switch(State)
        {
            case D3DRS_COLORWRITEENABLE:
            {
                DWORD OrigValue = Value;

                Value = 0;

                if(OrigValue & (1L<<16))
                    Value |= D3DCOLORWRITEENABLE_RED;
                if(OrigValue & (1L<<8))
                    Value |= D3DCOLORWRITEENABLE_GREEN;
                if(OrigValue & (1L<<0))
                    Value |= D3DCOLORWRITEENABLE_BLUE;
                if(OrigValue & (1L<<24))
                    Value |= D3DCOLORWRITEENABLE_ALPHA;

                DBG_PRINTF("D3DRS_COLORWRITEENABLE := 0x%.08X\n", Value);
            }
            break;

            case D3DRS_SHADEMODE:
                Value = EmuXB2PC_D3DSHADEMODE(Value);
                DBG_PRINTF("D3DRS_SHADEMODE := 0x%.08X\n", Value);
                break;

            case D3DRS_BLENDOP:
                Value = EmuXB2PC_D3DBLENDOP(Value);
                DBG_PRINTF("D3DRS_BLENDOP := 0x%.08X\n", Value);
                break;

            case D3DRS_SRCBLEND:
                Value = EmuXB2PC_D3DBLEND(Value);
                DBG_PRINTF("D3DRS_SRCBLEND := 0x%.08X\n", Value);
                break;

            case D3DRS_DESTBLEND:
                Value = EmuXB2PC_D3DBLEND(Value);
                DBG_PRINTF("D3DRS_DESTBLEND := 0x%.08X\n", Value);
                break;

            case D3DRS_ZFUNC:
                Value = EmuXB2PC_D3DCMPFUNC(Value);
                DBG_PRINTF("D3DRS_ZFUNC := 0x%.08X\n", Value);
                break;

            case D3DRS_ALPHAFUNC:
                Value = EmuXB2PC_D3DCMPFUNC(Value);
                DBG_PRINTF("D3DRS_ALPHAFUNC := 0x%.08X\n", Value);
                break;

            case D3DRS_ALPHATESTENABLE:
                DBG_PRINTF("D3DRS_ALPHATESTENABLE := 0x%.08X\n", Value);
                break;

            case D3DRS_ALPHABLENDENABLE:
                DBG_PRINTF("D3DRS_ALPHABLENDENABLE := 0x%.08X\n", Value);
                break;

            case D3DRS_ALPHAREF:
                DBG_PRINTF("D3DRS_ALPHAREF := %lf\n", DWtoF(Value));
                break;

            case D3DRS_ZWRITEENABLE:
                DBG_PRINTF("D3DRS_ZWRITEENABLE := 0x%.08X\n", Value);
                break;

            case D3DRS_DITHERENABLE:
                DBG_PRINTF("D3DRS_DITHERENABLE := 0x%.08X\n", Value);
                break;

			case D3DRS_STENCILZFAIL:
				Value = EmuXB2PC_D3DSTENCILOP(Value);
				DBG_PRINTF("D3DRS_STENCILZFAIL := 0x%.08X\n", Value);
				break;

			case D3DRS_STENCILPASS:
				Value = EmuXB2PC_D3DSTENCILOP(Value);
				DBG_PRINTF("D3DRS_STENCILPASS := 0x%.08X\n", Value);
				break;

			case D3DRS_STENCILFUNC:
				Value = EmuXB2PC_D3DCMPFUNC(Value);
				DBG_PRINTF("D3DRS_STENCILFUNC := 0x%.08X\n", Value);
				break;

			case D3DRS_STENCILREF:
				DBG_PRINTF("D3DRS_STENCILREF := 0x%.08X\n", Value);
				break;

			case D3DRS_STENCILMASK:
				DBG_PRINTF("D3DRS_STENCILMASK := 0x%.08X\n", Value);
				break;

			case D3DRS_STENCILWRITEMASK:
				DBG_PRINTF("D3DRS_STENCILWRITEMASK := 0x%.08X\n", Value);
				break;

            default:
                CxbxKrnlCleanup("Unsupported RenderState (0x%.08X)", State);
                break;
        };

        // TODO: verify these params as you add support for them!
        HRESULT hRet = g_pD3DDevice->SetRenderState((D3DRENDERSTATETYPE)State, Value);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetRenderState");
	}
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_VertexBlend
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_VertexBlend)
(
    DWORD Value
)
{
    LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_VertexBlend, (DWORD));
	XB_D3DDevice_SetRenderState_VertexBlend(Value);

    // convert from Xbox direct3d to PC direct3d enumeration
    if(Value <= 1) {
        Value = Value;
    } else if(Value == 3) {
        Value = 2;
    } else if(Value == 5) {
        Value = 3;
    } else {
        LOG_TEST_CASE("Unsupported D3DVERTEXBLENDFLAGS (%d)");
        return;
	}

    HRESULT hRet = g_pD3DDevice->SetRenderState(D3DRS_VERTEXBLEND, Value);
    DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetRenderState");
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_PSTextureModes
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_PSTextureModes)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_PSTextureModes, (DWORD));
	XB_D3DDevice_SetRenderState_PSTextureModes(Value);

	XTL::TemporaryPixelShaderRenderStates[XTL::X_D3DRS_PSTEXTUREMODES] = Value;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_CullMode
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_CullMode)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_CullMode, (DWORD));
	XB_D3DDevice_SetRenderState_CullMode(Value);

    // convert from Xbox D3D to PC D3D enumeration
    // TODO: XDK-Specific Tables? So far they are the same
    switch(Value)
    {
        case X_D3DCULL_NONE:
            Value = D3DCULL_NONE;
            break;
        case X_D3DCULL_CW:
            Value = D3DCULL_CW;
            break;
        case X_D3DCULL_CCW:
            Value = D3DCULL_CCW;
            break;
        default:
            CxbxKrnlCleanup("EmuD3DDevice_SetRenderState_CullMode: Unknown Cullmode (%d)", Value);
    }

	HRESULT hRet = g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, Value);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetRenderState");
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_LineWidth
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_LineWidth)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_LineWidth, (DWORD));
	XB_D3DDevice_SetRenderState_LineWidth(Value);

    // TODO: Convert to PC format??
//    g_pD3DDevice->SetRenderState(D3DRS_LINEPATTERN, Value);
	LOG_NOT_SUPPORTED();
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_StencilFail
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_StencilFail)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_StencilFail, (DWORD));
	XB_D3DDevice_SetRenderState_StencilFail(Value);

	HRESULT hRet = g_pD3DDevice->SetRenderState(D3DRS_STENCILFAIL, Value);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetRenderState");
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_OcclusionCullEnable
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_OcclusionCullEnable)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_OcclusionCullEnable, (DWORD));
	XB_D3DDevice_SetRenderState_OcclusionCullEnable(Value);

	LOG_NOT_SUPPORTED();
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_StencilCullEnable
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_StencilCullEnable)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_StencilCullEnable, (DWORD));
	XB_D3DDevice_SetRenderState_StencilCullEnable(Value);

	LOG_NOT_SUPPORTED();
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_RopZCmpAlwaysRead
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_RopZCmpAlwaysRead)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_RopZCmpAlwaysRead, (DWORD));
	XB_D3DDevice_SetRenderState_RopZCmpAlwaysRead(Value);

	LOG_NOT_SUPPORTED();
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_RopZRead
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_RopZRead)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_RopZRead, (DWORD));
	XB_D3DDevice_SetRenderState_RopZRead(Value);

	LOG_NOT_SUPPORTED();
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_DoNotCullUncompressed
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_DoNotCullUncompressed)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_DoNotCullUncompressed, (DWORD));
	XB_D3DDevice_SetRenderState_DoNotCullUncompressed(Value);

	LOG_NOT_SUPPORTED();
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_ZEnable
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_ZEnable)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_ZEnable, (DWORD));
	XB_D3DDevice_SetRenderState_ZEnable(Value);

	HRESULT hRet = g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, Value);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetRenderState");
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_StencilEnable
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_StencilEnable)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_StencilEnable, (DWORD));
	XB_D3DDevice_SetRenderState_StencilEnable(Value);

	HRESULT hRet = g_pD3DDevice->SetRenderState(D3DRS_STENCILENABLE, Value);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetRenderState");
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_MultiSampleAntiAlias)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_MultiSampleAntiAlias, (DWORD));
	XB_D3DDevice_SetRenderState_MultiSampleAntiAlias(Value);

	HRESULT hRet = g_pD3DDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, Value);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetRenderState");
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_MultiSampleMask
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_MultiSampleMask)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_MultiSampleMask, (DWORD));
	XB_D3DDevice_SetRenderState_MultiSampleMask(Value);

	HRESULT hRet = g_pD3DDevice->SetRenderState(D3DRS_MULTISAMPLEMASK, Value);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetRenderState");
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_MultiSampleMode
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_MultiSampleMode)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_MultiSampleMode, (DWORD));
	XB_D3DDevice_SetRenderState_MultiSampleMode(Value);

	LOG_NOT_SUPPORTED();
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_MultiSampleRenderTargetMode
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_MultiSampleRenderTargetMode)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_MultiSampleRenderTargetMode, (DWORD));
	XB_D3DDevice_SetRenderState_MultiSampleRenderTargetMode(Value);

	LOG_NOT_SUPPORTED();
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_ShadowFunc
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_ShadowFunc)
(
    DWORD Value
)
{
	LOG_FUNC_ONE_ARG(Value);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_ShadowFunc, (DWORD));
	XB_D3DDevice_SetRenderState_ShadowFunc(Value);

    // ShadowFunc reflects the following Xbox-only extension
    //
    // typedef enum _D3DRENDERSTATETYPE {
    //   ...
    //   D3DRS_SHADOWFUNC = 156, // D3DCMPFUNC
    //   ...
    // } D3DRENDERSTATETYPE;
    //
    // Value is a member of the D3DCMPFUNC enumeration that 
    // specifies what function to use with a shadow buffer. 
    // The default value is D3DCMP_NEVER. 

    // EmuXB2PC_D3DCMPFUNC(Value);

    // this warning just gets annoying
    // LOG_UNIMPLEMENTED();	
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
	DBG_PRINTF("D3DDevice_SetStreamSource_4(StreamNumber : %08X pStreamData : %08X Stride : %08X);\n", StreamNumber, pStreamData, Stride);

	// Forward to Xbox implementation
	// This should stop us having to patch GetStreamSource!
	//XB_trampoline(VOID, WINAPI, D3DDevice_SetStreamSource_4, (UINT, X_D3DVertexBuffer*, UINT));
	//XB_D3DDevice_SetStreamSource_4(StreamNumber, pStreamData, Stride);

	if (StreamNumber < 16) {
		g_D3DStreams[StreamNumber] = pStreamData;
		g_D3DStreamStrides[StreamNumber] = Stride;
	}
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
	DBG_PRINTF("D3DDevice_SetStreamSource_8(StreamNumber : %08X pStreamData : %08X Stride : %08X);\n", StreamNumber, pStreamData, Stride);

	// Forward to Xbox implementation
	// This should stop us having to patch GetStreamSource!
	//XB_trampoline(VOID, WINAPI, D3DDevice_SetStreamSource_8, (X_D3DVertexBuffer*, UINT));
	//XB_D3DDevice_SetStreamSource_8(pStreamData, Stride);

	if (StreamNumber < 16) {
		g_D3DStreams[StreamNumber] = pStreamData;
		g_D3DStreamStrides[StreamNumber] = Stride;
	}
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

	// Forward to Xbox implementation
	// This should stop us having to patch GetStreamSource!
	XB_trampoline(VOID, WINAPI, D3DDevice_SetStreamSource, (UINT, X_D3DVertexBuffer*, UINT));
	XB_D3DDevice_SetStreamSource(StreamNumber, pStreamData, Stride);

	if (StreamNumber < 16) {
		g_D3DStreams[StreamNumber] = pStreamData;
		g_D3DStreamStrides[StreamNumber] = Stride;
	}
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

    g_CurrentXboxVertexShaderHandle = Handle;
    // Store viewport offset and scale in constant registers 58 (c-38) and
    // 59 (c-37) used for screen space transformation.
    if(g_VertexShaderConstantMode != X_D3DSCM_NORESERVEDCONSTANTS)
    {
		D3DVIEWPORT ViewPort;
		g_pD3DDevice->GetViewport(&ViewPort);

        float vScale[] = { (2.0f / ViewPort.Width), (-2.0f / ViewPort.Height), 0.0f, 0.0f };
        static float vOffset[] = { -1.0f, 1.0f, 0.0f, 1.0f };

        g_pD3DDevice->SetVertexShaderConstantF(58, vScale, 1);
        g_pD3DDevice->SetVertexShaderConstantF(59, vOffset, 1);
    }

	if (VshHandleIsVertexShader(Handle)) {
 		CxbxVertexShader *pVertexShader = GetCxbxVertexShader(Handle);
		hRet = g_pD3DDevice->SetVertexDeclaration(pVertexShader->pHostDeclaration);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexDeclaration");
		hRet = g_pD3DDevice->SetVertexShader((XTL::IDirect3DVertexShader9*)pVertexShader->Handle);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexShader(VshHandleIsVertexShader)");
	} else {
		hRet = g_pD3DDevice->SetVertexShader(nullptr);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexShader");
		hRet = g_pD3DDevice->SetFVF(Handle);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetFVF");
	}
}

// TODO : Move to own file
constexpr UINT QuadToTriangleVertexCount(UINT NrOfQuadVertices)
{
	return (NrOfQuadVertices * VERTICES_PER_TRIANGLE * TRIANGLES_PER_QUAD) / VERTICES_PER_QUAD;
}

// TODO : Move to own file
bool WindingClockwise = true;
constexpr uint IndicesPerPage = PAGE_SIZE / sizeof(XTL::INDEX16);
constexpr uint InputQuadsPerPage = ((IndicesPerPage * VERTICES_PER_QUAD) / VERTICES_PER_TRIANGLE) / TRIANGLES_PER_QUAD;

// TODO : Move to own file
XTL::INDEX16 *CxbxAssureQuadListIndexBuffer(UINT NrOfQuadVertices)
{
	if (QuadToTriangleIndexBuffer_Size < NrOfQuadVertices)
	{
		QuadToTriangleIndexBuffer_Size = RoundUp(NrOfQuadVertices, InputQuadsPerPage);

		UINT NrOfTriangleVertices = QuadToTriangleVertexCount(QuadToTriangleIndexBuffer_Size);

		if (pQuadToTriangleIndexBuffer != nullptr)
			free(pQuadToTriangleIndexBuffer);

		pQuadToTriangleIndexBuffer = (XTL::INDEX16 *)malloc(sizeof(XTL::INDEX16) * NrOfTriangleVertices);

		UINT i = 0;
		XTL::INDEX16 j = 0;
		while (i + 6 < NrOfTriangleVertices)
		{
			if (WindingClockwise) {
				// ABCD becomes ABC+CDA, so this is triangle 1 :
				pQuadToTriangleIndexBuffer[i + 0] = j + 0;
				pQuadToTriangleIndexBuffer[i + 1] = j + 1;
				pQuadToTriangleIndexBuffer[i + 2] = j + 2;
				i += VERTICES_PER_TRIANGLE;

				// And this is triangle 2 :
				pQuadToTriangleIndexBuffer[i + 0] = j + 2;
				pQuadToTriangleIndexBuffer[i + 1] = j + 3;
				pQuadToTriangleIndexBuffer[i + 2] = j + 0;
				i += VERTICES_PER_TRIANGLE;
			}
			else
			{
				// ABCD becomes ADC+CBA, so this is triangle 1 :
				pQuadToTriangleIndexBuffer[i + 0] = j + 0;
				pQuadToTriangleIndexBuffer[i + 1] = j + 3;
				pQuadToTriangleIndexBuffer[i + 2] = j + 2;
				i += VERTICES_PER_TRIANGLE;

				// And this is triangle 2 :
				pQuadToTriangleIndexBuffer[i + 0] = j + 2;
				pQuadToTriangleIndexBuffer[i + 1] = j + 1;
				pQuadToTriangleIndexBuffer[i + 2] = j + 0;
				i += VERTICES_PER_TRIANGLE;
			}

			// Next quad, please :
			j += VERTICES_PER_QUAD;
		}
	}

	return pQuadToTriangleIndexBuffer;
}

// TODO : Move to own file
void CxbxAssureQuadListD3DIndexBuffer(UINT NrOfQuadVertices)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

	HRESULT hRet;

	if (QuadToTriangleD3DIndexBuffer_Size < NrOfQuadVertices)
	{
		// Round the number of indices up so we'll allocate whole pages
		QuadToTriangleD3DIndexBuffer_Size = RoundUp(NrOfQuadVertices, InputQuadsPerPage);
		UINT NrOfTriangleVertices = QuadToTriangleVertexCount(QuadToTriangleD3DIndexBuffer_Size); // 4 > 6
		UINT uiIndexBufferSize = sizeof(XTL::INDEX16) * NrOfTriangleVertices;

		// Create a new native index buffer of the above determined size :
		if (pQuadToTriangleD3DIndexBuffer != nullptr) {
			pQuadToTriangleD3DIndexBuffer->Release();
			pQuadToTriangleD3DIndexBuffer = nullptr;
		}

		hRet = g_pD3DDevice->CreateIndexBuffer(
			uiIndexBufferSize,
			D3DUSAGE_WRITEONLY,
			XTL::D3DFMT_INDEX16,
			XTL::D3DPOOL_MANAGED,
			&pQuadToTriangleD3DIndexBuffer,
			nullptr // pSharedHandle
		);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateIndexBuffer");

		if (FAILED(hRet))
			CxbxKrnlCleanup("CxbxAssureQuadListD3DIndexBuffer : IndexBuffer Create Failed!");

		// Put quadlist-to-triangle-list index mappings into this buffer :
		XTL::INDEX16* pIndexBufferData = nullptr;
		hRet = pQuadToTriangleD3DIndexBuffer->Lock(0, uiIndexBufferSize, (D3DLockData **)&pIndexBufferData, D3DLOCK_DISCARD);
		DEBUG_D3DRESULT(hRet, "pQuadToTriangleD3DIndexBuffer->Lock");

		if (pIndexBufferData == nullptr)
			CxbxKrnlCleanup("CxbxAssureQuadListD3DIndexBuffer : Could not lock index buffer!");

		memcpy(pIndexBufferData, CxbxAssureQuadListIndexBuffer(NrOfQuadVertices), uiIndexBufferSize);

		pQuadToTriangleD3DIndexBuffer->Unlock();
	}

	// Activate the new native index buffer :
	hRet = g_pD3DDevice->SetIndices(pQuadToTriangleD3DIndexBuffer);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetIndices");

	if (FAILED(hRet))
		CxbxKrnlCleanup("CxbxAssureQuadListD3DIndexBuffer : SetIndices Failed!"); // +DxbxD3DErrorString(hRet));
}

// TODO : Move to own file
// Calls SetIndices with a separate index-buffer, that's populated with the supplied indices.
void CxbxDrawIndexedClosingLine(XTL::INDEX16 LowIndex, XTL::INDEX16 HighIndex)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

	HRESULT hRet;

	const UINT uiIndexBufferSize = sizeof(XTL::INDEX16) * 2; // 4 bytes needed for 2 indices
	if (pClosingLineLoopIndexBuffer == nullptr) {
		hRet = g_pD3DDevice->CreateIndexBuffer(
			uiIndexBufferSize, 
			D3DUSAGE_WRITEONLY, 
			XTL::D3DFMT_INDEX16, 
			XTL::D3DPOOL_DEFAULT, 
			&pClosingLineLoopIndexBuffer,
			nullptr // pSharedHandle
		);
		if (FAILED(hRet))
			CxbxKrnlCleanup("Unable to create pClosingLineLoopIndexBuffer for D3DPT_LINELOOP emulation");
	}

	XTL::INDEX16 *pCxbxClosingLineLoopIndexBufferData = nullptr;
	hRet = pClosingLineLoopIndexBuffer->Lock(0, uiIndexBufferSize, (D3DLockData **)(&pCxbxClosingLineLoopIndexBufferData), D3DLOCK_DISCARD);
	DEBUG_D3DRESULT(hRet, "pClosingLineLoopIndexBuffer->Lock");

	pCxbxClosingLineLoopIndexBufferData[0] = LowIndex;
	pCxbxClosingLineLoopIndexBufferData[1] = HighIndex;

	hRet = pClosingLineLoopIndexBuffer->Unlock();
	DEBUG_D3DRESULT(hRet, "pClosingLineLoopIndexBuffer->Unlock");

	hRet = g_pD3DDevice->SetIndices(pClosingLineLoopIndexBuffer);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetIndices");

	hRet = g_pD3DDevice->DrawIndexedPrimitive(
		XTL::D3DPT_LINELIST,
		g_XboxBaseVertexIndex,
		LowIndex, // minIndex
		(HighIndex - LowIndex) + 1, // NumVertexIndices
		0, // startIndex
		1 // primCount
	);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitive(CxbxDrawIndexedClosingLine)");

	g_dwPrimPerFrame++;
}

// TODO : Move to own file
void CxbxDrawIndexedClosingLineUP(XTL::INDEX16 LowIndex, XTL::INDEX16 HighIndex, void *pHostVertexStreamZeroData, UINT uiHostVertexStreamZeroStride)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

	XTL::INDEX16 CxbxClosingLineIndices[2] = { LowIndex, HighIndex };

	HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitiveUP(
		XTL::D3DPT_LINELIST,
		LowIndex, // MinVertexIndex
		(HighIndex - LowIndex) + 1, // NumVertexIndices,
		1, // PrimitiveCount,
		CxbxClosingLineIndices, // pIndexData
		XTL::D3DFMT_INDEX16, // IndexDataFormat
		pHostVertexStreamZeroData,
		uiHostVertexStreamZeroStride
	);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitiveUP(CxbxDrawIndexedClosingLineUP)");

	g_dwPrimPerFrame++;
}

// Requires assigned pIndexData
// Called by D3DDevice_DrawIndexedVertices and EmuExecutePushBufferRaw (twice)
void XTL::CxbxDrawIndexed(CxbxDrawContext &DrawContext)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

	assert(DrawContext.dwStartVertex == 0);
	assert(DrawContext.pIndexData != nullptr);
	assert(IsValidCurrentShader());

	CxbxUpdateActiveIndexBuffer(DrawContext.pIndexData, DrawContext.dwVertexCount);

	CxbxVertexBufferConverter VertexBufferConverter = {};

	//Walk through index buffer
	// Determine highest and lowest index in use :
	INDEX16 LowIndex, HighIndex;
	WalkIndexBuffer(LowIndex, HighIndex, &(DrawContext.pIndexData[DrawContext.dwStartVertex]), DrawContext.dwVertexCount);
	VertexBufferConverter.Apply(&DrawContext, LowIndex);

	if (DrawContext.XboxPrimitiveType == X_D3DPT_QUADLIST) {
		UINT uiStartIndex = 0;
		int iNumVertices = (int)DrawContext.dwVertexCount;
		// Indexed quadlist can be drawn using unpatched indexes via multiple draws of 2 'strip' triangles :
		// 4 vertices are just enough for two triangles (a fan starts with 3 vertices for 1 triangle,
		// and adds 1 triangle via 1 additional vertex)
		// This is slower (because of call-overhead) but doesn't require any index buffer patching.
		// Draw 1 quad as a 2 triangles in a fan (which both have the same winding order) :
		// Test-cases : XDK samples reaching this case are : DisplacementMap, Ripple
		// Test-case : XDK Samples (Billboard, BumpLens, DebugKeyboard, Gamepad, Lensflare, PerfTest?VolumeLight, PointSprites, Tiling, VolumeFog, VolumeSprites, etc)
		while (iNumVertices >= VERTICES_PER_QUAD) {
			WalkIndexBuffer(LowIndex, HighIndex, &(DrawContext.pIndexData[uiStartIndex]), VERTICES_PER_QUAD);

			// Emulate a quad by drawing each as a fan of 2 triangles
			HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitive(
				D3DPT_TRIANGLEFAN,
				DrawContext.dwIndexBase,
				LowIndex, // minIndex
				(HighIndex - LowIndex) + 1, // NumVertices
				uiStartIndex,
				TRIANGLES_PER_QUAD // primCount = Draw 2 triangles
			);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitive(X_D3DPT_QUADLIST)");

			uiStartIndex += VERTICES_PER_QUAD;
			iNumVertices -= VERTICES_PER_QUAD;
			g_dwPrimPerFrame += TRIANGLES_PER_QUAD;
		}
	}
	else {
		// Primitives other than X_D3DPT_QUADLIST can be drawn using one DrawIndexedPrimitive call :
		HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitive(
			EmuXB2PC_D3DPrimitiveType(DrawContext.XboxPrimitiveType),
			DrawContext.dwIndexBase,
			/* MinVertexIndex = */LowIndex,
			/* NumVertices = */(HighIndex - LowIndex) + 1,//using index vertex span here.  // TODO : g_EmuD3DActiveStreamSizes[0], // Note : ATI drivers are especially picky about this -
			// NumVertices should be the span of covered vertices in the active vertex buffer (TODO : Is stream 0 correct?)
			DrawContext.dwStartVertex,
			DrawContext.dwHostPrimitiveCount);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitive");

		g_dwPrimPerFrame += DrawContext.dwHostPrimitiveCount;
		if (DrawContext.XboxPrimitiveType == X_D3DPT_LINELOOP) {
			// Close line-loops using a final single line, drawn from the end to the start vertex
			LOG_TEST_CASE("X_D3DPT_LINELOOP");
			// Read the end and start index from the supplied index data
			LowIndex = DrawContext.pIndexData[0];
			HighIndex = DrawContext.pIndexData[DrawContext.dwHostPrimitiveCount];
			// If needed, swap so highest index is higher than lowest (duh)
			if (HighIndex < LowIndex) {
				HighIndex ^= LowIndex;
				LowIndex ^= HighIndex;
				HighIndex ^= LowIndex;
			}

			// Draw the closing line using a helper function (which will SetIndices)
			CxbxDrawIndexedClosingLine(LowIndex, HighIndex);
			// NOTE : We don't restore the previously active index buffer
		}
	}
}

// TODO : Move to own file
// Drawing function specifically for rendering Xbox draw calls supplying a 'User Pointer'.
// Called by D3DDevice_DrawVerticesUP, EmuExecutePushBufferRaw and EmuFlushIVB
void XTL::CxbxDrawPrimitiveUP(CxbxDrawContext &DrawContext)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

	assert(DrawContext.dwStartVertex == 0);
	assert(DrawContext.pXboxVertexStreamZeroData != NULL);
	assert(DrawContext.uiXboxVertexStreamZeroStride > 0);
	assert(DrawContext.dwIndexBase == 0); // No IndexBase under Draw*UP

	CxbxVertexBufferConverter VertexBufferConverter = {};
	VertexBufferConverter.Apply(&DrawContext);
	if (DrawContext.XboxPrimitiveType == X_D3DPT_QUADLIST) {
		// LOG_TEST_CASE("X_D3DPT_QUADLIST"); // X-Marbles and XDK Sample PlayField hits this case
		// Draw quadlists using a single 'quad-to-triangle mapping' index buffer :
		INDEX16 *pIndexData = CxbxAssureQuadListIndexBuffer(DrawContext.dwVertexCount);
		// Convert quad vertex-count to triangle vertex count :
		UINT PrimitiveCount = DrawContext.dwHostPrimitiveCount * TRIANGLES_PER_QUAD;

		// Instead of calling WalkIndexBuffer on pQuadToTriangleIndexBuffer,
		// we can derive the LowIndex and HighIndexes ourselves here
		INDEX16 LowIndex = 0;
		INDEX16 HighIndex = (INDEX16)(DrawContext.dwVertexCount - 1);

		HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitiveUP(
			D3DPT_TRIANGLELIST, // Draw indexed triangles instead of quads
			LowIndex, // MinVertexIndex
			(HighIndex - LowIndex) + 1, // NumVertexIndices
			PrimitiveCount,
			pIndexData,
			D3DFMT_INDEX16, // IndexDataFormat
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
		if (DrawContext.XboxPrimitiveType == X_D3DPT_LINELOOP) {
			// Note : XDK samples reaching this case : DebugKeyboard, Gamepad, Tiling, ShadowBuffer
			// Since we can use pHostVertexStreamZeroData here, we can close the line simpler than
			// via CxbxDrawIndexedClosingLine, by drawing two indices via DrawIndexedPrimitiveUP.
			// (This is simpler because we use just indices and don't need to copy the vertices.)
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

void EmuUpdateActiveTextureStages()
{
	LOG_INIT;

	for (int i = 0; i < TEXTURE_STAGES; i++)
	{
		XTL::X_D3DBaseTexture *pBaseTexture = XTL::EmuD3DActiveTexture[i];
		if (pBaseTexture == nullptr) {
			HRESULT hRet = g_pD3DDevice->SetTexture(i, NULL);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetTexture");
			continue;
		}

		XTL::IDirect3DTexture *pHostTexture = GetHostTexture(pBaseTexture, i);

		if (pHostTexture != nullptr) {
			HRESULT hRet = g_pD3DDevice->SetTexture(i, pHostTexture);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetTexture");
		}
	}
}

void XTL::CxbxUpdateNativeD3DResources()
{
    EmuUpdateActiveTextureStages();

	// If Pixel Shaders are not disabled, process them
	if (!g_DisablePixelShaders) {
		XTL::DxbxUpdateActivePixelShader();
	}

	// Some titles set Vertex Shader constants directly via pushbuffers rather than through D3D
	// We handle that case by updating any constants that have the dirty flag set on the nv2a.
	auto nv2a = g_NV2A->GetDeviceState();
	for(int i = 0; i < 192; i++) {
		if (nv2a->pgraph.vsh_constants_dirty[i]) {
			g_pD3DDevice->SetVertexShaderConstantF(i, (float*)&nv2a->pgraph.vsh_constants[i][0], 1);
			nv2a->pgraph.vsh_constants_dirty[i] = false;
		}
	}

    XTL::EmuUpdateDeferredStates();
/* TODO : Port these :
	DxbxUpdateActiveVertexShader();
	DxbxUpdateActiveTextures();
	DxbxUpdateDeferredStates(); // BeginPush sample shows us that this must come *after* texture update!
	DxbxUpdateActiveVertexBufferStreams();
	DxbxUpdateActiveRenderTarget();
*/
}

VOID __declspec(noinline) D3DDevice_SetPixelShaderCommon(DWORD Handle)
{
	// Update the global pixel shader and Render States
	// TODO: Remove all of this as soon as RenderState functions are all unpatched!
	// At that point, SetPixelShader could be FULLY unpatched
	g_D3DActivePixelShader = (XTL::X_PixelShader*)Handle;

	// Copy the Pixel Shader data to the TemporaryPixelShaderRenderStates array
	// This mirrors the fact that unpathed SetPixelShader does the same thing!
	if (g_D3DActivePixelShader != nullptr) {
		memcpy(&(XTL::TemporaryPixelShaderRenderStates[XTL::X_D3DRS_PSALPHAINPUTS0]), g_D3DActivePixelShader->pPSDef, sizeof(XTL::X_D3DPIXELSHADERDEF) - 3 * sizeof(DWORD));
		XTL::TemporaryPixelShaderRenderStates[XTL::X_D3DRS_PSTEXTUREMODES] = g_D3DActivePixelShader->pPSDef->PSTextureModes;
	}
}

// LTCG specific D3DDevice_SetPixelShader function...
// This uses a custom calling convention where parameter is passed in EAX
// TODO: XB_trampoline plus Log function is not working due lost parameter in EAX.
// Test-case: Metal Wolf Chaos
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetPixelShader_0)
(
)
{
	DWORD           Handle;
	__asm mov Handle, eax;

	//LOG_FUNC_ONE_ARG(Handle);

	DBG_PRINTF("D3DDevice_SetPixelShader_0(Handle : %d);\n", Handle);

	// Call the Xbox function to make sure D3D structures get set
	//XB_trampoline(VOID, WINAPI, D3DDevice_SetPixelShader_0, ());
	//XB_D3DDevice_SetPixelShader_0();

	D3DDevice_SetPixelShaderCommon(Handle);
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

	if (!EmuD3DValidVertexCount(PrimitiveType, VertexCount)) {
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
		DrawContext.hVertexShader = g_CurrentXboxVertexShaderHandle;
		CxbxVertexBufferConverter VertexBufferConverter = {};
		VertexBufferConverter.Apply(&DrawContext);
		if (DrawContext.XboxPrimitiveType == X_D3DPT_QUADLIST) {
			// LOG_TEST_CASE("X_D3DPT_QUADLIST"); // ?X-Marbles and XDK Sample (Cartoon, ?maybe PlayField?) hits this case
			if (StartVertex > 0) {
				LOG_TEST_CASE("X_D3DPT_QUADLIST StartVertex > 0");
				// test-case : BLiNX: the time sweeper
				// test-case : Halo - Combat Evolved
				// test-case : Worms 3D Special Edition
				// test-case : XDK sample Lensflare 
				DrawContext.dwStartVertex = StartVertex; // Breakpoint location for testing. 
			}

			// Draw quadlists using a single 'quad-to-triangle mapping' index buffer :
			// Assure & activate that special index buffer :
			CxbxAssureQuadListD3DIndexBuffer(/*NrOfQuadVertices=*/DrawContext.dwVertexCount);
			// Convert quad vertex-count & start to triangle vertex count & start :
			UINT startIndex = QuadToTriangleVertexCount(DrawContext.dwStartVertex);
			UINT primCount = DrawContext.dwHostPrimitiveCount * TRIANGLES_PER_QUAD;
			// Determine highest and lowest index in use :
			INDEX16 LowIndex = pQuadToTriangleIndexBuffer[startIndex];
			INDEX16 HighIndex = LowIndex + (INDEX16)DrawContext.dwVertexCount - 1;
			// Emulate a quad by drawing each as a fan of 2 triangles
			HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitive(
				D3DPT_TRIANGLELIST, // Draw indexed triangles instead of quads
				0, // BaseVertexIndex
				LowIndex, // minIndex
				(HighIndex - LowIndex) + 1, // NumVertices,
				startIndex,
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
				INDEX16 LowIndex = (INDEX16)DrawContext.dwStartVertex;
				INDEX16 HighIndex = (INDEX16)(DrawContext.dwStartVertex + DrawContext.dwHostPrimitiveCount);
				// Draw the closing line using a helper function (which will SetIndices)
				CxbxDrawIndexedClosingLine(LowIndex, HighIndex);
				// NOTE : We don't restore the previously active index buffer
			}
		}
    }

	// Execute callback procedure
	if (g_CallbackType == X_D3DCALLBACK_WRITE) {
		if (g_pCallback) {
			g_pCallback(g_CallbackParam);
			// TODO: Reset pointer?
		}
	}
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

	if (!EmuD3DValidVertexCount(PrimitiveType, VertexCount)) {
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
		DrawContext.hVertexShader = g_CurrentXboxVertexShaderHandle;

		CxbxDrawPrimitiveUP(DrawContext);
    }

	// Execute callback procedure
	if (g_CallbackType == X_D3DCALLBACK_WRITE) {
		if (g_pCallback) {
			g_pCallback(g_CallbackParam);
			// TODO: Reset pointer?
		}
	}
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

	if (!EmuD3DValidVertexCount(PrimitiveType, VertexCount)) {
		LOG_TEST_CASE("Invalid VertexCount");
		return;
	}

	// TODO : Call unpatched D3DDevice_SetStateVB(0);

	CxbxUpdateNativeD3DResources();

	if (IsValidCurrentShader()) {
		CxbxUpdateActiveIndexBuffer(pIndexData, VertexCount);

		CxbxDrawContext DrawContext = {};

		DrawContext.XboxPrimitiveType = PrimitiveType;
		DrawContext.dwVertexCount = VertexCount;
		DrawContext.hVertexShader = g_CurrentXboxVertexShaderHandle;
		DrawContext.dwIndexBase = g_XboxBaseVertexIndex; // Used by GetVerticesInBuffer
		DrawContext.pIndexData = pIndexData; // Used by GetVerticesInBuffer

		// Test case JSRF draws all geometry through this function (only sparks are drawn via another method)
		// using X_D3DPT_TRIANGLELIST and X_D3DPT_TRIANGLESTRIP PrimitiveType
		CxbxDrawIndexed(DrawContext);
	}

	// Execute callback procedure
	if (g_CallbackType == X_D3DCALLBACK_WRITE) {
		if (g_pCallback) {
			g_pCallback(g_CallbackParam);
			// TODO: Reset pointer?
		}
	}

//#endif
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

	if (!EmuD3DValidVertexCount(PrimitiveType, VertexCount)) {
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
		DrawContext.hVertexShader = g_CurrentXboxVertexShaderHandle;
		// Don't set DrawContext.pIndexData = (INDEX16*)pIndexData; // Used by GetVerticesInBuffer

		CxbxVertexBufferConverter VertexBufferConverter = {};
		VertexBufferConverter.Apply(&DrawContext);
		if (DrawContext.XboxPrimitiveType == X_D3DPT_QUADLIST) {
			// Indexed quadlist can be drawn using unpatched indexes via multiple draws of 2 'strip' triangles :
			// Those 4 vertices are just enough for two triangles (a fan starts with 3 vertices for 1 triangle,
			// and adds 1 triangle via 1 additional vertex)
			// This is slower (because of call-overhead) but doesn't require any index buffer patching

			// Draw 1 quad as a 2 triangles in a fan (which both have the same winding order) :
			// LOG_TEST_CASE("X_D3DPT_QUADLIST"); // Test-case : Buffy: The Vampire Slayer, FastLoad XDK Sample
			INDEX16* pWalkIndexData = (INDEX16*)pIndexData;
			int iNumVertices = (int)VertexCount;
			while (iNumVertices >= VERTICES_PER_QUAD) {
				// Determine highest and lowest index in use :
				INDEX16 LowIndex;
				INDEX16 HighIndex;
				WalkIndexBuffer(LowIndex, HighIndex, pWalkIndexData, VERTICES_PER_QUAD);
				// Emulate a quad by drawing each as a fan of 2 triangles
				HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitiveUP(
					D3DPT_TRIANGLEFAN, // Draw a triangle-fan instead of a quad
					LowIndex, // MinVertexIndex
					(HighIndex - LowIndex) + 1, // NumVertexIndices
					TRIANGLES_PER_QUAD, // primCount = Draw 2 triangles
					pWalkIndexData,
					D3DFMT_INDEX16, // IndexDataFormat
					DrawContext.pHostVertexStreamZeroData,
					DrawContext.uiHostVertexStreamZeroStride
				);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitiveUP(X_D3DPT_QUADLIST)");

				pWalkIndexData += VERTICES_PER_QUAD;
				iNumVertices -= VERTICES_PER_QUAD;
			}

			g_dwPrimPerFrame += VertexCount / VERTICES_PER_QUAD * TRIANGLES_PER_QUAD;
		}
		else {
			// Walk through the index buffer
			INDEX16 LowIndex, HighIndex;
			WalkIndexBuffer(LowIndex, HighIndex, (INDEX16*)pIndexData, DrawContext.dwVertexCount);

			// LOG_TEST_CASE("DrawIndexedPrimitiveUP"); // Test-case : Burnout, Namco Museum 50th Anniversary
			HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitiveUP(
				EmuXB2PC_D3DPrimitiveType(DrawContext.XboxPrimitiveType),
				LowIndex, // MinVertexIndex
				(HighIndex - LowIndex) + 1, //this shall be Vertex Spans DrawContext.dwVertexCount, // NumVertexIndices
				DrawContext.dwHostPrimitiveCount,
				pIndexData,
				D3DFMT_INDEX16, // IndexDataFormat
				DrawContext.pHostVertexStreamZeroData,
				DrawContext.uiHostVertexStreamZeroStride
			);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitiveUP");

			g_dwPrimPerFrame += DrawContext.dwHostPrimitiveCount;
			if (DrawContext.XboxPrimitiveType == X_D3DPT_LINELOOP) {
				// Close line-loops using a final single line, drawn from the end to the start vertex
				LOG_TEST_CASE("X_D3DPT_LINELOOP"); // TODO : Which titles reach this case?
				// Read the end and start index from the supplied index data
				LowIndex = ((INDEX16*)pIndexData)[0];
				HighIndex = ((INDEX16*)pIndexData)[DrawContext.dwHostPrimitiveCount];
				// If needed, swap so highest index is higher than lowest (duh)
				if (HighIndex < LowIndex) {
					HighIndex ^= LowIndex;
					LowIndex ^= HighIndex;
					HighIndex ^= LowIndex;
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
    }

	// Execute callback procedure
	if (g_CallbackType == X_D3DCALLBACK_WRITE) {
		if (g_pCallback) {
			g_pCallback(g_CallbackParam);
			// TODO: Reset pointer?
		}
	}
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

// NOTE: NOT A PATCH
// This is the code common to both GetCubeMapSurface/GetCubeMapSurface2
HRESULT D3DCubeTexture_GetCubeMapSurfaceCommon
(
	XTL::X_D3DCubeTexture*	pThis,
	XTL::D3DCUBEMAP_FACES	FaceType,
	XTL::UINT				Level,
	XTL::X_D3DSurface**		ppCubeMapSurface
)
{
	// Now ppCubeMapSurface correctly points to an Xbox surface, while pThis points to an Xbox Cube Texture
	// We can use this to tie the host resources for both together, allowing Cube Mapping actually work!
	auto pHostCubeTexture = (XTL::IDirect3DCubeTexture*)GetHostResource(pThis, XTL::EmuXBFormatIsRenderTarget(GetXboxPixelContainerFormat(pThis)) ? D3DUSAGE_RENDERTARGET : 0);
	XTL::IDirect3DSurface* pHostCubeMapSurface;
	XTL::HRESULT hRet = pHostCubeTexture->GetCubeMapSurface(FaceType, Level, &pHostCubeMapSurface);
	if (FAILED(hRet)) {
		return hRet;
	}

	// Tie the Xbox CubeMapSurface and the host CubeMapSurface together
	SetHostSurface(*ppCubeMapSurface, pHostCubeMapSurface);

	return hRet;
}

// ******************************************************************
// * patch: IDirect3DCubeTexture8_GetCubeMapSurface
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DCubeTexture_GetCubeMapSurface)
(
	X_D3DCubeTexture*	pThis,
	D3DCUBEMAP_FACES	FaceType,
	UINT				Level,
	X_D3DSurface**		ppCubeMapSurface
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(FaceType)
		LOG_FUNC_ARG(Level)
		LOG_FUNC_ARG(ppCubeMapSurface)
		LOG_FUNC_END;

	// First, we need to fetch the Xbox cubemap surface via a trampoline
	HRESULT hRet;
	XB_trampoline(HRESULT, WINAPI, D3DCubeTexture_GetCubeMapSurface, (X_D3DCubeTexture*, D3DCUBEMAP_FACES, UINT, X_D3DSurface**));
	hRet = XB_D3DCubeTexture_GetCubeMapSurface(pThis, FaceType, Level, ppCubeMapSurface);

	// If the Xbox call failed, we must fail too
	if (FAILED(hRet)) {
		RETURN(hRet);
	}

	hRet = D3DCubeTexture_GetCubeMapSurfaceCommon(pThis, FaceType, Level, ppCubeMapSurface);
	RETURN(hRet);
}

// ******************************************************************
// * patch: IDirect3DCubeTexture8_GetCubeMapSurface2
// ******************************************************************
XTL::X_D3DSurface* WINAPI XTL::EMUPATCH(D3DCubeTexture_GetCubeMapSurface2)
(
	X_D3DCubeTexture*	pThis,
	D3DCUBEMAP_FACES	FaceType,
	UINT				Level
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(FaceType)
		LOG_FUNC_ARG(Level)
		LOG_FUNC_END;

	// First, we need to fetch the Xbox cubemap surface via a trampoline
	XB_trampoline(X_D3DSurface*, WINAPI, D3DCubeTexture_GetCubeMapSurface2, (X_D3DCubeTexture*, D3DCUBEMAP_FACES, UINT));
	X_D3DSurface* pCubeMapSurface = XB_D3DCubeTexture_GetCubeMapSurface2(pThis, FaceType, Level);

	// If the Xbox call failed, we must fail too
	if (pCubeMapSurface == nullptr) {
		RETURN(NULL);
	}

	D3DCubeTexture_GetCubeMapSurfaceCommon(pThis, FaceType, Level, &pCubeMapSurface);
	return pCubeMapSurface;
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
	if (g_XboxBackBufferSurface == xbnullptr) {
		g_XboxBackBufferSurface = pRenderTarget;
		// TODO : Some titles might render to another backbuffer later on,
		// if that happens, we might need to skip the first one or two calls?
	}

	// The current render target is only replaced if it's passed in here non-null
	if (pRenderTarget != xbnullptr) {
		g_pXboxRenderTarget = pRenderTarget;
	}
	else {
		// If non is given, use the current Xbox render target
		pRenderTarget = g_pXboxRenderTarget;
		// If there's no Xbox render target yet, fallback to the Xbox back buffer
		if (pRenderTarget == xbnullptr) {
			LOG_TEST_CASE("SetRenderTarget fallback to backbuffer");
			pRenderTarget = g_XboxBackBufferSurface;
		}
    }

	pHostRenderTarget = GetHostSurface(pRenderTarget, D3DUSAGE_RENDERTARGET);

	// The currenct depth stencil is always replaced by whats passed in here (even a null)
	g_pXboxDepthStencil = pNewZStencil;
    pHostDepthStencil = GetHostSurface(g_pXboxDepthStencil, D3DUSAGE_DEPTHSTENCIL);

	if (g_DirectHostBackBufferAccess && pRenderTarget == g_XboxBackBufferSurface) {
		HRESULT hRet = g_pD3DDevice->GetBackBuffer(
			0, // iSwapChain
			0, D3DBACKBUFFER_TYPE_MONO, &pHostRenderTarget);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetBackBuffer");

		if (FAILED(hRet)) {
			CxbxKrnlCleanup("Could not get host backbuffer");
		}
	}

	HRESULT hRet;
	// Mimick Direct3D 8 SetRenderTarget by only setting render target if non-null
	if (pHostRenderTarget) {
		hRet = g_pD3DDevice->SetRenderTarget(/*RenderTargetIndex=*/0, pHostRenderTarget);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetRenderTarget");
		if (FAILED(hRet)) {
			// If Direct3D 9 SetRenderTarget failed, skip setting depth stencil
			return;
		}

		pHostDepthStencil = g_DefaultHostDepthBufferSuface;
	}

	hRet = g_pD3DDevice->SetDepthStencilSurface(pHostDepthStencil);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetDepthStencilSurface");

	if (g_DirectHostBackBufferAccess && pRenderTarget == g_XboxBackBufferSurface) {
		pHostRenderTarget->Release();
	}

	if (SUCCEEDED(hRet)) {
		// Once we're sure the host depth-stencil is activated...
		UpdateDepthStencilFlags(pHostDepthStencil);
	}
}

// LTCG specific D3DDevice_SetPalette function...
// This uses a custom calling convention where parameter is passed in EAX
// Test-case: Ninja Gaiden
VOID __stdcall XTL::EMUPATCH(D3DDevice_SetPalette_4)
(
)
{
	static uint32 returnAddr;

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

	if (Stage < TEXTURE_STAGES) {
		if (g_pCurrentPalette[Stage] != GetDataFromXboxResource(pPalette) && XTL::EmuD3DActiveTexture[Stage] != nullptr) {
			// If the palette for a texture has changed, we need to re-convert the texture
			FreeHostResource(GetHostResourceKey(XTL::EmuD3DActiveTexture[Stage]));
		}

		// Cache palette data and size
		g_pCurrentPalette[Stage] = GetDataFromXboxResource(pPalette);
	}
}


// ******************************************************************
// * patch: IDirect3DPalette8_Lock
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DPalette_Lock)
(
	X_D3DPalette   *pThis,
	D3DCOLOR      **ppColors,
	DWORD           Flags
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG_OUT(ppColors)
		LOG_FUNC_ARG(Flags)
		LOG_FUNC_END;

	XB_trampoline(VOID, WINAPI, D3DPalette_Lock, (X_D3DPalette*, D3DCOLOR**, DWORD));
	XB_D3DPalette_Lock(pThis, ppColors, Flags);

	// Check if this palette is in use by a texture stage, and force it to be re-converted if yes
	for (int i = 0; i < TEXTURE_STAGES; i++) {
		if (EmuD3DActiveTexture[i] != nullptr && g_pCurrentPalette[i] == GetDataFromXboxResource(pThis)) {
			FreeHostResource(GetHostResourceKey(EmuD3DActiveTexture[i]));
		}
	}
}

// ******************************************************************
// * patch: IDirect3DPalette8_Lock2
// ******************************************************************
XTL::D3DCOLOR * WINAPI XTL::EMUPATCH(D3DPalette_Lock2)
(
	X_D3DPalette   *pThis,
	DWORD           Flags
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(Flags)
		LOG_FUNC_END;

	XB_trampoline(XTL::D3DCOLOR*, WINAPI, D3DPalette_Lock2, (X_D3DPalette*, DWORD));
	XTL::D3DCOLOR* pData = XB_D3DPalette_Lock2(pThis, Flags);

	// Check if this palette is in use by a texture stage, and force it to be re-converted if yes
	for (int i = 0; i < TEXTURE_STAGES; i++) {
		if (EmuD3DActiveTexture[i] != nullptr && g_pCurrentPalette[i] == GetDataFromXboxResource(pThis)) {
			FreeHostResource(GetHostResourceKey(EmuD3DActiveTexture[i]));
		}
	}

	RETURN(pData);
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
        *pSize = pVertexShader ? pVertexShader->Size : 0;
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
	

	DWORD HostVertexShaderHandle = 0;

	if (VshHandleIsVertexShader(Handle))
	{
		CxbxVertexShader *pVertexShader = GetCxbxVertexShader(Handle);
		SetCxbxVertexShader(Handle, nullptr);

		if (pVertexShader->pHostDeclaration) {
			pVertexShader->pHostDeclaration->Release();
		}

		HostVertexShaderHandle = pVertexShader->Handle;
		free(pVertexShader->pDeclaration);

		if (pVertexShader->pFunction)
		{
			free(pVertexShader->pFunction);
		}

		FreeVertexDynamicPatch(pVertexShader);

		free(pVertexShader);
	}

	// Don't attempt to release invalid null handles
	if (HostVertexShaderHandle) {
		HRESULT hRet = ((IDirect3DVertexShader9*)HostVertexShaderHandle)->Release();
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DeleteVertexShader");
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

    LOG_UNIMPLEMENTED(); 
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
        *pMode = g_VertexShaderConstantMode;
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
        (*pHandle) = g_CurrentXboxVertexShaderHandle;
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

	// TODO: HACK: Since Xbox vertex shader constants range from -96 to 96, during conversion
	// some shaders need to add 96 to use ranges 0 to 192.  This fixes 3911 - 4361 games and XDK
	// samples, but breaks Turok.
	// See D3DDevice_SetVertexShaderConstant
	if (g_BuildVersion <= 4361)
		Register += 96;

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

	// If Handle is NULL, all VertexShader input state is cleared.
	// Otherwise, Handle is the address of an Xbox VertexShader struct, or-ed with 1 (X_D3DFVF_RESERVED0)


    LOG_UNIMPLEMENTED(); 

	return;
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

	DWORD hCurrentShader = g_CurrentXboxVertexShaderHandle;

	// D3DDevice_LoadVertexShaderProgram splits the given function buffer into batch-wise pushes to the NV2A

	load_shader_program_key_t shaderCacheKey = ((load_shader_program_key_t)hCurrentShader << 32) | (DWORD)pFunction;

	// If the shader key was located in the cache, use the cached shader
	// TODO: When do we clear the cache? In this approach, shaders are
	// never freed...
	auto it = g_LoadVertexShaderProgramCache.find(shaderCacheKey);
	if (it != g_LoadVertexShaderProgramCache.end()) {
		EMUPATCH(D3DDevice_LoadVertexShader)(it->second, Address);
		EMUPATCH(D3DDevice_SelectVertexShader)(it->second, Address);
		return;
	}

	CxbxVertexShader *pVertexShader = GetCxbxVertexShader(hCurrentShader);
	if (pVertexShader != nullptr)
    {
		DWORD hNewShader = 0;

		// Save the contents of the existing vertex shader program
		DWORD* pDeclaration = (DWORD*) malloc( pVertexShader->OriginalDeclarationCount * sizeof(DWORD) );
		memmove( pDeclaration, pVertexShader->pDeclaration, pVertexShader->OriginalDeclarationCount * sizeof(DWORD));

		// Create a vertex shader with the new vertex program data
		HRESULT hr = EMUPATCH(D3DDevice_CreateVertexShader)( pDeclaration, pFunction, &hNewShader, 0 );
		free(pDeclaration);
		if( FAILED( hr ) )
			CxbxKrnlCleanup("Error creating new vertex shader!" );

		EMUPATCH(D3DDevice_LoadVertexShader)(hNewShader, Address);
		EMUPATCH(D3DDevice_SelectVertexShader)(hNewShader, Address);

		g_LoadVertexShaderProgramCache[shaderCacheKey] = hNewShader;

		EmuLog(LOG_LEVEL::WARNING, "Vertex Shader Cache Size: %d", g_LoadVertexShaderProgramCache.size());
    }
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
		CxbxVertexShader *pVertexShader = GetCxbxVertexShader(Handle);
		if (pVertexShader) {
			*pType = pVertexShader->Type;
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
        CxbxVertexShader *pVertexShader = GetCxbxVertexShader(Handle);
		if (pVertexShader) {
			DWORD sizeOfData = pVertexShader->OriginalDeclarationCount * sizeof(DWORD);
			if (*pSizeOfData < sizeOfData || !pData) {
				*pSizeOfData = sizeOfData;
				hRet = !pData ? D3D_OK : D3DERR_MOREDATA;
			}
			else {
				memcpy(pData, pVertexShader->pDeclaration, pVertexShader->OriginalDeclarationCount * sizeof(DWORD));
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
        CxbxVertexShader *pVertexShader = GetCxbxVertexShader(Handle);
		if (pVertexShader) {
			if (*pSizeOfData < pVertexShader->FunctionSize || !pData) {
				*pSizeOfData = pVertexShader->FunctionSize;
				hRet = !pData ? D3D_OK : D3DERR_MOREDATA;
			}
			else {
				memcpy(pData, pVertexShader->pFunction, pVertexShader->FunctionSize);
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

    EmuLog(LOG_LEVEL::WARNING, "EmuD3DDevice_SetScreenSpaceOffset ignored");
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

	// TODO: Implement
	g_pCallback = (D3DCALLBACK) pCallback;
	g_CallbackType = Type;
	g_CallbackParam = Context;

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
	D3DSWAPCALLBACK		pCallback
)
{
	LOG_FUNC_ONE_ARG(pCallback);

    g_pSwapCallback = pCallback;
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
// * patch: D3DDevice_SetRenderState_SampleAlpha
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_SampleAlpha)
(
	DWORD dwSampleAlpha
)
{
	LOG_FUNC_ONE_ARG(dwSampleAlpha);

	XB_trampoline(VOID, WINAPI, D3DDevice_SetRenderState_SampleAlpha, (DWORD));
	XB_D3DDevice_SetRenderState_SampleAlpha(dwSampleAlpha);

	// TODO: Implement?

	LOG_UNIMPLEMENTED();

		

	return D3D_OK;
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
    DBG_PRINTF("D3DDevice_SetPixelShaderConstant_4(Register : %d pConstantData : %08X ConstantCount : %d);\n", Register, pConstantData, ConstantCount);

    // TODO: This hack is necessary for Vertex Shaders on XDKs prior to 4361, but if this
    // causes problems with pixel shaders, feel free to comment out the hack below.
    if(g_BuildVersion <= 4361)
        Register += 96;

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
