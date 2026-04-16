# dx11 Branch: D3D9 Build Impact Report

**Branch:** `dx11` (b488d989b) vs `origin/master` (9370f1cc0)
**Scope:** 141 commits, 46 files, +5547/−1043 lines

Changes inside `#ifdef CXBX_USE_D3D11` blocks have **zero effect** on D3D9 builds and are only noted by size. Everything else is analyzed in detail.

---

## 1. Bug Fixes (affect D3D9)

| Fix | File | Detail |
|-----|------|--------|
| **RenderState float reinterpretation** | XbPixelShader.cpp, Direct3D9.cpp | `GetXboxRenderStateAsFloat()` used `reinterpret_cast<float*>` on integer DWORD values (`SPECULARENABLE`, `FOGENABLE`, `FOGTABLEMODE`). Integer `1` became `~1.4e-45` instead of `1.0f`. Fixed with `static_cast<float>(GetXboxRenderState())`. |
| **Fog density/start/end** | Direct3D9.cpp | Now correctly uses `GetXboxRenderStateAsFloat()` (these states store IEEE 754 float bit patterns) instead of manual `reinterpret_cast`. |
| **Operator precedence** | Direct3D9.cpp `EmuMsgProc` | `if (g_renderbase && g_renderbase->IsImGuiFocus() \|\| forceShow)` → added parentheses around the `&&` clause. |
| **Null checks for `g_renderbase`** | Direct3D9.cpp, EmuNV2A_PGRAPH.cpp | Three locations: `ToggleImGui()`, `Render()`, and NV2A PGRAPH ImGui call — all now guarded against null `g_renderbase`. |
| **Cubemap face iteration off-by-one** | Direct3D9.cpp | `face <= numFaces` → `face < numFaces` in `GetSurfaceFaceAndLevelWithinTexture`. |
| **Clear rects null check** | Direct3D9.cpp | `if (pRects != nullptr)` → `if (Count > 0 && pRects != nullptr)`. |
| **DSOUND DWORD×float precision** | DirectSoundGlobal.cpp | `bufferRangeStart *= scale` was DWORD × float stored back to DWORD (truncation). Now uses intermediate `float fBufferRangeStart` variable. |
| **Pixel shader state tracking** | XbPixelShader.cpp | Removed `GetPixelShader()` + compare + `SetPixelShader()` pattern. New `CxbxSetPixelShader()` helper avoids redundant state sets via internal tracking. |

---

## 2. Warning Fixes (affect D3D9)

| Category | Files | Detail |
|----------|-------|--------|
| uint32↔float casts | Direct3D9.cpp | Fog values, viewport DWORD→float, scissor rect float→LONG — all explicit `static_cast`. |
| float→int casts | XbPixelShader.cpp | Stage state fields cast to `int` for enum switch. |
| signed/unsigned | Shader.cpp, FixedFunctionState.cpp | `int` → `size_t` loop variables and comparison casts. |
| uint64→uint32 truncation | CxbxUtil.cpp | `Muldiv64()` return fields: explicit `static_cast<uint32_t>`. |
| double→float | DirectSoundGlobal.cpp | `1.0` → `1` in `height * 1.0f`. |
| double→uint64 | nv2a.cpp | vblank period: `static_cast<uint64_t>(16.6666666667 * 1000)`. |
| Typo fix | EmuKrnlIo.cpp | `identifable` → `identifiable` in comment. |
| `{0}` → `{}` | Direct3D9.cpp | Value-initialization for `ffShaderState`. |
| float literals | Direct3D9.cpp | `MinZ = 0` → `0.0f`, `MaxZ = 1` → `1.0f`. |

---

## 3. Type Abstraction Layer (affect D3D9 — structural, not behavioral)

The branch introduces an abstraction layer so the same source compiles against both D3D9 and D3D11 headers. In D3D9 builds, all these resolve to their original D3D9 types via typedefs/defines.

**New macros:**
- `_9_11(d3d9_val, d3d11_val)` → picks first arg in D3D9
- `EMUFORMAT` → `D3DFORMAT`
- 36 `EMUFMT_*` constants → `D3DFMT_*`

