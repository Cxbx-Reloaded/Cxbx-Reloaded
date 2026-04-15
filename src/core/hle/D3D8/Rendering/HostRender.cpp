#include "EmuD3D8_common.h"


static HRESULT CxbxSetRenderTarget(IDirect3DSurface* pHostRenderTarget)
{
	LOG_INIT; // Allows use of DEBUG_D3DRESULT

	HRESULT hRet;
#ifdef CXBX_USE_D3D11
	if (pHostRenderTarget == nullptr) {
		// Restore back buffer as render target; track backbuffer surface for dimension queries
		g_pD3DCurrentHostRenderTarget = g_pD3DBackBufferSurface;
		// Release any offscreen RTV before restoring the backbuffer view
		if (g_pD3DCurrentRTV != nullptr && g_pD3DCurrentRTV != g_pD3DBackBufferView) {
			g_pD3DCurrentRTV->Release();
		}
		g_pD3DCurrentRTV = g_pD3DBackBufferView;
		g_pD3DDeviceContext->OMSetRenderTargets(1, &g_pD3DBackBufferView, g_pD3DDepthStencilView);
		hRet = S_OK;
	} else {
		g_pD3DCurrentHostRenderTarget = pHostRenderTarget;
		// Get the texture description to determine the format
		D3D11_TEXTURE2D_DESC textureDesc = {};
		pHostRenderTarget->GetDesc(&textureDesc);

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		ComPtr<ID3D11RenderTargetView> renderTargetView;
		hRet = g_pD3DDevice->CreateRenderTargetView((ID3D11Resource*)pHostRenderTarget, &renderTargetViewDesc, renderTargetView.GetAddressOf());
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateRenderTargetView");

		if (SUCCEEDED(hRet)) {
			// Release the previous offscreen RTV (if any; not the backbuffer view which is owned separately)
			if (g_pD3DCurrentRTV != nullptr && g_pD3DCurrentRTV != g_pD3DBackBufferView) {
				g_pD3DCurrentRTV->Release();
			}
			g_pD3DCurrentRTV = renderTargetView.Get();
			g_pD3DCurrentRTV->AddRef(); // prevent ComPtr from releasing it when it goes out of scope
			g_pD3DDeviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), g_pD3DDepthStencilView);
		}
	}
#else
	hRet = g_pD3DDevice->SetRenderTarget(/*RenderTargetIndex=*/0, pHostRenderTarget);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetRenderTarget");
#endif

	return hRet;
}

static void CxbxD3DClear(
	DWORD          Count,
	CONST D3DRECT* pRects,
	DWORD          Flags,
	D3DCOLOR       Color,
	float          Z,
	DWORD          Stencil
)
{
	LOG_INIT; // Allows use of DEBUG_D3DRESULT

#ifdef CXBX_USE_D3D11
	// Convert D3DCOLOR (ARGB) to normalized RGBA float array for D3D11
	FLOAT clearColor[4];
	clearColor[0] = ((Color >> 16) & 0xFF) / 255.0f; // R
	clearColor[1] = ((Color >>  8) & 0xFF) / 255.0f; // G
	clearColor[2] = ((Color >>  0) & 0xFF) / 255.0f; // B
	clearColor[3] = ((Color >> 24) & 0xFF) / 255.0f; // A

	if ((Flags & D3DCLEAR_TARGET) && g_pD3DCurrentRTV != nullptr) {
		if (Count > 0 && pRects != nullptr) {
			// Use ClearView for rect-based clears (requires D3D11.1 context)
			ComPtr<ID3D11DeviceContext1> context1;
			if (SUCCEEDED(g_pD3DDeviceContext->QueryInterface(IID_PPV_ARGS(context1.GetAddressOf())))) {
				context1->ClearView(g_pD3DCurrentRTV, clearColor, (const D3D11_RECT*)pRects, Count);
			}
		} else {
			g_pD3DDeviceContext->ClearRenderTargetView(g_pD3DCurrentRTV, clearColor);
		}
	}

	if (g_pD3DDepthStencilView != nullptr) {
		UINT clearFlags = 0;
		if (Flags & D3DCLEAR_ZBUFFER) {
			clearFlags |= D3D11_CLEAR_DEPTH;
		}
		if (Flags & D3DCLEAR_STENCIL) {
			clearFlags |= D3D11_CLEAR_STENCIL;
		}
		if (clearFlags != 0) {
			g_pD3DDeviceContext->ClearDepthStencilView(g_pD3DDepthStencilView, clearFlags, Z, (UINT8)Stencil);
		}
	}
#else
	HRESULT hRet = g_pD3DDevice->Clear(Count, pRects, Flags, Color, Z, Stencil);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->Clear");
#endif
}

static void CxbxSetViewport(D3DVIEWPORT *pHostViewport)
{
#ifdef CXBX_USE_D3D11
	g_pD3DDeviceContext->RSSetViewports(1, pHostViewport);
#else
	g_pD3DDevice->SetViewport(pHostViewport);
#endif
}

static void CxbxSetScissorRect(CONST RECT *pHostViewportRect)
{
#ifdef CXBX_USE_D3D11
	g_pD3DDeviceContext->RSSetScissorRects(1, pHostViewportRect);
#else
	g_pD3DDevice->SetScissorRect(pHostViewportRect);
#endif
}

static void CxbxSetIndices(IDirect3DIndexBuffer* pHostIndexBuffer)
{
	LOG_INIT; // Allows use of DEBUG_D3DRESULT

#ifdef CXBX_USE_D3D11
	g_pD3DDeviceContext->IASetIndexBuffer(pHostIndexBuffer, /*Format=*/DXGI_FORMAT_R16_UINT, /*Offset=*/0);
#else
	HRESULT hRet = g_pD3DDevice->SetIndices(pHostIndexBuffer);
	// Note : Under Direct3D 9, the BaseVertexIndex argument is moved towards DrawIndexedPrimitive
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetIndices");

	if (FAILED(hRet))
		CxbxrAbort("CxbxSetIndices: SetIndices Failed!"); // +DxbxD3DErrorString(hRet));
#endif
}

