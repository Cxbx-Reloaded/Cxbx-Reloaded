
void CxbxUpdateHostTextures()
{
	LOG_INIT; // Allows use of DEBUG_D3DRESULT

	// Set the host texture for each stage
	for (int stage = 0; stage < xbox::X_D3DTS_STAGECOUNT; stage++) {
		auto pXboxBaseTexture = g_pXbox_SetTexture[stage];
		IDirect3DBaseTexture* pHostBaseTexture = nullptr;
		bool bNeedRelease = false;
		if (pXboxBaseTexture != xbox::zeroptr) {
			DWORD XboxResourceType = GetXboxCommonResourceType(pXboxBaseTexture);
			switch (XboxResourceType) {
			case X_D3DCOMMON_TYPE_TEXTURE:
				pHostBaseTexture = GetHostBaseTexture(pXboxBaseTexture, /*D3DUsage=*/0, stage);
				break;
			case X_D3DCOMMON_TYPE_SURFACE:
				// Surfaces can be set in the texture stages, instead of textures
				LOG_TEST_CASE("ActiveTexture set to a surface (non-texture) resource"); // Test cases : Burnout, Outrun 2006
				// We must wrap the surface before using it as a texture
				pHostBaseTexture = CxbxConvertXboxSurfaceToHostTexture(pXboxBaseTexture);
				// Release this texture (after SetTexture) when we succeeded in creating it :
				bNeedRelease = pHostBaseTexture != nullptr;
				break;
			default:
				LOG_TEST_CASE("ActiveTexture set to an unhandled resource type!");
				break;
			}

			// Read HostFormat from GetResourceCache :
			// TODO : Optimize this, as we're doing the lookup twice (once in GetHostBaseTexture, once here)
			auto key = GetHostResourceKey(pXboxBaseTexture, stage);
			auto& ResourceCache = GetResourceCache(key);
			auto it = ResourceCache.find(key);
			if (it != ResourceCache.end()) {
				g_HostTextureFormats[stage] = it->second.HostFormat;
			}
		}

#ifdef CXBX_USE_D3D11
		if (pHostBaseTexture != nullptr) {
			// Create a shader resource view for the texture
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			D3D11_RESOURCE_DIMENSION dim;
			pHostBaseTexture->GetType(&dim);

			switch (dim) {
			case D3D11_RESOURCE_DIMENSION_TEXTURE2D: {
				D3D11_TEXTURE2D_DESC texDesc = {};
				((ID3D11Texture2D*)pHostBaseTexture)->GetDesc(&texDesc);
				srvDesc.Format = texDesc.Format;
				if (texDesc.ArraySize == 6) {
					srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
					srvDesc.TextureCube.MipLevels = texDesc.MipLevels;
					srvDesc.TextureCube.MostDetailedMip = 0;
				} else {
					srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
					srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
					srvDesc.Texture2D.MostDetailedMip = 0;
				}
				break;
			}
			case D3D11_RESOURCE_DIMENSION_TEXTURE3D: {
				D3D11_TEXTURE3D_DESC texDesc = {};
				((ID3D11Texture3D*)pHostBaseTexture)->GetDesc(&texDesc);
				srvDesc.Format = texDesc.Format;
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
				srvDesc.Texture3D.MipLevels = texDesc.MipLevels;
				srvDesc.Texture3D.MostDetailedMip = 0;
				break;
			}
			default:
				// Unsupported resource type
				if (bNeedRelease) pHostBaseTexture->Release();
				continue;
			}

			ID3D11ShaderResourceView* pSRV = nullptr;
			HRESULT hRet = g_pD3DDevice->CreateShaderResourceView(pHostBaseTexture, &srvDesc, &pSRV);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateShaderResourceView");
			if (SUCCEEDED(hRet) && pSRV != nullptr) {
				g_pD3DDeviceContext->PSSetShaderResources(stage, 1, &pSRV);
				pSRV->Release();
			}
		} else {
			ID3D11ShaderResourceView* pNullSRV = nullptr;
			g_pD3DDeviceContext->PSSetShaderResources(stage, 1, &pNullSRV);
		}
#else
		HRESULT hRet = g_pD3DDevice->SetTexture(stage, pHostBaseTexture);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetTexture");
#endif
		if (bNeedRelease) {
			pHostBaseTexture->Release();
		}
	}
}

