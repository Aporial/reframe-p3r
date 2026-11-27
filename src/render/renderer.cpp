#include "platform.hpp"
#include "settings.hpp"
#include "renderer.hpp"
#include "ngp.hpp"
#include "vendor_backends.hpp"
#include "temporal_settings.hpp"
#include "sharpness.hpp"
#include "town_map_view.hpp"
#include "motion_cs.h"
#include <dxgi1_6.h>
#include <wrl/client.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <bit>
#include <cstring>
#include <chrono>
#include <cmath>
#include <memory>
#include <mutex>
#include <numeric>
#include <unordered_map>
#include <vector>

using Microsoft::WRL::ComPtr;

namespace reframe {
namespace {
using Clock = std::chrono::steady_clock;
constexpr UINT descriptors_per_pass = 5;
constexpr UINT max_bindings = 1024;
constexpr UINT town_map_slots = 16;
constexpr UINT scratch_targets = SharpnessPass::target_count + town_map_slots;
constexpr UINT sharpness_mask = (1u << SharpnessPass::target_count) - 1u;
constexpr size_t town_map_alignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
constexpr size_t town_map_slot_size = (sizeof(TownMapViewConstants) + town_map_alignment - 1) &
                                     ~(town_map_alignment - 1);
static_assert(scratch_targets < 32);
constexpr GUID resource_serial{0x12aad387,0x9ca5,0x4a08,{0x81,0xfd,0x49,0xd4,0x14,0x7b,0xfc,0x15}};

struct MotionPass {
    Size size;
    ComPtr<ID3D12Resource> motion;
    ComPtr<ID3D12Resource> exposure;
};

struct Binding {
    MotionPass* pass = nullptr;
    std::array<uint64_t, 3> resources;
    std::array<DXGI_FORMAT, 3> formats;
};

struct FrameStatistics {
    ComPtr<IDXGIAdapter3> adapter;
    Clock::time_point previous;
    Settings settings;
    Size render, display;
    std::array<float, 240> milliseconds{};
    size_t count = 0;
    size_t sharpened_count = 0;

    void Record(Settings current, Size input, Size output, Clock::time_point now, bool sharpened = false);
};

struct Plan {
    TemporalSelection settings{};
    Size display;
    RenderRecommendation recommendation;
    float percentage = 100;
};

struct ConfiguredFeature {
    TemporalSelection settings{};
    Size render, display;
    bool operator==(const ConfiguredFeature&) const = default;
};

struct QueueProgress {
    ComPtr<ID3D12CommandQueue> queue;
    ComPtr<ID3D12Fence> fence;
    UINT64 last_signal = 0;
    std::array<UINT64, scratch_targets> scratch_signals{};
};

struct MainView {
    Size pass_output;
    Size display;
};

struct Runtime {
    std::recursive_mutex mutex;
    ComPtr<ID3D12Device> device;
    ComPtr<ID3D12RootSignature> root;
    ComPtr<ID3D12PipelineState> pipeline;
    ComPtr<ID3D12DescriptorHeap> heap;
    UINT descriptor_size = 0;
    std::vector<std::unique_ptr<MotionPass>> passes;
    std::vector<Binding> bindings;
    DlssSession dlss;
    FsrApi fsr;
    XessApi xess;
    SharpnessPass sharpness_pass;
    ComPtr<ID3D12Resource> town_map_upload;
    std::byte* town_map_mapped = nullptr;
    uint32_t reported_sharpness_failures = 0;
    bool reported_sharpness_dispatch = false;
    Plan plan;
    ConfiguredFeature feature;
    std::vector<QueueProgress> queues;
    std::unordered_map<ID3D12CommandList*, uint32_t> recorded;
    unsigned submitting = 0;
    bool untracked_work = false;
    bool benchmark = false;
    bool diagnostic_hotkeys = false;
    std::array<bool, 4> available{};
    Technology active = Technology::Taa;
    Technology last_vendor = Technology::Taa;
    Quality quality = Quality::Native;
    Model model = Model::Auto;
    int sharpness = 0;
    int last_frame_sharpness = 0;
    Clock::time_point previous;
    TemporalSettings temporal;
    FrameStatistics stats;
    HWND game_window = nullptr;
    MainView main_view;
    uint32_t last_phases = 0;
    float last_mip_bias = 0;
    unsigned frames = 0;
    unsigned waiting_frames = 0;
    bool reported_off_plan_view = false;
    bool toggle_down = false;
    bool cycle_down = false;
    bool quality_down = false;