// Lock (Map) an index buffer for writing, returning a pointer to the data.
// Returns nullptr on failure.
static INDEX16* CxbxLockIndexBuffer(IDirect3DIndexBuffer* pHostIndexBuffer)
{
	LOG_INIT;

	INDEX16* pData = nullptr;
#ifdef CXBX_USE_D3D11
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	HRESULT hRet = g_pD3DDeviceContext->Map(pHostIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	DEBUG_D3DRESULT(hRet, "CxbxLockIndexBuffer: Map");
	if (SUCCEEDED(hRet))
		pData = (INDEX16*)mapped.pData;
#else
	HRESULT hRet = pHostIndexBuffer->Lock(0, /*entire SizeToLock=*/0, (D3DLockData**)&pData, D3DLOCK_DISCARD);
	DEBUG_D3DRESULT(hRet, "CxbxLockIndexBuffer: Lock");
#endif
	return pData;
}

// Unlock (Unmap) a previously locked index buffer.
static void CxbxUnlockIndexBuffer(IDirect3DIndexBuffer* pHostIndexBuffer)
{
	LOG_INIT;

#ifdef CXBX_USE_D3D11
	g_pD3DDeviceContext->Unmap(pHostIndexBuffer, 0);
#else
	HRESULT hRet = pHostIndexBuffer->Unlock();
	DEBUG_D3DRESULT(hRet, "CxbxUnlockIndexBuffer: Unlock");
#endif
}

// Draw indexed primitives using the currently bound index/vertex buffers.
static HRESULT CxbxDrawIndexedPrimitive(xbox::X_D3DPRIMITIVETYPE XboxPrimitiveType, UINT IndexCount, INT BaseVertexIndex, UINT StartIndex, UINT MinIndex, UINT NumVertices, UINT PrimCount)
{
	HRESULT hRet;
#ifdef CXBX_USE_D3D11
	g_pD3DDeviceContext->IASetPrimitiveTopology(EmuXB2PC_D3D11PrimitiveTopology(XboxPrimitiveType));
	g_pD3DDeviceContext->DrawIndexed(IndexCount, StartIndex, BaseVertexIndex);
	hRet = S_OK;
#else
	hRet = g_pD3DDevice->DrawIndexedPrimitive(
		EmuXB2PC_D3DPrimitiveType(XboxPrimitiveType),
		BaseVertexIndex,
		MinIndex,
		NumVertices,
		StartIndex,
		PrimCount);
#endif
	return hRet;
}

// Draw non-indexed primitives using the currently bound vertex buffers.
static HRESULT CxbxDrawPrimitive(xbox::X_D3DPRIMITIVETYPE XboxPrimitiveType, UINT VertexCount, UINT StartVertex, UINT PrimCount)
{
	HRESULT hRet;
#ifdef CXBX_USE_D3D11
	g_pD3DDeviceContext->IASetPrimitiveTopology(EmuXB2PC_D3D11PrimitiveTopology(XboxPrimitiveType));
	g_pD3DDeviceContext->Draw(VertexCount, StartVertex);
	hRet = S_OK;
#else
	hRet = g_pD3DDevice->DrawPrimitive(
		EmuXB2PC_D3DPrimitiveType(XboxPrimitiveType),
		StartVertex,
		PrimCount);
#endif
	return hRet;
}

static void CxbxImGui_RenderD3D(ImGuiUI* m_imgui, IDirect3DSurface* renderTarget)
{
#ifdef CXBX_USE_D3D11
	ImGui_ImplDX11_NewFrame();
#else
	ImGui_ImplDX9_NewFrame();
#endif
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	m_imgui->DrawMenu();
	m_imgui->DrawWidgets();

	ImGui::EndFrame();

	ImGui::Render();
	ImDrawData* drawData = ImGui::GetDrawData();
	if (drawData->TotalVtxCount > 0) {
#ifdef CXBX_USE_D3D11
		// For D3D11, render ImGui directly to the current render target
		(void)CxbxSetRenderTarget(renderTarget);
		ImGui_ImplDX11_RenderDrawData(drawData);
		// Restore back buffer
		(void)CxbxSetRenderTarget(nullptr);
#else
		IDirect3DSurface* pExistingRenderTarget = nullptr;
		if (SUCCEEDED(g_pD3DDevice->GetRenderTarget(0, &pExistingRenderTarget))) {
			(void)CxbxSetRenderTarget(renderTarget);
			ImGui_ImplDX9_RenderDrawData(drawData);
			(void)CxbxSetRenderTarget(pExistingRenderTarget);
			pExistingRenderTarget->Release();
		}
#endif
	}
}


/* Unused :
static xbox::dword_xt                  *g_Xbox_D3DDevice; // TODO: This should be a D3DDevice structure
*/

// Blit a host surface region to another host surface region with filtering.
// Works for both D3D9 (StretchRect + D3DXLoadSurfaceFromSurface fallback)
// and D3D11 (CopySubresourceRegion or shader blit).
static HRESULT CxbxBltSurface(
	IDirect3DSurface* pSrc, const RECT* pSrcRect,
	IDirect3DSurface* pDst, const RECT* pDstRect,
	D3DTEXTUREFILTERTYPE Filter)
{
#ifdef CXBX_USE_D3D11
	return CxbxD3D11Blt(pSrc, pSrcRect, pDst, pDstRect, Filter);
#else
	HRESULT hRet = g_pD3DDevice->StretchRect(pSrc, pSrcRect, pDst, pDstRect, Filter);
	if (FAILED(hRet)) {
		// Fallback for cases StretchRect cannot handle (e.g. texture to texture)
		hRet = D3DXLoadSurfaceFromSurface(pDst, nullptr, pDstRect, pSrc, nullptr, pSrcRect, (Filter == D3DTEXF_LINEAR) ? D3DX_FILTER_LINEAR : D3DX_FILTER_POINT, 0);
	}
	return hRet;
#endif
}

// Static Function(s)
static DWORD WINAPI                 EmuRenderWindow(LPVOID);
static LRESULT WINAPI               EmuMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static inline void                  EmuVerifyResourceIsRegistered(xbox::X_D3DResource *pResource, DWORD D3DUsage, int iTextureStage, DWORD dwSize);
HRESULT CxbxPresent()
{
	LOG_INIT;

	CxbxEndScene();

	HRESULT hRet;

#ifdef CXBX_USE_D3D11
	hRet = g_pSwapChain->Present(0, 0);
	DEBUG_D3DRESULT(hRet, "g_pSwapChain->Present");
#else
	hRet = g_pD3DDevice->Present(0, 0, 0, 0);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->Present");
#endif

	// Make sure that the actual Present return result is returned back, not the
	// result of this subsequent BeginScene, using another variable for that :
	CxbxBeginScene();

	return hRet;
}

static void DrawInitialBlackScreen
(
)
{
    // initially, show a black screen
    // Only clear depth buffer and stencil if present
    //
    // Avoids following DirectX Debug Runtime error report
    //    [424] Direct3D8: (ERROR) :Invalid flag D3DCLEAR_ZBUFFER: no zbuffer is associated with device. Clear failed. 
    //

	CxbxD3DClear(
		/*Count=*/0,
		/*pRects=*/nullptr,
		D3DCLEAR_TARGET | (g_bHasDepth ? D3DCLEAR_ZBUFFER : 0) | (g_bHasStencil ? D3DCLEAR_STENCIL : 0),
		/*Color=*/0xFF000000, // TODO : Use constant for this
		/*Z=*/g_bHasDepth ? 1.0f : 0.0f,
		/*Stencil=*/0);

	CxbxBeginScene();

	CxbxPresent();
}

static void CreateDefaultD3D9Device
(
    const xbox::X_D3DPRESENT_PARAMETERS     *pPresentationParameters
)
{
    LOG_INIT;

    // only one device should be created at once
    if (g_pD3DDevice != nullptr) {
        EmuLog(LOG_LEVEL::DEBUG, "CreateDefaultD3D9Device releasing old Device.");

		CxbxEndScene();

        ClearResourceCache(g_Cxbx_Cached_PaletizedTextures);
        ClearResourceCache(g_Cxbx_Cached_Direct3DResources);

        // TODO: ensure all other resources are cleaned up too

        // Final release of IDirect3DDevice9 must be called from the window message thread
        // See https://docs.microsoft.com/en-us/windows/win32/direct3d9/multithreading-issues
        RunOnWndMsgThread([] {
            // We only need to call bundled device release once here.
            g_renderbase->DeviceRelease();
        });
    }

    // Apply render scale factor for high-resolution rendering
    g_RenderUpscaleFactor = g_XBVideo.renderScaleFactor;

    // Setup the HostPresentationParameters
    SetupPresentationParameters(pPresentationParameters);

#ifndef CXBX_USE_D3D11
    // detect vertex processing capabilities
    DWORD BehaviorFlags;
    if((g_D3DCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) && g_EmuCDPD.DeviceType == D3DDEVTYPE_HAL)
    {
        EmuLog(LOG_LEVEL::DEBUG, "Using hardware vertex processing");

        BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }
    else
    {
        EmuLog(LOG_LEVEL::DEBUG, "Using software vertex processing");

        BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }

    // Dxbx addition : Prevent Direct3D from changing the FPU Control word :
    BehaviorFlags |= D3DCREATE_FPU_PRESERVE;

    // Direct3D8: (WARN) :Device that was created without D3DCREATE_MULTITHREADED is being used by a thread other than the creation thread.
    BehaviorFlags |= D3DCREATE_MULTITHREADED;

    // We never want auto-depth stencil on the host, Xbox D3D will handle this for us
    g_EmuCDPD.HostPresentationParameters.EnableAutoDepthStencil = FALSE;

	D3DDISPLAYMODEEX displayMode { sizeof(displayMode) };
	{
		const auto& presentParameters = g_EmuCDPD.HostPresentationParameters;
		displayMode.Width = presentParameters.BackBufferWidth;
		displayMode.Height = presentParameters.BackBufferHeight;
		displayMode.RefreshRate = presentParameters.FullScreen_RefreshRateInHz;
		displayMode.Format = presentParameters.BackBufferFormat;
		displayMode.ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;
	}
#endif
#ifdef CXBX_USE_D3D11
	// This flag adds support for surfaces with a different color channel 
	// ordering than the API default. It is required for compatibility with
	// Direct2D.
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT; // See enum D3D11_CREATE_DEVICE_FLAG
#if defined(_DEBUG)
	// If the project is in a debug build, enable debugging via SDK Layers.
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	// only use feature level 10.0
	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_10_0 };

	// Create the Direct3D 11 API device object and a corresponding context.
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
	HRESULT hr = D3D11CreateDevice(
		g_EmuCDPD.Adapter,
		g_EmuCDPD.DeviceType,
		nullptr,
		creationFlags,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION, // UWP apps must set this to D3D11_SDK_VERSION.
		&device, // Returns the Direct3D device created.
		nullptr, // pFeatureLevel
		&context // Returns the device immediate context.
	);
#if defined(_DEBUG)
	// If debug layer failed (SDK not installed), retry without it
	if (FAILED(hr) && (creationFlags & D3D11_CREATE_DEVICE_DEBUG)) {
		EmuLog(LOG_LEVEL::WARNING, "D3D11CreateDevice failed with debug layer (hr=0x%08X), retrying without", hr);
		creationFlags &= ~D3D11_CREATE_DEVICE_DEBUG;
		hr = D3D11CreateDevice(
			g_EmuCDPD.Adapter,
			g_EmuCDPD.DeviceType,
			nullptr,
			creationFlags,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			&device,
			nullptr,
			&context
		);
	}
#endif
    DEBUG_D3DRESULT(hr, "D3D11CreateDevice");
	if (FAILED(hr))
		CxbxrAbort("D3D11CreateDevice failed (hr=0x%08X)", hr);

	// Store pointers to the Direct3D 11 API device and immediate context.
	device->QueryInterface(__uuidof(ID3D11Device), reinterpret_cast<void**>(&g_pD3DDevice));
	context->QueryInterface(__uuidof(ID3D11DeviceContext), reinterpret_cast<void**>(&g_pD3DDeviceContext));

	// Create a swap chain using the HWND (Win32 window)
	// Get DXGI objects from device
	ComPtr<IDXGIDevice1> dxgiDevice;
	g_pD3DDevice->QueryInterface(__uuidof(IDXGIDevice1), reinterpret_cast<void**>(dxgiDevice.GetAddressOf()));

	ComPtr<IDXGIAdapter> dxgiAdapter;
	dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf());

	ComPtr<IDXGIFactory2> dxgiFactory;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(dxgiFactory.GetAddressOf()));

	// Configure swap chain description for Win32 HWND
	DXGI_SWAP_CHAIN_DESC1 SwapChainDesc = {};
	SwapChainDesc.Width = g_EmuCDPD.HostPresentationParameters.BackBufferWidth;
	SwapChainDesc.Height = g_EmuCDPD.HostPresentationParameters.BackBufferHeight;
	SwapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // Common back buffer format
	SwapChainDesc.Stereo = FALSE;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.BufferCount = 2;
	SwapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	SwapChainDesc.Flags = 0;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc = {};
	fullscreenDesc.RefreshRate.Numerator = g_EmuCDPD.HostPresentationParameters.FullScreen_RefreshRateInHz;
	fullscreenDesc.RefreshRate.Denominator = 1;
	fullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	fullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	fullscreenDesc.Windowed = g_EmuCDPD.HostPresentationParameters.Windowed;

	ComPtr<IDXGISwapChain1> swapChain1;
	hr = dxgiFactory->CreateSwapChainForHwnd(
		g_pD3DDevice,
		g_hEmuWindow,
		&SwapChainDesc,
		fullscreenDesc.Windowed ? nullptr : &fullscreenDesc,
		nullptr, // pRestrictToOutput
		&swapChain1
	);
	DEBUG_D3DRESULT(hr, "IDXGIFactory2::CreateSwapChainForHwnd");
	if (FAILED(hr))
		CxbxrAbort("IDXGIFactory2::CreateSwapChainForHwnd failed");

	swapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&g_pSwapChain));

	// Prevent DXGI from interfering with ALT+ENTER fullscreen toggle
	dxgiFactory->MakeWindowAssociation(g_hEmuWindow, DXGI_MWA_NO_ALT_ENTER);

	dxgiDevice->SetMaximumFrameLatency(1);

	// Configure the back buffer as a render target
	ComPtr<ID3D11Texture2D> backBuffer;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	DEBUG_D3DRESULT(hr, "IDXGISwapChain::GetBuffer");

	// Create a render target view on the back buffer.
	hr = g_pD3DDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &g_pD3DBackBufferView);
	DEBUG_D3DRESULT(hr, "g_pD3DDevice->CreateRenderTargetView");

	// Keep a reference to the back buffer texture for dimension queries
	g_pD3DBackBufferSurface = backBuffer.Get();
	g_pD3DBackBufferSurface->AddRef();

	D3D11_TEXTURE2D_DESC backBufferDesc = {};
	backBuffer->GetDesc(&backBufferDesc);

	// Create a depth/stencil buffer and view to match the back buffer dimensions
	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = backBufferDesc.Width;
	depthDesc.Height = backBufferDesc.Height;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;

	hr = g_pD3DDevice->CreateTexture2D(&depthDesc, nullptr, &g_pD3DDepthStencilBuffer);
	DEBUG_D3DRESULT(hr, "g_pD3DDevice->CreateTexture2D (depth stencil)");

	if (SUCCEEDED(hr)) {
		hr = g_pD3DDevice->CreateDepthStencilView(g_pD3DDepthStencilBuffer, nullptr, &g_pD3DDepthStencilView);
		DEBUG_D3DRESULT(hr, "g_pD3DDevice->CreateDepthStencilView");
	}

	// Bind render target and depth stencil views to the output merger stage
	g_pD3DCurrentRTV = g_pD3DBackBufferView;
	g_pD3DCurrentHostRenderTarget = g_pD3DBackBufferSurface;
	g_pD3DDeviceContext->OMSetRenderTargets(1, &g_pD3DBackBufferView, g_pD3DDepthStencilView);

	// Store back buffer description for later use
	g_HostBackBufferDesc = backBufferDesc;

	// Set up default viewport to match back buffer size
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(backBufferDesc.Width);
	viewport.Height = static_cast<float>(backBufferDesc.Height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	g_pD3DDeviceContext->RSSetViewports(1, &viewport);

	// Initialize default D3D11 rasterizer state desc
	g_D3D11RasterizerDesc.FillMode = D3D11_FILL_SOLID;
	g_D3D11RasterizerDesc.CullMode = D3D11_CULL_BACK;
	g_D3D11RasterizerDesc.FrontCounterClockwise = FALSE;
	g_D3D11RasterizerDesc.DepthBias = 0;
	g_D3D11RasterizerDesc.SlopeScaledDepthBias = 0.0f;
	g_D3D11RasterizerDesc.DepthBiasClamp = 0.0f;
	g_D3D11RasterizerDesc.DepthClipEnable = TRUE;
	g_D3D11RasterizerDesc.ScissorEnable = FALSE;
	g_D3D11RasterizerDesc.MultisampleEnable = FALSE;
	g_D3D11RasterizerDesc.AntialiasedLineEnable = FALSE;

	// Initialize default D3D11 depth stencil state desc (Z test enabled, write enabled)
	g_D3D11DepthStencilDesc.DepthEnable = TRUE;
	g_D3D11DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	g_D3D11DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	g_D3D11DepthStencilDesc.StencilEnable = FALSE;
	g_D3D11DepthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	g_D3D11DepthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	g_D3D11DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	g_D3D11DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	g_D3D11DepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	g_D3D11DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	g_D3D11DepthStencilDesc.BackFace = g_D3D11DepthStencilDesc.FrontFace;

	// Initialize default blend state (no blending)
	g_D3D11BlendDesc.AlphaToCoverageEnable = FALSE;
	g_D3D11BlendDesc.IndependentBlendEnable = FALSE;
	g_D3D11BlendDesc.RenderTarget[0].BlendEnable = FALSE;
	g_D3D11BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	g_D3D11BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	g_D3D11BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	g_D3D11BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	g_D3D11BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	g_D3D11BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	g_D3D11BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// Create the vertex shader constant buffer for D3D11
	{
		D3D11_BUFFER_DESC cbDesc = {};
		cbDesc.ByteWidth = CXBX_D3D11_VS_CB_COUNT * sizeof(float) * 4;
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;
		HRESULT cbHr = g_pD3DDevice->CreateBuffer(&cbDesc, nullptr, &g_pD3D11VSConstantBuffer);
		DEBUG_D3DRESULT(cbHr, "g_pD3DDevice->CreateBuffer (VS constant buffer)");
		if (SUCCEEDED(cbHr)) {
			g_pD3DDeviceContext->VSSetConstantBuffers(CXBX_D3D11_VS_CB_SLOT, 1, &g_pD3D11VSConstantBuffer);
		}
	}

	// Create the pixel shader constant buffer for D3D11
	{
		D3D11_BUFFER_DESC cbDesc = {};
		cbDesc.ByteWidth = CXBX_D3D11_PS_CB_COUNT * sizeof(float) * 4;
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;
		HRESULT cbHr = g_pD3DDevice->CreateBuffer(&cbDesc, nullptr, &g_pD3D11PSConstantBuffer);
		DEBUG_D3DRESULT(cbHr, "g_pD3DDevice->CreateBuffer (PS constant buffer)");
		if (SUCCEEDED(cbHr)) {
			g_pD3DDeviceContext->PSSetConstantBuffers(CXBX_D3D11_PS_CB_SLOT, 1, &g_pD3D11PSConstantBuffer);
		}
	}
#else
    // IDirect3D9::CreateDevice must be called from the window message thread
    // See https://docs.microsoft.com/en-us/windows/win32/direct3d9/multithreading-issues
    HRESULT hr;
    RunOnWndMsgThread([&hr, BehaviorFlags, &displayMode] {
        hr = g_pDirect3D->CreateDeviceEx(
            g_EmuCDPD.Adapter,
            g_EmuCDPD.DeviceType,
            g_hEmuWindow,
            BehaviorFlags,
            &g_EmuCDPD.HostPresentationParameters,
			g_EmuCDPD.HostPresentationParameters.Windowed ? nullptr : &displayMode,
            &g_pD3DDevice);
    });
    DEBUG_D3DRESULT(hr, "IDirect3D::CreateDeviceEx");

    if(FAILED(hr))
        CxbxrAbort("IDirect3D::CreateDeviceEx failed");
#endif

    // Which texture formats does this device support?
    DetermineSupportedD3DFormats();

#ifdef CXBX_USE_D3D11
	D3D11_QUERY_DESC QueryDesc;
	QueryDesc.Query = D3D11_QUERY_EVENT;
	QueryDesc.MiscFlags = 0;
#endif
    // Can host driver create event queries?
    if (SUCCEEDED(g_pD3DDevice->CreateQuery(_9_11(D3DQUERYTYPE_EVENT, &QueryDesc), nullptr))) {
        // Is host GPU query creation enabled?
        if (!g_bHack_DisableHostGPUQueries) {
            // Create a D3D event query to handle "wait-for-idle" with
            hr = g_pD3DDevice->CreateQuery(_9_11(D3DQUERYTYPE_EVENT, &QueryDesc), &g_pHostQueryWaitForIdle);
            DEBUG_D3DRESULT(hr, "g_pD3DDevice->CreateQuery (wait for idle)");

            // Create a D3D event query to handle "callback events" with
            hr = g_pD3DDevice->CreateQuery(_9_11(D3DQUERYTYPE_EVENT, &QueryDesc), &g_pHostQueryCallbackEvent);
            DEBUG_D3DRESULT(hr, "g_pD3DDevice->CreateQuery (callback event)");
        }
    } else {
        LOG_TEST_CASE("Can't CreateQuery(D3DQUERYTYPE_EVENT) on host!");
    }

    // Can host driver create occlusion queries?
#ifdef CXBX_USE_D3D11
	QueryDesc.Query = D3D11_QUERY_OCCLUSION;
#endif
    g_bEnableHostQueryVisibilityTest = false;
    if (SUCCEEDED(g_pD3DDevice->CreateQuery(_9_11(D3DQUERYTYPE_OCCLUSION, &QueryDesc), nullptr))) {
        // Is host GPU query creation enabled?
        if (!g_bHack_DisableHostGPUQueries) {
            g_bEnableHostQueryVisibilityTest = true;
        } else {
            LOG_TEST_CASE("Disabled D3DQUERYTYPE_OCCLUSION on host!");
        }
    } else {
        LOG_TEST_CASE("Can't CreateQuery(D3DQUERYTYPE_OCCLUSION) on host!");
    }

    DrawInitialBlackScreen();

    // Set up ImGui's render backend
#ifdef CXBX_USE_D3D11
	ImGui_ImplDX11_Init(g_pD3DDevice, g_pD3DDeviceContext);
	CxbxD3D11InitBlit();
    g_renderbase->SetDeviceRelease([] {
        ImGui_ImplDX11_Shutdown();
        g_VertexShaderCache.Clear();
        CxbxD3D11ReleaseBackendResources();
        if (g_pD3DDepthStencilView) { g_pD3DDepthStencilView->Release(); g_pD3DDepthStencilView = nullptr; }
        if (g_pD3DDepthStencilBuffer) { g_pD3DDepthStencilBuffer->Release(); g_pD3DDepthStencilBuffer = nullptr; }
        if (g_pD3DBackBufferView) { g_pD3DBackBufferView->Release(); g_pD3DBackBufferView = nullptr; }
        if (g_pD3DBackBufferSurface) { g_pD3DBackBufferSurface->Release(); g_pD3DBackBufferSurface = nullptr; }
        if (g_pD3DCurrentRTV && g_pD3DCurrentRTV != g_pD3DBackBufferView) { g_pD3DCurrentRTV->Release(); g_pD3DCurrentRTV = nullptr; }
        if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
        if (g_pD3DDeviceContext) { g_pD3DDeviceContext->Release(); g_pD3DDeviceContext = nullptr; }
        g_pD3DDevice->Release();
    });
#else
    ImGui_ImplDX9_Init(g_pD3DDevice);
    g_renderbase->SetDeviceRelease([] {
        ImGui_ImplDX9_Shutdown();
        g_pD3DDevice->Release();
    });
#endif
}


