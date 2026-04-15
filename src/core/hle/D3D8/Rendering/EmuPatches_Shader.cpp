#include "EmuD3D8_common.h"


xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_LoadVertexShader)
(
    dword_xt                       Handle,
    dword_xt                       Address
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Handle)
		LOG_FUNC_ARG(Address)
	LOG_FUNC_END;

	CxbxImpl_LoadVertexShader(Handle, Address);
}

// Overload for logging
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

// LTCG specific D3DDevice_SelectVertexShader function...
// This uses a custom calling convention where parameter is passed in EAX, EBX
// Test-case: Star Wars - Battlefront
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SelectVertexShader_0__LTCG_eax1_ebx2)
(
)
{
    dword_xt Handle;
    dword_xt Address;
    __asm {
        LTCG_PROLOGUE
        mov  Handle, eax
        mov  Address, ebx
    }

    // Log
    D3DDevice_SelectVertexShader_0__LTCG_eax1_ebx2(Handle, Address);

    CxbxImpl_SelectVertexShader(Handle, Address);

    __asm {
        LTCG_EPILOGUE
        ret
    }
}

// Overload for logging
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

// LTCG specific D3DDevice_SelectVertexShader function...
// This uses a custom calling convention where parameter is passed in EAX
// Test-case: Aggressive Inline
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SelectVertexShader_4__LTCG_eax1)
(
    dword_xt                       Address
)
{
    dword_xt Handle;
    __asm {
        LTCG_PROLOGUE
        mov  Handle, eax
    }

    // Log
    D3DDevice_SelectVertexShader_4__LTCG_eax1(Handle, Address);

    CxbxImpl_SelectVertexShader(Handle, Address);

    __asm {
        LTCG_EPILOGUE
        ret  4
    }
}

// ******************************************************************
// * patch: D3DDevice_SelectVertexShader
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SelectVertexShader)
(
    dword_xt                       Handle,
    dword_xt                       Address
)
{
    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(Handle)
        LOG_FUNC_ARG(Address)
        LOG_FUNC_END;

    CxbxImpl_SelectVertexShader(Handle, Address);
}

// ******************************************************************
// * patch: D3DDevice_SetGammaRamp
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetShaderConstantMode)
(
    X_VERTEXSHADERCONSTANTMODE Mode
)
{
	LOG_FUNC_ONE_ARG(Mode);

    g_Xbox_VertexShaderConstantMode = Mode;
}

