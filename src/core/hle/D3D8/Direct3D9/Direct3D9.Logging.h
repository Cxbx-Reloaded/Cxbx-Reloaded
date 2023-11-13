namespace Logging // Note : For ease of maintenance, sort below functions
{
	static void CDevice_SetStateUP_0__LTCG_esi1(xbox::addr_xt _this)
	{
		LOG_FUNC_ONE_ARG(_this);
	}

	static void D3DDevice_DeleteVertexShader_0
	(
		xbox::dword_xt Handle
	)
	{
		LOG_FUNC_ONE_ARG(Handle);
	}

	void D3DDevice_DrawVerticesUP
	(
		xbox::X_D3DPRIMITIVETYPE  PrimitiveType,
		xbox::uint_xt             VertexCount,
		CONST xbox::PVOID         pVertexStreamZeroData,
		xbox::uint_xt             VertexStreamZeroStride
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(PrimitiveType)
			LOG_FUNC_ARG(VertexCount)
			LOG_FUNC_ARG(pVertexStreamZeroData)
			LOG_FUNC_ARG(VertexStreamZeroStride)
			LOG_FUNC_END;
	}

	static void D3DDevice_GetBackBuffer2_0__LTCG_eax1(xbox::int_xt BackBuffer)
	{
		LOG_FUNC_ONE_ARG(BackBuffer);
	}

	static void D3DDevice_LoadVertexShader_0__LTCG_eax_Address_ecx_Handle
	(
		xbox::dword_xt                 Handle,
		xbox::dword_xt                 Address
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Handle)
			LOG_FUNC_ARG(Address)
			LOG_FUNC_END;
	}

	static void D3DDevice_LoadVertexShader_0__LTCG_eax_Address_edx_Handle
	(
		xbox::dword_xt                 Handle,
		xbox::dword_xt                 Address
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Handle)
			LOG_FUNC_ARG(Address)
			LOG_FUNC_END;
	}

	static void D3DDevice_LoadVertexShader_4
	(
		xbox::dword_xt                 Handle,
		xbox::dword_xt                 Address
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Handle)
			LOG_FUNC_ARG(Address)
			LOG_FUNC_END;
	}

	static void D3DDevice_Reset_0__LTCG_ebx1(xbox::X_D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		LOG_FUNC_ONE_ARG(pPresentationParameters);
	}

	static void D3DDevice_Reset_0__LTCG_edi1(xbox::X_D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		LOG_FUNC_ONE_ARG(pPresentationParameters);
	}

	static void D3DDevice_SelectVertexShader_0__LTCG_eax1_ebx2
	(
		xbox::dword_xt                 Handle,
		xbox::dword_xt                 Address
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Handle)
			LOG_FUNC_ARG(Address)
			LOG_FUNC_END;
	}

	static void D3DDevice_SelectVertexShader_4__LTCG_eax1
	(
		xbox::dword_xt                 Handle,
		xbox::dword_xt                 Address
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Handle)
			LOG_FUNC_ARG(Address)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetIndices_4
	(
		xbox::X_D3DIndexBuffer* pIndexData,
		xbox::uint_xt             BaseVertexIndex
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(pIndexData)
			LOG_FUNC_ARG(BaseVertexIndex)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetPalette_4
	(
		xbox::dword_xt      Stage,
		xbox::X_D3DPalette* pPalette
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Stage)
			LOG_FUNC_ARG(pPalette)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetPixelShader_0__LTCG_eax_handle
	(
		xbox::dword_xt      Handle
	)
	{
		LOG_FUNC_ONE_ARG(Handle);
	}

	static void D3DDevice_SetRenderTarget_0
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

	static void D3DDevice_SetStreamSource_0__LTCG_eax_StreamNumber_edi_pStreamData_ebx_Stride
	(
		xbox::uint_xt            StreamNumber,
		xbox::X_D3DVertexBuffer* pStreamData,
		xbox::uint_xt            Stride
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(StreamNumber)
			LOG_FUNC_ARG(pStreamData)
			LOG_FUNC_ARG(Stride)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetStreamSource_4
	(
		xbox::uint_xt            StreamNumber,
		xbox::X_D3DVertexBuffer* pStreamData,
		xbox::uint_xt            Stride
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(StreamNumber)
			LOG_FUNC_ARG(pStreamData)
			LOG_FUNC_ARG(Stride)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetStreamSource_8
	(
		xbox::uint_xt            StreamNumber,
		xbox::X_D3DVertexBuffer* pStreamData,
		xbox::uint_xt            Stride
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(StreamNumber)
			LOG_FUNC_ARG(pStreamData)
			LOG_FUNC_ARG(Stride)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetTexture_4__LTCG_eax_pTexture
	(
		xbox::dword_xt           Stage,
		xbox::X_D3DBaseTexture* pTexture
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Stage)
			LOG_FUNC_ARG(pTexture)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetTexture_4__LTCG_eax_Stage
	(
		xbox::dword_xt           Stage,
		xbox::X_D3DBaseTexture* pTexture
	)
	{
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(Stage)
			LOG_FUNC_ARG(pTexture)
			LOG_FUNC_END;
	}

	static void D3DDevice_SetVertexData4f_16
	(
		xbox::int_xt     Register,
		xbox::float_xt   a,
		xbox::float_xt   b,
		xbox::float_xt   c,
		xbox::float_xt   d
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

	static void D3DDevice_SetVertexShader_0
	(
		xbox::dword_xt Handle
	)
	{
		LOG_FUNC_ONE_ARG(Handle);
	}

	static void D3D_DestroyResource__LTCG(xbox::X_D3DResource* pResource)
	{
		LOG_FUNC_ONE_ARG(pResource);
	}

	static void Direct3D_CreateDevice_16__LTCG_eax_BehaviorFlags_ebx_ppReturnedDeviceInterface
	(
		xbox::uint_xt                 Adapter,
		D3DDEVTYPE                    DeviceType,
		HWND                          hFocusWindow,
		xbox::dword_xt                BehaviorFlags,
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

	static void Direct3D_CreateDevice_16__LTCG_eax_BehaviorFlags_ecx_ppReturnedDeviceInterface
	(
		xbox::uint_xt                 Adapter,
		D3DDEVTYPE                    DeviceType,
		HWND                          hFocusWindow,
		xbox::dword_xt                BehaviorFlags,
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
		xbox::dword_xt                BehaviorFlags,
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
}
