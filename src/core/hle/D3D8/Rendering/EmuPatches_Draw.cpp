
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_Begin)
(
    X_D3DPRIMITIVETYPE     PrimitiveType
)
{
	LOG_FUNC_ONE_ARG(PrimitiveType);

	CxbxImpl_Begin(PrimitiveType);
}

// ******************************************************************
// * patch: D3DDevice_SetVertexData2f
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetVertexData2f)
(
    int_xt     Register,
    float_xt   a,
    float_xt   b
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Register)
		LOG_FUNC_ARG(a)
		LOG_FUNC_ARG(b)
		LOG_FUNC_END;

	CxbxImpl_SetVertexData4f(Register, a, b, 0.0f, 1.0f);
}

static inline DWORD FtoDW(FLOAT f) { return *((DWORD*)&f); }
static inline FLOAT DWtoF(DWORD f) { return *((FLOAT*)&f); }

// ******************************************************************
// * patch: D3DDevice_SetVertexData2s
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetVertexData2s)
(
    int_xt   Register,
    short_xt a,
    short_xt b
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Register)
		LOG_FUNC_ARG(a)
		LOG_FUNC_ARG(b)
		LOG_FUNC_END;

	// Test case: Halo
	// Note : XQEMU verified that the int16_t arguments
	// must be mapped to floats in the range [-32768.0, 32767.0]
	// (See https://github.com/xqemu/xqemu/pull/176)
	const float fa = static_cast<float>(a);
	const float fb = static_cast<float>(b);


	CxbxImpl_SetVertexData4f(Register, a, b, 0.0f, 1.0f);
}

extern uint32_t HLE_read_NV2A_pgraph_register(const int reg); // Declared in PushBuffer.cpp

extern NV2ADevice* g_NV2A;

// Overload for logging
static void D3DDevice_SetVertexData4f_16__LTCG_edi1
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

// ******************************************************************
// * patch: D3DDevice_SetVertexData4f_16__LTCG_edi1
// ******************************************************************
// This is an LTCG specific version of SetVertexData4f where the first param is passed in EDI
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetVertexData4f_16__LTCG_edi1)
(
	float_xt   a,
	float_xt   b,
	float_xt   c,
	float_xt   d
)
{
	int_xt Register;

	__asm {
		LTCG_PROLOGUE
		mov  Register, edi
	}

	// Log
	D3DDevice_SetVertexData4f_16__LTCG_edi1(Register, a, b, c, d);

	CxbxImpl_SetVertexData4f(Register, a, b, c, d);

	_asm {
		LTCG_EPILOGUE
		ret  10h
	}
}

// ******************************************************************
// * patch: D3DDevice_SetVertexData4f
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetVertexData4f)
(
    int_xt     Register,
    float_xt   a,
    float_xt   b,
    float_xt   c,
    float_xt   d
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Register)
		LOG_FUNC_ARG(a)
		LOG_FUNC_ARG(b)
		LOG_FUNC_ARG(c)
		LOG_FUNC_ARG(d)
		LOG_FUNC_END;

	CxbxImpl_SetVertexData4f(Register, a, b, c, d);
}

// ******************************************************************
// * patch: D3DDevice_SetVertexData4ub
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetVertexData4ub)
(
	int_xt	Register,
	byte_xt	a,
	byte_xt	b,
	byte_xt	c,
	byte_xt	d
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Register)
		LOG_FUNC_ARG(a)
		LOG_FUNC_ARG(b)
		LOG_FUNC_ARG(c)
		LOG_FUNC_ARG(d)
		LOG_FUNC_END;

	const float fa = a / 255.0f;
	const float fb = b / 255.0f;
	const float fc = c / 255.0f;
	const float fd = d / 255.0f;

    CxbxImpl_SetVertexData4f(Register, fa, fb, fc, fd);
}

// ******************************************************************
// * patch: D3DDevice_SetVertexData4s
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetVertexData4s)
(
	int_xt	 Register,
	short_xt a,
	short_xt b,
	short_xt c,
	short_xt d
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Register)
		LOG_FUNC_ARG(a)
		LOG_FUNC_ARG(b)
		LOG_FUNC_ARG(c)
		LOG_FUNC_ARG(d)
		LOG_FUNC_END;

	// Test case: Halo
	// Note : XQEMU verified that the int16_t arguments
	// must be mapped to floats in the range [-32768.0, 32767.0]
	// (See https://github.com/xqemu/xqemu/pull/176)
	const float fa = static_cast<float>(a);
	const float fb = static_cast<float>(b);
	const float fc = static_cast<float>(c);
	const float fd = static_cast<float>(d);

    CxbxImpl_SetVertexData4f(Register, fa, fb, fc, fd);
}

