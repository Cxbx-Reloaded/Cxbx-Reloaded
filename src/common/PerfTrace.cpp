// ******************************************************************
// * PerfTrace.cpp - Implementation for lightweight perf tracing
// ******************************************************************
#include "PerfTrace.h"
#include <cstdarg>
#include <cstring>

bool g_PerfTraceEnabled = false;

ULONG g_VtxCacheHits = 0;
ULONG g_VtxCacheMisses = 0;

namespace PerfTraceInternal {

State g_state = {};

const char* g_catNames[PERF_CAT_COUNT] = {
    "Swap            ",
    "UpdateNative    ",
    "UpdateTextures  ",
    "UpdateVtxShader ",
    "UpdatePxlShader ",
    "Draw            ",
    "CreateResource  ",
    "VtxConvert      ",
    "ShaderCompile   ",
    "LockUnlock      ",
    "Present         ",
    "Blit            ",
    "RenderStates    ",
    "TextureStates   ",
    "VtxDecl+Const   ",
    "VS_Decl         ",
    "VS_Const        ",
    "Viewport        ",
    "GetBackbuffer   ",
    "FrameSleep      ",
    "EndScene        ",
    "EmuX86VEH       ",
    "KeWait          ",
    "KeDelay         ",
};

void RegisterXboxThread(HANDLE h)
{
    auto& s = g_state;
    // Reserve a slot atomically — thread-safe, up to PERF_MAX_XBOX_THREADS threads.
    LONG idx = InterlockedIncrement((LONG*)&s.xboxThreadCount) - 1;
    if (idx >= PERF_MAX_XBOX_THREADS) {
        // Undo overshoot so the count stays accurate.
        InterlockedDecrement((LONG*)&s.xboxThreadCount);
        return;
    }

    // Duplicate the handle so PerfTrace has its own reference independent of the
    // Xbox handle table lifecycle.  The dup is intentionally never closed (it
    // leaks at emulator exit, which is acceptable).
    HANDLE dup = INVALID_HANDLE_VALUE;
    DuplicateHandle(GetCurrentProcess(), h, GetCurrentProcess(), &dup,
                    THREAD_QUERY_INFORMATION, FALSE, 0);
    if (dup == INVALID_HANDLE_VALUE) {
        InterlockedDecrement((LONG*)&s.xboxThreadCount);
        return;
    }

    // Snapshot the initial cumulative TSC cycle count so the first frame's delta
    // starts from registration, not thread birth.
    ULONG64 cycles = 0;
    QueryThreadCycleTime(dup, &cycles);

    s.xboxThreads[idx]          = dup;
    s.xboxThreadCycleStart[idx] = cycles;
}

void Init(const char* logPath)
{
    auto& s = g_state;
    if (s.initialized) return;
    memset(&s, 0, sizeof(s));

    LARGE_INTEGER freq, now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&now);
    s.freq = freq.QuadPart;
    s.windowStartTick = now.QuadPart;
    s.swapStartTick = 0;

    s.logFile = fopen(logPath, "w");
    if (s.logFile) {
        fprintf(s.logFile,
            "# Cxbx-Reloaded PerfTrace — report every %.0f s\n"
            "# columns: frame | ftime(ms) | fps(instant) | fps(~smooth) | xcpu(ms,xN threads) | swap | blit | pres | end | sleep | gbuf | native(xN) | tex | ps | vs | draw | vtx(hits/misses) | rsrc(xN) | cmp | rs | ts | vd[dc co vp] | kewait kedelay | rkw(render-thread kewait) rkd(render-thread kedelay)\n"
            "#\n",
            (double)PERF_REPORT_INTERVAL_S);
        fflush(s.logFile);
        fprintf(stdout, "[PerfTrace] logging to %s\n", logPath);
    } else {
        fprintf(stderr, "[PerfTrace] ERROR: cannot open %s\n", logPath);
    }

    // Calibrate CPU frequency for TSC cycle -> ms conversion.
    // Measure how many TSC cycles elapse over a short QPC interval.
    {
        ULONG64 cyc0 = 0, cyc1 = 0;
        LARGE_INTEGER t0, t1;
        QueryThreadCycleTime(GetCurrentThread(), &cyc0);
        QueryPerformanceCounter(&t0);
        // Spin for ~5ms to get a stable sample without Sleep() (which yields the thread).
        long long spinTarget = t0.QuadPart + s.freq / 200; // 1/200 s = 5ms
        do { QueryPerformanceCounter(&t1); } while (t1.QuadPart < spinTarget);
        QueryThreadCycleTime(GetCurrentThread(), &cyc1);
        double elapsedSec = (double)(t1.QuadPart - t0.QuadPart) / (double)s.freq;
        s.cpuGhz = (double)(cyc1 - cyc0) / elapsedSec / 1e9;
        if (s.cpuGhz < 0.5 || s.cpuGhz > 10.0) s.cpuGhz = 3.0; // fallback guard
    }

    s.initialized = true;
}

