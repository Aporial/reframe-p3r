#include "platform.hpp"
#include "settings.hpp"
#include "renderer.hpp"
#include "native_menu.hpp"
#include "d3d12_detour.hpp"
#include "d3d12_slots.h"
#include <wrl/client.h>
#include <bcrypt.h>
#include <climits>
#include <cstddef>

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <map>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using Microsoft::WRL::ComPtr;

namespace reframe {
namespace {
constexpr GUID taa_marker{0xaf2f743e,0x4af5,0x4b11,{0x8e,0x1d,0xe6,0x3e,0x93,0xcb,0xd0,0x29}};
constexpr GUID root_marker{0xff134b13,0x0864,0x42cf,{0x9e,0x2b,0x61,0xdd,0x38,0x4f,0x4c,0x11}};
constexpr GUID lifetime_marker{0x7280fd07,0x15e4,0x4c92,{0x83,0x41,0x55,0x3d,0x56,0xa3,0x6d,0xdb}};
constexpr GUID indirect_marker{0xf311218d,0x6e3e,0x4e56,{0x9b,0xbf,0xea,0xbb,0x61,0x60,0x10,0x28}};
constexpr UINT slots = 64;
constexpr UINT unknown = UINT_MAX;
thread_local bool internal = false;
std::atomic_bool hooks_active{false};
std::atomic<ID3D12Device*> active_device{nullptr};
std::recursive_mutex install_mutex;

void FirstCall(const char* message, std::atomic_bool& seen) {
    if (!seen.exchange(true, std::memory_order_relaxed)) Log("%s", message);
}

struct Scope {
    bool previous = std::exchange(internal, true);
    ~Scope() { internal = previous; }
};

struct ShaderIdentity {
    std::array<unsigned char, 32> digest{};
    std::array<UINT, 3> threads{};
    bool has_digest = false;
    bool has_taa_resources = false;
    bool verified_taa = false;
};

struct RootSlots {
    UINT srv = unknown;
    UINT uav = unknown;
    UINT globals = unknown;
    UINT view = unknown;
};

struct CommandState {
    ID3D12PipelineState* pipeline = nullptr;
    ID3D12RootSignature* compute_root = nullptr;
    std::array<ID3D12DescriptorHeap*, 2> heaps{};
    UINT heap_count = 0;
    ShaderIdentity shader;
    ID3D12GraphicsCommandList* list = nullptr;
    bool direct = false;
    std::array<D3D12_GPU_DESCRIPTOR_HANDLE, slots> compute_tables{}, graphics_tables{};
    std::array<D3D12_GPU_VIRTUAL_ADDRESS, slots> compute_cbvs{};
};

struct Heap {
    SIZE_T cpu;
    UINT64 gpu;
    UINT increment;
    std::vector<View> views;
};

struct Buffer {
    ID3D12Resource* resource;
    UINT64 size;
};

struct Registry {
    std::mutex mutex;
    std::unordered_map<ID3D12GraphicsCommandList*, CommandState> lists;
    std::map<SIZE_T, Heap> heaps;
    std::map<D3D12_GPU_VIRTUAL_ADDRESS, Buffer> buffers;
};

// The callbacks can run when the process tears down D3D12 objects.
Registry& State() {
    static Registry* state = new Registry;
    return *state;
}

enum class Kind { List, Heap, Buffer };

class Lifetime final : public IUnknown {
public:
    Lifetime(Kind kind, UINT64 key) : kind_(kind), key_(key) {}
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** out) override {
        if (!out) return E_POINTER;
        *out = nullptr;
        if (iid != __uuidof(IUnknown)) return E_NOINTERFACE;
        *out = static_cast<IUnknown*>(this);
        AddRef();
        return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return ++refs_; }
    ULONG STDMETHODCALLTYPE Release() override {
        const ULONG left = --refs_;
        if (left) return left;
        if (armed_.load()) {
            if (kind_ == Kind::List) UpscaleCommandsDiscarded(reinterpret_cast<ID3D12GraphicsCommandList*>(key_));
            auto& state = State();
            std::lock_guard lock(state.mutex);
            switch (kind_) {
            case Kind::List: state.lists.erase(reinterpret_cast<ID3D12GraphicsCommandList*>(key_)); break;
            case Kind::Heap: state.heaps.erase(static_cast<SIZE_T>(key_)); break;
            case Kind::Buffer: state.buffers.erase(key_); break;
            }
        }
        delete this;
        return 0;
    }
    void Arm() { armed_ = true; }
private:
    std::atomic<ULONG> refs_{1};
    std::atomic_bool armed_{false};
    Kind kind_;
    UINT64 key_;
};

bool Watch(ID3D12Object* object, Kind kind, UINT64 key) {
    auto* lifetime = new (std::nothrow) Lifetime(kind, key);
    if (!lifetime) return false;
    const bool ok = SUCCEEDED(object->SetPrivateDataInterface(lifetime_marker, lifetime));
    if (ok) lifetime->Arm();
    lifetime->Release();
    return ok;
}

template<typename T>
ComPtr<T> Returned(void** out, HRESULT result) {
    ComPtr<T> value;
    if (SUCCEEDED(result) && out && *out) static_cast<IUnknown*>(*out)->QueryInterface(IID_PPV_ARGS(&value));
    return value;
}

View* CpuView(Registry& state, SIZE_T address) {
    auto it = state.heaps.upper_bound(address);
    if (it == state.heaps.begin()) return nullptr;
    auto& heap = (--it)->second;
    const auto offset = address - heap.cpu;
    if (offset % heap.increment || offset / heap.increment >= heap.views.size()) return nullptr;
    return &heap.views[offset / heap.increment];
}

View GpuView(Registry& state, const CommandState& command, D3D12_GPU_DESCRIPTOR_HANDLE table, UINT offset) {
    if (!table.ptr) return {};
    for (UINT i = 0; i < command.heap_count; ++i) {
        const auto cpu = command.heaps[i]->GetCPUDescriptorHandleForHeapStart().ptr;
        auto it = state.heaps.find(cpu);
        if (it == state.heaps.end()) continue;
        const auto& heap = it->second;
        if (!heap.gpu || table.ptr < heap.gpu) continue;
        const auto bytes = table.ptr - heap.gpu;
        const auto index = bytes / heap.increment + offset;
        if (bytes % heap.increment == 0 && index < heap.views.size()) return heap.views[index];
    }
    return {};
}

void SetView(D3D12_CPU_DESCRIPTOR_HANDLE dest, View value) {
    auto& state = State();
    std::lock_guard lock(state.mutex);
    if (auto* view = CpuView(state, dest.ptr)) *view = value;
}

void RememberBuffer(ID3D12Resource* buffer) {
    if (!buffer) return;
    const auto desc = buffer->GetDesc();
    if (desc.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER) return;
    D3D12_HEAP_PROPERTIES heap{};
    if (FAILED(buffer->GetHeapProperties(&heap, nullptr))) return;
    if (heap.Type != D3D12_HEAP_TYPE_UPLOAD &&
        (heap.Type != D3D12_HEAP_TYPE_CUSTOM || heap.CPUPageProperty != D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE)) return;
    const auto address = buffer->GetGPUVirtualAddress();
    if (!address || !Watch(buffer, Kind::Buffer, address)) return;
    auto& state = State();
    std::lock_guard lock(state.mutex);
    state.buffers.insert_or_assign(address, Buffer{buffer, desc.Width});
}

bool ReadBuffer(Registry& state, UINT64 address, UINT64 offset, void* dest, size_t bytes) {
    auto it = state.buffers.upper_bound(address);
    if (it == state.buffers.begin()) return false;
    const auto& [base, buffer] = *--it;
    const auto relative = address - base;
    if (!address || relative >= buffer.size || offset > buffer.size - relative) return false;
    const auto start = relative + offset;
    if (bytes > buffer.size - start) return false;
    const D3D12_RANGE read{static_cast<SIZE_T>(start), static_cast<SIZE_T>(start + bytes)};
    void* mapped = nullptr;
    if (FAILED(buffer.resource->Map(0, &read, &mapped)) || !mapped) return false;
    std::memcpy(dest, static_cast<const unsigned char*>(mapped) + start, bytes);
    const D3D12_RANGE written{0, 0};
    buffer.resource->Unmap(0, &written);
    return true;
}

using Fingerprint = decltype(ShaderIdentity::digest);
consteval Fingerprint Sha256(const char (&hex)[65]) {
    const auto nibble = [](char c) { return c <= '9' ? c - '0' : c - 'a' + 10; };
    Fingerprint bytes{};
    for (size_t i = 0; i < bytes.size(); ++i)
        bytes[i] = static_cast<unsigned char>(16 * nibble(hex[i * 2]) + nibble(hex[i * 2 + 1]));
    return bytes;
}

// Complete bytecode fingerprints from the game's three captured TAA variants.
struct TaaCapture { size_t length; Fingerprint fingerprint; };
constexpr TaaCapture taa_captures[]{
    {9652,  Sha256("538f0b0ec7bb8f35d77cf37e2095010df2abc231bde31d2974d8bc85f5063497")},
    {16292, Sha256("5ec6b9d908319f8be5d485c2da291c9eeef2c09f7afcf4b5b1db6a085cba3290")},
    {15916, Sha256("63cc58c5179e44ba96cc5802770095c7ec9620f9fcdd83c6ffe62c9a1f337d78")},
};

// PSV0 lists register kinds, spaces and bounds. Use it to surface new permutations in the log.
bool ReadPsv(const unsigned char* data, size_t size, std::array<UINT, 3>& threads) {
    const auto read = [&](size_t offset) {
        uint32_t value = 0;
        std::memcpy(&value, data + offset, 4);
        return value;
    };
    if (size < 12) return false;
    const size_t runtime_size = read(0);
    if (runtime_size > size - 4) return false;
    // PSVRuntimeInfo2 adds three UINT thread dimensions after the 36-byte v1.
    if (runtime_size >= 48) std::memcpy(threads.data(), data + 4 + 36, 12);
    size_t offset = runtime_size + 4;
    if (offset > size - 8) return false;
    const uint32_t count = read(offset);
    const uint32_t stride = read(offset + 4);
    offset += 8;
    if (!count || stride < 16 || count > (size - offset) / stride) return false;
    uint32_t srv_mask = 0, cbv_mask = 0;
    bool uav = false;
    for (uint32_t i = 0; i < count; ++i, offset += stride) {
        const auto type = read(offset);
        if (read(offset + 4) != 0) continue;
        const auto lower = read(offset + 8), upper = read(offset + 12);
        if (type == 3) {
            for (unsigned j = 0; j < 6; ++j) if (lower <= j && upper >= j) srv_mask |= 1u << j;
        } else if (type == 2) {
            for (unsigned j = 0; j < 2; ++j) if (lower <= j && upper >= j) cbv_mask |= 1u << j;
        } else if (type == 6 && lower == 0) {
            uav = true;
        }
    }
    return srv_mask == 0x3f && cbv_mask == 0x3 && uav;
}

ShaderIdentity Identify(D3D12_SHADER_BYTECODE shader) {
    ShaderIdentity identity;
    const auto* bytes = static_cast<const unsigned char*>(shader.pShaderBytecode);
    const auto size = shader.BytecodeLength;
    if (!bytes || size < 32 || size > ULONG_MAX || std::memcmp(bytes, "DXBC", 4) != 0) return identity;
    const auto read = [&](size_t offset) {
        uint32_t value = 0;
        std::memcpy(&value, bytes + offset, sizeof(value));
        return value;
    };
    const size_t parts = read(28);
    if (read(24) != size || parts > (size - 32) / 4) return identity;

    identity.has_digest = BCryptHash(BCRYPT_SHA256_ALG_HANDLE, nullptr, 0, const_cast<PUCHAR>(bytes),
                                     static_cast<ULONG>(size), identity.digest.data(),
                                     static_cast<ULONG>(identity.digest.size())) >= 0;
    if (identity.has_digest) {
        identity.verified_taa = std::any_of(std::begin(taa_captures), std::end(taa_captures), [&](const TaaCapture& capture) {
            return capture.length == size && capture.fingerprint == identity.digest;
        });
    }

    const auto first_part = 32 + parts * 4;
    for (size_t i = 0; i < parts; ++i) {
        const size_t offset = read(32 + i * 4);
        if (offset < first_part || offset > size - 8 || std::memcmp(bytes + offset, "PSV0", 4) != 0) continue;
        const auto payload = read(offset + 4);
        if (payload <= size - offset - 8)
            identity.has_taa_resources = ReadPsv(bytes + offset + 8, payload, identity.threads);
        break;
    }
    return identity;
}

std::string DigestText(const ShaderIdentity& shader) {
    char digest[65]{};
    for (size_t i = 0; i < shader.digest.size(); ++i)
        std::snprintf(digest + i * 2, 3, "%02x", shader.digest[i]);
    return digest;
}

void LogShader(const char* route, D3D12_SHADER_BYTECODE shader, const ShaderIdentity& id) {
    if (!id.has_digest) {
        static std::atomic_bool once{false};
        if (!once.exchange(true)) Log("%s produced an unrecognized compute container (%zu bytes).", route, shader.BytecodeLength);
        return;
    }
    const auto digest = DigestText(id);
    const std::string name = "SHA256-" + digest;
    static std::mutex mutex;
    static std::unordered_set<std::string> seen;
    std::lock_guard lock(mutex);
    if (!seen.emplace(name).second) return;
    char threads[64] = "thread group unavailable in PSV0";
    if (id.threads[0]) std::snprintf(threads, sizeof(threads), "threads %ux%ux%u", id.threads[0], id.threads[1], id.threads[2]);
    Log("CS %s via %s, %zu bytes, %s%s%s.", name.c_str(), route, shader.BytecodeLength, threads,
        id.has_taa_resources ? ", TAA registers" : "",
        id.verified_taa ? ", known TAA permutation" : "");
    static const bool save_dumps = DiagnosticEnabled(L"ShaderDumps");
    if (!save_dumps) return;
    const auto dir = DataDirectory() / L"ShaderDiagnostics";
    std::error_code error;
    std::filesystem::create_directories(dir, error);
    if (error) return;
    const auto path = dir / (name + ".cso");
    if (std::filesystem::exists(path, error) && !error) return;
    std::ofstream bytes(path, std::ios::binary);
    bytes.write(static_cast<const char*>(shader.pShaderBytecode), static_cast<std::streamsize>(shader.BytecodeLength));
}

void MarkPipeline(const char* route, D3D12_SHADER_BYTECODE shader, void** out, HRESULT result) noexcept {
    if (!shader.pShaderBytecode || !shader.BytecodeLength) return;
    const auto id = Identify(shader);
    if (const auto pipeline = Returned<ID3D12PipelineState>(out, result)) {
        pipeline->SetPrivateData(taa_marker, sizeof(id), &id);
        try { LogShader(route, shader, id); }
        catch (...) { Log("Unable to save compute shader diagnostics."); }
    }
}

ShaderIdentity PipelineIdentity(ID3D12PipelineState* pipeline) {
    ShaderIdentity identity;
    UINT bytes = sizeof(identity);
    if (!pipeline || FAILED(pipeline->GetPrivateData(taa_marker, &bytes, &identity))) return {};
    return identity;
}

// Match the documented pointer alignment of a D3D12 pipeline-stream subobject.
#pragma warning(push)
#pragma warning(disable: 4324)
template<typename T> struct alignas(void*) Subobject {
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type;
    T value;
};
#pragma warning(pop)

D3D12_SHADER_BYTECODE StreamComputeShader(const D3D12_PIPELINE_STATE_STREAM_DESC& stream) {
    const auto* bytes = static_cast<const uint8_t*>(stream.pPipelineStateSubobjectStream);
    if (!bytes) return {};
    for (size_t offset = 0; offset + sizeof(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE) <= stream.SizeInBytes;) {
        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type;
        std::memcpy(&type, bytes + offset, sizeof(type));
        size_t size = 0;
        // Compute-only pipeline streams contain the following subobjects.
        switch (type) {
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CS: {
            Subobject<D3D12_SHADER_BYTECODE> compute{};
            if (sizeof(compute) > stream.SizeInBytes - offset) return {};
            std::memcpy(&compute, bytes + offset, sizeof(compute));
            return compute.value;
        }
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE: size = sizeof(Subobject<ID3D12RootSignature*>); break;
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_NODE_MASK: size = sizeof(Subobject<UINT>); break;
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CACHED_PSO: size = sizeof(Subobject<D3D12_CACHED_PIPELINE_STATE>); break;
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_FLAGS: size = sizeof(Subobject<D3D12_PIPELINE_STATE_FLAGS>); break;
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS:
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS:
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DS:
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_HS:
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_GS:
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_AS:
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MS: size = sizeof(Subobject<D3D12_SHADER_BYTECODE>); break;
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_STREAM_OUTPUT: size = sizeof(Subobject<D3D12_STREAM_OUTPUT_DESC>); break;
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND: size = sizeof(Subobject<D3D12_BLEND_DESC>); break;
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER: size = sizeof(Subobject<D3D12_RASTERIZER_DESC>); break;
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER1: size = sizeof(Subobject<D3D12_RASTERIZER_DESC1>); break;
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER2: size = sizeof(Subobject<D3D12_RASTERIZER_DESC2>); break;
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL: size = sizeof(Subobject<D3D12_DEPTH_STENCIL_DESC>); break;
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL1: size = sizeof(Subobject<D3D12_DEPTH_STENCIL_DESC1>); break;
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL2: size = sizeof(Subobject<D3D12_DEPTH_STENCIL_DESC2>); break;
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT: size = sizeof(Subobject<D3D12_INPUT_LAYOUT_DESC>); break;
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_IB_STRIP_CUT_VALUE: size = sizeof(Subobject<D3D12_INDEX_BUFFER_STRIP_CUT_VALUE>); break;
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY: size = sizeof(Subobject<D3D12_PRIMITIVE_TOPOLOGY_TYPE>); break;
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS: size = sizeof(Subobject<D3D12_RT_FORMAT_ARRAY>); break;
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT: size = sizeof(Subobject<DXGI_FORMAT>); break;
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC: size = sizeof(Subobject<DXGI_SAMPLE_DESC>); break;
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK: size = sizeof(Subobject<UINT>); break;
        case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VIEW_INSTANCING: size = sizeof(Subobject<D3D12_VIEW_INSTANCING_DESC>); break;
        default: return {};
        }
        if (size > stream.SizeInBytes - offset) return {};
        offset += size;
    }
    return {};
}

void MarkRoot(const void* bytes, SIZE_T size, void** out, HRESULT status) {
    if (!bytes || FAILED(status) || !out || !*out) return;
    using Deserialize = decltype(&D3D12CreateVersionedRootSignatureDeserializer);
    static const auto deserialize = reinterpret_cast<Deserialize>(SystemD3D12("D3D12CreateVersionedRootSignatureDeserializer"));
    if (!deserialize) return;
    ComPtr<ID3D12VersionedRootSignatureDeserializer> serializer;
    if (FAILED(deserialize(bytes, size, IID_PPV_ARGS(&serializer)))) return;
    const D3D12_VERSIONED_ROOT_SIGNATURE_DESC* descriptor = nullptr;
    if (FAILED(serializer->GetRootSignatureDescAtVersion(D3D_ROOT_SIGNATURE_VERSION_1_0, &descriptor)) ||
        !descriptor || descriptor->Desc_1_0.NumParameters > slots) return;
    RootSlots found;
    const auto& root = descriptor->Desc_1_0;
    for (UINT i = 0; i < root.NumParameters; ++i) {
        const auto& param = root.pParameters[i];
        if (param.ShaderVisibility != D3D12_SHADER_VISIBILITY_ALL) continue;
        if (param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_CBV && param.Descriptor.RegisterSpace == 0) {
            if (param.Descriptor.ShaderRegister == 0) found.globals = i;
            if (param.Descriptor.ShaderRegister == 1) found.view = i;
        }
        if (param.ParameterType != D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE || !param.DescriptorTable.NumDescriptorRanges) continue;
        const auto& range = param.DescriptorTable.pDescriptorRanges[0];
        if (range.BaseShaderRegister != 0 || range.RegisterSpace != 0 ||
            (range.OffsetInDescriptorsFromTableStart != 0 && range.OffsetInDescriptorsFromTableStart != D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND))
            continue;
        if (range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SRV && range.NumDescriptors >= 6) found.srv = i;
        if (range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_UAV && range.NumDescriptors >= 1) found.uav = i;
    }
    if (found.srv != unknown && found.uav != unknown && found.globals != unknown && found.view != unknown)
        if (const auto signature = Returned<ID3D12RootSignature>(out, status))
            signature->SetPrivateData(root_marker, sizeof(found), &found);
}

bool Texture(View view, DXGI_FORMAT format, Size* size = nullptr) {
    if (!view.resource || view.format != format) return false;
    const auto desc = view.resource->GetDesc();
    if (desc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D || desc.DepthOrArraySize != 1 ||
        desc.SampleDesc.Count != 1 || desc.Width > D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION) return false;
    if (size) *size = {static_cast<UINT>(desc.Width), desc.Height};
    return true;
}

namespace taa {
// Factual layout for Steam build 22672075. The game registers these View
// members by name and byte offset. Pass constants come from the shader DXIL.
using Float4 = std::array<float, 4>;
struct PassConstants {
    std::byte unused[8];
    uint32_t camera_cut;
    std::byte before_output[0x170 - 12];
    Float4 output_size_and_inverse;
};

struct ViewConstants {
    static constexpr UINT base = 0x200;
    static constexpr UINT transform_start = 0x7a0;
    static constexpr UINT exposure_start = 0x874;
    static constexpr UINT mip_start = 0x8f4;
    static constexpr UINT phases_start = 0x980;