void CxbxUpdateHostTextureScaling()
{
	// Xbox works with "Linear" and "Swizzled" texture formats
	// Linear formats are not addressed with normalized coordinates (similar to https://www.khronos.org/opengl/wiki/Rectangle_Texture?)
	// We want to use normalized coordinates in our shaders, so need to be able to scale the coordinates back
	// Note texcoords aren't only used for texture lookups
	// TODO store scaling per texture instead of per stage, and scale during lookup in the pixel shader

	// Each texture stage has one texture coordinate set associated with it
	// We'll store scale factors for each texture coordinate set
	std::array<std::array<float, 4>, xbox::X_D3DTS_STAGECOUNT> texcoordScales;
	texcoordScales.fill({ 1, 1, 1, 1 });

	for (int stage = 0; stage < xbox::X_D3DTS_STAGECOUNT; stage++) {
		auto pXboxBaseTexture = g_pXbox_SetTexture[stage];

		// No texture, no scaling to do
		if (pXboxBaseTexture == xbox::zeroptr) {
			continue;
		}

		// Texcoord index. Just the texture stage unless fixed function mode
		int texCoordIndex = stage;
		if (g_Xbox_VertexShaderMode == VertexShaderMode::FixedFunction) {
			// Get TEXCOORDINDEX for the current texture stage's state
			// Stores both the texture stage index and information for generating coordinates
			// See D3DTSS_TEXCOORDINDEX
			auto texCoordIndexState = XboxTextureStates.Get(stage, xbox::X_D3DTSS_TEXCOORDINDEX);

			// If coordinates are generated, we don't have to worry about the coordinates coming from the title
			bool isGenerated = texCoordIndexState >= X_D3DTSS_TCI_CAMERASPACENORMAL;
			if (isGenerated) {
				continue;
			}

			// Determine the texture coordinate addressing this texture stage
			texCoordIndex = (texCoordIndexState & 0x3); // 0 - 3
		}

		auto texCoordScale = &texcoordScales[texCoordIndex];

		// Check for active linear textures.
		xbox::X_D3DFORMAT XboxFormat = GetXboxPixelContainerFormat(pXboxBaseTexture);
		if (EmuXBFormatIsLinear(XboxFormat)) {
			// Test-case : This is often hit by the help screen in XDK samples.
			// Set scaling factor for this texture, which will be applied to
			// all texture-coordinates in CxbxVertexShaderTemplate.hlsl
			// Note : Linear textures are two-dimensional at most (right?)
			float width, height;
			if ((xbox::X_D3DSurface*)pXboxBaseTexture == g_pXbox_BackBufferSurface) {
				// Account for MSAA
				// Test case: Max Payne 2 (bullet time)
				GetBackBufferPixelDimensions(width, height);
			}
			else {
				width = (float)GetPixelContainerWidth(pXboxBaseTexture);
				height = (float)GetPixelContainerHeight(pXboxBaseTexture);
			}

			*texCoordScale = {
				width,
				height,
				1.0f, // TODO should this be mip levels for volume textures?
				1.0f
			};
		}

		// When a depth buffer is used as a texture
		// We do 'Native Shadow Mapping'
		// https://aras-p.info/texts/D3D9GPUHacks.html
		// The z texture coordinate component holds a depth value, which needs to be normalized
		// TODO implement handling for
		// - X_D3DRS_SHADOWFUNC
		// - X_D3DRS_POLYGONOFFSETZSLOPESCALE
		// - X_D3DRS_POLYGONOFFSETZOFFSET
		if (EmuXBFormatIsDepthBuffer(XboxFormat)) {
			(*texCoordScale)[2] = (float)GetZScaleForPixelContainer(pXboxBaseTexture);
		}
	}
	// Pass above determined texture scaling factors to our HLSL shader.
	// Note : CxbxVertexShaderTemplate.hlsl applies texture scaling on
	// output registers oT0 to oT3. It may be needed to move the scaling
	// and apply it on input registers instead. In that case, we'd have to
	// figure out which registers are used to pass texture-coordinates into
	// the shader and allow scaling on any of the input registers (so we'd
	// need to allow scaling on all 16 attributes, instead of just the four
	// textures like we do right now).
	CxbxSetVertexShaderConstantF(CXBX_D3DVS_TEXTURES_SCALE_BASE, (float*)texcoordScales.data(), CXBX_D3DVS_TEXTURES_SCALE_SIZE);
}

