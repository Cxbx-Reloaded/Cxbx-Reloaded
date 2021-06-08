#define LOG_PREFIX CXBXR_MODULE::D3D8

#include "FixedFunctionState.h"
#include "Logging.h"
#include "core/kernel/init/CxbxKrnl.h"

xbox::X_D3DCOLORVALUE colorValue(float r, float g, float b, float a) {
    auto value = xbox::X_D3DCOLORVALUE();
    value.r = r;
    value.g = g;
    value.b = b;
    value.a = a;
    return value;
}

xbox::X_D3DVECTOR toVector(float x, float y, float z) {
    auto value = xbox::X_D3DVECTOR();
    value.x = x;
    value.y = y;
    value.z = z;
    return value;
}

D3D8LightState::D3D8LightState() {
    // Define the default light
    // When unset lights are enabled, they're set to the default light
    auto defaultLight = xbox::X_D3DLIGHT8();
    defaultLight.Type = xbox::X_D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL;
    defaultLight.Diffuse = colorValue(1, 1, 1, 0);
    defaultLight.Specular = colorValue(0, 0, 0, 0);
    defaultLight.Ambient = colorValue(0, 0, 0, 0);
    defaultLight.Position = toVector(0, 0, 0);
    defaultLight.Direction = toVector(0, 0, 1);
    defaultLight.Range = 0;
    defaultLight.Falloff = 0;
    defaultLight.Attenuation0 = 0;
    defaultLight.Attenuation1 = 0;
    defaultLight.Attenuation2 = 0;
    defaultLight.Theta = 0;
    defaultLight.Phi = 0;

    // We'll just preset every light to the default light
    Lights.fill(defaultLight);
    EnabledLights.fill(-1);
}

void D3D8LightState::EnableLight(uint32_t index, bool enable) {
    // Since Xbox only supports 8 lights, we keep track of the 8 most recently enabled lights
    // Lights are ordered oldest to newest, with disabled lights at the end

    // Check to see if the light is already enabled
    for (size_t i = 0; i < EnabledLightCount; i++) {

        // If the light is already in the enabled lights
        if (EnabledLights[i] == index) {
            // Either way we move this light to the end
            std::rotate(std::begin(EnabledLights) + i, std::begin(EnabledLights) + i + 1, std::begin(EnabledLights) + EnabledLightCount);

            if (enable) {
                // Don't need to do anything
                EmuLog(LOG_LEVEL::INFO, "Enabled light %d but it was already enabled", index);
            }
            else {
                // Disable the light
                EnabledLights[EnabledLightCount - 1] = -1;
                EnabledLightCount--;
            }
            return;
        }
    }

    if (enable) {
        // The light was not in the enabled lights. Let's add it
        if (EnabledLightCount < EnabledLights.size()) {
            EnabledLights[EnabledLightCount] = index; // add it to the end
            EnabledLightCount++;
        }
        else {
            // Replace the oldest element and move to end
            EmuLog(LOG_LEVEL::INFO, "Can't enable any more lights. Replacing the oldest light %i", EnabledLights[0]);
            EnabledLights[0] = index;
            std::rotate(std::begin(EnabledLights), std::begin(EnabledLights) + 1, std::end(EnabledLights));
        }
    }
    else {
        EmuLog(LOG_LEVEL::INFO, "Could not disable light %d because it wasn't enabled", index);
    }
}

D3D8TransformState::D3D8TransformState() {
	D3DMATRIX identity;
	D3DXMatrixIdentity((D3DXMATRIX*)&identity);

	this->Transforms.fill(identity);
	this->WorldView.fill(identity);
	this->WorldViewInverseTranspose.fill(identity);
	bWorldViewDirty.fill(true);
}

void D3D8TransformState::SetTransform(xbox::X_D3DTRANSFORMSTATETYPE state, const D3DMATRIX* pMatrix)
{
	using namespace xbox;

	LOG_INIT

	if (state >= this->Transforms.size()) {
		LOG_TEST_CASE("Transform state was not in expected range");
		return;
	}

	// Update transform state
	this->Transforms[state] = *pMatrix;

	if (state == X_D3DTS_VIEW) {
		bWorldViewDirty.fill(true);
	}
	if ((X_D3DTS_WORLD <= state) && (state <= X_D3DTS_WORLD3)) {
		bWorldViewDirty[state - X_D3DTS_WORLD] = true;
	}
}

void D3D8TransformState::RecalculateDependentMatrices(unsigned i)
{
	auto worldState = xbox::X_D3DTS_WORLD + i;
	D3DXMATRIX worldView;
	D3DXMatrixMultiply(&worldView, (D3DXMATRIX*)&Transforms[worldState], (D3DXMATRIX*)&Transforms[xbox::X_D3DTS_VIEW]);
	this->WorldView[i] = worldView;

	D3DXMATRIX worldViewInverseTranspose;
	D3DXMatrixInverse(&worldViewInverseTranspose, nullptr, &worldView);
	D3DXMatrixTranspose(&worldViewInverseTranspose, &worldViewInverseTranspose);
	this->WorldViewInverseTranspose[i] = worldViewInverseTranspose;
}

D3DMATRIX* D3D8TransformState::GetWorldView(unsigned i)
{
	assert(i < 4);

	if (bWorldViewDirty[i]) {
		RecalculateDependentMatrices(i);
		bWorldViewDirty[i] = false;
	}

	return &WorldView[i];
}

void D3D8TransformState::SetWorldView(unsigned i, const D3DMATRIX* pMatrix)
{
    assert(i < 4);

    if (!pMatrix) {
        // null indicates the title is done with setting
        // the worldview matrix explicitly
        bWorldViewDirty[i] = true;
        return;
    }
    else {
        bWorldViewDirty[i] = false;
        WorldView[i] = *pMatrix;
    }
}

D3DMATRIX* D3D8TransformState::GetWorldViewInverseTranspose(unsigned i)
{
	assert(i < 4);

	if (bWorldViewDirty[i]) {
		RecalculateDependentMatrices(i);
		bWorldViewDirty[i] = false;
	}

	return &WorldViewInverseTranspose[i];
}
