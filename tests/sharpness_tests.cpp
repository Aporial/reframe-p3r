#include "platform.hpp"
#include "settings.hpp"
#include "menu_text.hpp"
#include "sharpness.hpp"

#include <DirectXPackedVector.h>
#include <d3d12sdklayers.h>
#include <dxgi1_6.h>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <stdexcept>
#include <vector>

using Microsoft::WRL::ComPtr;
using DirectX::PackedVector::XMConvertFloatToHalf;
using DirectX::PackedVector::XMConvertHalfToFloat;
using namespace reframe;

namespace {
constexpr auto format = DXGI_FORMAT_R16G16B16A16_FLOAT;
constexpr auto uav = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
constexpr auto copy_dest = D3D12_RESOURCE_STATE_COPY_DEST;
constexpr auto copy_source = D3D12_RESOURCE_STATE_COPY_SOURCE;
using CasStatus = SharpnessPass::Status;

void Require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

void Check(HRESULT result, const char* message) {
    if (SUCCEEDED(result)) return;
    std::fprintf(stderr, "%s: HRESULT 0x%08lX\n", message, static_cast<unsigned long>(result));
    throw std::runtime_error(message);
}

void TestConfig() {
    static_assert(Settings{}.sharpness == 0);
    static_assert(NormalizeSharpness(-100) == 0 && NormalizeSharpness(1000) == 100);
    static_assert(NormalizeSharpness(2) == 0 && NormalizeSharpness(3) == 5);
    static_assert(CasPeak(0) == 0 && CasPeak(5) < 0 && CasPeak(100) == -0.2f);
    static_assert(CompleteMenuTranslations());
    Require(FormatSharpnessValue(0, LocalizedMenu(L"en-US")) == L"0%" &&
            FormatSharpnessValue(50, LocalizedMenu(L"fr-FR")) == L"50 %" &&
            FormatSharpnessValue(100, LocalizedMenu(L"tr-TR")) == L"%100", "Sharpness percent localization");
    for (const auto backend : {Technology::Taa, Technology::Dlss, Technology::Fsr, Technology::Xess}) {
        const Settings original{backend, Quality::Native, Model::Auto, 0};
        const Settings sharpened{backend, Quality::Native, Model::Auto, 100};
        Require(original != sharpened && original.Temporal() == sharpened.Temporal(), "Sharpness entered temporal identity");
    }

    struct TempDirectory {
        std::filesystem::path path = std::filesystem::temp_directory_path() /
            (L"reframe-sharpness-" + std::to_wstring(GetCurrentProcessId()) + L"-" + std::to_wstring(GetTickCount64()));
        ~TempDirectory() {
            std::error_code error;
            std::filesystem::remove_all(path, error);
        }
    } temp;
    const auto directory = temp.path / L"ReFrame";
    const auto legacy = temp.path / L"P3RGraphicsEnhanced" / L"Settings.ini";
    const auto current = directory / L"Settings.ini";
    std::filesystem::create_directories(legacy.parent_path());
    std::filesystem::create_directory(directory);
    constexpr std::array available{true, true, true, true};
    const auto load = [&] { return LoadGraphicsConfig(directory, available, Technology::Dlss); };
    const auto write = [](const auto& file, const wchar_t* key, const wchar_t* value) {
        Require(WritePrivateProfileStringW(L"Graphics", key, value, file.c_str()) != 0, "Writing fixture INI");
    };
    Require(load().settings == Settings{}, "Missing INI defaults");
    write(legacy, L"Technology", L"2");
    write(legacy, L"Quality", L"1");
    Require(load().settings == Settings{Technology::Fsr, Quality::Quality, Model::Auto, 0}, "Legacy INI migration");
    write(current, L"Technology", L"1");
    Require(load().settings == Settings{}, "Current INI without Sharpness");
    for (const auto& [raw, expected] : {std::pair{L"-5", 0}, {L"98", 100}, {L"23", 25}, {L"x25", 0},
                                        {L"25oops", 0}, {L"999999999999", 100}, {L"0", 0}}) {
        write(current, L"Sharpness", raw);
        Require(load().settings.sharpness == expected, "Normalizing INI Sharpness");
    }
    for (const auto& settings : {Settings{Technology::Dlss, Quality::Balanced, Model::M, 75},
                                 Settings{Technology::Fsr, Quality::Native, Model::M, 75},
                                 Settings{Technology::Xess, Quality::Quality, Model::K, 75},
                                 Settings{Technology::Taa, Quality::Quality, Model::K, 75}}) {
        const auto last = settings.technology == Technology::Taa ? Technology::Xess : settings.technology;
        Require(SaveGraphicsConfig(directory, settings, last), "Saving graphics config");
        const auto config = load();
        Require(config.settings == settings && config.last_vendor == last, "Round trip after a mode/backend change");
    }
    SharpnessPass disabled;
    Require(disabled.Record(nullptr, nullptr, nullptr, DXGI_FORMAT_UNKNOWN, {}, 0, 0).status == CasStatus::Disabled &&
            disabled.AllocatedBytes() == 0, "Zero sharpness must exit before touching D3D12");
    std::puts("Config, settings identity, localization and zero bypass passed.");
}

struct Pixel { uint16_t r, g, b, a; };
static_assert(sizeof(Pixel) == 8);

D3D12_RESOURCE_DESC BufferDesc(UINT64 size) {
    D3D12_RESOURCE_DESC desc{};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Width = size;
    desc.Height = desc.DepthOrArraySize = desc.MipLevels = 1;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    return desc;
}

ComPtr<ID3D12Resource> CreateResource(ID3D12Device* device, const D3D12_RESOURCE_DESC& desc,
                                      D3D12_HEAP_TYPE type, D3D12_RESOURCE_STATES state) {
    D3D12_HEAP_PROPERTIES heap{};
    heap.Type = type;
    heap.CreationNodeMask = heap.VisibleNodeMask = 1;
    ComPtr<ID3D12Resource> resource;
    Check(device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc, state, nullptr,
                                           IID_PPV_ARGS(&resource)), "CreateCommittedResource");
    return resource;
}

