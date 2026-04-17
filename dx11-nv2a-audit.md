# D3D11 NV2A Translation Audit

Comprehensive audit of every D3D11 code path that could produce different visual results from the NV2A hardware. Conducted across all rendering pipeline stages: vertex shaders, pixel shaders, render state, output merger, texture/sampler, and geometry/draw.

---

## Critical (Game-Breaking on Affected Titles)

### 1. Alpha Test Not Implemented in D3D11 Shaders
- [ ] Fixed
- **Files**: `Backend_D3D11.cpp:475`, `FixedFunctionPixelShader.hlsl`, `CxbxPixelShaderTemplate.hlsl`
- **Description**: `X_D3DRS_ALPHATESTENABLE`, `X_D3DRS_ALPHAREF`, `X_D3DRS_ALPHAFUNC` are grouped under "states handled as shader constants" at Backend_D3D11.cpp:475, but neither pixel shader reads or applies them. D3D11 has no fixed-function alpha test. The existing `AlphaKill` and `ColorKey` operations are per-texture-stage mechanisms, not the global alpha test. Any game using `D3DRS_ALPHATESTENABLE` will render all fragments regardless of alpha comparison.
- **Impact**: 100% of titles using alpha test (vegetation, fences, HUD elements, etc.)

### 2. Multiple Render Targets — Only 1 Bound
- [ ] Fixed
- **Files**: `Backend_D3D11.cpp:560` (and all `OMSetRenderTargets` calls)
- **Description**: All `OMSetRenderTargets` calls pass `1` as the count. Xbox supports 0–4 simultaneous render targets. Games using MRT will only write to target 0.
- **Impact**: Deferred rendering, shadow maps, bloom passes on affected titles

### 3. Index Buffer Always 16-bit
- [ ] Fixed
- **Files**: `Backend_D3D11.cpp:630`, `HostDraw.cpp:403`, `EmuPatches_Draw.cpp:419,648`
- **Description**: Every `IASetIndexBuffer` call uses `DXGI_FORMAT_R16_UINT`. If any Xbox title uses 32-bit indices via `D3DFMT_INDEX32`, vertex indices > 65535 wrap and produce corrupted geometry.
- **Impact**: Large meshes with > 65K vertices (rare on Xbox but possible)

### 4. FrontFace Render State Not Handled
- [ ] Fixed
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

### 6. Vertex Format: NORMPACKED3 Broken
- [ ] Fixed
- **Files**: `XbVertexShaderDecoder.cpp:536`, `XbVertexBuffer.cpp`
- **Description**: NORMPACKED3 is a single packed 32-bit integer with 11/11/10-bit signed components. The code reads it as 3×FLOAT32, producing garbage normals.
- **Impact**: Incorrect lighting on any title using packed normals

### 7. Vertex Format: NORMSHORT3 Uninitialized W
- [ ] Fixed
- **Files**: `XbVertexShaderDecoder.cpp:545`, `XbVertexBuffer.cpp:598`
- **Description**: NORMSHORT3 reads 3 signed shorts but leaves the 4th component (W) uninitialized, resulting in a garbage W value.
- **Impact**: Incorrect vertex transforms if W is read by shader

### 8. Vertex Format: PBYTE3 Missing Alpha
- [ ] Fixed
- **Files**: `XbVertexShaderDecoder.cpp:628`, `XbVertexBuffer.cpp:620`
- **Description**: PBYTE3 does not explicitly set alpha to 1.0, leaving it undefined.
- **Impact**: Transparent geometry where opaque was intended

### 9. Vertex Format: PBYTE1/PBYTE2 Missing Expansion
- [ ] Fixed
- **Files**: `XbVertexBuffer.cpp:554`
- **Description**: No format expansion for signed byte formats PBYTE1 and PBYTE2.
- **Impact**: Vertex data precision loss for affected formats

### 10. Fog Coordinate Not Clamped in VS
- [ ] Fixed
- **Files**: `CxbxVertexShaderTemplate.hlsl:289`
- **Description**: Comment says "TODO *NEEDS TESTING*" for fog coordinate output. NV2A clamps fog factor to [0,1]; the shader may pass unclamped values to the pixel shader.
- **Impact**: Over-fogged or under-fogged scenes

### 11. Front/Back Face Detection: D3D11 bool vs D3D9 float
- [ ] Fixed
- **Files**: `CxbxPixelShaderTemplate.hlsl:487`
- **Description**: D3D11 `SV_IsFrontFace` is a `bool`, converted via `(xIn.iFF ? 1.0f : -1.0f) * FRONTFACE_FACTOR >= 0`. This works for normal cases but edge behavior around degenerate triangles may differ from D3D9's float VFACE semantic.
- **Impact**: Potential wrong front/back color on degenerate triangles

