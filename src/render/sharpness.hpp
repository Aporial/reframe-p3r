#pragma once

#include "frame.hpp"
#include <array>
#include <wrl/client.h>

namespace reframe {
constexpr float CasPeak(int percent) {
    const float amount = NormalizeSharpness(percent) / 100.0f;
    // Fade in the filter and sweep AMD's tuning range through to -1/5.
    return -amount / (8.0f - 3.0f * amount);
}

class SharpnessPass {
public:
    static constexpr UINT target_count = 4;
    static_assert(target_count < 32);
    enum class Status { Disabled, Recorded, Unsupported, PipelineFailed, AllocationFailed, TargetsBusy };
    struct Result {
        Status status;
        uint32_t target = 0;

        bool Succeeded() const noexcept { return status == Status::Disabled || status == Status::Recorded; }
    };

    // The caller holds the output in UAV state and retires recorded lists
    // and their submissions before marking a target reusable or clearing the pass.
    Result Record(ID3D12Device* device, ID3D12GraphicsCommandList* list, ID3D12Resource* output,
                  DXGI_FORMAT format, Size display, float pre_exposure, int percent,
                  uint32_t reusable = 0) noexcept;
    void ClearTargets() noexcept;
    UINT64 AllocatedBytes() const noexcept { return allocated_bytes_; }

private:
    struct Target {
        Microsoft::WRL::ComPtr<ID3D12Resource> output;
        Microsoft::WRL::ComPtr<ID3D12Resource> scratch;
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
        Size display;
        UINT64 bytes = 0;
        UINT64 last_used = 0;
    };
    bool Initialize(ID3D12Device* device) noexcept;
    bool BindTarget(ID3D12Device* device, ID3D12Resource* output, DXGI_FORMAT format,
                    Size display, Target& target, UINT index) noexcept;

    // A scratch texture follows each game output's queue ordering. Distinct
    // outputs can be processed concurrently by different direct queues.
    std::array<Target, target_count> targets_{};
    Microsoft::WRL::ComPtr<ID3D12RootSignature> root_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipeline_;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap_;
    UINT stride_ = 0;
    UINT64 uses_ = 0;
    UINT64 allocated_bytes_ = 0;
    bool initialization_failed_ = false;
};
}