void Transition(ID3D12GraphicsCommandList* list, ID3D12Resource* resource, D3D12_RESOURCE_STATES from,
                D3D12_RESOURCE_STATES to) {
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition = {resource, 0, from, to};
    list->ResourceBarrier(1, &barrier);
}

class GpuTest {
public:
    explicit GpuTest(bool warp);
    void Run(Size display, bool measure, DXGI_FORMAT storage = format, bool recycle = false);

private:
    ComPtr<IDXGIAdapter1> adapter;
    ComPtr<ID3D12Device> device;
    ComPtr<ID3D12CommandQueue> queue;
    ComPtr<ID3D12CommandAllocator> allocator;
    ComPtr<ID3D12GraphicsCommandList> list;
    ComPtr<ID3D12Fence> fence;
    SharpnessPass pass;
    UINT64 signal = 0;
};

GpuTest::GpuTest(bool warp) {
    using Debug = decltype(&D3D12GetDebugInterface);
    const auto get_debug = reinterpret_cast<Debug>(SystemD3D12("D3D12GetDebugInterface"));
    ComPtr<ID3D12Debug> debug;
    if (get_debug && SUCCEEDED(get_debug(IID_PPV_ARGS(&debug)))) debug->EnableDebugLayer();
    ComPtr<IDXGIFactory6> factory;
    Check(CreateDXGIFactory1(IID_PPV_ARGS(&factory)), "CreateDXGIFactory1");
    if (warp) Check(factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)), "EnumWarpAdapter");
    else Check(factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)),
               "EnumAdapterByGpuPreference");
    DXGI_ADAPTER_DESC1 desc{};
    Check(adapter->GetDesc1(&desc), "GetDesc1");
    std::printf("Adapter vendor 0x%04X, D3D12 debug layer %s.\n", desc.VendorId, debug ? "on" : "unavailable");
    using CreateDevice = decltype(&D3D12CreateDevice);
    const auto create = reinterpret_cast<CreateDevice>(SystemD3D12("D3D12CreateDevice"));
    Require(create != nullptr, "D3D12CreateDevice export");
    Check(create(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)), "D3D12CreateDevice");
    D3D12_COMMAND_QUEUE_DESC spec{};
    spec.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    Check(device->CreateCommandQueue(&spec, IID_PPV_ARGS(&queue)), "CreateCommandQueue");
    Check(device->CreateCommandAllocator(spec.Type, IID_PPV_ARGS(&allocator)), "CreateCommandAllocator");
    Check(device->CreateCommandList(0, spec.Type, allocator.Get(), nullptr, IID_PPV_ARGS(&list)), "CreateCommandList");
    Check(list->Close(), "Close command list");
    Check(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)), "CreateFence");
}