    Settings Selection() const { return {active, quality, model, sharpness}; }
};

constexpr const char* Names[] = {"Original TAA", "NVIDIA DLSS", "AMD FSR", "Intel XeSS"};
constexpr size_t Slot(Technology technology) { return static_cast<size_t>(technology); }
constexpr const char* QualityNames[] = {"Native AA", "Quality", "Balanced", "Performance", "Ultra Quality", "Ultra Quality Plus"};
constexpr auto QualityCount = static_cast<size_t>(Quality::Count);

const char* ModeName(TemporalSelection settings) {
    if (settings.technology == Technology::Taa) return "Original";
    if (settings.technology == Technology::Dlss && settings.quality == Quality::Native) return "DLAA";
    return QualityNames[static_cast<size_t>(settings.quality)];
}

void FrameStatistics::Record(Settings current, Size input, Size output, Clock::time_point now, bool sharpened) {
    const float elapsed = std::chrono::duration<float, std::milli>(now - previous).count();
    if (current != settings || render != input || display != output || previous == Clock::time_point{}) {
        count = 0;
        sharpened_count = 0;
        settings = current;
        render = input;
        display = output;
    } else if (elapsed > 0) {
        milliseconds[count++] = elapsed;
        sharpened_count += sharpened;
    }
    previous = now;
    if (count != milliseconds.size()) return;
    const float mean = std::accumulate(milliseconds.begin(), milliseconds.end(), 0.0f) / count;
    std::sort(milliseconds.begin(), milliseconds.end());
    const float p95 = milliseconds[count * 95 / 100 - 1];
    const size_t slow_count = (count + 99) / 100;
    const float slow_mean = std::accumulate(milliseconds.end() - slow_count, milliseconds.end(), 0.0f) / slow_count;
    DXGI_QUERY_VIDEO_MEMORY_INFO memory{};
    if (adapter && SUCCEEDED(adapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memory))) {
        Log("Main-view cadence [%s / %s %ux%u -> %ux%u, Sharpness %d%%], %zu frames (%zu CAS): %.1f FPS, %.1f 1%% low, %.2f ms mean, %.2f ms p95, process VRAM %.1f MiB / %.1f MiB budget.",
            Names[Slot(current.technology)], ModeName(current.Temporal()), input.width, input.height, output.width, output.height, current.sharpness,
            count, sharpened_count, 1000.0f / mean, 1000.0f / slow_mean, mean, p95,
            memory.CurrentUsage / 1048576.0, memory.Budget / 1048576.0);
    } else {
        Log("Main-view cadence [%s / %s %ux%u -> %ux%u, Sharpness %d%%], %zu frames (%zu CAS): %.1f FPS, %.1f 1%% low, %.2f ms mean, %.2f ms p95.",
            Names[Slot(current.technology)], ModeName(current.Temporal()), input.width, input.height, output.width, output.height, current.sharpness,
            count, sharpened_count, 1000.0f / mean, 1000.0f / slow_mean, mean, p95);
    }
    count = 0;
    sharpened_count = 0;
}

Runtime& App() {
    // D3D12 and NGX objects remain valid until the operating system tears down the process.
    static Runtime* instance = new Runtime;
    return *instance;
}

bool InitializeMotionPass(Runtime& app) {
    D3D12_DESCRIPTOR_RANGE ranges[2]{};
    ranges[0] = {D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0, 0, 0};
    ranges[1] = {D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 2, 0, 0, 3};
    D3D12_ROOT_PARAMETER parameters[2]{};
    parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    parameters[0].Constants = {0, 0, 20};
    parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    parameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    parameters[1].DescriptorTable = {2, ranges};
    parameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    D3D12_ROOT_SIGNATURE_DESC root{2, parameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_NONE};
    using Serialize = decltype(&D3D12SerializeRootSignature);
    const auto serialize = reinterpret_cast<Serialize>(SystemD3D12("D3D12SerializeRootSignature"));
    ComPtr<ID3DBlob> bytes;
    if (!serialize || FAILED(serialize(&root, D3D_ROOT_SIGNATURE_VERSION_1_0, &bytes, nullptr))) return false;
    auto* device = app.device.Get();
    if (FAILED(device->CreateRootSignature(0, bytes->GetBufferPointer(), bytes->GetBufferSize(),
                                           IID_PPV_ARGS(&app.root)))) return false;
    D3D12_COMPUTE_PIPELINE_STATE_DESC pipeline{};
    pipeline.pRootSignature = app.root.Get();
    pipeline.CS = {MotionCs, sizeof(MotionCs)};
    if (FAILED(device->CreateComputePipelineState(&pipeline, IID_PPV_ARGS(&app.pipeline)))) return false;
    D3D12_DESCRIPTOR_HEAP_DESC heap{};
    heap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heap.NumDescriptors = max_bindings * descriptors_per_pass;
    heap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    if (FAILED(device->CreateDescriptorHeap(&heap, IID_PPV_ARGS(&app.heap)))) return false;
    app.descriptor_size = device->GetDescriptorHandleIncrementSize(heap.Type);
    app.bindings.reserve(max_bindings);
    return app.descriptor_size != 0;
}

bool CreateTexture(ID3D12Device* device, Size size, DXGI_FORMAT format, ID3D12Resource** out) {
    D3D12_HEAP_PROPERTIES heap{};
    heap.Type = D3D12_HEAP_TYPE_DEFAULT;
    heap.CreationNodeMask = heap.VisibleNodeMask = 1;
    D3D12_RESOURCE_DESC desc{};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width = size.width;
    desc.Height = size.height;
    desc.MipLevels = desc.DepthOrArraySize = 1;
    desc.SampleDesc.Count = 1;
    desc.Format = format;
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    return SUCCEEDED(device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc,
                   D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, nullptr, IID_PPV_ARGS(out)));
}

