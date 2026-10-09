#pragma once
#include "JvsIo.h"
#include <string>

namespace JvsInput {
void Init(const std::string& dataPath, const std::string& executable);
bool IsEnabled();
void Poll();
const jvs_input_states_t& GetState();
bool Test();
bool Service();
// Normalized destination rectangle, published by the rendering thread.
void SetRenderBounds(float left, float top, float right, float bottom);
}