void CxbxUpdateDirtyVertexShaderConstants(const float* constants, bool* dirty) {
	// Reduce the number of calls by updating contiguous "batches" of dirty states
	int batchStartIndex = -1; // -1 means we aren't in a batch

	for (int i = 0; i < X_D3DVS_CONSTREG_COUNT; i++) {
		if (batchStartIndex == -1 && dirty[i]) {
			batchStartIndex = i; // Start a batch
		}
		else if (batchStartIndex != -1 && !dirty[i]) {
			// Finish the batch
			int count = i - batchStartIndex;
			CxbxSetVertexShaderConstantF(batchStartIndex, &constants[batchStartIndex * 4], count);
			batchStartIndex = -1;
		}

		// Constant is no longer dirty
		dirty[i] = false;
	}

	// Send the final batch
	if (batchStartIndex != -1) {
		int count = X_D3DVS_CONSTREG_COUNT - batchStartIndex + 1;
		CxbxSetVertexShaderConstantF(batchStartIndex, &constants[batchStartIndex * 4], count);
	}
}

extern float* HLE_get_NV2A_vertex_constant_float4_ptr(unsigned const_index); // TMP glue
// TODO : Once we're able to flush the NV2A push buffer
// remove our patches on D3DDevice_SetVertexShaderConstant (and CxbxImpl_SetVertexShaderConstant)
void CxbxUpdateHostVertexShaderConstants()
{
	// For Xbox vertex shader programs, the Xbox vertex shader constants
	// are mirrored on the host.
	// Otherwise, the same set of constants is used for the fixed function vertex shader
	// implementation instead

	// Track which constants are currently written
	// So we can skip updates
	static bool isXboxConstants = false;

	if (g_Xbox_VertexShaderMode == VertexShaderMode::FixedFunction) {
		// Write host FF shader state
		// TODO dirty tracking like for Xbox constants?
		UpdateFixedFunctionVertexShaderState();
		isXboxConstants = false;
	}
	else {
		// Write Xbox constants
		auto pg = &(g_NV2A->GetDeviceState()->pgraph);
		auto constant_floats = (float*)pg->vsh_constants;

		if (isXboxConstants) {
			// Only need to overwrite what's changed
			CxbxUpdateDirtyVertexShaderConstants(constant_floats, pg->vsh_constants_dirty);
		}
		else {
			// We need to update everything
			CxbxSetVertexShaderConstantF(0, constant_floats, X_D3DVS_CONSTREG_COUNT);
		}

		// We've written the Xbox constants
		isXboxConstants = true;

		// FIXME our viewport constants don't match Xbox values
		// If we write them to pgraph constants, like we do with constants set by the title,
		// the Xbox could overwrite them (at any time?) and we get flickering geometry.
		// For now, set our viewport constants directly in the call below,
		// overwriting whatever was in pgraph
		// Test case:
		// Xbox dashboard (during initial fade from black)
		// Need for Speed: Hot Pursuit 2 (car select)
		CxbxUpdateHostViewPortOffsetAndScaleConstants();
	}

	// Placed this here until we find a better place
	const float fogTableMode = static_cast<float>(XboxRenderStates.GetXboxRenderState(xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGTABLEMODE));
	const float fogDensity = XboxRenderStates.GetXboxRenderStateAsFloat(xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGDENSITY);
	const float fogStart = XboxRenderStates.GetXboxRenderStateAsFloat(xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGSTART);
	const float fogEnd = XboxRenderStates.GetXboxRenderStateAsFloat(xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGEND);
	float fogStuff[4] = {fogTableMode, fogDensity, fogStart, fogEnd};
	CxbxSetVertexShaderConstantF(CXBX_D3DVS_CONSTREG_FOGINFO, fogStuff, 1);
}

