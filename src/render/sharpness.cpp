#include "platform.hpp"
#include "sharpness.hpp"
#include "sharpness_cs.h"

#include <algorithm>
#include <cmath>
#include <limits>

using Microsoft::WRL::ComPtr;

namespace reframe {
namespace {
constexpr UINT descriptors_per_target = 2;
constexpr D3D12_RESOURCE_STATES uav = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

float ChannelLimit(DXGI_FORMAT storage, DXGI_FORMAT view) {
    switch (view) {
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
        return storage == view || storage == DXGI_FORMAT_R16G16B16A16_TYPELESS ? 65504.0f : 0.0f;
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
        return storage == view || storage == DXGI_FORMAT_R32G32B32A32_TYPELESS
            ? std::numeric_limits<float>::max() : 0.0f;
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        return storage == view || storage == DXGI_FORMAT_R8G8B8A8_TYPELESS ? 1.0f : 0.0f;
    default:
        return 0.0f;
    }
}

D3D12_RESOURCE_BARRIER Transition(ID3D12Resource* resource, D3D12_RESOURCE_STATES before,
                                  D3D12_RESOURCE_STATES after) {
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition = {resource, 0, before, after};
    return barrier;
}

struct Constants {
    UINT width, height;
    float peak, pre_exposure, channel_limit;
};
static_assert(sizeof(Constants) == 5 * sizeof(UINT));
}

bool SharpnessPass::Initialize(ID3D12Device* device) noexcept {
    const D3D12_DESCRIPTOR_RANGE ranges[]{
        {D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, 0},
        {D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, 1}
    };
    D3D12_ROOT_PARAMETER parameters[2]{};
    parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    parameters[0].Constants = {0, 0, 5};
    parameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    parameters[1].DescriptorTable = {static_cast<UINT>(std::size(ranges)), ranges};
    const D3D12_ROOT_SIGNATURE_DESC root{static_cast<UINT>(std::size(parameters)), parameters, 0, nullptr,
                                        D3D12_ROOT_SIGNATURE_FLAG_NONE};
    using Serialize = decltype(&D3D12SerializeRootSignature);
    const auto serialize = reinterpret_cast<Serialize>(SystemD3D12("D3D12SerializeRootSignature"));
    ComPtr<ID3DBlob> bytes;
    if (!serialize || FAILED(serialize(&root, D3D_ROOT_SIGNATURE_VERSION_1_0, &bytes, nullptr)) ||
        FAILED(device->CreateRootSignature(0, bytes->GetBufferPointer(), bytes->GetBufferSize(),
                                           IID_PPV_ARGS(&root_)))) return false;
    D3D12_COMPUTE_PIPELINE_STATE_DESC pipeline{};
    pipeline.pRootSignature = root_.Get();
    pipeline.CS = {SharpnessCs, sizeof(SharpnessCs)};
    if (FAILED(device->CreateComputePipelineState(&pipeline, IID_PPV_ARGS(&pipeline_)))) return false;
    const D3D12_DESCRIPTOR_HEAP_DESC heap{D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        static_cast<UINT>(targets_.size()) * descriptors_per_target, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0};
    if (FAILED(device->CreateDescriptorHeap(&heap, IID_PPV_ARGS(&heap_)))) return false;
    stride_ = device->GetDescriptorHandleIncrementSize(heap.Type);
    return stride_ != 0;
}

bool SharpnessPass::BindTarget(ID3D12Device* device, ID3D12Resource* output, DXGI_FORMAT format,
                               Size display, Target& target, UINT index) noexcept {
    auto cpu = heap_->GetCPUDescriptorHandleForHeapStart();
    cpu.ptr += index * descriptors_per_target * stride_;
    if (!target.scratch || target.format != format || target.display != display) {
        D3D12_FEATURE_DATA_FORMAT_SUPPORT support{format};
        if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &support, sizeof(support))) ||
            !(support.Support1 & D3D12_FORMAT_SUPPORT1_SHADER_LOAD) ||
            !(support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE)) return false;

        D3D12_HEAP_PROPERTIES heap{};
        heap.Type = D3D12_HEAP_TYPE_DEFAULT;
        heap.CreationNodeMask = heap.VisibleNodeMask = 1;
        D3D12_RESOURCE_DESC desc{};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Width = display.width;
        desc.Height = display.height;
        desc.MipLevels = desc.DepthOrArraySize = 1;
        desc.Format = format;
        desc.SampleDesc.Count = 1;
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        ComPtr<ID3D12Resource> scratch;
        if (FAILED(device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc, uav, nullptr,
                                                   IID_PPV_ARGS(&scratch)))) return false;
        scratch->SetName(L"RE:FRAME CAS scratch");
        const auto bytes = device->GetResourceAllocationInfo(0, 1, &desc).SizeInBytes;
        allocated_bytes_ = allocated_bytes_ - target.bytes + bytes;
        target.bytes = bytes;
        target.scratch = std::move(scratch);
        const D3D12_CPU_DESCRIPTOR_HANDLE uav_handle{cpu.ptr + stride_};
        device->CreateUnorderedAccessView(target.scratch.Get(), nullptr, nullptr, uav_handle);
    }
    D3D12_SHADER_RESOURCE_VIEW_DESC source{};
    source.Format = format;
    source.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    source.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    source.Texture2D.MipLevels = 1;
    device->CreateShaderResourceView(output, &source, cpu);
    target.output = output;
    target.format = format;
    target.display = display;
    return true;
}