MotionPass* Pass(Runtime& app, Size size) {
    for (const auto& pass : app.passes) if (pass->size == size) return pass.get();
    if (!app.passes.empty()) return nullptr;
    auto pass = std::make_unique<MotionPass>();
    pass->size = size;
    if (!CreateTexture(app.device.Get(), size, DXGI_FORMAT_R16G16_FLOAT, &pass->motion) ||
        !CreateTexture(app.device.Get(), {1,1}, DXGI_FORMAT_R32_FLOAT, &pass->exposure)) return nullptr;
    auto* ptr = pass.get();
    app.passes.push_back(std::move(pass));
    return ptr;
}

bool FindBinding(Runtime& app, MotionPass& pass, const GameFrame& frame, D3D12_GPU_DESCRIPTOR_HANDLE& gpu) {
    const std::array<View, 3> views{frame.velocity, frame.depth, frame.eye};
    std::array<uint64_t, 3> serials{};
    static std::atomic_uint64_t next_serial{1};
    for (size_t i = 0; i < views.size(); ++i) {
        UINT size = sizeof(serials[i]);
        if (FAILED(views[i].resource->GetPrivateData(resource_serial, &size, &serials[i]))) {
            serials[i] = next_serial.fetch_add(1, std::memory_order_relaxed);
            if (FAILED(views[i].resource->SetPrivateData(resource_serial, sizeof(serials[i]), &serials[i]))) return false;
        }
    }
    size_t index = 0;
    for (; index < app.bindings.size(); ++index) {
        const auto& binding = app.bindings[index];
        if (binding.pass != &pass) continue;
        bool match = true;
        for (size_t i = 0; i < views.size(); ++i)
            match &= binding.resources[i] == serials[i] && binding.formats[i] == views[i].format;
        if (match) break;
    }
    if (index == max_bindings) return false;
    const UINT64 offset = index * descriptors_per_pass * app.descriptor_size;
    if (index == app.bindings.size()) {
        auto cpu = app.heap->GetCPUDescriptorHandleForHeapStart();
        cpu.ptr += offset;
        Binding binding;
        binding.pass = &pass;
        binding.resources = serials;
        for (size_t i = 0; i < views.size(); ++i) {
            const auto view = views[i];
            D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
            desc.Format = view.format;
            desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            desc.Texture2D.MipLevels = 1;
            app.device->CreateShaderResourceView(view.resource, &desc, cpu);
            binding.formats[i] = view.format;
            cpu.ptr += app.descriptor_size;
        }
        for (auto* resource : {pass.motion.Get(), pass.exposure.Get()}) {
            D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};
            desc.Format = resource->GetDesc().Format;
            desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
            app.device->CreateUnorderedAccessView(resource, nullptr, &desc, cpu);
            cpu.ptr += app.descriptor_size;
        }
        // The original TAA holds these inputs alive for the submitted list.
        // Immutable descriptors use unique resource IDs, so reuse of a pointer
        // in another scene can't reuse the descriptor of the old texture.
        app.bindings.push_back(std::move(binding));
    }
    gpu = app.heap->GetGPUDescriptorHandleForHeapStart();
    gpu.ptr += offset;
    return true;
}

void RecordMotion(Runtime& app, MotionPass& pass, ID3D12GraphicsCommandList* list, GameFrame& game,
                  D3D12_GPU_DESCRIPTOR_HANDLE binding) {
    D3D12_RESOURCE_BARRIER barriers[2]{};
    ID3D12Resource* textures[]{pass.motion.Get(), pass.exposure.Get()};
    for (size_t i = 0; i < std::size(barriers); ++i) {
        barriers[i].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barriers[i].Transition = {textures[i], D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                                  D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS};
    }
    list->ResourceBarrier(2, barriers);
    ID3D12DescriptorHeap* heaps[]{app.heap.Get()};
    list->SetDescriptorHeaps(1, heaps);
    list->SetPipelineState(app.pipeline.Get());
    list->SetComputeRootSignature(app.root.Get());
    list->SetComputeRoot32BitConstants(0, 16, game.clip_to_previous, 0);
    const float input_size[] = {static_cast<float>(pass.size.width), static_cast<float>(pass.size.height), 0, 0};
    list->SetComputeRoot32BitConstants(0, 4, input_size, 16);
    list->SetComputeRootDescriptorTable(1, binding);
    list->Dispatch((pass.size.width + 15) / 16, (pass.size.height + 3) / 4, 1);
    for (auto& barrier : barriers) std::swap(barrier.Transition.StateBefore, barrier.Transition.StateAfter);
    list->ResourceBarrier(2, barriers);
    game.frame.motion = pass.motion.Get();
    game.frame.exposure = pass.exposure.Get();
}