// ******************************************************************
// * patch: D3DDevice_SetVertexDataColor
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetVertexDataColor)
(
    int_xt      Register,
    X_D3DCOLOR  Color
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Register)
		LOG_FUNC_ARG(Color)
		LOG_FUNC_END;

    const D3DXCOLOR XColor = Color;

    CxbxImpl_SetVertexData4f(Register, XColor.r, XColor.g, XColor.b, XColor.a);
}

// ******************************************************************
// * patch: D3DDevice_End
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_End)()
{
	LOG_FUNC();

	CxbxImpl_End();
}

// ******************************************************************
// * patch: D3DDevice_RunPushBuffer
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_RunPushBuffer)
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
// * patch: D3DDevice_RunPushBuffer_4__LTCG_eax2
// ******************************************************************
// Overload for logging
static void D3DDevice_RunPushBuffer_4__LTCG_eax2
(
    xbox::X_D3DPushBuffer       *pPushBuffer,
	xbox::X_D3DFixup            *pFixup
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pPushBuffer)
		LOG_FUNC_ARG(pFixup)
		LOG_FUNC_END;
}

// This uses a custom calling convention where parameter is passed in EAX
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_RunPushBuffer_4__LTCG_eax2)
(
    X_D3DPushBuffer *pPushBuffer
)
{
	X_D3DFixup* pFixup;
	__asm {
		LTCG_PROLOGUE
		mov  pFixup, eax
	}

	// Log
	D3DDevice_RunPushBuffer_4__LTCG_eax2(pPushBuffer, pFixup);

	EmuExecutePushBuffer(pPushBuffer, pFixup);

	__asm {
		LTCG_EPILOGUE
		ret  4
	}
}

// ******************************************************************
// * patch: D3DDevice_Clear
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_DrawVertices)
(
    X_D3DPRIMITIVETYPE PrimitiveType,
    uint_xt            StartVertex,
    uint_xt            VertexCount
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

	// TODO : Call unpatched CDevice_SetStateVB[_8](0);

	CxbxUpdateNativeD3DResources();

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
			// Convert quad vertex count to triangle vertex count :
			UINT NumVertices = QuadToTriangleVertexCount(DrawContext.dwVertexCount);
			// Convert quad primitive count to triangle primitive count :
			UINT primCount = DrawContext.dwHostPrimitiveCount * TRIANGLES_PER_QUAD;
			// See https://docs.microsoft.com/en-us/windows/win32/direct3d9/rendering-from-vertex-and-index-buffers
			// for an explanation on the function of the BaseVertexIndex, MinVertexIndex, NumVertices and StartIndex arguments.
			// Emulate drawing quads by drawing each quad with two indexed triangles :
			HRESULT hRet = CxbxDrawIndexedPrimitive(
				/*XboxPrimitiveType=*/(xbox::X_D3DPRIMITIVETYPE)xbox::X_D3DPT_TRIANGLELIST,
				/*IndexCount=*/primCount * 3,
				/*BaseVertexIndex=*/0, // Base vertex index has been accounted for in the stream conversion
				/*StartIndex=*/0,
				/*MinIndex=*/0,
				NumVertices,
				primCount
			);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitive(X_D3DPT_QUADLIST)");

			g_dwPrimPerFrame += primCount;
		}
		else {
			// if (StartVertex > 0) LOG_TEST_CASE("StartVertex > 0 (non-quad)"); // Verified test case : XDK Sample (PlayField)
			HRESULT hRet = CxbxDrawPrimitive(
				DrawContext.XboxPrimitiveType,
				DrawContext.dwVertexCount,
				/*StartVertex=*/0, // Start vertex has been accounted for in the stream conversion
				DrawContext.dwHostPrimitiveCount
			);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawPrimitive");

			g_dwPrimPerFrame += DrawContext.dwHostPrimitiveCount;
			if (DrawContext.XboxPrimitiveType == X_D3DPT_LINELOOP) {
				// Close line-loops using a final single line, drawn from the end to the start vertex
				LOG_TEST_CASE("X_D3DPT_LINELOOP"); // TODO : Text-cases needed

				assert(DrawContext.dwBaseVertexIndex == 0); // if this fails, it needs to be added to LowIndex and HighIndex :
				INDEX16 LowIndex = 0;
				INDEX16 HighIndex = (INDEX16)(DrawContext.dwHostPrimitiveCount);
				// Draw the closing line using a helper function (which will SetIndices)
				CxbxDrawIndexedClosingLine(LowIndex, HighIndex);
				// NOTE : We don't restore the previously active index buffer
			}
		}

	CxbxHandleXboxCallbacks();
}

