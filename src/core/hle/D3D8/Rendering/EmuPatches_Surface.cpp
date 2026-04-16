#include "EmuD3D8_common.h"


xbox::X_D3DSurface* WINAPI xbox::EMUPATCH(D3DDevice_GetBackBuffer2)
(
	int_xt                 BackBuffer
)
{
	LOG_FUNC_ONE_ARG(BackBuffer);

	return CxbxrImpl_GetBackBuffer2(BackBuffer);
}

static void D3DDevice_GetBackBuffer2_0__LTCG_eax1(xbox::int_xt BackBuffer)
{
	LOG_FUNC_ONE_ARG(BackBuffer);
}

// LTCG specific GetBackBuffer2 function...
// This uses a custom calling convention where parameter is passed in EAX
__declspec(naked) xbox::X_D3DSurface* WINAPI xbox::EMUPATCH(D3DDevice_GetBackBuffer2_0__LTCG_eax1)
(
)
{

	int_xt BackBuffer;
	xbox::X_D3DSurface* pBackBuffer;
	__asm {
		LTCG_PROLOGUE
		mov  BackBuffer, eax
	}

	// Log
	D3DDevice_GetBackBuffer2_0__LTCG_eax1(BackBuffer);

	pBackBuffer = CxbxrImpl_GetBackBuffer2(BackBuffer);

	__asm {
		mov  eax, pBackBuffer
		LTCG_EPILOGUE
		ret
	}
}

// ******************************************************************
// * patch: D3DDevice_GetBackBuffer
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_GetBackBuffer)
(
    int_xt                BackBuffer,
    D3DBACKBUFFER_TYPE    Type,
    X_D3DSurface        **ppBackBuffer
)
{
	LOG_FORWARD("D3DDevice_GetBackBuffer2");

	*ppBackBuffer = CxbxrImpl_GetBackBuffer2(BackBuffer);
}

// ******************************************************************
// * patch: D3DDevice_GetBackBuffer_8__LTCG_eax1
// ******************************************************************
// Overload for logging
static void D3DDevice_GetBackBuffer_8__LTCG_eax1()
{
	LOG_FORWARD("D3DDevice_GetBackBuffer2");
}

// Test case: NBA 2K2
// This uses a custom calling convention where parameter is passed in EAX
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_GetBackBuffer_8__LTCG_eax1)
(
    D3DBACKBUFFER_TYPE Type,
    X_D3DSurface     **ppBackBuffer
)
{
	int_xt BackBuffer;
	__asm {
		LTCG_PROLOGUE
		mov  BackBuffer, eax
	}

	// Log
	D3DDevice_GetBackBuffer_8__LTCG_eax1();

	*ppBackBuffer = CxbxrImpl_GetBackBuffer2(BackBuffer);

	__asm {
		LTCG_EPILOGUE
		ret  8
	}
}

xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_Clear)
(
    dword_xt           Count,
    CONST X_D3DRECT   *pRects,
    dword_xt           Flags,
	X_D3DCOLOR         Color,
    float              Z,
    dword_xt           Stencil
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

	// Clear requires the Xbox viewport to be applied
	CxbxUpdateNativeD3DResources();

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

    if (Count > 0 && pRects != nullptr) {
        // Scale the fill based on our scale factor and MSAA scale
		float aaX, aaY;
		GetMultiSampleScaleRaw(aaX, aaY);
		float Xscale = aaX * g_RenderUpscaleFactor;
		float Yscale = aaY * g_RenderUpscaleFactor;

        std::vector<D3DRECT> rects(Count);
        for (DWORD i = 0; i < Count; i++) {
            rects[i]._9_11(x1, left) = static_cast<LONG>(pRects[i].x1 * Xscale);
            rects[i]._9_11(x2, right) = static_cast<LONG>(pRects[i].x2 * Xscale);
            rects[i]._9_11(y1, top) = static_cast<LONG>(pRects[i].y1 * Yscale);
            rects[i]._9_11(y2, bottom) = static_cast<LONG>(pRects[i].y2 * Yscale);
		}
        CxbxD3DClear(Count, rects.data(), HostFlags, Color, Z, Stencil);
    } else {
		CxbxD3DClear(Count, reinterpret_cast<const D3DRECT*>(pRects), HostFlags, Color, Z, Stencil);
    }
}


