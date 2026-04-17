// CxbxIndexConvertComputeShader.hlsl
// Reference HLSL for the GPU-accelerated index buffer topology conversion
// compute shader. Converts quad-list and triangle-fan indices to triangle-list.
// The actual shader is compiled from an inline string in Backend_D3D11.cpp.
//
// Input:  ByteAddressBuffer of 16-bit Xbox indices (or sequential when g_IsIndexed == 0)
// Output: RWBuffer<uint> of packed 16-bit triangle-list indices (R32_UINT, 2 indices per element)
//
// Conversion modes:
//   0 = Quad list (CW winding) -> Triangle list: ABCD -> ABD + BCD
//   1 = Quad list (CCW winding) -> Triangle list: ABCD -> ADB + BDC
//   2 = Triangle fan -> Triangle list: hub + fan vertices -> individual triangles

cbuffer IndexConvertCB : register(b0)
{
    uint g_VertexCount;     // Number of source vertices
    uint g_ConversionMode;  // 0 = quad CW, 1 = quad CCW, 2 = fan
    uint g_IsIndexed;       // 1 = read from InputIndices, 0 = sequential
    uint g_Pad;
};

ByteAddressBuffer InputIndices : register(t0);
RWBuffer<uint> OutputIndices : register(u0);

uint ReadIndex16(uint idx)
{
    if (g_IsIndexed == 0) return idx;
    uint byteOffset = idx * 2;
    uint dwordOffset = byteOffset & ~3u;
    uint rawValue = InputIndices.Load(dwordOffset);
    return (byteOffset & 2u) ? (rawValue >> 16) : (rawValue & 0xFFFF);
}

uint Pack16(uint lo, uint hi)
{
    return (lo & 0xFFFF) | (hi << 16);
}

[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint tid = DTid.x;

    if (g_ConversionMode <= 1)
    {
        // Quad to Triangle List
        // Each thread handles one quad (4 input vertices -> 6 output indices)
        uint numQuads = g_VertexCount / 4;
        if (tid >= numQuads) return;

        uint srcBase = tid * 4;
        uint A = ReadIndex16(srcBase + 0);
        uint B = ReadIndex16(srcBase + 1);
        uint C = ReadIndex16(srcBase + 2);
        uint D = ReadIndex16(srcBase + 3);

        // Output: 6 indices packed as 3 uint32 values (2 indices per uint)
        uint dstBase = tid * 3;

        if (g_ConversionMode == 0)
        {
            // CW winding: ABD + BCD (split along B-D diagonal, matching NV2A hardware)
            OutputIndices[dstBase + 0] = Pack16(A, B);
            OutputIndices[dstBase + 1] = Pack16(D, B);
            OutputIndices[dstBase + 2] = Pack16(C, D);
        }
        else
        {
            // CCW winding: ADB + BDC (split along B-D diagonal, matching NV2A hardware)
            OutputIndices[dstBase + 0] = Pack16(A, D);
            OutputIndices[dstBase + 1] = Pack16(B, B);
            OutputIndices[dstBase + 2] = Pack16(D, C);
        }
    }
    else
    {
        // Triangle Fan to Triangle List
        // Each thread handles a pair of triangles for 32-bit alignment:
        //   tri 2i:   (hub, i1, i2)
        //   tri 2i+1: (hub, i2, i3)
        // Packed as 3 uints: [hub|i1], [i2|hub], [i2|i3]
        uint numTris = (g_VertexCount >= 3) ? (g_VertexCount - 2) : 0;
        uint pairIdx = tid;
        uint triIdx = pairIdx * 2;
        if (triIdx >= numTris) return;

        uint hub = ReadIndex16(0);
        uint i1 = ReadIndex16(triIdx + 1);
        uint i2 = ReadIndex16(triIdx + 2);

        uint dstBase = pairIdx * 3;

        if (triIdx + 1 < numTris)
        {
            // Two complete triangles
            uint i3 = ReadIndex16(triIdx + 3);
            OutputIndices[dstBase + 0] = Pack16(hub, i1);
            OutputIndices[dstBase + 1] = Pack16(i2, hub);
            OutputIndices[dstBase + 2] = Pack16(i2, i3);
        }
        else
        {
            // Last triangle (odd count) - only 3 indices (6 bytes)
            // Upper 16 bits of the last uint are padding (not read by DrawIndexed)
            OutputIndices[dstBase + 0] = Pack16(hub, i1);
            OutputIndices[dstBase + 1] = Pack16(i2, 0);
        }
    }
}
