#pragma once

#include <MinHook.h>
#include <array>
#include <mutex>
#include <utility>

namespace reframe {
// Distinct COM implementations can expose the same method. Each patched entry
// gets its own trampoline, including when a wrapper forwards to another hook.
template<typename Fn, Fn Hook>
struct Detour;

template<typename Result, typename... Args, Result (STDMETHODCALLTYPE* Hook)(Args...)>
struct Detour<Result (STDMETHODCALLTYPE*)(Args...), Hook> {
    using Fn = Result (STDMETHODCALLTYPE*)(Args...);
    static inline thread_local Fn original = nullptr;

    static bool Install(void* object, size_t slot, const char* name) {
        auto* address = (*static_cast<void***>(object))[slot];
        std::lock_guard lock(mutex_);
        for (const auto& entry : entries_) if (entry.target == address) return true;
        for (size_t i = 0; i < entries_.size(); ++i) {
            auto& entry = entries_[i];
            if (entry.target) continue;
            void* trampoline = nullptr;
            const auto created = MH_CreateHook(address, reinterpret_cast<void*>(thunks_[i]), &trampoline);
            if (created != MH_OK) {
                Log("D3D12 hook %s at %p: %s.", name, address, MH_StatusToString(created));
                return false;
            }
            entry = {address, reinterpret_cast<Fn>(trampoline)};
            const auto queued = MH_QueueEnableHook(address);
            if (queued != MH_OK) {
                Log("D3D12 hook %s: %s.", name, MH_StatusToString(queued));
                return false;
            }
            Log("D3D12 route %s implementation %p installed.", name, address);
            return true;
        }
        Log("D3D12 hook %s has too many implementations.", name);
        return false;
    }

    // Set by a discovery callback and consumed only after the hook batch succeeds.
    static bool HasTarget(void* object, size_t slot) {
        const auto* address = (*static_cast<void***>(object))[slot];
        std::lock_guard lock(mutex_);
        for (const auto& entry : entries_) if (entry.target == address) return true;
        return false;
    }

private:
    static constexpr size_t max_routes = 8;
    struct Entry { void* target = nullptr; Fn trampoline = nullptr; };
    static inline std::array<Entry, max_routes> entries_{};
    static inline std::mutex mutex_;

    template<size_t Index>
    static Result STDMETHODCALLTYPE Invoke(Args... args) {
        struct Restore {
            Fn previous;
            ~Restore() { original = previous; }
        } restore{std::exchange(original, entries_[Index].trampoline)};
        return Hook(args...);
    }

    template<size_t... Index>
    static constexpr auto Thunks(std::index_sequence<Index...>) {
        return std::array<Fn, sizeof...(Index)>{ &Invoke<Index>... };
    }
    static inline constexpr auto thunks_ = Thunks(std::make_index_sequence<max_routes>{});
};
}