// check if a resource has been registered yet (if not, register it)
bool GetHostRenderTargetDimensions(DWORD *pHostWidth, DWORD *pHostHeight, IDirect3DSurface* pHostRenderTarget = nullptr)
{
	bool shouldRelease = false;
	if (pHostRenderTarget == nullptr) {
#ifdef CXBX_USE_D3D11
		// In D3D11, use the current back buffer as the render target
		pHostRenderTarget = g_pD3DCurrentHostRenderTarget;
#else
		g_pD3DDevice->GetRenderTarget(
			0, // RenderTargetIndex
			&pHostRenderTarget);

		shouldRelease = true;
#endif
	}

	// The following can only work if we could retrieve a host render target
	if (!pHostRenderTarget) {
		return false;
	}

	// Get current host render target dimensions
	D3DSurfaceDesc HostRenderTarget_Desc;
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
    DWORD XboxRenderTarget_Width_Scaled = XboxRenderTarget_Width * g_RenderUpscaleFactor;
    DWORD XboxRenderTarget_Height_Scaled = XboxRenderTarget_Height * g_RenderUpscaleFactor;
    if (HostRenderTarget_Width != XboxRenderTarget_Width_Scaled || HostRenderTarget_Height != XboxRenderTarget_Height_Scaled) {
        LOG_TEST_CASE("Existing RenderTarget width/height changed");

        FreeHostResource(GetHostResourceKey(g_pXbox_RenderTarget)); CxbxSetRenderTarget(GetHostSurface(g_pXbox_RenderTarget, D3DUSAGE_RENDERTARGET));
		FreeHostResource(GetHostResourceKey(g_pXbox_DepthStencil));
#ifdef CXBX_USE_D3D11
		{
			IDirect3DSurface *pHostDepthStencil = GetHostSurface(g_pXbox_DepthStencil, D3DUSAGE_DEPTHSTENCIL);
			ID3D11DepthStencilView* pDSV = nullptr;
			if (pHostDepthStencil != nullptr) {
				D3D11_TEXTURE2D_DESC texDesc = {};
				pHostDepthStencil->GetDesc(&texDesc);
				D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
				dsvDesc.Format = texDesc.Format;
				dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
				dsvDesc.Texture2D.MipSlice = 0;
				g_pD3DDevice->CreateDepthStencilView(pHostDepthStencil, &dsvDesc, &pDSV);
			}
			if (g_pD3DDepthStencilView) { g_pD3DDepthStencilView->Release(); }
			g_pD3DDepthStencilView = pDSV;
			g_pD3DDeviceContext->OMSetRenderTargets(1, &g_pD3DCurrentRTV, g_pD3DDepthStencilView);
		}
#else
		g_pD3DDevice->SetDepthStencilSurface(GetHostSurface(g_pXbox_DepthStencil, D3DUSAGE_DEPTHSTENCIL));
#endif
    }
}

