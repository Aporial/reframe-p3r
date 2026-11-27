#pragma once
#include "frame.hpp"
#include <array>
#include <cstddef>

namespace reframe {
struct TownMapViewConstants {
    std::array<std::byte, 0x820> prefix;
    std::array<float, 4> view_size;
    std::array<std::byte, 0x10> between_sizes;
    std::array<float, 4> buffer_size;
    std::array<std::byte, 5220 - 0x850> suffix;
};

static_assert(offsetof(TownMapViewConstants, view_size) == 0x820);
static_assert(offsetof(TownMapViewConstants, buffer_size) == 0x840);
static_assert(sizeof(TownMapViewConstants) == 5220);

constexpr std::array<float, 4> ExtentAndInverse(Size size) {
    return {static_cast<float>(size.width), static_cast<float>(size.height),
            1.f / size.width, 1.f / size.height};
}

constexpr bool MatchesInverseExtent(float x, float y, Size size) {
    const float dx = x * size.width - 1.f;
    const float dy = y * size.height - 1.f;
    return dx >= -.002f && dx <= .002f && dy >= -.002f && dy <= .002f;
}

constexpr bool CorrectTownMapView(TownMapViewConstants& view, Size render, Size display) {
    if (!render.width || !render.height || render.width >= display.width || render.height >= display.height ||
        view.view_size[0] != render.width || view.view_size[1] != render.height ||
        !MatchesInverseExtent(view.view_size[2], view.view_size[3], render)) return false;

    if (!(view.buffer_size[0] >= render.width && view.buffer_size[0] <= display.width &&
          view.buffer_size[1] >= render.height && view.buffer_size[1] <= display.height)) return false;
    const Size buffer{static_cast<UINT>(view.buffer_size[0]), static_cast<UINT>(view.buffer_size[1])};
    if (view.buffer_size[0] != buffer.width || view.buffer_size[1] != buffer.height ||
        !MatchesInverseExtent(view.buffer_size[2], view.buffer_size[3], buffer)) return false;

    view.view_size = ExtentAndInverse(display);
    view.buffer_size = view.view_size;
    return true;
}
}
