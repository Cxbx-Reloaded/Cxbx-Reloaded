// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuD3D8.cpp
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
#include "xxhash32.h"
#include <condition_variable>

// prevent name collisions
namespace xboxkrnl
{
    #include <xboxkrnl/xboxkrnl.h>
};

#include "CxbxUtil.h"
#include "CxbxVersion.h"
#include "CxbxKrnl.h"
#include "Emu.h"
#include "EmuFS.h"
#include "EmuShared.h"
#include "DbgConsole.h"
#include "ResourceTracker.h"
#include "VMManager.h" // for g_VMManager
#include "EmuXTL.h"
#include "HLEDatabase.h"
#include "Logging.h"
#include "EmuD3D8Logging.h"
#include "HLEIntercept.h" // for bLLE_GPU
#include "Cxbx\\ResCxbx.h"

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
static inline void                  EmuVerifyResourceIsRegistered(XTL::X_D3DResource *pResource, int iTextureStage, DWORD dwSize);
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
static XBVideo                      g_XBVideo;
static XTL::D3DVBLANKCALLBACK       g_pVBCallback   = NULL; // Vertical-Blank callback routine
static std::condition_variable		g_VBConditionVariable;	// Used in BlockUntilVerticalBlank
static std::mutex					g_VBConditionMutex;		// Used in BlockUntilVerticalBlank
static XTL::D3DSWAPCALLBACK			g_pSwapCallback = NULL;	// Swap/Present callback routine
static XTL::D3DCALLBACK				g_pCallback		= NULL;	// D3DDevice::InsertCallback routine
static XTL::X_D3DCALLBACKTYPE		g_CallbackType;			// Callback type
static DWORD						g_CallbackParam;		// Callback param
static BOOL                         g_bHasDepthStencil = FALSE;  // Does device have a Depth/Stencil Buffer?
//static DWORD						g_dwPrimPerFrame = 0;	// Number of primitives within one frame

// D3D based variables
static GUID                         g_ddguid;               // DirectDraw driver GUID
static XTL::IDirect3D              *g_pDirect3D = nullptr;
static XTL::D3DCAPS                 g_D3DCaps;              // Direct3D Caps

// wireframe toggle
static int                          g_iWireframe    = 0;

// build version
extern uint32						g_BuildVersion;

typedef uint64_t resource_key_t;

// resource caching for _Register
std::vector<resource_key_t> g_RegisteredResources;

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
static XTL::X_D3DSurface           *g_pXboxRenderTarget = NULL;
static XTL::X_D3DSurface           *g_pXboxDepthStencil = NULL;
static BOOL                         g_fYuvEnabled = FALSE;
static DWORD                        g_dwVertexShaderUsage = 0;
static DWORD                        g_VertexShaderSlots[136];

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

#ifdef _DEBUG_TRACE

#define DEBUG_D3DRESULT(hRet, message) \
	do { \
		if (FAILED(hRet)) \
			if(g_bPrintfOn) \
				printf("%s : %s D3D error (0x%.08X: %s)\n", _logFuncPrefix.c_str(), message, hRet, D3DErrorString(hRet)); \
	} while (0)

#else

#define DEBUG_D3DRESULT(hRet, message) \
	do { \
		if (FAILED(hRet)) \
			if(g_bPrintfOn) \
				DbgPrintf("%s : %s D3D error (0x%.08X: %s)\n", __func__, message, hRet, D3DErrorString(hRet)); \
	} while (0)

#endif

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
    g_EmuShared->GetXBVideo(&g_XBVideo);

    if(g_XBVideo.GetFullscreen())
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
			CxbxPopupMessage(szBuffer);
			EmuShared::Cleanup();
			ExitProcess(0);
		}

		// Ported from Dxbx :
		// If possible, assign this thread to another core than the one that runs Xbox1 code :
		if (bFullInit)
			SetThreadAffinityMask(hRenderWindowThread, g_CPUOthers);

        while(!g_bRenderWindowActive)
            SwitchToThread(); 

		SwitchToThread();
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
} host_resource_info_t;

std::unordered_map <resource_key_t, host_resource_info_t> g_HostResources;

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
	// Cleanup RegisteredResources array
	auto registeredResourceIterator = std::find(g_RegisteredResources.begin(), g_RegisteredResources.end(), key);
	// We can remove this soon, after a little more cleanup
	if (registeredResourceIterator != g_RegisteredResources.end()) {
		g_RegisteredResources.erase(registeredResourceIterator);
	}

	// Release the host resource and remove it from the list
	auto hostResourceIterator = g_HostResources.find(key);
	if (hostResourceIterator != g_HostResources.end()) {
		if (hostResourceIterator->second.pHostResource) {
			(hostResourceIterator->second.pHostResource)->Release();
		}

		g_HostResources.erase(hostResourceIterator);
	}
}

void ForceResourceRehash(XTL::X_D3DResource* pXboxResource)
{
	auto key = GetHostResourceKey(pXboxResource);
	auto it = g_HostResources.find(key);
	if (it != g_HostResources.end()) {
		it->second.forceRehash = true;
	}
}