void PersistSelection(const Runtime& app) {
    if (!SaveGraphicsConfig(DataDirectory(), app.Selection(), app.last_vendor))
        Log("Failed to save graphics settings.");
}

void SaveSelection(Runtime& app) {
    app.previous = {};
    app.frames = 0;
    app.waiting_frames = 0;
    PersistSelection(app);
    Log("Selected %s / %s.", Names[Slot(app.active)], ModeName(app.Selection().Temporal()));
}

void SetTechnology(Runtime& app, Technology tech) {
    if (app.active == tech) return;
    app.active = tech;
    if (tech != Technology::Taa) app.last_vendor = tech;
    SaveSelection(app);
}

bool Near(Size a, Size b, int tolerance = 2) {
    return std::abs(static_cast<int>(a.width) - static_cast<int>(b.width)) <= tolerance &&
           std::abs(static_cast<int>(a.height) - static_cast<int>(b.height)) <= tolerance;
}

Size ClientSize(HWND window) {
    DWORD owner = 0;
    if (!window || !GetWindowThreadProcessId(window, &owner) || owner != GetCurrentProcessId() || IsIconic(window))
        return {};
    RECT client{};
    if (!GetClientRect(window, &client) || client.right < 64 || client.bottom < 64) return {};
    return {static_cast<UINT>(client.right), static_cast<UINT>(client.bottom)};
}

Size MainDisplay(Runtime& app, Size pass_output, Size render) {
    const auto matched = [&](Size client) {
        if (!client.width) return false;
        if (client == pass_output) return true;
        // Stock TAA followed by a spatial scale operates at the internal size.
        // Accept the old size while the render thread catches up to a new scale.
        return render == pass_output &&
            (app.temporal.MatchesScreenPercentage(client, pass_output) ||
             (app.main_view.display == client && app.main_view.pass_output == pass_output));
    };
    const HWND foreground = GetForegroundWindow();
    const Size foreground_size = ClientSize(foreground);
    if (matched(foreground_size)) {
        app.game_window = foreground;
        return foreground_size;
    }
    if (app.game_window) {
        const Size last_window = ClientSize(app.game_window);
        return matched(last_window) ? last_window : Size{};
    }
    if (foreground_size.width) return {};
    if (!app.main_view.display.width) return pass_output;
    return matched(app.main_view.display) ? app.main_view.display : Size{};
}

RenderRecommendation Recommend(Runtime& app, Technology technology, Quality quality, Size output) {
    if (!app.temporal.CanScale()) return {};
    switch (technology) {
    case Technology::Dlss: return app.dlss.Recommend(quality, output);
    case Technology::Fsr: return app.fsr.Recommend(quality, output);
    case Technology::Xess: return app.xess.Recommend(quality, output);
    default: return {};
    }
}

bool ValidTarget(Size target, Size output, Quality quality) {
    if (quality == Quality::Native) return output.width && output.height && target == output;
    return target.width && target.height && target.width <= output.width && target.height <= output.height;
}

Size MenuDisplay(const Runtime& app) {
    if (const Size display = ClientSize(GetForegroundWindow()); display.width) return display;
    if (const Size display = ClientSize(app.game_window); display.width) return display;
    return app.main_view.display;
}

void UpdateToggle(Runtime& app, Size output) {
    DWORD owner = 0;
    GetWindowThreadProcessId(GetForegroundWindow(), &owner);
    const bool down = owner == GetCurrentProcessId() && (GetAsyncKeyState(VK_F9) & 0x8000);
    if (down && !app.toggle_down) {
        const bool resume = app.active == Technology::Taa && app.available[Slot(app.last_vendor)];
        SetTechnology(app, resume ? app.last_vendor : Technology::Taa);
    }
    app.toggle_down = down;
    const bool cycle = owner == GetCurrentProcessId() && (GetAsyncKeyState(VK_F8) & 0x8000);
    if (cycle && !app.cycle_down) {
        for (size_t offset = 1; offset < app.available.size(); ++offset) {
            const size_t next = (Slot(app.active) + offset) % app.available.size();
            if (!next || !app.available[next]) continue;
            SetTechnology(app, static_cast<Technology>(next));
            break;
        }
    }
    app.cycle_down = cycle;
    const bool quality = owner == GetCurrentProcessId() && (GetAsyncKeyState(VK_F7) & 0x8000);
    if (quality && !app.quality_down) {
        const auto backend = app.active == Technology::Taa ? app.last_vendor : app.active;
        constexpr Quality order[] = {Quality::Native, Quality::UltraQualityPlus, Quality::UltraQuality,
                                     Quality::Quality, Quality::Balanced, Quality::Performance};
        const size_t start = std::find(std::begin(order), std::end(order), app.quality) - std::begin(order);
        for (size_t step = 1; step < std::size(order); ++step) {
            const auto next = order[(start + step) % std::size(order)];
            if (!Recommend(app, backend, next, output).optimal.width) continue;
            app.quality = next;
            SaveSelection(app);
            break;
        }
    }
    app.quality_down = quality;
}

