#include "platform.hpp"
#include "ngp.hpp"
#include "vendor_backends.hpp"
#include <dxgi1_6.h>
#include <wrl/client.h>

#include <array>
#include <cstdio>
#include <cwchar>
#include <cstring>
#include <stdexcept>
#include <string_view>

using Microsoft::WRL::ComPtr;
using namespace reframe;

namespace {
constexpr Size size{1920,1080};
void Check(HRESULT result, const char* where) {
    if (FAILED(result)) {
        std::fprintf(stderr, "%s failed (0x%08lX).\n", where, static_cast<unsigned long>(result));
        throw std::runtime_error(where);
    }
}

ComPtr<ID3D12Resource> Resource(ID3D12Device* device, const D3D12_RESOURCE_DESC& desc,
                                D3D12_HEAP_TYPE type, D3D12_RESOURCE_STATES state) {
    D3D12_HEAP_PROPERTIES heap{};
    heap.Type = type;
    heap.CreationNodeMask = heap.VisibleNodeMask = 1;
    ComPtr<ID3D12Resource> resource;
    Check(device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc, state, nullptr,
                                          IID_PPV_ARGS(&resource)), "CreateCommittedResource");
    return resource;
}

void Transition(ID3D12GraphicsCommandList* list, ID3D12Resource* resource,
                D3D12_RESOURCE_STATES from, D3D12_RESOURCE_STATES to) {
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition = {resource, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, from, to};
    list->ResourceBarrier(1, &barrier);
}
}

