#pragma once
#include "frame.hpp"
#include <filesystem>

struct NVSDK_NGX_Handle;
struct NVSDK_NGX_Parameter;

namespace reframe {
class DlssSession {
public:
    DlssSession() = default;
    DlssSession(const DlssSession&) = delete;
    DlssSession& operator=(const DlssSession&) = delete;
    ~DlssSession();
    bool Open(ID3D12Device*, const std::filesystem::path&);
    RenderRecommendation Recommend(Quality, Size output) const;
    bool Create(ID3D12GraphicsCommandList*, Settings, Size render, Size display);
    bool Dispatch(ID3D12GraphicsCommandList*, const Frame&);
    void ReleaseFeature();
    void Close();
private:
    ID3D12Device* device_ = nullptr;
    NVSDK_NGX_Parameter* parameters_ = nullptr;
    NVSDK_NGX_Handle* feature_ = nullptr;
};
}