bool FinishedGpuWork(const Runtime& app) {
    if (app.untracked_work || app.submitting || !app.recorded.empty()) return false;
    for (const auto& q : app.queues) {
        const auto completed = q.fence->GetCompletedValue();
        if (completed == UINT64_MAX || completed < q.last_signal) return false;
    }
    return true;
}

uint32_t ReusableScratchTargets(const Runtime& app) {
    if (app.untracked_work || app.submitting) return 0;
    uint32_t busy = 0;
    for (const auto& [list, targets] : app.recorded) busy |= targets;
    for (const auto& queue : app.queues) {
        const auto completed = queue.fence->GetCompletedValue();
        if (completed == UINT64_MAX) return 0;
        for (UINT i = 0; i < queue.scratch_signals.size(); ++i)
            if (queue.scratch_signals[i] > completed) busy |= 1u << i;
    }
    return ((1u << scratch_targets) - 1u) & ~busy;
}

const char* SharpnessStatusReason(SharpnessPass::Status status) {
    switch (status) {
    case SharpnessPass::Status::Unsupported: return "unsupported output or exposure";
    case SharpnessPass::Status::PipelineFailed: return "pipeline initialization failed";
    case SharpnessPass::Status::AllocationFailed: return "scratch binding failed";
    case SharpnessPass::Status::TargetsBusy: return "all scratch bindings still in use";
    default: return "unknown";
    }
}

bool ReleaseFeature(Runtime& app) {
    if (!app.feature.display.width) return true;
    if (!FinishedGpuWork(app)) return false;
    app.dlss.ReleaseFeature();
    app.fsr.ReleaseFeature();
    app.xess.ReleaseFeature();
    app.sharpness_pass.ClearTargets();
    app.reported_sharpness_failures = 0;
    app.reported_sharpness_dispatch = false;
    app.bindings.clear();
    app.passes.clear();
    app.feature = {};
    return true;
}

int HaltonBaseCount(const Plan& plan, Size render) {
    const double target_fraction = plan.percentage / 100.0;
    const double current_fraction = std::min(double(render.width) / plan.display.width,
                                             double(render.height) / plan.display.height);
    // During a mode switch either extent can still be present. Limit the
    // base count using the larger area amplification of the two cases.
    const double worst_fraction = std::clamp(std::min(target_fraction, current_fraction), 0.25, 1.0);
    const int phase_budget = static_cast<int>(127 * worst_fraction * worst_fraction);
    const double target_area = target_fraction * target_fraction;
    const int sdk_base = static_cast<int>(std::ceil(plan.recommendation.jitter_phases * target_area));
    return std::clamp(std::max(16, sdk_base), 1, phase_budget);
}

bool Accepts(const Plan& plan, Size input) {
    if (plan.settings.quality == Quality::Native) return input == plan.display;
    const auto& rec = plan.recommendation;
    return Near(rec.optimal, input) &&
        (!rec.minimum.width || (input.width >= rec.minimum.width && input.height >= rec.minimum.height)) &&
        (!rec.maximum.width || (input.width <= rec.maximum.width && input.height <= rec.maximum.height));
}
}

MenuGraphicsSelection ReadMenuGraphics() {
    auto& app = App();
    std::lock_guard lock(app.mutex);
    auto available = app.available;
    available[Slot(Technology::Taa)] = true;
    return {app.Selection(), available, MenuDisplay(app)};
}

RenderRecommendation RecommendMenuQuality(Technology technology, Quality quality, Size output) {
    auto& app = App();
    std::lock_guard lock(app.mutex);
    if (!output.width || !output.height || Slot(technology) >= app.available.size() ||
        !app.available[Slot(technology)]) return {};
    return Recommend(app, technology, quality, output);
}

bool ApplyMenuGraphics(Settings settings) {
    auto& app = App();
    std::lock_guard lock(app.mutex);
    const auto i = Slot(settings.technology);
    if (i >= app.available.size() || (settings.technology != Technology::Taa && !app.available[i]) ||
        settings.quality >= Quality::Count || settings.model > Model::L) return false;
    settings.sharpness = NormalizeSharpness(settings.sharpness);
    const auto current = app.Selection();
    if (settings.Temporal() == current.Temporal()) {
        if (settings.sharpness != current.sharpness) {
            app.sharpness = settings.sharpness;
            PersistSelection(app);
        }
        return true;
    }
    if (settings.technology == Technology::Taa) {
        app.temporal.RestoreScreenPercentage();
    } else {
        if (!app.temporal.CanScale()) return false;
        const Size output = MenuDisplay(app);
        if (output.width) {
            const auto optimal = Recommend(app, settings.technology, settings.quality, output).optimal;
            if (ValidTarget(optimal, output, settings.quality)) {
                const auto percentage = static_cast<float>(RenderScalePercent(optimal, output));
                if (app.temporal.SetScreenPercentage(percentage))
                    Log("Native menu requested gameplay viewport %.3f%% for target %ux%u -> %ux%u.",
                        percentage, optimal.width, optimal.height, output.width, output.height);
            }
        }
    }
    // The render path verifies the new view and owns GPU context changes.
    const bool mode_changed = app.quality != settings.quality || app.model != settings.model;
    app.quality = settings.quality;
    app.model = settings.model;
    app.sharpness = settings.sharpness;
    if (app.active != settings.technology) SetTechnology(app, settings.technology);
    else if (mode_changed) SaveSelection(app);
    return true;
}

