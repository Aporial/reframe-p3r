#include "platform.hpp"
#include "ngp.hpp"
#include <nvsdk_ngx.h>
#include <nvsdk_ngx_helpers.h>

namespace reframe {
namespace {
constexpr char project_id[] = "575791b2-19cc-4120-ada2-6add3da293df";
using Preset = NVSDK_NGX_DLSS_Hint_Render_Preset;
struct Mode {
    NVSDK_NGX_PerfQuality_Value quality;
    const char* hint;
    const char* name;
};
constexpr Mode modes[] = {
    {NVSDK_NGX_PerfQuality_Value_DLAA, NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_DLAA, "DLAA"},
    {NVSDK_NGX_PerfQuality_Value_MaxQuality, NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_Quality, "Quality"},
    {NVSDK_NGX_PerfQuality_Value_Balanced, NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_Balanced, "Balanced"},
    {NVSDK_NGX_PerfQuality_Value_MaxPerf, NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_Performance, "Performance"},
};

void NVSDK_CONV NgxLog(const char* message, NVSDK_NGX_Logging_Level, NVSDK_NGX_Feature) {
    if (message && (strstr(message, "error") || strstr(message, "Error"))) Log("NGX: %s", message);
}

Preset PresetFor(TemporalSelection settings) {
    switch (settings.model) {
    case Model::K: return NVSDK_NGX_DLSS_Hint_Render_Preset_K;
    case Model::M: return NVSDK_NGX_DLSS_Hint_Render_Preset_M;
    case Model::L: return NVSDK_NGX_DLSS_Hint_Render_Preset_L;
    case Model::Auto:
        return settings.quality == Quality::Performance
            ? NVSDK_NGX_DLSS_Hint_Render_Preset_M : NVSDK_NGX_DLSS_Hint_Render_Preset_K;
    }
    return NVSDK_NGX_DLSS_Hint_Render_Preset_K;
}
}

DlssSession::~DlssSession() { Close(); }

bool DlssSession::Open(ID3D12Device* device, const std::filesystem::path& directory) {
    if (device_ || directory.empty() || !std::filesystem::is_regular_file(directory / L"nvngx_dlss.dll")) {
        Log("NVIDIA DLSS runtime missing or NGX is already initialized.");
        return false;
    }
    const wchar_t* paths[] = {directory.c_str()};
    NVSDK_NGX_FeatureCommonInfo common{};
    common.PathListInfo.Path = paths;
    common.PathListInfo.Length = 1;
    common.LoggingInfo.LoggingCallback = NgxLog;
    common.LoggingInfo.MinimumLoggingLevel = NVSDK_NGX_LOGGING_LEVEL_ON;
    common.LoggingInfo.DisableOtherLoggingSinks = true;
    const auto result = NVSDK_NGX_D3D12_Init_with_ProjectID(project_id, NVSDK_NGX_ENGINE_TYPE_CUSTOM,
                                                          mod_version, directory.c_str(), device, &common);
    if (NVSDK_NGX_FAILED(result)) {
        Log("NVIDIA DLSS SDK %s: NGX initialization failed (0x%08X).", dlss_sdk, static_cast<unsigned>(result));
        return false;
    }
    device_ = device;
    NVSDK_NGX_Parameter* caps = nullptr;
    int available = 0;
    int driver_update = 0;
    if (NVSDK_NGX_SUCCEED(NVSDK_NGX_D3D12_GetCapabilityParameters(&caps)) && caps) {
        NVSDK_NGX_Parameter_GetI(caps, NVSDK_NGX_Parameter_SuperSampling_Available, &available);
        NVSDK_NGX_Parameter_GetI(caps, NVSDK_NGX_Parameter_SuperSampling_NeedsUpdatedDriver, &driver_update);
        NVSDK_NGX_D3D12_DestroyParameters(caps);
    }
    if (available && !driver_update && NVSDK_NGX_SUCCEED(NVSDK_NGX_D3D12_AllocateParameters(&parameters_))) {
        Log("NVIDIA DLSS SDK %s available on active D3D12 adapter.", dlss_sdk);
        return parameters_ != nullptr;
    }
    Log("NVIDIA DLSS unavailable (driver update required: %d).", driver_update);
    Close();
    return false;
}

RenderRecommendation DlssSession::Recommend(Quality quality, Size output) const {
    if (!device_ || static_cast<size_t>(quality) >= std::size(modes)) return {};
    NVSDK_NGX_Parameter* capabilities = nullptr;
    if (NVSDK_NGX_FAILED(NVSDK_NGX_D3D12_GetCapabilityParameters(&capabilities)) || !capabilities) return {};
    RenderRecommendation result;
    float sharpness = 0;
    const auto status = NGX_DLSS_GET_OPTIMAL_SETTINGS(capabilities, output.width, output.height,
        modes[static_cast<size_t>(quality)].quality, &result.optimal.width, &result.optimal.height,
        &result.maximum.width, &result.maximum.height, &result.minimum.width, &result.minimum.height, &sharpness);
    NVSDK_NGX_D3D12_DestroyParameters(capabilities);
    return NVSDK_NGX_SUCCEED(status) ? result : RenderRecommendation{};
}

bool DlssSession::Create(ID3D12GraphicsCommandList* cmd, TemporalSelection settings, Size render, Size display) {
    if (!parameters_ || feature_ || static_cast<size_t>(settings.quality) >= std::size(modes)) return false;
    const auto& mode = modes[static_cast<std::size_t>(settings.quality)];
    const auto preset = PresetFor(settings);
    NVSDK_NGX_Parameter_SetUI(parameters_, mode.hint, static_cast<unsigned>(preset));
    NVSDK_NGX_DLSS_Create_Params params{};
    params.Feature.InWidth = render.width;
    params.Feature.InHeight = render.height;
    params.Feature.InTargetWidth = display.width;
    params.Feature.InTargetHeight = display.height;
    params.Feature.InPerfQualityValue = mode.quality;
    // Leave the deprecated optional NGX sharpening flag disabled.
    params.InFeatureCreateFlags = NVSDK_NGX_DLSS_Feature_Flags_IsHDR |
        NVSDK_NGX_DLSS_Feature_Flags_MVLowRes | NVSDK_NGX_DLSS_Feature_Flags_DepthInverted;
    const auto result = NGX_D3D12_CREATE_DLSS_EXT(cmd, 1, 1, &feature_, parameters_, &params);
    if (NVSDK_NGX_FAILED(result)) {
        Log("NVIDIA %s creation failed (0x%08X).", mode.name, static_cast<unsigned>(result));
        feature_ = nullptr;
        return false;
    }
    if (!feature_) return false;
    Log("NVIDIA %s ready (model %c, %ux%u -> %ux%u).", mode.name,
        preset == NVSDK_NGX_DLSS_Hint_Render_Preset_M ? 'M' :
        preset == NVSDK_NGX_DLSS_Hint_Render_Preset_L ? 'L' : 'K',
        render.width, render.height, display.width, display.height);
    return true;
}

bool DlssSession::Dispatch(ID3D12GraphicsCommandList* cmd, const Frame& frame) {
    if (!feature_) return false;
    NVSDK_NGX_D3D12_DLSS_Eval_Params params{};
    params.Feature.pInColor = frame.color;
    params.Feature.pInOutput = frame.output;
    params.pInDepth = frame.depth;
    params.pInMotionVectors = frame.motion;
    params.pInExposureTexture = frame.exposure;
    params.InJitterOffsetX = frame.jitter_x;
    params.InJitterOffsetY = frame.jitter_y;
    params.InRenderSubrectDimensions = {frame.render.width, frame.render.height};
    params.InMVScaleX = params.InMVScaleY = params.InExposureScale = 1.0f;
    params.InPreExposure = frame.pre_exposure;
    params.InFrameTimeDeltaInMsec = frame.frame_time_ms;
    params.InReset = frame.reset;
    const auto result = NGX_D3D12_EVALUATE_DLSS_EXT(cmd, feature_, parameters_, &params);
    if (NVSDK_NGX_SUCCEED(result)) return true;
    Log("NVIDIA DLSS evaluation failed (0x%08X).", static_cast<unsigned>(result));
    return false;
}

void DlssSession::ReleaseFeature() {
    if (feature_) NVSDK_NGX_D3D12_ReleaseFeature(feature_);
    feature_ = nullptr;
}

void DlssSession::Close() {
    ReleaseFeature();
    if (parameters_) NVSDK_NGX_D3D12_DestroyParameters(parameters_);
    if (device_) NVSDK_NGX_D3D12_Shutdown1(device_);
    parameters_ = nullptr;
    device_ = nullptr;
}
}
