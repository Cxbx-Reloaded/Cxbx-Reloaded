#ifndef FIXEDFUNCTIONSTATE_H
#define FIXEDFUNCTIONSTATE_H

#include "XbD3D8Types.h"
#include <array>

class D3D8LightState {
public:
    std::array<xbox::X_D3DLIGHT8, 4096> Lights;

    // The indices of last 8 enabled lights
    // From least recently to most recently enabled
    // -1 represents empty light slots
    // which always appear after enabled lights
    std::array<int, 8> EnabledLights;

    // The number of enabled lights
    uint32_t EnabledLightCount = 0;

    D3D8LightState();

    // Enable a light
    void EnableLight(uint32_t index, bool enable);
};

class D3D8TransformState {
public:
	D3D8TransformState();
	void SetTransform(xbox::X_D3DTRANSFORMSTATETYPE state, const D3DMATRIX* pMatrix);
	D3DMATRIX* GetWorldView(unsigned i);
	D3DMATRIX* GetWorldViewInverseTranspose(unsigned i);

	// The transforms set by the Xbox title
	std::array<D3DMATRIX, xbox::X_D3DTS_MAX> Transforms;

private:
	void RecalculateDependentMatrices();

	bool bWorldViewDirty;
	// Combines world/view matrices
	std::array<D3DMATRIX, 4> WorldView;
	// World/view inverse transpose for lighting calculations
	std::array<D3DMATRIX, 4> WorldViewInverseTranspose;
};

#endif
