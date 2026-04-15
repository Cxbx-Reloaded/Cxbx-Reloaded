
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetIndices)
(
	X_D3DIndexBuffer      *pIndexData,
	uint_xt                BaseVertexIndex
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pIndexData)
		LOG_FUNC_ARG(BaseVertexIndex)
		LOG_FUNC_END;

	// Cache the base vertex index then call the Xbox function
	g_Xbox_BaseVertexIndex = BaseVertexIndex;

	XB_TRMP(D3DDevice_SetIndices)(pIndexData, BaseVertexIndex);
}

// ******************************************************************
// * patch: Direct3D_CreateDevice
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(Direct3D_CreateDevice)
(
    uint_xt                     Adapter,
    X_D3DDEVTYPE                DeviceType,
	X_HWND                      hFocusWindow,
    dword_xt                    BehaviorFlags,
    X_D3DPRESENT_PARAMETERS    *pPresentationParameters,
    X_D3DDevice               **ppReturnedDeviceInterface
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Adapter)
		LOG_FUNC_ARG(DeviceType)
		LOG_FUNC_ARG(hFocusWindow)
		LOG_FUNC_ARG(BehaviorFlags)
		LOG_FUNC_ARG(pPresentationParameters)
		LOG_FUNC_ARG_OUT(ppReturnedDeviceInterface)
		LOG_FUNC_END;

	Direct3D_CreateDevice_Start(pPresentationParameters);

	// HACK: Disable Software VertexProcessing (Fixes CreateDevice failure in Chihiro titles)
	if (BehaviorFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING) {
		BehaviorFlags &= ~D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		BehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}

	// Only then call Xbox CreateDevice function
	hresult_xt hRet = XB_TRMP(Direct3D_CreateDevice)(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

	Direct3D_CreateDevice_End(pPresentationParameters);

	return hRet;
}

// ******************************************************************
// * patch: D3DDevice_Reset
// ******************************************************************

static void CxbxImpl_Reset(xbox::X_D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	// Unlike the host version of Reset, The Xbox version does not actually reset the entire device
	// Instead, it simply re-creates the backbuffer with a new configuration

	// Store the new multisampling configuration
	SetXboxMultiSampleType(pPresentationParameters->MultiSampleType);

	// Update scaling aspect ratio
	SetAspectRatioScale(pPresentationParameters);

	// Since Reset will call create a new backbuffer surface, we can clear our current association
	// NOTE: We don't actually free the Xbox data, the Xbox side will do this for us when we call the trampoline below.
	// We must not reset the values to nullptr, since the XDK will re-use the same addresses for the data headers
	// (they are members of the Direct3DDevice object). if we overwrite then, the reference to the xbox backbuffer will be lost
	// and we'll get a black screen.
	FreeHostResource(GetHostResourceKey(g_pXbox_BackBufferSurface));
	FreeHostResource(GetHostResourceKey(g_pXbox_DefaultDepthStencilSurface));

	// Below requirement for patched function(s) in order to function properly.
	// Perform xbox's D3DDevice_Reset call.

	// Perform CxbxImpl_SetRenderTarget call.
}

xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_Reset)
(
	X_D3DPRESENT_PARAMETERS* pPresentationParameters
)
{
	LOG_FUNC_ONE_ARG(pPresentationParameters);

	CxbxImpl_Reset(pPresentationParameters);

	// Call the Xbox Reset function to do the rest of the work for us
	hresult_xt hRet = XB_TRMP(D3DDevice_Reset)(pPresentationParameters);

	// Refresh the current render target and depth stencil, to apply changes made within D3DDevice_Reset
	// Some XDKs do this for us, but not all do!
	CxbxImpl_SetRenderTarget(g_pXbox_RenderTarget, g_pXbox_DepthStencil);

	return hRet;
}

static void D3DDevice_Reset_0__LTCG_edi1(xbox::X_D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	LOG_FUNC_ONE_ARG(pPresentationParameters);
}

