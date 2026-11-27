#pragma once
#include <algorithm>
#include <cstdint>
#include <span>

namespace reframe {
constexpr uint8_t TownMapPauseType = 2;

constexpr bool ContainsTownMapPause(std::span<const uint8_t> stack) {
    return std::find(stack.begin(), stack.end(), TownMapPauseType) != stack.end();
}

constexpr bool HasReducedSdkScale(float percentage) { return percentage > 0.f && percentage < 100.f; }

constexpr bool GameplayUsesSdkScale(float requested_percentage, bool gameplay_viewport, bool town_map_family,
                                   bool allow_town_map = false) {
    return gameplay_viewport && (!town_map_family || allow_town_map) &&
           requested_percentage > 0.f && requested_percentage <= 100.f;
}

struct TownMapPatchWatch {
    // Give queued render work a chance to produce a corrected map background.
    static constexpr uint32_t MaxUncorrectedViews = 90;

    constexpr bool Observe(uint32_t progress) {
        if (progress != last_progress) {
            last_progress = progress;
            uncorrected_views = 0;
        } else {
            ++uncorrected_views;
        }
        return uncorrected_views <= MaxUncorrectedViews;
    }

private:
    uint32_t last_progress = 0;
    uint32_t uncorrected_views = 0;
};

enum class PrimaryScaleMethod : unsigned { Spatial, Temporal, RawOutput };
}