void GpuTest::Run(Size display, bool measure, DXGI_FORMAT storage_format, bool recycle) {
    D3D12_RESOURCE_DESC texture{};
    texture.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texture.Width = display.width + 11;
    texture.Height = display.height + 3;
    texture.DepthOrArraySize = texture.MipLevels = 1;
    texture.Format = storage_format;
    texture.SampleDesc.Count = 1;
    texture.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    auto output = CreateResource(device.Get(), texture, D3D12_HEAP_TYPE_DEFAULT, copy_dest);
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout{};
    UINT64 bytes = 0;
    device->GetCopyableFootprints(&texture, 0, 1, 0, &layout, nullptr, nullptr, &bytes);
    const auto buffer = BufferDesc(bytes);
    auto upload = CreateResource(device.Get(), buffer, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
    auto readback = CreateResource(device.Get(), buffer, D3D12_HEAP_TYPE_READBACK, copy_dest);
    auto timestamps = CreateResource(device.Get(), BufferDesc(2 * sizeof(UINT64)), D3D12_HEAP_TYPE_READBACK, copy_dest);
    const D3D12_QUERY_HEAP_DESC query{D3D12_QUERY_HEAP_TYPE_TIMESTAMP, 2, 0};
    ComPtr<ID3D12QueryHeap> query_heap;
    Check(device->CreateQueryHeap(&query, IID_PPV_ARGS(&query_heap)), "CreateQueryHeap");
    UINT64 frequency = 0;
    Check(queue->GetTimestampFrequency(&frequency), "GetTimestampFrequency");

    const auto middle = display.width / 2;
    const auto pattern = [&](UINT x, UINT y) {
        if (x >= display.width || y >= display.height)
            return Pixel{XMConvertFloatToHalf(30.0f), 0, 0, XMConvertFloatToHalf(0.25f)};
        const float luma = (x < middle ? 0.4f : x == middle ? 0.55f : 0.8f) * (y < display.height / 2 ? 1.0f : 8.0f);
        return Pixel{XMConvertFloatToHalf(luma), XMConvertFloatToHalf(luma * 0.8f),
                     XMConvertFloatToHalf(luma * 0.6f), XMConvertFloatToHalf((x % 8) / 8.0f)};
    };
    const auto offset = [&](UINT x, UINT y) { return y * size_t(layout.Footprint.RowPitch) + x * sizeof(Pixel); };
    void* mapped = nullptr;
    const D3D12_RANGE empty{0, 0};
    Check(upload->Map(0, &empty, &mapped), "Map upload");
    for (UINT y = 0; y < texture.Height; ++y)
        for (UINT x = 0; x < texture.Width; ++x)
            *reinterpret_cast<Pixel*>(static_cast<std::byte*>(mapped) + offset(x, y)) = pattern(x, y);
    upload->Unmap(0, nullptr);

    D3D12_TEXTURE_COPY_LOCATION source{upload.Get(), D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT};
    source.PlacedFootprint = layout;
    D3D12_TEXTURE_COPY_LOCATION scene{output.Get(), D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX};
    D3D12_TEXTURE_COPY_LOCATION target{readback.Get(), D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT};
    target.PlacedFootprint = layout;

    const auto draw = [&](int percent, CasStatus expected, uint32_t reusable = 0) {
        Check(allocator->Reset(), "Reset allocator");
        Check(list->Reset(allocator.Get(), nullptr), "Reset command list");
        list->CopyTextureRegion(&scene, 0, 0, 0, &source, nullptr);
        Transition(list.Get(), output.Get(), copy_dest, uav);
        list->EndQuery(query_heap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 0);
        const auto start = std::chrono::steady_clock::now();
        const auto view_format = expected == CasStatus::Unsupported ? DXGI_FORMAT_R32_FLOAT : format;
        const auto result = pass.Record(device.Get(), list.Get(), output.Get(), view_format, display, 1.0f,
                                        percent, reusable);
        const double cpu = std::chrono::duration<double, std::micro>(std::chrono::steady_clock::now() - start).count();
        Require(result.status == expected && (result.target != 0) == (expected == CasStatus::Recorded),
                "Unexpected sharpening decision");
        if (reusable) Require((result.target & reusable) != 0, "Rebound a target that was not marked safe");
        list->EndQuery(query_heap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 1);
        list->ResolveQueryData(query_heap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 0, 2, timestamps.Get(), 0);
        Transition(list.Get(), output.Get(), uav, copy_source);
        list->CopyTextureRegion(&target, 0, 0, 0, &scene, nullptr);
        Transition(list.Get(), output.Get(), copy_source, copy_dest);
        Check(list->Close(), "Close command list");
        ID3D12CommandList* lists[]{list.Get()};
        queue->ExecuteCommandLists(1, lists);
        Check(queue->Signal(fence.Get(), ++signal), "Signal fence");
        Check(fence->SetEventOnCompletion(signal, nullptr), "Wait for fence");
        Check(device->GetDeviceRemovedReason(), "D3D12 device");
        const D3D12_RANGE timestamp_range{0, 2 * sizeof(UINT64)};
        Check(timestamps->Map(0, &timestamp_range, &mapped), "Map timestamps");
        const auto* ticks = static_cast<const UINT64*>(mapped);
        const double gpu = 1000.0 * (ticks[1] - ticks[0]) / frequency;
        timestamps->Unmap(0, &empty);
        return std::pair{cpu, gpu};
    };
    const auto inspect = [&](int percent) {
        const D3D12_RANGE range{0, static_cast<SIZE_T>(bytes)};
        Check(readback->Map(0, &range, &mapped), "Map scene readback");
        const auto pixel = [&](UINT x, UINT y) {
            return *reinterpret_cast<const Pixel*>(static_cast<const std::byte*>(mapped) + offset(x, y));
        };
        for (const auto& [x, y] : {std::pair{0u, 0u}, {middle, 1u}, {middle, display.height - 1},
                                  {display.width - 1, display.height - 1}, {display.width, 0u}, {0u, display.height}}) {
            const auto src = pattern(x, y);
            const auto dst = pixel(x, y);
            Require(src.a == dst.a, "Alpha was modified");
            if (!percent || x != middle) {
                if (src.r != dst.r || src.g != dst.g || src.b != dst.b)
                    std::fprintf(stderr, "%ux%u, %d%%, pixel (%u,%u): RGB %04x/%04x/%04x -> %04x/%04x/%04x\n",
                                 display.width, display.height, percent, x, y, src.r, src.g, src.b, dst.r, dst.g, dst.b);
                Require(src.r == dst.r && src.g == dst.g && src.b == dst.b, "Flat region, zero/fallback, or padding was modified");
            }
        }
        const auto detail = pixel(middle, 1);
        const auto hdr = pixel(middle, display.height - 1);
        const auto red = XMConvertHalfToFloat(detail.r);
        const auto green = XMConvertHalfToFloat(detail.g);
        Require(std::abs(green / red - 0.8f) < 0.002f, "Sharpening shifted hue");
        Require(XMConvertHalfToFloat(hdr.r) > 1.0f, "HDR signal was clipped");
        Require(red >= XMConvertHalfToFloat(pattern(middle - 1, 1).r), "Sharpening overshot local extrema");
        readback->Unmap(0, &empty);
        return red;
    };

    draw(0, CasStatus::Disabled);
    float previous = inspect(0);
    const float base = previous;
    const auto cached_bytes = pass.AllocatedBytes();
    draw(50, CasStatus::Unsupported);
    inspect(0);
    if (recycle) {
        draw(50, CasStatus::TargetsBusy);
        inspect(0);
    }
    Require(pass.AllocatedBytes() == cached_bytes, "Bypass or rejected format allocated scratch storage");
    for (int percent : {25, 50, 75, 100}) {
        draw(percent, CasStatus::Recorded, recycle && percent == 25 ? 1u << 1 : 0);
        const auto current = inspect(percent);
        Require(current < previous, "Sharpness did not increase monotonically");
        previous = current;
    }
    Require(base - previous >= 0.035f, "Maximum strength barely affected the synthetic edge");
    draw(0, CasStatus::Disabled);
    inspect(0);
    const auto allocated = pass.AllocatedBytes();
    if (recycle) Require(allocated == cached_bytes, "Rebinding allocated a new scratch texture");
    else Require(allocated > cached_bytes, "No scratch allocation for a new output");
    double cpu_us = 0, gpu_ms = 0;
    if (measure) {
        constexpr int samples = 24;
        for (int i = 0; i < samples; ++i) {
            const auto [cpu, gpu] = draw(50, CasStatus::Recorded);
            cpu_us += cpu;
            gpu_ms += gpu;
        }
        std::printf("%ux%u: CAS 50%%, %.3f ms GPU, %.1f us CPU Record, %.2f MiB scratch, 1 GPU copy (%d samples).\n",
                    display.width, display.height, gpu_ms / samples, cpu_us / samples,
                    (allocated - cached_bytes) / 1048576.0, samples);
    }
    Require(pass.AllocatedBytes() == allocated, "Repeated dispatch allocated additional scratch storage");

    // Reset removes all descriptor references from the recorded command list.
    Check(allocator->Reset(), "Retire allocator");
    Check(list->Reset(allocator.Get(), nullptr), "Retire command list");
    Check(list->Close(), "Close retired command list");
    if (measure || recycle) {
        pass.ClearTargets();
        Require(pass.AllocatedBytes() == 0, "Fence-retired storage was not released");
    }
    ComPtr<ID3D12InfoQueue> messages;
    if (SUCCEEDED(device.As(&messages))) {
        for (UINT64 i = 0; i < messages->GetNumStoredMessages(); ++i) {
            SIZE_T size = 0;
            messages->GetMessage(i, nullptr, &size);
            std::vector<std::byte> storage(size);
            auto* message = reinterpret_cast<D3D12_MESSAGE*>(storage.data());
            Check(messages->GetMessage(i, message, &size), "Read D3D12 debug message");
            if (message->Severity <= D3D12_MESSAGE_SEVERITY_ERROR) throw std::runtime_error(message->pDescription);
        }
        messages->ClearStoredMessages();
    }
}
}

int main(int argc, char** argv) {
    try {
        TestConfig();
        if (argc == 1) return 0;
        const auto mode = std::string_view(argv[1]);
        if (argc != 2 || (mode != "--warp" && mode != "--hardware")) return 2;
        GpuTest test(mode == "--warp");
        for (UINT slot = 0; slot < SharpnessPass::target_count; ++slot)
            test.Run({129, 73}, false, slot == 0 ? DXGI_FORMAT_R16G16B16A16_TYPELESS : format);
        test.Run({129, 73}, false, format, true);
        for (const Size size : {Size{1280, 720}, {1920, 1080}, {2560, 1440}, {3840, 2160}, {3440, 1440}})
            test.Run(size, true);
        std::puts("D3D12 shader, padded outputs, alpha, HDR, fallback, resize and resource reuse passed.");
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