__declspec(naked) xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_Reset_0__LTCG_edi1)()
{
	X_D3DPRESENT_PARAMETERS* pPresentationParameters;
	__asm {
		LTCG_PROLOGUE
		mov  pPresentationParameters, edi
	}

	// Log
	D3DDevice_Reset_0__LTCG_edi1(pPresentationParameters);

	CxbxImpl_Reset(pPresentationParameters);

	// Call the Xbox version of DestroyResource
	__asm {
		mov  edi, pPresentationParameters
		call XB_TRMP(D3DDevice_Reset_0__LTCG_edi1)
	}

	CxbxImpl_SetRenderTarget(g_pXbox_RenderTarget, g_pXbox_DepthStencil);

	__asm {
		LTCG_EPILOGUE
		ret
	}
}

static void D3DDevice_Reset_0__LTCG_ebx1(xbox::X_D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	LOG_FUNC_ONE_ARG(pPresentationParameters);
}

__declspec(naked) xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_Reset_0__LTCG_ebx1)()
{
	X_D3DPRESENT_PARAMETERS* pPresentationParameters;
	__asm {
		LTCG_PROLOGUE
		mov  pPresentationParameters, ebx
	}

	// Log
	D3DDevice_Reset_0__LTCG_ebx1(pPresentationParameters);

	CxbxImpl_Reset(pPresentationParameters);

	// Call the Xbox version of DestroyResource
	__asm {
		mov  ebx, pPresentationParameters
		call XB_TRMP(D3DDevice_Reset_0__LTCG_ebx1)
	}

	CxbxImpl_SetRenderTarget(g_pXbox_RenderTarget, g_pXbox_DepthStencil);

	__asm {
		LTCG_EPILOGUE
		ret
	}
}


// ******************************************************************
// * patch: D3DDevice_GetDisplayFieldStatus
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_GetDisplayFieldStatus)(X_D3DFIELD_STATUS *pFieldStatus)
{
	// NOTE: This can be unpatched only when NV2A does it's own VBlank and HLE _Swap function is unpatched


	LOG_FUNC_ONE_ARG(pFieldStatus);

	// Read AV Flags to determine if Progressive or Interlaced
	// The xbox does this by reading from pDevice->m_Miniport.m_CurrentAvInfo
	// but we don't have an OOVPA for that. Instead, we call the Xbox implementation of 
	// D3DDevice_GetDisplayMode and read the result

	X_D3DDISPLAYMODE displayMode;

	// If we can find the Xbox version of GetDisplayMode, use the real data returned, otherwise
	// use a sensible default
	if (XB_TRMP(D3DDevice_GetDisplayMode) != nullptr) {
		XB_TRMP(D3DDevice_GetDisplayMode)(&displayMode);
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
// * patch: D3DDevice_BeginPush_4
// TODO: Find a test case and verify this
// Starting from XDK 4531, this changed to 1 parameter only.
// Is this definition incorrect, or did it change at some point?
// ******************************************************************
xbox::PDWORD WINAPI xbox::EMUPATCH(D3DDevice_BeginPush_4)(dword_xt Count)
{
	LOG_FUNC_ONE_ARG(Count);

	if (g_pXbox_BeginPush_Buffer != nullptr)
	{
		EmuLog(LOG_LEVEL::WARNING, "D3DDevice_BeginPush called without D3DDevice_EndPush in between?!");
		delete[] g_pXbox_BeginPush_Buffer; // prevent a memory leak
	}

	dword_xt *pRet = new dword_xt[Count];

    g_pXbox_BeginPush_Buffer = pRet;

    return pRet;
}

// ******************************************************************
// * patch: D3DDevice_BeginPush_8
// TODO: Find a test case and verify this: RalliSport Challenge XDK 4134
// For XDK before 4531
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_BeginPush_8)(dword_xt Count, dword_xt** ppPush)
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

	dword_xt *pRet = new dword_xt[Count];

	g_pXbox_BeginPush_Buffer = pRet;

	*ppPush=pRet;
}

// ******************************************************************
// * patch: D3DDevice_EndPush
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_EndPush)(dword_xt *pPush)
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