// ******************************************************************
// * patch: D3DDevice_DrawVerticesUP
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_DrawVerticesUP)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    uint_xt             VertexCount,
    CONST PVOID         pVertexStreamZeroData,
    uint_xt             VertexStreamZeroStride
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

	// TODO : Call unpatched CDevice_SetStateUP();

	CxbxUpdateNativeD3DResources();

	CxbxDrawContext DrawContext = {};

	DrawContext.XboxPrimitiveType = PrimitiveType;
	DrawContext.dwVertexCount = VertexCount;
	DrawContext.pXboxVertexStreamZeroData = pVertexStreamZeroData;
	DrawContext.uiXboxVertexStreamZeroStride = VertexStreamZeroStride;

	CxbxDrawPrimitiveUP(DrawContext);

	CxbxHandleXboxCallbacks();
}

// LTCG specific D3DDevice_DrawVerticesUP function...
// This uses a custom calling convention where pVertexStreamZeroData is passed in EBX
// Test-case: NASCAR Heat 20002
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_DrawVerticesUP_12__LTCG_ebx3)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    uint_xt             VertexCount,
    uint_xt             VertexStreamZeroStride
)
{
    PVOID pVertexStreamZeroData;
    __asm {
        LTCG_PROLOGUE
        mov  pVertexStreamZeroData, ebx
    }

    EMUPATCH(D3DDevice_DrawVerticesUP)(PrimitiveType, VertexCount, pVertexStreamZeroData, VertexStreamZeroStride);

    __asm {
        LTCG_EPILOGUE
        ret  0Ch
    }
}

// ******************************************************************
// * patch: D3DDevice_DrawIndexedVertices
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_DrawIndexedVertices)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    uint_xt             VertexCount,
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

	// TODO : Call unpatched CDevice_SetStateVB[_8](g_Xbox_BaseVertexIndex);

	CxbxUpdateNativeD3DResources();

	CxbxDrawContext DrawContext = {};

	DrawContext.XboxPrimitiveType = PrimitiveType;
	DrawContext.dwVertexCount = VertexCount;
	DrawContext.dwBaseVertexIndex = g_Xbox_BaseVertexIndex; // Multiplied by vertex stride and added to the vertex buffer start
	DrawContext.pXboxIndexData = pIndexData; // Used to derive VerticesInBuffer

	// Test case JSRF draws all geometry through this function (only sparks are drawn via another method)
	// using X_D3DPT_TRIANGLELIST and X_D3DPT_TRIANGLESTRIP PrimitiveType
	CxbxDrawIndexed(DrawContext);

	CxbxHandleXboxCallbacks();
}

// ******************************************************************
// * patch: D3DDevice_DrawIndexedVerticesUP
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_DrawIndexedVerticesUP)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    uint_xt                VertexCount,
    CONST PVOID         pIndexData,
    CONST PVOID         pVertexStreamZeroData,
    uint_xt                VertexStreamZeroStride
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

	// TODO : Call unpatched CDevice_SetStateUP();

	CxbxUpdateNativeD3DResources();

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

		HRESULT hRet;
#ifdef CXBX_USE_D3D11
		// D3D11 has no DrawIndexedPrimitiveUP - use temporary dynamic buffers
		UINT vertexDataSize = DrawContext.dwVertexCount * DrawContext.uiHostVertexStreamZeroStride;
		UINT indexDataSize = (bConvertQuadListToTriangleList ? PrimitiveCount * 3 : DrawContext.dwVertexCount) * sizeof(INDEX16);

		D3D11_BUFFER_DESC vbDesc = {};
		vbDesc.ByteWidth = vertexDataSize;
		vbDesc.Usage = D3D11_USAGE_DYNAMIC;
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA vbInit = {};
		vbInit.pSysMem = DrawContext.pHostVertexStreamZeroData;

		ID3D11Buffer* pTempVB = nullptr;
		hRet = g_pD3DDevice->CreateBuffer(&vbDesc, &vbInit, &pTempVB);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateBuffer (DrawIndexedPrimitiveUP VB)");

		D3D11_BUFFER_DESC ibDesc = {};
		ibDesc.ByteWidth = indexDataSize;
		ibDesc.Usage = D3D11_USAGE_DYNAMIC;
		ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA ibInit = {};
		ibInit.pSysMem = pHostIndexData;

		ID3D11Buffer* pTempIB = nullptr;
		hRet = g_pD3DDevice->CreateBuffer(&ibDesc, &ibInit, &pTempIB);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateBuffer (DrawIndexedPrimitiveUP IB)");

		if (pTempVB != nullptr && pTempIB != nullptr) {
			UINT stride = DrawContext.uiHostVertexStreamZeroStride;
			UINT offset = 0;
			g_pD3DDeviceContext->IASetVertexBuffers(0, 1, &pTempVB, &stride, &offset);
			g_pD3DDeviceContext->IASetIndexBuffer(pTempIB, DXGI_FORMAT_R16_UINT, 0);
			D3D_PRIMITIVE_TOPOLOGY topology = bConvertQuadListToTriangleList ?
				D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST :
				EmuXB2PC_D3D11PrimitiveTopology(DrawContext.XboxPrimitiveType);
			g_pD3DDeviceContext->IASetPrimitiveTopology(topology);
			UINT indexCount = bConvertQuadListToTriangleList ? PrimitiveCount * 3 : DrawContext.dwVertexCount;
			g_pD3DDeviceContext->DrawIndexed(indexCount, 0, 0);
			hRet = S_OK;
		}

		if (pTempVB) pTempVB->Release();
		if (pTempIB) pTempIB->Release();
