// CxbxTextureFormatDecode.hlsli
//
// Shader-based texture format decoding for Xbox (NV2A) formats without
// native DXGI equivalents.  Raw texture data is uploaded as integer-typed
// SRVs (R16_UINT for 16-bit formats, R32_UINT for 32-bit) and decoded
// in the pixel shader with bitwise operations.
//
// Also implements NV2A texture filtering modes that D3D11 does not
// natively support (quincunx, Gaussian cubic).
//
// Integration:
//   - The C++ backend binds raw integer SRVs at t4-t7 for stages that
//     need decode, alongside the normal float SRVs at t0-t3.
//   - TEXDECODEINFO[4] (register c44) tells the shader per-stage:
//     x = format type (0 = normal sampling, no decode)
//     y = texture width in texels
//     z = texture height in texels
//     w = NV2A filter mode
//   - When TEXDECODEINFO[ts].x != 0, the decode path is taken instead
//     of the normal tex2D() / tex3D() / texCUBE() path.

#ifdef CXBX_USE_D3D11

// ============================================================================
// Constants
// ============================================================================

// Per-stage decode info: float4(formatType, width, height, filterMode)
uniform const float4 TEXDECODEINFO[4] : register(c44);

// Format type constants (TEXDECODEINFO.x)
#define TEXDECODE_FMT_NONE      0   // Normal sampling via float SRV
#define TEXDECODE_FMT_L6V5U5    1   // 16-bit: L(6) V(5) U(5), V/U signed two's complement
#define TEXDECODE_FMT_YUY2      2   // 32-bit macro-pixel: Y0 U Y1 V (4:2:2 packed YUV)
#define TEXDECODE_FMT_UYVY      3   // 32-bit macro-pixel: U Y0 V Y1 (4:2:2 packed YUV)
#define TEXDECODE_FMT_V8U8      4   // 16-bit: V(8) U(8), both signed two's complement
#define TEXDECODE_FMT_X8L8V8U8  5   // 32-bit: X(8) L(8) V(8) U(8), V/U signed

// NV2A texture filter mode constants (TEXDECODEINFO.w)
// These match xbox::X_D3DTEXTUREFILTERTYPE values
#define TEXDECODE_FILTER_NONE            0
#define TEXDECODE_FILTER_POINT           1
#define TEXDECODE_FILTER_LINEAR          2
#define TEXDECODE_FILTER_ANISOTROPIC     3
#define TEXDECODE_FILTER_QUINCUNX        4   // NV2A "flat cubic"
#define TEXDECODE_FILTER_GAUSSIANCUBIC   5   // NV2A Gaussian bicubic

// ============================================================================
// Raw integer texture SRVs (one per texture stage)
// ============================================================================
// Separate declarations required; D3D11 HLSL cannot dynamically index
// texture objects.  The Sample2D_Decode macro resolves these at compile
// time via token-pasting.

Texture2D<uint> rawTex_0 : register(t4);
Texture2D<uint> rawTex_1 : register(t5);
Texture2D<uint> rawTex_2 : register(t6);
Texture2D<uint> rawTex_3 : register(t7);

// ============================================================================
// Format decode functions
// ============================================================================
// Each function takes a raw uint value and returns float4(R, G, B, A)
// matching the NV2A's channel mapping for the format.

float4 DecodeL6V5U5(uint raw)
{
	// Bit layout: [15:10]=L(6, unsigned), [9:5]=V(5, signed), [4:0]=U(5, signed)
	int u5 = int(raw & 0x1Fu);
	int v5 = int((raw >> 5u) & 0x1Fu);
	uint l6 = (raw >> 10u) & 0x3Fu;

	// Sign-extend 5-bit two's complement
	if (u5 >= 16) u5 -= 32;
	if (v5 >= 16) v5 -= 32;

	// Normalize: U,V → [-1,+1], L → [0,1]
	float U = float(u5) / 15.0;
	float V = float(v5) / 15.0;
	float L = float(l6) / 63.0;

	// NV2A L6V5U5 channel mapping: R=U, G=V, B=L, A=1
	return float4(U, V, L, 1.0);
}

float4 DecodeV8U8(uint raw)
{
	// Bit layout: [7:0]=U(8, signed), [15:8]=V(8, signed)
	int u8 = int(raw & 0xFFu);
	int v8 = int((raw >> 8u) & 0xFFu);

	// Sign-extend 8-bit two's complement
	if (u8 >= 128) u8 -= 256;
	if (v8 >= 128) v8 -= 256;

	float U = float(u8) / 127.0;
	float V = float(v8) / 127.0;

	// NV2A V8U8 (G8B8 alias) channel mapping: R=U, G=V, B=0, A=1
	return float4(U, V, 0.0, 1.0);
}