// ******************************************************************
// * patch: D3DDevice_CopyRects
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_CopyRects)
(
    X_D3DSurface*  pSourceSurface,
    CONST X_RECT*  pSourceRectsArray,
    uint_xt        cRects,
    X_D3DSurface*  pDestinationSurface,
    CONST X_POINT* pDestPointsArray
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
    auto pHostSourceSurface = GetHostSurface(pSourceSurface);
    auto pHostDestSurface = GetHostSurface(pDestinationSurface);

    if (pHostSourceSurface == nullptr || pHostDestSurface == nullptr) {
        // Test Case: DOA2 attempts to copy from an index buffer resource type
        // TODO: What should we do here?
        LOG_TEST_CASE("D3DDevice-CopyRects: Failed to fetch host surfaces");
        return;
    }

	D3DSurfaceDesc hostSourceDesc, hostDestDesc;
    pHostSourceSurface->GetDesc(&hostSourceDesc);
    pHostDestSurface->GetDesc(&hostDestDesc);

    // If the source is a render-target and the destination is not, we need force it to be re-created as one
    // This is because StrechRects cannot copy from a Render-Target to a Non-Render Target
    // Test Case: Crash Bandicoot: Wrath of Cortex attemps to copy the render-target to a texture
    // This fixes an issue on the pause screen where the screenshot of the current scene was not displayed correctly
    if ((hostSourceDesc.Usage & D3DUSAGE_RENDERTARGET) != 0 && (hostDestDesc.Usage & D3DUSAGE_RENDERTARGET) == 0) {
        pHostDestSurface = GetHostSurface(pDestinationSurface, D3DUSAGE_RENDERTARGET);
        pHostDestSurface->GetDesc(&hostDestDesc);
    }

    // If no rectangles were given, default to 1 (entire surface)
    if (cRects == 0) {
        cRects = 1;
    }

    // Get Xbox surface dimensions
    // Host resources may be scaled so we'll account for that later
    auto xboxSourceWidth = GetPixelContainerWidth(pSourceSurface);
    auto xboxSourceHeight = GetPixelContainerHeight(pSourceSurface);
    auto xboxDestWidth = GetPixelContainerWidth(pDestinationSurface);
    auto xboxDestHeight = GetPixelContainerHeight(pDestinationSurface);

    for (UINT i = 0; i < cRects; i++) {
        RECT SourceRect, DestRect;

        if (pSourceRectsArray != nullptr) {
            SourceRect.left = pSourceRectsArray[i].left;
            SourceRect.right = pSourceRectsArray[i].right;
            SourceRect.top = pSourceRectsArray[i].top;
            SourceRect.bottom = pSourceRectsArray[i].bottom;
        } else {
            SourceRect.left = 0;
            SourceRect.right = xboxSourceWidth;
            SourceRect.top = 0;
            SourceRect.bottom = xboxSourceHeight;
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
            DestRect.right = xboxDestWidth;
            DestRect.top = 0;
            DestRect.bottom = xboxDestHeight;
        }

        // Scale the source and destination rects
        auto sourceScaleX = (uint32_t)hostSourceDesc.Width / xboxSourceWidth;
        auto sourceScaleY = (uint32_t)hostSourceDesc.Height / xboxSourceHeight;

        SourceRect.left *= sourceScaleX;
        SourceRect.right *= sourceScaleX;
        SourceRect.top *= sourceScaleY;
        SourceRect.bottom *= sourceScaleY;

        auto destScaleX = (uint32_t) hostDestDesc.Width / xboxDestWidth;
        auto destScaleY = (uint32_t) hostDestDesc.Height / xboxDestHeight;

        DestRect.left *= destScaleX;
        DestRect.right *= destScaleX;
        DestRect.top *= destScaleY;
        DestRect.bottom *= destScaleY;

        HRESULT hRet;
        hRet = CxbxBltSurface(pHostSourceSurface, &SourceRect, pHostDestSurface, &DestRect, D3DTEXF_LINEAR);
        if (FAILED(hRet)) {
            LOG_TEST_CASE("D3DDevice_CopyRects: Failed to copy surface");
        }
    }
}

