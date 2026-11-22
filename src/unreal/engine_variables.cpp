#include "platform.hpp"
#include "engine_variables.hpp"

namespace reframe {
namespace {
constexpr DWORD image_size = 0x16376000;

const BYTE* GameImage() {
    const auto base = reinterpret_cast<const BYTE*>(GetModuleHandleW(nullptr));
    if (!base) return nullptr;
    const auto& dos = *reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    if (dos.e_magic != IMAGE_DOS_SIGNATURE || dos.e_lfanew < static_cast<LONG>(sizeof(dos)) ||
        dos.e_lfanew > 0x1000) return nullptr;
    const auto& pe = *reinterpret_cast<const IMAGE_NT_HEADERS64*>(base + dos.e_lfanew);
    if (pe.Signature != IMAGE_NT_SIGNATURE || pe.FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 ||
        pe.FileHeader.TimeDateStamp != 0x69ce3439 || pe.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC ||
        pe.OptionalHeader.SizeOfImage != image_size || pe.OptionalHeader.CheckSum != 0x15d4a63a) return nullptr;
    return base;
}

template<typename T, DWORD slot>
T* Bind(const BYTE* base) {
    static_assert(slot % alignof(T*) == 0 && slot <= image_size - sizeof(T*));
    const auto values = *reinterpret_cast<T* const*>(base + slot);
    MEMORY_BASIC_INFORMATION region{};
    if (!values || reinterpret_cast<uintptr_t>(values) % alignof(T) ||
        !VirtualQuery(values, &region, sizeof(region)) || region.State != MEM_COMMIT ||
        region.Type != MEM_PRIVATE || region.Protect != PAGE_READWRITE) return nullptr;
    const auto offset = reinterpret_cast<uintptr_t>(values) - reinterpret_cast<uintptr_t>(region.BaseAddress);
    return offset <= region.RegionSize && region.RegionSize - offset >= 2 * sizeof(T) ? values : nullptr;
}
}

EngineVariables BindEngineVariables() {
    const auto base = GameImage();
    if (!base) return {};
    // The typed registration accessor stores its two-copy data pointer at each RVA.
    return {Bind<int32_t, 0x058ef070>(base), Bind<int32_t, 0x058920a8>(base),
            Bind<float, 0x05892350>(base), Bind<float, 0x0571ec48>(base),
            Bind<int32_t, 0x057dc7c0>(base), Bind<int32_t, 0x0571ede0>(base), Bind<float, 0x058f51b8>(base)};
}
}