float4 DecodeX8L8V8U8(uint raw)
{
	// Bit layout: [7:0]=U(8, signed), [15:8]=V(8, signed), [23:16]=L(8, unsigned), [31:24]=X(8, ignored)
	int u8 = int(raw & 0xFFu);
	int v8 = int((raw >> 8u) & 0xFFu);
	uint l8 = (raw >> 16u) & 0xFFu;

	if (u8 >= 128) u8 -= 256;
	if (v8 >= 128) v8 -= 256;

	float U = float(u8) / 127.0;
	float V = float(v8) / 127.0;
	float L = float(l8) / 255.0;

	// NV2A X8L8V8U8 channel mapping: R=U, G=V, B=L, A=1
	return float4(U, V, L, 1.0);
}

float4 DecodeYUY2(uint raw, bool isEvenPixel)
{
	// YUY2 macro-pixel (32 bits encodes 2 horizontal pixels):
	// byte[0]=Y0, byte[1]=Cb(U), byte[2]=Y1, byte[3]=Cr(V)
	float Y = float(isEvenPixel ? (raw & 0xFFu) : ((raw >> 16u) & 0xFFu)) / 255.0;
	float Cb = float((raw >> 8u)  & 0xFFu) / 255.0 - 0.5;
	float Cr = float((raw >> 24u) & 0xFFu) / 255.0 - 0.5;

	// BT.601 YCbCr → RGB
	float R = Y + 1.402 * Cr;
	float G = Y - 0.344136 * Cb - 0.714136 * Cr;
	float B = Y + 1.772 * Cb;

	return float4(saturate(R), saturate(G), saturate(B), 1.0);
}

float4 DecodeUYVY(uint raw, bool isEvenPixel)
{
	// UYVY macro-pixel (32 bits encodes 2 horizontal pixels):
	// byte[0]=Cb(U), byte[1]=Y0, byte[2]=Cr(V), byte[3]=Y1
	float Y = float(isEvenPixel ? ((raw >> 8u) & 0xFFu) : ((raw >> 24u) & 0xFFu)) / 255.0;
	float Cb = float(raw & 0xFFu)          / 255.0 - 0.5;
	float Cr = float((raw >> 16u) & 0xFFu) / 255.0 - 0.5;

	float R = Y + 1.402 * Cr;
	float G = Y - 0.344136 * Cb - 0.714136 * Cr;
	float B = Y + 1.772 * Cb;

	return float4(saturate(R), saturate(G), saturate(B), 1.0);
}

// ============================================================================
// Texel loading helpers
// ============================================================================

// Decode a single texel at integer coordinates from a raw texture.
// Handles coordinate wrapping (repeat addressing mode).
float4 DecodeTexelAt(Texture2D<uint> rawTex, int2 coord, int2 texSize, float fmtType)
{
	// Wrap coordinates (repeat addressing)
	coord = ((coord % texSize) + texSize) % texSize;

	if (fmtType == TEXDECODE_FMT_L6V5U5) {
		uint raw = rawTex.Load(int3(coord, 0)) & 0xFFFFu;
		return DecodeL6V5U5(raw);
	}
	if (fmtType == TEXDECODE_FMT_V8U8) {
		uint raw = rawTex.Load(int3(coord, 0)) & 0xFFFFu;
		return DecodeV8U8(raw);
	}
	if (fmtType == TEXDECODE_FMT_X8L8V8U8) {
		uint raw = rawTex.Load(int3(coord, 0));
		return DecodeX8L8V8U8(raw);
	}
	if (fmtType == TEXDECODE_FMT_YUY2) {
		// YUY2: 2 pixels per 32-bit macro-pixel.  Raw SRV width = texWidth/2.
		int2 macroCoord = int2(coord.x >> 1, coord.y);
		macroCoord = ((macroCoord % int2(texSize.x >> 1, texSize.y)) + int2(texSize.x >> 1, texSize.y)) % int2(texSize.x >> 1, texSize.y);
		uint raw = rawTex.Load(int3(macroCoord, 0));
		return DecodeYUY2(raw, (coord.x & 1) == 0);
	}
	if (fmtType == TEXDECODE_FMT_UYVY) {
		int2 macroCoord = int2(coord.x >> 1, coord.y);
		macroCoord = ((macroCoord % int2(texSize.x >> 1, texSize.y)) + int2(texSize.x >> 1, texSize.y)) % int2(texSize.x >> 1, texSize.y);
		uint raw = rawTex.Load(int3(macroCoord, 0));
		return DecodeUYVY(raw, (coord.x & 1) == 0);
	}

	return float4(1.0, 0.0, 1.0, 1.0); // Magenta = unsupported format
}

// ============================================================================
// Texture filtering implementations
// ============================================================================

// --- Point (nearest neighbor) ---
float4 FilterPoint(Texture2D<uint> rawTex, float2 uv, float2 texSize, float fmtType)
{
	int2 coord = int2(floor(uv * texSize));
	return DecodeTexelAt(rawTex, coord, int2(texSize), fmtType);
}

