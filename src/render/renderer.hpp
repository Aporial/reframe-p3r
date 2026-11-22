#pragma once
#include "frame.hpp"
#include <array>

namespace reframe {
struct View {
    ID3D12Resource* resource = nullptr;
    DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
};

struct GameFrame {
    Frame frame;
    View depth;
    View velocity;
    View eye;
    float clip_to_previous[16]{};
    float mip_bias = 0;
    uint32_t jitter_index = 0;
    uint32_t jitter_phases = 0;
    Size color_allocation;
    Size output_allocation;
};

struct MenuGraphicsSelection {
    Settings current;
    std::array<bool, 4> available;
    Size output;
};

MenuGraphicsSelection ReadMenuGraphics();
RenderRecommendation RecommendMenuQuality(Technology technology, Quality quality, Size output);
bool ApplyMenuGraphics(Settings settings);
bool InitializeUpscalers(ID3D12Device* device);
bool RenderUpscaled(ID3D12GraphicsCommandList* list, GameFrame& frame);
bool BeginUpscaleSubmission(UINT count, ID3D12CommandList* const* lists) noexcept;
void EndUpscaleSubmission(ID3D12CommandQueue* queue) noexcept;
void UpscaleCommandsDiscarded(ID3D12GraphicsCommandList* list) noexcept;
void ObserveUpscaleQueue(ID3D12CommandQueue* queue) noexcept;
}
