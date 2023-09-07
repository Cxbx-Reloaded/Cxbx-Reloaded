// virtual NV2A register for HLE API handler
#   define NV097_HLE_API                                      0x00000080
#include "..\XbD3D8Types.h"
// enum for xbox D3DDevice APIs
typedef enum _X_D3DAPI_ENUM {

    X_CDevice_SetStateUP,
    X_CDevice_SetStateUP_4,
    X_CDevice_SetStateUP_0__LTCG_esi1,
    X_CDevice_SetStateVB,
    X_CDevice_SetStateVB_8,
    X_D3DDevice_ApplyStateBlock,
    X_D3DDevice_Begin,
    X_D3DDevice_BeginPush,
    X_D3DDevice_BeginPush_4,
    X_D3DDevice_BeginPush_8,
    X_D3DDevice_BeginPushBuffer,
    X_D3DDevice_BeginScene,
    X_D3DDevice_BeginState,
    X_D3DDevice_BeginStateBig,
    X_D3DDevice_BeginStateBlock,
    X_D3DDevice_BeginVisibilityTest,
    X_D3DDevice_BlockOnFence,
    X_D3DDevice_BlockUntilIdle,
    X_D3DDevice_BlockUntilVerticalBlank,
    X_D3DDevice_CaptureStateBlock,
    X_D3DDevice_Clear,
    X_D3DDevice_CopyRects,
    X_D3DDevice_CreateCubeTexture,
    X_D3DDevice_CreateDepthStencilSurface,
    X_D3DDevice_CreateFixup,
    X_D3DDevice_CreateImageSurface,
    X_D3DDevice_CreateIndexBuffer,
    X_D3DDevice_CreatePalette,
    X_D3DDevice_CreatePixelShader,
    X_D3DDevice_CreatePushBuffer,
    X_D3DDevice_CreateRenderTarget,
    X_D3DDevice_CreateStateBlock,
    X_D3DDevice_CreateTexture,
    X_D3DDevice_CreateTexture2,
    X_D3DDevice_CreateVertexBuffer,
    X_D3DDevice_CreateVertexBuffer2,
    X_D3DDevice_CreateVertexShader,
    X_D3DDevice_CreateVolumeTexture,
    X_D3DDevice_DeletePatch,
    X_D3DDevice_DeletePixelShader,
    X_D3DDevice_DeleteStateBlock,
    X_D3DDevice_DeleteVertexShader, X_D3DDevice_DeleteVertexShader_0,
    X_D3DDevice_DrawIndexedPrimitive,
    X_D3DDevice_DrawIndexedPrimitiveUP,
    X_D3DDevice_DrawIndexedVertices,
    X_D3DDevice_DrawIndexedVerticesUP,
    X_D3DDevice_DrawPrimitive,
    X_D3DDevice_DrawPrimitiveUP,
    X_D3DDevice_DrawRectPatch,
    X_D3DDevice_DrawTriPatch,
    X_D3DDevice_DrawVertices, X_D3DDevice_DrawVertices_4__LTCG_ecx2_eax3, X_D3DDevice_DrawVertices_8__LTCG_eax3,
    X_D3DDevice_DrawVerticesUP, X_D3DDevice_DrawVerticesUP_12__LTCG_ebx3,
    X_D3DDevice_EnableOverlay,
    X_D3DDevice_End,
    X_D3DDevice_EndPush,
    X_D3DDevice_EndPushBuffer,
    X_D3DDevice_EndScene,
    X_D3DDevice_EndState,
    X_D3DDevice_EndStateBlock,
    X_D3DDevice_EndVisibilityTest, X_D3DDevice_EndVisibilityTest_0,
    X_D3DDevice_FlushVertexCache,
    X_D3DDevice_GetBackBuffer, X_D3DDevice_GetBackBuffer2, X_D3DDevice_GetBackBuffer2_0__LTCG_eax1,
    X_D3DDevice_GetBackBufferScale,
    X_D3DDevice_GetBackMaterial,
    X_D3DDevice_GetCopyRectsState,
    X_D3DDevice_GetCreationParameters,
    X_D3DDevice_GetDebugMarker,
    X_D3DDevice_GetDepthClipPlanes,
    X_D3DDevice_GetDepthStencilSurface, X_D3DDevice_GetDepthStencilSurface2,
    X_D3DDevice_GetDeviceCaps,
    X_D3DDevice_GetDirect3D,
    X_D3DDevice_GetDisplayFieldStatus,
    X_D3DDevice_GetDisplayMode,
    X_D3DDevice_GetGammaRamp,
    X_D3DDevice_GetIndices,
    X_D3DDevice_GetLight,
    X_D3DDevice_GetLightEnable,
    X_D3DDevice_GetMaterial,
    X_D3DDevice_GetModelView,
    X_D3DDevice_GetOverlayUpdateStatus,
    X_D3DDevice_GetOverscanColor,
    X_D3DDevice_GetPalette,
    X_D3DDevice_GetPersistedSurface,
    X_D3DDevice_GetPixelShader,
    X_D3DDevice_GetPixelShaderConstant,
    X_D3DDevice_GetPixelShaderFunction,
    X_D3DDevice_GetProjectionViewportMatrix,
    X_D3DDevice_GetPushBufferOffset,
    X_D3DDevice_GetPushDistance,
    X_D3DDevice_GetRasterStatus,
    X_D3DDevice_GetRenderState,
    X_D3DDevice_GetRenderTarget, X_D3DDevice_GetRenderTarget2,
    X_D3DDevice_GetScissors,
    X_D3DDevice_GetScreenSpaceOffset,
    X_D3DDevice_GetShaderConstantMode,
    X_D3DDevice_GetStipple,
    X_D3DDevice_GetStreamSource,
    X_D3DDevice_GetTexture,
    X_D3DDevice_GetTextureStageState,
    X_D3DDevice_GetTile,
    X_D3DDevice_GetTileCompressionTags,
    X_D3DDevice_GetTransform,
    X_D3DDevice_GetVertexBlendModelView,
    X_D3DDevice_GetVertexShader,
    X_D3DDevice_GetVertexShaderConstant,
    X_D3DDevice_GetVertexShaderDeclaration,
    X_D3DDevice_GetVertexShaderFunction,
    X_D3DDevice_GetVertexShaderInput,
    X_D3DDevice_GetVertexShaderSize,
    X_D3DDevice_GetVertexShaderType,
    X_D3DDevice_GetViewport,
    X_D3DDevice_GetViewportOffsetAndScale,
    X_D3DDevice_GetVisibilityTestResult,
    X_D3DDevice_InsertCallback,
    X_D3DDevice_InsertFence,
    X_D3DDevice_IsBusy,
    X_D3DDevice_IsFencePending,
    X_D3DDevice_KickPushBuffer,
    X_D3DDevice_LightEnable,
    X_D3DDevice_LoadVertexShader, X_D3DDevice_LoadVertexShader_0__LTCG_eax_Address_ecx_Handle, X_D3DDevice_LoadVertexShader_0__LTCG_eax_Address_edx_Handle, X_D3DDevice_LoadVertexShader_4,
    X_D3DDevice_LoadVertexShaderProgram,
    X_D3DDevice_MultiplyTransform,
    X_D3DDevice_Nop,
    X_D3DDevice_PersistDisplay,
    X_D3DDevice_Present,
    X_D3DDevice_PrimeVertexCache,
    X_D3DDevice_Reset, X_D3DDevice_Reset_0__LTCG_edi1, X_D3DDevice_Reset_0__LTCG_ebx1,
    X_D3DDevice_RunPushBuffer,
    X_D3DDevice_RunVertexStateShader,
    X_D3DDevice_SelectVertexShader, X_D3DDevice_SelectVertexShader_0__LTCG_eax1_ebx2, X_D3DDevice_SelectVertexShader_4__LTCG_eax1,
    X_D3DDevice_SelectVertexShaderDirect,
    X_D3DDevice_SetBackBufferScale,
    X_D3DDevice_SetBackMaterial,
    X_D3DDevice_SetCopyRectsState,
    X_D3DDevice_SetDebugMarker,
    X_D3DDevice_SetDepthClipPlanes,
    X_D3DDevice_SetFlickerFilter, X_D3DDevice_SetFlickerFilter_0,
    X_D3DDevice_SetGammaRamp,
    X_D3DDevice_SetIndices, X_D3DDevice_SetIndices_4,
    X_D3DDevice_SetLight,
    X_D3DDevice_SetMaterial,
    X_D3DDevice_SetModelView,
    X_D3DDevice_SetOverscanColor,
    X_D3DDevice_SetPalette, X_D3DDevice_SetPalette_4,
    X_D3DDevice_SetPixelShader, X_D3DDevice_SetPixelShader_0__LTCG_eax_handle,
    X_D3DDevice_SetPixelShaderConstant, X_D3DDevice_SetPixelShaderConstant_4,
    X_D3DDevice_SetPixelShaderProgram,
    X_D3DDevice_SetRenderState, X_D3DDevice_SetRenderState_Simple,
    X_D3DDevice_SetRenderStateNotInline,
    X_D3DDevice_SetRenderTarget, X_D3DDevice_SetRenderTarget_0,
    X_D3DDevice_SetRenderTargetFast,
    X_D3DDevice_SetScissors,
    X_D3DDevice_SetScreenSpaceOffset,
    X_D3DDevice_SetShaderConstantMode, X_D3DDevice_SetShaderConstantMode_0__LTCG_eax1,
    X_D3DDevice_SetSoftDisplayFilter,
    X_D3DDevice_SetStipple,
    X_D3DDevice_SetStreamSource, X_D3DDevice_SetStreamSource_0__LTCG_eax_StreamNumber_edi_pStreamData_ebx_Stride, X_D3DDevice_SetStreamSource_4, X_D3DDevice_SetStreamSource_8, X_D3DDevice_SetStreamSource_8__LTCG_edx_StreamNumber,
    X_D3DDevice_SetSwapCallback,
    X_D3DDevice_SetTexture, X_D3DDevice_SetTexture_4__LTCG_eax_pTexture, X_D3DDevice_SetTexture_4__LTCG_eax_Stage,
    X_D3DDevice_SetTextureStageState,
    X_D3DDevice_SetTextureStageStateNotInline,
    X_D3DDevice_SetTile,
    X_D3DDevice_SetTimerCallback,
    X_D3DDevice_SetTransform, X_D3DDevice_SetTransform_0__LTCG_eax1_edx2,
    X_D3DDevice_SetVertexBlendModelView,
    X_D3DDevice_SetVertexData2f,
    X_D3DDevice_SetVertexData2s,
    X_D3DDevice_SetVertexData4f, X_D3DDevice_SetVertexData4f_16,
    X_D3DDevice_SetVertexData4s,
    X_D3DDevice_SetVertexData4ub,
    X_D3DDevice_SetVertexDataColor,
    X_D3DDevice_SetVertexShader, X_D3DDevice_SetVertexShader_0,
    X_D3DDevice_SetVertexShaderConstant, X_D3DDevice_SetVertexShaderConstant_8, X_D3DDevice_SetVertexShaderConstant1, X_D3DDevice_SetVertexShaderConstant4,
    X_D3DDevice_SetVertexShaderConstantFast, X_D3DDevice_SetVertexShaderConstant1Fast, X_D3DDevice_SetVertexShaderConstantNotInline, X_D3DDevice_SetVertexShaderConstantNotInlineFast,
    X_D3DDevice_SetVertexShaderInput,
    X_D3DDevice_SetVertexShaderInputDirect,
    X_D3DDevice_SetVerticalBlankCallback,
    X_D3DDevice_SetViewport,
    X_D3DDevice_SetWaitCallback,
    X_D3DDevice_Swap, X_D3DDevice_Swap_0,
    X_D3DDevice_SwitchTexture,
    X_D3DDevice_UpdateOverlay,
    X_D3DResource_BlockUntilNotBusy,
    X_D3D_BlockOnTime, X_D3D_BlockOnTime_4,
    X_D3D_CommonSetRenderTarget,
    X_D3D_DestroyResource, X_D3D_DestroyResource__LTCG,
    X_D3D_LazySetPointParams,
    X_D3D_SetCommonDebugRegisters,
    X_Direct3D_CreateDevice, X_Direct3D_CreateDevice_16__LTCG_eax_BehaviorFlags_ebx_ppReturnedDeviceInterface, X_Direct3D_CreateDevice_16__LTCG_eax_BehaviorFlags_ecx_ppReturnedDeviceInterface, X_Direct3D_CreateDevice_4,
    X_Lock2DSurface,
    X_Lock3DSurface,
    X_D3DAPI_FORCE_DWORD = 0x7fffffff,

} X_D3DAPI_ENUM;

