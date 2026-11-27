#include "platform.hpp"
#include "engine_variables.hpp"
#include "view_scale.hpp"
#include <MinHook.h>
#include <intrin.h>
#include <array>
#include <atomic>
#include <cstring>

namespace reframe {
namespace {
constexpr DWORD image_size = 0x16376000;
constexpr DWORD driver_ctor = 0x031d1390;
constexpr DWORD gameplay_caller = 0x03112454;
constexpr DWORD gameplay_return = 0x03112463;
constexpr DWORD view_ctor = 0x033f2cd0;
constexpr DWORD local_view_caller = 0x031f2654;
constexpr DWORD local_view_return = 0x031f2661;
constexpr DWORD pause_manager_accessor = 0x010a6970;
constexpr size_t pause_stack_offset = 0x30;
constexpr size_t view_family_offset = 0xb0;
constexpr size_t scale_driver_offset = 0xb0;
constexpr size_t anti_aliasing_method_offset = 0x15f0;
constexpr size_t primary_scale_method_offset = 0x15f4;
constexpr uint32_t temporal_anti_aliasing = 2;

using ConstructDriver = void* (*)(void*, const void*, float, bool, float);
using ConstructView = void* (*)(void*, const void*);
using GetPauseManager = const void* (*)();
ConstructDriver original_driver = nullptr;
ConstructView original_view = nullptr;
const BYTE* game_image = nullptr;
std::atomic<float> requested_percentage{0.f};

struct PauseStack {
    const uint8_t* data;
    int count;
    int capacity;
};

struct GameplayViewFamily {
    const void* family = nullptr;
    const void* driver = nullptr;
};

thread_local GameplayViewFamily sdk_view_family;

template<typename T>
T Read(const void* pointer, size_t offset = 0) {
    T value{};
    std::memcpy(&value, static_cast<const BYTE*>(pointer) + offset, sizeof(value));
    return value;
}

bool Readable(const void* pointer, size_t bytes) {
    MEMORY_BASIC_INFORMATION memory{};
    if (!pointer || !VirtualQuery(pointer, &memory, sizeof(memory)) || memory.State != MEM_COMMIT ||
        (memory.Protect & (PAGE_GUARD | PAGE_NOACCESS))) return false;
    const auto start = reinterpret_cast<uintptr_t>(pointer);
    const auto offset = start - reinterpret_cast<uintptr_t>(memory.BaseAddress);
    return offset < memory.RegionSize && bytes <= memory.RegionSize - offset;
}

bool IsTownMapActive() {
    // Match the game's live pause stack. A nested dialogue or pause screen
    // can sit above TownMap while the map's 3D view is still being drawn.
    const auto* manager = reinterpret_cast<GetPauseManager>(game_image + pause_manager_accessor)();
    if (!Readable(manager, pause_stack_offset + sizeof(PauseStack))) return false;
    const auto stack = Read<PauseStack>(manager, pause_stack_offset);
    if (stack.count <= 0 || stack.count > stack.capacity || stack.count > 64 ||
        !Readable(stack.data, static_cast<size_t>(stack.count))) return false;
    return ContainsTownMapPause({stack.data, static_cast<size_t>(stack.count)});
}

void* CreateSceneView(void* object, const void* options) {
    const bool local_player = _ReturnAddress() == game_image + local_view_return;
    auto* view = original_view(object, options);
    if (!view || !local_player || !sdk_view_family.driver) return view;
    const auto* family = Read<const void*>(options, view_family_offset);
    if (family != sdk_view_family.family ||
        Read<const void*>(family, scale_driver_offset) != sdk_view_family.driver) return view;
    sdk_view_family = {};

    // Use the same field as the engine's TAAU branch before this view is
    // queued to the renderer. All other views keep their constructor result.
    const auto method = Read<PrimaryScaleMethod>(view, primary_scale_method_offset);
    const auto aa = Read<uint32_t>(view, anti_aliasing_method_offset);
    const bool enable_taau = aa == temporal_anti_aliasing && method == PrimaryScaleMethod::Spatial;
    if (enable_taau) {
        const auto temporal = PrimaryScaleMethod::Temporal;
        std::memcpy(static_cast<BYTE*>(view) + primary_scale_method_offset, &temporal, sizeof(temporal));
    }
    static std::atomic_bool reported{false};
    if (!reported.exchange(true, std::memory_order_relaxed))
        Log("SDK gameplay view matched; AA method %u, primary scale method %u -> %u.", aa,
            static_cast<unsigned>(method), static_cast<unsigned>(enable_taau ? PrimaryScaleMethod::Temporal : method));
    return view;
}

void* CreateScreenPercentageDriver(void* object, const void* family, float stock, bool screen_percentage, float upper) {
    if (_ReturnAddress() != game_image + gameplay_return)
        return original_driver(object, family, stock, screen_percentage, upper);

    const auto requested = GameplayScreenPercentage();
    const bool town_map = HasReducedSdkScale(requested) && screen_percentage && IsTownMapActive();
    const bool use_sdk = GameplayUsesSdkScale(requested, screen_percentage, town_map);
    const auto target = use_sdk ? requested / 100.f : stock;
    auto* driver = original_driver(object, family, target, screen_percentage, use_sdk ? target : upper);
    sdk_view_family = use_sdk ? GameplayViewFamily{family, driver} : GameplayViewFamily{};

    static thread_local bool was_town_map = false;
    static thread_local float reported_request = 0.f;
    if (town_map && !was_town_map)
        Log("TownMap pause active; stock view scale %.3f%% and stock upscale method.", 100.f * stock);
    if (use_sdk && (requested != reported_request || was_town_map))
        Log("Gameplay view family uses %.3f%% (stock %.3f%%).", 100.f * target, 100.f * stock);
    reported_request = use_sdk ? requested : 0.f;
    was_town_map = town_map;
    return driver;
}

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

bool InstallGameplayScreenPercentage() {
    const auto* base = GameImage();
    if (!base) return false;
    const auto matches = [base](DWORD rva, auto bytes) {
        return std::memcmp(base + rva, bytes.data(), bytes.size()) == 0;
    };
    // Check the viewport driver and local-player view call sites, the view's TAA
    // fields, and the game's pause stack accessor on this Steam build.
    if (!matches(driver_ctor, std::array<unsigned char, 13>{
            0xf3, 0x0f, 0x10, 0x44, 0x24, 0x28, 0x48, 0x8d, 0x05, 0x23, 0x3d, 0x89, 0x01}) ||
        !matches(gameplay_caller, std::array<unsigned char, 15>{
            0x48, 0x8d, 0x95, 0x70, 0x02, 0x00, 0x00, 0x48, 0x8b, 0xc8, 0xe8, 0x2d, 0xef, 0x0b, 0x00}) ||
        !matches(view_ctor, std::array<unsigned char, 16>{
            0x48, 0x89, 0x5c, 0x24, 0x18, 0x55, 0x56, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57}) ||
        !matches(local_view_caller, std::array<unsigned char, 13>{
            0x48, 0x8d, 0x54, 0x24, 0x50, 0x48, 0x8b, 0xc8, 0xe8, 0x6f, 0x06, 0x20, 0x00}) ||
        !matches(0x03112472, std::array<unsigned char, 7>{0x48, 0x89, 0x9d, 0x20, 0x03, 0x00, 0x00}) ||
        !matches(pause_manager_accessor, std::array<unsigned char, 11>{
            0x40, 0x57, 0x48, 0x83, 0xec, 0x20, 0xe8, 0x15, 0x43, 0x18, 0x00}) ||
        !matches(0x010a7370, std::array<unsigned char, 14>{
            0x48, 0x8b, 0x41, 0x30, 0x4c, 0x63, 0x41, 0x38, 0x41, 0x0f, 0xb6, 0x4c, 0x00, 0xff}) ||
        !matches(0x04377520, std::array{'E','A','p','p','P','a','u','s','e','T','y','p','e',':',':','T','o','w','n','M','a','p','\0'}) ||
        !matches(0x043774b8, std::array<unsigned char, 8>{TownMapPauseType}) ||
        !matches(0x033f3bd7, std::array<unsigned char, 7>{0x83, 0xbb, 0xf0, 0x15, 0x00, 0x00, 0x02}) ||
        !matches(0x033f3c18, std::array<unsigned char, 10>{0xc7, 0x83, 0xf4, 0x15, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}))
        return false;

    auto* driver = const_cast<BYTE*>(base + driver_ctor);
    auto* view = const_cast<BYTE*>(base + view_ctor);
    void* driver_trampoline = nullptr;
    void* view_trampoline = nullptr;
    if (MH_CreateHook(driver, reinterpret_cast<void*>(&CreateScreenPercentageDriver), &driver_trampoline) != MH_OK)
        return false;
    if (MH_CreateHook(view, reinterpret_cast<void*>(&CreateSceneView), &view_trampoline) != MH_OK) {
        MH_RemoveHook(driver);
        return false;
    }

    game_image = base;
    original_driver = reinterpret_cast<ConstructDriver>(driver_trampoline);
    original_view = reinterpret_cast<ConstructView>(view_trampoline);
    if (MH_QueueEnableHook(driver) == MH_OK && MH_QueueEnableHook(view) == MH_OK && MH_ApplyQueued() == MH_OK)
        return true;
    MH_RemoveHook(view);
    MH_RemoveHook(driver);
    return false;
}

bool SetGameplayScreenPercentage(float percentage) {
    return requested_percentage.exchange(percentage, std::memory_order_relaxed) != percentage;
}

float GameplayScreenPercentage() {
    return requested_percentage.load(std::memory_order_relaxed);
}

EngineVariables BindEngineVariables() {
    const auto base = GameImage();
    if (!base) return {};
    // The typed registration accessor stores its two-copy data pointer at each RVA.
    return {Bind<int32_t, 0x058920a8>(base), Bind<float, 0x05892350>(base), Bind<float, 0x0571ec48>(base),
            Bind<int32_t, 0x057dc7c0>(base), Bind<int32_t, 0x0571ede0>(base), Bind<float, 0x058f51b8>(base)};
}
}