#define CXBX_SWAP_PRESENT_FORWARD (256 + X_D3DSWAP_FINISH + X_D3DSWAP_COPY) // = CxbxPresentForwardMarker + D3DSWAP_FINISH + D3DSWAP_COPY

// ******************************************************************
// * patch: D3DDevice_Present
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_Present)
(
    CONST X_RECT* pSourceRect,
    CONST X_RECT* pDestRect,
    PVOID         pDummy1,
    PVOID         pDummy2
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

std::chrono::steady_clock::time_point frameStartTime;

// LTCG specific swap function...
// This uses a custom calling convention where parameter is passed in EAX
__declspec(naked) xbox::dword_xt WINAPI xbox::EMUPATCH(D3DDevice_Swap_0__LTCG_eax1)
(
)
{
    dword_xt Flags;
	dword_xt result;
    __asm {
        LTCG_PROLOGUE
        mov  Flags, eax
    }

    result = EMUPATCH(D3DDevice_Swap)(Flags);

    __asm {
		mov  eax, result
        LTCG_EPILOGUE
        ret
    }
}

// ******************************************************************
// * patch: D3DDevice_Swap
// ******************************************************************
xbox::dword_xt WINAPI xbox::EMUPATCH(D3DDevice_Swap)
(
	dword_xt Flags
)
{
	LOG_FUNC_ONE_ARG(Flags);

	// Handle swap flags
	// We don't maintain a swap chain, and draw everything to backbuffer 0
	// so just hack around the swap flags for now...
	static float prevBackBufferScaleX;
	if (Flags == X_D3DSWAP_BYPASSCOPY) {
		// Test case: MotoGp2
		// Title handles copying to the frontbuffer itself, but we don't keep track of one
		// MotoGp2 seems to copy a black rectangle over the backbuffer
		// HACK: Effectively disable drawing - so the title can't copy anything around via draws
		// and we just have to hope the title leaves the backbuffer untouched...
		prevBackBufferScaleX = g_Xbox_BackbufferScaleX;
		g_Xbox_BackbufferScaleX = 0;
	}
	else if (g_LastD3DSwap == X_D3DSWAP_BYPASSCOPY) {
		g_Xbox_BackbufferScaleX = prevBackBufferScaleX;
	}

	g_LastD3DSwap = (xbox::X_D3DSWAP)Flags;

	// Early exit if we're not ready to present
	// Test Case: Antialias sample, BackBufferScale sample
	// Which use D3DSWAP_COPY to render UI directly to the frontbuffer
	// If we present before the UI is drawn, it will flicker
	if (Flags != X_D3DSWAP_DEFAULT && !(Flags & X_D3DSWAP_FINISH)) {
		if (Flags == X_D3DSWAP_COPY) { LOG_TEST_CASE("X_D3DSWAP_COPY"); }
		if (Flags == X_D3DSWAP_BYPASSCOPY) { LOG_TEST_CASE("X_D3DSWAP_BYPASSCOPY"); }
		return g_Xbox_SwapData.Swap;
	}

	// Fetch the host backbuffer
	IDirect3DSurface *pCurrentHostBackBuffer = nullptr;
	HRESULT hRet = CxbxGetBackBuffer(&pCurrentHostBackBuffer);

	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetBackBuffer - Unable to get backbuffer surface!");
	if (hRet == D3D_OK) {
		assert(pCurrentHostBackBuffer != nullptr);

        // Clear the backbuffer surface, this prevents artifacts when switching aspect-ratio
        // Test-case: Dashboard
        IDirect3DSurface* pExistingRenderTarget = CxbxGetCurrentRenderTarget();
        if (pExistingRenderTarget) {
            (void)CxbxSetRenderTarget(pCurrentHostBackBuffer);
            CxbxD3DClear(
                /*Count=*/0,
                /*pRects=*/nullptr,
                D3DCLEAR_TARGET | (g_bHasDepth ? D3DCLEAR_ZBUFFER : 0) | (g_bHasStencil ? D3DCLEAR_STENCIL : 0),
                /*Color=*/0xFF000000, // TODO : Use constant for this
                /*Z=*/g_bHasDepth ? 1.0f : 0.0f,
                /*Stencil=*/0);
            (void)CxbxSetRenderTarget(pExistingRenderTarget);
#ifndef CXBX_USE_D3D11
            pExistingRenderTarget->Release();
#endif
        }
        
        // TODO: Implement a hot-key to change the filter?
        // Note: LoadSurfaceFilter Must be D3DTEXF_NONE, D3DTEXF_POINT or D3DTEXF_LINEAR
        // Before StretchRects we used D3DX_FILTER_POINT here, but that gave jagged edges in Dashboard.
        // Dxbx note : D3DX_FILTER_LINEAR gives a smoother image, but 'bleeds' across borders
        // LoadOverlayFilter must be a D3DX filter DWORD value
        const D3DTEXTUREFILTERTYPE LoadSurfaceFilter = D3DTEXF_LINEAR;
        const DWORD LoadOverlayFilter = D3DX_DEFAULT;

        // Use backbuffer width/height since that may differ from the Window size
        const auto width = g_XBVideo.bMaintainAspect ? g_AspectRatioScaleWidth * g_AspectRatioScale : g_HostBackBufferDesc.Width;
        const auto height = g_XBVideo.bMaintainAspect ? g_AspectRatioScaleHeight * g_AspectRatioScale : g_HostBackBufferDesc.Height;

		auto pXboxBackBufferHostSurface = GetHostSurface(g_pXbox_BackBufferSurface, D3DUSAGE_RENDERTARGET);
		if (pXboxBackBufferHostSurface) {
            // Calculate the centered rectangle
            RECT dest{};
            dest.top = (LONG)((g_HostBackBufferDesc.Height - height) / 2);
            dest.left = (LONG)((g_HostBackBufferDesc.Width - width) / 2);
            dest.right = (LONG)(dest.left + width);
            dest.bottom = (LONG)(dest.top + height);

            // Blit Xbox BackBuffer to host BackBuffer
            hRet = CxbxBltSurface(
                /* pSrc = */ pXboxBackBufferHostSurface,
                /* pSrcRect = */ nullptr,
                /* pDst = */ pCurrentHostBackBuffer,
                /* pDstRect = */ &dest,
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
			EMUFORMAT PCFormat;
			// TODO : Before reading from pgraph, flush all pending push-buffer commands
			switch (GET_MASK(HLE_read_NV2A_pgraph_register(NV_PGRAPH_CONTROL_0), NV_PGRAPH_CONTROL_0_CSCONVERT)) {
			case 0:  // = pass-through
				PCFormat = EMUFMT_YUY2;
				break;
			case 1: // = CRYCB_TO_RGB
				PCFormat = EMUFMT_YUY2; // Test-case : Turok (intro movie)
				break;
			case 2: // = SCRYSCB_TO_RGB
				LOG_TEST_CASE("SCRYSCB_TO_RGB");
				PCFormat = EMUFMT_YUY2;
				break;
			default:
				LOG_TEST_CASE("Unrecognized NV_PGRAPH_CONTROL_0_CSCONVERT");
				PCFormat = EMUFMT_YUY2;
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

				// We also need to account for any MSAA which may have enlarged the Xbox Backbuffer
				float xScale, yScale;
				GetMultiSampleScaleRaw(xScale, yScale);

                xScale = (float)width / ((float)XboxBackBufferWidth / xScale);
                yScale = (float)height / ((float)XboxBackBufferHeight / yScale);

                // Scale the destination co-ordinates by the correct scale factor
                EmuDestRect.top = (LONG)(EmuDestRect.top * yScale);
                EmuDestRect.left = (LONG)(EmuDestRect.left * xScale);
                EmuDestRect.bottom = (LONG)(EmuDestRect.bottom * yScale);
                EmuDestRect.right = (LONG)(EmuDestRect.right * xScale);

                // Finally, adjust to correct on-screen position (
                EmuDestRect.top += (LONG)((g_HostBackBufferDesc.Height - height) / 2);
                EmuDestRect.left += (LONG)((g_HostBackBufferDesc.Width - width) / 2);
                EmuDestRect.right += (LONG)((g_HostBackBufferDesc.Width - width) / 2);
                EmuDestRect.bottom += (LONG)((g_HostBackBufferDesc.Height - height) / 2);
			} else {
                // Calculate the centered rectangle
                EmuDestRect.top = (LONG)((g_HostBackBufferDesc.Height - height) / 2);
                EmuDestRect.left = (LONG)((g_HostBackBufferDesc.Width - width) / 2);
                EmuDestRect.right = (LONG)(EmuDestRect.left + width);
                EmuDestRect.bottom = (LONG)(EmuDestRect.top + height);
			}

			// load the YUY2 into the backbuffer

			// Limit the width and height of the output to the backbuffer dimensions.
			// This will (hopefully) prevent exceptions in Blinx - The Time Sweeper
			// (see https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/285)
			{
				// Use our (bounded) copy when bounds exceed :
				if (EmuDestRect.right > (LONG)g_HostBackBufferDesc.Width) {
					EmuDestRect.right = (LONG)g_HostBackBufferDesc.Width;
				}

				if (EmuDestRect.bottom > (LONG)g_HostBackBufferDesc.Height) {
					EmuDestRect.bottom = (LONG)g_HostBackBufferDesc.Height;
				}
			}

            // Create a temporary surface to hold the overlay
            // This is faster than loading directly into the backbuffer because it offloads scaling to the GPU
            // Without this, upscaling tanks the frame-rate!
#ifdef CXBX_USE_D3D11
            // D3D11 overlay: create a staging texture, upload the overlay data, and copy to backbuffer
            // Note: This is a simplified implementation that skips format conversion (assumes ARGB/compatible format)
            // and does not support scaling (source rect is copied 1:1 to destination position)
            HRESULT hRet = E_FAIL;
            {
                D3D11_TEXTURE2D_DESC texDesc = {};
                texDesc.Width = OverlayWidth;
                texDesc.Height = OverlayHeight;
                texDesc.MipLevels = 1;
                texDesc.ArraySize = 1;
                texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // Matches EMUFMT_A8R8G8B8
                texDesc.SampleDesc.Count = 1;
                texDesc.Usage = D3D11_USAGE_STAGING;
                texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

                ID3D11Texture2D* pStagingTex = nullptr;
                hRet = g_pD3DDevice->CreateTexture2D(&texDesc, nullptr, &pStagingTex);
                if (SUCCEEDED(hRet) && pStagingTex) {
                    D3D11_MAPPED_SUBRESOURCE mapped = {};
                    hRet = g_pD3DDeviceContext->Map(pStagingTex, 0, D3D11_MAP_WRITE, 0, &mapped);
                    if (SUCCEEDED(hRet)) {
                        // Copy overlay data row by row (source pitch may differ from staging pitch)
                        const uint8_t* pSrc = pOverlayData;
                        uint8_t* pDst = (uint8_t*)mapped.pData;
                        UINT copyWidth = OverlayWidth * 4; // 4 bytes per pixel for ARGB
                        for (UINT row = 0; row < OverlayHeight; row++) {
                            memcpy(pDst, pSrc, copyWidth);
                            pSrc += OverlayRowPitch;
                            pDst += mapped.RowPitch;
                        }
                        g_pD3DDeviceContext->Unmap(pStagingTex, 0);

                        // Copy from staging texture to the backbuffer
                        D3D11_BOX srcBox = {};
                        srcBox.left = EmuSourRect.left;
                        srcBox.top = EmuSourRect.top;
                        srcBox.right = EmuSourRect.right;
                        srcBox.bottom = EmuSourRect.bottom;
                        srcBox.front = 0;
                        srcBox.back = 1;
                        g_pD3DDeviceContext->CopySubresourceRegion(
                            pCurrentHostBackBuffer, 0,
                            EmuDestRect.left, EmuDestRect.top, 0,
                            pStagingTex, 0,
                            &srcBox);
                    }
                    pStagingTex->Release();
                }

                if (FAILED(hRet)) {
                    EmuLog(LOG_LEVEL::WARNING, "UpdateOverlay: D3D11 overlay rendering failed : %X", hRet);
                }
            }
#else
            IDirect3DSurface* pTemporaryOverlaySurface;
            HRESULT hRet = g_pD3DDevice->CreateOffscreenPlainSurface(
                OverlayWidth,
                OverlayHeight,
                EMUFMT_A8R8G8B8,
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
#endif
		}

		// Render ImGui
		if (g_renderbase) {
			static std::function<void(ImGuiUI*, IDirect3DSurface*)> internal_render = &CxbxImGui_RenderD3D;
			g_renderbase->Render(internal_render, pCurrentHostBackBuffer);
		}

		pCurrentHostBackBuffer->Release();
	}

	hRet = CxbxPresent();

    // RenderStates need reapplying each frame, but can be re-used between draw calls
    // This forces them to be reset
    XboxRenderStates.SetDirty();

    // Check if we need to enable our frame-limiter
    xbox::dword_xt presentationInverval = g_Xbox_PresentationInterval_Override > 0 ? g_Xbox_PresentationInterval_Override : g_Xbox_PresentationInterval_Default;
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

        // Wait until it's time for the next frame
        auto frameMs = (1000.0 / targetRefreshRate) * multiplier;
        auto targetDuration = std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double, std::milli>(frameMs));
        auto targetTimestamp = frameStartTime + targetDuration;
        SleepPrecise(targetTimestamp);
    }

    frameStartTime = std::chrono::steady_clock::now();

	g_renderbase->UpdateFPSCounter();

	if (Flags == CXBX_SWAP_PRESENT_FORWARD) // Only do this when forwarded from Present
	{
		// TODO: print the primitives per frame with ImGui

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

		if(g_pXbox_SwapCallback != xbox::zeroptr) 
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

xbox::void_xt WINAPI xbox::EMUPATCH(Lock2DSurface)
(
	X_D3DPixelContainer *pPixelContainer,
	X_D3DCUBEMAP_FACES   FaceType,
	uint_xt              Level,
	X_D3DLOCKED_RECT    *pLockedRect,
	X_RECT              *pRect,
	dword_xt             Flags
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
	XB_TRMP(Lock2DSurface)(pPixelContainer, FaceType, Level, pLockedRect, pRect, Flags);

	// Mark the resource as modified
	ForceResourceRehash(pPixelContainer);
}

// ******************************************************************
// * patch: Lock2DSurface_16__LTCG_esi4_eax5
// ******************************************************************
// Overload for logging
static void Lock2DSurface_16__LTCG_esi4_eax5
(
	xbox::X_D3DPixelContainer *pPixelContainer,
	xbox::X_D3DCUBEMAP_FACES   FaceType,
	xbox::uint_xt              Level,
	xbox::X_D3DLOCKED_RECT    *pLockedRect,
	RECT                      *pRect,
	xbox::dword_xt             Flags
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
}

// This uses a custom calling convention where parameter is passed in ESI, EAX
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(Lock2DSurface_16__LTCG_esi4_eax5)
(
	X_D3DPixelContainer *pPixelContainer,
	X_D3DCUBEMAP_FACES   FaceType,
	uint_xt              Level,
	dword_xt             Flags
)
{
	X_D3DLOCKED_RECT *pLockedRect;
	RECT *pRect;
	__asm {
		LTCG_PROLOGUE
		mov  pRect, eax
		mov  pLockedRect, esi
	}

	// Log
	Lock2DSurface_16__LTCG_esi4_eax5(pPixelContainer, FaceType, Level, pLockedRect, pRect, Flags);

	// Pass through to the Xbox implementation of this function
	__asm {
		push Flags
		mov  eax, pRect
		mov  esi, pLockedRect
		push Level
		push FaceType
		push pPixelContainer
		call XB_TRMP(Lock2DSurface_16__LTCG_esi4_eax5)
	}

	// Mark the resource as modified
	ForceResourceRehash(pPixelContainer);

	__asm {
		LTCG_EPILOGUE
		ret  16
	}
}

// ******************************************************************
// * patch: Lock3DSurface
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(Lock3DSurface)
(
	X_D3DPixelContainer *pPixelContainer,
	uint_xt				Level,
	X_D3DLOCKED_BOX		*pLockedVolume,
	X_D3DBOX			*pBox,
	dword_xt			Flags
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
	XB_TRMP(Lock3DSurface)(pPixelContainer, Level, pLockedVolume, pBox, Flags);

	// Mark the resource as modified
	ForceResourceRehash(pPixelContainer);
}

// ******************************************************************
// * patch: Lock3DSurface_16__LTCG_eax4
// ******************************************************************
// Overload for logging
static void Lock3DSurface_16__LTCG_eax4
(
	xbox::X_D3DPixelContainer *pPixelContainer,
	xbox::uint_xt              Level,
	xbox::X_D3DLOCKED_BOX     *pLockedVolume,
	xbox::X_D3DBOX            *pBox,
	xbox::dword_xt             Flags
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pPixelContainer)
		LOG_FUNC_ARG(Level)
		LOG_FUNC_ARG(pLockedVolume)
		LOG_FUNC_ARG(pBox)
		LOG_FUNC_ARG(Flags)
		LOG_FUNC_END;
}

// This uses a custom calling convention where parameters are passed in EAX
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(Lock3DSurface_16__LTCG_eax4)
(
	X_D3DPixelContainer *pPixelContainer,
	uint_xt              Level,
	X_D3DLOCKED_BOX     *pLockedVolume,
	dword_xt             Flags
)
{
	X_D3DBOX *pBox;
	__asm {
		LTCG_PROLOGUE
		mov  pBox, eax
	}

	// Log
	Lock3DSurface_16__LTCG_eax4(pPixelContainer, Level, pLockedVolume, pBox, Flags);


	// Pass through to the Xbox implementation of this function
	__asm {
		push Flags
		mov  eax, pBox
		push pLockedVolume
		push Level
		push pPixelContainer
		call XB_TRMP(Lock3DSurface_16__LTCG_eax4)
	}

	// Mark the resource as modified
	ForceResourceRehash(pPixelContainer);

	__asm {
		LTCG_EPILOGUE
		ret  16
	}
}

// ******************************************************************
// * patch: D3DDevice_PersistDisplay
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_PersistDisplay)()
{
	LOG_FUNC();

	LOG_INCOMPLETE();

	// TODO: This function simply saves a copy of the display to a surface and persists it in contiguous memory
	// This function, if ever required, can be implemented as the following
	// 1. Check for an existing persisted surface via AvGetSavedDataAddress, free it if necessary
	// 2. Create an Xbox format surface with the same size and format as active display
	// 3. Copy the host framebuffer to the xbox surface, converting format if necessary
	// 4. Set the display mode via AvSetDisplayMode to the same format as the persisted surface,
	//    passing the ->Data pointer of the xbox surface as the framebuffer pointer.
	// 5. Use MmPersistContigousMemory to persist the surface data across reboot
	// 6. Call AvSetSavedDataAddress, passing the xbox surface data pointer

	// Call the native Xbox function so that AvSetSavedDataAddress is called and the VMManager can know its correct address
	if (XB_TRMP(D3DDevice_PersistDisplay)) {
		return XB_TRMP(D3DDevice_PersistDisplay)();
	}
	return 0;
}