#define HLE_API_PUSHBFFER_COMMAND 0x403C0080
#define NV097_HLE_API 0x00000080


/*
//template for xbox D3D API patch routine

    API_name(args)
{
    // init pushbuffer related pointers
    DWORD * pPush_local = (DWORD *)*g_pXbox_pPush;         //pointer to current pushbuffer
    DWORD * pPush_limit = (DWORD *)*g_pXbox_pPushLimit;    //pointer to the end of current pushbuffer
    if ( (unsigned int)pPush_local+64 >= (unsigned int)pPush_limit )//check if we still have enough space
        pPush_local = (DWORD *)CxbxrImpl_MakeSpace(); //make new pushbuffer space and get the pointer to it.

    // process xbox D3D API enum and arguments and push them to pushbuffer for pgraph to handle later.
    pPush_local[0]=HLE_API_PUSHBFFER_COMMAND;
    pPush_local[1]=X_D3DAPI_ENUM::X_APINAME;//enum of this patched API
    pPush_local[2] = (DWORD) arg[0]; //total 14 DWORD space for arguments.
    pPush_local[3] = (DWORD) arg[1];
    pPush_local[4] = (DWORD) arg[2];
    pPush_local[5] = (DWORD) arg[3]; 
    pPush_local[6] = (DWORD) arg[4];
    pPush_local[7] = (DWORD) arg[5];
    pPush_local[8] =  *(DWORD*) &  arg[6];// use this to copy float arg to pushbuffer

    //set pushbuffer pointer to the new beginning
    // always reserve 1 command DWORD, 1 API enum, and 14 argmenet DWORDs.
    *(DWORD **)g_pXbox_pPush+=0x10; 

}

//template for xbox D3D API handler in pgraph

HLEApi=arg[0];
switch (HLEApi)
    
{
    case X_APIName:
        // convert arg[1]... to corresponded arg[0] of CabxrImpl_APIName() handler.
        call CxbxrImpl_APIName(arg[1]....);

    break;
}

*/
static inline DWORD FtoDW(FLOAT f) { return *((DWORD*)&f); }
static inline FLOAT DWtoF(DWORD f) { return *((FLOAT*)&f); }