### 12. PS Register Combiner: Input Modifier Precision (8-bit vs float32)
- [ ] Fixed
- **Files**: `CxbxPixelShaderTemplate.hlsl:35-43`
- **Description**: NV2A register combiners operate at 8-bit color precision (per-channel clamp, quantized). The shader operates at float32. Operations like `s_bx2(x) = 2*max(0,x) - 1` and `s_bias(x) = max(0,x) - 0.5` produce slightly different results without 8-bit quantization between stages. Comment: "TODO : Should all these 'max(0, x)' actually be 'saturate(x)'?"
- **Impact**: Subtle color banding/precision differences in combiner-heavy shaders

### 13. PS: Complement Operator Post-Clamping Wrong
- [ ] Fixed
- **Files**: `CxbxPixelShaderTemplate.hlsl:37`
- **Description**: `s_comp(x) = 1 - max(0, x)` — NV2A does `1 - clamp(x, 0, 1)` (saturate). If x > 1, the shader produces a negative result; NV2A would produce 0.
- **Impact**: Incorrect color values when combiner inputs exceed 1.0

### 14. COLORWRITEENABLE Only for RT[0]
- [ ] Fixed
- **Files**: `Backend_D3D11.cpp:465`
- **Description**: Only `g_D3D11BlendDesc.RenderTarget[0].RenderTargetWriteMask` is set. RT[1-3] use default masks. Will matter when MRT is fixed.
- **Impact**: Blocked by issue #2; will need fixing alongside MRT support

### 15. Quad-to-Triangle Winding Order Hardcoded
- [ ] Fixed
- **Files**: `IndexBufferConvert.cpp:69`
- **Description**: `bUseClockWiseWindingOrder = true` with TODO comment. Should read `X_D3DRS_FRONTFACE` at conversion time. Games using CCW front face get incorrect quad splitting diagonal.
- **Impact**: Wrong triangle edges visible on quad geometry with CCW winding

### 16. Depth Bias Precision Loss
- [ ] Fixed
- **Files**: `Backend_D3D11.cpp:375`
- **Description**: `g_D3D11RasterizerDesc.DepthBias = (INT)Value` — Xbox ZBIAS is a float-encoded DWORD; casting to INT truncates the fractional component.
- **Impact**: Shadow acne or Z-fighting where depth bias should fix it

### 17. LINELOOP Closure Edge Missing
- [ ] Fixed
- **Files**: `XbConvert.cpp:366` (D3D topology mapping)
- **Description**: Xbox `X_D3DPT_LINELOOP` mapped to `D3D_PRIMITIVE_TOPOLOGY_LINESTRIP`. The closing edge (last vertex → first vertex) is never drawn.
- **Impact**: Open outlines instead of closed loops

### 18. DOT_PRODUCT3 Bias/Scale Timing
- [ ] Fixed
- **Files**: `FixedFunctionPixelShader.hlsl:189`
- **Description**: Bias/scale may be applied at the wrong point in the DOT_PRODUCT3 calculation relative to NV2A hardware order.
- **Impact**: Incorrect bump mapping / per-pixel lighting on FF pipeline titles

### 19. BUMPENVMAP Source Register Uncertain
- [ ] Fixed
- **Files**: `FixedFunctionPixelShader.hlsl:199-209`
- **Description**: Comment "TODO : Verify" on which register provides the bump environment map source.
- **Impact**: Incorrect environment mapping on titles using BUMPENVMAP texture op

---

## Moderate (Edge Cases, Specific Titles)

### 20. Volume Texture Support Incomplete
- [ ] Fixed
- **Files**: `HostResourceCreate.cpp:477`
- **Description**: Contains "TODO : Implement standalone volume creation" — 3D texture creation may fall back to 2D.
- **Impact**: 3D textures (fog volumes, 3D LUTs) non-functional

### 21. Fog Calculation: `if` Instead of `else if`
- [ ] Fixed
- **Files**: `FixedFunctionPixelShader.hlsl:329`, `CxbxPixelShaderTemplate.hlsl:449`
- **Description**: Both pixel shaders use chained `if` for fog mode instead of `else if`. Functionally correct since only one mode matches at a time, but `fogFactor` would be uninitialized if `FogTableMode` has an unexpected value. The FF PS also has a redundant `FogEnable == 0` check inside the else branch.
- **Impact**: Cosmetic code issue; potential uninitialized value on invalid fog mode

### 22. Texture Filter Fallbacks
- [ ] Fixed
- **Files**: `TextureStates.cpp:224-233`
- **Description**: `X_D3DTEXF_QUINCUNX` → `D3DTEXF_ANISOTROPIC`, `X_D3DTEXF_GAUSSIANCUBIC` → `D3DTEXF_GAUSSIANQUAD`. Both are approximations, not exact Xbox filtering behavior.
- **Impact**: Slightly different texture appearance; blur/sharpness mismatch