// LTCG specific D3DDevice_SetVertexShaderConstant function...
// This uses a custom calling convention where ConstantCount parameter is passed in EDX
// Test-case: Murakumo
xbox::void_xt __fastcall xbox::EMUPATCH(D3DDevice_SetVertexShaderConstant_8__LTCG_edx3)
(
    void*,
    dword_xt    ConstantCount,
    int_xt      Register,
    CONST PVOID pConstantData
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Register)
		LOG_FUNC_ARG(pConstantData)
		LOG_FUNC_ARG(ConstantCount)
		LOG_FUNC_END;

	CxbxImpl_SetVertexShaderConstant(Register, pConstantData, ConstantCount);
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstant
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetVertexShaderConstant)
(
    int_xt         Register,
    CONST PVOID pConstantData,
    dword_xt       ConstantCount
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Register)
		LOG_FUNC_ARG(pConstantData)
		LOG_FUNC_ARG(ConstantCount)
		LOG_FUNC_END;

	// TODO : Should we trampoline into Xbox code as well here,
	// so that besides pushing NV2A commands, Xbox internal D3D
	// state gets updated?
	// Or better yet, remove all D3DDevice_SetVertexShaderConstant patches
	// once CxbxUpdateHostVertexShaderConstants is reliable (ie. : when we're
	// able to flush the NV2A push buffer)
	CxbxImpl_SetVertexShaderConstant(Register, pConstantData, ConstantCount);
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstant1
// ******************************************************************
xbox::void_xt __fastcall xbox::EMUPATCH(D3DDevice_SetVertexShaderConstant1)
(
    int_xt         Register,
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
xbox::void_xt __fastcall xbox::EMUPATCH(D3DDevice_SetVertexShaderConstant1Fast)
(
    int_xt         Register,
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
xbox::void_xt __fastcall xbox::EMUPATCH(D3DDevice_SetVertexShaderConstant4)
(
    int_xt         Register,
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
xbox::void_xt __fastcall xbox::EMUPATCH(D3DDevice_SetVertexShaderConstantNotInline)
(
    int_xt      Register,
    CONST PVOID pConstantData,
    dword_xt    ConstantCount
)
{
	LOG_FORWARD("D3DDevice_SetVertexShaderConstant");

    // The XDK uses a macro to automatically adjust to 0..191 range
    // but D3DDevice_SetVertexShaderConstant expects -96..95 range
    // so we adjust before forwarding
	EMUPATCH(D3DDevice_SetVertexShaderConstant)(Register - X_D3DSCM_CORRECTION, pConstantData, ConstantCount / 4);
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstantNotInline_0__LTCG_ebx1_edx2_eax3
// ******************************************************************
// Overload for logging
static void D3DDevice_SetVertexShaderConstantNotInline_0__LTCG_ebx1_edx2_eax3()
{
	LOG_FORWARD("D3DDevice_SetVertexShaderConstant");
}

// This uses a custom calling convention where parameter is passed in EBX, EDX, EAX
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetVertexShaderConstantNotInline_0__LTCG_ebx1_edx2_eax3)()
{
	int_xt Register;
	PVOID pConstantData;
	dword_xt ConstantCount;
	__asm {
		LTCG_PROLOGUE
		mov Register, ebx
		mov pConstantData, edx
		mov ConstantCount, eax
	}

	// Log
	D3DDevice_SetVertexShaderConstantNotInline_0__LTCG_ebx1_edx2_eax3();

	// The XDK uses a macro to automatically adjust to 0..191 range
	// but D3DDevice_SetVertexShaderConstant expects -96..95 range
	// so we adjust before forwarding
	EMUPATCH(D3DDevice_SetVertexShaderConstant)(Register - X_D3DSCM_CORRECTION, pConstantData, ConstantCount / 4);

	__asm {
        LTCG_EPILOGUE
        ret
    }
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstantNotInlineFast
// ******************************************************************
xbox::void_xt __fastcall xbox::EMUPATCH(D3DDevice_SetVertexShaderConstantNotInlineFast)
(
    int_xt         Register,
    CONST PVOID pConstantData,
    dword_xt       ConstantCount
)
{
	LOG_FORWARD("D3DDevice_SetVertexShaderConstant");

    // The XDK uses a macro to automatically adjust to 0..191 range
    // but D3DDevice_SetVertexShaderConstant expects -96..95 range
    // so we adjust before forwarding
	EMUPATCH(D3DDevice_SetVertexShaderConstant)(Register - X_D3DSCM_CORRECTION, pConstantData, ConstantCount / 4);
}

// Overload for logging
static void D3DDevice_SetTexture_4__LTCG_eax2
(
    xbox::dword_xt           Stage,
    xbox::X_D3DBaseTexture  *pTexture
)
{
    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(Stage)
        LOG_FUNC_ARG(pTexture)
        LOG_FUNC_END;
}

// LTCG specific D3DDevice_SetTexture function...
// This uses a custom calling convention where pTexture is passed in EAX
// Test-case: NASCAR Heat 2002
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetTexture_4__LTCG_eax2)
(
    dword_xt           Stage
)
{
    X_D3DBaseTexture *pTexture;
    __asm {
        LTCG_PROLOGUE
        mov  pTexture, eax
    }

    // Log
    D3DDevice_SetTexture_4__LTCG_eax2(Stage, pTexture);

    // Call the Xbox implementation of this function, to properly handle reference counting for us
    __asm {
        mov eax, pTexture
        push Stage
        call XB_TRMP(D3DDevice_SetTexture_4__LTCG_eax2)
    }

    g_pXbox_SetTexture[Stage] = pTexture;

    __asm {
        LTCG_EPILOGUE
        ret  4
    }
}

// Overload for logging
static void D3DDevice_SetTexture_4__LTCG_eax1
(
    xbox::dword_xt           Stage,
    xbox::X_D3DBaseTexture  *pTexture
)
{
    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(Stage)
        LOG_FUNC_ARG(pTexture)
        LOG_FUNC_END;
}

// LTCG specific D3DDevice_SetTexture function...
// This uses a custom calling convention where Stage is passed in EAX
// Test-case: Metal Wolf Chaos
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetTexture_4__LTCG_eax1)
(
    X_D3DBaseTexture  *pTexture
)
{
    dword_xt Stage;
    __asm {
        LTCG_PROLOGUE
        mov  Stage, eax
    }

    // Log
	D3DDevice_SetTexture_4__LTCG_eax1(Stage, pTexture);

    // Call the Xbox implementation of this function, to properly handle reference counting for us
    __asm {
        mov eax, Stage
        push pTexture
        call XB_TRMP(D3DDevice_SetTexture_4__LTCG_eax1)
    }

    g_pXbox_SetTexture[Stage] = pTexture;

    __asm {
        LTCG_EPILOGUE
        ret  4
    }
}

// ******************************************************************
// * patch: D3DDevice_SetTexture
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetPixelShader)
(
	dword_xt           Handle
)
{
	LOG_FUNC_ONE_ARG(Handle);

	// Call the Xbox function to make sure D3D structures get set
	XB_TRMP(D3DDevice_SetPixelShader)(Handle);

	CxbxImpl_SetPixelShader(Handle);
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
// * patch: D3DDevice_DrawVertices_4
// LTCG specific D3DDevice_DrawVertices function...
// This uses a custom calling convention where parameter is passed in ECX, EAX and Stack
// Test Case: Conker
// ******************************************************************
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_DrawVertices_4__LTCG_ecx2_eax3)
(
    X_D3DPRIMITIVETYPE PrimitiveType
)
{
    uint_xt VertexCount;
    uint_xt StartVertex;
    __asm {
        LTCG_PROLOGUE
        mov  VertexCount, eax
        mov  StartVertex, ecx
    }

    EMUPATCH(D3DDevice_DrawVertices)(PrimitiveType, StartVertex, VertexCount);

    __asm {
        LTCG_EPILOGUE
        ret  4
    }
}

// ******************************************************************
// * patch: D3DDevice_DrawVertices_8__LTCG_eax3
// LTCG specific D3DDevice_DrawVertices function...
// ******************************************************************
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_DrawVertices_8__LTCG_eax3)
(
    X_D3DPRIMITIVETYPE PrimitiveType,
    uint_xt            StartVertex
)
{
    uint_xt VertexCount;
    __asm {
        LTCG_PROLOGUE
        mov  VertexCount, eax
    }

    EMUPATCH(D3DDevice_DrawVertices)(PrimitiveType, StartVertex, VertexCount);

    __asm {
        LTCG_EPILOGUE
        ret  4
    }
}