**Type aliases (D3D9 mode = original type):**
- `IDirect3DDevice` → `IDirect3DDevice9Ex`
- `IDirect3DPixelShader` → `IDirect3DPixelShader9`
- `IDirect3DVertexShader` → `IDirect3DVertexShader9`
- `D3DSurfaceDesc` → `D3DSURFACE_DESC`
- `D3DVIEWPORT` → `D3DVIEWPORT9`
- `D3DVERTEXELEMENT` → `D3DVERTEXELEMENT9`
- ~15 interface types: `IDirect3DTexture`, `IDirect3DSurface`, `IDirect3DVertexBuffer`, `IDirect3DIndexBuffer`, `IDirect3DVolumeTexture`, `IDirect3DCubeTexture`, `IDirect3DVolume`, `IDirect3DQuery`, `IDirect3DResource`, `IDirect3DBaseTexture`, `IDirect3DVertexDeclaration`

**Xbox types decoupled from D3D9 headers (same memory layout, distinct C++ types):**

| New standalone type | Was (master) |
|---|---|
| `X_D3DLIGHT8` | `typedef D3DLIGHT9` |
| `X_D3DMATERIAL8` | `typedef D3DMATERIAL9` |
| `X_D3DVIEWPORT8` | `typedef D3DVIEWPORT9` |
| `X_D3DSWAPEFFECT` | `typedef D3DSWAPEFFECT` |
| `X_D3DCOLORVALUE` | `typedef D3DCOLORVALUE` |
| `X_D3DLIGHTTYPE` | `typedef D3DLIGHTTYPE` |
| `X_D3DSTATEBLOCKTYPE` | `typedef D3DSTATEBLOCKTYPE` |

**New Xbox types (typedef to Win32/D3D9 in D3D9 mode):**

`X_HWND`, `X_RECT`, `X_POINT`, `X_D3DPOOL`, `X_D3DCOLOR`, `X_D3DVECTOR`, `X_D3DMATRIX`, `X_D3DRECT`, `X_D3DBOX`, `X_D3DLOCKED_RECT`, `X_D3DLOCKED_BOX`, `X_D3DBACKBUFFER_TYPE`, `X_D3DBASISTYPE`, `X_D3DDEGREETYPE`, `X_D3DDEVTYPE`, `X_D3DCUBEMAP_FACES`, `X_D3DDEVICE_CREATION_PARAMETERS`, `X_D3DRECTPATCH_INFO`, `X_D3DTRIPATCH_INFO`

All function signatures across `Direct3D9.cpp/h`, `XbPixelShader.cpp`, `XbVertexShader.cpp`, `XbConvert.cpp/h`, `FixedFunctionState.cpp/h`, `XbD3D8Logging.cpp/h`, `XGraphic.cpp/h` updated to use these abstractions.

---

## 4. Rendering Logic Changes (affect D3D9)

| Change | Detail |
|--------|--------|
| **SetTransform no longer calls host D3D9** | `g_pD3DDevice->SetTransform()` removed. Only updates internal `d3d8TransformState`. Comment: "handled in our fixed function shader." |
| **SetLight no longer calls host D3D9** | `g_pD3DDevice->SetLight()` removed. Only updates `d3d8LightState`. |
| **LightEnable no longer calls host D3D9** | `g_pD3DDevice->LightEnable()` removed. Uses `d3d8LightState.EnableLight()`. |
| **SetMaterial now calls trampoline** | Added `XB_TRMP(D3DDevice_SetMaterial)(pMaterial)`. Removed `g_pD3DDevice->SetMaterial()`. |
| **GetMaterial removed** | Function deleted from active code (moved to `.unused-patches`). Patch entry commented out. |
| **SetBackMaterial added** | New function + patch entry populating `ffShaderState.Materials[1]` (back material). |
| **Fixed function shader condition simplified** | `g_Xbox_VertexShaderMode == FixedFunction && g_UseFixedFunctionVertexShader` → just `g_Xbox_VertexShaderMode == FixedFunction`. |
| **Update order swapped** | `CxbxUpdateHostVertexShader()` now runs BEFORE `CxbxUpdateHostVertexDeclaration()` (was reversed). |
| **Matrix multiply** | `Out = mtxProjection * mtxViewport` → `D3DXMatrixMultiply(&Out, &mtxProjection, &mtxViewport)`. |
| **TexCoordComponentCount access** | Direct `ffShaderState.TexCoordComponentCount[i]` → `reinterpret_cast<float*>(&ffShaderState.TexCoordComponentCount)[i]` — implies underlying type changed. |
| **ffShaderState error** | `CxbxrAbort("Failed to write...")` → TODO comment (no longer fatal). |
| **EmuPC2XB_D3DFormat removed** | Single remaining caller restructured to skip the host→Xbox format round-trip entirely — selects Xbox format directly. |