#else
		hRet = g_pD3DDevice->DrawIndexedPrimitiveUP(
			/*PrimitiveType=*/EmuXB2PC_D3DPrimitiveType(DrawContext.XboxPrimitiveType),
			/*MinVertexIndex=*/DrawContext.LowIndex,
			/*NumVertexIndices=*/(DrawContext.HighIndex - DrawContext.LowIndex) + 1,
			PrimitiveCount,
			pHostIndexData,
			/*IndexDataFormat=*/EMUFMT_INDEX16,
			DrawContext.pHostVertexStreamZeroData,
			DrawContext.uiHostVertexStreamZeroStride
		);
#endif
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

	CxbxHandleXboxCallbacks();
}

// ******************************************************************
// * patch: D3DDevice_SetLight
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(CDevice_SetStateVB)(ulong_xt Unknown1)
{
	addr_xt _this;
	__asm mov _this, ecx;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(_this)
		LOG_FUNC_ARG(Unknown1)
		LOG_FUNC_END;

	// TODO: Anything?
//	__asm int 3;

	LOG_UNIMPLEMENTED();
}

xbox::void_xt WINAPI xbox::EMUPATCH(CDevice_SetStateVB_8)(addr_xt _this, ulong_xt Unknown1)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(_this)
		LOG_FUNC_ARG(Unknown1)
		LOG_FUNC_END;

	// TODO: Anything?
//	__asm int 3;

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * patch: CDevice_SetStateUP (D3D::CDevice::SetStateUP)
// ******************************************************************
xbox::void_xt CxbxrImpl_CDevice_SetStateUP(xbox::addr_xt _this)
{
	LOG_UNIMPLEMENTED();

	// TODO: Anything?
	//__asm int 3;
}

xbox::void_xt WINAPI xbox::EMUPATCH(CDevice_SetStateUP)()
{
	addr_xt _this;
	__asm mov _this, ecx;

	LOG_FUNC_ONE_ARG(_this);

	CxbxrImpl_CDevice_SetStateUP(_this);
}

xbox::void_xt WINAPI xbox::EMUPATCH(CDevice_SetStateUP_4)(xbox::addr_xt _this)
{
	LOG_FUNC_ONE_ARG(_this);

	CxbxrImpl_CDevice_SetStateUP(_this);
}
static void CDevice_SetStateUP_0__LTCG_esi1(xbox::addr_xt _this)
{
	LOG_FUNC_ONE_ARG(_this);
}

__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(CDevice_SetStateUP_0__LTCG_esi1)()
{
	addr_xt _this;
	__asm {
		LTCG_PROLOGUE
		mov  _this, esi
	}

	// Log
	CDevice_SetStateUP_0__LTCG_esi1(_this);

	CxbxrImpl_CDevice_SetStateUP(_this);

	__asm {
		LTCG_EPILOGUE
		ret
	}
}

// ******************************************************************
// * patch: D3DDevice_SetStipple
// ******************************************************************
void WINAPI xbox::EMUPATCH(D3DDevice_SetStipple)( dword_xt* pPattern )
{
	LOG_FUNC_ONE_ARG(pPattern);

	// We need an OpenGL port... badly

	LOG_IGNORED();
}

// ******************************************************************
// * patch: D3DDevice_SetSwapCallback
// ******************************************************************
void WINAPI xbox::EMUPATCH(D3DDevice_SetSwapCallback)
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
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_PrimeVertexCache)
(
	uint_xt  VertexCount,
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