XTL::IDirect3DResource *GetHostResource(XTL::X_D3DResource *pXboxResource, int iTextureStage = 0)
{
	if (pXboxResource == NULL || pXboxResource->Data == NULL)
		return nullptr;

	EmuVerifyResourceIsRegistered(pXboxResource, iTextureStage, /*dwSize=*/0);

	if (pXboxResource->Lock == X_D3DRESOURCE_LOCK_PALETTE)
		return nullptr;

	auto key = GetHostResourceKey(pXboxResource);
	auto it = g_HostResources.find(key);
	if (it == g_HostResources.end()) {
		EmuWarning("EmuResource is not a valid pointer!");
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
	auto it = g_HostResources.find(key);
	if (it == g_HostResources.end()) {
		return false;
	}

	// Currently, we only dynamically update Textures and Surfaces, so if our resource
	// isn't of these types, do nothing
	DWORD type = GetXboxCommonResourceType(it->second.pXboxResource);
	if (type != X_D3DCOMMON_TYPE_SURFACE && type != X_D3DCOMMON_TYPE_TEXTURE) {
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
	auto it = g_HostResources.find(key);
	if (it != g_HostResources.end()) {
		EmuWarning("SetHostResource: Overwriting an existing host resource");
	}

	host_resource_info_t hostResourceInfo;
	hostResourceInfo.pHostResource = pHostResource;
	hostResourceInfo.pXboxResource = pXboxResource;
	hostResourceInfo.dwXboxResourceType = GetXboxCommonResourceType(pXboxResource);
	hostResourceInfo.pXboxData = GetDataFromXboxResource(pXboxResource);
	hostResourceInfo.szXboxDataSize = dwSize > 0 ? dwSize : GetXboxResourceSize(pXboxResource);
	hostResourceInfo.hash = XXHash32::hash(hostResourceInfo.pXboxData, hostResourceInfo.szXboxDataSize, 0);
	hostResourceInfo.hashLifeTime = 1ms;
    hostResourceInfo.lastUpdate = std::chrono::high_resolution_clock::now();
	hostResourceInfo.nextHashTime = hostResourceInfo.lastUpdate + hostResourceInfo.hashLifeTime;
	hostResourceInfo.forceRehash = false;

	g_HostResources[key] = hostResourceInfo;
}

XTL::IDirect3DSurface *GetHostSurface(XTL::X_D3DResource *pXboxResource)
{
	if (pXboxResource == NULL)
		return nullptr;

	if (GetXboxCommonResourceType(pXboxResource) != X_D3DCOMMON_TYPE_SURFACE) // Allows breakpoint below
		assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_SURFACE);

	return (XTL::IDirect3DSurface*) GetHostResource(pXboxResource);
}

XTL::IDirect3DBaseTexture *GetHostBaseTexture(XTL::X_D3DResource *pXboxResource, int iTextureStage = 0)
{
	if (pXboxResource == NULL)
		return nullptr;

	if (GetXboxCommonResourceType(pXboxResource) != X_D3DCOMMON_TYPE_TEXTURE) { // Allows breakpoint below
		; // TODO : Log instead of assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_TEXTURE);
		// Burnout hits this case (retreiving a surface instead of a texture)
	}

	return (XTL::IDirect3DBaseTexture*)GetHostResource(pXboxResource, iTextureStage);
}

XTL::IDirect3DTexture *GetHostTexture(XTL::X_D3DResource *pXboxResource, int iTextureStage = 0)
{
	return (XTL::IDirect3DTexture *)GetHostBaseTexture(pXboxResource, iTextureStage);

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

VOID CxbxReleaseBackBufferLock()
{
	XTL::IDirect3DSurface *pBackBuffer = nullptr;

	if (D3D_OK == g_pD3DDevice->GetBackBuffer(
#ifdef CXBX_USE_D3D9
		0, // iSwapChain
#endif
		0, XTL::D3DBACKBUFFER_TYPE_MONO, &pBackBuffer))
	{
		assert(pBackBuffer != nullptr);

		pBackBuffer->UnlockRect(); // remove old lock
		pBackBuffer->Release();
	}
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
		g_EmuCDPD.Adapter = g_XBVideo.GetDisplayAdapter();
		g_EmuCDPD.DeviceType = (g_XBVideo.GetDirect3DDevice() == 0) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF;
		g_EmuCDPD.hFocusWindow = g_hEmuWindow;
	}

	// create Direct3D8 and retrieve caps
    {
        // xbox Direct3DCreate8 returns "1" always, so we need our own ptr
        g_pDirect3D = Direct3DCreate(D3D_SDK_VERSION);
        if(g_pDirect3D == NULL)
            CxbxKrnlCleanup("Could not initialize Direct3D8!");

        g_pDirect3D->GetDeviceCaps(g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType, &g_D3DCaps);
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

    // register window class
    {
        LOGBRUSH logBrush = {BS_SOLID, RGB(0,0,0)};

        g_hBgBrush = CreateBrushIndirect(&logBrush);

        WNDCLASSEX wc =
        {
            sizeof(WNDCLASSEX),
            CS_CLASSDC,
            EmuMsgProc,
            0, 0, GetModuleHandle(NULL),
			0, // TODO : LoadIcon(hmodule, ?)
            LoadCursor(NULL, IDC_ARROW),
            (HBRUSH)(g_hBgBrush), NULL,
            "CxbxRender",
            NULL
        };

        RegisterClassEx(&wc);
    }

	bool bQuickReboot;
	g_EmuShared->GetMultiXbeFlag(&bQuickReboot);

	// precaution for multi-xbe titles in the case CrashMonitor has still not destoyed the previous mutex
	while (bQuickReboot)
	{
		g_EmuShared->GetMultiXbeFlag(&bQuickReboot);
	}

	HANDLE hCrashMutex = CreateMutex(NULL, TRUE, "CrashMutex");

    // create the window
    {
        HWND hwndParent = GetDesktopWindow();
        DWORD dwStyle = WS_POPUP;
		RECT windowRect = { 0 };

        if (!g_XBVideo.GetFullscreen()) {
			hwndParent = CxbxKrnl_hEmuParent;
			GetWindowRect(hwndParent, &windowRect);
			dwStyle = (CxbxKrnl_hEmuParent == 0) ? WS_OVERLAPPEDWINDOW : WS_CHILD;
        }

        g_hEmuWindow = CreateWindow
        (
            "CxbxRender", "Cxbx-Reloaded",
            dwStyle, 
			windowRect.left,
			windowRect.top,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
            hwndParent, NULL, GetModuleHandle(NULL), NULL
        );
    }

    ShowWindow(g_hEmuWindow, ((CxbxKrnl_hEmuParent == 0) || g_XBVideo.GetFullscreen()) ? SW_SHOWDEFAULT : SW_SHOWMAXIMIZED);
    UpdateWindow(g_hEmuWindow);

    if(!g_XBVideo.GetFullscreen() && (CxbxKrnl_hEmuParent != NULL))
    {
        SetFocus(CxbxKrnl_hEmuParent);
    }

    // initialize direct input
    if(!XTL::EmuDInputInit())
        CxbxKrnlCleanup("Could not initialize DirectInput!");

    DbgPrintf("EmuD3D8: Message-Pump thread is running.\n");

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
				SwitchToThread();

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

		if (hCrashMutex != NULL) { ReleaseMutex(hCrashMutex); }

        CxbxKrnlCleanup(NULL);
    }

    return 0;
}

// simple helper function
void ToggleFauxFullscreen(HWND hWnd)
{
    if(g_XBVideo.GetFullscreen())
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
        }
        break;

        case WM_KEYDOWN:
        {
            /*! disable fullscreen if we are set to faux mode, and faux fullscreen is active */
            if(wParam == VK_ESCAPE)
            {
                if(g_XBVideo.GetFullscreen())
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
            }
            else if(wParam == VK_F9)
            {
                XTL::g_bBrkPush = TRUE;
            }
            else if(wParam == VK_F10)
            {
                ToggleFauxFullscreen(hWnd);
            }
            else if(wParam == VK_F11)
            {
                if(g_iWireframe++ == 2)
                    g_iWireframe = 0;
            }
            else if(wParam == VK_F12)
            {
                XTL::g_bStepPush = !XTL::g_bStepPush;
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
                    if(g_XBVideo.GetFullscreen())
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
            if(g_XBVideo.GetFullscreen() || g_bIsFauxFullscreen)
            {
                SetCursor(NULL);
                return D3D_OK; // = 0
            }

            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
        break;

        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return D3D_OK; // = 0
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

    DbgPrintf("EmuD3D8: Timing thread is running.\n");

    // current vertical blank count
    int curvb = 0;

	// Calculate Next VBlank time
	auto nextVBlankTime = GetNextVBlankTime();

    while(true)
    {
        xboxkrnl::KeTickCount = timeGetTime();	
		SwitchToThread();

        //
        // Poll input
        //

        {
            int v;

            for(v=0;v<XINPUT_SETSTATE_SLOTS;v++)
            {
                HANDLE hDevice = g_pXInputSetStateStatus[v].hDevice;

                if(hDevice == 0)
                    continue;

                DWORD dwLatency = g_pXInputSetStateStatus[v].dwLatency++;

                if(dwLatency < XINPUT_SETSTATE_LATENCY)
                    continue;

                g_pXInputSetStateStatus[v].dwLatency = 0;

                XTL::PXINPUT_FEEDBACK pFeedback = (XTL::PXINPUT_FEEDBACK)g_pXInputSetStateStatus[v].pFeedback;

                if(pFeedback == 0)
                    continue;

                //
                // Only update slot if it has not already been updated
                //

                if(pFeedback->Header.dwStatus != ERROR_SUCCESS)
                {
                    if(pFeedback->Header.hEvent != 0)
                    {
                        SetEvent(pFeedback->Header.hEvent);
                    }

                    pFeedback->Header.dwStatus = ERROR_SUCCESS;
                }
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

// thread dedicated to create devices
static DWORD WINAPI EmuCreateDeviceProxy(LPVOID)
{
	LOG_FUNC();

	CxbxSetThreadName("Cxbx CreateDevice Proxy");

    DbgPrintf("EmuD3D8: CreateDevice proxy thread is running.\n");

    while(true)
    {
        // if we have been signalled, create the device with cached parameters
        if(g_EmuCDPD.bReady)
        {
            DbgPrintf("EmuD3D8: CreateDevice proxy thread received request.\n");

            // only one device should be created at once
            if (g_pD3DDevice != nullptr) {
                DbgPrintf("EmuD3D8: CreateDevice proxy thread releasing old Device.\n");

                g_pD3DDevice->EndScene();

				for (auto &hostResourceIterator : g_HostResources) {
					if (hostResourceIterator.second.pHostResource) {
						(hostResourceIterator.second.pHostResource)->Release();
					}
				}
				g_HostResources.clear();

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
                    EmuWarning("BufferSurfaces[0] : 0x%.08X", g_EmuCDPD.XboxPresentationParameters.BufferSurfaces[0]);

                if(g_EmuCDPD.XboxPresentationParameters.DepthStencilSurface != NULL)
                    EmuWarning("DepthStencilSurface : 0x%.08X", g_EmuCDPD.XboxPresentationParameters.DepthStencilSurface);

				// Make a binary copy of the Xbox D3DPRESENT_PARAMETERS
				memcpy(&g_EmuCDPD.HostPresentationParameters, &(g_EmuCDPD.XboxPresentationParameters), sizeof(XTL::D3DPRESENT_PARAMETERS));

				// make adjustments to parameters to make sense with windows Direct3D
                {
                    g_EmuCDPD.HostPresentationParameters.Windowed = !g_XBVideo.GetFullscreen();

                    if(g_XBVideo.GetVSync())
#ifdef CXBX_USE_D3D9
                        g_EmuCDPD.HostPresentationParameters.SwapEffect = XTL::D3DSWAPEFFECT_COPY;
#else
                        g_EmuCDPD.HostPresentationParameters.SwapEffect = XTL::D3DSWAPEFFECT_COPY_VSYNC;
#endif

                    g_EmuCDPD.HostPresentationParameters.BackBufferFormat       = XTL::EmuXB2PC_D3DFormat(g_EmuCDPD.XboxPresentationParameters.BackBufferFormat);
					g_EmuCDPD.HostPresentationParameters.AutoDepthStencilFormat = XTL::EmuXB2PC_D3DFormat(g_EmuCDPD.XboxPresentationParameters.AutoDepthStencilFormat);

                    if(!g_XBVideo.GetVSync() && (g_D3DCaps.PresentationIntervals & D3DPRESENT_INTERVAL_IMMEDIATE) && g_XBVideo.GetFullscreen())
                        g_EmuCDPD.HostPresentationParameters.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
                    else
                    {
                        if(g_D3DCaps.PresentationIntervals & D3DPRESENT_INTERVAL_ONE && g_XBVideo.GetFullscreen())
                            g_EmuCDPD.HostPresentationParameters.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
                        else
                            g_EmuCDPD.HostPresentationParameters.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
                    }

					// HACK: Disable Tripple Buffering for now...
					// TODO: Enumerate maximum BackBufferCount if possible.
					if(g_EmuCDPD.XboxPresentationParameters.BackBufferCount > 1)
					{
						EmuWarning("Limiting BackBufferCount to 1...");
						g_EmuCDPD.HostPresentationParameters.BackBufferCount = 1;
					}

                    // TODO: Support Xbox extensions if possible
                    if(g_EmuCDPD.XboxPresentationParameters.MultiSampleType != 0)
                    {
                        EmuWarning("MultiSampleType 0x%.08X is not supported!", g_EmuCDPD.XboxPresentationParameters.MultiSampleType);

                        g_EmuCDPD.HostPresentationParameters.MultiSampleType = XTL::D3DMULTISAMPLE_NONE;

                        // TODO: Check card for multisampling abilities
            //            if(g_EmuCDPD.XboxPresentationParameters.MultiSampleType == X_D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX) // = 0x00001121
            //                g_EmuCDPD.HostPresentationParameters.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
            //            else
            //                CxbxKrnlCleanup("Unknown MultiSampleType (0x%.08X)", g_EmuCDPD.XboxPresentationParameters.MultiSampleType);
                    }

                    g_EmuCDPD.HostPresentationParameters.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

                    // retrieve resolution from configuration
                    if(g_EmuCDPD.HostPresentationParameters.Windowed)
                    {
						const char* resolution = g_XBVideo.GetVideoResolution();
						if (2 != sscanf(resolution, "%d x %d", &g_EmuCDPD.HostPresentationParameters.BackBufferWidth, &g_EmuCDPD.HostPresentationParameters.BackBufferHeight)) {
							DbgPrintf("EmuCreateDeviceProxy: Couldn't parse resolution : %s.\n", resolution);
						}
						else {
							if (g_EmuCDPD.HostPresentationParameters.BackBufferWidth == 640)
								DbgPrintf("EmuCreateDeviceProxy: Default width wasn't updated.\n");
							if (g_EmuCDPD.HostPresentationParameters.BackBufferWidth == 480)
								DbgPrintf("EmuCreateDeviceProxy: Default height wasn't updated.\n");
						}

                        XTL::D3DDISPLAYMODE D3DDisplayMode;

                        g_pDirect3D->GetAdapterDisplayMode(g_EmuCDPD.Adapter, &D3DDisplayMode);

                        g_EmuCDPD.HostPresentationParameters.BackBufferFormat = D3DDisplayMode.Format;
                        g_EmuCDPD.HostPresentationParameters.FullScreen_RefreshRateInHz = 0;
                    }
                    else
                    {
                        char szBackBufferFormat[16];

						const char* resolution = g_XBVideo.GetVideoResolution();
						if (4 != sscanf(resolution, "%d x %d %*dbit %s (%d hz)",
							&g_EmuCDPD.HostPresentationParameters.BackBufferWidth,
							&g_EmuCDPD.HostPresentationParameters.BackBufferHeight,
							szBackBufferFormat,
							&g_EmuCDPD.HostPresentationParameters.FullScreen_RefreshRateInHz)) {
							DbgPrintf("EmuCreateDeviceProxy: Couldn't parse resolution : %s.\n", resolution);
						}
						else {
							if (g_EmuCDPD.HostPresentationParameters.BackBufferWidth == 640)
								DbgPrintf("EmuCreateDeviceProxy: Default width wasn't updated.\n");
							if (g_EmuCDPD.HostPresentationParameters.BackBufferWidth == 480)
								DbgPrintf("EmuCreateDeviceProxy: Default height wasn't updated.\n");
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
                    DbgPrintf("EmuD3D8: Using hardware vertex processing\n");

                    g_EmuCDPD.BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
                    g_dwVertexShaderUsage = 0;
                }
                else
                {
                    DbgPrintf("EmuD3D8: Using software vertex processing\n");

                    g_EmuCDPD.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
                    g_dwVertexShaderUsage = D3DUSAGE_SOFTWAREPROCESSING;
                }

				// Dxbx addition : Prevent Direct3D from changing the FPU Control word :
				g_EmuCDPD.BehaviorFlags |= D3DCREATE_FPU_PRESERVE;

	            // Address debug DirectX runtime warning in _DEBUG builds
                // Direct3D8: (WARN) :Device that was created without D3DCREATE_MULTITHREADED is being used by a thread other than the creation thread.
                #ifdef _DEBUG
                    g_EmuCDPD.BehaviorFlags |= D3DCREATE_MULTITHREADED;
                #endif

				// For some reason, D3DFMT_D16_LOCKABLE as the AudoDepthStencil causes CreateDevice to fail...
				if (g_EmuCDPD.HostPresentationParameters.AutoDepthStencilFormat == XTL::D3DFMT_D16_LOCKABLE) {
					g_EmuCDPD.HostPresentationParameters.AutoDepthStencilFormat = XTL::D3DFMT_D16;
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
				for (int X_Format = XTL::X_D3DFMT_L8; X_Format <= XTL::X_D3DFMT_LIN_R8G8B8A8; X_Format++) {
					// Only process Xbox formats that are directly mappable to host
					if (!XTL::EmuXBFormatRequiresConversionToARGB((XTL::X_D3DFORMAT)X_Format)) {
						// Convert the Xbox format into host format (without warning, thanks to the above restriction)
						XTL::D3DFORMAT PCFormat = XTL::EmuXB2PC_D3DFormat((XTL::X_D3DFORMAT)X_Format);
						if (PCFormat != XTL::D3DFMT_UNKNOWN) {
							// Index with Xbox D3DFormat, because host FourCC codes are too big to be used as indices
							g_bSupportsFormatSurface[X_Format] =
								(D3D_OK == g_pDirect3D->CheckDeviceFormat(
									g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
									g_EmuCDPD.HostPresentationParameters.BackBufferFormat, 0,
									XTL::D3DRTYPE_SURFACE, PCFormat));
							g_bSupportsFormatSurfaceRenderTarget[X_Format] =
								(D3D_OK == g_pDirect3D->CheckDeviceFormat(
									g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
									g_EmuCDPD.HostPresentationParameters.BackBufferFormat, D3DUSAGE_RENDERTARGET,
									XTL::D3DRTYPE_SURFACE, PCFormat));
							g_bSupportsFormatSurfaceDepthStencil[X_Format] =
								(D3D_OK == g_pDirect3D->CheckDeviceFormat(
									g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
									g_EmuCDPD.HostPresentationParameters.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
									XTL::D3DRTYPE_SURFACE, PCFormat));
							g_bSupportsFormatTexture[X_Format] = 
								(D3D_OK == g_pDirect3D->CheckDeviceFormat(
									g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
									g_EmuCDPD.HostPresentationParameters.BackBufferFormat, 0,
									XTL::D3DRTYPE_TEXTURE, PCFormat));
							g_bSupportsFormatTextureRenderTarget[X_Format] =
								(D3D_OK == g_pDirect3D->CheckDeviceFormat(
									g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
									g_EmuCDPD.HostPresentationParameters.BackBufferFormat, D3DUSAGE_RENDERTARGET,
									XTL::D3DRTYPE_TEXTURE, PCFormat));
							g_bSupportsFormatTextureDepthStencil[X_Format] =
								(D3D_OK == g_pDirect3D->CheckDeviceFormat(
									g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
									g_EmuCDPD.HostPresentationParameters.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
									XTL::D3DRTYPE_TEXTURE, PCFormat));
							g_bSupportsFormatVolumeTexture[X_Format] =
								(D3D_OK == g_pDirect3D->CheckDeviceFormat(
									g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
									g_EmuCDPD.HostPresentationParameters.BackBufferFormat, 0,
									XTL::D3DRTYPE_VOLUMETEXTURE, PCFormat));
							g_bSupportsFormatCubeTexture[X_Format] =
								(D3D_OK == g_pDirect3D->CheckDeviceFormat(
									g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
									g_EmuCDPD.HostPresentationParameters.BackBufferFormat, 0,
									XTL::D3DRTYPE_CUBETEXTURE, PCFormat));
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
						DbgPrintf("EmuD3D8: FourCC[%d] = %.4s\n", v, (char *)&(lpCodes[v]));
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
							EmuWarning("EmuD3D8: FourCC format %.4s not previously detected via CheckDeviceFormat()! Enabling it.", (char *)&(lpCodes[v]));
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
#ifdef CXBX_USE_D3D9
					0, // RenderTargetIndex
#endif
					&pCurrentHostRenderTarget);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetRenderTarget");
				// TODO : SetHostResource(BackBuffer[0], pCurrentHostRenderTarget);
*/

				// update z-stencil surface cache
				XTL::IDirect3DSurface *pCurrentHostDepthStencil = nullptr;
				hRet = g_pD3DDevice->GetDepthStencilSurface(&pCurrentHostDepthStencil);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetDepthStencilSurface");

				g_bHasDepthStencil = SUCCEEDED(hRet) && (pCurrentHostDepthStencil != nullptr);

				if (pCurrentHostDepthStencil) {
					pCurrentHostDepthStencil->Release();
				}

				hRet = g_pD3DDevice->CreateVertexBuffer
                (
                    1, 0, 0, XTL::D3DPOOL_MANAGED,
                    &g_pDummyBuffer
#ifdef CXBX_USE_D3D9
					, nullptr
#endif
                );
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateVertexBuffer");

                for(int Streams = 0; Streams < 16; Streams++)
                {
                    hRet = g_pD3DDevice->SetStreamSource(Streams, g_pDummyBuffer, 
#ifdef CXBX_USE_D3D9
						0, // OffsetInBytes
#endif
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
					D3DCLEAR_TARGET | (g_bHasDepthStencil ? D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL : 0),
					/*Color=*/0xFF000000, // TODO : Use constant for this
					/*Z=*/g_bHasDepthStencil ? 1.0f : 0.0f,
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
void CreateHostResource(XTL::X_D3DResource *pResource, int iTextureStage, DWORD dwSize); // Forward declartion to prevent restructure of code
static void EmuVerifyResourceIsRegistered(XTL::X_D3DResource *pResource, int iTextureStage = 0, DWORD dwSize = 0)
{
	// Skip resources without data
	if (pResource->Data == NULL)
		return;

	auto key = GetHostResourceKey(pResource);
	if (std::find(g_RegisteredResources.begin(), g_RegisteredResources.end(), key) != g_RegisteredResources.end()) {
		if (!HostResourceRequiresUpdate(key, dwSize)) {
			return;
		}

		FreeHostResource(key);
	}

	CreateHostResource(pResource, iTextureStage, dwSize);
        
	g_RegisteredResources.push_back(key);
}

typedef struct {
	DWORD Hash = 0;
	DWORD IndexCount = 0;;
	XTL::IDirect3DIndexBuffer* pHostIndexBuffer = nullptr;
} ConvertedIndexBuffer;

std::unordered_map<PWORD, ConvertedIndexBuffer> g_ConvertedIndexBuffers;
	
void CxbxRemoveIndexBuffer(PWORD pData)
{
	// HACK: Never Free
}

DWORD g_CachedIndexBase = 0;

void CxbxUpdateActiveIndexBuffer
(
	PWORD         pIndexData,
	PDWORD		  pIndexBase,	
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
#ifdef CXBX_USE_D3D9
			, nullptr
#endif
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
		BYTE* pData = nullptr;
		indexBuffer.pHostIndexBuffer->Lock(0, 0, &pData, D3DLOCK_DISCARD);
		if (pData == nullptr) {
			CxbxKrnlCleanup("CxbxUpdateActiveIndexBuffer: Could not lock index buffer!");
		}

		DbgPrintf("CxbxUpdateActiveIndexBuffer: Copying %d indices (D3DFMT_INDEX16)\n", IndexCount);
		memcpy(pData, pIndexData, IndexCount * 2);

		indexBuffer.pHostIndexBuffer->Unlock();
	}

	
	*pIndexBase = g_CachedIndexBase;

	// Activate the new native index buffer :
#ifdef CXBX_USE_D3D9
	HRESULT hRet = g_pD3DDevice->SetIndices(indexBuffer.pHostIndexBuffer);
	// Note : *pIndexBase is moved to DrawIndexedPrimitive
#else
	HRESULT hRet = g_pD3DDevice->SetIndices(indexBuffer.pHostIndexBuffer, *pIndexBase);
#endif
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetIndices");

	if (FAILED(hRet))
		CxbxKrnlCleanup("CxbxUpdateActiveIndexBuffer: SetIndices Failed!");
}

// LTCG specific Direct3D_CreateDevice function...
// This uses a custom calling convention passed unknown parameters
// Test-case: Ninja Gaiden
HRESULT WINAPI XTL::EMUPATCH(Direct3D_CreateDevice_4)
(
    X_D3DPRESENT_PARAMETERS     *pPresentationParameters
)
{
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pPresentationParameters)
		LOG_FUNC_END;

	// create default device *before* calling Xbox Direct3D_CreateDevice trampline
	// to avoid hitting EMUPATCH'es that need a valid g_pD3DDevice
	{
		// Wait until proxy is done with an existing call (i highly doubt this situation will come up)
		while (g_EmuCDPD.bReady)
			Sleep(10);

		// Cache parameters
		memcpy(&(g_EmuCDPD.XboxPresentationParameters), pPresentationParameters, sizeof(X_D3DPRESENT_PARAMETERS));

		// Signal proxy thread (this will trigger EmuCreateDeviceProxy to call CreateDevice)
		g_EmuCDPD.bCreate = true;
		g_EmuCDPD.bReady = true;

		// Wait until host proxy is completed (otherwise, Xbox code could hit patches that need an assigned g_pD3DDevice)
		while (g_EmuCDPD.bReady)
			Sleep(10);
	}

	// Only then call Xbox CreateDevice function
	XB_trampoline(HRESULT, WINAPI, Direct3D_CreateDevice_4, (X_D3DPRESENT_PARAMETERS*));
	HRESULT hRet = XB_Direct3D_CreateDevice_4(pPresentationParameters);

	// Set g_XboxD3DDevice to point to the Xbox D3D Device
	if ((DWORD*)XRefDataBase[XREF_D3DDEVICE] != nullptr && ((DWORD)XRefDataBase[XREF_D3DDEVICE]) != XREF_ADDR_DERIVE) {
		g_XboxD3DDevice = *((DWORD**)XRefDataBase[XREF_D3DDEVICE]);
	}

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
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Adapter)
		LOG_FUNC_ARG(DeviceType)
		LOG_FUNC_ARG(hFocusWindow)
		LOG_FUNC_ARG(pPresentationParameters)
		LOG_FUNC_END;

	// create default device *before* calling Xbox Direct3D_CreateDevice trampline
	// to avoid hitting EMUPATCH'es that need a valid g_pD3DDevice
	{
		// Wait until proxy is done with an existing call (i highly doubt this situation will come up)
		while (g_EmuCDPD.bReady)
			Sleep(10);

		// Cache parameters
		memcpy(&(g_EmuCDPD.XboxPresentationParameters), pPresentationParameters, sizeof(X_D3DPRESENT_PARAMETERS));

		// Signal proxy thread (this will trigger EmuCreateDeviceProxy to call CreateDevice)
		g_EmuCDPD.bCreate = true;
		g_EmuCDPD.bReady = true;

		// Wait until host proxy is completed (otherwise, Xbox code could hit patches that need an assigned g_pD3DDevice)
		while (g_EmuCDPD.bReady)
			Sleep(10);
	}

	// Only then call Xbox CreateDevice function
	XB_trampoline(HRESULT, WINAPI, Direct3D_CreateDevice_16, (UINT, D3DDEVTYPE, HWND, X_D3DPRESENT_PARAMETERS*));
	HRESULT hRet = XB_Direct3D_CreateDevice_16(Adapter, DeviceType, hFocusWindow, pPresentationParameters);

	// Set g_XboxD3DDevice to point to the Xbox D3D Device
	if ((DWORD*)XRefDataBase[XREF_D3DDEVICE] != nullptr && ((DWORD)XRefDataBase[XREF_D3DDEVICE]) != XREF_ADDR_DERIVE) {
		g_XboxD3DDevice = *((DWORD**)XRefDataBase[XREF_D3DDEVICE]);
	}

	return hRet;
}

// ******************************************************************
// * patch: D3DDevice_SetIndices
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetIndices)
(
	X_D3DIndexBuffer   *pIndexData,
	UINT                BaseVertexIndex
)
{
	FUNC_EXPORTS

		LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pIndexData)
		LOG_FUNC_ARG(BaseVertexIndex)
		LOG_FUNC_END;

	// Cache the base vertex index then call the Xbox function
	g_CachedIndexBase = BaseVertexIndex;

	XB_trampoline(HRESULT, WINAPI, D3DDevice_SetIndices, (X_D3DIndexBuffer*, UINT));
	RETURN(XB_D3DDevice_SetIndices(pIndexData, BaseVertexIndex));
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
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Adapter)
		LOG_FUNC_ARG(DeviceType)
		LOG_FUNC_ARG(hFocusWindow)
		LOG_FUNC_ARG(BehaviorFlags) // Xbox ignores BehaviorFlags
		LOG_FUNC_ARG(pPresentationParameters)
		LOG_FUNC_ARG(ppReturnedDeviceInterface)
		LOG_FUNC_END;

	
	// create default device *before* calling Xbox Direct3D_CreateDevice trampline
	// to avoid hitting EMUPATCH'es that need a valid g_pD3DDevice
	{
		// Wait until proxy is done with an existing call (i highly doubt this situation will come up)
		while (g_EmuCDPD.bReady)
			Sleep(10);

		// Cache parameters
		memcpy(&(g_EmuCDPD.XboxPresentationParameters), pPresentationParameters, sizeof(X_D3DPRESENT_PARAMETERS));

		// Signal proxy thread (this will trigger EmuCreateDeviceProxy to call CreateDevice)
		g_EmuCDPD.bCreate = true;
		g_EmuCDPD.bReady = true;

		// Wait until host proxy is completed (otherwise, Xbox code could hit patches that need an assigned g_pD3DDevice)
		while (g_EmuCDPD.bReady)
			Sleep(10);
	}

	// Only then call Xbox CreateDevice function
	XB_trampoline(HRESULT, WINAPI, Direct3D_CreateDevice, (UINT, D3DDEVTYPE, HWND, DWORD, X_D3DPRESENT_PARAMETERS*, IDirect3DDevice**));
	HRESULT hRet = XB_Direct3D_CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

	// Set g_XboxD3DDevice to point to the Xbox D3D Device
	if ((DWORD*)XRefDataBase[XREF_D3DDEVICE] != nullptr && ((DWORD)XRefDataBase[XREF_D3DDEVICE]) != XREF_ADDR_DERIVE) {
		g_XboxD3DDevice = *((DWORD**)XRefDataBase[XREF_D3DDEVICE]);
	}

	return hRet;
}

// ******************************************************************
// * patch: IDirect3DResource8_IsBusy
// ******************************************************************
BOOL WINAPI XTL::EMUPATCH(D3DDevice_IsBusy)()
{
	FUNC_EXPORTS

	LOG_FUNC();
    
    return FALSE;
}

// ******************************************************************
// * patch: D3DDevice_GetDisplayFieldStatus
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetDisplayFieldStatus)(X_D3DFIELD_STATUS *pFieldStatus)
{
	// NOTE: This can be unpatched only when NV2A does it's own VBlank and HLE _Swap function is unpatched
	FUNC_EXPORTS

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Count);

	if (g_pPrimaryPB != nullptr)
	{
		EmuWarning("D3DDevice_BeginPush called without D3DDevice_EndPush in between?!");
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
	FUNC_EXPORTS

		LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Count)
		LOG_FUNC_ARG(ppPush)
		LOG_FUNC_END;

	if (g_pPrimaryPB != nullptr)
	{
		EmuWarning("D3DDevice_BeginPush2 called without D3DDevice_EndPush in between?!");
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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(pPush);

#ifdef _DEBUG_TRACK_PB
//	DbgDumpPushBuffer(g_pPrimaryPB, g_dwPrimaryPBCount*sizeof(DWORD));
#endif

	if (g_pPrimaryPB == nullptr)
		EmuWarning("D3DDevice_EndPush called without preceding D3DDevice_BeginPush?!");
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
	FUNC_EXPORTS

	LOG_FUNC();

	LOG_UNIMPLEMENTED();
}

// LTCG specific D3DDevice_EndVisibilityTest function...
// This uses a custom calling convention where parameter is passed in EAX
// UNTESTED - Need test-case!
HRESULT __stdcall XTL::EMUPATCH(D3DDevice_EndVisibilityTest_0)
(
)
{
	FUNC_EXPORTS;

	DWORD                       Index;

	__asm {
		mov Index, eax
	}

	LOG_TEST_CASE("Validate this function!");
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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Index);

	LOG_UNIMPLEMENTED();

    return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_SetBackBufferScale
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetBackBufferScale)(FLOAT x, FLOAT y)
{
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
// This uses a custom calling convention where parameter is passed in EAX
// Test-case: Ninja Gaiden
VOID __stdcall XTL::EMUPATCH(D3DDevice_LoadVertexShader_4)
(
)
{
	FUNC_EXPORTS;

	static uint32 returnAddr;

#ifdef _DEBUG_TRACE
		__asm add esp, 4
#endif

	__asm {
		pop returnAddr
		push eax
		call EmuPatch_D3DDevice_LoadVertexShader
		mov eax, 0
		push returnAddr
		ret
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
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Handle)
		LOG_FUNC_ARG(Address)
		LOG_FUNC_END;

	// Handle is always address of an Xbox VertexShader struct, or-ed with 1 (X_D3DFVF_RESERVED0)
	// An Xbox VertexShader contains : a 'Vshd' signature, flags, a size, a program (and constants)
	// Address is the slot (offset) from which the program must be written onwards (as whole DWORDS)
	// D3DDevice_LoadVertexShader pushes the program contained in the Xbox VertexShader struct to the NV2A
    if(Address < 136 && VshHandleIsVertexShader(Handle))
    {
        VERTEX_SHADER *pVertexShader = (VERTEX_SHADER *)(VshHandleGetVertexShader(Handle))->Handle;
        for (DWORD i = Address; i < pVertexShader->Size; i++)
        {
            // TODO: This seems very fishy
            g_VertexShaderSlots[i] = Handle;
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
	FUNC_EXPORTS;

    DWORD                       Handle;
    DWORD                       Address;

	__asm {
		mov Address, eax
		mov Handle, ebx
	}

	return EMUPATCH(D3DDevice_SelectVertexShader)(Handle, Address);
}

// LTCG specific D3DDevice_SelectVertexShader function...
// This uses a custom calling convention where parameter is passed in EAX
// UNTESTED - Need test-case!
VOID __stdcall XTL::EMUPATCH(D3DDevice_SelectVertexShader_4)
(
)
{
	FUNC_EXPORTS;

	static uint32 returnAddr;

#ifdef _DEBUG_TRACE
		__asm add esp, 4
#endif

	__asm {
		pop returnAddr
		push eax
		call EmuPatch_D3DDevice_SelectVertexShader
		mov eax, 0
		push returnAddr
	}

	LOG_TEST_CASE("Validate this function!");
	return;
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
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Handle)
		LOG_FUNC_ARG(Address)
		LOG_FUNC_END;

	HRESULT hRet;

	// Address always indicates a previously loaded vertex shader slot (from where the program is used).
	// Handle can be null if the current Xbox VertexShader is assigned
	// Handle can be an address of an Xbox VertexShader struct, or-ed with 1 (X_D3DFVF_RESERVED0)
	// If Handle is assigned, it becomes the new current Xbox VertexShader,
	// which resets a bit of state (nv2a execution mode, viewport, ?)
	// Either way, the given address slot is selected as the start of the current vertex shader program
	g_CurrentXboxVertexShaderHandle = Handle;

    if(VshHandleIsVertexShader(Handle))
    {
        VERTEX_SHADER *pVertexShader = (VERTEX_SHADER *)(((X_D3DVertexShader *)(Handle & 0x7FFFFFFF))->Handle);
        hRet = g_pD3DDevice->SetVertexShader(pVertexShader->Handle);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexShader(VshHandleIsVertexShader)");
    }
    else if(Handle == NULL)
    {
#ifdef CXBX_USE_D3D9
		hRet = g_pD3DDevice->SetVertexShader(nullptr);
		hRet = g_pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_TEX0);
#else
		hRet = g_pD3DDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_TEX0);
#endif
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_TEX0)");
	}
    else if(Address < 136)
    {
        X_D3DVertexShader *pVertexShader = (X_D3DVertexShader*)g_VertexShaderSlots[Address];

        if(pVertexShader != NULL)
        {
			hRet = g_pD3DDevice->SetVertexShader(((VERTEX_SHADER *)((X_D3DVertexShader *)g_VertexShaderSlots[Address])->Handle)->Handle);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexShader(pVertexShader)");
		}
        else
        {
            EmuWarning("g_VertexShaderSlots[%d] = 0", Address);
			hRet = D3D_OK;
		}
    }

	if (FAILED(hRet))
	{
		EmuWarning("We're lying about setting a vertext shader!");

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
	FUNC_EXPORTS

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
#ifdef CXBX_USE_D3D9
		0, // iSwapChain
#endif
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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(pRamp);

    D3DGAMMARAMP *pGammaRamp = (D3DGAMMARAMP *)malloc(sizeof(D3DGAMMARAMP));

    g_pD3DDevice->GetGammaRamp(
#ifdef CXBX_USE_D3D9
		0, // iSwapChain
#endif
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
	FUNC_EXPORTS

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
#ifdef CXBX_USE_D3D9
			hRet = g_pD3DDevice->CreateOffscreenPlainSurface(640, 480, D3DFMT_A8R8G8B8, /*D3DPool=* /0, &pCachedPrimarySurface, nullptr);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateOffscreenPlainSurface");
#else
			hRet = g_pD3DDevice->CreateImageSurface(640, 480, D3DFMT_A8R8G8B8, &pCachedPrimarySurface);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateImageSurface");
#endif
		}

		SetHostSurface(pBackBuffer, pCachedPrimarySurface);

		hRet = g_pD3DDevice->GetFrontBuffer(pCachedPrimarySurface);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetFrontBuffer");

		if (FAILED(hRet)) {
			EmuWarning("Could not retrieve primary surface, using backbuffer");
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
#ifdef CXBX_USE_D3D9
			0, // iSwapChain
#endif
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
#ifdef CXBX_USE_D3D9
		0, // iSwapChain
#endif
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

	// Now we can fetch the host backbuffer
	XTL::IDirect3DSurface *pCurrentHostBackBuffer = nullptr;

	if (BackBuffer == -1) {
		BackBuffer = 0;
	}

	HRESULT hRet = g_pD3DDevice->GetBackBuffer(
#ifdef CXBX_USE_D3D9
		0, // iSwapChain
#endif
		BackBuffer, D3DBACKBUFFER_TYPE_MONO, &pCurrentHostBackBuffer);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetBackBuffer");

	if (FAILED(hRet)) {
		CxbxKrnlCleanup("Unable to retrieve back buffer");
	}

	XTL::IDirect3DSurface* pCopySrcSurface = nullptr;

	D3DLOCKED_RECT lockedRect;
	hRet = pCurrentHostBackBuffer->LockRect(&lockedRect, NULL, D3DLOCK_READONLY);
	if (hRet != D3D_OK) {
		EmuWarning("Could not lock Host Back Buffer");
	}
	else {
		D3DSURFACE_DESC hostSurfaceDesc;

		hRet = pCurrentHostBackBuffer->GetDesc(&hostSurfaceDesc);
		if (hRet != D3D_OK) {
			EmuWarning("Could not get Back Buffer Host Surface Desc");
		}
		else {
			// Get the host surface for the destination Xbox surface
			pCopySrcSurface = GetHostSurface(pXboxBackBuffer);
			// Do the copy from the host backbuffer to the host surface representing the backbuffer
			// This handles format conversion back to Xbox format (assuming the host surface is the same format), and makes sure our HostResource is up to date
			// That way if it gets used as a trex
			hRet = D3DXLoadSurfaceFromMemory(pCopySrcSurface, NULL, NULL, lockedRect.pBits, hostSurfaceDesc.Format, lockedRect.Pitch, NULL, NULL, 0, 0);
			// TODO : Make D3DXLoadSurfaceFromMemory stretch when Xbox size is different from host (pass SrcRect, DstRect and a stretching flag?)
		}

		pCurrentHostBackBuffer->UnlockRect();
	}

	pCurrentHostBackBuffer->Release();

	if (hRet == D3D_OK) {
		assert(pCopySrcSurface);

		D3DLOCKED_RECT copyLockedRect;

		hRet = pCopySrcSurface->LockRect(&copyLockedRect, NULL, D3DLOCK_READONLY);
		if (hRet != D3D_OK) {
			EmuWarning("Could not lock Host Resource for Xbox Back Buffer");
		}
		else {
			D3DSURFACE_DESC copySurfaceDesc;

			hRet = pCopySrcSurface->GetDesc(&copySurfaceDesc);
			if (hRet != D3D_OK) {
				EmuWarning("Could not get Xbox Back Buffer Host Surface Desc");
			}
			else {
#ifdef CXBX_USE_D3D9
				DWORD Size = copyLockedRect.Pitch * copySurfaceDesc.Height; // TODO : What about mipmap levels?
#else
				DWORD Size = copySurfaceDesc.Size;
#endif
				// Finally, do the copy from the converted host resource to the xbox resource
				memcpy((void*)GetDataFromXboxResource(pXboxBackBuffer), copyLockedRect.pBits, Size);
			}

			pCopySrcSurface->UnlockRect();
		}
	}

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
	FUNC_EXPORTS

	LOG_FORWARD("D3DDevice_GetBackBuffer2");

    *ppBackBuffer = EMUPATCH(D3DDevice_GetBackBuffer2)(BackBuffer);
}

bool GetHostRenderTargetDimensions(DWORD *pHostWidth, DWORD *pHostHeight)
{
	XTL::IDirect3DSurface* pHostRenderTarget = nullptr;

	g_pD3DDevice->GetRenderTarget(
#ifdef CXBX_USE_D3D9
		0, // RenderTargetIndex
#endif
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
	if (g_EmuCDPD.XboxPresentationParameters.Flags & X_D3DPRESENTFLAG_FIELD) {
		HostRenderTarget_Desc.Height /= 2;
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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(pViewport);

	D3DVIEWPORT HostViewPort = *pViewport;

	if (g_ScaleViewport) {
#if 0 // Disabled for now, as the Xbox code triggers an error-code 6 in uc_emu_start()
		// Use a trampoline here, so GetViewport can be unpatched
		XB_trampoline(VOID, WINAPI, D3DDevice_SetViewport, (CONST X_D3DVIEWPORT8 *));
		XB_D3DDevice_SetViewport(pViewport);
#endif

		// Get current Xbox render target dimensions
		DWORD XboxRenderTarget_Width = GetPixelContainerWidth(g_pXboxRenderTarget);
		DWORD XboxRenderTarget_Height = GetPixelContainerHeigth(g_pXboxRenderTarget);

		// Get current host render target dimensions
		DWORD HostRenderTarget_Width;
		DWORD HostRenderTarget_Height;

		if (GetHostRenderTargetDimensions(&HostRenderTarget_Width, &HostRenderTarget_Height)) {

			// Scale Xbox to host dimensions (avoiding hard-coding 640 x 480)
			HostViewPort.X = ScaleDWORD(pViewport->X, XboxRenderTarget_Width, HostRenderTarget_Width);
			HostViewPort.Y = ScaleDWORD(pViewport->Y, XboxRenderTarget_Height, HostRenderTarget_Height);
			HostViewPort.Width = ScaleDWORD(pViewport->Width, XboxRenderTarget_Width, HostRenderTarget_Width);
			HostViewPort.Height = ScaleDWORD(pViewport->Height, XboxRenderTarget_Height, HostRenderTarget_Height);
			// TODO : Fix test-case Shenmue 2 (which halves height, leaving the bottom half unused)
			HostViewPort.MinZ = pViewport->MinZ; // No need scale Z for now
			HostViewPort.MaxZ = pViewport->MaxZ;
		}
		else {
			EmuWarning("GetHostRenderTargetDimensions failed - SetViewport sets Xbox viewport instead!");
		}
	}

	HRESULT hRet = g_pD3DDevice->SetViewport(&HostViewPort);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetViewport");
}

// ******************************************************************
// * patch: D3DDevice_GetViewport
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetViewport)
(
    X_D3DVIEWPORT8 *pViewport
)
{
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(pViewport);

	D3DVIEWPORT HostViewPort;

	HRESULT hRet = g_pD3DDevice->GetViewport(&HostViewPort);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetViewport");

	if (!g_ScaleViewport) {
		*pViewport = HostViewPort;
	}
	else {
		// Note : We cannot return the Xbox viewport as set in EMUPATCH(D3DDevice_SetViewport)
		// because various Xbox D3D functions reset the Xbox viewport. Since we call comparable
		// functions on host D3D, the host viewport is better suited as a return value;
		// We just need to scale the host viewport back to Xbox dimensions - the exact opposite
		// operation from the up-scaling that happens in EMUPATCH(D3DDevice_SetViewport).

		// Get current host render target dimensions
		DWORD HostRenderTarget_Width;
		DWORD HostRenderTarget_Height;

		if (GetHostRenderTargetDimensions(&HostRenderTarget_Width, &HostRenderTarget_Height)) {

			// Get current Xbox render target dimensions
			DWORD XboxRenderTarget_Width = GetPixelContainerWidth(g_pXboxRenderTarget);
			DWORD XboxRenderTarget_Height = GetPixelContainerHeigth(g_pXboxRenderTarget);

			// Scale host back to Xbox dimensions (avoiding hard-coding 640 x 480)
			pViewport->X = ScaleDWORD(HostViewPort.X, HostRenderTarget_Width, XboxRenderTarget_Width);
			pViewport->Y = ScaleDWORD(HostViewPort.Y, HostRenderTarget_Height, XboxRenderTarget_Height);
			pViewport->Width = ScaleDWORD(HostViewPort.Width, HostRenderTarget_Width, XboxRenderTarget_Width);
			pViewport->Height = ScaleDWORD(HostViewPort.Height, HostRenderTarget_Height, XboxRenderTarget_Height);
			pViewport->MinZ = HostViewPort.MinZ; // No need scale Z for now
			pViewport->MaxZ = HostViewPort.MaxZ;
		}
		else {
			*pViewport = HostViewPort;
			EmuWarning("GetHostRenderTargetDimensions failed - GetViewport returns host viewport instead!");
		}
	}
}

// LTCG specific D3DDevice_GetViewportOffsetAndScale function...
// This function is still not working so should be investigated...
// This uses a custom calling convention where parameter is passed in EDX, ECX
// Test-case: RalliSport Challenge 2
VOID __stdcall XTL::EMUPATCH(D3DDevice_GetViewportOffsetAndScale_0)
(
)
{
	//FUNC_EXPORTS;

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
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pOffset)
		LOG_FUNC_ARG(pScale)
		LOG_FUNC_END;

	LOG_TEST_CASE("D3DDevice_GetViewportOffsetAndScale"); // Get us some test-cases
	// Test case : Spongebob - Battle for Bikini Bottom

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
	FUNC_EXPORTS;

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Mode);

    g_VertexShaderConstantMode = Mode;
}

// ******************************************************************
// * patch: D3DDevice_GetRenderTarget
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetRenderTarget)
(
    X_D3DSurface  **ppRenderTarget
)
{
	FUNC_EXPORTS

	LOG_FORWARD("D3DDevice_GetRenderTarget2");

	*ppRenderTarget = EMUPATCH(D3DDevice_GetRenderTarget2)();

    return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_GetRenderTarget2
// ******************************************************************
XTL::X_D3DSurface * WINAPI XTL::EMUPATCH(D3DDevice_GetRenderTarget2)()
{
	FUNC_EXPORTS

	LOG_FUNC();

	X_D3DSurface *result = g_pXboxRenderTarget;

	if (result)
		result->Common++; // EMUPATCH(D3DResource_AddRef)(result);

    RETURN(result);
}

// ******************************************************************
// * patch: D3DDevice_GetDepthStencilSurface
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetDepthStencilSurface)
(
    X_D3DSurface  **ppZStencilSurface
)
{
	FUNC_EXPORTS

	LOG_FORWARD("D3DDevice_GetDepthStencilSurface2");

    *ppZStencilSurface = EMUPATCH(D3DDevice_GetDepthStencilSurface2)();

    return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_GetDepthStencilSurface2
// ******************************************************************
XTL::X_D3DSurface * WINAPI XTL::EMUPATCH(D3DDevice_GetDepthStencilSurface2)()
{
	FUNC_EXPORTS

	LOG_FUNC();

	X_D3DSurface *result = g_pXboxDepthStencil;

	if (result)
		result->Common++; // EMUPATCH(D3DResource_AddRef)(result);
		
	RETURN(result);
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
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pDeclaration)
		LOG_FUNC_ARG(pFunction)
		LOG_FUNC_ARG(pHandle)
		LOG_FUNC_ARG_TYPE(X_D3DUSAGE, Usage)
		LOG_FUNC_END;

	// Allocates an Xbox VertexShader struct
	// Sets reference count to 1
	// Puts Usage in VertexShader->Flags
	// If pFunction is not null, it points to DWORDS shader type, length and a binary compiled xbox vertex shader
	// If pDeclaration is not null, it's parsed, resulting in a number of constants
	// Parse results are pushed to the push buffer
	// Sets other fields
	// pHandle recieves the addres of the new shader, or-ed with 1 (D3DFVF_RESERVED0)

    // create emulated shader struct
    X_D3DVertexShader *pD3DVertexShader = (X_D3DVertexShader*)g_VMManager.AllocateZeroed(sizeof(X_D3DVertexShader));
    VERTEX_SHADER     *pVertexShader = (VERTEX_SHADER*)g_VMManager.AllocateZeroed(sizeof(VERTEX_SHADER));

    // TODO: Intelligently fill out these fields as necessary

    // HACK: TODO: support this situation
    if(pDeclaration == NULL)
    {
        *pHandle = NULL;

        

        return D3D_OK;
    }

    LPD3DXBUFFER pRecompiledBuffer = NULL;
    DWORD        *pRecompiledDeclaration = NULL;
    DWORD        *pRecompiledFunction = NULL;
    DWORD        VertexShaderSize = 0;
    DWORD        DeclarationSize = 0;
    DWORD        Handle = 0;

    HRESULT hRet = XTL::EmuRecompileVshDeclaration((DWORD*)pDeclaration,
                                                   &pRecompiledDeclaration,
                                                   &DeclarationSize,
                                                   pFunction == NULL,
                                                   &pVertexShader->VertexDynamicPatch);

    if(SUCCEEDED(hRet) && pFunction)
    {
		boolean bUseDeclarationOnly = 0;

        hRet = XTL::EmuRecompileVshFunction((DWORD*)pFunction,
                                            &pRecompiledBuffer,
                                            &VertexShaderSize,
                                            g_VertexShaderConstantMode == X_D3DSCM_NORESERVEDCONSTANTS,
											&bUseDeclarationOnly,
											pRecompiledDeclaration);
        if(SUCCEEDED(hRet))
        {
			if(!bUseDeclarationOnly)
				pRecompiledFunction = (DWORD*)pRecompiledBuffer->GetBufferPointer();
			else
				pRecompiledFunction = NULL;
        }
        else
        {
            pRecompiledFunction = NULL;
            EmuWarning("Couldn't recompile vertex shader function.");
            hRet = D3D_OK; // Try using a fixed function vertex shader instead
        }
    }

    //DbgPrintf("MaxVertexShaderConst = %d\n", g_D3DCaps.MaxVertexShaderConst);

    if(SUCCEEDED(hRet))
    {
        hRet = g_pD3DDevice->CreateVertexShader
        (
            pRecompiledDeclaration,
            pRecompiledFunction,
            &Handle,
            g_dwVertexShaderUsage   // TODO: HACK: Xbox has extensions!
        );
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateVertexShader");

        if(pRecompiledBuffer != nullptr)
        {
            pRecompiledBuffer->Release();
            pRecompiledBuffer = nullptr;
        }

        //* Fallback to dummy shader.
        if(FAILED(hRet))
        {
            static const char dummy[] =
                "vs.1.1\n"
				"dp4 oPos.x, v0, c96\n"
				"dp4 oPos.y, v0, c97\n"
				"dp4 oPos.z, v0, c98\n"
				"dp4 oPos.w, v0, c99\n";

            EmuWarning("Trying fallback:\n%s", dummy);
            hRet = D3DXAssembleShader(dummy,
                                      strlen(dummy),
                                      D3DXASM_SKIPVALIDATION,
                                      NULL,
                                      &pRecompiledBuffer,
                                      NULL);
			DEBUG_D3DRESULT(hRet, "D3DXAssembleShader");

			hRet = g_pD3DDevice->CreateVertexShader
            (
                pRecompiledDeclaration,
                (DWORD*)pRecompiledBuffer->GetBufferPointer(),
                &Handle,
                g_dwVertexShaderUsage
            );
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateVertexShader(fallback)");
		}
        //*/
    }
    // Save the status, to remove things later
    pVertexShader->Status = hRet;

    free(pRecompiledDeclaration);

    pVertexShader->pDeclaration = (DWORD*)g_VMManager.Allocate(DeclarationSize);
    memcpy(pVertexShader->pDeclaration, pDeclaration, DeclarationSize);

    pVertexShader->FunctionSize = 0;
    pVertexShader->pFunction = NULL;
    pVertexShader->Type = X_VST_NORMAL;
    pVertexShader->Size = (VertexShaderSize - sizeof(VSH_SHADER_HEADER)) / VSH_INSTRUCTION_SIZE_BYTES;
    pVertexShader->DeclarationSize = DeclarationSize;

    if(SUCCEEDED(hRet))
    {
        if(pFunction != NULL)
        {
            pVertexShader->pFunction = (DWORD*)g_VMManager.Allocate(VertexShaderSize);
            memcpy(pVertexShader->pFunction, pFunction, VertexShaderSize);
            pVertexShader->FunctionSize = VertexShaderSize;
        }
        else
        {
            pVertexShader->pFunction = NULL;
            pVertexShader->FunctionSize = 0;
        }
        pVertexShader->Handle = Handle;
    }
    else
    {
        pVertexShader->Handle = D3DFVF_XYZ | D3DFVF_TEX0;
    }

    pD3DVertexShader->Handle = (DWORD)pVertexShader;

	*pHandle = (DWORD)pD3DVertexShader; // DON'T collide with PHYSICAL_MAP_BASE (see VshHandleIsFVF and VshHandleIsVertexShader)

    if(FAILED(hRet))
    {
#ifdef _DEBUG_TRACK_VS
        if (pFunction)
        {
            char pFileName[30];
            static int FailedShaderCount = 0;
            VSH_SHADER_HEADER *pHeader = (VSH_SHADER_HEADER*)pFunction;
            EmuWarning("Couldn't create vertex shader!");
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
	FUNC_EXPORTS;

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
	FUNC_EXPORTS

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

	if(g_BuildVersion <= 4361)
		Register += 96;

    HRESULT hRet;
#ifdef CXBX_USE_D3D9
	hRet = g_pD3DDevice->SetVertexShaderConstantF(
		Register,
		(float*)pConstantData,
		ConstantCount
	);
#else
    hRet = g_pD3DDevice->SetVertexShaderConstant
    (
        Register,
        pConstantData,
        ConstantCount
    );
#endif
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexShaderConstant");

    if(FAILED(hRet))
    {
        EmuWarning("We're lying about setting a vertex shader constant!");
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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

	LOG_FORWARD("D3DDevice_SetVertexShaderConstant");

	// Redirect to the standard version.

	EMUPATCH(D3DDevice_SetVertexShaderConstant)(Register, pConstantData, ConstantCount / 4);
}

BOOL g_bBadIndexData = FALSE;

// LTCG specific D3DDevice_SetTexture function...
// This uses a custom calling convention where parameter is passed in EAX
// TODO: XB_trampoline plus Log function is not working due lost parameter in EAX.
// Test-case: Metal Wolf Chaos
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetTexture_4)
(
	X_D3DBaseTexture  *pTexture
)
{
	FUNC_EXPORTS;

	DWORD           Stage;
	__asm mov Stage, eax;

	//LOG_FUNC_BEGIN
	//	LOG_FUNC_ARG(Stage)
	//	LOG_FUNC_ARG(pTexture)
	//	LOG_FUNC_END;
	DbgPrintf("D3DDevice_SetTexture_4(Stage : %d pTexture : %08x);\n", Stage, pTexture);

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Method)
		LOG_FUNC_ARG(Data)
		LOG_FUNC_ARG(Format)
		LOG_FUNC_END;

    DWORD StageLookup[TEXTURE_STAGES] = { 0x00081b00, 0x00081b40, 0x00081b80, 0x00081bc0 };
    DWORD Stage = -1;

    for(int v=0;v<TEXTURE_STAGES;v++)
    {
        if(StageLookup[v] == Method)
        {
            Stage = v;
        }
    }

    if(Stage == -1)
    {
        EmuWarning("Unknown Method (0x%.08X)", Method);
    }
    else
    {
		// Switch Texture updates the data pointer of an active texture using pushbuffer commands
		// assert(EmuD3DActiveTexture[Stage] != xbnullptr);

		// Update data and format separately, instead of via GetDataFromXboxResource()
		CxbxActiveTextureCopies[Stage].Common = EmuD3DActiveTexture[Stage]->Common;
		CxbxActiveTextureCopies[Stage].Data = Data;
		CxbxActiveTextureCopies[Stage].Format = Format;
		CxbxActiveTextureCopies[Stage].Lock = 0;
		CxbxActiveTextureCopies[Stage].Size = EmuD3DActiveTexture[Stage]->Size;

		// Use the above modified copy, instead of altering the active Xbox texture
		EmuD3DActiveTexture[Stage] = &CxbxActiveTextureCopies[Stage];
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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

	LOG_FORWARD("D3DDevice_SetVertexData4f");

    DWORD dwA = a, dwB = b;

    EMUPATCH(D3DDevice_SetVertexData4f)(Register, DWtoF(dwA), DWtoF(dwB), 0.0f, 1.0f);
}

DWORD FloatsToDWORD(FLOAT d, FLOAT a, FLOAT b, FLOAT c)
{
	DWORD ca = (FtoDW(d) << 24);
	DWORD cr = (FtoDW(a) << 16) & 0x00FF0000;
	DWORD cg = (FtoDW(b) << 8) & 0x0000FF00;
	DWORD cb = (FtoDW(c) << 0) & 0x000000FF;

	return ca | cr | cg | cb;
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
	FUNC_EXPORTS

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
		DbgPrintf("Reallocated g_InlineVertexBuffer_Table to %d entries\n", g_InlineVertexBuffer_TableLength);
	}

	// Is this the initial call after D3DDevice_Begin() ?
	if (g_InlineVertexBuffer_FVF == 0) {
		// Set first vertex to zero (preventing leaks from prior Begin/End calls)
		g_InlineVertexBuffer_Table[0] = {};
		// Set default diffuse color to white
		g_InlineVertexBuffer_Table[0].Diffuse = D3DCOLOR_ARGB(255, 255, 255, 255);
		// TODO : Handle D3DUSAGE_PERSISTENTDIFFUSE, D3DUSAGE_PERSISTENTSPECULAR,
		// D3DUSAGE_PERSISTENTBACKDIFFUSE, and D3DUSAGE_PERSISTENTBACKSPECULAR 
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
				EmuWarning("D3DDevice_SetVertexData4f unexpected FVF when selecting D3DFVF_XYZ(RHW) : %x", g_InlineVertexBuffer_FVF);
				// TODO : How to resolve this?
			}

			// Start a new vertex
			g_InlineVertexBuffer_TableOffset++;
			// Copy all attributes of the previous vertex (if any) to the new vertex
			static UINT uiPreviousOffset = ~0; // Can't use 0, as that may be copied too
			if (uiPreviousOffset != ~0) {
				g_InlineVertexBuffer_Table[g_InlineVertexBuffer_TableOffset] = g_InlineVertexBuffer_Table[uiPreviousOffset];
			}
			uiPreviousOffset = g_InlineVertexBuffer_TableOffset;
	
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
				EmuWarning("D3DDevice_SetVertexData4f unexpected FVF when processing X_D3DVSDE_BLENDWEIGHT : %x", g_InlineVertexBuffer_FVF);
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
            g_InlineVertexBuffer_Table[o].Diffuse = FloatsToDWORD(d, a, b, c);
            g_InlineVertexBuffer_FVF |= D3DFVF_DIFFUSE;
			break;
        }

		case X_D3DVSDE_SPECULAR:
        {
            g_InlineVertexBuffer_Table[o].Specular = FloatsToDWORD(d, a, b, c);
            g_InlineVertexBuffer_FVF |= D3DFVF_SPECULAR;
			break;
        }

		case X_D3DVSDE_FOG: // Xbox extension
		{
			g_InlineVertexBuffer_Table[o].Fog = a; // TODO : What about the other (b, c and d) arguments?
			EmuWarning("Host Direct3D8 doesn''t support FVF FOG");
			break;
		}

		// Note : X_D3DVSDE_POINTSIZE: Maps to D3DFVF_PSIZE, which is not available on Xbox FVF's

		case X_D3DVSDE_BACKDIFFUSE: // Xbox extension
		{
			g_InlineVertexBuffer_Table[o].BackDiffuse = FloatsToDWORD(d, a, b, c);
			EmuWarning("Host Direct3D8 doesn''t support FVF BACKDIFFUSE");
			break;
		}

		case X_D3DVSDE_BACKSPECULAR: // Xbox extension
		{
			g_InlineVertexBuffer_Table[o].BackSpecular = FloatsToDWORD(d, a, b, c);
			EmuWarning("Host Direct3D8 doesn''t support FVF BACKSPECULAR");
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
            EmuWarning("Unknown IVB Register : %d", Register);
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
	FUNC_EXPORTS

	LOG_FORWARD("D3DDevice_SetVertexData4f");

	DWORD dwA = a, dwB = b, dwC = c, dwD = d;

    EMUPATCH(D3DDevice_SetVertexData4f)(Register, DWtoF(dwA), DWtoF(dwB), DWtoF(dwC), DWtoF(dwD));
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
	FUNC_EXPORTS

	LOG_FORWARD("D3DDevice_SetVertexData4f");

	DWORD dwA = a, dwB = b, dwC = c, dwD = d;

    EMUPATCH(D3DDevice_SetVertexData4f)(Register, DWtoF(dwA), DWtoF(dwB), DWtoF(dwC), DWtoF(dwD));
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
	FUNC_EXPORTS

	LOG_FORWARD("D3DDevice_SetVertexData4f");

    FLOAT a = DWtoF((Color & 0xFF000000) >> 24);
    FLOAT r = DWtoF((Color & 0x00FF0000) >> 16);
    FLOAT g = DWtoF((Color & 0x0000FF00) >> 8);
    FLOAT b = DWtoF((Color & 0x000000FF) >> 0);

    EMUPATCH(D3DDevice_SetVertexData4f)(Register, r, g, b, a);
}

// ******************************************************************
// * patch: D3DDevice_End
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_End)()
{
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Count)
		LOG_FUNC_ARG(pRects)
		LOG_FUNC_ARG(Flags)
		LOG_FUNC_ARG(Color)
		LOG_FUNC_ARG(Z)
		LOG_FUNC_ARG(Stencil)
		LOG_FUNC_END;

    // make adjustments to parameters to make sense with windows d3d
    {
        DWORD newFlags = 0;

		if (Flags & X_D3DCLEAR_TARGET) {
			// TODO: D3DCLEAR_TARGET_A, *R, *G, *B don't exist on windows
			if ((Flags & X_D3DCLEAR_TARGET) != X_D3DCLEAR_TARGET)
				EmuWarning("Unsupported : Partial D3DCLEAR_TARGET flag(s) for D3DDevice_Clear : 0x%.08X", Flags & X_D3DCLEAR_TARGET);
		
			newFlags |= D3DCLEAR_TARGET;
		}

        // Do not needlessly clear Z Buffer
		if (Flags & X_D3DCLEAR_ZBUFFER) {
			if (g_bHasDepthStencil)
				newFlags |= D3DCLEAR_ZBUFFER;
			else
				EmuWarning("Unsupported : D3DCLEAR_ZBUFFER flag for D3DDevice_Clear without ZBuffer");
		}

		// Only clear depth buffer and stencil if present
		//
		// Avoids following DirectX Debug Runtime error report
		//    [424] Direct3D8: (ERROR) :Invalid flag D3DCLEAR_ZBUFFER: no zbuffer is associated with device. Clear failed. 
		if (Flags & X_D3DCLEAR_STENCIL) {
			if (g_bHasDepthStencil) // TODO : Introduce/use g_bHasStencil
				newFlags |= D3DCLEAR_STENCIL;
			else
				EmuWarning("Unsupported : D3DCLEAR_STENCIL flag for D3DDevice_Clear without ZBuffer");
		}

        if(Flags & ~(X_D3DCLEAR_TARGET | X_D3DCLEAR_ZBUFFER | X_D3DCLEAR_STENCIL))
            EmuWarning("Unsupported Flag(s) for D3DDevice_Clear : 0x%.08X", Flags & ~(X_D3DCLEAR_TARGET | X_D3DCLEAR_ZBUFFER | X_D3DCLEAR_STENCIL));

        Flags = newFlags;
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

    hRet = g_pD3DDevice->Clear(Count, pRects, Flags, Color, Z, Stencil);
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
	FUNC_EXPORTS

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
	FUNC_EXPORTS;

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
	FUNC_EXPORTS

		LOG_FUNC_ONE_ARG(Flags);

	// TODO: Ensure this flag is always the same across library versions
	if (Flags != 0)
		if (Flags != CXBX_SWAP_PRESENT_FORWARD) // Avoid a warning when forwarded
			EmuWarning("XTL::EmuD3DDevice_Swap: Flags != 0");

	CxbxReleaseBackBufferLock();

	g_pD3DDevice->EndScene();
	HRESULT hRet = g_pD3DDevice->Present(0, 0, 0, 0);
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
void CreateHostResource(XTL::X_D3DResource *pResource, int iTextureStage, DWORD dwSize)
{

	// DO NOT FUNC_EXPORTS!

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
		EmuWarning("CreateHostResource :-> Unrecognized Xbox Resource Type 0x%.08X", XboxResourceType);
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
		EmuWarning("CreateHostResource :-> %s carries no data - skipping conversion", ResourceTypeName);
		return;
	}

    switch (XboxResourceType) {
	case XTL::X_D3DRTYPE_NONE: {
		break;
	}

	case XTL::X_D3DRTYPE_SURFACE:
	case XTL::X_D3DRTYPE_VOLUME:
	case XTL::X_D3DRTYPE_TEXTURE:
	case XTL::X_D3DRTYPE_VOLUMETEXTURE:
	case XTL::X_D3DRTYPE_CUBETEXTURE: {
		XTL::X_D3DPixelContainer *pPixelContainer = (XTL::X_D3DPixelContainer*)pResource;
		XTL::X_D3DFORMAT X_Format = GetXboxPixelContainerFormat(pPixelContainer);
		DWORD D3DUsage = 0;
		XTL::D3DPOOL D3DPool = XTL::D3DPOOL_MANAGED; // TODO : Nuance D3DPOOL where/when needed

		if (EmuXBFormatIsDepthBuffer(X_Format)) {
			D3DUsage = D3DUSAGE_DEPTHSTENCIL;
			D3DPool = XTL::D3DPOOL_DEFAULT;
		}
		else if (pPixelContainer == g_pXboxRenderTarget) {
			if (EmuXBFormatIsRenderTarget(X_Format))
				D3DUsage = D3DUSAGE_RENDERTARGET;
			else
				EmuWarning("Updating RenderTarget %s with an incompatible format!", ResourceTypeName);
		}

		// Determine the format we'll be using on host D3D
		XTL::D3DFORMAT PCFormat;
		bool bConvertToARGB = false;

		if (EmuXBFormatRequiresConversionToARGB(X_Format)) {
			bConvertToARGB = true;
			PCFormat = XTL::D3DFMT_A8R8G8B8;
		}
		else {
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
			}
			else {
				// Otherwise, choose a fallback for the format not supported on host
				switch (X_Format) {
				case XTL::X_D3DFMT_LIN_D24S8: { // Note : This case could be removed, as the default below can handle it too
					EmuWarning("D3DFMT_LIN_D24S8 %s not supported - using D3DFMT_A8R8G8B8!", ResourceTypeName);
					// Note : This cannot set bConvertToARGB - we just copy it as-is
					PCFormat = XTL::D3DFMT_A8R8G8B8;
					break;
				}
				case XTL::X_D3DFMT_LIN_D16: {
					// Test case : Turok (when entering menu)
					EmuWarning("D3DFMT_LIN_D16 %s not supported - USING D3DFMT_R5G6B5!", ResourceTypeName);
					// Note : This cannot set bConvertToARGB - we just copy it as-is
					PCFormat = XTL::D3DFMT_R5G6B5; // TODO : Do something smarter
					break;
				}
				case XTL::X_D3DFMT_X1R5G5B5: { // Note : This case could be removed, as the default below can handle it too
					// Test case : JSRF (after loading)
					EmuWarning("D3DFMT_X1R5G5B5 %s will be converted to ARGB", ResourceTypeName);
					bConvertToARGB = true;
					PCFormat = XTL::D3DFMT_A8R8G8B8;
					break;
				}
				default:
					// Can the format be converted to ARGB?
					if (EmuXBFormatCanBeConvertedToARGB(X_Format)) {
						EmuWarning("Xbox %s Format %x will be converted to ARGB", ResourceTypeName, X_Format);
						bConvertToARGB = true;
						PCFormat = XTL::D3DFMT_A8R8G8B8;
					}
					else {
						// Otherwise, use a best matching format
						/*CxbxKrnlCleanup*/EmuWarning("Encountered a completely incompatible %s format!", ResourceTypeName);
						PCFormat = EmuXB2PC_D3DFormat(X_Format);
					}
				}
			}
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

		if (dwDepth != 1) {
			LOG_TEST_CASE("CreateHostResource : Depth != 1");
			EmuWarning("Unsupported depth (%d) - resetting to 1 for now", dwDepth);
			dwDepth = 1;
		}

		// The following is necessary for DXT* textures (4x4 blocks minimum)
		// TODO: Figure out if this is necessary under other circumstances?
		if (bCompressed) {
			if (dwWidth < dwMinSize) {
				LOG_TEST_CASE("CreateHostResource : dwWidth < dwMinSize");
				EmuWarning("Expanding %s width (%d->%d)", ResourceTypeName, dwWidth, dwMinSize);
				dwWidth = dwMinSize;
			}

			if (dwHeight < dwMinSize) {
				LOG_TEST_CASE("CreateHostResource : dwHeight < dwMinSize");
				EmuWarning("Expanding %s height (%d->%d)", ResourceTypeName, dwHeight, dwMinSize);
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
			if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
				hRet = g_pD3DDevice->CreateDepthStencilSurface(dwWidth, dwHeight, PCFormat,
					g_EmuCDPD.HostPresentationParameters.MultiSampleType, 
#ifdef CXBX_USE_D3D9
					0, // MultisampleQuality
					false, // Discard
#endif
					&pNewHostSurface
#ifdef CXBX_USE_D3D9
					, nullptr
#endif
				);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateDepthStencilSurface");
			}
			else {
#ifdef CXBX_USE_D3D9
				D3DPool = D3DPOOL_SYSTEMMEM;
				hRet = g_pD3DDevice->CreateOffscreenPlainSurface(dwWidth, dwHeight, PCFormat, D3DPool, &pNewHostSurface, nullptr);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateOffscreenPlainSurface");
#else
				hRet = g_pD3DDevice->CreateImageSurface(dwWidth, dwHeight, PCFormat, &pNewHostSurface);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateImageSurface");
#endif
			}

			// First fail, retry with a fallback format
			// If this succeeds, the surface may not render correctly, but it won't crash
			if (hRet != D3D_OK) {
				if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
					EmuWarning("CreateDepthStencilSurface Failed\n\nError: %s\nDesc: %s",
						XTL::DXGetErrorString(hRet), XTL::DXGetErrorDescription(hRet));
				}
				else {
					EmuWarning("CreateImageSurface Failed\n\nError: %s\nDesc: %s",
						XTL::DXGetErrorString(hRet), XTL::DXGetErrorDescription(hRet));
				}

				EmuWarning("Trying Fallback");
#ifdef CXBX_USE_D3D9
				hRet = g_pD3DDevice->CreateOffscreenPlainSurface(dwWidth, dwHeight, PCFormat, D3DPool, &pNewHostSurface, nullptr);
#else
				hRet = g_pD3DDevice->CreateImageSurface(dwWidth, dwHeight, XTL::D3DFMT_A8R8G8B8, &pNewHostSurface);
#endif
			}

			// If the fallback failed, show an error and exit execution.
			if (hRet != D3D_OK) {
				// We cannot safely continue in this state.
				CxbxKrnlCleanup("CreateImageSurface Failed!\n\nError: %s\nDesc: %s",
					XTL::DXGetErrorString(hRet), XTL::DXGetErrorDescription(hRet));
			}

			SetHostSurface(pResource, pNewHostSurface);
			DbgPrintf("CreateHostResource : Successfully created %s (0x%.08X, 0x%.08X)\n",
				ResourceTypeName, pResource, pNewHostSurface);
			DbgPrintf("CreateHostResource : Width : %d, Height : %d, Format : %d\n",
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
			// DbgPrintf("CreateHostResource : Successfully created %s (0x%.08X, 0x%.08X)\n",
			//	ResourceTypeName, pResource, pNewHostVolume);
			break;
		}

		case XTL::X_D3DRTYPE_TEXTURE: {
/* TODO : Enabled this if D3DPool is ever anything else but D3DPOOL_MANAGED :
#ifdef CXBX_USE_D3D9
			if (D3DPool == D3DPOOL_DEFAULT) {
				if ((D3DUsage & D3DUSAGE_DYNAMIC) == 0) {
					if ((D3DUsage & D3DUSAGE_RENDERTARGET) == 0) {
						D3DUsage |= D3DUSAGE_DYNAMIC;
					}
				}
			}
#endif
*/
			hRet = g_pD3DDevice->CreateTexture(dwWidth, dwHeight, dwMipMapLevels,
				D3DUsage, PCFormat, D3DPool, &pNewHostTexture
#ifdef CXBX_USE_D3D9
				, nullptr
#endif
			);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateTexture");

			// If the above failed, we might be able to use an ARGB texture instead
			if ((hRet != D3D_OK) && (PCFormat != XTL::D3DFMT_A8R8G8B8) && EmuXBFormatCanBeConvertedToARGB(X_Format)) {
				hRet = g_pD3DDevice->CreateTexture(dwWidth, dwHeight, dwMipMapLevels,
					D3DUsage, XTL::D3DFMT_A8R8G8B8, D3DPool, &pNewHostTexture
#ifdef CXBX_USE_D3D9
					, nullptr
#endif
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
					D3DPOOL_SYSTEMMEM, &pNewHostTexture
#ifdef CXBX_USE_D3D9
					, nullptr
#endif
					);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateTexture(D3DPOOL_SYSTEMMEM)");
			}*/


			if (hRet != D3D_OK) {
				CxbxKrnlCleanup("CreateTexture Failed!\n\n"
					"Error: 0x%X\nFormat: %d\nDimensions: %dx%d", hRet, PCFormat, dwWidth, dwHeight);
			}

			SetHostTexture(pResource, pNewHostTexture);
			DbgPrintf("CreateHostResource : Successfully created %s (0x%.08X, 0x%.08X)\n",
				ResourceTypeName, pResource, pNewHostTexture);
			break;
		}

		case XTL::X_D3DRTYPE_VOLUMETEXTURE: {
			hRet = g_pD3DDevice->CreateVolumeTexture(dwWidth, dwHeight, dwDepth,
				dwMipMapLevels, D3DUsage, PCFormat, D3DPool, &pNewHostVolumeTexture
#ifdef CXBX_USE_D3D9
				, nullptr
#endif
			);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateVolumeTexture");

			if (hRet != D3D_OK) {
				CxbxKrnlCleanup("CreateVolumeTexture Failed!\n\nError: %s\nDesc: %s",
					XTL::DXGetErrorString(hRet), XTL::DXGetErrorDescription(hRet));
			}

			SetHostVolumeTexture(pResource, pNewHostVolumeTexture);
			DbgPrintf("CreateHostResource : Successfully created %s (0x%.08X, 0x%.08X)\n",
				ResourceTypeName, pResource, pNewHostVolumeTexture);
			break;
		}

		case XTL::X_D3DRTYPE_CUBETEXTURE: {
			DbgPrintf("CreateCubeTexture(%d, %d, 0, %d, D3DPOOL_MANAGED)\n", dwWidth,
				dwMipMapLevels, PCFormat);

			hRet = g_pD3DDevice->CreateCubeTexture(dwWidth, dwMipMapLevels, D3DUsage,
				PCFormat, D3DPool, &pNewHostCubeTexture
#ifdef CXBX_USE_D3D9
				, nullptr
#endif
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
			DbgPrintf("CreateHostResource : Successfully created %s (0x%.08X, 0x%.08X)\n",
				ResourceTypeName, pResource, pNewHostCubeTexture);
			break;
		}
		} // switch XboxResourceType

		DWORD D3DLockFlags = D3DLOCK_NOSYSLOCK; // Note : D3DLOCK_DISCARD is only valid for D3DUSAGE_DYNAMIC
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
					EmuWarning("Locking host %s failed!", ResourceTypeName);
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
					DbgPrintf("Unsupported texture format, expanding to D3DFMT_A8R8G8B8");

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
					EmuWarning("Unlocking host %s failed!", ResourceTypeName);
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
		DWORD dwSize = g_VMManager.QuerySize(VirtualAddr);
		if (dwSize == 0) {
			// TODO: once this is known to be working, remove the warning
			EmuWarning("Push buffer allocation size unknown");
			pPushBuffer->Lock = X_D3DRESOURCE_LOCK_FLAG_NOSIZE;
			break;
		}

		DbgPrintf("CreateHostResource : Successfully created %S (0x%.08X, 0x%.08X, 0x%.08X)\n", ResourceTypeName, pResource->Data, pPushBuffer->Size, pPushBuffer->AllocationSize);
		break;
	}

	//case X_D3DRTYPE_PALETTE: { break;	}

	case XTL::X_D3DRTYPE_FIXUP: {
		XTL::X_D3DFixup *pFixup = (XTL::X_D3DFixup*)pResource;

		EmuWarning("X_D3DRTYPE_FIXUP is not yet supported\n"
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
	FUNC_EXPORTS
		LOG_FUNC_ONE_ARG(pThis);

	// Backup the key now, as the Xbox resource may be wiped out by the following release call!
	auto key = GetHostResourceKey(pThis);

	// Call the Xbox version of D3DResource_Release and store the result
	XB_trampoline(ULONG, WINAPI, D3DResource_Release, (X_D3DResource*));

	ULONG uRet = XB_D3DResource_Release(pThis);

	// Was the Xbox resource freed?
	if (uRet == 0) {

		// If this was a cached renter target or depth surface, clear the cache variable too!
		if (pThis == g_pXboxRenderTarget) {
			g_pXboxRenderTarget = nullptr;
		}

		if (pThis == g_pXboxDepthStencil) {
			g_pXboxDepthStencil = nullptr;
		}

		// Also release the host copy (if it exists!)
		FreeHostResource(key); 
	}

    return uRet;
}

// ******************************************************************
// * patch: IDirect3DResource8_IsBusy
// ******************************************************************
BOOL WINAPI XTL::EMUPATCH(D3DResource_IsBusy)
(
    X_D3DResource      *pThis
)
{
	FUNC_EXPORTS

    /* too much output
	LOG_FUNC_ONE_ARG(pThis);
    //*/

    return FALSE;
}

// ******************************************************************
// * patch: D3DDevice_EnableOverlay
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_EnableOverlay)
(
    BOOL Enable
)
{
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Enable);
	g_fYuvEnabled = Enable;

	// We can safely ignore this, as long as we properly handle UpdateOverlay
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
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pSurface)
		LOG_FUNC_ARG(SrcRect)
		LOG_FUNC_ARG(DstRect)
		LOG_FUNC_ARG(EnableColorKey)
		LOG_FUNC_ARG(ColorKey)
		LOG_FUNC_END;

	if (!g_fYuvEnabled) {
		return;
	}

	if (pSurface == NULL) {
		EmuWarning("pSurface == NULL!");
	} else {
		uint08 *pOverlayData = (uint08*)GetDataFromXboxResource(pSurface);
		UINT OverlayWidth, OverlayHeight, OverlayDepth, OverlayRowPitch, OverlaySlicePitch;
		CxbxGetPixelContainerMeasures(
			(XTL::X_D3DPixelContainer *)pSurface,
			0, // dwMipMapLevel
			&OverlayWidth, &OverlayHeight, &OverlayDepth, &OverlayRowPitch, &OverlaySlicePitch);

		RECT EmuSourRect;
		RECT EmuDestRect;

		if (SrcRect != NULL) {
			EmuSourRect = *SrcRect;
		} else {
			SetRect(&EmuSourRect, 0, 0, OverlayWidth, OverlayHeight);
		}

		if (DstRect != NULL) {
			// If there's a destination rectangle given, copy that into our local variable :
			EmuDestRect = *DstRect;
		} else {
			GetClientRect(g_hEmuWindow, &EmuDestRect);
		}

		IDirect3DSurface *pCurrentHostBackBuffer = nullptr;
		HRESULT hRet = g_pD3DDevice->GetBackBuffer(
#ifdef CXBX_USE_D3D9
			0, // iSwapChain
#endif
			0, D3DBACKBUFFER_TYPE_MONO, &pCurrentHostBackBuffer);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetBackBuffer - Unable to get backbuffer surface!");

		// if we obtained the backbuffer, load the YUY2 into the backbuffer
		if (SUCCEEDED(hRet)) {
			// Get backbuffer dimenions; TODO : remember this once, at creation/resize time
			D3DSURFACE_DESC BackBufferDesc;
			pCurrentHostBackBuffer->GetDesc(&BackBufferDesc);

			// Limit the width and height of the output to the backbuffer dimensions.
			// This will (hopefully) prevent exceptions in Blinx - The Time Sweeper
			// (see https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/285)
			{
				// Use our (bounded) copy when bounds exceed :
				if (EmuDestRect.right > (LONG)BackBufferDesc.Width) {
					EmuDestRect.right = (LONG)BackBufferDesc.Width;
					DstRect = &EmuDestRect;
				}

				if (EmuDestRect.bottom > (LONG)BackBufferDesc.Height) {
					EmuDestRect.bottom = (LONG)BackBufferDesc.Height;
					DstRect = &EmuDestRect;
				}
			}

			// Use D3DXLoadSurfaceFromMemory() to do conversion, stretching and filtering
			// avoiding the need for YUY2toARGB() (might become relevant when porting to D3D9 or OpenGL)
			// see https://msdn.microsoft.com/en-us/library/windows/desktop/bb172902(v=vs.85).aspx
			hRet = D3DXLoadSurfaceFromMemory(
				/* pDestSurface = */ pCurrentHostBackBuffer,
				/* pDestPalette = */ nullptr, // Palette not needed for YUY2
				/* pDestRect = */DstRect, // Either the unmodified original (can be NULL) or a pointer to our local variable
				/* pSrcMemory = */ pOverlayData, // Source buffer
				/* SrcFormat = */ D3DFMT_YUY2,
				/* SrcPitch = */ OverlayRowPitch,
				/* pSrcPalette = */ nullptr, // Palette not needed for YUY2
				/* SrcRect = */ &EmuSourRect,
				/* Filter = */ D3DX_FILTER_POINT, // Dxbx note : D3DX_FILTER_LINEAR gives a smoother image, but 'bleeds' across borders
				/* ColorKey = */ EnableColorKey ? ColorKey : 0);
			DEBUG_D3DRESULT(hRet, "D3DXLoadSurfaceFromMemory - UpdateOverlay could not convert buffer!\n");

			pCurrentHostBackBuffer->Release();
		}

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(pCallback);

    g_pVBCallback = pCallback;    
}

// LTCG specific D3DDevice_SetTextureState_TexCoordIndex function...
// This uses a custom calling convention where parameter is passed in ESI
// Test-case: Metal Wolf Chaos
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetTextureState_TexCoordIndex_4)
(
    DWORD Value
)
{
	FUNC_EXPORTS

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
		EmuWarning("EmuD3DDevice_SetTextureState_TexCoordIndex: Unknown TexCoordIndex Value (0x%.08X)", Value);
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
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Stage)
		LOG_FUNC_ARG(Value)
		LOG_FUNC_END;

	// TODO: Xbox Direct3D supports sphere mapping OpenGL style.

	// BUG FIX: The lower 16 bits were causing false Unknown TexCoordIndex errors.
	// Check for 0x00040000 instead.

	if (Value >= 0x00040000) {
		EmuWarning("EmuD3DDevice_SetTextureState_TexCoordIndex: Unknown TexCoordIndex Value (0x%.08X)", Value);
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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

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
	FUNC_EXPORTS

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
}

// LTCG specific D3DDevice_SetTextureState_BorderColor function...
// This uses a custom calling convention where parameter is passed in EAX, EBX
// Test-case: Metal Wolf Chaos
VOID XTL::EMUPATCH(D3DDevice_SetTextureState_BorderColor_0)
(
)
{
	FUNC_EXPORTS;

	DWORD Stage;
	DWORD Value;

	__asm {
		mov Stage, eax
		mov Value, ebx
	}

	return EMUPATCH(D3DDevice_SetTextureState_BorderColor)(Stage, Value);
}

// LTCG specific D3DDevice_SetTextureState_BorderColor function...
// This uses a custom calling convention where parameter is passed in EAX
// Test-case: Murakumo
VOID __stdcall XTL::EMUPATCH(D3DDevice_SetTextureState_BorderColor_4)
(
)
{
	FUNC_EXPORTS;

	static uint32 returnAddr;

#ifdef _DEBUG_TRACE
		__asm add esp, 4
#endif

	__asm {
		pop returnAddr
		push eax
		call EmuPatch_D3DDevice_SetTextureState_BorderColor
		mov eax, 0
		push returnAddr
		ret
	}
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
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Stage)
		LOG_FUNC_ARG(Value)
		LOG_FUNC_END;

    HRESULT hRet;
#ifdef CXBX_USE_D3D9
	hRet = g_pD3DDevice->SetSamplerState(Stage, D3DSAMP_BORDERCOLOR, Value);
#else
    hRet = g_pD3DDevice->SetTextureStageState(Stage, D3DTSS_BORDERCOLOR, Value);
#endif
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetTextureStageState");
}

// LTCG specific D3DDevice_SetTextureState_ColorKeyColor function...
// This uses a custom calling convention where parameter is passed in ESI, EBX
// Test-case: Metal Wolf Chaos
VOID XTL::EMUPATCH(D3DDevice_SetTextureState_ColorKeyColor_0)
(
)
{
	FUNC_EXPORTS;

	DWORD Stage;
	DWORD Value;

	__asm {
		mov Stage, esi
		mov Value, ebx
	}

	return EMUPATCH(D3DDevice_SetTextureState_ColorKeyColor)(Stage, Value);
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
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Stage)
		LOG_FUNC_ARG(Value)
		LOG_FUNC_END;

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
	FUNC_EXPORTS;

	DWORD           Stage;
	__asm mov Stage, eax;

	//LOG_FUNC_BEGIN
	//	LOG_FUNC_ARG(Stage)
	//	LOG_FUNC_ARG(Type)
	//	LOG_FUNC_ARG(Value)
	//	LOG_FUNC_END;
	DbgPrintf("D3DDevice_SetTextureState_BumpEnv_8(Stage : %d Type : %d Value : %d);\n", Stage, Type, Value);

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
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Stage)
		LOG_FUNC_ARG(Type)
		LOG_FUNC_ARG(Value)
		LOG_FUNC_END;

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

	HRESULT hRet;
#ifdef CXBX_USE_D3D9
	FLOAT Biased = static_cast<FLOAT>(Value) * -0.000005f;
	Value = *reinterpret_cast<const DWORD *>(&Biased);
	hRet = g_pD3DDevice->SetRenderState(D3DRS_DEPTHBIAS, Value);
#else
    hRet = g_pD3DDevice->SetRenderState(D3DRS_ZBIAS, Value);
#endif
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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

//  TODO: Analyze performance and compatibility (undefined behavior on PC with triangles or points)
//  HRESULT hRet = g_pD3DDevice->SetRenderState(D3DRS_EDGEANTIALIAS, Value);
//	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetRenderState");

    LOG_UNIMPLEMENTED();	
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_FillMode
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_FillMode)
(
    DWORD Value
)
{
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

    DWORD dwFillMode;

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

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
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Method)
		LOG_FUNC_ARG(Value)
		LOG_FUNC_END;

    int State = -1;

    // Todo: make this faster and more elegant
    for(int v=0;v<174;v++)
    {
        if(EmuD3DRenderStateSimpleEncoded[v] == Method)
        {
            State = v;
            break;
        }
    }

	// Special Case: Handle PixelShader related Render States
	// TODO: Port over EmuMappedD3DRenderState and related code from Dxbx or Wip_LessVertexPatching
	// After this, we don't need to do this part anymore
	switch (Method & 0x00001FFC) {
		case /*0x00000a60*/NV2A_RC_CONSTANT_COLOR0(0): TemporaryPixelShaderConstants[X_D3DRS_PSCONSTANT0_0] = Value; return;
		case /*0x00000a64*/NV2A_RC_CONSTANT_COLOR0(1): TemporaryPixelShaderConstants[X_D3DRS_PSCONSTANT0_1] = Value; return;
		case /*0x00000a68*/NV2A_RC_CONSTANT_COLOR0(2): TemporaryPixelShaderConstants[X_D3DRS_PSCONSTANT0_2] = Value; return;
		case /*0x00000a6c*/NV2A_RC_CONSTANT_COLOR0(3): TemporaryPixelShaderConstants[X_D3DRS_PSCONSTANT0_3] = Value; return;
		case /*0x00000a70*/NV2A_RC_CONSTANT_COLOR0(4): TemporaryPixelShaderConstants[X_D3DRS_PSCONSTANT0_4] = Value; return;
		case /*0x00000a74*/NV2A_RC_CONSTANT_COLOR0(5): TemporaryPixelShaderConstants[X_D3DRS_PSCONSTANT0_5] = Value; return;
		case /*0x00000a78*/NV2A_RC_CONSTANT_COLOR0(6): TemporaryPixelShaderConstants[X_D3DRS_PSCONSTANT0_6] = Value; return;
		case /*0x00000a7c*/NV2A_RC_CONSTANT_COLOR0(7): TemporaryPixelShaderConstants[X_D3DRS_PSCONSTANT0_7] = Value; return;
		case /*0x00000a80*/NV2A_RC_CONSTANT_COLOR1(0): TemporaryPixelShaderConstants[X_D3DRS_PSCONSTANT1_0] = Value; return;
		case /*0x00000a84*/NV2A_RC_CONSTANT_COLOR1(1): TemporaryPixelShaderConstants[X_D3DRS_PSCONSTANT1_1] = Value; return;
		case /*0x00000a88*/NV2A_RC_CONSTANT_COLOR1(2): TemporaryPixelShaderConstants[X_D3DRS_PSCONSTANT1_2] = Value; return;
		case /*0x00000a8c*/NV2A_RC_CONSTANT_COLOR1(3): TemporaryPixelShaderConstants[X_D3DRS_PSCONSTANT1_3] = Value; return;
		case /*0x00000a90*/NV2A_RC_CONSTANT_COLOR1(4): TemporaryPixelShaderConstants[X_D3DRS_PSCONSTANT1_4] = Value; return;
		case /*0x00000a94*/NV2A_RC_CONSTANT_COLOR1(5): TemporaryPixelShaderConstants[X_D3DRS_PSCONSTANT1_5] = Value; return;
		case /*0x00000a98*/NV2A_RC_CONSTANT_COLOR1(6): TemporaryPixelShaderConstants[X_D3DRS_PSCONSTANT1_6] = Value; return;
		case /*0x00000a9c*/NV2A_RC_CONSTANT_COLOR1(7): TemporaryPixelShaderConstants[X_D3DRS_PSCONSTANT1_7] = Value; return;
		case /*0x00001e20*/NV2A_RC_COLOR0: TemporaryPixelShaderConstants[X_D3DRS_PSFINALCOMBINERCONSTANT0] = Value; return;
		case /*0x00001e24*/NV2A_RC_COLOR1: TemporaryPixelShaderConstants[X_D3DRS_PSFINALCOMBINERCONSTANT1] = Value; return;
	}

    if(State == -1)
        EmuWarning("RenderState_Simple(0x%.08X, 0x%.08X) is unsupported!", Method, Value);
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

                DbgPrintf("D3DRS_COLORWRITEENABLE := 0x%.08X\n", Value);
            }
            break;

            case D3DRS_SHADEMODE:
                Value = EmuXB2PC_D3DSHADEMODE(Value);
                DbgPrintf("D3DRS_SHADEMODE := 0x%.08X\n", Value);
                break;

            case D3DRS_BLENDOP:
                Value = EmuXB2PC_D3DBLENDOP(Value);
                DbgPrintf("D3DRS_BLENDOP := 0x%.08X\n", Value);
                break;

            case D3DRS_SRCBLEND:
                Value = EmuXB2PC_D3DBLEND(Value);
                DbgPrintf("D3DRS_SRCBLEND := 0x%.08X\n", Value);
                break;

            case D3DRS_DESTBLEND:
                Value = EmuXB2PC_D3DBLEND(Value);
                DbgPrintf("D3DRS_DESTBLEND := 0x%.08X\n", Value);
                break;

            case D3DRS_ZFUNC:
                Value = EmuXB2PC_D3DCMPFUNC(Value);
                DbgPrintf("D3DRS_ZFUNC := 0x%.08X\n", Value);
                break;

            case D3DRS_ALPHAFUNC:
                Value = EmuXB2PC_D3DCMPFUNC(Value);
                DbgPrintf("D3DRS_ALPHAFUNC := 0x%.08X\n", Value);
                break;

            case D3DRS_ALPHATESTENABLE:
                DbgPrintf("D3DRS_ALPHATESTENABLE := 0x%.08X\n", Value);
                break;

            case D3DRS_ALPHABLENDENABLE:
                DbgPrintf("D3DRS_ALPHABLENDENABLE := 0x%.08X\n", Value);
                break;

            case D3DRS_ALPHAREF:
                DbgPrintf("D3DRS_ALPHAREF := %lf\n", DWtoF(Value));
                break;

            case D3DRS_ZWRITEENABLE:
                DbgPrintf("D3DRS_ZWRITEENABLE := 0x%.08X\n", Value);
                break;

            case D3DRS_DITHERENABLE:
                DbgPrintf("D3DRS_DITHERENABLE := 0x%.08X\n", Value);
                break;

			case D3DRS_STENCILZFAIL:
				Value = EmuXB2PC_D3DSTENCILOP(Value);
				DbgPrintf("D3DRS_STENCILZFAIL := 0x%.08X\n", Value);
				break;

			case D3DRS_STENCILPASS:
				Value = EmuXB2PC_D3DSTENCILOP(Value);
				DbgPrintf("D3DRS_STENCILPASS := 0x%.08X\n", Value);
				break;

			case D3DRS_STENCILFUNC:
				Value = EmuXB2PC_D3DCMPFUNC(Value);
				DbgPrintf("D3DRS_STENCILFUNC := 0x%.08X\n", Value);
				break;

			case D3DRS_STENCILREF:
				DbgPrintf("D3DRS_STENCILREF := 0x%.08X\n", Value);
				break;

			case D3DRS_STENCILMASK:
				DbgPrintf("D3DRS_STENCILMASK := 0x%.08X\n", Value);
				break;

			case D3DRS_STENCILWRITEMASK:
				DbgPrintf("D3DRS_STENCILWRITEMASK := 0x%.08X\n", Value);
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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

    // convert from Xbox direct3d to PC direct3d enumeration
    if(Value <= 1)
        Value = Value;
    else if(Value == 3)
        Value = 2;
    else if(Value == 5)
        Value = 3;
    else
        CxbxKrnlCleanup("Unsupported D3DVERTEXBLENDFLAGS (%d)", Value);

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

    // TODO: do something..

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_CullMode
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_CullMode)
(
    DWORD Value
)
{
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Value);

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

// ******************************************************************
// * patch: D3DDevice_SetRenderState_YuvEnable
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_YuvEnable)
(
    BOOL Enable
)
{
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Enable);

    // HACK: Display YUV surface by using an overlay.
    if(Enable != g_fYuvEnabled)
    {
        g_fYuvEnabled = Enable;

        EmuWarning("EmuD3DDevice_SetRenderState_YuvEnable using overlay!");
        
		EMUPATCH(D3DDevice_EnableOverlay)(g_fYuvEnabled);
    }
}

// LTCG specific D3DDevice_SetTransform function...
// This uses a custom calling convention where parameter is passed in EAX, EDX
VOID __stdcall XTL::EMUPATCH(D3DDevice_SetTransform_0)
(
)
{
	FUNC_EXPORTS;

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	DbgPrintf("D3DDevice_SetStreamSource_4(StreamNumber : %08X pStreamData : %08X Stride : %08X);\n", StreamNumber, pStreamData, Stride);

	// Forward to Xbox implementation
	// This should stop us having to patch GetStreamSource!
	//XB_trampoline(VOID, WINAPI, D3DDevice_SetStreamSource_4, (UINT, X_D3DVertexBuffer*, UINT));
	//XB_D3DDevice_SetStreamSource_4(StreamNumber, pStreamData, Stride);

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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

#ifdef CXBX_USE_D3D9
        g_pD3DDevice->SetVertexShaderConstantF(58, vScale, 1);
        g_pD3DDevice->SetVertexShaderConstantF(59, vOffset, 1);
#else
        g_pD3DDevice->SetVertexShaderConstant(58, vScale, 1);
        g_pD3DDevice->SetVertexShaderConstant(59, vOffset, 1);
#endif
    }

    DWORD RealHandle;
    if(VshHandleIsVertexShader(Handle))
    {
        RealHandle = ((VERTEX_SHADER *)(VshHandleGetVertexShader(Handle))->Handle)->Handle;
    }
    else
    {
        RealHandle = Handle;
		RealHandle &= ~D3DFVF_XYZ;
		RealHandle &= ~D3DFVF_XYZRHW;
		RealHandle &= ~D3DFVF_XYZB1;
		RealHandle &= ~D3DFVF_XYZB2;
		RealHandle &= ~D3DFVF_XYZB3;
		RealHandle &= ~D3DFVF_XYZB4;
		RealHandle &= ~D3DFVF_DIFFUSE;
		RealHandle &= ~D3DFVF_NORMAL;
		RealHandle &= ~D3DFVF_SPECULAR;
		RealHandle &= 0x00FF;
		if( RealHandle != 0 )
			EmuWarning("EmuD3DDevice_SetVertexShader Handle = 0x%.08X", RealHandle );
		RealHandle = Handle;
    }

#ifdef CXBX_USE_D3D9
	hRet = g_pD3DDevice->SetVertexShader(nullptr);
	hRet = g_pD3DDevice->SetFVF(RealHandle);
#else
	hRet = g_pD3DDevice->SetVertexShader(RealHandle);
#endif
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexShader");    
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

void CxbxUpdateNativeD3DResources()
{
	EmuUpdateActiveTextureStages();

	// If Pixel Shaders are not disabled, process them
	if (!g_DisablePixelShaders) {
		XTL::DxbxUpdateActivePixelShader();
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

// LTCG specific D3DDevice_SetPixelShader function...
// This uses a custom calling convention where parameter is passed in EAX
// TODO: XB_trampoline plus Log function is not working due lost parameter in EAX.
// Test-case: Metal Wolf Chaos
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetPixelShader_0)
(
)
{
	FUNC_EXPORTS;

	DWORD           Handle;
	__asm mov Handle, eax;

	//LOG_FUNC_ONE_ARG(Handle);

	DbgPrintf("D3DDevice_SetPixelShader_0(Handle : %d);\n", Handle);

	// Call the Xbox function to make sure D3D structures get set
	//XB_trampoline(VOID, WINAPI, D3DDevice_SetPixelShader_0, ());
	//XB_D3DDevice_SetPixelShader_0();

	// Update the global pixel shader
	g_D3DActivePixelShader = (X_PixelShader*)Handle;
}

// ******************************************************************
// * patch: D3DDevice_SetPixelShader
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetPixelShader)
(
	DWORD           Handle
)
{
	FUNC_EXPORTS
	LOG_FUNC_ONE_ARG(Handle);

	// Call the Xbox function to make sure D3D structures get set
	XB_trampoline(VOID, WINAPI, D3DDevice_SetPixelShader, (DWORD));
	XB_D3DDevice_SetPixelShader(Handle);

	// Update the global pixel shader
	g_D3DActivePixelShader = (X_PixelShader*)Handle;
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
	FUNC_EXPORTS

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

    if(IsValidCurrentShader()) {
		VertexPatchDesc VPDesc;

		VPDesc.XboxPrimitiveType = PrimitiveType;
		VPDesc.dwVertexCount = VertexCount;
		VPDesc.dwStartVertex = StartVertex;
		VPDesc.pXboxVertexStreamZeroData = 0;
		VPDesc.uiXboxVertexStreamZeroStride = 0;
		VPDesc.hVertexShader = g_CurrentXboxVertexShaderHandle;

		VertexPatcher VertPatch;

		bool bPatched = VertPatch.Apply(&VPDesc, NULL);

        #ifdef _DEBUG_TRACK_VB
        if(!g_bVBSkipStream) {
        #endif

        HRESULT hRet = g_pD3DDevice->DrawPrimitive
        (
            EmuXB2PC_D3DPrimitiveType(VPDesc.XboxPrimitiveType),
            StartVertex,
            VPDesc.dwHostPrimitiveCount
        );
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawPrimitive");

		g_dwPrimPerFrame += VPDesc.dwHostPrimitiveCount;

        #ifdef _DEBUG_TRACK_VB
        }
        #endif
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
	FUNC_EXPORTS

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
		VertexPatchDesc VPDesc;

		VPDesc.XboxPrimitiveType = PrimitiveType;
		VPDesc.dwVertexCount = VertexCount;
		VPDesc.dwStartVertex = 0;
		VPDesc.pXboxVertexStreamZeroData = pVertexStreamZeroData;
		VPDesc.uiXboxVertexStreamZeroStride = VertexStreamZeroStride;
		VPDesc.hVertexShader = g_CurrentXboxVertexShaderHandle;

		VertexPatcher VertPatch;

		bool bPatched = VertPatch.Apply(&VPDesc, NULL);

        #ifdef _DEBUG_TRACK_VB
        if(!g_bVBSkipStream)
        #endif
        {
			HRESULT hRet = g_pD3DDevice->DrawPrimitiveUP
			(
				EmuXB2PC_D3DPrimitiveType(VPDesc.XboxPrimitiveType),
				VPDesc.dwHostPrimitiveCount,
				VPDesc.pXboxVertexStreamZeroData,
				VPDesc.uiXboxVertexStreamZeroStride
			);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawPrimitiveUP");

			g_dwPrimPerFrame += VPDesc.dwHostPrimitiveCount;
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

#define VERTICES_PER_QUAD 4
#define TRIANGLES_PER_QUAD 2

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
	FUNC_EXPORTS

	// Note : In gamepad.xbe, the gamepad is drawn by D3DDevice_DrawIndexedVertices

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(PrimitiveType)
		LOG_FUNC_ARG(VertexCount)
		LOG_FUNC_ARG(pIndexData)
		LOG_FUNC_END;

	// Dxbx Note : In DrawVertices and DrawIndexedVertices, PrimitiveType may not be D3DPT_POLYGON

	if (!EmuD3DValidVertexCount(PrimitiveType, VertexCount)) {
		LOG_TEST_CASE("Invalid VertexCount");
		return;
	}

	// TODO : Call unpatched D3DDevice_SetStateVB(0);

	CxbxUpdateNativeD3DResources();
	if (IsValidCurrentShader()) {
		DWORD indexBase = 0;
		CxbxUpdateActiveIndexBuffer(pIndexData, &indexBase, VertexCount);

		VertexPatchDesc VPDesc;
		VPDesc.XboxPrimitiveType = PrimitiveType;
		VPDesc.dwVertexCount = VertexCount;
		VPDesc.dwStartVertex = 0;
		VPDesc.pXboxVertexStreamZeroData = 0;
		VPDesc.uiXboxVertexStreamZeroStride = 0;
		VPDesc.hVertexShader = g_CurrentXboxVertexShaderHandle;
		VPDesc.pIndexData = pIndexData;
		VPDesc.dwIndexBase = indexBase;

		VertexPatcher VertPatch;
		bool FatalError = false;
		VertPatch.Apply(&VPDesc, &FatalError);

		UINT uiStartIndex = 0;
		UINT uiNumVertices = VertexCount;

		if (!FatalError)
		{
			VertexCount = VPDesc.dwVertexCount; // Dxbx addition : Use the new VertexCount

			if (VPDesc.XboxPrimitiveType == X_D3DPT_QUADLIST)
			{
				// Indexed quadlist can be drawn using unpatched indexes via multiple draws of 2 'strip' triangles :
				// 4 vertices are just enough for two triangles (a fan starts with 3 vertices for 1 triangle,
				// and adds 1 triangle via 1 additional vertex)
				// This is slower (because of call-overhead) but doesn't require any index buffer patching at all!
				// Note : XDK samples reaching this case are : DisplacementMap, Ripple
				while ((int)VertexCount >= VERTICES_PER_QUAD)
				{
					HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitive
					(
						D3DPT_TRIANGLEFAN, // Draw a triangle-fan instead of a quad
#ifdef CXBX_USE_D3D9
						/* BaseVertexIndex = */0, // TODO : Use g_CachedIndexBase here too? Or transformed somehow?
#endif
						/* MinVertexIndex = */0,
						/* NumVertices = */VERTICES_PER_QUAD, // Use all 4 vertices of 1 quad
						uiStartIndex,
						/* primCount = */TRIANGLES_PER_QUAD // Draw 2 triangles with that
					);
					DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitive(X_D3DPT_QUADLIST)");

					uiStartIndex += VERTICES_PER_QUAD;
					VertexCount -= VERTICES_PER_QUAD;
				}
			}
			else
			{
				HRESULT hRet;
				// Other primitives than X_D3DPT_QUADLIST can be drawn normally :
				hRet = g_pD3DDevice->DrawIndexedPrimitive(
					EmuXB2PC_D3DPrimitiveType(VPDesc.XboxPrimitiveType),
#ifdef CXBX_USE_D3D9
					g_CachedIndexBase,
#endif
					/* MinVertexIndex = */0,
					/* NumVertices = */uiNumVertices, // TODO : g_EmuD3DActiveStreamSizes[0], // Note : ATI drivers are especially picky about this -
					// NumVertices should be the span of covered vertices in the active vertex buffer (TODO : Is stream 0 correct?)
					uiStartIndex,
					VPDesc.dwHostPrimitiveCount);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitive");

				if (VPDesc.XboxPrimitiveType == X_D3DPT_LINELOOP)
				{
					EmuWarning("Unsupported PrimitiveType! (%d)", (DWORD)PrimitiveType);
					//CxbxKrnlCleanup("XTL_EmuD3DDevice_DrawIndexedVertices : X_D3DPT_LINELOOP not unsupported yet!");
					// TODO : Close line-loops using a final single line, drawn from the end to the start vertex
				}
			}

			g_dwPrimPerFrame += VPDesc.dwHostPrimitiveCount;
		}

#ifdef CXBX_USE_D3D9
		g_pD3DDevice->SetIndices(nullptr);
#else
		g_pD3DDevice->SetIndices(NULL, 0);
#endif
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
	FUNC_EXPORTS

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

    if( (PrimitiveType == X_D3DPT_LINELOOP) || (PrimitiveType == X_D3DPT_QUADLIST) )
        EmuWarning("Unsupported PrimitiveType! (%d)", (DWORD)PrimitiveType);

    if (IsValidCurrentShader()) {
		VertexPatchDesc VPDesc;

		VPDesc.XboxPrimitiveType = PrimitiveType;
		VPDesc.dwVertexCount = VertexCount;
		VPDesc.dwStartVertex = 0;
		VPDesc.pXboxVertexStreamZeroData = pVertexStreamZeroData;
		VPDesc.uiXboxVertexStreamZeroStride = VertexStreamZeroStride;
		VPDesc.hVertexShader = g_CurrentXboxVertexShaderHandle;
		VPDesc.pIndexData = (PWORD)pIndexData;

		VertexPatcher VertPatch;

		bool bPatched = VertPatch.Apply(&VPDesc, NULL);

		#ifdef _DEBUG_TRACK_VB
		if(!g_bVBSkipStream) {
		#endif

        HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitiveUP
        (
            EmuXB2PC_D3DPrimitiveType(VPDesc.XboxPrimitiveType), 0, VPDesc.dwVertexCount, VPDesc.dwHostPrimitiveCount, pIndexData, D3DFMT_INDEX16, VPDesc.pXboxVertexStreamZeroData, VPDesc.uiXboxVertexStreamZeroStride
        );
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitiveUP");

		g_dwPrimPerFrame += VPDesc.dwHostPrimitiveCount;

		#ifdef _DEBUG_TRACK_VB
		}
		#endif
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
// * patch: D3DDevice_GetLight
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetLight)
(
    DWORD            Index,
    X_D3DLIGHT8     *pLight
)
{
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Index)
		LOG_FUNC_ARG(pLight)
		LOG_FUNC_END;

    HRESULT hRet = g_pD3DDevice->GetLight(Index, pLight);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetLight");    
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
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Index)
		LOG_FUNC_ARG(pLight)
		LOG_FUNC_END;

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Index)
		LOG_FUNC_ARG(bEnable)
		LOG_FUNC_END;

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
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pRenderTarget)
		LOG_FUNC_ARG(pNewZStencil)
		LOG_FUNC_END;

	IDirect3DSurface *pHostRenderTarget = nullptr;
	IDirect3DSurface *pHostDepthStencil = nullptr;

	// The current render target is only replaced if it's passed in here non-null
    if (pRenderTarget != NULL) {
		g_pXboxRenderTarget = pRenderTarget;
		pHostRenderTarget = GetHostSurface(g_pXboxRenderTarget);
    }

	// The currenct depth stencil is always replaced by whats passed in here (even a null)
	g_pXboxDepthStencil = pNewZStencil;
    pHostDepthStencil = GetHostSurface(g_pXboxDepthStencil);

    HRESULT hRet = g_pD3DDevice->SetRenderTarget(pHostRenderTarget, pHostDepthStencil);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetRenderTarget");
}

// LTCG specific D3DDevice_SetPalette function...
// This uses a custom calling convention where parameter is passed in EAX
// Test-case: Ninja Gaiden
VOID __stdcall XTL::EMUPATCH(D3DDevice_SetPalette_4)
(
)
{
	FUNC_EXPORTS;

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS
	
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
	FUNC_EXPORTS

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
	FUNC_EXPORTS;

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Handle)
		LOG_FUNC_ARG(pSize)
		LOG_FUNC_END;

	// Handle is always address of an Xbox VertexShader struct, or-ed with 1 (X_D3DFVF_RESERVED0)

    if(pSize  && VshHandleIsVertexShader(Handle))
    {
        X_D3DVertexShader *pD3DVertexShader = (X_D3DVertexShader *)(Handle & 0x7FFFFFFF);
        VERTEX_SHADER *pVertexShader = (VERTEX_SHADER *)pD3DVertexShader->Handle;
        *pSize = pVertexShader->Size;
    }
    else if(pSize)
    {
        *pSize = 0;
    }

    
}

// LTCG specific D3DDevice_DeleteVertexShader function...
// This uses a custom calling convention where parameter is passed in EAX
// UNTESTED - Need test-case!
VOID __stdcall XTL::EMUPATCH(D3DDevice_DeleteVertexShader_0)
(
)
{
	FUNC_EXPORTS;

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(Handle);

	// Handle is always address of an Xbox VertexShader struct, or-ed with 1 (X_D3DFVF_RESERVED0)
	// It's reference count is lowered. If it reaches zero (0), the struct is freed.

	DWORD RealHandle = 0;

    if(VshHandleIsVertexShader(Handle))
    {
        X_D3DVertexShader *pD3DVertexShader = (X_D3DVertexShader *)(Handle & 0x7FFFFFFF);
        VERTEX_SHADER *pVertexShader = (VERTEX_SHADER *)pD3DVertexShader->Handle;

        RealHandle = pVertexShader->Handle;
		g_VMManager.Deallocate((VAddr)pVertexShader->pDeclaration);

        if(pVertexShader->pFunction)
        {
			g_VMManager.Deallocate((VAddr)pVertexShader->pFunction);
        }

        FreeVertexDynamicPatch(pVertexShader);

		g_VMManager.Deallocate((VAddr)pVertexShader);
        g_VMManager.Deallocate((VAddr)pD3DVertexShader);
    }

    HRESULT hRet = g_pD3DDevice->DeleteVertexShader(RealHandle);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DeleteVertexShader");    
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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Register)
		LOG_FUNC_ARG(pConstantData)
		LOG_FUNC_ARG(ConstantCount)
		LOG_FUNC_END;

    HRESULT hRet = g_pD3DDevice->GetVertexShaderConstant
    (
        Register + 96,
        pConstantData,
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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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

	if( VshHandleIsVertexShader(hCurrentShader) )
    {
		DWORD hNewShader = 0;
        X_D3DVertexShader *pD3DVertexShader = (X_D3DVertexShader *)(hCurrentShader & 0x7FFFFFFF);
        VERTEX_SHADER *pVertexShader = (VERTEX_SHADER *)pD3DVertexShader->Handle;

		// Save the contents of the existing vertex shader program
		DWORD* pDeclaration = (DWORD*) malloc( pVertexShader->DeclarationSize );
		memmove( pDeclaration, pVertexShader->pDeclaration, pVertexShader->DeclarationSize );

		// Create a new vertex shader with the new 
		HRESULT hr = EMUPATCH(D3DDevice_CreateVertexShader)( pDeclaration, pFunction, &hNewShader, 0 );
		free(pDeclaration);
		if( FAILED( hr ) )
			CxbxKrnlCleanup( "Error creating new vertex shader!" );

		EMUPATCH(D3DDevice_LoadVertexShader)(hNewShader, Address);
		EMUPATCH(D3DDevice_SelectVertexShader)(hNewShader, Address);

		g_LoadVertexShaderProgramCache[shaderCacheKey] = hNewShader;

		EmuWarning("Vertex Shader Cache Size: %d", g_LoadVertexShaderProgramCache.size());
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
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Handle)
		LOG_FUNC_ARG(pType)
		LOG_FUNC_END;

	// Handle is always address of an Xbox VertexShader struct, or-ed with 1 (X_D3DFVF_RESERVED0)
	// *pType is set according to flags in the VertexShader struct

	if(pType && VshHandleIsVertexShader(Handle))
    {
        *pType = ((VERTEX_SHADER *)(VshHandleGetVertexShader(Handle))->Handle)->Type;
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
	FUNC_EXPORTS

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

    if(pSizeOfData && VshHandleIsVertexShader(Handle))
    {
        VERTEX_SHADER *pVertexShader = (VERTEX_SHADER *)(VshHandleGetVertexShader(Handle))->Handle;
        if(*pSizeOfData < pVertexShader->DeclarationSize || !pData)
        {
            *pSizeOfData = pVertexShader->DeclarationSize;

            hRet = !pData ? D3D_OK : D3DERR_MOREDATA;
        }
        else
        {
            memcpy(pData, pVertexShader->pDeclaration, pVertexShader->DeclarationSize);
            hRet = D3D_OK;
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
	FUNC_EXPORTS

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

    if(pSizeOfData && VshHandleIsVertexShader(Handle))
    {
        VERTEX_SHADER *pVertexShader = (VERTEX_SHADER *)(VshHandleGetVertexShader(Handle))->Handle;
        if(*pSizeOfData < pVertexShader->FunctionSize || !pData)
        {
            *pSizeOfData = pVertexShader->FunctionSize;

            hRet = !pData ? D3D_OK : D3DERR_MOREDATA;
        }
        else
        {
            memcpy(pData, pVertexShader->pFunction, pVertexShader->FunctionSize);
            hRet = D3D_OK;
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
	FUNC_EXPORTS

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
            EmuWarning("Unknown SetDepthClipPlanes Flags provided");;
    }

    // TODO

    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_InsertFence
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(D3DDevice_InsertFence)()
{
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(pThis);

    // TODO: Implement
	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * patch: D3DDevice_SetScissors
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetScissors)
(
    DWORD          Count,
    BOOL           Exclusive,
    CONST D3DRECT  *pRects
)
{
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Count)
		LOG_FUNC_ARG(Exclusive)
		LOG_FUNC_ARG(pRects)
		LOG_FUNC_END;

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
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_END;

    EmuWarning("EmuD3DDevice_SetScreenSpaceOffset ignored");
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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(pCallback);

    g_pSwapCallback = pCallback;
}

// ******************************************************************
// * patch: D3DDevice_PersistDisplay
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_PersistDisplay)()
{
	FUNC_EXPORTS

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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(dwSampleAlpha);

	// TODO: Implement?

	LOG_UNIMPLEMENTED();

		

	return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_Deferred
// ******************************************************************
VOID __fastcall XTL::EMUPATCH(D3DDevice_SetRenderState_Deferred)
(
	DWORD State,
	DWORD Value
)
{
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(State)
		LOG_FUNC_ARG(Value)
		LOG_FUNC_END;

	// TODO: HACK: Technically, this function doesn't need to be emulated.
	// The location of EmuD3DDeferredRenderState for 3911 isn't correct and at
	// the time of writing, I don't understand how to fix it.  Until then, 
	// I'm going to implement this in a reckless manner.  When the offset for
	// EmuD3DDeferredRenderState is fixed for 3911, this function should be
	// obsolete!

	if( State > 81 && State < 116 )
		EmuD3DDeferredRenderState[State-82] = Value;
	else
		CxbxKrnlCleanup("Unknown Deferred RenderState! (%d)\n", State);

	/*
	XDK 3911 Deferred RenderState values
	D3DRS_FOGENABLE                 = 82,   // TRUE to enable fog blending 
    D3DRS_FOGTABLEMODE              = 83,   // D3DFOGMODE 
    D3DRS_FOGSTART                  = 84,   // float fog start (for both vertex and pixel fog) 
    D3DRS_FOGEND                    = 85,   // float fog end      
    D3DRS_FOGDENSITY                = 86,   // float fog density  
    D3DRS_RANGEFOGENABLE            = 87,   // TRUE to enable range-based fog 
    D3DRS_WRAP0                     = 88,   // D3DWRAP* flags (D3DWRAP_U, D3DWRAPCOORD_0, etc.) for 1st texture coord.
    D3DRS_WRAP1                     = 89,   // D3DWRAP* flags (D3DWRAP_U, D3DWRAPCOORD_0, etc.) for 2nd texture coord. 
    D3DRS_WRAP2                     = 90,   // D3DWRAP* flags (D3DWRAP_U, D3DWRAPCOORD_0, etc.) for 3rd texture coord. 
    D3DRS_WRAP3                     = 91,   // D3DWRAP* flags (D3DWRAP_U, D3DWRAPCOORD_0, etc.) for 4th texture coord. 
    D3DRS_LIGHTING                  = 92,   // TRUE to enable lighting
    D3DRS_SPECULARENABLE            = 93,   // TRUE to enable specular 
    D3DRS_LOCALVIEWER               = 94,   // TRUE to enable camera-relative specular highlights
    D3DRS_COLORVERTEX               = 95,   // TRUE to enable per-vertex color
    D3DRS_BACKSPECULARMATERIALSOURCE= 96,   // D3DMATERIALCOLORSOURCE (Xbox extension)
    D3DRS_BACKDIFFUSEMATERIALSOURCE = 97,   // D3DMATERIALCOLORSOURCE (Xbox extension)
    D3DRS_BACKAMBIENTMATERIALSOURCE = 98,   // D3DMATERIALCOLORSOURCE (Xbox extension)
    D3DRS_BACKEMISSIVEMATERIALSOURCE= 99,   // D3DMATERIALCOLORSOURCE (Xbox extension)
    D3DRS_SPECULARMATERIALSOURCE    = 100,  // D3DMATERIALCOLORSOURCE 
    D3DRS_DIFFUSEMATERIALSOURCE     = 101,  // D3DMATERIALCOLORSOURCE 
    D3DRS_AMBIENTMATERIALSOURCE     = 102,  // D3DMATERIALCOLORSOURCE 
    D3DRS_EMISSIVEMATERIALSOURCE    = 103,  // D3DMATERIALCOLORSOURCE 
    D3DRS_BACKAMBIENT               = 104,  // D3DCOLOR (Xbox extension)
    D3DRS_AMBIENT                   = 105,  // D3DCOLOR 
    D3DRS_POINTSIZE                 = 106,  // float point size 
    D3DRS_POINTSIZE_MIN             = 107,  // float point size min threshold 
    D3DRS_POINTSPRITEENABLE         = 108,  // TRUE to enable point sprites
    D3DRS_POINTSCALEENABLE          = 109,  // TRUE to enable point size scaling
    D3DRS_POINTSCALE_A              = 110,  // float point attenuation A value 
    D3DRS_POINTSCALE_B              = 111,  // float point attenuation B value 
    D3DRS_POINTSCALE_C              = 112,  // float point attenuation C value 
    D3DRS_POINTSIZE_MAX             = 113,  // float point size max threshold 
    D3DRS_PATCHEDGESTYLE            = 114,  // D3DPATCHEDGESTYLE
    D3DRS_PATCHSEGMENTS             = 115,  // DWORD number of segments per edge when drawing patches
	*/

		
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
	FUNC_EXPORTS

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
	FUNC_EXPORTS

	LOG_FUNC();

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * patch: D3DDevice_GetModelView
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetModelView)(D3DXMATRIX* pModelView)
{
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(pModelView);

	D3DXMATRIX mtxWorld, mtxView;

	// I hope this is right
	g_pD3DDevice->GetTransform( D3DTS_WORLD, &mtxWorld );
	g_pD3DDevice->GetTransform( D3DTS_VIEW, &mtxView );

	*pModelView = mtxWorld * mtxView;

	return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_SetBackMaterial
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetBackMaterial)
(
	X_D3DMATERIAL8* pMaterial
)
{
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(pMaterial);

	LOG_NOT_SUPPORTED();
}

DWORD PushBuffer[64 * 1024 / sizeof(DWORD)];

// ******************************************************************
// * patch: D3D_SetCommonDebugRegisters
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3D_SetCommonDebugRegisters)()
{
	FUNC_EXPORTS

	LOG_FUNC();

	// NOTE: I added this because I was too lazy to deal with emulating certain render
	// states that use it.  

	LOG_UNIMPLEMENTED();

}

// ******************************************************************
// * patch: D3D_BlockOnTime
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3D_BlockOnTime)( DWORD Unknown1, int Unknown2 )
{
	FUNC_EXPORTS

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
// * patch: D3D_BlockOnResource
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3D_BlockOnResource)( X_D3DResource* pResource )
{
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(pResource);

	// TODO: Implement
	// NOTE: Azurik appears to call this directly from numerous points
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
	FUNC_EXPORTS

	LOG_FORWARD("D3DDevice_SetRenderTarget");

	// Redirect to the standard version.
	
	EMUPATCH(D3DDevice_SetRenderTarget)(pRenderTarget, pNewZStencil);
}

// ******************************************************************
// * patch: D3DDevice_GetScissors
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetScissors)
(
	DWORD	*pCount, 
	BOOL	*pExclusive, 
	D3DRECT *pRects
)
{
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pCount)
		LOG_FUNC_ARG(pExclusive)
		LOG_FUNC_ARG(pRects)
		LOG_FUNC_END;

    // TODO: Save a copy of each scissor rect in case this function is called
	// in conjunction with D3DDevice::SetScissors. So far, only Outrun2 uses
	// this function. For now, just return the values within the current
	// viewport.

	D3DVIEWPORT vp;

	HRESULT hRet = g_pD3DDevice->GetViewport( &vp );
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetViewport");

	pRects->x1 = pRects->y1 = 0;
	pRects->x2 = vp.Width;
	pRects->y2 = vp.Height;

	pExclusive[0] = FALSE;
}

// ******************************************************************
// * patch: D3DDevice_GetBackMaterial
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetBackMaterial)
(
	X_D3DMATERIAL8* pMaterial
)
{
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(pMaterial);

	LOG_NOT_SUPPORTED();

	HRESULT hRet = D3D_OK;

	// TODO: HACK: This is wrong, but better than nothing, right?
	if (pMaterial)
	{
		HRESULT hRet = g_pD3DDevice->GetMaterial(pMaterial);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetMaterial");
	}

	if (FAILED(hRet))
	{
		EmuWarning("We're lying about getting a back material!");
		hRet = D3D_OK;
	}
}

// ******************************************************************
// * patch: D3D::LazySetPointParams
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3D_LazySetPointParams)
(
	void* Device
)
{
	FUNC_EXPORTS

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
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(pMaterial);

	HRESULT hRet = D3D_OK;

	if (pMaterial)
	{
		HRESULT hRet = g_pD3DDevice->GetMaterial(pMaterial);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetMaterial");
	}

	if(FAILED(hRet))
    {
		EmuWarning("We're lying about getting a material!");
        hRet = D3D_OK;
    }
}