D3D12_GPU_VIRTUAL_ADDRESS CopyTownMapView(ID3D12GraphicsCommandList* list, const TownMapViewConstants& view) {
    auto& app = App();
    std::lock_guard lock(app.mutex);
    if (!app.device || app.queues.empty()) return 0;
    const uint32_t mask = ReusableScratchTargets(app) & ~sharpness_mask;
    if (!mask) return 0;
    const auto bit = std::countr_zero(mask);
    const auto index = bit - SharpnessPass::target_count;

    if (!app.town_map_upload) {
        D3D12_HEAP_PROPERTIES heap{};
        heap.Type = D3D12_HEAP_TYPE_UPLOAD;
        heap.CreationNodeMask = heap.VisibleNodeMask = 1;
        D3D12_RESOURCE_DESC desc{};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        desc.Width = town_map_slot_size * town_map_slots;
        desc.Height = 1;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.SampleDesc.Count = 1;
        desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        ComPtr<ID3D12Resource> resource;
        if (FAILED(app.device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc,
                   D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource)))) return 0;
        void* mapped = nullptr;
        const D3D12_RANGE empty{0, 0};
        if (FAILED(resource->Map(0, &empty, &mapped)) || !mapped) return 0;
        app.town_map_mapped = static_cast<std::byte*>(mapped);
        app.town_map_upload = std::move(resource);
    }
    const auto offset = index * town_map_slot_size;
    app.recorded.try_emplace(list, 0).first->second |= 1u << bit;
    std::memcpy(app.town_map_mapped + offset, &view, sizeof(view));
    return app.town_map_upload->GetGPUVirtualAddress() + offset;
}

void ObserveUpscaleQueue(ID3D12CommandQueue* queue) noexcept {
    try {
        auto& app = App();
        std::lock_guard lock(app.mutex);
        for (const auto& q : app.queues) if (q.queue.Get() == queue) return;
        QueueProgress state;
        state.queue = queue;
        if (SUCCEEDED(app.device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&state.fence)))) {
            app.queues.push_back(std::move(state));
            Log("Tracking direct queue %p for SDK lifetime fences.", static_cast<void*>(queue));
        }
    } catch (...) { Log("Unable to track a direct queue."); }
}

UpscaleSubmission BeginUpscaleSubmission(UINT count, ID3D12CommandList* const* lists) noexcept {
    auto& app = App();
    std::lock_guard lock(app.mutex);
    UpscaleSubmission submission;
    for (UINT i = 0; i < count; ++i) {
        const auto record = app.recorded.find(lists[i]);
        if (record == app.recorded.end()) continue;
        submission.tracked = true;
        submission.scratch_targets |= record->second;
    }
    if (submission.tracked) ++app.submitting;
    return submission;
}

void EndUpscaleSubmission(ID3D12CommandQueue* queue, UpscaleSubmission submission) noexcept {
    auto& app = App();
    std::lock_guard lock(app.mutex);
    // A list may be reset as soon as the underlying ExecuteCommandLists returns.
    // The submission stays live until its retirement fence has been queued.
    --app.submitting;
    for (auto& q : app.queues) {
        if (q.queue.Get() != queue) continue;
        if (FAILED(queue->Signal(q.fence.Get(), ++q.last_signal))) break;
        if (submission.scratch_targets)
            for (UINT i = 0; i < q.scratch_signals.size(); ++i)
                if (submission.scratch_targets & (1u << i)) q.scratch_signals[i] = q.last_signal;
        return;
    }
    if (!app.untracked_work) Log("SDK work could not be fenced; preserving Original TAA until restart.");
    app.untracked_work = true;
}

void UpscaleCommandsDiscarded(ID3D12GraphicsCommandList* list) noexcept {
    auto& app = App();
    std::lock_guard lock(app.mutex);
    app.recorded.erase(list);
}

