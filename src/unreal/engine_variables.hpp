#pragma once
#include <cstdint>

namespace reframe {
struct EngineVariables {
    int32_t* temporal_upsampling;
    int32_t* temporal_samples;
    float* texture_mip_offset;
    float* screen_percentage;
    int32_t* sync_interval;
    int32_t* vsync;
    float* max_fps;
};

EngineVariables BindEngineVariables();
}
