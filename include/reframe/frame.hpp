#pragma once
#include <d3d12.h>
#include <cstdint>

namespace reframe {
enum class Quality { Native, Quality, Balanced, Performance, UltraQuality, UltraQualityPlus, Count };
enum class Technology { Taa, Dlss, Fsr, Xess };
enum class Model { Auto, K, M, L };

struct Settings {
    Technology technology = Technology::Dlss;
    Quality quality = Quality::Native;
    Model model = Model::Auto;
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