bool InitializeUpscalers(ID3D12Device* device) {
    auto& app = App();
    std::lock_guard lock(app.mutex);
    app.device = device;
    const auto& directory = DataDirectory();
    app.available[Slot(Technology::Dlss)] = app.dlss.Open(device, directory);
    app.available[Slot(Technology::Fsr)] = app.fsr.Open(device, directory);
    app.available[Slot(Technology::Xess)] = app.xess.Open(device, directory);
    for (size_t i = 1; i < app.available.size(); ++i) {
        if (!app.available[i]) continue;
        app.last_vendor = static_cast<Technology>(i);
        break;
    }
    if (app.last_vendor == Technology::Taa || !InitializeMotionPass(app)) {
        Log("Temporal upscaler unavailable; using Original TAA.");
        return false;
    }
    app.temporal.Discover();
    if (!app.temporal.CanScale()) {
        app.available.fill(false);
        app.last_vendor = Technology::Taa;
        Log("Engine render scale control unavailable; using Original TAA.");
        return false;
    }
    ComPtr<IDXGIFactory4> factory;
    if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))))
        factory->EnumAdapterByLuid(device->GetAdapterLuid(), IID_PPV_ARGS(&app.stats.adapter));
    const auto config = LoadGraphicsConfig(directory, app.available, app.last_vendor);
    app.active = config.settings.technology;
    app.quality = config.settings.quality;
    app.model = config.settings.model;
    app.sharpness = config.settings.sharpness;
    app.last_vendor = config.last_vendor;
    app.benchmark = BenchmarkEnabled();
    app.diagnostic_hotkeys = DiagnosticEnabled(L"Hotkeys");
    app.temporal.Benchmark(app.benchmark);
    Log("Restored selection %s / %s, Sharpness %d%%.", Names[Slot(app.active)], ModeName(app.Selection().Temporal()), app.sharpness);
    return true;
}