void OnSwapBegin()
{
    auto& s = g_state;
    if (!s.logFile) return;

    // Record the render thread ID — the host thread that calls D3DDevice_Swap IS the render thread.
    s.renderThreadId = GetCurrentThreadId();

    // Capture current tick for frame-time measurement before any other work.
    LARGE_INTEGER nowLI;
    QueryPerformanceCounter(&nowLI);
    long long nowTick = nowLI.QuadPart;

    // Compute frame-to-frame time and FPS using the tick from the previous OnSwapBegin.
    double frameDeltaMs = 0.0;
    double instantFps = 0.0;
    if (s.lastSwapBeginTick != 0 && s.frameCount > 0) {
        frameDeltaMs = (double)(nowTick - s.lastSwapBeginTick) / (double)s.freq * 1000.0;
        if (frameDeltaMs > 0.0) {
            instantFps = 1000.0 / frameDeltaMs;
            s.smoothFps = (s.smoothFps == 0.0) ? instantFps : (0.9 * s.smoothFps + 0.1 * instantFps);
        }
    }
    s.lastSwapBeginTick = nowTick;

    // Sample Xbox thread CPU time using QueryThreadCycleTime (TSC-based, nanosecond resolution).
    // Unlike GetThreadTimes, this has no 15.625ms quantization — reads the hardware TSC directly.
    // Divide by cpuGhz (cycles/ns) to get milliseconds.
    double xcpuMs = 0.0;
    int nthr = s.xboxThreadCount; // read snapshot; newly-added threads missed this frame only
    for (int i = 0; i < nthr; i++) {
        ULONG64 cur = 0;
        if (QueryThreadCycleTime(s.xboxThreads[i], &cur)) {
            ULONG64 delta = cur - s.xboxThreadCycleStart[i];
            xcpuMs += (double)delta / (s.cpuGhz * 1e6); // cycles / (GHz * 1e6) = ms
            s.xboxThreadCycleStart[i] = cur;
        }
    }
    s.xcpuMs = xcpuMs;

    // Print the PREVIOUS frame's accumulated data before resetting.
    // draw/native/tex/etc all happen before D3DDevice_Swap is called,
    // so we must print first, then clear.
    if (s.frameCount > 0) {
        double swapMs  = (double)s.frame[PERF_CAT_SWAP].totalTicks            / (double)s.freq * 1000.0;
        double blitMs  = (double)s.frame[PERF_CAT_BLIT].totalTicks             / (double)s.freq * 1000.0;
        double presMs  = (double)s.frame[PERF_CAT_PRESENT].totalTicks          / (double)s.freq * 1000.0;
        double nativeMs= (double)s.frame[PERF_CAT_UPDATE_NATIVE].totalTicks   / (double)s.freq * 1000.0;
        double texMs   = (double)s.frame[PERF_CAT_UPDATE_TEXTURES].totalTicks / (double)s.freq * 1000.0;
        double psMs    = (double)s.frame[PERF_CAT_UPDATE_PS].totalTicks       / (double)s.freq * 1000.0;
        double vsMs    = (double)s.frame[PERF_CAT_UPDATE_VS].totalTicks       / (double)s.freq * 1000.0;
        double drawMs  = (double)s.frame[PERF_CAT_DRAW].totalTicks            / (double)s.freq * 1000.0;
        double vtxMs   = (double)s.frame[PERF_CAT_VTX_CONVERT].totalTicks    / (double)s.freq * 1000.0;
        double rsrcMs  = (double)s.frame[PERF_CAT_CREATE_RESOURCE].totalTicks / (double)s.freq * 1000.0;
        double cmpMs   = (double)s.frame[PERF_CAT_SHADER_COMPILE].totalTicks  / (double)s.freq * 1000.0;
        double gbMs    = (double)s.frame[PERF_CAT_GET_BACKBUFFER].totalTicks  / (double)s.freq * 1000.0;
        double sleepMs = (double)s.frame[PERF_CAT_FRAME_SLEEP].totalTicks     / (double)s.freq * 1000.0;
        double endMs   = (double)s.frame[PERF_CAT_ENDSCENE].totalTicks        / (double)s.freq * 1000.0;
        long long nativeCalls = s.frame[PERF_CAT_UPDATE_NATIVE].calls;
        long long rsrcCalls   = s.frame[PERF_CAT_CREATE_RESOURCE].calls;
        ULONG vtxHits = g_VtxCacheHits;
        ULONG vtxMisses = g_VtxCacheMisses;

        fprintf(s.logFile,
            "F%06llu  ftime=%7.3f  fps=%5.1f(~%5.1f)  xcpu=%7.3f(x%d)  swap=%6.2f  blit=%6.2f  pres=%6.2f  end=%6.2f  sleep=%6.2f  gbuf=%6.2f  native=%6.2f(x%lld)  tex=%6.2f  ps=%6.2f  vs=%6.2f  draw=%6.2f  vtx=%6.2f(%lu/%lu)  rsrc=%6.2f(x%lld)  cmp=%6.2f  rs=%6.2f  ts=%6.2f  vd=%6.2f[dc=%5.2f co=%5.2f vp=%5.2f]  kewait=%6.2f(x%lld)  kedelay=%6.2f(x%lld)  rkw=%6.2f(x%lld)  rkd=%6.2f(x%lld) ms\n",
            (unsigned long long)s.frameCount,
            frameDeltaMs, instantFps, s.smoothFps,
            s.xcpuMs, nthr,
            swapMs, blitMs, presMs, endMs, sleepMs, gbMs, nativeMs, nativeCalls,
            texMs, psMs, vsMs, drawMs, vtxMs, vtxHits, vtxMisses, rsrcMs, rsrcCalls, cmpMs,
            (double)s.frame[PERF_CAT_RENDER_STATES].totalTicks / (double)s.freq * 1000.0,
            (double)s.frame[PERF_CAT_TEX_STATES].totalTicks / (double)s.freq * 1000.0,
            (double)s.frame[PERF_CAT_VTX_DECL].totalTicks / (double)s.freq * 1000.0,
            (double)s.frame[PERF_CAT_VS_DECL].totalTicks / (double)s.freq * 1000.0,
            (double)s.frame[PERF_CAT_VS_CONST].totalTicks / (double)s.freq * 1000.0,
            (double)s.frame[PERF_CAT_VIEWPORT].totalTicks / (double)s.freq * 1000.0,
            (double)s.frame[PERF_CAT_KE_WAIT].totalTicks / (double)s.freq * 1000.0,
            s.frame[PERF_CAT_KE_WAIT].calls,
            (double)s.frame[PERF_CAT_KE_DELAY].totalTicks / (double)s.freq * 1000.0,
            s.frame[PERF_CAT_KE_DELAY].calls,
            (double)s.frameRt[0].totalTicks / (double)s.freq * 1000.0,
            s.frameRt[0].calls,
            (double)s.frameRt[1].totalTicks / (double)s.freq * 1000.0,
            s.frameRt[1].calls);
    }

    // Reset per-frame accumulators for the new frame.
    memset(s.frame, 0, sizeof(s.frame));
    memset(s.frameRt, 0, sizeof(s.frameRt));
    g_VtxCacheHits = 0;
    g_VtxCacheMisses = 0;

    // Record the swap start time so OnSwapEnd can measure total swap duration.
    // Reuse nowTick already captured above to avoid an extra QPC call.
    s.swapStartTick = nowTick;
}

