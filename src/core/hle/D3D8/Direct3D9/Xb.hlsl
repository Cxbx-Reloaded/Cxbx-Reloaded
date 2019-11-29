struct VS_INPUT
{
	float4 v[16] : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 oPos : POSITION;
	float4 oD0  : COLOR0; // Colour 0
	float4 oD1  : COLOR1; // Colour 1
	float  oFog : FOG;
	float  oPts : PSIZE;
	float4 oB0  : TEXCOORD4; // Backface Colour 0
	float4 oB1  : TEXCOORD5; // Backface Colour 1
	float4 oT0  : TEXCOORD0; // Texture Coord 0
	float4 oT1  : TEXCOORD1; // Texture Coord 0
	float4 oT2  : TEXCOORD2; // Texture Coord 0
	float4 oT3  : TEXCOORD3; // Texture Coord 0
};

extern float4 c[192]; // Constant registers
static float4 r[16]; // Temporary registers
static int a; // address register

VS_OUTPUT defaultOut() {
	VS_OUTPUT o;

	// Initialize default values
	o.oPos = float4(0, 0, 0, 1);
	o.oD0  = float4(0, 0, 0, 1);
	o.oD1  = float4(0, 0, 0, 1);
	o.oFog = 0;
	o.oPts = 0;
	o.oB0 = float4(0, 0, 0, 1);
	o.oB1 = float4(0, 0, 0, 1);
	o.oT0 = float4(0, 0, 0, 1);
	o.oT1 = float4(0, 0, 0, 1);
	o.oT2 = float4(0, 0, 0, 1);
	o.oT3 = float4(0, 0, 0, 1);

	return o;
}

VS_OUTPUT main(const VS_INPUT xIn)
{
	VS_OUTPUT xOut = defaultOut();

	// Insert Xbox shader here

	<Xbox Shader>

	return xOut;
}