// ******************************************************************
// * patch: D3DDevice_DrawVertices
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_DeleteVertexShader)
(
	dword_xt Handle
)
{
	LOG_FUNC_ONE_ARG(Handle);

	CxbxImpl_DeleteVertexShader(Handle);

	// When deleting, call trampoline *after* our implementation,
	// so that we can still access it's fields before it gets deleted!
	XB_TRMP(D3DDevice_DeleteVertexShader)(Handle);
}



// ******************************************************************
// * patch: D3DDevice_GetShaderConstantMode
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_GetShaderConstantMode)
(
    dword_xt *pMode
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
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_GetVertexShader)
(
    dword_xt *pHandle
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
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_GetVertexShaderConstant)
(
    int_xt   Register,
    void  *pConstantData,
    dword_xt ConstantCount
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

#ifdef CXBX_USE_D3D11
	// For D3D11, read from our local shadow of the constants
	if (Register >= 0 && (UINT)Register < CXBX_D3D11_VS_CB_COUNT && pConstantData != nullptr) {
		UINT copyCount = std::min((UINT)ConstantCount, CXBX_D3D11_VS_CB_COUNT - (UINT)Register);
		CxbxGetVertexShaderConstants((UINT)Register, (float*)pConstantData, copyCount);
	}
#else
	HRESULT hRet = g_pD3DDevice->GetVertexShaderConstantF
    (
        Register,
        (float*)pConstantData, // TODO : Validate this work correctly under D3D9
        ConstantCount
    );

	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetVertexShaderConstant");
#endif
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderInput
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetVertexShaderInput)
(
    dword_xt              Handle,
    uint_xt               StreamCount,
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

	/* LOG_TEST_CASE("SetVertexShaderInput");
	/* Test-cases :
		PushBuffer XDK sample
		Halo 2-3ebe4439.ini:D3DDevice_SetVertexShaderInput = 0x3f7440
		Kung Fu Chaos-d9ab292c.ini:D3DDevice_SetVertexShaderInput = 0x2bc0e0
		NBA LIVE 2005-71d4eeb1.ini:D3DDevice_SetVertexShaderInput = 0x5cf810
		NBA LIVE 2005-71d4eeb1.ini:D3DDevice_SetVertexShaderInputDirect = 0x5ceba0
		Prince of Persia WW-4ccf7369.ini:D3DDevice_SetVertexShaderInput = 0x494830
		Prince of Persia WW-4ccf7369.ini:D3DDevice_SetVertexShaderInputDirect = 0x494280
		Spyro A Hero's Tail-b18e00e5.ini:D3DDevice_SetVertexShaderInput = 0x286cf0
		Spyro A Hero's Tail-b18e00e5.ini:D3DDevice_SetVertexShaderInputDirect = 0x286760
	*/

	CxbxImpl_SetVertexShaderInput(Handle, StreamCount, pStreamInputs);

	// Call trampoline
	if (XB_TRMP(D3DDevice_SetVertexShaderInput))
		XB_TRMP(D3DDevice_SetVertexShaderInput)(Handle, StreamCount, pStreamInputs);
}

