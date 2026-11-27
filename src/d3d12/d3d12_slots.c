// The SDK's C interface incorporates the correct Windows ABI branches, including
// the conditional signatures of methods that return structures by value.
#define CINTERFACE
#include <windows.h>
#pragma warning(push)
#pragma warning(disable: 4201)
#include <d3d12.h>
#pragma warning(pop)
#include "d3d12_slots.h"

#define SLOT(interface_type, method) (offsetof(interface_type##Vtbl, method) / sizeof(void*))
#define LIST(method) SLOT(ID3D12GraphicsCommandList, method)
#define DEVICE(method) SLOT(ID3D12Device, method)

const D3d12Slots d3d12_slots = {
    DEVICE(CreateComputePipelineState),
    DEVICE(CreateCommandList),
    DEVICE(CreateDescriptorHeap),
    DEVICE(CreateRootSignature),
    DEVICE(CreateConstantBufferView),
    DEVICE(CreateShaderResourceView),
    DEVICE(CreateUnorderedAccessView),
    DEVICE(CopyDescriptors),
    DEVICE(CopyDescriptorsSimple),
    DEVICE(CreateCommittedResource),
    DEVICE(CreatePlacedResource),
    DEVICE(CreateCommandSignature),
    SLOT(ID3D12Device1, CreatePipelineLibrary),
    SLOT(ID3D12Device2, CreatePipelineState),
    SLOT(ID3D12Device4, CreateCommandList1),
    SLOT(ID3D12Device4, CreateCommittedResource1),
    SLOT(ID3D12Device8, CreateCommittedResource2),
    SLOT(ID3D12Device8, CreatePlacedResource1),
    SLOT(ID3D12PipelineLibrary, LoadComputePipeline),
    SLOT(ID3D12PipelineLibrary1, LoadPipeline),
    LIST(Reset),
    LIST(ClearState),
    LIST(Dispatch),
    LIST(SetPipelineState),
    LIST(SetDescriptorHeaps),
    LIST(SetComputeRootSignature),
    LIST(SetGraphicsRootSignature),
    LIST(SetComputeRootDescriptorTable),
    LIST(SetGraphicsRootDescriptorTable),
    LIST(SetComputeRootConstantBufferView),
    LIST(ExecuteIndirect),
    SLOT(ID3D12DeviceFactory, CreateDevice),
    SLOT(ID3D12SDKConfiguration1, CreateDeviceFactory),
    DEVICE(CreateCommandQueue),
    SLOT(ID3D12CommandQueue, ExecuteCommandLists),
    DEVICE(CreateRenderTargetView),
    LIST(RSSetViewports),
    LIST(RSSetScissorRects),
    LIST(OMSetRenderTargets),
    LIST(DrawIndexedInstanced),
    DEVICE(CreateGraphicsPipelineState),
    SLOT(ID3D12PipelineLibrary, LoadGraphicsPipeline),
    LIST(SetGraphicsRootConstantBufferView)
};