void Report()
{
    auto& s = g_state;
    if (!s.logFile) return;

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    double windowSec = (double)(now.QuadPart - s.windowStartTick) / (double)s.freq;
    double framesInWindow = (double)s.windowFrameCount;

    fprintf(s.logFile, "# === frame %-6llu  window=%.3fs  (%.1f fps avg) ===\n",
        (unsigned long long)s.frameCount,
        windowSec,
        framesInWindow / windowSec);

    for (int i = 0; i < PERF_CAT_COUNT; i++) {
        auto& c = s.cats[i];
        if (c.calls == 0) continue;
        double totalMs  = (double)c.totalTicks / (double)s.freq * 1000.0;
        double avgUs    = (double)c.totalTicks / (double)c.calls / (double)s.freq * 1000000.0;
        double maxMs    = (double)c.maxTicks   / (double)s.freq * 1000.0;
        double callsPerFrame = (double)c.calls / framesInWindow;
        fprintf(s.logFile,
            "  %s  calls/frame=%6.1f  total=%8.2f ms  avg=%7.2f us  max=%7.3f ms\n",
            g_catNames[i], callsPerFrame, totalMs, avgUs, maxMs);
    }
    fprintf(s.logFile, "\n");
    fflush(s.logFile);

    // Reset window accumulators
    memset(s.cats, 0, sizeof(s.cats));
    s.windowStartTick = now.QuadPart;
    s.windowFrameCount = 0;
}

void OnSwapEnd()
{
    auto& s = g_state;
    if (!s.logFile) return;

    // Record total swap time (StretchRect + Present + all swap work).
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    if (s.swapStartTick != 0) {
        long long elapsed = now.QuadPart - s.swapStartTick;
        s.frame[PERF_CAT_SWAP].totalTicks += elapsed;
        s.frame[PERF_CAT_SWAP].calls++;
        if (elapsed > s.frame[PERF_CAT_SWAP].maxTicks) s.frame[PERF_CAT_SWAP].maxTicks = elapsed;
        s.cats[PERF_CAT_SWAP].totalTicks += elapsed;
        s.cats[PERF_CAT_SWAP].calls++;
        if (elapsed > s.cats[PERF_CAT_SWAP].maxTicks) s.cats[PERF_CAT_SWAP].maxTicks = elapsed;
    }

    s.frameCount++;
    s.windowFrameCount++;

    double elapsed_s = (double)(now.QuadPart - s.windowStartTick) / (double)s.freq;
    if (elapsed_s >= PERF_REPORT_INTERVAL_S) {
        Report();
    }
}

} // namespace PerfTraceInternal