float GetZScaleForPixelContainer(xbox::X_D3DPixelContainer* pSurface)
{
    // If no surface was present, fallback to 1
    if (pSurface == xbox::zeroptr) {
        return 1.0f;
    }

    auto format = GetXboxPixelContainerFormat(pSurface);
    switch (format) {
        case xbox::X_D3DFMT_D16:
        case xbox::X_D3DFMT_LIN_D16:
            return 65535.0f;

        case xbox::X_D3DFMT_D24S8:
        case xbox::X_D3DFMT_LIN_D24S8:
            return 16777215.0f;

        case xbox::X_D3DFMT_F16:
        case xbox::X_D3DFMT_LIN_F16:
            return 511.9375f;

        case xbox::X_D3DFMT_F24S8:
        case xbox::X_D3DFMT_LIN_F24S8:
            // 24bit floating point is close to precision maximum, so a lower value is used
            // We can't use a double here since the vertex shader is only at float precision
            return 1.0e30f; 
    }

    // Default to 1 if unknown depth format
    LOG_TEST_CASE("GetZScaleForSurface: Unknown Xbox Depth Format");
    return 1.0f;
}

// Get viewport offset and scale values intended to match the XDK calculations
// We should remove this when we can get the real viewport values from the xbox reliably via NV2A LLE
void GetXboxViewportOffsetAndScale(float (&vOffset)[4], float(&vScale)[4])
{
	// Antialiasing mode affects the viewport offset and scale
	float aaScaleX, aaScaleY;
	float aaOffsetX, aaOffsetY;
	GetScreenScaleFactors(aaScaleX, aaScaleY);
	GetMultiSampleOffset(aaOffsetX, aaOffsetY);

	float scaledX = g_Xbox_Viewport.X * aaScaleX;
	float scaledY = g_Xbox_Viewport.Y * aaScaleY;
	float scaledWidth = g_Xbox_Viewport.Width * aaScaleX;
	float scaledHeight = g_Xbox_Viewport.Height * aaScaleY;

	// D3D9 viewport info
	// https://docs.microsoft.com/en-us/windows/win32/direct3d9/viewports-and-clipping
	// Unlike D3D9, Xbox should scale by the z buffer scale
	// Test case: GTA III

	auto zRange = g_Xbox_Viewport.MaxZ - g_Xbox_Viewport.MinZ;
	vScale[0] = scaledWidth / 2;
	vScale[1] = -scaledHeight / 2;
	vScale[2] = zRange * g_ZScale;
	vScale[3] = 1;
	vOffset[0] = scaledWidth / 2 + scaledX;
	vOffset[1] = scaledHeight / 2 + scaledY;
	vOffset[2] = g_Xbox_Viewport.MinZ * g_ZScale;
	vOffset[3] = 0;
}

