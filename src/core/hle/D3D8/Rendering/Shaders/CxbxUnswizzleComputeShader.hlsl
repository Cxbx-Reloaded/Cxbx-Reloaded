// CxbxUnswizzleComputeShader.hlsl
// Compute shader that performs Xbox texture unswizzling (Morton/Z-order decode)
// directly on the GPU, replacing the CPU-side EmuUnswizzleBox loop.
//
// The raw swizzled bytes are uploaded to a ByteAddressBuffer.
// The compute shader reads each pixel from the swizzled source using Morton
// indexing and writes it to the destination texture via a UAV.
//
// NOTE: This file is a reference copy. The actual shader is compiled from
// a string literal in CxbxD3D11InitBlit() in Backend_D3D11.cpp.

// Swizzled source data (raw Xbox texture bytes)
ByteAddressBuffer g_SrcBuffer : register(t0);

// Destination texture (linear layout)
RWTexture2D<uint> g_DstTexture : register(u0);

// Constants: maskX, maskY, width, bpp (bytes per pixel)
cbuffer UnswizzleConstants : register(b0)
{
    uint maskX;
    uint maskY;
    uint texWidth;
    uint bpp; // 1, 2, or 4
};

// Compute Morton/Z-order index from (x, y) given precomputed masks.
// This interleaves bits of x and y according to the mask patterns.
uint MortonIndex(uint x, uint y)
{
    uint mx = maskX;
    uint my = maskY;
    uint result = 0;
    uint xBit = 1, yBit = 1, outBit = 1;
    uint totalMask = mx | my;
    
    // Walk each bit of the combined mask
    [unroll(20)] // Max 20 bits covers textures up to 1024x1024
    for (uint i = 0; i < 20; i++)
    {
        if ((totalMask & outBit) == 0)
            break;
        if (mx & outBit)
        {
            if (x & xBit)
                result |= outBit;
            xBit <<= 1;
        }
        if (my & outBit)
        {
            if (y & yBit)
                result |= outBit;
            yBit <<= 1;
        }
        outBit <<= 1;
    }
    return result;
}

[numthreads(8, 8, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint x = dispatchThreadID.x;
    uint y = dispatchThreadID.y;
    
    if (x >= texWidth)
        return;
    // Height check is implicit — dispatch is sized to texture dimensions
    
    uint mortonIdx = MortonIndex(x, y);
    
    // Read pixel from swizzled source
    // ByteAddressBuffer.Load reads 4 bytes at a DWORD-aligned address
    uint srcByteOffset = mortonIdx * bpp;
    uint value;
    
    if (bpp == 4)
    {
        value = g_SrcBuffer.Load(srcByteOffset);
    }
    else if (bpp == 2)
    {
        // Load the containing DWORD, then extract the 16-bit value
        uint dwordAddr = srcByteOffset & ~3u;
        uint shift = (srcByteOffset & 2u) * 8u; // 0 or 16
        uint dword = g_SrcBuffer.Load(dwordAddr);
        uint halfword = (dword >> shift) & 0xFFFF;
        // Pack into the low 16 bits — RWTexture2D<uint> stores full 32 bits
        value = halfword;
    }
    else // bpp == 1
    {
        uint dwordAddr = srcByteOffset & ~3u;
        uint shift = (srcByteOffset & 3u) * 8u; // 0, 8, 16, or 24
        uint dword = g_SrcBuffer.Load(dwordAddr);
        uint byte_val = (dword >> shift) & 0xFF;
        value = byte_val;
    }
    
    g_DstTexture[uint2(x, y)] = value;
}
