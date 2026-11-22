#include "platform.hpp"
#include "temporal_settings.hpp"
#include "engine_variables.hpp"

namespace reframe {
namespace {
constexpr bool MatchesScaledViewport(Size display, Size pass, float percentage) {
    if (!display.width || !display.height || !pass.width || !pass.height ||
        !(percentage >= 25 && percentage <= 400)) return false;
    // The engine quantizes the view rect. Compare both axes with room for
    // rounding up to the next eight-pixel boundary.
    const auto matches_axis = [percentage](UINT full, UINT actual) {
        const double difference = actual - full * (static_cast<double>(percentage) / 100.0);
        return difference >= -7 && difference <= 7;
    };
    return matches_axis(display.width, pass.width) && matches_axis(display.height, pass.height);
}

static_assert(MatchesScaledViewport({1920, 1080}, {1440, 810}, 75));
static_assert(MatchesScaledViewport({1920, 1080}, {1440, 816}, 75));
static_assert(MatchesScaledViewport({1920, 1080}, {3840, 2160}, 200));
static_assert(!MatchesScaledViewport({1920, 1080}, {1440, 810}, 100));
static_assert(!MatchesScaledViewport({1920, 1080}, {1440, 720}, 75));
}
template<typename T>
bool TemporalSettings::Override<T>::Set(T next) {
    if (!values) return false;
    const bool changed = !active || next != applied;
    for (size_t i = 0; i < original.size(); ++i) {
        if (!active || values[i] != applied) original[i] = values[i];
        values[i] = next;
    }
    applied = next;
    active = true;
    return changed;
}

template<typename T>
bool TemporalSettings::Override<T>::Restore() {
    if (!active) return false;
    for (size_t i = 0; i < original.size(); ++i)
        if (values[i] == applied) values[i] = original[i];
    active = false;
    return true;
}

void TemporalSettings::Discover() {
    const auto engine = BindEngineVariables();
    upsampling_.values = engine.temporal_upsampling;
    samples_ = engine.temporal_samples;
    mip_bias_.values = engine.texture_mip_offset;
    percentage_.values = engine.screen_percentage;
    sync_interval_ = engine.sync_interval;
    vsync_ = engine.vsync;
    max_fps_ = engine.max_fps;
    Log("UE typed variables: temporal controls %s; screen percentage %s.",
        CanOverride() ? "bound" : "unavailable", percentage_.values ? "bound" : "unavailable");
}

bool TemporalSettings::SetScreenPercentage(float percentage) {
    return percentage_.Set(percentage);
}

bool TemporalSettings::RestoreScreenPercentage() {
    if (!percentage_.Restore()) return false;
    Log("Restored game r.ScreenPercentage %.2f.", percentage_.values[1]);
    return true;
}

bool TemporalSettings::MatchesScreenPercentage(Size display, Size pass) const {
    if (!percentage_.values) return false;
    for (size_t i = 0; i < percentage_.original.size(); ++i) {
        if (MatchesScaledViewport(display, pass, percentage_.values[i]) ||
            (percentage_.active && MatchesScaledViewport(display, pass, percentage_.original[i]))) return true;
    }
    return false;
}

void TemporalSettings::Benchmark(bool enable) {
    if (!enable) return;
    if (!benchmark_discovered_) {
        benchmark_discovered_ = true;
        Log("Benchmark mode (process only): rhi.SyncInterval %s, r.VSync %s, t.MaxFPS %s.",
            sync_interval_ ? "0" : "unavailable", vsync_ ? "0" : "unavailable", max_fps_ ? "0" : "unavailable");
    }
    if (sync_interval_) sync_interval_[0] = sync_interval_[1] = 0;
    if (vsync_) vsync_[0] = vsync_[1] = 0;
    if (max_fps_) max_fps_[0] = max_fps_[1] = 0;
}

bool TemporalSettings::Apply(Technology technology, float percentage, int samples) {
    if (technology == Technology::Taa) {
        const bool restored_scale = RestoreScreenPercentage();
        const bool restored_bias = mip_bias_.Restore();
        const bool restored_upsampling = upsampling_.Restore();
        if (!overriding_samples_) return restored_scale || restored_bias || restored_upsampling;
        if (samples_[1] == applied_samples_) samples_[1] = samples_[0];
        overriding_samples_ = false;
        Log("Restored game temporal settings (upsampling %d, samples %d, mip bias %.3f).",
            upsampling_.values[1], samples_[1], mip_bias_.values[1]);
        return true;
    }

    if (!CanOverride()) return false;
    // UE TAAU already adds log2(render/display). FSR and DLSS need the
    // extra -1 LOD unit. XeSS uses the engine's scale term as-is.
    const float offset = technology == Technology::Xess ? 0.0f : -1.0f;
    const bool scale_changed = SetScreenPercentage(percentage);
    const bool bias_changed = mip_bias_.Set(offset);
    const bool upsampling_changed = upsampling_.Set(1);
    const bool samples_changed = !overriding_samples_ || applied_samples_ != samples;
    // The game keeps ownership of the game-thread sample count (slot zero).
    samples_[1] = samples;
    applied_samples_ = samples;
    overriding_samples_ = true;
    const bool changed = scale_changed || bias_changed || upsampling_changed || samples_changed;
    if (changed) Log("UE temporal settings requested: Halton, %d base phases, mip bias offset %.1f, screen percentage %.3f.",
                     samples, offset, percentage);
    return changed;
}
}