void CxbxUpdateHostViewPortOffsetAndScaleConstants()
{
    float vScaleOffset[2][4]; // 0 - scale 1 - offset
    GetXboxViewportOffsetAndScale(vScaleOffset[1], vScaleOffset[0]);


	// Xbox outputs vertex positions in rendertarget pixel coordinate space, with non-normalized Z
	// e.g. 0 < x < 640 and 0 < y < 480
	// We want to scale it back to normalized device coordinates i.e. XY are (-1, +1) and Z is (0, 1)

	// The screenspace is a combination of the rendertarget
	// and various scale factors
	// Get the rendertarget width and height
	float xboxRenderTargetWidth;
	float xboxRenderTargetHeight;
	GetRenderTargetBaseDimensions(xboxRenderTargetWidth, xboxRenderTargetHeight);

	float screenScaleX, screenScaleY;
	float aaOffsetX, aaOffsetY;
	GetScreenScaleFactors(screenScaleX, screenScaleY);
	GetMultiSampleOffset(aaOffsetX, aaOffsetY);

	// Add D3D9 half-pixel offset (-0.5 given this offset is subtracted)
	// We should be able to remove this when off D3D9
	// https://aras-p.info/blog/2016/04/08/solving-dx9-half-pixel-offset/
	aaOffsetX -= 0.5f;
	aaOffsetY -= 0.5f;

	float xboxScreenspaceWidth = xboxRenderTargetWidth * screenScaleX;
	float xboxScreenspaceHeight = xboxRenderTargetHeight * screenScaleY;

	// Passthrough should range 0 to 1, instead of 0 to zbuffer depth
	// Test case: DoA3 character select
	float zOutputScale = g_Xbox_VertexShaderMode == VertexShaderMode::Passthrough ? 1 : g_ZScale;

	float screenspaceScale[4] = { xboxScreenspaceWidth / 2,  -xboxScreenspaceHeight / 2, zOutputScale, 1 };
	float screenspaceOffset[4] = { xboxScreenspaceWidth / 2 + aaOffsetX, xboxScreenspaceHeight / 2 + aaOffsetY, 0, 0 };
#ifdef CXBX_USE_D3D11
	// In D3D11, no half-pixel offset needed
	screenspaceOffset[0] = xboxScreenspaceWidth / 2;
	screenspaceOffset[1] = xboxScreenspaceHeight / 2;
#endif
	CxbxSetVertexShaderConstantF(CXBX_D3DVS_SCREENSPACE_SCALE_BASE, screenspaceScale, CXBX_D3DVS_NORMALIZE_SCALE_SIZE);
	CxbxSetVertexShaderConstantF(CXBX_D3DVS_SCREENSPACE_OFFSET_BASE, screenspaceOffset, CXBX_D3DVS_NORMALIZE_OFFSET_SIZE);

	// Store viewport offset and scale in constant registers 58 (c-38) and
	// 59 (c-37) used for screen space transformation.
	// We only do this if X_D3DSCM_NORESERVEDCONSTANTS is not set,
	// since enabling this flag frees up these registers for shader use
	// Treat this as a flag
	// Test Case: GTA III, Soldier of Fortune II
	if (!(g_Xbox_VertexShaderConstantMode & X_D3DSCM_NORESERVEDCONSTANTS)) {
		CxbxSetVertexShaderConstantF(X_D3DSCM_RESERVED_CONSTANT_SCALE_CORRECTED, reinterpret_cast<float*>(vScaleOffset), 2);
	}

}