    std::array<Float4, 4> view_to_clip_no_aa;
    std::byte before_transform[transform_start - base - 64];
    float current_to_history[16];
    Float4 temporal_jitter;
    std::byte before_origin[0x810 - 0x7f0];
    Float4 view_origin;
    Float4 view_size_and_inverse;
    std::byte before_exposure[exposure_start - 0x830];
    float pre_exposure;
    std::byte before_mip[mip_start - exposure_start - 4];
    float material_mip_bias;
    std::byte before_phases[phases_start - mip_start - 4];
    Float4 temporal_params;
};

static_assert(offsetof(PassConstants, camera_cut) == 8 && offsetof(PassConstants, output_size_and_inverse) == 0x170);
static_assert(offsetof(ViewConstants, current_to_history) + ViewConstants::base == 0x7a0);
static_assert(offsetof(ViewConstants, temporal_jitter) + ViewConstants::base == 0x7e0);
static_assert(offsetof(ViewConstants, view_origin) + ViewConstants::base == 0x810);
static_assert(offsetof(ViewConstants, view_size_and_inverse) + ViewConstants::base == 0x820);
static_assert(offsetof(ViewConstants, pre_exposure) + ViewConstants::base == 0x874);
static_assert(offsetof(ViewConstants, material_mip_bias) + ViewConstants::base == 0x8f4);
static_assert(offsetof(ViewConstants, temporal_params) + ViewConstants::base == 0x980);

Size Viewport(const Float4& dimensions, Size allocation) {
    if (!(dimensions[0] >= 64 && dimensions[0] <= allocation.width &&
          dimensions[1] >= 64 && dimensions[1] <= allocation.height)) return {};
    const Size size{static_cast<UINT>(dimensions[0]), static_cast<UINT>(dimensions[1])};
    if (dimensions[0] != size.width || dimensions[1] != size.height ||
        !(std::abs(dimensions[2] * size.width - 1.0f) <= 0.01f &&
          std::abs(dimensions[3] * size.height - 1.0f) <= 0.01f)) return {};
    return size;
}

void ReadProjection(const ViewConstants& view, Frame& frame) {
    // For the game's infinite reversed-Z perspective, depth = near / z.
    // m11 is cot(verticalFov/2), and m32 is the near-plane numerator.
    const auto& p = view.view_to_clip_no_aa;
    if (std::abs(p[2][3] - 1.0f) > 0.001f || std::abs(p[2][2]) > 1e-5f ||
        !(std::isfinite(p[1][1]) && p[1][1] > 0 && std::isfinite(p[3][2]) && p[3][2] > 0) ||
        !std::isfinite(p[2][2]) || !std::isfinite(p[2][3])) return;
    frame.camera_near = p[3][2];
    frame.vertical_fov = 2.0f * std::atan2(1.0f, p[1][1]);
}

uint32_t WholePhase(float value, uint32_t low, uint32_t high) {
    if (!(value >= low && value <= high)) return 0;
    const auto integral = static_cast<uint32_t>(value);
    return value == integral ? integral : 0;
}
}

const char* ReadFrame(Registry& state, const CommandState& command, GameFrame& game, UINT x, UINT y, UINT z) {
    if (!command.compute_root || z != 1) return "missing compute root or unexpected group depth";
    RootSlots root;
    UINT bytes = sizeof(root);
    if (FAILED(command.compute_root->GetPrivateData(root_marker, &bytes, &root))) return "unexpected root layout";

    const auto table = command.compute_tables[root.srv];
    const auto color = GpuView(state, command, table, 1);
    const auto output = GpuView(state, command, command.compute_tables[root.uav], 0);
    game.eye = GpuView(state, command, table, 0);
    game.depth = GpuView(state, command, table, 2);
    game.velocity = GpuView(state, command, table, 3);
    Size velocity, depth, color_allocation, output_allocation, eye;
    if (!Texture(game.velocity, DXGI_FORMAT_R16G16B16A16_UNORM, &velocity))
        return "missing or unexpected velocity SRV";
    if (velocity.width <= 1 || velocity.height <= 1)
        return "dummy velocity texture; preserving the secondary view";
    const auto depth_format = game.depth.format;
    if (depth_format != DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS && depth_format != DXGI_FORMAT_R24_UNORM_X8_TYPELESS &&
        depth_format != DXGI_FORMAT_R32_FLOAT) return "unexpected depth SRV format";
    if (!Texture(game.depth, depth_format, &depth) ||
        !Texture(color, DXGI_FORMAT_R16G16B16A16_FLOAT, &color_allocation) ||
        !Texture(output, DXGI_FORMAT_R16G16B16A16_FLOAT, &output_allocation) ||
        !Texture(game.eye, DXGI_FORMAT_R32G32B32A32_FLOAT, &eye) || eye != Size{1, 1} || color.resource == output.resource)
        return "unexpected color, depth, exposure or output storage";

    taa::PassConstants pass{};
    taa::ViewConstants view{};
    if (!ReadBuffer(state, command.compute_cbvs[root.globals], 0, &pass, sizeof(pass)) ||
        !ReadBuffer(state, command.compute_cbvs[root.view], view.base, &view, sizeof(view)))
        return "could not read TAA pass and View constants";
    const Size display = taa::Viewport(pass.output_size_and_inverse, output_allocation);
    if (!display.width || x != (display.width + 7) / 8 || y != (display.height + 7) / 8)
        return "output viewport does not match TAA dispatch or storage";
    const Size input_limit{std::min({color_allocation.width, depth.width, velocity.width}),
                           std::min({color_allocation.height, depth.height, velocity.height})};
    const Size render = taa::Viewport(view.view_size_and_inverse, input_limit);
    if (!render.width || view.view_origin[0] != 0 || view.view_origin[1] != 0)
        return "unsupported input viewport or origin";

    for (float component : view.current_to_history)
        if (!std::isfinite(component)) return "invalid camera reprojection";
    const float jitter_x = render.width * (view.temporal_jitter[0] * 0.5f);
    const float jitter_y = render.height * (view.temporal_jitter[1] * -0.5f);
    if (!(std::abs(jitter_x) <= 1 && std::abs(jitter_y) <= 1 &&
          std::isfinite(view.pre_exposure) && view.pre_exposure > 0))
        return "invalid jitter or pre-exposure";

    game.frame = {color.resource, game.depth.resource, nullptr, output.resource, nullptr, render, display,
                  jitter_x, jitter_y, view.pre_exposure, 0, pass.camera_cut != 0};
    std::memcpy(game.clip_to_previous, view.current_to_history, sizeof(game.clip_to_previous));
    game.color_allocation = color_allocation;
    game.output_allocation = output_allocation;
    game.mip_bias = std::isfinite(view.material_mip_bias) ? view.material_mip_bias : 0;
    game.jitter_index = taa::WholePhase(view.temporal_params[0], 0, 127);
    game.jitter_phases = taa::WholePhase(view.temporal_params[1], 1, 128);
    taa::ReadProjection(view, game.frame);
    return nullptr;
}

void Restore(ID3D12GraphicsCommandList* list, const CommandState& cmd) {
    list->SetDescriptorHeaps(cmd.heap_count, cmd.heaps.data());
    list->SetComputeRootSignature(cmd.compute_root);
    for (UINT i = 0; i < slots; ++i) {
        if (cmd.compute_tables[i].ptr) list->SetComputeRootDescriptorTable(i, cmd.compute_tables[i]);
        if (cmd.graphics_tables[i].ptr) list->SetGraphicsRootDescriptorTable(i, cmd.graphics_tables[i]);
        if (cmd.compute_cbvs[i]) list->SetComputeRootConstantBufferView(i, cmd.compute_cbvs[i]);
    }
    list->SetPipelineState(cmd.pipeline);
}

template<typename Fn>
void TrackCommand(ID3D12GraphicsCommandList* list, Fn&& update) {
    if (internal || !hooks_active.load(std::memory_order_relaxed)) return;
    auto& state = State();
    std::lock_guard lock(state.mutex);
    if (auto found = state.lists.find(list); found != state.lists.end()) update(found->second);
}

void TrackNewList(ID3D12GraphicsCommandList* list, ID3D12PipelineState* pipeline) {
    if (!list || internal || !hooks_active.load(std::memory_order_relaxed)) return;
    const auto type = list->GetType();
    if (type != D3D12_COMMAND_LIST_TYPE_DIRECT && type != D3D12_COMMAND_LIST_TYPE_COMPUTE) return;
    {
        auto& state = State();
        std::lock_guard lock(state.mutex);
        if (state.lists.contains(list)) return;
    }
    if (!Watch(list, Kind::List, reinterpret_cast<UINT64>(list))) return;
    auto& state = State();
    std::lock_guard lock(state.mutex);
    auto& command = state.lists[list];
    command.pipeline = pipeline;
    command.shader = PipelineIdentity(pipeline);
    command.list = list;
    command.direct = type == D3D12_COMMAND_LIST_TYPE_DIRECT;
}

void ResetState(CommandState& cmd, ID3D12PipelineState* pipeline) {
    CommandState reset;
    reset.list = cmd.list;
    reset.direct = cmd.direct;
    reset.pipeline = pipeline;
    reset.shader = PipelineIdentity(pipeline);
    cmd = reset;
}

void AttachCommandList(ID3D12GraphicsCommandList* list);
void AttachPipelineLibrary(ID3D12PipelineLibrary* library);
void AttachCommandQueue(ID3D12CommandQueue* queue);

#define HOOK(ret, name, arguments) \
    ret STDMETHODCALLTYPE Hook##name arguments; \
    using Route##name = Detour<decltype(&Hook##name), &Hook##name>; \
    thread_local auto& Original##name = Route##name::original; \
    ret STDMETHODCALLTYPE Hook##name arguments

HOOK(HRESULT, ComputePipeline, (ID3D12Device* dev, const D3D12_COMPUTE_PIPELINE_STATE_DESC* desc,
                               REFIID iid, void** out)) {
    const HRESULT result = OriginalComputePipeline(dev, desc, iid, out);
    if (!internal && hooks_active && desc && SUCCEEDED(result)) {
        static std::atomic_bool seen{false};
        FirstCall("Game created a compute pipeline through ID3D12Device.", seen);
        MarkPipeline("CreateComputePipelineState", desc->CS, out, result);
    }
    return result;
}

HOOK(HRESULT, PipelineStream, (ID3D12Device2* dev, const D3D12_PIPELINE_STATE_STREAM_DESC* desc,
                              REFIID iid, void** out)) {
    const HRESULT result = OriginalPipelineStream(dev, desc, iid, out);
    if (!internal && hooks_active && desc && SUCCEEDED(result)) {
        static std::atomic_bool seen{false};
        const auto shader = StreamComputeShader(*desc);
        FirstCall("Game called ID3D12Device2::CreatePipelineState.", seen);
        MarkPipeline("CreatePipelineState", shader, out, result);
    }
    return result;
}

HOOK(HRESULT, LibraryCompute, (ID3D12PipelineLibrary* library, LPCWSTR name,
                               const D3D12_COMPUTE_PIPELINE_STATE_DESC* desc, REFIID iid, void** out)) {
    const auto result = OriginalLibraryCompute(library, name, desc, iid, out);
    if (!internal && hooks_active && desc && SUCCEEDED(result)) MarkPipeline("LoadComputePipeline", desc->CS, out, result);
    return result;
}

HOOK(HRESULT, LibraryStream, (ID3D12PipelineLibrary1* library, LPCWSTR name,
                              const D3D12_PIPELINE_STATE_STREAM_DESC* desc, REFIID iid, void** out)) {
    const auto result = OriginalLibraryStream(library, name, desc, iid, out);
    if (!internal && hooks_active && desc && SUCCEEDED(result)) MarkPipeline("LoadPipeline", StreamComputeShader(*desc), out, result);
    return result;
}

HOOK(HRESULT, CreateLibrary, (ID3D12Device1* dev, const void* blob, SIZE_T size, REFIID iid, void** out)) {
    const HRESULT result = OriginalCreateLibrary(dev, blob, size, iid, out);
    if (!internal && hooks_active) {
        Log("CreatePipelineLibrary on device %p, %zu cached bytes, HRESULT %08lX.",
            static_cast<void*>(dev), size, static_cast<unsigned long>(result));
        if (const auto library = Returned<ID3D12PipelineLibrary>(out, result))
            AttachPipelineLibrary(library.Get());
    }
    return result;
}

HOOK(HRESULT, CommandSignature, (ID3D12Device* dev, const D3D12_COMMAND_SIGNATURE_DESC* desc,
                                ID3D12RootSignature* root, REFIID iid, void** out)) {
    const HRESULT result = OriginalCommandSignature(dev, desc, root, iid, out);
    if (!internal && hooks_active && desc && desc->pArgumentDescs && SUCCEEDED(result)) {
        for (UINT i = 0; i < desc->NumArgumentDescs; ++i) {
            if (desc->pArgumentDescs[i].Type != D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH) continue;
            if (const auto signature = Returned<ID3D12CommandSignature>(out, result)) {
                const uint8_t yes = 1;
                signature->SetPrivateData(indirect_marker, sizeof(yes), &yes);
            }
            break;
        }
    }
    return result;
}

HOOK(HRESULT, RootSignature, (ID3D12Device* dev, UINT mask, const void* bytes, SIZE_T size, REFIID iid, void** out)) {
    const HRESULT result = OriginalRootSignature(dev, mask, bytes, size, iid, out);
    if (!internal && hooks_active) MarkRoot(bytes, size, out, result);
    return result;
}

HOOK(HRESULT, Heap, (ID3D12Device* dev, const D3D12_DESCRIPTOR_HEAP_DESC* desc, REFIID iid, void** out)) {
    const HRESULT result = OriginalHeap(dev, desc, iid, out);
    if (internal || !hooks_active || !desc || desc->Type != D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) return result;
    try {
        if (const auto heap = Returned<ID3D12DescriptorHeap>(out, result)) {
            const SIZE_T cpu = heap->GetCPUDescriptorHandleForHeapStart().ptr;
            Heap tracked{cpu, heap->GetGPUDescriptorHandleForHeapStart().ptr,
                dev->GetDescriptorHandleIncrementSize(desc->Type), std::vector<View>(desc->NumDescriptors)};
            if (tracked.increment && Watch(heap.Get(), Kind::Heap, cpu)) {
                auto& state = State();
                std::lock_guard lock(state.mutex);
                state.heaps.insert_or_assign(cpu, std::move(tracked));
            }
        }
    } catch (...) { hooks_active = false; }
    return result;
}

HOOK(void, Srv, (ID3D12Device* dev, ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc,
                D3D12_CPU_DESCRIPTOR_HANDLE dest)) {
    OriginalSrv(dev, resource, desc, dest);
    if (!internal && hooks_active) SetView(dest, {resource, desc ? desc->Format : resource ? resource->GetDesc().Format : DXGI_FORMAT_UNKNOWN});
}

HOOK(void, Uav, (ID3D12Device* dev, ID3D12Resource* resource, ID3D12Resource* counter,
                const D3D12_UNORDERED_ACCESS_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE dest)) {
    OriginalUav(dev, resource, counter, desc, dest);
    if (!internal && hooks_active) SetView(dest, {resource, desc ? desc->Format : resource ? resource->GetDesc().Format : DXGI_FORMAT_UNKNOWN});
}

HOOK(void, Cbv, (ID3D12Device* dev, const D3D12_CONSTANT_BUFFER_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE dest)) {
    OriginalCbv(dev, desc, dest);
    if (!internal && hooks_active) SetView(dest, {});
}

HOOK(void, CopySimple, (ID3D12Device* dev, UINT count, D3D12_CPU_DESCRIPTOR_HANDLE dest,
                       D3D12_CPU_DESCRIPTOR_HANDLE src, D3D12_DESCRIPTOR_HEAP_TYPE type)) {
    OriginalCopySimple(dev, count, dest, src, type);
    if (internal || !hooks_active || type != D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) return;
    const UINT step = dev->GetDescriptorHandleIncrementSize(type);
    auto& state = State();
    std::lock_guard lock(state.mutex);
    for (UINT i = 0; i < count; ++i) {
        auto* to = CpuView(state, dest.ptr + SIZE_T(i) * step);
        auto* from = CpuView(state, src.ptr + SIZE_T(i) * step);
        if (to) *to = from ? *from : View{};
    }
}

HOOK(void, Copy, (ID3D12Device* dev, UINT dest_count, const D3D12_CPU_DESCRIPTOR_HANDLE* dest, const UINT* dest_sizes,
                 UINT src_count, const D3D12_CPU_DESCRIPTOR_HANDLE* src, const UINT* src_sizes,
                 D3D12_DESCRIPTOR_HEAP_TYPE type)) {
    OriginalCopy(dev, dest_count, dest, dest_sizes, src_count, src, src_sizes, type);
    if (internal || !hooks_active || type != D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) return;
    const UINT step = dev->GetDescriptorHandleIncrementSize(type);
    auto& state = State();
    std::lock_guard lock(state.mutex);
    UINT src_range = 0, src_index = 0;
    for (UINT range = 0; range < dest_count; ++range) {
        const UINT count = dest_sizes ? dest_sizes[range] : 1;
        for (UINT i = 0; i < count; ++i) {
            while (src_range < src_count && src_index >= (src_sizes ? src_sizes[src_range] : 1)) {
                ++src_range;
                src_index = 0;
            }
            if (src_range == src_count) return;
            auto* to = CpuView(state, dest[range].ptr + SIZE_T(i) * step);
            auto* from = CpuView(state, src[src_range].ptr + SIZE_T(src_index++) * step);
            if (to) *to = from ? *from : View{};
        }
    }
}

void TrackNewBuffer(void** out, HRESULT status) {
    if (internal || !hooks_active || FAILED(status)) return;
    try { RememberBuffer(Returned<ID3D12Resource>(out, status).Get()); }
    catch (...) { hooks_active = false; }
}

HOOK(HRESULT, Committed, (ID3D12Device* dev, const D3D12_HEAP_PROPERTIES* heap, D3D12_HEAP_FLAGS flags,
                         const D3D12_RESOURCE_DESC* desc, D3D12_RESOURCE_STATES initial,
                         const D3D12_CLEAR_VALUE* clear, REFIID iid, void** out)) {
    const HRESULT result = OriginalCommitted(dev, heap, flags, desc, initial, clear, iid, out);
    if (desc && desc->Dimension == D3D12_RESOURCE_DIMENSION_BUFFER) TrackNewBuffer(out, result);
    return result;
}

HOOK(HRESULT, Placed, (ID3D12Device* dev, ID3D12Heap* heap, UINT64 offset, const D3D12_RESOURCE_DESC* desc,
                      D3D12_RESOURCE_STATES initial, const D3D12_CLEAR_VALUE* clear, REFIID iid, void** out)) {
    const HRESULT result = OriginalPlaced(dev, heap, offset, desc, initial, clear, iid, out);
    if (desc && desc->Dimension == D3D12_RESOURCE_DIMENSION_BUFFER) TrackNewBuffer(out, result);
    return result;
}

HOOK(HRESULT, Committed1, (ID3D12Device4* dev, const D3D12_HEAP_PROPERTIES* heap, D3D12_HEAP_FLAGS flags,
                          const D3D12_RESOURCE_DESC* desc, D3D12_RESOURCE_STATES initial,
                          const D3D12_CLEAR_VALUE* clear, ID3D12ProtectedResourceSession* session, REFIID iid, void** out)) {
    const HRESULT result = OriginalCommitted1(dev, heap, flags, desc, initial, clear, session, iid, out);
    if (desc && desc->Dimension == D3D12_RESOURCE_DIMENSION_BUFFER) TrackNewBuffer(out, result);
    return result;
}

HOOK(HRESULT, Committed2, (ID3D12Device8* dev, const D3D12_HEAP_PROPERTIES* heap, D3D12_HEAP_FLAGS flags,
                          const D3D12_RESOURCE_DESC1* desc, D3D12_RESOURCE_STATES initial,
                          const D3D12_CLEAR_VALUE* clear, ID3D12ProtectedResourceSession* session, REFIID iid, void** out)) {
    const HRESULT result = OriginalCommitted2(dev, heap, flags, desc, initial, clear, session, iid, out);
    if (desc && desc->Dimension == D3D12_RESOURCE_DIMENSION_BUFFER) TrackNewBuffer(out, result);
    return result;
}

HOOK(HRESULT, Placed1, (ID3D12Device8* dev, ID3D12Heap* heap, UINT64 offset, const D3D12_RESOURCE_DESC1* desc,
                       D3D12_RESOURCE_STATES initial, const D3D12_CLEAR_VALUE* clear, REFIID iid, void** out)) {
    const HRESULT result = OriginalPlaced1(dev, heap, offset, desc, initial, clear, iid, out);
    if (desc && desc->Dimension == D3D12_RESOURCE_DIMENSION_BUFFER) TrackNewBuffer(out, result);
    return result;
}

HOOK(HRESULT, CreateList, (ID3D12Device* dev, UINT mask, D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator* allocator,
                          ID3D12PipelineState* pipeline, REFIID iid, void** out)) {
    const HRESULT result = OriginalCreateList(dev, mask, type, allocator, pipeline, iid, out);
    if (!internal && hooks_active && (type == D3D12_COMMAND_LIST_TYPE_DIRECT || type == D3D12_COMMAND_LIST_TYPE_COMPUTE)) {
        try {
            if (const auto list = Returned<ID3D12GraphicsCommandList>(out, result)) {
                static std::atomic_bool seen{false};
                FirstCall("Game created a list via CreateCommandList.", seen);
                AttachCommandList(list.Get());
                TrackNewList(list.Get(), pipeline);
            }
        } catch (...) { Log("Unable to track CreateCommandList."); hooks_active = false; }
    }
    return result;
}

HOOK(HRESULT, CreateList1, (ID3D12Device4* dev, UINT mask, D3D12_COMMAND_LIST_TYPE type, D3D12_COMMAND_LIST_FLAGS flags,
                           REFIID iid, void** out)) {
    const HRESULT result = OriginalCreateList1(dev, mask, type, flags, iid, out);
    if (!internal && hooks_active && (type == D3D12_COMMAND_LIST_TYPE_DIRECT || type == D3D12_COMMAND_LIST_TYPE_COMPUTE)) {
        try {
            if (const auto list = Returned<ID3D12GraphicsCommandList>(out, result)) {
                static std::atomic_bool seen{false};
                FirstCall("Game created a list via CreateCommandList1.", seen);
                AttachCommandList(list.Get());
                TrackNewList(list.Get(), nullptr);
            }
        } catch (...) { Log("Unable to track CreateCommandList1."); hooks_active = false; }
    }
    return result;
}

HOOK(HRESULT, Reset, (ID3D12GraphicsCommandList* list, ID3D12CommandAllocator* allocator, ID3D12PipelineState* pso)) {
    const HRESULT result = OriginalReset(list, allocator, pso);
    if (SUCCEEDED(result) && !internal && hooks_active) {
        UpscaleCommandsDiscarded(list);
        try {
            AttachCommandList(list);
            TrackNewList(list, pso);
            TrackCommand(list, [&](CommandState& cmd) { ResetState(cmd, pso); });
        } catch (...) { Log("Unable to track Reset."); hooks_active = false; }
    }
    return result;
}

HOOK(void, Clear, (ID3D12GraphicsCommandList* list, ID3D12PipelineState* pso)) {
    OriginalClear(list, pso);
    TrackCommand(list, [&](CommandState& cmd) { ResetState(cmd, pso); });
}

HOOK(void, Pipeline, (ID3D12GraphicsCommandList* list, ID3D12PipelineState* pso)) {
    OriginalPipeline(list, pso);
    TrackCommand(list, [&](CommandState& cmd) { cmd.pipeline = pso; cmd.shader = PipelineIdentity(pso); });
}

HOOK(void, Heaps, (ID3D12GraphicsCommandList* list, UINT count, ID3D12DescriptorHeap* const* heaps)) {
    OriginalHeaps(list, count, heaps);
    TrackCommand(list, [&](CommandState& cmd) {
        if (count > cmd.heaps.size()) { cmd.heap_count = 0; return; }
        if (cmd.heap_count == count && std::equal(cmd.heaps.begin(), cmd.heaps.begin() + count, heaps)) return;
        cmd.heap_count = count;
        if (count) std::copy_n(heaps, count, cmd.heaps.begin());
        cmd.compute_tables = {};
        cmd.graphics_tables = {};
    });
}

HOOK(void, ComputeRoot, (ID3D12GraphicsCommandList* list, ID3D12RootSignature* root)) {
    OriginalComputeRoot(list, root);
    TrackCommand(list, [&](CommandState& cmd) {
        if (cmd.compute_root == root) return;
        cmd.compute_root = root;
        cmd.compute_tables = {};
        cmd.compute_cbvs = {};
    });
}

HOOK(void, GraphicsRoot, (ID3D12GraphicsCommandList* list, ID3D12RootSignature* root)) {
    OriginalGraphicsRoot(list, root);
    TrackCommand(list, [&](CommandState& cmd) { cmd.graphics_tables = {}; });
}

HOOK(void, ComputeTable, (ID3D12GraphicsCommandList* list, UINT slot, D3D12_GPU_DESCRIPTOR_HANDLE address)) {
    OriginalComputeTable(list, slot, address);
    if (slot < slots) TrackCommand(list, [&](CommandState& cmd) { cmd.compute_tables[slot] = address; });
}

HOOK(void, GraphicsTable, (ID3D12GraphicsCommandList* list, UINT slot, D3D12_GPU_DESCRIPTOR_HANDLE address)) {
    OriginalGraphicsTable(list, slot, address);
    if (slot < slots) TrackCommand(list, [&](CommandState& cmd) { cmd.graphics_tables[slot] = address; });
}

HOOK(void, ComputeCbv, (ID3D12GraphicsCommandList* list, UINT slot, D3D12_GPU_VIRTUAL_ADDRESS address)) {
    OriginalComputeCbv(list, slot, address);
    if (slot < slots) TrackCommand(list, [&](CommandState& cmd) { cmd.compute_cbvs[slot] = address; });
}

HOOK(void, Dispatch, (ID3D12GraphicsCommandList* list, UINT x, UINT y, UINT z)) {
    if (!internal && hooks_active) {
        static std::atomic_bool seen{false};
        FirstCall("Intercepted Direct3D 12 Dispatch.", seen);
        CommandState snapshot;
        GameFrame game;
        const char* reason = "untracked command list";
        bool recognized = false;
        bool candidate = false;
        {
            auto& state = State();
            std::lock_guard lock(state.mutex);
            const auto it = state.lists.find(list);
            if (it != state.lists.end()) {
                const auto& shader = it->second.shader;
                recognized = shader.verified_taa;
                candidate = shader.has_taa_resources &&
                    (!shader.threads[0] || shader.threads == std::array<UINT, 3>{8, 8, 1});
                if (recognized || candidate) {
                    snapshot = it->second;
                    reason = ReadFrame(state, snapshot, game, x, y, z);
                }
            }
        }
        if (recognized || candidate) {
            static std::mutex mutex;
            static std::unordered_set<std::string> observed;
            const auto digest = DigestText(snapshot.shader);
            const auto key = digest + '/' + std::to_string(x) + 'x' + std::to_string(y) + (reason ? reason : " valid");
            std::lock_guard lock(mutex);
            if (observed.emplace(key).second)
                Log("Dispatch %ux%ux%u %s %s on %s list %p; %s.", x, y, z, recognized ? "known TAA" : "candidate",
                    digest.c_str(), snapshot.direct ? "direct" : "compute", static_cast<void*>(list),
                    reason ? reason : "main-view inputs validated");
        }
        ComPtr<ID3D12Device> device;
        if (recognized && !reason && snapshot.direct && active_device &&
            SUCCEEDED(list->GetDevice(IID_PPV_ARGS(&device))) && device.Get() == active_device) {
            Scope scope;
            bool replaced = false;
            try { replaced = RenderUpscaled(list, game); }
            catch (...) { hooks_active = false; }
            Restore(list, snapshot);
            if (replaced) return;
        }
    }
    OriginalDispatch(list, x, y, z);
}

HOOK(void, Indirect, (ID3D12GraphicsCommandList* list, ID3D12CommandSignature* signature, UINT count,
                      ID3D12Resource* arguments, UINT64 offset, ID3D12Resource* count_buffer, UINT64 count_offset)) {
    if (!internal && hooks_active) {
        static std::atomic_bool first{false};
        FirstCall("Intercepted ExecuteIndirect.", first);
        UINT size = 1;
        uint8_t dispatch = 0;
        if (signature && SUCCEEDED(signature->GetPrivateData(indirect_marker, &size, &dispatch)) && dispatch) {
            static std::atomic_bool first_compute{false};
            FirstCall("Game used an indirect compute dispatch. Original commands preserved.", first_compute);
        }
    }
    OriginalIndirect(list, signature, count, arguments, offset, count_buffer, count_offset);
}

HOOK(HRESULT, CreateQueue, (ID3D12Device* device, const D3D12_COMMAND_QUEUE_DESC* desc, REFIID iid, void** out)) {
    const auto hr = OriginalCreateQueue(device, desc, iid, out);
    if (!internal && hooks_active && desc && desc->Type == D3D12_COMMAND_LIST_TYPE_DIRECT && device == active_device) {
        if (const auto queue = Returned<ID3D12CommandQueue>(out, hr)) AttachCommandQueue(queue.Get());
    }
    return hr;
}

HOOK(void, ExecuteLists, (ID3D12CommandQueue* queue, UINT count, ID3D12CommandList* const* lists)) {
    if (internal || !hooks_active) {
        OriginalExecuteLists(queue, count, lists);
        return;
    }
    Scope scope;
    const bool uses_upscaler = BeginUpscaleSubmission(count, lists);
    OriginalExecuteLists(queue, count, lists);
    if (uses_upscaler) EndUpscaleSubmission(queue);
}

#define INSTALL(obj, slot, name) ok &= Route##name::Install((obj), d3d12_slots.slot, #name)

void AttachCommandQueue(ID3D12CommandQueue* queue) {
    std::lock_guard lock(install_mutex);
    bool ok = true;
    INSTALL(queue, execute_lists, ExecuteLists);
    if (ok && MH_ApplyQueued() == MH_OK) ObserveUpscaleQueue(queue);
    else Log("Unable to observe direct queue submissions; temporal modes will preserve Original TAA.");
}

void AttachCommandList(ID3D12GraphicsCommandList* list) {
    if (RouteDispatch::HasTarget(list, d3d12_slots.dispatch) && RouteReset::HasTarget(list, d3d12_slots.reset)) return;
    std::lock_guard lock(install_mutex);
    bool ok = true;
    INSTALL(list, reset, Reset);
    INSTALL(list, clear, Clear);
    INSTALL(list, dispatch, Dispatch);
    INSTALL(list, pipeline, Pipeline);
    INSTALL(list, heaps, Heaps);
    INSTALL(list, compute_root, ComputeRoot);
    INSTALL(list, graphics_root, GraphicsRoot);
    INSTALL(list, compute_table, ComputeTable);
    INSTALL(list, graphics_table, GraphicsTable);
    INSTALL(list, compute_cbv, ComputeCbv);
    INSTALL(list, indirect, Indirect);
    if (!ok || MH_ApplyQueued() != MH_OK) {
        hooks_active = false;
        Log("Command list implementation could not be tracked; using Original TAA.");
    }
}

void AttachPipelineLibrary(ID3D12PipelineLibrary* library) {
    std::lock_guard lock(install_mutex);
    bool ok = true;
    INSTALL(library, library_compute, LibraryCompute);
    ComPtr<ID3D12PipelineLibrary1> version;
    if (SUCCEEDED(library->QueryInterface(IID_PPV_ARGS(&version)))) { INSTALL(version.Get(), library_stream, LibraryStream); }
    if (!ok || MH_ApplyQueued() != MH_OK) Log("Unable to track a pipeline library implementation.");
}

bool InitializeHooks() {
    static const bool ready = MH_Initialize() == MH_OK;
    return ready;
}

HOOK(HRESULT, FactoryDevice, (ID3D12DeviceFactory* factory, IUnknown* adapter, D3D_FEATURE_LEVEL level,
                              REFIID iid, void** out)) {
    const HRESULT result = OriginalFactoryDevice(factory, adapter, level, iid, out);
    if (!internal) {
        if (const auto device = Returned<ID3D12Device>(out, result)) {
            Log("Device factory returned D3D12 device %p.", static_cast<void*>(device.Get()));
            AttachRenderer(device.Get());
        }
    }
    return result;
}

void AttachDeviceFactory(ID3D12DeviceFactory* factory) {
    std::lock_guard lock(install_mutex);
    if (!InitializeHooks()) return;
    bool ok = true;
    INSTALL(factory, factory_device, FactoryDevice);
    if (!ok || MH_ApplyQueued() != MH_OK) Log("Unable to track the D3D12 device factory.");
}

HOOK(HRESULT, SdkFactory, (ID3D12SDKConfiguration1* config, UINT sdk, LPCSTR path, REFIID iid, void** out)) {
    const HRESULT result = OriginalSdkFactory(config, sdk, path, iid, out);
    if (const auto factory = Returned<ID3D12DeviceFactory>(out, result)) AttachDeviceFactory(factory.Get());
    return result;
}
}

