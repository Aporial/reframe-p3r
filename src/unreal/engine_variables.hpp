#pragma once
#include <cstdint>

namespace reframe {
struct EngineVariables {
    int32_t* temporal_samples;
    float* texture_mip_offset;
    float* screen_percentage;
    int32_t* sync_interval;
    int32_t* vsync;
    float* max_fps;
};

EngineVariables BindEngineVariables();
bool InstallGameplayScreenPercentage();
bool SetGameplayScreenPercentage(float percentage);
float GameplayScreenPercentage();
uint32_t TownMapEpoch();
bool NotifyTownMapCorrected(uint32_t epoch);
}