---

## 5. New Dispatch Helpers (affect D3D9 — thin wrappers calling original D3D9 API)

These are `_9_11` helpers with full D3D11 implementations in the `#ifdef` path. In D3D9 builds, they just call the original D3D9 API:

`CxbxSetRenderTarget`, `CxbxD3DClear`, `CxbxSetViewport`, `CxbxSetScissorRect`, `CxbxSetIndices`, `CxbxLockIndexBuffer`, `CxbxUnlockIndexBuffer`, `CxbxDrawIndexedPrimitive`, `CxbxDrawPrimitive`, `CxbxBltSurface`, `CxbxPresent`, `CxbxSetVertexShaderConstantF`, `CxbxSetPixelShaderConstantF`, `CxbxSetPixelShader`, `CxbxSetVertexShader`, `CxbxCreateVertexShader`, `CxbxCreateHostVertexDeclaration`, `CxbxSetStreamSource`, `CxbxBeginScene`, `CxbxEndScene`, `CxbxQueryIssueBegin/End/GetData`, `SetHostSurface`, `SetHostTexture`, `SetHostVolumeTexture`, `SetHostCubeTexture`

---

## 6. Format Mapping Changes (affect D3D9)

| Xbox Format | Master | dx11 branch | Risk |
|---|---|---|---|
| `X_D3DFMT_R8B8` (0x29) | `D3DFMT_R5G6B5` | `EMUFMT_A8L8` | **Changed** |
| `X_D3DFMT_LIN_R8B8` (0x16) | `D3DFMT_R5G6B5` | `EMUFMT_A8L8` | **Changed** |
| `X_D3DFMT_LIN_G8B8` (0x17) | `D3DFMT_R5G6B5` | `EMUFMT_A8L8` | **Changed** |
| `X_D3DFMT_F16` (0x2D) | `D3DFMT_D16` | `EMUFMT_R16F` | **Changed** |
| `X_D3DFMT_LIN_F16` (0x31) | `D3DFMT_D16` | `EMUFMT_R16F` | **Changed** |

Note: All other format mappings changed from `D3DFMT_*` to `EMUFMT_*` names but resolve to the same D3D9 values via the `_9_11` macro.

---

## 7. Removed Symbols

| Symbol | Was | Notes |
|---|---|---|
| `g_UseFixedFunctionVertexShader` | Global `bool` | No remaining references — safe. |
| `EmuXB2PC_D3DTS()` | Transform state converter | No remaining references (callers updated). |
| `EmuPC2XB_D3DFormat()` | Format reverse converter | replaced by direct Xbox format selection at call site. |
| `VertexShaderCache::ResetD3DDevice()` | Reset device pointer | Replaced by `Clear()`. No remaining callers. |
| `VertexShaderCache::pD3DDevice` | Member | Uses global `g_pD3DDevice`. |
| `D3DDevice_GetMaterial` | Patch function | Moved to `.unused-patches`, patch entry commented out. |

---

## 8. Disabled Patches (affect D3D9)

| Patch | Status |
|---|---|
| D3DDevice_GetCreationParameters | Moved to `.unused-patches` |
| D3DDevice_SetScissors | Moved to `.unused-patches` |
| D3DDevice_CreateStateBlock | Moved to `.unused-patches` |
| D3DDevice_GetMaterial | Moved to `.unused-patches`, patch entry commented out |

