#pragma once
#include "frame.hpp"
#include <filesystem>
#include <memory>

namespace reframe {
class FsrApi {
public:
    FsrApi();
    ~FsrApi();
    bool Open(ID3D12Device*, const std::filesystem::path&);
    RenderRecommendation Recommend(Quality, Size display) const;
    bool Create(Size render, Size display);
    bool Dispatch(ID3D12GraphicsCommandList*, const Frame&);
    void ReleaseFeature();
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

class XessApi {
public:
    XessApi();
    ~XessApi();
    bool Open(ID3D12Device*, const std::filesystem::path&);
    RenderRecommendation Recommend(Quality, Size output) const;
    bool Create(Quality, Size output);
    bool Dispatch(ID3D12GraphicsCommandList*, const Frame&);
    void ReleaseFeature();
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
}