// prototypes for xbox D3DDevice API HLE handlers in NV2A pgraph, implemented in Direct3D9.cpp, called in EmuNV2A_PGRAPH.cpp
// we create defines for each patched api in general format USEPGRAPH_ + apu post names without D3DDevice_. define it as 1 to enable the patch and prgaph handler, as 0 to keep original Cxbxr behavior. this is to speed up the test which api is not feasible for this POC.
void CxbxrImpl_Begin(xbox::X_D3DPRIMITIVETYPE PrimitiveType);
void CxbxrImpl_Clear(xbox::dword_xt Count, CONST D3DRECT* pRects, xbox::dword_xt Flags, D3DCOLOR Color, float Z, xbox::dword_xt Stencil);
#define USEPGRAPH_Clear 1
void WINAPI CxbxrImpl_CopyRects(xbox::X_D3DSurface* pSourceSurface, CONST RECT* pSourceRectsArray, xbox::uint_xt cRects, xbox::X_D3DSurface* pDestinationSurface, CONST POINT* pDestPointsArray);
#define USEPGRAPH_CopyRects 1 /*CopyRects() is not permitted in pushbuffer recording.*/
;
;
//void WINAPI CxbxrImpl_DrawIndexedVertices(xbox::X_D3DPRIMITIVETYPE  PrimitiveType, xbox::uint_xt VertexCount, CONST PWORD pIndexData);
//void WINAPI CxbxrImpl_DrawIndexedVerticesUP(xbox::X_D3DPRIMITIVETYPE  PrimitiveType, xbox::uint_xt VertexCount, CONST PVOID pIndexData, CONST PVOID pVertexStreamZeroData, xbox::uint_xt VertexStreamZeroStride);
//void WINAPI CxbxrImpl_DrawRectPatch(xbox::uint_xt	Handle, CONST xbox::float_xt* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo);
//void WINAPI CxbxrImpl_DrawTriPatch(xbox::uint_xt	Handle, CONST xbox::float_xt* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo);
//void WINAPI CxbxrImpl_DrawVertices(xbox::X_D3DPRIMITIVETYPE PrimitiveType, xbox::uint_xt StartVertex, xbox::uint_xt VertexCount);
#define USEPGRAPH_DrawVertices 0
void WINAPI CxbxrImpl_DrawVerticesUP(xbox::X_D3DPRIMITIVETYPE  PrimitiveType, xbox::uint_xt VertexCount, CONST PVOID pVertexStreamZeroData, xbox::uint_xt VertexStreamZeroStride);
#define USEPGRAPH_DrawVerticesUP 0
void CxbxrImpl_End();
void CxbxrImpl_InsertCallback(xbox::X_D3DCALLBACKTYPE Type, xbox::X_D3DCALLBACK pCallback, xbox::dword_xt Context);
//void WINAPI CxbxrImpl_LightEnable(xbox::dword_xt Index, xbox::bool_xt bEnable);
void CxbxrImpl_LoadVertexShader(DWORD Handle, DWORD ProgramRegister);
#define USEPGRAPH_LoadVertexShader 1
void CxbxrImpl_LoadVertexShaderProgram(CONST DWORD* pFunction, DWORD Address);
#define USEPGRAPH_LoadVertexShaderProgram 0
void WINAPI CxbxrImpl_RunVertexStateShader(xbox::dword_xt Address, CONST xbox::float_xt* pData);
void CxbxrImpl_SelectVertexShader(DWORD Handle, DWORD Address);
#define USEPGRAPH_SelectVertexShader 1
void WINAPI CxbxrImpl_SetBackBufferScale(xbox::float_xt x, xbox::float_xt y);
// todo: CxbxrImpl_SetFlickerFilter() not implemented yet.
void WINAPI CxbxrImpl_SetFlickerFilter(xbox::dword_xt Filter);
//void WINAPI CxbxrImpl_SetLight(xbox::dword_xt Index, CONST xbox::X_D3DLIGHT8* pLight);
//void WINAPI CxbxrImpl_SetMaterial(CONST xbox::X_D3DMATERIAL8* pMaterial);
//void WINAPI CxbxrImpl_SetModelView(CONST D3DMATRIX* pModelView, CONST D3DMATRIX* pInverseModelView, CONST D3DMATRIX* pComposite);
void CxbxrImpl_SetPalette(xbox::dword_xt Stage, xbox::X_D3DPalette* pPalette);
void CxbxrImpl_SetPixelShader(xbox::dword_xt Handle);
//void WINAPI CxbxrImpl_SetRenderState_Simple(xbox::dword_xt Method, xbox::dword_xt Value);
void CxbxrImpl_SetRenderTarget(xbox::X_D3DSurface* pRenderTarget, xbox::X_D3DSurface* pNewZStencil);
#define USEPGRAPH_SetRenderTarget 0 /*SetRenderTarget() is used in D3DDevice_Create() and we need it to be implemented right away. to do: */
void CxbxrImpl_SetScreenSpaceOffset(float x, float y);
#define USEPGRAPH_SetScreenSpaceOffset 0
void WINAPI CxbxrImpl_SetShaderConstantMode(xbox::X_VERTEXSHADERCONSTANTMODE Mode);
// todo: CxbxrImpl_SetSoftDisplayFilter() not implemented yet.
void WINAPI CxbxrImpl_SetSoftDisplayFilter(xbox::bool_xt Enable);
void CxbxrImpl_SetStreamSource(UINT StreamNumber, xbox::X_D3DVertexBuffer* pStreamData, UINT Stride);
//#define USEPGRAPH_SetStreamSource 0 /*not permitted in pushbuffer recording*/
void WINAPI CxbxrImpl_SetTexture(xbox::dword_xt Stage, xbox::X_D3DBaseTexture* pTexture);
void CxbxrImpl_SwitchTexture(xbox::dword_xt Method, xbox::dword_xt Data, xbox::dword_xt Format);
void CxbxrImpl_SetTransform(xbox::X_D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix);
void CxbxrImpl_SetVertexData4f(int Register, FLOAT a, FLOAT b, FLOAT c, FLOAT d);
void CxbxrImpl_SetVertexShader(DWORD Handle);
#define USEPGRAPH_SetVertexShader 1
//all SetVertexShaderConstant variants are unpatched now. but we have to use CxbxrImpl_SetVertexShaderConstant() to handle the constant change in pgraph.
//void CxbxrImpl_GetVertexShaderConstant(INT Register, PVOID pConstantData, DWORD ConstantCount);
//void CxbxrImpl_SetVertexShaderConstant(INT Register, PVOID pConstantData, DWORD ConstantCount);
//void CxbxrImpl_SetVertexShaderInput(DWORD Handle, UINT StreamCount, xbox::X_STREAMINPUT* pStreamInputs);
//void WINAPI CxbxrImpl_SetVertexShaderInputDirect(xbox::X_VERTEXATTRIBUTEFORMAT* pVAF, UINT StreamCount, xbox::X_STREAMINPUT* pStreamInputs);
void CxbxrImpl_SetViewport(xbox::X_D3DVIEWPORT8* pViewport);
#define USEPGRAPH_SetViewport 1
// Present() also calls Swap(), patched LTCG version of Swap also calls Swap(). so we only handle Swap().
xbox::void_xt WINAPI CxbxrImpl_Present(xbox::dword_xt Flags);
#define USEPGRAPH_Present 1
DWORD CxbxrImpl_Swap(xbox::dword_xt Flags);
#define USEPGRAPH_Swap 1 /*Present() calles Swap() implementation. Present() is not permitted in pushbuffer recording*/
//void WINAPI CxbxrImpl_SwitchTexture(xbox::dword_xt Method, xbox::dword_xt Data, xbox::dword_xt Format);
#define PGRAPHUSE_EmuKickOff 0
#define PGRAPHUSE_EmuKickOffWait 0
// Warning: CxbxrImpl_MakeSpace() uses trampoline inside, do not use this function in pgraph. only use it in xbox/HLE patch.
xbox::dword_xt* CxbxrImpl_MakeSpace(void);
void CxbxrImpl_ReleaseRenderTarget(xbox::X_D3DSurface*pTarget, xbox::X_D3DSurface*pZbuffer);