// --- Bilinear (2×2 weighted average) ---
float4 FilterBilinear(Texture2D<uint> rawTex, float2 uv, float2 texSize, float fmtType)
{
	// Align to texel centers (D3D convention: texel center at +0.5)
	float2 tc = uv * texSize - 0.5;
	int2 base = int2(floor(tc));
	float2 f = tc - float2(base);

	float4 tl = DecodeTexelAt(rawTex, base + int2(0, 0), int2(texSize), fmtType);
	float4 tr = DecodeTexelAt(rawTex, base + int2(1, 0), int2(texSize), fmtType);
	float4 bl = DecodeTexelAt(rawTex, base + int2(0, 1), int2(texSize), fmtType);
	float4 br = DecodeTexelAt(rawTex, base + int2(1, 1), int2(texSize), fmtType);

	return lerp(lerp(tl, tr, f.x), lerp(bl, br, f.x), f.y);
}

// --- Quincunx (NV2A "flat cubic") ---
// 5-tap pattern: center (weight 1/2) + 4 diagonal neighbors (weight 1/8 each).
// The NV2A quincunx pattern uses half-texel offsets in a diamond/cross arrangement.
// This provides slightly smoother results than bilinear with minimal extra cost.
float4 FilterQuincunx(Texture2D<uint> rawTex, float2 uv, float2 texSize, float fmtType)
{
	float2 tc = uv * texSize - 0.5;
	int2 center = int2(floor(tc + 0.5));
	int2 ts = int2(texSize);

	float4 c  = DecodeTexelAt(rawTex, center,               ts, fmtType);
	float4 tl = DecodeTexelAt(rawTex, center + int2(-1, -1), ts, fmtType);
	float4 tr = DecodeTexelAt(rawTex, center + int2( 1, -1), ts, fmtType);
	float4 bl = DecodeTexelAt(rawTex, center + int2(-1,  1), ts, fmtType);
	float4 br = DecodeTexelAt(rawTex, center + int2( 1,  1), ts, fmtType);

	// NV2A quincunx weights: center=1/2, corners=1/8
	return c * 0.5 + (tl + tr + bl + br) * 0.125;
}

// --- Gaussian cubic weight function ---
// Catmull-Rom / Mitchell-Netravali with B=0, C=0.5 (common approximation
// of the Gaussian cubic kernel used by the NV2A GAUSSIANCUBIC filter).
float CubicWeight(float x)
{
	float ax = abs(x);
	if (ax < 1.0)
		return (1.5 * ax - 2.5) * ax * ax + 1.0;
	if (ax < 2.0)
		return ((-0.5 * ax + 2.5) * ax - 4.0) * ax + 2.0;
	return 0.0;
}

// --- Gaussian cubic (4×4 bicubic, NV2A-specific) ---
// 16-tap bicubic with Catmull-Rom weights, matching the NV2A's
// D3DTEXF_GAUSSIANCUBIC quality level.
float4 FilterGaussianCubic(Texture2D<uint> rawTex, float2 uv, float2 texSize, float fmtType)
{
	float2 tc = uv * texSize - 0.5;
	int2 base = int2(floor(tc));
	float2 f = tc - float2(base);
	int2 ts = int2(texSize);

	float4 result = 0;
	float totalWeight = 0;

	[unroll] for (int dy = -1; dy <= 2; dy++) {
		float wy = CubicWeight(f.y - float(dy));
		[unroll] for (int dx = -1; dx <= 2; dx++) {
			float wx = CubicWeight(f.x - float(dx));
			float w = wx * wy;
			result += DecodeTexelAt(rawTex, base + int2(dx, dy), ts, fmtType) * w;
			totalWeight += w;
		}
	}

	return result / totalWeight;
}

// ============================================================================
// Main decode + filter dispatch
// ============================================================================
// Called from the modified Sample2D path when TEXDECODEINFO[ts].x != 0.

float4 SampleWithDecode(Texture2D<uint> rawTex, float2 uv, float4 decodeInfo)
{
	float fmtType    = decodeInfo.x;
	float2 texSize   = decodeInfo.yz;
	float filterMode = decodeInfo.w;

	// Dispatch based on NV2A filter mode
	if (filterMode <= TEXDECODE_FILTER_POINT)
		return FilterPoint(rawTex, uv, texSize, fmtType);

	if (filterMode <= TEXDECODE_FILTER_ANISOTROPIC) // LINEAR and ANISOTROPIC both use bilinear
		return FilterBilinear(rawTex, uv, texSize, fmtType);

	if (filterMode <= TEXDECODE_FILTER_QUINCUNX)
		return FilterQuincunx(rawTex, uv, texSize, fmtType);

	// GAUSSIANCUBIC (and any unknown mode)
	return FilterGaussianCubic(rawTex, uv, texSize, fmtType);
}

#endif // CXBX_USE_D3D11