// ******************************************************************
// * patch: D3DDevice_SetViewport
// ******************************************************************
void UpdateFixedFunctionShaderLight(int d3dLightIndex, Light* pShaderLight, D3DXVECTOR4* pLightAmbient) {
	if (d3dLightIndex == -1) {
		pShaderLight->Type = 0; // Disable the light
		return;
	}

	auto d3dLight = &d3d8LightState.Lights[d3dLightIndex];
	auto viewTransform = (D3DXMATRIX)d3d8TransformState.Transforms[xbox::X_D3DTS_VIEW];

	// TODO remove D3DX usage
	// Pre-transform light position to viewspace
	D3DXVECTOR4 positionV;
	D3DXVec3Transform(&positionV, (D3DXVECTOR3*)&d3dLight->Position, &viewTransform);
	pShaderLight->PositionV = (D3DXVECTOR3)positionV;

	// Pre-transform light direction to viewspace and normalize
	D3DXVECTOR4 directionV;
	D3DXMATRIX viewTransform3x3;
	D3DXMatrixIdentity(&viewTransform3x3);
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			viewTransform3x3.m[x][y] = viewTransform.m[x][y];
		}
	}

	D3DXVec3Transform(&directionV, (D3DXVECTOR3*)&d3dLight->Direction, &viewTransform3x3);
	D3DXVec3Normalize((D3DXVECTOR3*)&pShaderLight->DirectionVN, (D3DXVECTOR3*)&directionV);

	bool SpecularEnable = XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_SPECULARENABLE) != FALSE;

	// Map D3D light to state struct
	pShaderLight->Type = (float)((int)d3dLight->Type);
	pShaderLight->Diffuse = toVector(d3dLight->Diffuse);
	pShaderLight->Specular = SpecularEnable ? toVector(d3dLight->Specular) : toVector(0);
	pShaderLight->Range = d3dLight->Range;
	pShaderLight->Falloff = d3dLight->Falloff;
	pShaderLight->Attenuation.x = d3dLight->Attenuation0;
	pShaderLight->Attenuation.y = d3dLight->Attenuation1;
	pShaderLight->Attenuation.z = d3dLight->Attenuation2;

	pLightAmbient->x += d3dLight->Ambient.r;
	pLightAmbient->y += d3dLight->Ambient.g;
	pLightAmbient->z += d3dLight->Ambient.b;

	auto cosHalfPhi = cos(d3dLight->Phi / 2);
	pShaderLight->CosHalfPhi = cosHalfPhi;
	pShaderLight->SpotIntensityDivisor = cos(d3dLight->Theta / 2) - cos(d3dLight->Phi / 2);
}

