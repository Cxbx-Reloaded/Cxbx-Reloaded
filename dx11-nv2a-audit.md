# D3D11 NV2A Translation Audit

Comprehensive audit of every D3D11 code path that could produce different visual results from the NV2A hardware. Conducted across all rendering pipeline stages: vertex shaders, pixel shaders, render state, output merger, texture/sampler, and geometry/draw.

---

## Critical (Game-Breaking on Affected Titles)

### 1. Alpha Test Not Implemented in D3D11 Shaders
- [x] Fixed
- **Files**: `Backend_D3D11.cpp:475`, `FixedFunctionPixelShader.hlsl`, `CxbxPixelShaderTemplate.hlsl`
- **Description**: `X_D3DRS_ALPHATESTENABLE`, `X_D3DRS_ALPHAREF`, `X_D3DRS_ALPHAFUNC` are grouped under "states handled as shader constants" at Backend_D3D11.cpp:475, but neither pixel shader reads or applies them. D3D11 has no fixed-function alpha test. The existing `AlphaKill` and `ColorKey` operations are per-texture-stage mechanisms, not the global alpha test. Any game using `D3DRS_ALPHATESTENABLE` will render all fragments regardless of alpha comparison.
- **Impact**: 100% of titles using alpha test (vegetation, fences, HUD elements, etc.)

### 2. ~~Multiple Render Targets — Only 1 Bound~~ (N/A)
- [x] Fixed
- **Files**: `Backend_D3D11.cpp:560` (and all `OMSetRenderTargets` calls)
- **Description**: All `OMSetRenderTargets` calls pass `1` as the count. Xbox supports 0–4 simultaneous render targets. Games using MRT will only write to target 0.
- **Resolution**: N/A — Xbox D3D8 API `SetRenderTarget(pRenderTarget, pNewZStencil)` only supports a single render target. MRT is not exposed through the Xbox SDK. The `1` in `OMSetRenderTargets` is correct.
- **Impact**: None