**New patch added:** `D3DDevice_SetBackMaterial`

---

## 9. Build System Changes (affect D3D9)

| Change | Effect |
|---|---|
| `CXBX_USE_D3D11` option | Defined in both top-level and `cxbxr-emu` `CMakeLists.txt`. Defaults ON — plain `cmake` builds D3D11. D3D9 requires `-DCXBX_USE_D3D11=OFF`. |
| imgui backend selection | `CMakeLists.txt` conditionally selects `imgui_impl_dx11` or `imgui_impl_dx9` based on `CXBX_USE_D3D11`. |
| D3D9/D3D11 link libraries | `d3d9`, `d3dx9`, `d3dcompiler` removed from unconditional list. Now selected conditionally: D3D11 links `d3d11 dxgi d3dcompiler`; D3D9 links `d3d9 d3dx9 d3dcompiler`. |
| `/ignore:4099` on cxbxr-emu | Suppresses distorm PDB warning. |
| `/wd` flags | libusb C5287; capstone C4018/C4244/C4267. |
| New source files | `Backend_D3D11.cpp` and `PatchDraw.cpp` added to build. |

---

## 10. Non-D3D Changes (affect all builds)

| Area | Change |
|---|---|
| **EmuKrnl.h** | New `g_InterruptSignal` (`condition_variable`) and `g_AnyInterruptAsserted` (`atomic_bool`) globals. Adds `<condition_variable>` and `<mutex>` includes. |
| **CxbxKrnl.cpp** | Six new `EmuLogInit` diagnostic lines throughout init sequence (LLE flags, HLE intercept, per-title keys, FS init, x86 init, thread launch). |
| **nv2a_regs.h** | ~250 lines of new NV2A register defines (point params, materials, stipple, vertex arrays — from xemu). |
| **EmuNV2A_PGRAPH.cpp** | `g_renderbase` null check added around ImGui render call. |
| **nv2a.cpp** | `vblank_period` double→uint64 explicit cast. |
| **WndMain.cpp** | `XboxFormat` → `XbFormat` rename. Added `XbConvert.h` include (was implicitly included via `Direct3D9.h`). |

---

## 11. D3D11-Only Code (no D3D9 impact — size summary)

| Area | Lines |
|---|---|
| XbD3D8Types.h compatibility layer | ~615 |
| Direct3D9.cpp D3D11 blocks | ~750 |
| Direct3D9.h D3D11 blocks | ~45 |
| XbVertexShader.cpp D3D11 blocks | ~250 |
| Backend_D3D11.cpp/h (entirely D3D11) | ~470 |
| PatchDraw.cpp/h (entirely D3D11) | ~564 |
| RenderStates/TextureStates D3D11 | ~200 |
| VertexShaderCache D3D11 | ~30 |
| HLSL D3D11 defines | ~20 |
| DlgVideoConfig.cpp DXGI path | ~100 |
| XbVertexBuffer.cpp D3D11 blocks | ~80 |
| XbPixelShader.cpp D3D11 blocks | ~40 |
| **Total D3D11-only** | **~3164** |

---

## 12. Submodule Updates

| Submodule | Change |
|---|---|
| `import/mio` | Updated (+2 commits) — fixes multiply-defined `s_2_ws` linker error. |
| `import/nv2a_vsh_cpu` | Updated (+6 commits) — ILU float fix, default case handling, CI workflow. |

---

## 13. Items Needing Attention

| Priority | Issue |
|---|---|
| **MEDIUM** | Format mapping changes (R8B8→A8L8, F16→R16F) may alter rendering for affected textures. Verify intentional. |
| **MEDIUM** | Disabled patches (`SetScissors`, `CreateStateBlock`, `GetCreationParameters`) may affect games relying on them. |
| **MEDIUM** | `SetTransform`/`SetLight`/`LightEnable` no longer call host D3D9 — rendering depends entirely on custom fixed-function shader now. |
| **LOW** | `g_InterruptSignal`/`g_AnyInterruptAsserted` globals added but not yet used outside declarations — future interrupt-driven architecture. |
