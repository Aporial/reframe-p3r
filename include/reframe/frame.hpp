#pragma once
#include <d3d12.h>
#include <cstdint>

namespace reframe {
enum class Quality { Native, Quality, Balanced, Performance, UltraQuality, UltraQualityPlus, Count };
enum class Technology { Taa, Dlss, Fsr, Xess };
enum class Model { Auto, K, M, L };

inline constexpr int sharpness_step = 5;
inline constexpr int sharpness_max = 100;

constexpr int NormalizeSharpness(int value) {
    if (value <= 0) return 0;
    if (value >= sharpness_max) return sharpness_max;
    return ((value + sharpness_step / 2) / sharpness_step) * sharpness_step;
}

struct TemporalSelection {
    Technology technology;
    Quality quality;
    Model model;
    bool operator==(const TemporalSelection&) const = default;
};

struct Settings {
    Technology technology = Technology::Dlss;
    Quality quality = Quality::Native;
    Model model = Model::Auto;
    int sharpness = 0;
    constexpr TemporalSelection Temporal() const { return {technology, quality, model}; }
    bool operator==(const Settings&) const = default;
};

struct Size {
    UINT width = 0;
    UINT height = 0;
    bool operator==(const Size&) const = default;
};

constexpr double RenderScalePercent(Size render, Size display) {
    return render.width && render.height && display.width && display.height
        ? 100.0 * render.width / display.width : 0.0;
}

struct RenderRecommendation {
    Size optimal;
    Size minimum;
    Size maximum;
    unsigned jitter_phases = 0;
};

struct Frame {
    ID3D12Resource* color = nullptr;
    ID3D12Resource* depth = nullptr;
    ID3D12Resource* motion = nullptr;
    ID3D12Resource* output = nullptr;
    ID3D12Resource* exposure = nullptr;
    Size render;
    Size display;
    float jitter_x = 0;
    float jitter_y = 0;
    float pre_exposure = 1;
    float frame_time_ms = 0;
    bool reset = true;
    float camera_near = 0;
    float vertical_fov = 0;
};
}