void CxbxUpdateHostViewport() {
	// We don't have a fixed function shader so we rely on D3D9 fixed function mode
	// So we have to set a viewport based on the current Xbox viewport
	// Antialiasing mode affects the viewport offset and scale
	float aaScaleX, aaScaleY;
	float aaOffsetX, aaOffsetY;
	GetMultiSampleScaleRaw(aaScaleX, aaScaleY);
	GetMultiSampleOffset(aaOffsetX, aaOffsetY);

	DWORD HostRenderTarget_Width, HostRenderTarget_Height;
	if (!GetHostRenderTargetDimensions(&HostRenderTarget_Width, &HostRenderTarget_Height)) {
		LOG_TEST_CASE("Could not get rendertarget dimensions while setting the viewport");
	}

	float Xscale = aaScaleX * g_RenderUpscaleFactor;
	float Yscale = aaScaleY * g_RenderUpscaleFactor;

	if (g_Xbox_VertexShaderMode == VertexShaderMode::FixedFunction) {
		// Set viewport
		D3DVIEWPORT hostViewport;
		hostViewport._9_11(X, TopLeftX) = g_Xbox_Viewport.X * Xscale;
		hostViewport._9_11(Y, TopLeftY) = g_Xbox_Viewport.Y * Yscale;
		hostViewport.Width = g_Xbox_Viewport.Width * Xscale;
		hostViewport.Height = g_Xbox_Viewport.Height * Yscale;
		hostViewport._9_11(MinZ, MinDepth) = g_Xbox_Viewport.MinZ; // ?? * Zscale;
		hostViewport._9_11(MaxZ, MaxDepth) = g_Xbox_Viewport.MaxZ; // ?? * Zscale;
		CxbxSetViewport(&hostViewport);

		// Reset scissor rect
		RECT viewportRect;
		viewportRect.left = 0;
		viewportRect.top = 0;
		viewportRect.right = HostRenderTarget_Width;
		viewportRect.bottom = HostRenderTarget_Height;
		CxbxSetScissorRect(&viewportRect);
	}
	else {
		// Set default viewport over the whole screen
		// And let the vertex shader take care of vertex placement
		// So we can handle shaders that don't use the Xbox viewport constants and don't align
		// with the currently set viewport
		// Test case: ???

		D3DVIEWPORT hostViewport;
		hostViewport._9_11(X, TopLeftX) = 0;
		hostViewport._9_11(Y, TopLeftY) = 0;
		hostViewport.Width = static_cast<float>(HostRenderTarget_Width);
		hostViewport.Height = static_cast<float>(HostRenderTarget_Height);
		hostViewport._9_11(MinZ, MinDepth) = 0.0f;
		hostViewport._9_11(MaxZ, MaxDepth) = 1.0f;

		CxbxSetViewport(&hostViewport);

		// We still need to clip to the viewport
		// Scissor to viewport
#ifdef CXBX_USE_D3D11
		g_D3D11RasterizerDesc.ScissorEnable = TRUE;
		g_bD3D11RasterizerStateDirty = true;
#else
		g_pD3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
#endif
		RECT viewportRect;
		viewportRect.left = static_cast<LONG>(g_Xbox_Viewport.X * Xscale);
		viewportRect.top = static_cast<LONG>(g_Xbox_Viewport.Y * Yscale);
		viewportRect.right = static_cast<LONG>(viewportRect.left + (g_Xbox_Viewport.Width * Xscale));
		viewportRect.bottom = static_cast<LONG>(viewportRect.top + (g_Xbox_Viewport.Height * Yscale));
		CxbxSetScissorRect(&viewportRect);
	}
}

extern void CxbxUpdateHostVertexDeclaration(); // TMP glue
extern void CxbxUpdateHostVertexShader(); // TMP glue