void UpdateFixedFunctionVertexShaderState()
{
	extern xbox::X_VERTEXATTRIBUTEFORMAT* GetXboxVertexAttributeFormat(); // TMP glue
	using namespace xbox;

	// Vertex blending
	// Prepare vertex blending mode variables used in transforms, below
	auto VertexBlend = XboxRenderStates.GetXboxRenderState(X_D3DRS_VERTEXBLEND);
	// Xbox and host D3DVERTEXBLENDFLAGS :
	//     D3DVBF_DISABLE           = 0 : 1 matrix,   0 weights => final weight 1
	//     D3DVBF_1WEIGHTS          = 1 : 2 matrices, 1 weights => final weight calculated
	//     D3DVBF_2WEIGHTS          = 3 : 3 matrices, 2 weights => final weight calculated
	//     D3DVBF_3WEIGHTS          = 5 : 4 matrices, 3 weights => final weight calculated
	// Xbox X_D3DVERTEXBLENDFLAGS :
	//   X_D3DVBF_2WEIGHTS2MATRICES = 2 : 2 matrices, 2 weights
	//   X_D3DVBF_3WEIGHTS3MATRICES = 4 : 3 matrices, 3 weights
	//   X_D3DVBF_4WEIGHTS4MATRICES = 6 : 4 matrices, 4 weights
	//
	if (VertexBlend > xbox::X_D3DVBF_4WEIGHTS4MATRICES) LOG_TEST_CASE("X_D3DRS_VERTEXBLEND out of range");
	// Calculate the number of matrices, by adding the LSB to turn (0,1,3,5) and (0,2,4,6) into (0,2,4,6); Then divide by 2 to get (0,1,2,3), and add 1 to get 1, 2, 3 or 4 matrices :
	auto NrBlendMatrices = ((VertexBlend + (VertexBlend & 1)) / 2) + 1;
	// Looking at the above values, 0 or the LSB of VertexBlend signals that the final weight needs to be calculated from all previous weigths (deducting them all from an initial 1) :
	auto CalcLastBlendWeight = (VertexBlend == xbox::X_D3DVBF_DISABLE) || (VertexBlend & 1);
	// Copy the resulting values over to shader state :
	ffShaderState.Modes.VertexBlend_NrOfMatrices = (float)NrBlendMatrices;
	ffShaderState.Modes.VertexBlend_CalcLastWeight = (float)CalcLastBlendWeight;

	// Transforms
	// Transpose row major to column major for HLSL
	D3DXMatrixTranspose((D3DXMATRIX*)&ffShaderState.Transforms.Projection, (D3DXMATRIX*)&d3d8TransformState.Transforms[X_D3DTS_PROJECTION]);
	D3DXMatrixTranspose((D3DXMATRIX*)&ffShaderState.Transforms.View, (D3DXMATRIX*)&d3d8TransformState.Transforms[X_D3DTS_VIEW]);

	for (unsigned i = 0; i < 4; i++) { // TODO : Would it help to limit this to just the active texture channels?
		D3DXMatrixTranspose((D3DXMATRIX*)&ffShaderState.Transforms.Texture[i], (D3DXMATRIX*)&d3d8TransformState.Transforms[X_D3DTS_TEXTURE0 + i]);
	}

	for (unsigned i = 0; i < ffShaderState.Modes.VertexBlend_NrOfMatrices; i++) {
		D3DXMatrixTranspose((D3DXMATRIX*)&ffShaderState.Transforms.WorldView[i], (D3DXMATRIX*)d3d8TransformState.GetWorldView(i));
		D3DXMatrixTranspose((D3DXMATRIX*)&ffShaderState.Transforms.WorldViewInverseTranspose[i], (D3DXMATRIX*)d3d8TransformState.GetWorldViewInverseTranspose(i));
	}

	// Lighting
	// Point sprites aren't lit - 'each point is always rendered with constant colors.'
	// https://docs.microsoft.com/en-us/windows/win32/direct3d9/point-sprites
	bool PointSpriteEnable = XboxRenderStates.GetXboxRenderState(X_D3DRS_POINTSPRITEENABLE);
	bool LightingEnable = XboxRenderStates.GetXboxRenderState(X_D3DRS_LIGHTING);
	ffShaderState.Modes.Lighting = LightingEnable && !PointSpriteEnable;
	ffShaderState.Modes.TwoSidedLighting = (float)XboxRenderStates.GetXboxRenderState(X_D3DRS_TWOSIDEDLIGHTING);
	ffShaderState.Modes.LocalViewer = (float)XboxRenderStates.GetXboxRenderState(X_D3DRS_LOCALVIEWER);

	// Material sources
	bool ColorVertex = XboxRenderStates.GetXboxRenderState(X_D3DRS_COLORVERTEX) != FALSE;
	ffShaderState.Modes.AmbientMaterialSource = (float)(ColorVertex ? XboxRenderStates.GetXboxRenderState(X_D3DRS_AMBIENTMATERIALSOURCE) : D3DMCS_MATERIAL);
	ffShaderState.Modes.DiffuseMaterialSource = (float)(ColorVertex ? XboxRenderStates.GetXboxRenderState(X_D3DRS_DIFFUSEMATERIALSOURCE) : D3DMCS_MATERIAL);
	ffShaderState.Modes.SpecularMaterialSource = (float)(ColorVertex ? XboxRenderStates.GetXboxRenderState(X_D3DRS_SPECULARMATERIALSOURCE) : D3DMCS_MATERIAL);
	ffShaderState.Modes.EmissiveMaterialSource = (float)(ColorVertex ? XboxRenderStates.GetXboxRenderState(X_D3DRS_EMISSIVEMATERIALSOURCE) : D3DMCS_MATERIAL);
	ffShaderState.Modes.BackAmbientMaterialSource = (float)(ColorVertex ? XboxRenderStates.GetXboxRenderState(X_D3DRS_BACKAMBIENTMATERIALSOURCE) : D3DMCS_MATERIAL);
	ffShaderState.Modes.BackDiffuseMaterialSource = (float)(ColorVertex ? XboxRenderStates.GetXboxRenderState(X_D3DRS_BACKDIFFUSEMATERIALSOURCE) : D3DMCS_MATERIAL);
	ffShaderState.Modes.BackSpecularMaterialSource = (float)(ColorVertex ? XboxRenderStates.GetXboxRenderState(X_D3DRS_BACKSPECULARMATERIALSOURCE) : D3DMCS_MATERIAL);
	ffShaderState.Modes.BackEmissiveMaterialSource = (float)(ColorVertex ? XboxRenderStates.GetXboxRenderState(X_D3DRS_BACKEMISSIVEMATERIALSOURCE) : D3DMCS_MATERIAL);

	// Point sprites; Fetch required variables
	float pointSize = XboxRenderStates.GetXboxRenderStateAsFloat(X_D3DRS_POINTSIZE);
	float pointSize_Min = XboxRenderStates.GetXboxRenderStateAsFloat(X_D3DRS_POINTSIZE_MIN);
	float pointSize_Max = XboxRenderStates.GetXboxRenderStateAsFloat(X_D3DRS_POINTSIZE_MAX);
	bool PointScaleEnable = XboxRenderStates.GetXboxRenderState(X_D3DRS_POINTSCALEENABLE);
	float pointScale_A = XboxRenderStates.GetXboxRenderStateAsFloat(X_D3DRS_POINTSCALE_A);
	float pointScale_B = XboxRenderStates.GetXboxRenderStateAsFloat(X_D3DRS_POINTSCALE_B);
	float pointScale_C = XboxRenderStates.GetXboxRenderStateAsFloat(X_D3DRS_POINTSCALE_C);
	float renderTargetHeight = (float)GetPixelContainerHeight(g_pXbox_RenderTarget);
	// Make sure to disable point scaling when point sprites are not enabled
	PointScaleEnable &= PointSpriteEnable;
	// Set variables in shader state
	ffShaderState.PointSprite.PointSize = PointSpriteEnable ? pointSize : 1.0f;
	ffShaderState.PointSprite.PointSize_Min = PointSpriteEnable ? pointSize_Min : 1.0f;
	ffShaderState.PointSprite.PointSize_Max = PointSpriteEnable ? pointSize_Max : 1.0f;
	ffShaderState.PointSprite.PointScaleABC.x = PointScaleEnable ? pointScale_A : 1.0f;
	ffShaderState.PointSprite.PointScaleABC.y = PointScaleEnable ? pointScale_B : 0.0f;
	ffShaderState.PointSprite.PointScaleABC.z = PointScaleEnable ? pointScale_C : 0.0f;
	ffShaderState.PointSprite.XboxRenderTargetHeight = PointScaleEnable ? renderTargetHeight : 1.0f;
	ffShaderState.PointSprite.RenderUpscaleFactor = (float)g_RenderUpscaleFactor;

	// Fog
	// Determine how the fog depth is transformed into the fog factor
	auto fogEnable = XboxRenderStates.GetXboxRenderState(X_D3DRS_FOGENABLE);
	auto fogTableMode = XboxRenderStates.GetXboxRenderState(X_D3DRS_FOGTABLEMODE);
	ffShaderState.Fog.Enable = static_cast<float>(fogEnable);
	// FIXME remove when fixed function PS is implemented
	// Note if we are using the fixed function pixel shader
	// We only want to produce the fog depth value in the VS, not the fog factor
	ffShaderState.Fog.TableMode = static_cast<float>(!g_UseFixedFunctionPixelShader ? D3DFOG_NONE : fogTableMode);

	// Determine how fog depth is calculated
	if (fogEnable && fogTableMode != D3DFOG_NONE) {
		D3DXMATRIX projMtx = ffShaderState.Transforms.Projection;

		if (XboxRenderStates.GetXboxRenderState(X_D3DRS_RANGEFOGENABLE)) {
			LOG_TEST_CASE("Using RANGE fog");
			ffShaderState.Fog.DepthMode = FixedFunctionVertexShader::FOG_DEPTH_RANGE;
		}
		else if (projMtx._14 == 0 &&
			projMtx._24 == 0 &&
			projMtx._34 == 0 &&
			projMtx._44 == 1) {
			LOG_TEST_CASE("Using Z fog");
			ffShaderState.Fog.DepthMode = FixedFunctionVertexShader::FOG_DEPTH_Z;
		}
		else {
			// Test case:
			// Fog sample
			// JSRF (non-compliant projection matrix)
			ffShaderState.Fog.DepthMode = FixedFunctionVertexShader::FOG_DEPTH_W;
		}

		ffShaderState.Fog.Density = XboxRenderStates.GetXboxRenderStateAsFloat(X_D3DRS_FOGDENSITY);
		ffShaderState.Fog.Start = XboxRenderStates.GetXboxRenderStateAsFloat(X_D3DRS_FOGSTART);
		ffShaderState.Fog.End = XboxRenderStates.GetXboxRenderStateAsFloat(X_D3DRS_FOGEND);
	}
	else {
		ffShaderState.Fog.DepthMode = FixedFunctionVertexShader::FOG_DEPTH_NONE;
	}

	// Texture state
	for (int i = 0; i < xbox::X_D3DTS_STAGECOUNT; i++) {
		auto transformFlags = XboxTextureStates.Get(i, X_D3DTSS_TEXTURETRANSFORMFLAGS);
		ffShaderState.TextureStates[i].TextureTransformFlagsCount = (float)(transformFlags & ~D3DTTFF_PROJECTED);
		ffShaderState.TextureStates[i].TextureTransformFlagsProjected = (float)(transformFlags & D3DTTFF_PROJECTED);

		auto texCoordIndex = XboxTextureStates.Get(i, X_D3DTSS_TEXCOORDINDEX);
		ffShaderState.TextureStates[i].TexCoordIndex = (float)(texCoordIndex & 0x7); // 8 coords
		ffShaderState.TextureStates[i].TexCoordIndexGen = (float)(texCoordIndex >> 16); // D3DTSS_TCI flags
	}

	// Read current TexCoord component counts
	xbox::X_VERTEXATTRIBUTEFORMAT* pXboxVertexAttributeFormat = GetXboxVertexAttributeFormat();
	// Note : There seem to be other ways to access this, but we can use only this one;
	// This, because CxbxGetVertexDeclaration() can't be used, since it doesn't track VertexAttributes
	// (plus, it contains the overhead of shader lookup).
	// Another, GetXboxVertexShader(), can't be used, because it doesn't honor vertex attribute overrides
	// like those that apply for g_InlineVertexBuffer_DeclarationOverride and active SetVertexShaderInput.
	// Also, the xbox::X_D3DVertexShader.Dimensionality[] field contains somewhat strange values.
	for (int i = 0; i < xbox::X_D3DTS_STAGECOUNT; i++) {
		auto vertexDataFormat = pXboxVertexAttributeFormat->Slots[xbox::X_D3DVSDE_TEXCOORD0 + i].Format;
		reinterpret_cast<float*>(&ffShaderState.TexCoordComponentCount)[i] = (float)GetXboxVertexDataComponentCount(vertexDataFormat);
	}

	// Update lights
	auto LightAmbient = D3DXVECTOR4(0.f, 0.f, 0.f, 0.f);
	for (size_t i = 0; i < ffShaderState.Lights.size(); i++) {
		UpdateFixedFunctionShaderLight(d3d8LightState.EnabledLights[i], &ffShaderState.Lights[i], &LightAmbient);
	}

	D3DXVECTOR4 Ambient = toVector(XboxRenderStates.GetXboxRenderState(X_D3DRS_AMBIENT));
	D3DXVECTOR4 BackAmbient = toVector(XboxRenderStates.GetXboxRenderState(X_D3DRS_BACKAMBIENT));

	ffShaderState.TotalLightsAmbient.Front = (D3DXVECTOR3)(LightAmbient + Ambient);
	ffShaderState.TotalLightsAmbient.Back = (D3DXVECTOR3)(LightAmbient + BackAmbient);

	// Misc flags
	ffShaderState.Modes.NormalizeNormals = (float)XboxRenderStates.GetXboxRenderState(X_D3DRS_NORMALIZENORMALS);

	// Write fixed function state to shader constants
	const int slotSize = 16;
	const int fixedFunctionStateSize = (sizeof(FixedFunctionVertexShaderState) + slotSize - 1) / slotSize;
	CxbxSetVertexShaderConstantF(0, (float*)&ffShaderState, fixedFunctionStateSize);

	// TODO: Error handling for D3D9 path only (SetVertexShaderConstantF returns HRESULT there)
}

