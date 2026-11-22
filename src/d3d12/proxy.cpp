#include "platform.hpp"
#include <wrl/client.h>

namespace {
bool IsPersona() noexcept {
    wchar_t executable[MAX_PATH]{};
    const DWORD length = GetModuleFileNameW(nullptr, executable, MAX_PATH);
    if (!length || length >= MAX_PATH) return false;
    const wchar_t* separator = wcsrchr(executable, L'\\');
    return separator && _wcsicmp(separator + 1, L"P3R.exe") == 0;
}
}

extern "C" HRESULT WINAPI ProxyCreateDevice(IUnknown* adapter, D3D_FEATURE_LEVEL level, REFIID iid, void** out) {
    const auto create = reinterpret_cast<PFN_D3D12_CREATE_DEVICE>(reframe::SystemD3D12("D3D12CreateDevice"));
    if (!create) return E_NOINTERFACE;
    const HRESULT result = create(adapter, level, iid, out);
    if (SUCCEEDED(result) && out && *out && IsPersona()) {
        Microsoft::WRL::ComPtr<ID3D12Device> device;
        if (SUCCEEDED(static_cast<IUnknown*>(*out)->QueryInterface(IID_PPV_ARGS(&device)))) {
            reframe::AttachRenderer(device.Get());
        }
    }
    return result;
}

#define FORWARD_HRESULT(wrapper, symbol, parameters, arguments) \
    extern "C" HRESULT WINAPI wrapper parameters { \
        const auto call = reinterpret_cast<decltype(&wrapper)>(reframe::SystemD3D12(#symbol)); \
        return call ? call arguments : E_NOINTERFACE; \
    }

FORWARD_HRESULT(ProxyGetDebug, D3D12GetDebugInterface, (REFIID iid, void** out), (iid, out))
extern "C" HRESULT WINAPI ProxyGetInterface(REFCLSID clsid, REFIID iid, void** out) {
    const auto call = reinterpret_cast<decltype(&ProxyGetInterface)>(reframe::SystemD3D12("D3D12GetInterface"));
    const HRESULT result = call ? call(clsid, iid, out) : E_NOINTERFACE;
    if (SUCCEEDED(result) && out && *out && IsPersona()) reframe::AttachD3d12Interface(static_cast<IUnknown*>(*out), iid);
    return result;
}
FORWARD_HRESULT(ProxySerialize, D3D12SerializeRootSignature,
    (const D3D12_ROOT_SIGNATURE_DESC* descriptor, D3D_ROOT_SIGNATURE_VERSION version, ID3DBlob** blob, ID3DBlob** error),
    (descriptor, version, blob, error))
FORWARD_HRESULT(ProxySerializeVersioned, D3D12SerializeVersionedRootSignature,
    (const D3D12_VERSIONED_ROOT_SIGNATURE_DESC* descriptor, ID3DBlob** blob, ID3DBlob** error),
    (descriptor, blob, error))
FORWARD_HRESULT(ProxyDeserialize, D3D12CreateRootSignatureDeserializer,
    (LPCVOID bytes, SIZE_T count, REFIID iid, void** out), (bytes, count, iid, out))
FORWARD_HRESULT(ProxyDeserializeVersioned, D3D12CreateVersionedRootSignatureDeserializer,
    (LPCVOID bytes, SIZE_T count, REFIID iid, void** out), (bytes, count, iid, out))
FORWARD_HRESULT(ProxyExperimental, D3D12EnableExperimentalFeatures,
    (UINT count, const IID* iids, void* config, UINT* sizes), (count, iids, config, sizes))