SharpnessPass::Result SharpnessPass::Record(ID3D12Device* device, ID3D12GraphicsCommandList* list,
                                           ID3D12Resource* output, DXGI_FORMAT format, Size display,
                                           float pre_exposure, int percent, uint32_t reusable) noexcept {
    if (percent <= 0) return {Status::Disabled};
    if (!output || !display.width || !display.height || !std::isfinite(pre_exposure) || pre_exposure <= 0)
        return {Status::Unsupported};
    const auto desc = output->GetDesc();
    const float channel_limit = ChannelLimit(desc.Format, format);
    if (!channel_limit || desc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D ||
        desc.DepthOrArraySize != 1 || desc.SampleDesc.Count != 1 || desc.Width < display.width ||
        desc.Height < display.height || (desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) ||
        !(desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)) return {Status::Unsupported};
    if (initialization_failed_) return {Status::PipelineFailed};
    if (!heap_ && !Initialize(device)) {
        initialization_failed_ = true;
        return {Status::PipelineFailed};
    }

    UINT index = 0;
    for (; index < target_count; ++index) {
        const auto& target = targets_[index];
        if (target.output.Get() == output && target.format == format && target.display == display)
            break;
    }
    if (index == target_count) {
        for (UINT candidate = 0; candidate < target_count; ++candidate) {
            const auto& target = targets_[candidate];
            if (!target.output) {
                index = candidate;
                break;
            }
            if ((reusable & (1u << candidate)) &&
                (index == target_count || target.last_used < targets_[index].last_used)) index = candidate;
        }
        if (index == target_count) return {Status::TargetsBusy};
        if (!BindTarget(device, output, format, display, targets_[index], index)) return {Status::AllocationFailed};
    }
    auto& target = targets_[index];
    target.last_used = ++uses_;

    // All fallible work is complete. A transition orders the SDK's UAV
    // writes before CAS reads, and another orders CAS writes before the copy.
    auto read = Transition(output, uav, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    list->ResourceBarrier(1, &read);
    ID3D12DescriptorHeap* heaps[]{heap_.Get()};
    list->SetDescriptorHeaps(1, heaps);
    list->SetPipelineState(pipeline_.Get());
    list->SetComputeRootSignature(root_.Get());
    const Constants constants{display.width, display.height, CasPeak(percent), std::max(pre_exposure, 1e-6f),
                              channel_limit};
    list->SetComputeRoot32BitConstants(0, 5, &constants, 0);
    auto gpu = heap_->GetGPUDescriptorHandleForHeapStart();
    gpu.ptr += index * descriptors_per_target * stride_;
    list->SetComputeRootDescriptorTable(1, gpu);
    list->Dispatch((display.width + 7) / 8, (display.height + 7) / 8, 1);

    auto* scratch = target.scratch.Get();
    const D3D12_RESOURCE_BARRIER copy[]{
        Transition(scratch, uav, D3D12_RESOURCE_STATE_COPY_SOURCE),
        Transition(output, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST)
    };
    list->ResourceBarrier(static_cast<UINT>(std::size(copy)), copy);
    D3D12_TEXTURE_COPY_LOCATION source{scratch, D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX};
    D3D12_TEXTURE_COPY_LOCATION dest{output, D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX};
    const D3D12_BOX rect{0, 0, 0, display.width, display.height, 1};
    list->CopyTextureRegion(&dest, 0, 0, 0, &source, &rect);
    const D3D12_RESOURCE_BARRIER restore[]{
        Transition(scratch, D3D12_RESOURCE_STATE_COPY_SOURCE, uav),
        Transition(output, D3D12_RESOURCE_STATE_COPY_DEST, uav)
    };
    list->ResourceBarrier(static_cast<UINT>(std::size(restore)), restore);
    return {Status::Recorded, 1u << index};
}

void SharpnessPass::ClearTargets() noexcept {
    targets_ = {};
    uses_ = 0;
    allocated_bytes_ = 0;
}
}
