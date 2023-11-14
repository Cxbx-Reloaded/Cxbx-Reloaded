namespace Logging // Note : For ease of maintenance, sort below functions
{
	static void CDevice_SetStateUP
	(
		xbox::addr_xt _this
	)
	{
		LOG_FUNC_ONE_ARG(_this);
	}

	static void CDevice_SetStateVB
	(
		xbox::addr_xt _this,
		xbox::ulong_xt Unknown1
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(_this)
			LOG_FUNC_ARG(Unknown1)
			LOG_FUNC_END;
	}

	static void D3DDevice_ApplyStateBlock
	(
		xbox::dword_xt Token
	)
	{
		LOG_FUNC_ONE_ARG(Token);
	}

	static void D3DDevice_Begin
	(
		xbox::X_D3DPRIMITIVETYPE PrimitiveType
	)
	{
		LOG_FUNC_ONE_ARG(PrimitiveType);
	}

	static void D3DDevice_BeginPush_4
	(
		xbox::dword_xt Count
	)
	{
		LOG_FUNC_ONE_ARG(Count);
	}

	static void D3DDevice_BeginPush_8
	(
		xbox::dword_xt Count,
		xbox::dword_xt** ppPush
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Count)
			LOG_FUNC_ARG(ppPush)
			LOG_FUNC_END;
	}

	static void D3DDevice_BeginPushBuffer
	(
		xbox::dword_xt* pPush
	)
	{
		LOG_FUNC_ONE_ARG(pPush);
	}

	static void D3DDevice_BeginVisibilityTest()
	{
		LOG_FUNC();
	}

	static void D3DDevice_BlockOnFence
	(
		xbox::dword_xt Fence
	)
	{
		LOG_FUNC_ONE_ARG(Fence);
	}

	static void D3DDevice_BlockUntilVerticalBlank()
	{
		LOG_FUNC();
	}

	static void D3DDevice_Clear
	(
		xbox::dword_xt Count,
		CONST D3DRECT* pRects,
		xbox::dword_xt Flags,
		D3DCOLOR Color,
		float Z,
		xbox::dword_xt Stencil
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
	}

	static void D3DDevice_CopyRects
	(
		xbox::X_D3DSurface* pSourceSurface,
		CONST RECT* pSourceRectsArray,
		xbox::uint_xt cRects,
		xbox::X_D3DSurface* pDestinationSurface,
		CONST POINT* pDestPointsArray
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(pSourceSurface)
			LOG_FUNC_ARG(pSourceRectsArray)
			LOG_FUNC_ARG(cRects)
			LOG_FUNC_ARG(pDestinationSurface)
			LOG_FUNC_ARG(pDestPointsArray)
			LOG_FUNC_END;
	}

	static void D3DDevice_DeleteVertexShader
	(
		xbox::dword_xt Handle
	)
	{
		LOG_FUNC_ONE_ARG(Handle);
	}

	static void D3DDevice_DeleteVertexShader_0
	(
		xbox::dword_xt Handle
	)
	{
		LOG_FUNC_ONE_ARG(Handle);
	}

	static void D3DDevice_DrawIndexedVertices
	(
		xbox::X_D3DPRIMITIVETYPE PrimitiveType,
		xbox::uint_xt VertexCount,
		CONST PWORD pIndexData
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(PrimitiveType)
			LOG_FUNC_ARG(VertexCount)
			LOG_FUNC_ARG(pIndexData)
			LOG_FUNC_END;
	}

	static void D3DDevice_DrawIndexedVerticesUP
	(
		xbox::X_D3DPRIMITIVETYPE PrimitiveType,
		xbox::uint_xt VertexCount,
		CONST PVOID pIndexData,
		CONST PVOID pVertexStreamZeroData,
		xbox::uint_xt VertexStreamZeroStride
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(PrimitiveType)
			LOG_FUNC_ARG(VertexCount)
			LOG_FUNC_ARG(pIndexData)
			LOG_FUNC_ARG(pVertexStreamZeroData)
			LOG_FUNC_ARG(VertexStreamZeroStride)
			LOG_FUNC_END;
	}

	static void D3DDevice_DrawRectPatch
	(
		xbox::uint_xt Handle,
		CONST xbox::float_xt* pNumSegs,
		CONST D3DRECTPATCH_INFO* pRectPatchInfo
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Handle)
			LOG_FUNC_ARG(pNumSegs)
			LOG_FUNC_ARG(pRectPatchInfo)
			LOG_FUNC_END;
	}

	static void D3DDevice_DrawTriPatch
	(
		xbox::uint_xt Handle,
		CONST xbox::float_xt* pNumSegs,
		CONST D3DTRIPATCH_INFO* pTriPatchInfo
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Handle)
			LOG_FUNC_ARG(pNumSegs)
			LOG_FUNC_ARG(pTriPatchInfo)
			LOG_FUNC_END;
	}

	static void D3DDevice_DrawVertices
	(
		xbox::X_D3DPRIMITIVETYPE PrimitiveType,
		xbox::uint_xt StartVertex,
		xbox::uint_xt VertexCount
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(PrimitiveType)
			LOG_FUNC_ARG(StartVertex)
			LOG_FUNC_ARG(VertexCount)
			LOG_FUNC_END;
	}

	static void D3DDevice_DrawVerticesUP
	(
		xbox::X_D3DPRIMITIVETYPE PrimitiveType,
		xbox::uint_xt VertexCount,
		CONST xbox::PVOID pVertexStreamZeroData,
		xbox::uint_xt VertexStreamZeroStride
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(PrimitiveType)
			LOG_FUNC_ARG(VertexCount)
			LOG_FUNC_ARG(pVertexStreamZeroData)
			LOG_FUNC_ARG(VertexStreamZeroStride)
			LOG_FUNC_END;
	}

	static void D3DDevice_EnableOverlay
	(
		xbox::bool_xt Enable
	)
	{
		LOG_FUNC_ONE_ARG(Enable);
	}

	static void D3DDevice_End()
	{
		LOG_FUNC();
	}

	static void D3DDevice_EndPush
	(
		xbox::dword_xt* pPush
	)
	{
		LOG_FUNC_ONE_ARG(pPush);
	}

	static void D3DDevice_EndVisibilityTest
	(
		xbox::dword_xt Index
	)
	{
		LOG_FUNC_ONE_ARG(Index);
	}

	static void D3DDevice_FlushVertexCache()
	{
		LOG_FUNC();
	}

	static void D3DDevice_GetBackBuffer
	(
		xbox::int_xt BackBuffer,
		D3DBACKBUFFER_TYPE Type,
		xbox::X_D3DSurface** ppBackBuffer
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(BackBuffer)
			LOG_FUNC_ARG(Type)
			LOG_FUNC_ARG(ppBackBuffer)
			LOG_FUNC_END;
	}

	static void D3DDevice_GetBackBuffer2
	(
		xbox::int_xt BackBuffer
	)
	{
		LOG_FUNC_ONE_ARG(BackBuffer);
	}

	static void D3DDevice_GetDisplayFieldStatus
	(
		xbox::X_D3DFIELD_STATUS* pFieldStatus
	)
	{
		LOG_FUNC_ONE_ARG(pFieldStatus);
	}

	static void D3DDevice_GetGammaRamp
	(
		xbox::X_D3DGAMMARAMP* pRamp
	)
	{
		LOG_FUNC_ONE_ARG(pRamp);
	}

	static void D3DDevice_GetMaterial
	(
		xbox::X_D3DMATERIAL8* pMaterial
	)
	{
		LOG_FUNC_ONE_ARG(pMaterial);
	}

	static void D3DDevice_GetModelView(D3DXMATRIX* pModelView)
	{
		LOG_FUNC_ONE_ARG(pModelView);
	}

	static void D3DDevice_GetOverlayUpdateStatus()
	{
		LOG_FUNC();
	}

	static void D3DDevice_GetProjectionViewportMatrix
	(
		D3DXMATRIX* pProjectionViewport
	)
	{
		LOG_FUNC_ONE_ARG(pProjectionViewport);
	}

	static void D3DDevice_GetShaderConstantMode
	(
		xbox::dword_xt* pMode
	)
	{
		LOG_FUNC_ONE_ARG(pMode);
	}

	static void D3DDevice_GetVertexShader
	(
		xbox::dword_xt* pHandle
	)
	{
		LOG_FUNC_ONE_ARG(pHandle);
	}

	static void D3DDevice_GetVertexShaderConstant
	(
		xbox::int_xt Register,
		void* pConstantData,
		xbox::dword_xt ConstantCount
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Register)
			LOG_FUNC_ARG(pConstantData)
			LOG_FUNC_ARG(ConstantCount)
			LOG_FUNC_END;
	}

	static void D3DDevice_GetVisibilityTestResult
	(
		xbox::dword_xt Index,
		xbox::uint_xt* pResult,
		xbox::ulonglong_xt* pTimeStamp
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Index)
			LOG_FUNC_ARG(pResult)
			LOG_FUNC_ARG(pTimeStamp)
			LOG_FUNC_END;
	}

	static void D3DDevice_InsertCallback
	(
		xbox::X_D3DCALLBACKTYPE	Type,
		xbox::X_D3DCALLBACK pCallback,
		xbox::dword_xt Context
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Type)
			LOG_FUNC_ARG((void*)pCallback) // FIXME // Error	C2593	'operator <<' is ambiguous
			LOG_FUNC_ARG(Context)
			LOG_FUNC_END;
	}

	static void D3DDevice_InsertFence()
	{
		LOG_FUNC();
	}

	static void D3DDevice_IsBusy()
	{
		LOG_FUNC();
	}

	static void D3DDevice_IsFencePending
	(
		xbox::dword_xt Fence
	)
	{
		LOG_FUNC_ONE_ARG(Fence);
	}

	static void D3DDevice_LightEnable
	(
		xbox::dword_xt Index,
		xbox::bool_xt bEnable
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Index)
			LOG_FUNC_ARG(bEnable)
			LOG_FUNC_END;
	}

	static void D3DDevice_LoadVertexShader
	(
		xbox::dword_xt Handle,
		xbox::dword_xt Address
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Handle)
			LOG_FUNC_ARG(Address)
			LOG_FUNC_END;
	}

	static void D3DDevice_LoadVertexShaderProgram
	(
		CONST xbox::dword_xt* pFunction,
		xbox::dword_xt Address
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(pFunction)
			LOG_FUNC_ARG(Address)
			LOG_FUNC_END;
	}

	static void D3DDevice_MultiplyTransform
	(
		xbox::X_D3DTRANSFORMSTATETYPE State,
		CONST D3DMATRIX* pMatrix
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(State)
			LOG_FUNC_ARG(pMatrix)
			LOG_FUNC_END;
	}

	static void D3DDevice_Present
	(
		CONST RECT* pSourceRect,
		CONST RECT* pDestRect,
		PVOID pDummy1,
		PVOID pDummy2
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(pSourceRect)
			LOG_FUNC_ARG(pDestRect)
			LOG_FUNC_ARG(pDummy1)
			LOG_FUNC_ARG(pDummy2)
			LOG_FUNC_END;
	}

	static void D3DDevice_PrimeVertexCache
	(
		xbox::uint_xt VertexCount,
		WORD* pIndexData
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(VertexCount)
			LOG_FUNC_ARG(pIndexData)
			LOG_FUNC_END;
	}

	static void D3DDevice_Reset
	(
		xbox::X_D3DPRESENT_PARAMETERS* pPresentationParameters
	)
	{
		LOG_FUNC_ONE_ARG(pPresentationParameters);
	}

	static void D3DDevice_RunPushBuffer
	(
		xbox::X_D3DPushBuffer* pPushBuffer,
		xbox::X_D3DFixup* pFixup
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(pPushBuffer)
			LOG_FUNC_ARG(pFixup)
			LOG_FUNC_END;
	}

	static void D3DDevice_RunVertexStateShader
	(
		xbox::dword_xt Address,
		CONST xbox::float_xt* pData
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Address)
			LOG_FUNC_ARG(pData)
			LOG_FUNC_END;
	}

	static void D3DDevice_SelectVertexShader
	(
		xbox::dword_xt Handle,
		xbox::dword_xt Address
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Handle)
			LOG_FUNC_ARG(Address)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetBackBufferScale
	(
		xbox::float_xt x,
		xbox::float_xt y
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(x)
			LOG_FUNC_ARG(y)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetDepthClipPlanes
	(
		xbox::float_xt Near,
		xbox::float_xt Far,
		xbox::dword_xt Flags
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Near)
			LOG_FUNC_ARG(Far)
			LOG_FUNC_ARG(Flags)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetFlickerFilter
	(
		xbox::dword_xt Filter
	)
	{
		LOG_FUNC_ONE_ARG(Filter);
	}

	static void D3DDevice_SetGammaRamp
	(
		xbox::dword_xt dwFlags,
		CONST xbox::X_D3DGAMMARAMP* pRamp
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(dwFlags)
			LOG_FUNC_ARG(pRamp)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetIndices
	(
		xbox::X_D3DIndexBuffer* pIndexData,
		xbox::uint_xt BaseVertexIndex
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(pIndexData)
			LOG_FUNC_ARG(BaseVertexIndex)
			LOG_FUNC_END;

	}

	static void D3DDevice_SetIndices_4
	(
		xbox::X_D3DIndexBuffer* pIndexData,
		xbox::uint_xt BaseVertexIndex
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(pIndexData)
			LOG_FUNC_ARG(BaseVertexIndex)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetLight
	(
		xbox::dword_xt Index,
		CONST xbox::X_D3DLIGHT8* pLight
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Index)
			LOG_FUNC_ARG(pLight)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetMaterial
	(
		CONST xbox::X_D3DMATERIAL8* pMaterial
	)
	{
		LOG_FUNC_ONE_ARG(pMaterial);
	}

	static void D3DDevice_SetModelView
	(
		CONST D3DMATRIX* pModelView,
		CONST D3DMATRIX* pInverseModelView,
		CONST D3DMATRIX* pComposite
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(pModelView)
			LOG_FUNC_ARG(pInverseModelView)
			LOG_FUNC_ARG(pComposite)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetPalette
	(
		xbox::dword_xt Stage,
		xbox::X_D3DPalette* pPalette
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Stage)
			LOG_FUNC_ARG(pPalette)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetPixelShader
	(
		xbox::dword_xt Handle
	)
	{
		LOG_FUNC_ONE_ARG(Handle);
	}

	static void D3DDevice_SetRenderTarget
	(
		xbox::X_D3DSurface* pRenderTarget,
		xbox::X_D3DSurface* pNewZStencil
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(pRenderTarget)
			LOG_FUNC_ARG(pNewZStencil)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetRenderTargetFast
	(
		xbox::X_D3DSurface* pRenderTarget,
		xbox::X_D3DSurface* pNewZStencil,
		xbox::dword_xt Flags
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(pRenderTarget)
			LOG_FUNC_ARG(pNewZStencil)
			LOG_FUNC_ARG(Flags)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetScreenSpaceOffset
	(
		xbox::float_xt x,
		xbox::float_xt y
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(x)
			LOG_FUNC_ARG(y)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetShaderConstantMode
	(
		xbox::X_VERTEXSHADERCONSTANTMODE Mode
	)
	{
		LOG_FUNC_ONE_ARG(Mode);
	}

	static void D3DDevice_SetSoftDisplayFilter
	(
		xbox::bool_xt Enable
	)
	{
		LOG_FUNC_ONE_ARG(Enable);
	}

	static void D3DDevice_SetStreamSource
	(
		xbox::uint_xt StreamNumber,
		xbox::X_D3DVertexBuffer* pStreamData,
		xbox::uint_xt Stride
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(StreamNumber)
			LOG_FUNC_ARG(pStreamData)
			LOG_FUNC_ARG(Stride)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetStipple
	(
		xbox::dword_xt* pPattern
	)
	{
		LOG_FUNC_ONE_ARG(pPattern);
	}

	static void D3DDevice_SetSwapCallback
	(
		xbox::X_D3DSWAPCALLBACK pCallback
	)
	{
		LOG_FUNC_ONE_ARG((void*)pCallback); // FIXME // Error	C2593	'operator <<' is ambiguous

	}

	static void D3DDevice_SetTexture
	(
		xbox::dword_xt Stage,
		xbox::X_D3DBaseTexture* pTexture
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Stage)
			LOG_FUNC_ARG(pTexture)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetTransform
	(
		xbox::X_D3DTRANSFORMSTATETYPE State,
		CONST D3DMATRIX* pMatrix
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(State)
			LOG_FUNC_ARG(pMatrix)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetVertexData2f
	(
		xbox::int_xt Register,
		xbox::float_xt a,
		xbox::float_xt b
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Register)
			LOG_FUNC_ARG(a)
			LOG_FUNC_ARG(b)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetVertexData2s
	(
		xbox::int_xt Register,
		xbox::short_xt a,
		xbox::short_xt b
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Register)
			LOG_FUNC_ARG(a)
			LOG_FUNC_ARG(b)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetVertexData4f
	(
		xbox::int_xt Register,
		xbox::float_xt a,
		xbox::float_xt b,
		xbox::float_xt c,
		xbox::float_xt d
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Register)
			LOG_FUNC_ARG(a)
			LOG_FUNC_ARG(b)
			LOG_FUNC_ARG(c)
			LOG_FUNC_ARG(d)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetVertexData4f_16
	(
		xbox::int_xt Register,
		xbox::float_xt a,
		xbox::float_xt b,
		xbox::float_xt c,
		xbox::float_xt d
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Register)
			LOG_FUNC_ARG(a)
			LOG_FUNC_ARG(b)
			LOG_FUNC_ARG(c)
			LOG_FUNC_ARG(d)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetVertexData4s
	(
		xbox::int_xt Register,
		xbox::short_xt a,
		xbox::short_xt b,
		xbox::short_xt c,
		xbox::short_xt d
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Register)
			LOG_FUNC_ARG(a)
			LOG_FUNC_ARG(b)
			LOG_FUNC_ARG(c)
			LOG_FUNC_ARG(d)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetVertexData4ub
	(
		xbox::int_xt Register,
		xbox::byte_xt a,
		xbox::byte_xt b,
		xbox::byte_xt c,
		xbox::byte_xt d
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Register)
			LOG_FUNC_ARG(a)
			LOG_FUNC_ARG(b)
			LOG_FUNC_ARG(c)
			LOG_FUNC_ARG(d)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetVertexDataColor
	(
		xbox::int_xt Register,
		D3DCOLOR Color
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Register)
			LOG_FUNC_ARG(Color)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetVertexShader
	(
		xbox::dword_xt Handle
	)
	{
		LOG_FUNC_ONE_ARG(Handle);
	}

	static void D3DDevice_SetVertexShaderConstant
	(
		xbox::int_xt Register,
		CONST xbox::PVOID pConstantData,
		xbox::dword_xt ConstantCount
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Register)
			LOG_FUNC_ARG(pConstantData)
			LOG_FUNC_ARG(ConstantCount)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetVertexShaderInput
	(
		xbox::dword_xt Handle,
		xbox::uint_xt StreamCount,
		xbox::X_STREAMINPUT* pStreamInputs
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Handle)
			LOG_FUNC_ARG(StreamCount)
			LOG_FUNC_ARG(pStreamInputs)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetVerticalBlankCallback
	(
		xbox::X_D3DVBLANKCALLBACK pCallback
	)
	{
		LOG_FUNC_ONE_ARG((void*)pCallback); // FIXME // Error	C2593	'operator <<' is ambiguous
	}

	static void D3DDevice_SetViewport
	(
		xbox::X_D3DVIEWPORT8* pViewport
	)
	{
		LOG_FUNC_ONE_ARG(pViewport);
	}

	static void D3DDevice_Swap
	(
		xbox::dword_xt Flags
	)
	{
		LOG_FUNC_ONE_ARG(Flags);
	}

	static void D3DDevice_SwitchTexture
	(
		xbox::dword_xt Method,
		xbox::dword_xt Data,
		xbox::dword_xt Format
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Method)
			LOG_FUNC_ARG(Data)
			LOG_FUNC_ARG(Format)
			LOG_FUNC_END;
	}

	static void D3DDevice_UpdateOverlay
	(
		xbox::X_D3DSurface* pSurface,
		CONST RECT* SrcRect,
		CONST RECT* DstRect,
		xbox::bool_xt EnableColorKey,
		D3DCOLOR ColorKey
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(pSurface)
			LOG_FUNC_ARG(SrcRect)
			LOG_FUNC_ARG(DstRect)
			LOG_FUNC_ARG(EnableColorKey)
			LOG_FUNC_ARG(ColorKey)
			LOG_FUNC_END;
	}

	static void D3DResource_BlockUntilNotBusy
	(
		xbox::X_D3DResource* pThis
	)
	{
		LOG_FUNC_ONE_ARG(pThis);
	}

	static void D3D_BlockOnTime
	(
		xbox::dword_xt Unknown1,
		int Unknown2
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Unknown1)
			LOG_FUNC_ARG(Unknown2)
			LOG_FUNC_END;
	}

	static void D3D_CommonSetRenderTarget
	(
		xbox::X_D3DSurface* pRenderTarget,
		xbox::X_D3DSurface* pNewZStencil,
		void* unknown
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(pRenderTarget)
			LOG_FUNC_ARG(pNewZStencil)
			LOG_FUNC_ARG(unknown)
			LOG_FUNC_END;
	}

	static void D3D_DestroyResource
	(
		xbox::X_D3DResource* pResource
	)
	{
		LOG_FUNC_ONE_ARG(pResource);
	}

	static void D3D_LazySetPointParams
	(
		void* Device
	)
	{
		LOG_FUNC_ONE_ARG(Device);
	}

	static void D3D_SetCommonDebugRegisters()
	{
		LOG_FUNC();
	}

	static void Direct3D_CreateDevice
	(
		xbox::uint_xt Adapter,
		D3DDEVTYPE DeviceType,
		HWND hFocusWindow,
		xbox::dword_xt BehaviorFlags,
		xbox::X_D3DPRESENT_PARAMETERS* pPresentationParameters,
		xbox::X_D3DDevice** ppReturnedDeviceInterface
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
	}

	static void Direct3D_CreateDevice_4
	(
		xbox::dword_xt BehaviorFlags,
		xbox::X_D3DPRESENT_PARAMETERS* pPresentationParameters,
		xbox::X_D3DDevice** ppReturnedDeviceInterface
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(BehaviorFlags)
			LOG_FUNC_ARG(pPresentationParameters)
			LOG_FUNC_ARG_OUT(ppReturnedDeviceInterface)
			LOG_FUNC_END;
	}

	static void Lock2DSurface
	(
		xbox::X_D3DPixelContainer* pPixelContainer,
		D3DCUBEMAP_FACES FaceType,
		xbox::uint_xt Level,
		D3DLOCKED_RECT* pLockedRect,
		RECT* pRect,
		xbox::dword_xt Flags
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

	static void Lock3DSurface
	(
		xbox::X_D3DPixelContainer* pPixelContainer,
		xbox::uint_xt				Level,
		D3DLOCKED_BOX* pLockedVolume,
		D3DBOX* pBox,
		xbox::dword_xt				Flags
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
}