// ******************************************************************
// * patch: D3DDevice_RunVertexStateShader
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_RunVertexStateShader)
(
    dword_xt Address,
    CONST float_xt *pData
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Address)
		LOG_FUNC_ARG(pData)
		LOG_FUNC_END;

	CxbxrImpl_RunVertexStateShader(Address, pData);
}

// ******************************************************************
// * patch: D3DDevice_RunVertexStateShader_4__LTCG_esi2
// ******************************************************************
// Overload for logging
static void D3DDevice_RunVertexStateShader_4__LTCG_esi2
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

// This uses a custom calling convention where parameter is passed in ESI
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_RunVertexStateShader_4__LTCG_esi2)
(
    dword_xt Address
)
{
	float_xt *pData;
	__asm {
		LTCG_PROLOGUE
		mov  pData, esi
	}

	// Log
	D3DDevice_RunVertexStateShader_4__LTCG_esi2(Address, pData);

	CxbxrImpl_RunVertexStateShader(Address, pData);

	__asm {
		LTCG_EPILOGUE
		ret  4
	}
}

// ******************************************************************
// * patch: D3DDevice_LoadVertexShaderProgram
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_LoadVertexShaderProgram)
(
	CONST dword_xt *pFunction,
	dword_xt        Address
	)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pFunction)
		LOG_FUNC_ARG(Address)
		LOG_FUNC_END;

	CxbxImpl_LoadVertexShaderProgram((DWORD *)pFunction, Address);
}

// ******************************************************************
// * patch: D3DDevice_SetDepthClipPlanes
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_SetDepthClipPlanes)
(
    float_xt Near,
    float_xt Far,
    dword_xt Flags
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
