// ******************************************************************
// * PerfTrace.h - Lightweight per-frame performance tracing
// *
// * Usage:
// *   - Wrap a scope with  PERF_SCOPE(PERF_CAT_xxx)  — RAII, zero cost when off.
// *   - At frame end call  PerfTrace_OnSwap()  from EMUPATCH(D3DDevice_Swap).
// *   - Output goes to  <ShaderCacheDir>/../perf_trace.log
// *   - Toggle at runtime: global bool g_PerfTraceEnabled (default: false)
// *     or pass command-line flag  --perf-trace  (wired up in Emu.cpp).
// *
// * The log prints one line per category every PERF_REPORT_INTERVAL_S seconds:
// *   [frame N | Δt ms]  CAT_NAME  calls=N  total=X.XXms  avg=X.XXms  max=X.XXms
// ******************************************************************
#pragma once
#ifndef PERFTRACE_H
#define PERFTRACE_H

#include <windows.h>
#include <cstdio>
#include <cstring>
#include <atomic>

// ── tunables ──────────────────────────────────────────────────────────────────
// Seconds between log lines per category (reduce for more granular data)
#ifndef PERF_REPORT_INTERVAL_S
#define PERF_REPORT_INTERVAL_S 5.0
#endif
// ─────────────────────────────────────────────────────────────────────────────

// Category IDs — add new ones here and mirror in PerfTrace.cpp's g_catNames[]
enum PerfCat : int {
    PERF_CAT_SWAP            = 0,  // Full D3DDevice_Swap incl. StretchRect + Present
    PERF_CAT_UPDATE_NATIVE   = 1,  // CxbxUpdateNativeD3DResources (per draw call)
    PERF_CAT_UPDATE_TEXTURES = 2,  // CxbxUpdateHostTextures
    PERF_CAT_UPDATE_VS       = 3,  // CxbxUpdateHostVertexShader + constants
    PERF_CAT_UPDATE_PS       = 4,  // DxbxUpdateActivePixelShader
    PERF_CAT_DRAW            = 5,  // Host DrawIndexedPrimitive / DrawPrimitive
    PERF_CAT_CREATE_RESOURCE = 6,  // CreateHostResource (texture/surface upload)
    PERF_CAT_VTX_CONVERT     = 7,  // Vertex buffer conversion (VertexBufferConverter.Apply)
    PERF_CAT_SHADER_COMPILE  = 8,  // EmuCompileShader (sync miss only)
    PERF_CAT_LOCK_UNLOCK     = 9,  // Lock/Unlock surface or texture
    PERF_CAT_PRESENT         = 10, // Just the IDirect3DDevice9::Present call
    PERF_CAT_BLIT            = 11, // StretchRect blit (Xbox BB -> Host BB)
    PERF_CAT_RENDER_STATES   = 12, // XboxRenderStates.Apply()
    PERF_CAT_TEX_STATES      = 13, // XboxTextureStates.Apply()
    PERF_CAT_VTX_DECL        = 14, // CxbxUpdateHostVertexDeclaration + constants + viewport
    PERF_CAT_VS_DECL         = 15, // CxbxUpdateHostVertexDeclaration only
    PERF_CAT_VS_CONST        = 16, // CxbxUpdateHostVertexShaderConstants only
    PERF_CAT_VIEWPORT        = 17, // CxbxUpdateHostViewport only
    PERF_CAT_COUNT           = 18,
};

// ── internal state (defined in PerfTrace.h to keep it header-only) ───────────
extern bool g_PerfTraceEnabled;

// Per-frame vtx cache statistics (incremented by ConvertStream, reset each swap)
extern ULONG g_VtxCacheHits;
extern ULONG g_VtxCacheMisses;

namespace PerfTraceInternal {

struct CatStats {
    long long totalTicks;
    long long maxTicks;
    long long calls;
};

struct State {
    CatStats cats[PERF_CAT_COUNT];
    long long windowStartTick;
    long long swapStartTick;  // set in OnSwapBegin, read in OnSwapEnd
    long long freq;
    unsigned long long frameCount;
    unsigned long long windowFrameCount;
    FILE* logFile;
    bool initialized;
    // simple per-frame accumulators reset each Swap
    CatStats frame[PERF_CAT_COUNT];
};

extern State g_state;
extern const char* g_catNames[PERF_CAT_COUNT];

void Init(const char* logPath);
void Report();          // called by PerfTrace_OnSwap every N seconds
void OnSwapBegin();     // resets per-frame accumulators, bumps frame counter
void OnSwapEnd();       // accumulates frame totals into window totals, maybe Report

} // namespace PerfTraceInternal

// ── RAII scope guard ──────────────────────────────────────────────────────────
struct PerfScopeGuard {
    LARGE_INTEGER start;
    PerfCat cat;

    inline PerfScopeGuard(PerfCat c) : cat(c) {
        if (g_PerfTraceEnabled)
            QueryPerformanceCounter(&start);
    }
    inline ~PerfScopeGuard() {
        if (!g_PerfTraceEnabled) return;
        LARGE_INTEGER end;
        QueryPerformanceCounter(&end);
        long long elapsed = end.QuadPart - start.QuadPart;

        auto& s = PerfTraceInternal::g_state;
        // per-frame
        s.frame[cat].totalTicks += elapsed;
        s.frame[cat].calls++;
        if (elapsed > s.frame[cat].maxTicks) s.frame[cat].maxTicks = elapsed;
        // window
        s.cats[cat].totalTicks += elapsed;
        s.cats[cat].calls++;
        if (elapsed > s.cats[cat].maxTicks) s.cats[cat].maxTicks = elapsed;
    }
};

// PERF_SCOPE(cat) — use at the top of a scope/function.
// Compiles to nothing when tracing is disabled (the inline guard checks the bool).
#define PERF_SCOPE(cat) PerfScopeGuard _perf_guard_##cat(cat)

// ── public API ────────────────────────────────────────────────────────────────

// Call once during emulator init (before any draw calls).
// logPath = full path for the output file.
inline void PerfTrace_Init(const char* logPath) {
    PerfTraceInternal::Init(logPath);
}

// Call at the START of EMUPATCH(D3DDevice_Swap) (before PERF_SCOPE(PERF_CAT_SWAP)).
inline void PerfTrace_OnSwapBegin() {
    if (g_PerfTraceEnabled) PerfTraceInternal::OnSwapBegin();
}

// Call at the END of EMUPATCH(D3DDevice_Swap).
inline void PerfTrace_OnSwapEnd() {
    if (g_PerfTraceEnabled) PerfTraceInternal::OnSwapEnd();
}

#endif // PERFTRACE_H