// D3D api
void WINAPI CxbxrImpl_DestroyResource(xbox::X_D3DResource* pResource);


//helper functions
ULONG CxbxrImpl_Resource_AddRef(xbox::X_D3DResource* pResource);
ULONG CxbxrImpl_Resource_Release(xbox::X_D3DResource* pResource);
extern DWORD PBTokenArray[16];
void Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM token, int argCount, DWORD* argv);
/*
    // with this helper function, we can reduce the token pushbuffer setup in a further simpler way.
    
    //fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
    PBTokenArray[2] = (DWORD)StreamNumber;
    PBTokenArray[3] = (DWORD)pStreamData;
    PBTokenArray[4] = (DWORD)Stride;
    //give the correct token enum here, and it's done.
    Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetStreamSource, 3);//argCount, not necessary, default to 14
*/

/*
notes for my trials and errors.
Original idea was to use a virtual pushbuffer method to represent each patched HLE api and push the virtual pushbuffer method to pushbuffer then let pgraph method handler handle the patched HLE api later.
this should solve the sync issue between pushbuffer and HLE api.
So I introudced a new virtual method 0x00000080, with method count 15, NOINC, sub channel 0. each HLE api has a unique token represented with a enum X_D3DAPI_ENUM, which will be the first argument.
this left me 14 dword arguments and I put each arguments for the api call to the method arguments one by one.
but some apis needs more data to be pushed to pushbuffer when the api was called, for example, DrawVerticesUp(), which pushes all vertixes data to pushbuffer. In such case, we can't use this approach, unless we copied all necessary data to a seperate buffer for later use which is way too complicate.
so for such apis, I decide to unpatch them directly and create praph method handler instead.

these global variables are essential for converting xbox side status to vertex shader/pixel shader/host D3D status.

XboxRenderStateConverter XboxRenderStates; // this var directly access Xbox internal renderstate variables.
XboxTextureStateConverter XboxTextureStates; // this var directly access Xbox intern TextureState variables.

NV2ARenderStates and NV2ATextureStates are counter parts which we store states composed from NV2A/KelvinPrimitive.


D3D8LightState d3d8LightState = D3D8LightState();
D3D8TransformState d3d8TransformState = D3D8TransformState();
FixedFunctionVertexShaderState ffShaderState = {0}; // this var converts xbox states to vertex shader/pixel shader settings/constants.


D3DDevice_RunPushBuffer() must be unpatched to prevent reentrance of pgraph_handle_method()
DrawVertices/DrawVerticesUP/DrawIndexedXXX calls can't be HLEed inside pgraph because xbox pushes all vertex data directly into pushbuffer.
SetVertexShader/LoadVertexShader/SelectVertexShader/LoadVertexShaderProgram can't be HLEed, because xbox pushes all shader programs/vertex formats into pushbuffer.
SetVertexConstantsXXX variants are unpatched and handled inside pgraph because of the timing sync with acutal shader/draw calls.
extra EmuKickOff()/EmuKickOffWait() shall be removed to avoid PFIFO reentrance issue.// TODO: this might not be true, the real root cause should be the patched D3DDevice_RunPushBuffer()
MakeSpace() shall make sure there is a "this" pointer available before trampoline was called. // TODO: we shall check similar trampoline calls
g_pXbox_PixelShader=&g_Xbox_PixelShader which is a xbox pixel shader made up from contents of KelvinPrimitive.

Render States related pgraph methods/xbox calls
    NV097_SET_DOT_RGBMAPPING),               D3DRS_PSDOTMAPPING,       //XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_PSDOTMAPPING, pg->KelvinPrimitive.SetDotRGBMapping);
    NV097_SET_SHADER_OTHER_STAGE_INPUT),     D3DRS_PSINPUTTEXTURE,

    NV097_SET_DEPTH_FUNC),                   D3DRS_ZFUNC,              // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_ZFUNC, pg->KelvinPrimitive.SetDepthFunc);
    NV097_SET_ALPHA_FUNC),                   D3DRS_ALPHAFUNC,          // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_ALPHAFUNC, pg->KelvinPrimitive.SetAlphaFunc);
    NV097_SET_BLEND_ENABLE),                 D3DRS_ALPHABLENDENABLE,   // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_ALPHABLENDENABLE, pg->KelvinPrimitive.SetBlendEnable);
    NV097_SET_ALPHA_TEST_ENABLE),            D3DRS_ALPHATESTENABLE,    // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_ALPHATESTENABLE, pg->KelvinPrimitive.SetAlphaTestEnable);
    NV097_SET_ALPHA_REF),                    D3DRS_ALPHAREF,           // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_ALPHAREF, pg->KelvinPrimitive.SetAlphaRef);
    NV097_SET_BLEND_FUNC_SFACTOR),           D3DRS_SRCBLEND,           // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_SRCBLEND, pg->KelvinPrimitive.SetBlendFuncSfactor);
    NV097_SET_BLEND_FUNC_DFACTOR),           D3DRS_DESTBLEND,          // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_DESTBLEND, pg->KelvinPrimitive.SetBlendFuncDfactor);
    NV097_SET_DEPTH_MASK),                   D3DRS_ZWRITEENABLE,       // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_ZWRITEENABLE, pg->KelvinPrimitive.SetDepthMask);
    NV097_SET_DITHER_ENABLE),                D3DRS_DITHERENABLE,       // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_DITHERENABLE, pg->KelvinPrimitive.SetDitherEnable);
    NV097_SET_SHADE_MODE),                   D3DRS_SHADEMODE,          // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_SHADEMODE, pg->KelvinPrimitive.SetShadeMode);
    NV097_SET_COLOR_MASK),                   D3DRS_COLORWRITEENABLE,   // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_COLORWRITEENABLE, pg->KelvinPrimitive.SetColorMask);
    NV097_SET_STENCIL_OP_ZFAIL),             D3DRS_STENCILZFAIL,       // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_STENCILZFAIL, pg->KelvinPrimitive.SetStencilOpZfail);
    NV097_SET_STENCIL_OP_ZPASS),             D3DRS_STENCILPASS,        // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_STENCILPASS, pg->KelvinPrimitive.SetStencilOpZpass);
    NV097_SET_STENCIL_FUNC),                 D3DRS_STENCILFUNC,        // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_STENCILFUNC, pg->KelvinPrimitive.SetStencilFunc);
    NV097_SET_STENCIL_FUNC_REF),             D3DRS_STENCILREF,         // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_STENCILREF, pg->KelvinPrimitive.SetStencilFuncRef);
    NV097_SET_STENCIL_FUNC_MASK),            D3DRS_STENCILMASK,        // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_STENCILMASK, pg->KelvinPrimitive.SetStencilFuncMask);
    NV097_SET_STENCIL_MASK),                 D3DRS_STENCILWRITEMASK,   // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_STENCILWRITEMASK, pg->KelvinPrimitive.SetStencilMask);
    NV097_SET_BLEND_EQUATION),               D3DRS_BLENDOP,            // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_BLENDOP, pg->KelvinPrimitive.SetBlendEquation);
    NV097_SET_BLEND_COLOR),                  D3DRS_BLENDCOLOR,         // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_BLENDCOLOR, pg->KelvinPrimitive.SetBlendColor);
    NV097_SET_SWATH_WIDTH),                  D3DRS_SWATHWIDTH,         // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_SWATHWIDTH, pg->KelvinPrimitive.SetSwathWidth);
    NV097_SET_POLYGON_OFFSET_SCALE_FACTOR),  D3DRS_POLYGONOFFSETZSLOPESCALE,  // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_POLYGONOFFSETZSLOPESCALE, pg->KelvinPrimitive.SetPolygonOffsetScaleFactor);
    NV097_SET_POLYGON_OFFSET_BIAS),          D3DRS_POLYGONOFFSETZOFFSET,      // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_POLYGONOFFSETZOFFSET, pg->KelvinPrimitive.SetPolygonOffsetBias);
    NV097_SET_POLY_OFFSET_POINT_ENABLE),     D3DRS_POINTOFFSETENABLE,         // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_POINTOFFSETENABLE, pg->KelvinPrimitive.SetPolyOffsetPointEnable);
    NV097_SET_POLY_OFFSET_LINE_ENABLE),      D3DRS_WIREFRAMEOFFSETENABLE,     // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_WIREFRAMEOFFSETENABLE, pg->KelvinPrimitive.SetPolyOffsetPointEnable);
    NV097_SET_POLY_OFFSET_FILL_ENABLE),      D3DRS_SOLIDOFFSETENABLE,         // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_SOLIDOFFSETENABLE, pg->KelvinPrimitive.SetPolyOffsetPointEnable);

    NV097_SET_LINE_SMOOTH_ENABLE == NV097_SET_POLY_SMOOTH_ENABLE   D3DRS_EDGEANTIALIAS // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_EDGEANTIALIAS, pg->KelvinPrimitive.SetPolySmoothEnable);
    NV097_SET_SHADOW_DEPTH_FUNC(value-D3DCMP_NEVER(== 0x200) )     D3DRS_SHADOWFUNC    // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_SHADOWFUNC, pg->KelvinPrimitive.SetShadowDepthFunc + 0x200);
    NV097_SET_FOG_COLOR SwapRgb(Value)                             D3DRS_FOGCOLOR      // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_FOGCOLOR, xbox_fog_color);
    NV097_SET_CULL_FACE_ENABLE  D3DRS_CULLMODE    // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_CULLMODE, pg->KelvinPrimitive.SetCullFaceEnable);
    NV097_SET_CULL_FACE         D3DRS_FRONTFACE   // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_FRONTFACE, pg->KelvinPrimitive.SetCullFace);
    NV097_SET_NORMALIZATION_ENABLE  D3DRS_NORMALIZENORMALS  //XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_NORMALIZENORMALS, pg->KelvinPrimitive.SetNormalizationEnable);
    NV097_SET_DEPTH_TEST_ENABLE  NV097_SET_ZMIN_MAX_CONTROL D3DRS_ZENABLE could be D3DZB_FALSE(0), D3DZB_TRUE(1), D3DZB_USEW(2) depending on NV097_SET_ZMIN_MAX_CONTROL
        //XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_ZENABLE, pg->KelvinPrimitive.SetDepthTestEnable); 
    NV097_SET_STENCIL_TEST_ENABLE  D3DRS_STENCILENABLE  //  XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_STENCILENABLE, pg->KelvinPrimitive.SetStencilTestEnable);
    NV097_SET_STENCIL_OP_FAIL D3DRS_STENCILFAIL  //  XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_STENCILFAIL, pg->KelvinPrimitive.SetStencilOpFail);
    NV097_SET_COMBINER_FACTOR0 NV097_SET_COMBINER_FACTOR1 D3DRS_TEXTUREFACTOR=value only when fix funciton pixel shader in place // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_TEXTUREFACTOR, argv[0]);
    NV097_SET_LINE_WIDTH D3DRS_LINEWIDTH // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_LINEWIDTH, FtoDW(DWtoF(pg->KelvinPrimitive.SetLineWidth) / 8.0f));  // need SuperSampleScale
    NV097_NO_OPERATION with arg0= NVX_DXT1_NOISE_ENABLE, value stores in NV097_SET_ZSTENCIL_CLEAR_VALUE  D3DRS_DXT1NOISEENABLE  //XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_DXT1NOISEENABLE, pg->KelvinPrimitive.SetZStencilClearValue);
    NV097_SET_OCCLUDE_ZSTENCIL_EN  D3DRS_OCCLUSIONCULLENABLE D3DRS_STENCILCULLENABLE    //XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_OCCLUSIONCULLENABLE, 1); XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_STENCILCULLENABLE, (pg->KelvinPrimitive.SetOccludeZStencilEn & NV097_SET_OCCLUDE_ZSTENCIL_EN_OCCLUDE_STENCIL_EN_ENABLE));
    CommonSetDebugRegisters() D3DRS_ROPZCMPALWAYSREAD D3DRS_ROPZREAD D3DRS_DONOTCULLUNCOMPRESSED //
    CommonSetAntiAliasingControl NV097_SET_ANTI_ALIASING_CONTROL D3DRS_MULTISAMPLEANTIALIAS D3DRS_MULTISAMPLEMASK//XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_MULTISAMPLEANTIALIAS, pg->KelvinPrimitive.SetAntiAliasingControl& NV097_SET_ANTI_ALIASING_CONTROL_ENABLE_TRUE);
                    //XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_MULTISAMPLEMASK, (pg->KelvinPrimitive.SetAntiAliasingControl& NV097_SET_ANTI_ALIASING_CONTROL_ENABLE_TRUE)>>16);
    NV097_SET_LOGIC_OP_ENABLE D3DRS_LOGICOP // if (pg->KelvinPrimitive.SetLogicOp == 0) XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_LOGICOP, 0); else if (method_count == 2) XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_LOGICOP, argv[1]);
    NV097_SET_FRONT_POLYGON_MODE NV097_SET_BACK_POLYGON_MODE D3DRS_FILLMODE D3DRS_BACKFILLMODE D3DRS_TWOSIDEDLIGHTING //
    NV097_SET_SKIN_MODE D3DRS_VERTEXBLEND // XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_VERTEXBLEND, pg->KelvinPrimitive.SetSkinMode);
    LazySetLights()  NV097_SET_LIGHTING_ENABLE NV097_SET_SPECULAR_ENABLE NV097_SET_LIGHT_CONTROL NV097_SET_TWO_SIDE_LIGHT_EN D3DRS_LIGHTING NV097_SET_COLOR_MATERIAL NV097_SET_LIGHT_LOCAL_RANGE NV097_SET_LIGHT_LOCAL_POSITION NV097_SET_LIGHT_ENABLE_MASK D3DRS_SPECULARENABLE D3DRS_TWOSIDEDLIGHTING D3DRS_LOCALVIEWER 
    LazySetTransform()
    LazySetPointParams() NV097_SET_POINT_PARAMS_ENABLE/D3DRS_POINTSCALEENABLE  NV097_SET_POINT_SMOOTH_ENABLE/D3DRS_POINTSPRITEENABLE NV097_SET_POINT_SIZE NV097_SET_POINT_PARAMS/D3DRS_POINTSIZE D3DRS_POINTSIZE_MIN D3DRS_POINTSIZE_MAX D3DRS_POINTSCALE_A D3DRS_POINTSCALE_B D3DRS_POINTSCALE_C
    LazySetCombiners()

    double check:
    1. pixel shader set/update: done
    2. vertex shader set/update
    3. vertex format/vertex buffer/stream srouce
    CxbxrImpl_SetVertexShaderInput() affects these globals/routines.
        GetXboxVertexAttributeFormat(),  uses g_Xbox_SetVertexShaderInput_Count g_Xbox_SetVertexShaderInput_Attributes : using set_IVB_DECL_override(), g_InlineVertexBuffer_DeclarationOverride = &g_NV2AVertexAttributeFormat; use pgraph vertex data array format setup g_NV2AVertexAttributeFormat
            UpdateFixedFunctionVertexShaderState()/CxbxGetVertexDeclaration()
        GetXboxVertexStreamInput() g_Xbox_SetVertexShaderInput_Data g_Xbox_SetVertexShaderInput_Count : pDrawContext->pXboxVertexStreamZeroData set to pgraph vertex data array offset or inline array vertex buffer can avoid using GetXboxVertexStreamInput()
            CountActiveD3DStreams()/CxbxVertexBufferConverter::ConvertStream
        
    4. xbox draw call internals and pgraph instructions.

        DrawVerticesUP
            SetStateUP() // compose g_NV2AVertexAttributeFormat with NV097_SET_VERTEX_DATA_ARRAY_FORMAT(i), calculate vertex stride, set DrawContext->pXboxVertexStreamZeroData to inline array vertex buffer.
            Push1(pPush, NV097_SET_BEGIN_END, PrimitiveType);
            PushCount(pPush++, PUSHER_NOINC(NV097_INLINE_ARRAY), count);
            Push1(pPush, NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END);

        DrawIndexedVerticesUP
            SetStateUP(); // compose g_NV2AVertexAttributeFormat with NV097_SET_VERTEX_DATA_ARRAY_FORMAT(i), calculate vertex stride, set DrawContext->pXboxVertexStreamZeroData to inline array vertex buffer.
            Push1(pPush, NV097_SET_BEGIN_END, PrimitiveType);
            PushCount(pPush++, PUSHER_NOINC(NV097_INLINE_ARRAY), count);
            Push1(pPush, NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END);

        DrawVertices
            SetStateVB(0); // compose g_NV2AVertexAttributeFormat and vertex stride with NV097_SET_VERTEX_DATA_ARRAY_FORMAT(i), set DrawContext->pXboxVertexStreamZeroData to NV097_SET_VERTEX_DATA_ARRAY_OFFSET(i)
            Push1(pPush, NV097_SET_BEGIN_END, PrimitiveType);
            PushCount(pPush + 2, PUSHER_NOINC(NV097_DRAW_ARRAYS), drawArraysCount);
            while (VertexCount > DRAW_COUNT_BATCH)
            {
                *(pPush + 3) = DRF_NUMFAST(097, _DRAW_ARRAYS, _COUNT, DRAW_COUNT_BATCH - 1)
                             | DRF_NUMFAST(097, _DRAW_ARRAYS, _START_INDEX, StartVertex);

                pPush++;
                StartVertex += DRAW_COUNT_BATCH;
                VertexCount -= DRAW_COUNT_BATCH;
            }
            *(pPush + 3) = DRF_NUMFAST(097, _DRAW_ARRAYS, _COUNT, VertexCount - 1)
                 | DRF_NUMFAST(097, _DRAW_ARRAYS, _START_INDEX, StartVertex);

            Push1(pPush + 4, NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END);

        DrawIndexedVertices  // compose g_NV2AVertexAttributeFormat and vertex stride with NV097_SET_VERTEX_DATA_ARRAY_FORMAT(i), set DrawContext->pXboxVertexStreamZeroData to NV097_SET_VERTEX_DATA_ARRAY_OFFSET(i)
                             // compose index buffer from NV097_ARRAY_ELEMENT16/NV097_ARRAY_ELEMENT32
            SetStateVB(IndexBase);
            Push1(pPush, NV097_SET_BEGIN_END, PrimitiveType);
                PushCount(pPush, PUSHER_NOINC(NV097_ARRAY_ELEMENT16), arrayCount);//put two indexes in one dword, send in via multiple method_count.
                Push1(pPush, NV097_ARRAY_ELEMENT32, (DWORD) *pIndexData); //optional if there were odd vertex index, handle the very last index.
            Push1(pPush, NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END);

        SetStateUP()  // set vertex format with  sizeAndType but without vertex stride. NV097_SET_VERTEX_DATA_ARRAY_OFFSET(0) is not set because all vertex attributes shall be pushed to pushbuffer with NV097_INLINE_ARRAY
            PushCount(pPush++, NV097_SET_VERTEX_DATA_ARRAY_FORMAT(0), 16);
                sizeAndType = pVertexShader->Slot[slot].SizeAndType;
                *pPush++ = sizeAndType;

        SetStateVB(pIndexBase) // set vertex format with sizeAndType and vertex stride<<8. NV097_SET_VERTEX_DATA_ARRAY_OFFSET(0) is set to each stream's vertex buffer source offset.
            PushCount(pPush++, NV097_SET_VERTEX_DATA_ARRAY_FORMAT(0), 16);
                *pPush++ = pVertexShader->Slot[slot].SizeAndType
                    + (g_Stream[pVertexShader->Slot[slot].StreamIndex].Stride << 8);
            
                offset = GetGPUDataFromResource(pStream->pVertexBuffer)
                                     + pVertexShader->Slot[slot].Offset
                                     + pStream->Offset;
           if (IndexBase != 0)
               offset += IndexBase * pStream->Stride;
           Push1(pPush,
                 NV097_SET_VERTEX_DATA_ARRAY_OFFSET(i),
                 offset);
   
do not trampoline to xbox code from pgraph handler. this will cause pushbuffer data corruption.

    */