int wmain(int argc, wchar_t** argv) {
    if (argc < 3 || argc > 5) return 2;
    const auto name = std::wstring_view(argv[1]);
    if (name != L"dlss" && name != L"fsr" && name != L"xess") return 2;
    wchar_t* end = nullptr;
    const auto index = argc >= 4 ? std::wcstoul(argv[3], &end, 10) : 0;
    if ((end && (end == argv[3] || *end)) || index >= static_cast<unsigned>(Quality::Count)) return 2;
    const auto quality = static_cast<Quality>(index);
    const auto model_index = argc == 5 ? std::wcstoul(argv[4], &end, 10) : 0;
    if (argc == 5 && (name != L"dlss" || end == argv[4] || *end || model_index > static_cast<unsigned>(Model::L)))
        return 2;
    try {
        ComPtr<IDXGIFactory6> factory;
        Check(CreateDXGIFactory1(IID_PPV_ARGS(&factory)), "CreateDXGIFactory1");
        ComPtr<IDXGIAdapter1> adapter;
        Check(factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)),
              "EnumAdapterByGpuPreference");
        DXGI_ADAPTER_DESC1 adapter_description{};
        Check(adapter->GetDesc1(&adapter_description), "GetDesc1");
        std::wprintf(L"%ls on %ls (driver adapter vendor 0x%04x).\n", argv[1], adapter_description.Description,
                    adapter_description.VendorId);

        ComPtr<ID3D12Device> dev;
        using CreateDevice = decltype(&D3D12CreateDevice);
        const auto create_device = reinterpret_cast<CreateDevice>(SystemD3D12("D3D12CreateDevice"));
        if (!create_device) return 3;
        Check(create_device(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&dev)), "D3D12CreateDevice");

        const auto directory = std::filesystem::absolute(argv[2]);
        DlssSession dlss;
        FsrApi fsr;
        XessApi xess;
        RenderRecommendation recommendation;
        if (name == L"dlss" && dlss.Open(dev.Get(), directory)) recommendation = dlss.Recommend(quality, size);
        if (name == L"fsr" && fsr.Open(dev.Get(), directory)) recommendation = fsr.Recommend(quality, size);
        if (name == L"xess" && xess.Open(dev.Get(), directory)) recommendation = xess.Recommend(quality, size);
        const auto input = recommendation.optimal;
        if (!input.width || !input.height) throw std::runtime_error("SDK did not recommend a render size");
        std::wprintf(L"Quality %lu: SDK input %ux%u, output %ux%u.\n", index, input.width, input.height, size.width, size.height);

        D3D12_COMMAND_QUEUE_DESC queue_desc{};
        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        ComPtr<ID3D12CommandQueue> queue;
        Check(dev->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&queue)), "CreateCommandQueue");
        ComPtr<ID3D12CommandAllocator> allocator;
        Check(dev->CreateCommandAllocator(queue_desc.Type, IID_PPV_ARGS(&allocator)), "CreateCommandAllocator");
        ComPtr<ID3D12GraphicsCommandList> list;
        Check(dev->CreateCommandList(0, queue_desc.Type, allocator.Get(), nullptr, IID_PPV_ARGS(&list)), "CreateCommandList");

        constexpr std::array<DXGI_FORMAT, 5> formats{DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R32_FLOAT,
            DXGI_FORMAT_R16G16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R32_FLOAT};
        constexpr float clear[5][4]{{0.5f,0.25f,0.125f,1},{0.5f,0,0,0},{0,0,0,0},{0,0,0,0},{1,0,0,0}};
        std::array<ComPtr<ID3D12Resource>, 5> textures;
        D3D12_DESCRIPTOR_HEAP_DESC heap_desc{};
        heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        heap_desc.NumDescriptors = static_cast<UINT>(textures.size());
        heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        ComPtr<ID3D12DescriptorHeap> gpu_heap, cpu_heap;
        Check(dev->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&gpu_heap)), "CreateDescriptorHeap(gpu)");
        heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        Check(dev->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&cpu_heap)), "CreateDescriptorHeap(cpu)");
        ID3D12DescriptorHeap* heaps[]{gpu_heap.Get()};
        list->SetDescriptorHeaps(1, heaps);
        auto cpu = cpu_heap->GetCPUDescriptorHandleForHeapStart();
        auto cpu_gpu = gpu_heap->GetCPUDescriptorHandleForHeapStart();
        auto gpu = gpu_heap->GetGPUDescriptorHandleForHeapStart();
        const auto step = dev->GetDescriptorHandleIncrementSize(heap_desc.Type);
        for (size_t i = 0; i < textures.size(); ++i) {
            D3D12_RESOURCE_DESC desc{};
            desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            const Size extent = i < 3 ? input : i == 3 ? size : Size{1,1};
            desc.Width = extent.width;
            desc.Height = extent.height;
            desc.DepthOrArraySize = desc.MipLevels = 1;
            desc.SampleDesc.Count = 1;
            desc.Format = formats[i];
            desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
            textures[i] = Resource(dev.Get(), desc, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
            dev->CreateUnorderedAccessView(textures[i].Get(), nullptr, nullptr, cpu);
            dev->CopyDescriptorsSimple(1, cpu_gpu, cpu, heap_desc.Type);
            list->ClearUnorderedAccessViewFloat(gpu, cpu, textures[i].Get(), clear[i], 0, nullptr);
            if (i != 3) Transition(list.Get(), textures[i].Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
                                   D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
            cpu.ptr += step;
            cpu_gpu.ptr += step;
            gpu.ptr += step;
        }

        Frame frame{textures[0].Get(), textures[1].Get(), textures[2].Get(), textures[3].Get(), textures[4].Get(),
                    input, size, -0.25f, 0.25f, 1.0f, 16.6f, true, 10.0f, 1.0f};
        bool ok = false;
        if (name == L"dlss") {
            ok = dlss.Create(list.Get(), {Technology::Dlss, quality, static_cast<Model>(model_index)}, input, size) &&
                dlss.Dispatch(list.Get(), frame);
        }
        if (name == L"fsr") ok = fsr.Create(input, size) && fsr.Dispatch(list.Get(), frame);
        if (name == L"xess") ok = xess.Create(quality, size) && xess.Dispatch(list.Get(), frame);
        if (!ok) throw std::runtime_error("SDK initialization or recording failed");

        const auto output = textures[3]->GetDesc();
        D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
        UINT64 bytes = 0;
        dev->GetCopyableFootprints(&output, 0, 1, 0, &footprint, nullptr, nullptr, &bytes);
        D3D12_RESOURCE_DESC readback_desc{};
        readback_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        readback_desc.Width = bytes;
        readback_desc.Height = readback_desc.DepthOrArraySize = readback_desc.MipLevels = 1;
        readback_desc.SampleDesc.Count = 1;
        readback_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        auto readback = Resource(dev.Get(), readback_desc, D3D12_HEAP_TYPE_READBACK, D3D12_RESOURCE_STATE_COPY_DEST);
        Transition(list.Get(), textures[3].Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
        D3D12_TEXTURE_COPY_LOCATION to{readback.Get(), D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT};
        to.PlacedFootprint = footprint;
        D3D12_TEXTURE_COPY_LOCATION from{textures[3].Get(), D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX};
        list->CopyTextureRegion(&to, 0, 0, 0, &from, nullptr);
        Check(list->Close(), "Close");
        ComPtr<ID3D12Fence> fence;
        Check(dev->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)), "CreateFence");
        ID3D12CommandList* command_lists[]{list.Get()};
        queue->ExecuteCommandLists(1, command_lists);
        Check(queue->Signal(fence.Get(), 1), "Signal");
        Check(fence->SetEventOnCompletion(1, nullptr), "Wait for GPU");
        Check(dev->GetDeviceRemovedReason(), "GPU health");

        const auto pixel_at = footprint.Footprint.RowPitch * (size.height / 2) + 8 * (size.width / 2);
        const D3D12_RANGE range{pixel_at, pixel_at + 8};
        void* raw = nullptr;
        Check(readback->Map(0, &range, &raw), "Map");
        uint16_t rgba[4]{};
        std::memcpy(rgba, static_cast<const uint8_t*>(raw) + pixel_at, sizeof(rgba));
        const D3D12_RANGE written{0,0};
        readback->Unmap(0, &written);
        std::wprintf(L"Output center FP16: %04x %04x %04x %04x\n", rgba[0], rgba[1], rgba[2], rgba[3]);
        // The RGB inputs are 0.5, 0.25, 0.125. A zero-cleared or badly exposed output fails.
        return rgba[0] >= 0x36cc && rgba[0] <= 0x38cc && rgba[1] >= 0x32cc && rgba[1] <= 0x34cc &&
               rgba[2] >= 0x2ecc && rgba[2] <= 0x30cc ? 0 : 4;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 3;
    }
}
