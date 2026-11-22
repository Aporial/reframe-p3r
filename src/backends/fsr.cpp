#include "platform.hpp"
#include "vendor_backends.hpp"
#include <dx12/ffx_api_dx12.h>
#include <ffx_upscale.h>
#include <array>
#include <cfloat>
#include <cstring>

namespace reframe {
namespace {
constexpr auto upscaler_type = FFX_API_CREATE_CONTEXT_DESC_TYPE_UPSCALE;

bool Version(const char* name, const char* number) {
    if (!name) return false;
    const char* match = std::strstr(name, number);
    if (!match) return false;
    const char next = match[std::strlen(number)];
    return !next || next == ' ' || next == '(';
}
}

struct FsrApi::Impl {
    HMODULE library = nullptr;
    PfnFfxCreateContext create = nullptr;
    PfnFfxDestroyContext destroy = nullptr;
    PfnFfxQuery query = nullptr;
    PfnFfxDispatch dispatch = nullptr;
    ID3D12Device* device = nullptr;
    ffxContext context = nullptr;
    std::array<uint64_t, 2> versions{};
    ~Impl() {
        if (context) destroy(&context, nullptr);
        if (library) FreeLibrary(library);
    }
};

FsrApi::FsrApi() = default;
FsrApi::~FsrApi() = default;

bool FsrApi::Open(ID3D12Device* device, const std::filesystem::path& directory) {
    if (impl_) return true;
    auto state = std::make_unique<Impl>();
    const auto loader = directory / L"amd_fidelityfx_loader_dx12.dll";
    const auto upscaler = directory / L"amd_fidelityfx_upscaler_dx12.dll";
    if (!std::filesystem::is_regular_file(loader) || !std::filesystem::is_regular_file(upscaler)) return false;
    state->library = LoadLibraryExW(loader.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!state->library) return false;
    state->create = reinterpret_cast<PfnFfxCreateContext>(GetProcAddress(state->library, "ffxCreateContext"));
    state->destroy = reinterpret_cast<PfnFfxDestroyContext>(GetProcAddress(state->library, "ffxDestroyContext"));
    state->query = reinterpret_cast<PfnFfxQuery>(GetProcAddress(state->library, "ffxQuery"));
    state->dispatch = reinterpret_cast<PfnFfxDispatch>(GetProcAddress(state->library, "ffxDispatch"));
    if (!state->create || !state->destroy || !state->query || !state->dispatch) return false;
    state->device = device;

    uint64_t count = 0;
    ffxQueryDescGetVersions query{{FFX_API_QUERY_DESC_TYPE_GET_VERSIONS, nullptr}, upscaler_type, device, &count};
    if (state->query(nullptr, &query.header) != FFX_API_RETURN_OK || count == 0 || count > 128) return false;
    std::array<uint64_t, 128> ids{};
    std::array<const char*, 128> names{};
    query.versionIds = ids.data();
    query.versionNames = names.data();
    if (state->query(nullptr, &query.header) != FFX_API_RETURN_OK || count > ids.size()) return false;
    for (uint64_t i = 0; i < count; ++i) {
        if (!names[i]) continue;
        if (Version(names[i], "4.1.2") && !std::strchr(names[i], '*')) state->versions[0] = ids[i];
        if (Version(names[i], "3.1.6") && !std::strchr(names[i], '*')) state->versions[1] = ids[i];
    }
    if (!state->versions[0] && !state->versions[1]) return false;
    Log("AMD FSR SDK 2.4.0 reported bundled 4.1.2=%d, 3.1.6=%d for the active device.",
        state->versions[0] != 0, state->versions[1] != 0);
    impl_ = std::move(state);
    return true;
}

RenderRecommendation FsrApi::Recommend(Quality quality, Size output) const {
    if (!impl_ || quality > Quality::Performance) return {};
    const uint64_t id = impl_->versions[0] ? impl_->versions[0] : impl_->versions[1];
    ffxOverrideVersion version{{FFX_API_DESC_TYPE_OVERRIDE_VERSION, nullptr}, id};
    ffxCreateBackendDX12Desc backend{{FFX_API_CREATE_CONTEXT_DESC_TYPE_BACKEND_DX12, &version.header}, impl_->device};
    RenderRecommendation result;
    ffxQueryDescUpscaleGetRenderResolutionFromQualityMode query{
        {FFX_API_QUERY_DESC_TYPE_UPSCALE_GETRENDERRESOLUTIONFROMQUALITYMODE, &backend.header},
        output.width, output.height, static_cast<uint32_t>(quality), &result.optimal.width, &result.optimal.height};
    if (impl_->query(nullptr, &query.header) != FFX_API_RETURN_OK || !result.optimal.width) return {};
    int32_t phases = 0;
    ffxQueryDescUpscaleGetJitterPhaseCount jitter{{FFX_API_QUERY_DESC_TYPE_UPSCALE_GETJITTERPHASECOUNT, &backend.header},
        result.optimal.width, output.width, &phases};
    if (impl_->query(nullptr, &jitter.header) == FFX_API_RETURN_OK && phases > 0) result.jitter_phases = phases;
    return result;
}

bool FsrApi::Create(Size render, Size output) {
    if (!impl_ || impl_->context) return false;
    for (const auto id : impl_->versions) {
        if (!id) continue;
        ffxOverrideVersion version{{FFX_API_DESC_TYPE_OVERRIDE_VERSION, nullptr}, id};
        ffxCreateBackendDX12Desc backend{{FFX_API_CREATE_CONTEXT_DESC_TYPE_BACKEND_DX12, &version.header}, impl_->device};
        ffxCreateContextDescUpscale shared{{upscaler_type, &backend.header},
            FFX_UPSCALE_ENABLE_HIGH_DYNAMIC_RANGE | FFX_UPSCALE_ENABLE_DEPTH_INVERTED | FFX_UPSCALE_ENABLE_DEPTH_INFINITE,
            {render.width, render.height}, {output.width, output.height}, nullptr};
        ffxContext context = nullptr;
        const auto status = impl_->create(&context, &shared.header, nullptr);
        if (status != FFX_API_RETURN_OK || !context) {
            Log("AMD FSR context creation failed (%u).", status);
            continue;
        }
        impl_->context = context;
        ffxQueryGetProviderVersion provider{{FFX_API_QUERY_DESC_TYPE_GET_PROVIDER_VERSION, nullptr}};
        impl_->query(&impl_->context, &provider.header);
        Log("AMD FSR provider %s (%ux%u -> %ux%u).", provider.versionName ? provider.versionName : "unknown",
            render.width, render.height, output.width, output.height);
        return true;
    }
    return false;
}

void FsrApi::ReleaseFeature() {
    if (impl_ && impl_->context) {
        impl_->destroy(&impl_->context, nullptr);
        impl_->context = nullptr;
    }
}

bool FsrApi::Dispatch(ID3D12GraphicsCommandList* list, const Frame& frame) {
    if (!impl_ || !impl_->context || frame.camera_near <= 0 || frame.vertical_fov <= 0) return false;
    ffxDispatchDescUpscale params{};
    params.header.type = FFX_API_DISPATCH_DESC_TYPE_UPSCALE;
    params.commandList = list;
    params.color = ffxApiGetResourceDX12(frame.color);
    params.depth = ffxApiGetResourceDX12(frame.depth);
    params.motionVectors = ffxApiGetResourceDX12(frame.motion);
    params.exposure = ffxApiGetResourceDX12(frame.exposure);
    params.output = ffxApiGetResourceDX12(frame.output, FFX_API_RESOURCE_STATE_UNORDERED_ACCESS);
    params.jitterOffset = {frame.jitter_x, frame.jitter_y};
    params.motionVectorScale = {1, 1};
    params.renderSize = {frame.render.width, frame.render.height};
    params.upscaleSize = {frame.display.width, frame.display.height};
    params.frameTimeDelta = frame.frame_time_ms;
    params.preExposure = frame.pre_exposure;
    params.reset = frame.reset;
    params.cameraNear = frame.camera_near;
    params.cameraFar = FLT_MAX;
    params.cameraFovAngleVertical = frame.vertical_fov;
    params.viewSpaceToMetersFactor = 0.01f;
    const auto result = impl_->dispatch(&impl_->context, &params.header);
    if (result == FFX_API_RETURN_OK) return true;
    Log("AMD FSR dispatch failed (%u).", result);
    return false;
}
}