bool RenderUpscaled(ID3D12GraphicsCommandList* list, GameFrame& game) {
    auto& app = App();
    std::lock_guard lock(app.mutex);
    auto& frame = game.frame;
    const Size output = MainDisplay(app, frame.display, frame.render);
    if (!output.width) return false;
    if (app.diagnostic_hotkeys) UpdateToggle(app, output);
    app.temporal.Benchmark(app.benchmark);
    const auto now = Clock::now();
    const auto original = [&](bool restore) {
        app.previous = {};
        if (restore) app.temporal.Apply(Technology::Taa);
        app.stats.Record({Technology::Taa}, frame.render, output, now);
        return false;
    };
    const auto tech = app.active;
    const size_t slot = Slot(tech);
    if (tech == Technology::Taa || !app.available[slot] || app.queues.empty() || app.untracked_work) {
        app.main_view = {frame.display, output};
        ReleaseFeature(app);
        return original(true);
    }
    TemporalSelection settings{tech, app.quality, tech == Technology::Dlss ? app.model : Model::Auto};
    if (app.plan.settings != settings || app.plan.display != output) {
        auto recommendation = Recommend(app, tech, settings.quality, output);
        if (!ValidTarget(recommendation.optimal, output, settings.quality) && settings.quality != Quality::Native) {
            settings.quality = app.quality = Quality::Native;
            recommendation = Recommend(app, tech, settings.quality, output);
            SaveSelection(app);
        }
        if (!ValidTarget(recommendation.optimal, output, settings.quality)) {
            Log("%s has no SDK resolution for %ux%u; preserving Original TAA.", Names[slot], output.width, output.height);
            SetTechnology(app, Technology::Taa);
            return original(true);
        }
        const float percentage = static_cast<float>(RenderScalePercent(recommendation.optimal, output));
        app.plan = {settings, output, recommendation, percentage};
        app.waiting_frames = 0;
        app.reported_off_plan_view = false;
        Log("%s / %s SDK optimal %ux%u -> %ux%u, min %ux%u, max %ux%u.",
            Names[slot], ModeName(settings), recommendation.optimal.width, recommendation.optimal.height, output.width, output.height,
            recommendation.minimum.width, recommendation.minimum.height, recommendation.maximum.width, recommendation.maximum.height);
    }
    const auto& plan = app.plan;
    const bool matches_extent = frame.display == output && Accepts(plan, frame.render);
    // A stock pass can share the window size and still be an independent view.
    // The startup timeout qualifies a new selection only.
    if (!matches_extent && app.frames && app.feature.settings == settings && app.feature.display == output) {
        if (!app.reported_off_plan_view) {
            Log("Preserving stock view outside the active SDK plan: render %ux%u (allocated %ux%u), output %ux%u (allocated %ux%u).",
                frame.render.width, frame.render.height, game.color_allocation.width, game.color_allocation.height,
                frame.display.width, frame.display.height, game.output_allocation.width, game.output_allocation.height);
            app.reported_off_plan_view = true;
        }
        return false;
    }
    app.main_view = {frame.display, output};
    const bool changed_settings = app.temporal.Apply(tech, plan.percentage, HaltonBaseCount(plan, frame.render));
    if (!matches_extent ||
        (settings.quality != Quality::Native && (!game.jitter_phases || game.jitter_phases > 127))) {
        if (!app.waiting_frames)
            Log("Waiting for SDK viewport: actual %ux%u -> %ux%u; expected %ux%u -> %ux%u.",
                frame.render.width, frame.render.height, frame.display.width, frame.display.height,
                plan.recommendation.optimal.width, plan.recommendation.optimal.height, output.width, output.height);
        if (++app.waiting_frames == 180) {
            Log("Viewport did not reach SDK target: actual %ux%u -> %ux%u; expected %ux%u -> %ux%u. Using Original TAA.",
                frame.render.width, frame.render.height, frame.display.width, frame.display.height,
                plan.recommendation.optimal.width, plan.recommendation.optimal.height, output.width, output.height);
            SetTechnology(app, Technology::Taa);
            return original(true);
        }
        return original(false);
    }
    app.waiting_frames = 0;
    if (tech == Technology::Fsr && frame.camera_near <= 0) {
        static bool logged = false;
        if (!logged) Log("FSR needs the main view's reversed infinite perspective projection; preserving Original TAA for this view.");
        logged = true;
        return false;
    }
    const ConfiguredFeature requested{settings, frame.render, output};
    if (app.feature != requested) {
        if (!ReleaseFeature(app)) return original(false);
        app.feature = requested;
        bool ready = false;
        switch (tech) {
        case Technology::Dlss:
            app.recorded.try_emplace(list, 0);
            ready = app.dlss.Create(list, settings, frame.render, frame.display);
            break;
        case Technology::Fsr: ready = app.fsr.Create(frame.render, frame.display); break;
        case Technology::Xess: ready = app.xess.Create(settings.quality, frame.display); break;
        default: break;
        }
        if (!ready) {
            Log("%s / %s context creation failed; using Original TAA.", Names[slot], ModeName(settings));
            SetTechnology(app, Technology::Taa);
        }
        return original(!ready);
    }
    auto* motion = Pass(app, frame.render);
    D3D12_GPU_DESCRIPTOR_HANDLE bindings{};
    if (!motion || !FindBinding(app, *motion, game, bindings)) {
        SetTechnology(app, Technology::Taa);
        Log("Cannot prepare temporal inputs; using Original TAA.");
        return original(true);
    }

    const bool first = app.previous == Clock::time_point{};
    frame.frame_time_ms = first ? 1000.0f / 60.0f :
        std::chrono::duration<float, std::milli>(now - app.previous).count();
    const bool changed_projection = app.last_phases != game.jitter_phases || app.last_mip_bias != game.mip_bias;
    frame.reset |= first || changed_settings || changed_projection || frame.frame_time_ms > 200.0f;
    auto& used_targets = app.recorded.try_emplace(list, 0).first->second;
    RecordMotion(app, *motion, list, game, bindings);
    bool rendered = false;
    switch (tech) {
    case Technology::Dlss: rendered = app.dlss.Dispatch(list, frame); break;
    case Technology::Fsr: rendered = app.fsr.Dispatch(list, frame); break;
    case Technology::Xess: rendered = app.xess.Dispatch(list, frame); break;
    default: break;
    }
    if (!rendered) {
        app.available[slot] = false;
        SetTechnology(app, Technology::Taa);
        Log("%s failed; using Original TAA.", Names[slot]);
        return original(true);
    }
    const bool changed_sharpness = app.last_frame_sharpness != app.sharpness;
    if (changed_sharpness) {
        app.reported_sharpness_failures = 0;
        app.reported_sharpness_dispatch = false;
    }
    bool sharpened = false;
    if (app.sharpness > 0) {
        const auto record = [&](uint32_t reusable) {
            return app.sharpness_pass.Record(app.device.Get(), list, frame.output, game.output_format,
                                              frame.display, frame.pre_exposure, app.sharpness, reusable);
        };
        auto result = record(0);
        if (result.status == SharpnessPass::Status::TargetsBusy) {
            if (const auto reusable = ReusableScratchTargets(app) & sharpness_mask) result = record(reusable);
        }
        if (result.Succeeded()) {
            used_targets |= result.target;
            sharpened = true;
            if (!app.reported_sharpness_dispatch) {
                Log("CAS recorded at %d%% on %ux%u, scene pre-exposure %.4f.",
                    app.sharpness, frame.display.width, frame.display.height, frame.pre_exposure);
                app.reported_sharpness_dispatch = true;
            }
        } else {
            const uint32_t reason = 1u << static_cast<unsigned>(result.status);
            if (!(app.reported_sharpness_failures & reason)) {
                Log("CAS skipped (%s), %ux%u format %u. Using the reconstructed image.",
                    SharpnessStatusReason(result.status), frame.display.width, frame.display.height,
                    static_cast<unsigned>(game.output_format));
                app.reported_sharpness_failures |= reason;
            }
        }
    }

    app.previous = now;
    app.last_phases = game.jitter_phases;
    app.last_mip_bias = game.mip_bias;
    app.stats.Record({settings.technology, settings.quality, settings.model, app.sharpness}, frame.render, output, now, sharpened);
    app.last_frame_sharpness = app.sharpness;
    if (++app.frames == 1 || changed_projection || changed_sharpness || app.frames % 1800 == 0)
        Log("%s / %s evaluated %u frames, render %ux%u (allocated %ux%u), output %ux%u (allocated %ux%u), jitter %+.3f %+.3f, phase %u/%u, mip bias %.3f, reset %d, Sharpness %d%%.",
            Names[slot], ModeName(settings), app.frames, frame.render.width, frame.render.height,
            game.color_allocation.width, game.color_allocation.height, output.width, output.height,
            game.output_allocation.width, game.output_allocation.height,
            frame.jitter_x, frame.jitter_y, game.jitter_index, game.jitter_phases, game.mip_bias, frame.reset, app.sharpness);
    return true;
}
}