void AttachD3d12Interface(IUnknown* object, REFIID iid) noexcept {
    try {
        if (iid == __uuidof(ID3D12DeviceFactory)) {
            AttachDeviceFactory(static_cast<ID3D12DeviceFactory*>(object));
        } else if (iid == __uuidof(ID3D12SDKConfiguration1) || iid == __uuidof(ID3D12SDKConfiguration)) {
            ComPtr<ID3D12SDKConfiguration1> config;
            if (FAILED(object->QueryInterface(IID_PPV_ARGS(&config)))) return;
            std::lock_guard lock(install_mutex);
            if (!InitializeHooks()) return;
            bool ok = true;
            INSTALL(config.Get(), sdk_factory, SdkFactory);
            if (!ok || MH_ApplyQueued() != MH_OK) Log("Unable to track the D3D12 SDK configuration.");
        }
    } catch (...) { Log("Failed to attach D3D12 interface tracking."); }
}

void AttachRenderer(ID3D12Device* device) noexcept {
    if (internal) return;
    try {
        std::lock_guard lock(install_mutex);
        Scope scope;
        static std::unordered_set<ID3D12Device*> devices;
        if (!devices.emplace(device).second || !InitializeHooks()) return;
        const auto luid = device->GetAdapterLuid();
        Log("RE:FRAME %s attached device %p, adapter LUID %08lX:%08lX.",
            mod_version, static_cast<void*>(device), static_cast<unsigned long>(luid.HighPart),
            static_cast<unsigned long>(luid.LowPart));
        Log("SDK COM slots: CreatePipelineLibrary=%zu, CreatePipelineState=%zu, CreateCommandList1=%zu.",
            d3d12_slots.create_library, d3d12_slots.pipeline_stream, d3d12_slots.create_list1);
        if (!active_device && InitializeUpscalers(device)) active_device = device;
        InitializeNativeMenu();
        bool ok = true;
        const auto device_methods = [&](ID3D12Device* entry) {
            INSTALL(entry, create_queue, CreateQueue);
            INSTALL(entry, compute_pipeline, ComputePipeline);
            INSTALL(entry, create_list, CreateList);
            INSTALL(entry, heap, Heap);
            INSTALL(entry, root_signature, RootSignature);
            INSTALL(entry, cbv, Cbv);
            INSTALL(entry, srv, Srv);
            INSTALL(entry, uav, Uav);
            INSTALL(entry, copy, Copy);
            INSTALL(entry, copy_simple, CopySimple);
            INSTALL(entry, committed, Committed);
            INSTALL(entry, placed, Placed);
            INSTALL(entry, command_signature, CommandSignature);
        };
        device_methods(device);
        ComPtr<ID3D12Device1> dev1;
        if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&dev1)))) { INSTALL(dev1.Get(), create_library, CreateLibrary); }
        ComPtr<ID3D12Device2> dev2;
        if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&dev2)))) {
            device_methods(dev2.Get());
            INSTALL(dev2.Get(), create_library, CreateLibrary);
            INSTALL(dev2.Get(), pipeline_stream, PipelineStream);
        }
        ComPtr<ID3D12Device4> dev4;
        if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&dev4)))) {
            device_methods(dev4.Get());
            INSTALL(dev4.Get(), create_list1, CreateList1);
            INSTALL(dev4.Get(), committed1, Committed1);
        }
        ComPtr<ID3D12Device8> dev8;
        if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&dev8)))) {
            device_methods(dev8.Get());
            INSTALL(dev8.Get(), committed2, Committed2);
            INSTALL(dev8.Get(), placed1, Placed1);
        }
        if (!ok || MH_ApplyQueued() != MH_OK) {
            hooks_active = false;
            Log("Direct3D 12 hooks failed; using Original TAA.");
            return;
        }
        hooks_active = true;
        Log("Direct3D 12 device tracking enabled; command lists and pipeline libraries will be tracked as created.");
    } catch (...) {
        Log("Renderer initialization failed; using Original TAA.");
    }
}
#undef INSTALL
}