### 3. ~~Index Buffer Always 16-bit~~ (N/A)
- [x] Fixed
- **Files**: `Backend_D3D11.cpp:630`, `HostDraw.cpp:403`, `EmuPatches_Draw.cpp:419,648`
- **Description**: Every `IASetIndexBuffer` call uses `DXGI_FORMAT_R16_UINT`. If any Xbox title uses 32-bit indices via `D3DFMT_INDEX32`, vertex indices > 65535 wrap and produce corrupted geometry.
- **Resolution**: N/A — Xbox NV2A hardware only supports 16-bit indices. `X_D3DFMT_INDEX16` is the only index format defined (and it's noted as 'not an Xbox format, used internally'). The 16-bit hardcoding is correct.
- **Impact**: None

### 4. FrontFace Render State Not Handled
- [x] Fixed
- **Files**: `Backend_D3D11.cpp:364`, `IndexBufferConvert.cpp:69`
- **Description**: `X_D3DRS_FRONTFACE` is never mapped to `g_D3D11RasterizerDesc.FrontCounterClockwise`. It defaults to `FALSE` (CW = front). Games changing front face will get wrong culling AND wrong `SV_IsFrontFace` in pixel shaders, causing front/back color selection errors. Related: `bUseClockWiseWindingOrder` in IndexBufferConvert.cpp is hardcoded `true` with a TODO.
- **Impact**: Games using CCW front face render inside-out, wrong diffuse/specular colors

### 5. MSAA Disabled — Hardcoded SampleCount = 1
- [ ] Fixed
- **Files**: `HostResourceCreate.cpp`
- **Description**: All textures/render targets use `SampleDesc.Count = 1`. No MSAA resolve infrastructure exists. All anti-aliased rendering by Xbox titles is lost.
- **Impact**: Visual quality loss; aliased edges everywhere

---

## High (Visible on Many Titles)

### 6. ~~Vertex Format: NORMPACKED3 Broken~~ (N/A)
- [x] Fixed
- **Files**: `XbVertexShaderDecoder.cpp:536`, `XbVertexBuffer.cpp`
- **Description**: NORMPACKED3 is a single packed 32-bit integer with 11/11/10-bit signed components. The code reads it as 3×FLOAT32, producing garbage normals.
- **Resolution**: N/A — D3D11 path correctly unpacks 11/11/10-bit fields via bitfield union into 3 floats using `PackedIntToFloat()`. Maps to `DXGI_FORMAT_R32G32B32_FLOAT`.
- **Impact**: None

### 7. ~~Vertex Format: NORMSHORT3 Uninitialized W~~ (N/A)
- [x] Fixed
- **Files**: `XbVertexShaderDecoder.cpp:545`, `XbVertexBuffer.cpp:598`
- **Description**: NORMSHORT3 reads 3 signed shorts but leaves the 4th component (W) uninitialized, resulting in a garbage W value.
- **Resolution**: N/A — D3D11 path maps to `DXGI_FORMAT_R16G16B16A16_SNORM` and explicitly sets `pHostVertexAsShort[3] = 32767` (=1.0 in SNORM).
- **Impact**: None

### 8. ~~Vertex Format: PBYTE3 Missing Alpha~~ (N/A)
- [x] Fixed
- **Files**: `XbVertexShaderDecoder.cpp:628`, `XbVertexBuffer.cpp:620`
- **Description**: PBYTE3 does not explicitly set alpha to 1.0, leaving it undefined.
- **Resolution**: N/A — D3D11 path maps to `DXGI_FORMAT_R8G8B8A8_UNORM` and explicitly sets `pHostVertexAsByte[3] = 255` (=1.0 in UNORM).
- **Impact**: None

### 9. ~~Vertex Format: PBYTE1/PBYTE2 Missing Expansion~~ (N/A)
- [x] Fixed
- **Files**: `XbVertexBuffer.cpp:554`
- **Description**: No format expansion for signed byte formats PBYTE1 and PBYTE2.
- **Resolution**: N/A — D3D11 uses native `DXGI_FORMAT_R8_UNORM` / `DXGI_FORMAT_R8G8_UNORM` which provide correct default component values (0 for missing RGB, 1 for missing A). No expansion needed.
- **Impact**: None

### 10. Fog Coordinate Not Clamped in VS
- [x] Fixed
- **Files**: `CxbxVertexShaderTemplate.hlsl:289`
- **Description**: Comment says "TODO *NEEDS TESTING*" for fog coordinate output. NV2A clamps fog factor to [0,1]; the shader may pass unclamped values to the pixel shader.
- **Impact**: Over-fogged or under-fogged scenes

### 11. Front/Back Face Detection: D3D11 bool vs D3D9 float
- [ ] Fixed
- **Files**: `CxbxPixelShaderTemplate.hlsl:487`
- **Description**: D3D11 `SV_IsFrontFace` is a `bool`, converted via `(xIn.iFF ? 1.0f : -1.0f) * FRONTFACE_FACTOR >= 0`. This works for normal cases but edge behavior around degenerate triangles may differ from D3D9's float VFACE semantic.
- **Impact**: Potential wrong front/back color on degenerate triangles

### 12. PS Register Combiner: Input Modifier Precision (8-bit vs float32)
- [x] Fixed
- **Files**: `CxbxPixelShaderTemplate.hlsl:35-43`
- **Description**: NV2A register combiners operate at 8-bit color precision (per-channel clamp, quantized). The shader operates at float32. Operations like `s_bx2(x) = 2*max(0,x) - 1` and `s_bias(x) = max(0,x) - 0.5` produce slightly different results without 8-bit quantization between stages. Comment: "TODO : Should all these 'max(0, x)' actually be 'saturate(x)'?"
- **Impact**: Subtle color banding/precision differences in combiner-heavy shaders

### 13. PS: Complement Operator Post-Clamping Wrong
- [x] Fixed
- **Files**: `CxbxPixelShaderTemplate.hlsl:37`
- **Description**: `s_comp(x) = 1 - max(0, x)` — NV2A does `1 - clamp(x, 0, 1)` (saturate). If x > 1, the shader produces a negative result; NV2A would produce 0.
- **Impact**: Incorrect color values when combiner inputs exceed 1.0

### 14. ~~COLORWRITEENABLE Only for RT[0]~~ (N/A)
- [x] Fixed
- **Files**: `Backend_D3D11.cpp:465`
- **Description**: Only `g_D3D11BlendDesc.RenderTarget[0].RenderTargetWriteMask` is set. RT[1-3] use default masks. Will matter when MRT is fixed.
- **Resolution**: N/A — Xbox D3D8 only supports a single render target (see issue #2). RT[0] is the only active target.
- **Impact**: None

### 15. Quad-to-Triangle Winding Order Hardcoded
- [x] Fixed
- **Files**: `IndexBufferConvert.cpp:69`
- **Description**: `bUseClockWiseWindingOrder = true` with TODO comment. Should read `X_D3DRS_FRONTFACE` at conversion time. Games using CCW front face get incorrect quad splitting diagonal.
- **Impact**: Wrong triangle edges visible on quad geometry with CCW winding

### 16. Depth Bias Precision Loss
- [x] Fixed
- **Files**: `Backend_D3D11.cpp:375`
- **Description**: `g_D3D11RasterizerDesc.DepthBias = (INT)Value` — Xbox ZBIAS is a float-encoded DWORD; casting to INT truncates the fractional component.
- **Impact**: Shadow acne or Z-fighting where depth bias should fix it

### 17. ~~LINELOOP Closure Edge Missing~~ (N/A)
- [x] Fixed
- **Files**: `XbConvert.cpp:366` (D3D topology mapping)
- **Description**: Xbox `X_D3DPT_LINELOOP` mapped to `D3D_PRIMITIVE_TOPOLOGY_LINESTRIP`. The closing edge (last vertex → first vertex) is never drawn.
- **Resolution**: N/A — `CxbxDrawIndexedClosingLine()` / `CxbxDrawIndexedClosingLineUP()` already draw the closing edge after the linestrip when `X_D3DPT_LINELOOP` is detected. See `HostDraw.cpp:343-361`.
- **Impact**: None

### 18. ~~DOT_PRODUCT3 Bias/Scale Timing~~ (N/A)
- [x] Fixed
- **Files**: `FixedFunctionPixelShader.hlsl:189`
- **Description**: Bias/scale may be applied at the wrong point in the DOT_PRODUCT3 calculation relative to NV2A hardware order.
- **Resolution**: N/A — Per D3D9/Xbox SDK docs, `D3DTOP_DOTPRODUCT3` is defined to automatically expand inputs from [0,1] to [-1,1]. The bias/scale at DOT_PRODUCT3 time is the correct and intended behavior. Misleading TODO comment cleaned up.
- **Impact**: None

### 19. ~~BUMPENVMAP Source Register Uncertain~~ (N/A)
- [x] Fixed
- **Files**: `FixedFunctionPixelShader.hlsl:199-209`
- **Description**: Comment "TODO : Verify" on which register provides the bump environment map source.
- **Resolution**: N/A — Using `ctx.CURRENT` (previous stage result) is correct per D3D9/Xbox SDK. Bump perturbation data comes from the processed pipeline result. Misleading TODO comment cleaned up.
- **Impact**: None

---

## Moderate (Edge Cases, Specific Titles)

### 20. Volume Texture Support Incomplete
- [ ] Fixed
- **Files**: `HostResourceCreate.cpp:477`
- **Description**: Contains "TODO : Implement standalone volume creation" — 3D texture creation may fall back to 2D.
- **Impact**: 3D textures (fog volumes, 3D LUTs) non-functional

### 21. Fog Calculation: `if` Instead of `else if`
- [x] Fixed
- **Files**: `FixedFunctionPixelShader.hlsl:329`, `CxbxPixelShaderTemplate.hlsl:449`
- **Description**: Both pixel shaders use chained `if` for fog mode instead of `else if`. Functionally correct since only one mode matches at a time, but `fogFactor` would be uninitialized if `FogTableMode` has an unexpected value. The FF PS also has a redundant `FogEnable == 0` check inside the else branch.
- **Impact**: Cosmetic code issue; potential uninitialized value on invalid fog mode

### 22. Texture Filter Fallbacks
- [x] Fixed
- **Files**: `TextureStates.cpp:224-233`
- **Description**: `X_D3DTEXF_QUINCUNX` → `D3DTEXF_ANISOTROPIC`, `X_D3DTEXF_GAUSSIANCUBIC` → `D3DTEXF_GAUSSIANQUAD`. Both are approximations, not exact Xbox filtering behavior. Copy-paste bug in log message (said QUINCUNX for GAUSSIANCUBIC).
- **Resolution**: Fixed log message. Filter approximations are the best available on D3D11 hardware.
- **Impact**: Slightly different texture appearance; blur/sharpness mismatch

### 23. ~~Border Color Not Propagated to D3D11 Sampler~~ (N/A)
- [x] Fixed
- **Files**: `TextureStates.cpp:289`
- **Description**: `X_D3DTSS_BORDERCOLOR` marked "handled elsewhere" but no D3D11 sampler `BorderColor` assignment found. Textures with `CLAMP_TO_BORDER` address mode will use black (default) instead of the game-specified border color.
- **Resolution**: N/A — Border color IS correctly propagated in the D3D11 sampler creation path at `TextureStates.cpp:342-348`, setting `samplerDesc.BorderColor[0..3]` from the converted BORDERCOLOR value. The audit missed this code path.
- **Impact**: None

### 24. BRDF Texture Mode Incomplete
- [ ] Fixed
- **Files**: `CxbxPixelShaderTemplate.hlsl:398`
- **Description**: "TODO : Complete 16 bit phi/sigma retrieval" — BRDF texture mode not fully implemented.
- **Impact**: Titles using BRDF-based lighting via texture combiners

### 25. DOT_ST / DOT_ZW Texture Modes Untested
- [x] Fixed
- **Files**: `CxbxPixelShaderTemplate.hlsl`, `XbPixelShader.cpp:255-256`
- **Description**: Both modes marked `LOG_TEST_CASE`, never verified against hardware. DOT_ZW also has a potential division-by-zero (float == 0 comparison unreliable).
- **Impact**: Titles using dot-product texture addressing

### 26. HILO Dot Mapping Questionable
- [ ] Fixed
- **Files**: `CxbxPixelShaderTemplate.hlsl:245`
- **Description**: Comment: "TODO : Verify whether this works at all!"
- **Impact**: Titles using HILO texture format with dot product mapping

### 27. Fog Start/End Negative Range Hack
- [x] Fixed
- **Files**: `RenderStates.cpp:270`
- **Description**: Contains "HACK" comment — negative fog start/end values are forced positive, which produces wrong fog density for games that legitimately use negative Z fog ranges.
- **Impact**: Wrong fog density in camera-relative fog setups

### 28. PS Constant Register Precision (8-bit vs float32)
- [ ] Fixed
- **Files**: `XbPixelShaderCompiler.cpp`
- **Description**: NV2A pixel shader constant registers are 8-bit per channel. The emulator uses float32 uniforms. Games relying on 8-bit quantization behavior will see smooth transitions instead of discrete steps.
- **Impact**: Subtle precision differences in combiner constants

### 29. ~~V0/V1 Range Not Converted (NV2A -1..1 vs D3D9 0..1)~~ (N/A)
- [x] Fixed
- **Files**: `XbPixelShaderCompiler.cpp:80-94`
- **Description**: NV2A vertex color registers v0/v1 have range -1..1 in register combiners, but D3D9 clamps to 0..1. No range conversion applied.
- **Resolution**: N/A — NV2A vertex colors are clamped to [0,1], not [-1,1]. The VS already applies `saturate(oD0)` / `saturate(oD1)` and the COLOR semantic further clamps during rasterization. No range conversion needed.
- **Impact**: None

### 30. Screen-Space Transform Precision with Large W
- [ ] Fixed
- **Files**: `CxbxVertexShaderTemplate.hlsl:253-268`
- **Description**: Screen-space vertex transformation may lose precision when W is very large, as the reciprocal `1/W` computation differs from NV2A fixed-function hardware.
- **Impact**: Depth precision artifacts on distant geometry using pre-transformed vertices

### 31. ~~oT0-oT3 Texture Coordinates Not Clamped~~ (N/A)
- [x] Fixed
- **Files**: `CxbxVertexShaderTemplate.hlsl:287-290`
- **Description**: NV2A may clamp texture coordinate outputs; the HLSL vertex shader does not.
- **Resolution**: N/A — NV2A does NOT clamp texture coordinate VS outputs. Clamping/wrapping happens at texture sampler stage via address modes. Not clamping is correct behavior.
- **Impact**: None

---

## Minor (Cosmetic / Rare)

### 32. Edge Antialiasing
- [ ] Fixed
- **Files**: `Backend_D3D11.cpp:382`
- **Description**: `X_D3DRS_EDGEANTIALIAS` → `AntialiasedLineEnable`. NV2A edge AA is polygon-level smoothing, not just lines.
- **Impact**: Polygon edge AA not accurately reproduced

### 33. Point Sprite Implementation
- [ ] Fixed
- **Files**: `TextureStates.cpp:167`
- **Description**: Point sprites require stage 3→stage 0 texture override and shader-side expansion. D3D11's native geometry shader point sprite expansion is not used.
- **Impact**: Particle systems using point sprites may render incorrectly

### 34. Render Target View Cache Miss
- [x] Fixed
- **Files**: `Backend_D3D11.cpp:597`
- **Description**: `CxbxSetRenderTarget()` creates a new RTV every call without format validation or caching. If format doesn't match the texture, D3D11 fails silently.
- **Impact**: Silent rendering failure; uses stale render target

### 35. ~~LSB Mux Correctness~~ (Verified)
- [x] Fixed
- **Files**: `CxbxPixelShaderTemplate.hlsl:141`
- **Description**: "TODO : Verify correctness" comment on LSB-based mux selection in register combiners.
- **Resolution**: Math is correct: `(r0.a * 255) % 2 >= 1` extracts the LSB of an 8-bit value. May have float precision issues near integer boundaries but zero known test-cases use LSB mode. TODO comment cleaned up.
- **Impact**: Theoretical float precision edge case only

### 36. ~~Eye Register Mapping~~ (N/A)
- [x] Fixed
- **Files**: `CxbxPixelShaderTemplate.hlsl:401`
- **Description**: "TODO : Map iT[1/2/3] through PS_INPUTTEXTURE_[]?" — eye vector register may not be correctly sourced.
- **Resolution**: N/A — NV2A `texm3x3vspec` instruction hardcodes the eye vector from the q components of texture coordinates 1, 2, 3. It does NOT route through PS_INPUTTEXTURE mapping. Direct indexing is correct. TODO comment cleaned up.
- **Impact**: None

### 37. Project2D/Project3D Perspective Divide
- [ ] Fixed
- **Files**: `CxbxPixelShaderTemplate.hlsl:409`
- **Description**: Comment asks "are x/w and y/w implicit?" — unclear if perspective division matches NV2A.
- **Impact**: Projected texture coordinates may be wrong

### 38. Resource Cache Eviction Policy
- [ ] Fixed
- **Files**: `HostResource.cpp`
- **Description**: "TODO : Implement a better cache eviction algorithm" — simple FIFO instead of LRU causes unnecessary texture re-uploads.
- **Impact**: Performance only; texture thrashing causes frame drops, not visual errors

### 39. ~~Lazy Input Layout Creation~~ (N/A)
- [x] Fixed
- **Files**: `XbVertexShader.cpp:729-778`
- **Description**: Input layout created lazily on first use. If multiple threads access the creation path, a race condition could create duplicate layouts or use a partially initialized one.
- **Resolution**: N/A — `CxbxUpdateHostVertexDeclaration()` is called from the draw path which runs on a single emulation/rendering thread. Xbox D3D is single-threaded. No race condition possible.
- **Impact**: None