void CxbxUpdateNativeD3DResources()
{
	// Before we start, make sure our resource cache stays limited in size
	PrunePaletizedTexturesCache(); // TODO : Could we move this to Swap instead?

	// NOTE: Vertex shader must be updated before vertex declaration,
	// because D3D11 input layout creation depends on compiled VS bytecode
	CxbxUpdateHostVertexShader();

	CxbxUpdateHostVertexDeclaration();

	CxbxUpdateHostVertexShaderConstants();

	CxbxUpdateHostViewport();

	// NOTE: Order is important here
    // Some Texture States depend on RenderState values (Point Sprites)
    // And some Pixel Shaders depend on Texture State values (BumpEnvMat, etc)
	CxbxUpdateHostTextures();
	CxbxUpdateHostTextureScaling();
    XboxRenderStates.Apply();
    XboxTextureStates.Apply();

    // If Pixel Shaders are not disabled, process them
    if (!g_DisablePixelShaders) {
        DxbxUpdateActivePixelShader();
    }

#ifdef CXBX_USE_D3D11
	// Apply any pending D3D11 state object changes before drawing
	CxbxD3D11ApplyDirtyStates();
#endif


/* TODO : Port these :
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

	// Add an end marker to the command buffer queue.
	// This, so that the next GetData will always have at least one
	// final query event to flush out, after which GPU will be done.
	CxbxQueryIssueEnd(g_pHostQueryWaitForIdle);

	// Empty the command buffer and wait until host GPU is idle.
#ifdef CXBX_USE_D3D11
	BOOL queryData = FALSE;
	while (CxbxQueryGetData(g_pHostQueryWaitForIdle, &queryData, sizeof(queryData), 0) == S_FALSE)
		CxbxCPUIdleWait();
#else
	while (S_FALSE == CxbxQueryGetData(g_pHostQueryWaitForIdle, nullptr, 0, D3DGETDATA_FLUSH))
		CxbxCPUIdleWait();
#endif

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
#ifdef CXBX_USE_D3D11
		BOOL queryData = FALSE;
		if (S_FALSE == CxbxQueryGetData(g_pHostQueryCallbackEvent, &queryData, sizeof(queryData), 0)) {
#else
		if (S_FALSE == CxbxQueryGetData(g_pHostQueryCallbackEvent, nullptr, 0, 0)) {
#endif
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
		if (XboxCallback.Type == xbox::X_D3DCALLBACK_WRITE) {
			// Write callbacks should wait until GPU is idle
			if (!CxbxFlushHostGPU()) {
				// Host GPU can't be flushed. In the old behaviour, we made the callback anyway
				// TODO : Should we keep doing that?
			}
		} else {
			assert(XboxCallback.Type == xbox::X_D3DCALLBACK_READ);
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
	xbox::X_D3DCALLBACKTYPE	Type,
	xbox::X_D3DCALLBACK		pCallback,
	xbox::dword_xt				Context
)
{
	if (Type > xbox::X_D3DCALLBACK_WRITE) {
		LOG_TEST_CASE("Illegal callback type!");
		return;
	}

	if (pCallback == xbox::zeroptr) {
		LOG_TEST_CASE("pCallback == xbox::zeroptr!");
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
		CxbxQueryIssueEnd(g_pHostQueryCallbackEvent);
	}
}

xbox::void_xt CxbxImpl_SetPixelShader(xbox::dword_xt Handle)
{
    // Cache the active shader handle
    g_pXbox_PixelShader = (xbox::X_PixelShader*)Handle;

    // Copy the Pixel Shader data to our RenderState handler (this includes values for pixel shader constants)
    // This mirrors the fact that unpatched SetPixelShader does the same thing!
    // This shouldn't be necessary anymore, but shaders still break if we don't do this
	// This breakage might be caused by our push-buffer processing could be "trailing behind" what our patches do;
	// By writing to render state during this patch, we avoid missing out on updates that push buffer commands would perform.
	// However, any updates that occur mid-way can overwrite what we store here, and still cause problems!
	// The only viable solution for that would be to draw entirely based on push-buffer handling (which might require removing possibly all D3D patches!)
    if (g_pXbox_PixelShader != nullptr) {
        // TODO : If D3DDevice_SetPixelShader() in XDKs don't overwrite the X_D3DRS_PS_RESERVED slot with PSDef.PSTextureModes,
        // store it here and restore after memcpy, or alternatively, perform two separate memcpy's (the halves before, and after the reserved slot).
        memcpy(XboxRenderStates.GetPixelShaderRenderStatePointer(), g_pXbox_PixelShader->pPSDef, sizeof(xbox::X_D3DPIXELSHADERDEF) - 3 * sizeof(DWORD));
        // Copy the PSDef.PSTextureModes field to it's dedicated slot, which lies outside the range of PixelShader render state slots
        // Note : This seems to be what XDK's do as well. Needs verification.
        XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_PSTEXTUREMODES, g_pXbox_PixelShader->pPSDef->PSTextureModes);
    }
}

// Overload for logging
static void D3DDevice_SetPixelShader_0__LTCG_eax1
(
    xbox::dword_xt      Handle
)
{
    LOG_FUNC_ONE_ARG(Handle);
}

// LTCG specific D3DDevice_SetPixelShader function...
// This uses a custom calling convention where parameter is passed in EAX
// Test-case: Metal Wolf Chaos
// Test-case: Lord of the Rings: The Third Age
// Test-case: Midtown Madness 3
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetPixelShader_0__LTCG_eax1)()
{
    dword_xt Handle;
    __asm {
        LTCG_PROLOGUE
        mov  Handle, eax
    }

    // Log
    D3DDevice_SetPixelShader_0__LTCG_eax1(Handle);

    __asm {
        mov  eax, Handle
        call XB_TRMP(D3DDevice_SetPixelShader_0__LTCG_eax1)
    }

    CxbxImpl_SetPixelShader(Handle);

    __asm {
        LTCG_EPILOGUE
        ret
    }
}

// ******************************************************************
// * patch: D3DDevice_SetPixelShader
// ******************************************************************
