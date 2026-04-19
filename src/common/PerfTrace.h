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
// Maximum number of Xbox threads to track for CPU time accounting
#define PERF_MAX_XBOX_THREADS 32
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
    PERF_CAT_GET_BACKBUFFER  = 18, // GetHostSurface for backbuffer in swap path
    PERF_CAT_FRAME_SLEEP     = 19, // SleepPrecise frame limiter inside Swap
    PERF_CAT_ENDSCENE        = 20, // EndScene + GPU flush before Present
    PERF_CAT_EMU_X86         = 21, // EmuX86_DecodeException VEH handler (privileged instrs)
    PERF_CAT_KE_WAIT         = 22, // KeWaitForSingleObject / KeWaitForMultipleObjects
    PERF_CAT_KE_DELAY        = 23, // KeDelayExecutionThread
    PERF_CAT_COUNT           = 24,
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
    long long lastSwapBeginTick; // for computing frame-to-frame time
    long long freq;
    unsigned long long frameCount;
    unsigned long long windowFrameCount;
    double smoothFps;  // EMA-smoothed FPS (alpha=0.1)
    FILE* logFile;
    bool initialized;
    // simple per-frame accumulators reset each Swap
    CatStats frame[PERF_CAT_COUNT];
    // Xbox thread CPU time tracking (QueryThreadCycleTime, TSC-based)
    HANDLE xboxThreads[PERF_MAX_XBOX_THREADS];        // duplicated native Win32 handles
    ULONGLONG xboxThreadCycleStart[PERF_MAX_XBOX_THREADS]; // cumulative TSC cycles at last sample
    ULONGLONG xcpuCyclesPerFrame;  // last frame: total Xbox-thread TSC cycles (converted to ms using cpuGhz)
    double cpuGhz;        // measured CPU frequency (GHz) used to convert TSC cycles -> ms
    int xboxThreadCount;  // number of registered Xbox threads (written atomically)
    double xcpuMs;        // last frame: total Xbox-thread CPU time in ms
    // Render thread tracking: host thread ID of the thread that calls D3DDevice_Swap
    DWORD renderThreadId; // set each frame in OnSwapBegin from GetCurrentThreadId()
    // Per-frame render-thread exclusive stats for KE_WAIT and KE_DELAY
    CatStats frameRt[2]; // [0]=KE_WAIT, [1]=KE_DELAY, render thread only
};

extern State g_state;
extern const char* g_catNames[PERF_CAT_COUNT];

void Init(const char* logPath);
void RegisterXboxThread(HANDLE h); // duplicate h and track its CPU time per-frame
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
        // render-thread exclusive tracking for kewait/kedelay
        if ((cat == PERF_CAT_KE_WAIT || cat == PERF_CAT_KE_DELAY) && s.renderThreadId != 0) {
            if (GetCurrentThreadId() == s.renderThreadId) {
                int rtIdx = (cat == PERF_CAT_KE_WAIT) ? 0 : 1;
                s.frameRt[rtIdx].totalTicks += elapsed;
                s.frameRt[rtIdx].calls++;
            }
        }
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

// Register a native Win32 thread handle to have its CPU time tracked per-frame.
// Safe to call from any thread; a duplicate of h is stored internally.
// Typically called once per Xbox thread from PsCreateSystemThreadEx.
inline void PerfTrace_RegisterXboxThread(HANDLE h) {
    if (g_PerfTraceEnabled) PerfTraceInternal::RegisterXboxThread(h);
}

#endif // PERFTRACE_H
