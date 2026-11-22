#pragma once
#include "frame.hpp"
#include <array>

namespace reframe {
class TemporalSettings {
public:
    void Discover();
    bool Apply(Technology technology, float percentage = 100.0f, int samples = 16);
    bool CanScale() const { return percentage_.values && CanOverride(); }
    bool SetScreenPercentage(float percentage);
    bool RestoreScreenPercentage();
    bool MatchesScreenPercentage(Size display, Size pass) const;
    void Benchmark(bool enable);
private:
    template<typename T>
    struct Override {
        T* values = nullptr;
        std::array<T, 2> original{};
        T applied{};
        bool active = false;
        bool Set(T next);
        bool Restore();
    };

    bool CanOverride() const { return upsampling_.values && samples_ && mip_bias_.values; }
    Override<int32_t> upsampling_;
    Override<float> mip_bias_;
    Override<float> percentage_;
    int32_t* samples_ = nullptr;
    int32_t applied_samples_ = 0;
    bool overriding_samples_ = false;
    int32_t* sync_interval_ = nullptr;
    int32_t* vsync_ = nullptr;
    float* max_fps_ = nullptr;
    bool benchmark_discovered_ = false;
};
}
