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

constexpr bool GameplayUsesSdkScale(float requested_percentage, bool gameplay_viewport, bool town_map_family) {
    return gameplay_viewport && !town_map_family && requested_percentage > 0.f && requested_percentage <= 100.f;
}

enum class PrimaryScaleMethod : unsigned { Spatial, Temporal, RawOutput };
}
