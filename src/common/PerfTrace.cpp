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
};

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
            "# columns: frame | swap | blit | pres | native(xN) | tex | ps | vs | draw | vtx(hits/misses) | rsrc(xN) | cmp | rs | ts | vd[dc co vp]\n"
            "#\n",
            (double)PERF_REPORT_INTERVAL_S);
        fflush(s.logFile);
        fprintf(stdout, "[PerfTrace] logging to %s\n", logPath);
    } else {
        fprintf(stderr, "[PerfTrace] ERROR: cannot open %s\n", logPath);
    }
    s.initialized = true;
}

void OnSwapBegin()
{
    auto& s = g_state;
    if (!s.logFile) return;

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
        long long nativeCalls = s.frame[PERF_CAT_UPDATE_NATIVE].calls;
        long long rsrcCalls   = s.frame[PERF_CAT_CREATE_RESOURCE].calls;
        ULONG vtxHits = g_VtxCacheHits;
        ULONG vtxMisses = g_VtxCacheMisses;

        fprintf(s.logFile,
            "F%06llu  swap=%6.2f  blit=%6.2f  pres=%6.2f  native=%6.2f(x%lld)  tex=%6.2f  ps=%6.2f  vs=%6.2f  draw=%6.2f  vtx=%6.2f(%lu/%lu)  rsrc=%6.2f(x%lld)  cmp=%6.2f  rs=%6.2f  ts=%6.2f  vd=%6.2f[dc=%5.2f co=%5.2f vp=%5.2f] ms\n",
            (unsigned long long)s.frameCount,
            swapMs, blitMs, presMs, nativeMs, nativeCalls,
            texMs, psMs, vsMs, drawMs, vtxMs, vtxHits, vtxMisses, rsrcMs, rsrcCalls, cmpMs,
            (double)s.frame[PERF_CAT_RENDER_STATES].totalTicks / (double)s.freq * 1000.0,
            (double)s.frame[PERF_CAT_TEX_STATES].totalTicks / (double)s.freq * 1000.0,
            (double)s.frame[PERF_CAT_VTX_DECL].totalTicks / (double)s.freq * 1000.0,
            (double)s.frame[PERF_CAT_VS_DECL].totalTicks / (double)s.freq * 1000.0,
            (double)s.frame[PERF_CAT_VS_CONST].totalTicks / (double)s.freq * 1000.0,
            (double)s.frame[PERF_CAT_VIEWPORT].totalTicks / (double)s.freq * 1000.0);
    }

    // Reset per-frame accumulators for the new frame.
    memset(s.frame, 0, sizeof(s.frame));
    g_VtxCacheHits = 0;
    g_VtxCacheMisses = 0;

    // Record the swap start time so OnSwapEnd can measure total swap duration.
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    s.swapStartTick = now.QuadPart;
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