### 23. Border Color Not Propagated to D3D11 Sampler
- [ ] Fixed
- **Files**: `TextureStates.cpp:289`
- **Description**: `X_D3DTSS_BORDERCOLOR` marked "handled elsewhere" but no D3D11 sampler `BorderColor` assignment found. Textures with `CLAMP_TO_BORDER` address mode will use black (default) instead of the game-specified border color.
- **Impact**: Wrong edge color on textures using border clamp addressing

### 24. BRDF Texture Mode Incomplete
- [ ] Fixed
- **Files**: `CxbxPixelShaderTemplate.hlsl:398`
- **Description**: "TODO : Complete 16 bit phi/sigma retrieval" — BRDF texture mode not fully implemented.
- **Impact**: Titles using BRDF-based lighting via texture combiners

### 25. DOT_ST / DOT_ZW Texture Modes Untested
- [ ] Fixed
- **Files**: `CxbxPixelShaderTemplate.hlsl`, `XbPixelShader.cpp:255-256`
- **Description**: Both modes marked `LOG_TEST_CASE`, never verified against hardware. DOT_ZW also has a potential division-by-zero (float == 0 comparison unreliable).
- **Impact**: Titles using dot-product texture addressing

### 26. HILO Dot Mapping Questionable
- [ ] Fixed
- **Files**: `CxbxPixelShaderTemplate.hlsl:245`
- **Description**: Comment: "TODO : Verify whether this works at all!"
- **Impact**: Titles using HILO texture format with dot product mapping

### 27. Fog Start/End Negative Range Hack
- [ ] Fixed
- **Files**: `RenderStates.cpp:270`
- **Description**: Contains "HACK" comment — negative fog start/end values are forced positive, which produces wrong fog density for games that legitimately use negative Z fog ranges.
- **Impact**: Wrong fog density in camera-relative fog setups

### 28. PS Constant Register Precision (8-bit vs float32)
- [ ] Fixed
- **Files**: `XbPixelShaderCompiler.cpp`
- **Description**: NV2A pixel shader constant registers are 8-bit per channel. The emulator uses float32 uniforms. Games relying on 8-bit quantization behavior will see smooth transitions instead of discrete steps.
- **Impact**: Subtle precision differences in combiner constants

### 29. V0/V1 Range Not Converted (NV2A -1..1 vs D3D9 0..1)
- [ ] Fixed
- **Files**: `XbPixelShaderCompiler.cpp:80-94`
- **Description**: NV2A vertex color registers v0/v1 have range -1..1 in register combiners, but D3D9 clamps to 0..1. No range conversion applied.
- **Impact**: Negative vertex colors lost (rare, but some effects depend on it)

### 30. Screen-Space Transform Precision with Large W
- [ ] Fixed
- **Files**: `CxbxVertexShaderTemplate.hlsl:253-268`
- **Description**: Screen-space vertex transformation may lose precision when W is very large, as the reciprocal `1/W` computation differs from NV2A fixed-function hardware.
- **Impact**: Depth precision artifacts on distant geometry using pre-transformed vertices

### 31. oT0-oT3 Texture Coordinates Not Clamped
- [ ] Fixed
- **Files**: `CxbxVertexShaderTemplate.hlsl:287-290`
- **Description**: NV2A may clamp texture coordinate outputs; the HLSL vertex shader does not.
- **Impact**: Texture coordinates outside [0,1] may wrap differently than on hardware

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
- [ ] Fixed
- **Files**: `Backend_D3D11.cpp:597`
- **Description**: `CxbxSetRenderTarget()` creates a new RTV every call without format validation or caching. If format doesn't match the texture, D3D11 fails silently.
- **Impact**: Silent rendering failure; uses stale render target

### 35. LSB Mux Correctness
- [ ] Fixed
- **Files**: `CxbxPixelShaderTemplate.hlsl:141`
- **Description**: "TODO : Verify correctness" comment on LSB-based mux selection in register combiners.
- **Impact**: Potential wrong mux branch in final combiner

### 36. Eye Register Mapping
- [ ] Fixed
- **Files**: `CxbxPixelShaderTemplate.hlsl:401`
- **Description**: "TODO : Map iT[1/2/3] through PS_INPUTTEXTURE_[]?" — eye vector register may not be correctly sourced.
- **Impact**: Incorrect reflection/specular calculations in rare texture modes

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

### 39. Lazy Input Layout Creation
- [ ] Fixed
- **Files**: `XbVertexShader.cpp:729-778`
- **Description**: Input layout created lazily on first use. If multiple threads access the creation path, a race condition could create duplicate layouts or use a partially initialized one.
- **Impact**: Potential crash or wrong vertex layout on first frame of a new format
