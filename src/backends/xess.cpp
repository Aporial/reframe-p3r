#include "platform.hpp"
#include "vendor_backends.hpp"
#include <xess/xess_d3d12.h>
#include <array>

namespace reframe {
namespace {
constexpr xess_quality_settings_t presets[] = {XESS_QUALITY_SETTING_AA, XESS_QUALITY_SETTING_QUALITY,
    XESS_QUALITY_SETTING_BALANCED, XESS_QUALITY_SETTING_PERFORMANCE,
    XESS_QUALITY_SETTING_ULTRA_QUALITY, XESS_QUALITY_SETTING_ULTRA_QUALITY_PLUS};
constexpr uint32_t flags = XESS_INIT_FLAG_INVERTED_DEPTH | XESS_INIT_FLAG_EXPOSURE_SCALE_TEXTURE;
}

struct XessApi::Impl {
    HMODULE library = nullptr;
    ID3D12Device* device = nullptr;
    decltype(&xessD3D12CreateContext) create = nullptr;
    decltype(&xessDestroyContext) destroy = nullptr;
    decltype(&xessD3D12Init) init = nullptr;
    decltype(&xessD3D12Execute) execute = nullptr;
    decltype(&xessSetExposureMultiplier) exposure = nullptr;
    decltype(&xessGetOptimalInputResolution) resolution = nullptr;
    xess_context_handle_t context = nullptr;
    xess_context_handle_t probe = nullptr;

    template<typename T>
    T Export(const char* name) const { return reinterpret_cast<T>(GetProcAddress(library, name)); }

    ~Impl() {
        if (context && destroy) destroy(context);
        if (probe && destroy) destroy(probe);
        if (library) FreeLibrary(library);
    }
};

XessApi::XessApi() = default;
XessApi::~XessApi() = default;

bool XessApi::Open(ID3D12Device* device, const std::filesystem::path& directory) {
    if (impl_) return true;
    auto state = std::make_unique<Impl>();
    const auto dll = directory / L"libxess.dll";
    if (!std::filesystem::is_regular_file(dll)) return false;
    state->library = LoadLibraryExW(dll.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!state->library) return false;
    state->device = device;
    state->create = state->Export<decltype(state->create)>("xessD3D12CreateContext");
    const auto version = state->Export<decltype(&xessGetVersion)>("xessGetVersion");
    state->destroy = state->Export<decltype(state->destroy)>("xessDestroyContext");
    state->init = state->Export<decltype(state->init)>("xessD3D12Init");
    state->execute = state->Export<decltype(state->execute)>("xessD3D12Execute");
    state->exposure = state->Export<decltype(state->exposure)>("xessSetExposureMultiplier");
    state->resolution = state->Export<decltype(state->resolution)>("xessGetOptimalInputResolution");
    if (!state->create || !version || !state->destroy || !state->init || !state->execute || !state->exposure || !state->resolution)
        return false;
    const auto result = state->create(device, &state->probe);
    if (result != XESS_RESULT_SUCCESS || !state->probe) {
        Log("Intel XeSS SR is unavailable (%d).", result);
        return false;
    }
    xess_version_t sdk{};
    if (version(&sdk) == XESS_RESULT_SUCCESS) Log("Intel XeSS SR SDK %u.%u.%u is available.", sdk.major, sdk.minor, sdk.patch);
    impl_ = std::move(state);
    return true;
}

RenderRecommendation XessApi::Recommend(Quality quality, Size output) const {
    if (!impl_ || !output.width || static_cast<size_t>(quality) >= std::size(presets)) return {};
    const xess_2d_t size{output.width, output.height};
    xess_2d_t optimal{}, min{}, max{};
    if (impl_->resolution(impl_->probe, &size, presets[static_cast<size_t>(quality)], &optimal, &min, &max) != XESS_RESULT_SUCCESS)
        return {};
    return {{optimal.x, optimal.y}, {min.x, min.y}, {max.x, max.y}};
}

bool XessApi::Create(Quality quality, Size output) {
    if (!impl_ || impl_->context || static_cast<size_t>(quality) >= std::size(presets)) return false;
    xess_context_handle_t context = nullptr;
    if (impl_->create(impl_->device, &context) != XESS_RESULT_SUCCESS || !context) return false;
    xess_d3d12_init_params_t params{};
    params.outputResolution = {output.width, output.height};
    params.qualitySetting = presets[static_cast<size_t>(quality)];
    params.initFlags = flags;
    params.creationNodeMask = params.visibleNodeMask = 1;
    const auto result = impl_->init(context, &params);
    if (result != XESS_RESULT_SUCCESS) {
        Log("Intel XeSS SR initialization failed (%d).", result);
        impl_->destroy(context);
        return false;
    }
    impl_->context = context;
    Log("Intel XeSS SR ready, output %ux%u.", output.width, output.height);
    return true;
}

void XessApi::ReleaseFeature() {
    if (impl_ && impl_->context) {
        impl_->destroy(impl_->context);
        impl_->context = nullptr;
    }
}

bool XessApi::Dispatch(ID3D12GraphicsCommandList* list, const Frame& frame) {
    if (!impl_ || !impl_->context ||
        impl_->exposure(impl_->context, 1.0f / frame.pre_exposure) != XESS_RESULT_SUCCESS) return false;
    xess_d3d12_execute_params_t params{};
    params.pColorTexture = frame.color;
    params.pVelocityTexture = frame.motion;
    params.pDepthTexture = frame.depth;
    params.pExposureScaleTexture = frame.exposure;
    params.pOutputTexture = frame.output;
    params.jitterOffsetX = frame.jitter_x;
    params.jitterOffsetY = frame.jitter_y;
    params.exposureScale = 1;
    params.resetHistory = frame.reset;
    params.inputWidth = frame.render.width;
    params.inputHeight = frame.render.height;
    const auto result = impl_->execute(impl_->context, list, &params);
    if (result == XESS_RESULT_SUCCESS) return true;
    Log("Intel XeSS SR dispatch failed (%d).", result);
    return false;
}
}
