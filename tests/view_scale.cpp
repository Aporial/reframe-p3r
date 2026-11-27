#include "view_scale.hpp"
#include "town_map_view.hpp"
#include <array>
#include <limits>

using reframe::GameplayUsesSdkScale;
using reframe::ContainsTownMapPause;

static_assert(!GameplayUsesSdkScale(50.f, false, false));
static_assert(!GameplayUsesSdkScale(0.f, true, false));
static_assert(GameplayUsesSdkScale(100.f, true, false));
static_assert(GameplayUsesSdkScale(66.6667f, true, false));
static_assert(!GameplayUsesSdkScale(66.6667f, true, true));
static_assert(GameplayUsesSdkScale(66.6667f, true, true, true));
static_assert(GameplayUsesSdkScale(58.854f, true, true, true));
static_assert(!GameplayUsesSdkScale(50.f, false, true, true));
static_assert(!GameplayUsesSdkScale(0.f, true, true, true));
static_assert(!GameplayUsesSdkScale(-1.f, true, false));
static_assert(!GameplayUsesSdkScale(101.f, true, false));
static_assert(!GameplayUsesSdkScale(std::numeric_limits<float>::quiet_NaN(), true, false));
static_assert(!GameplayUsesSdkScale(std::numeric_limits<float>::infinity(), true, false));

static_assert(!reframe::HasReducedSdkScale(0.f) && !reframe::HasReducedSdkScale(100.f));
static_assert(reframe::HasReducedSdkScale(43.49f));

static_assert(!ContainsTownMapPause({}));
static_assert(!ContainsTownMapPause(std::array<uint8_t, 4>{0, 1, 3, 4}));
static_assert(ContainsTownMapPause(std::array<uint8_t, 2>{0, 2}));
static_assert(ContainsTownMapPause(std::array<uint8_t, 3>{0, 2, 3}));
static_assert(GameplayUsesSdkScale(66.6667f, true, ContainsTownMapPause(std::array<uint8_t, 2>{0, 1})));
static_assert(!GameplayUsesSdkScale(66.6667f, true, ContainsTownMapPause(std::array<uint8_t, 2>{0, 2})));

constexpr bool CorrectedTownMapSizes(reframe::Size render, reframe::Size old_buffer, reframe::Size display) {
    reframe::TownMapViewConstants view{};
    view.view_size = reframe::ExtentAndInverse(render);
    view.buffer_size = reframe::ExtentAndInverse(old_buffer);
    return reframe::CorrectTownMapView(view, render, display) &&
           view.view_size == reframe::ExtentAndInverse(display) && view.buffer_size == view.view_size;
}

static_assert(CorrectedTownMapSizes({1280, 720}, {1280, 720}, {1920, 1080}));
static_assert(CorrectedTownMapSizes({1130, 636}, {1132, 636}, {1920, 1080}));
static_assert(CorrectedTownMapSizes({1280, 720}, {1920, 1080}, {1920, 1080}));
static_assert(!CorrectedTownMapSizes({1920, 1080}, {1920, 1080}, {1920, 1080}));
static_assert(!CorrectedTownMapSizes({1280, 720}, {1024, 720}, {1920, 1080}));

constexpr bool TownMapWatchFallsBackWithoutCorrectedDraws() {
    using reframe::TownMapPatchWatch;
    TownMapPatchWatch watch;
    for (uint32_t view = 0; view < TownMapPatchWatch::MaxUncorrectedViews; ++view)
        if (!watch.Observe(0)) return false;
    return !watch.Observe(0);
}

constexpr bool TownMapWatchAllowsOngoingCorrections() {
    using reframe::TownMapPatchWatch;
    TownMapPatchWatch watch;
    for (uint32_t view = 0; view < TownMapPatchWatch::MaxUncorrectedViews; ++view)
        if (!watch.Observe(0)) return false;
    for (uint32_t draw = 1; draw <= 2; ++draw) {
        if (!watch.Observe(draw)) return false;
        for (uint32_t view = 0; view < TownMapPatchWatch::MaxUncorrectedViews; ++view)
            if (!watch.Observe(draw)) return false;
    }
    return !watch.Observe(2);
}

static_assert(TownMapWatchFallsBackWithoutCorrectedDraws());
static_assert(TownMapWatchAllowsOngoingCorrections());

int main() { return 0; }
