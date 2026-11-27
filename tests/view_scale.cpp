#include "view_scale.hpp"
#include <array>
#include <limits>

using reframe::GameplayUsesSdkScale;
using reframe::ContainsTownMapPause;

static_assert(!GameplayUsesSdkScale(50.f, false, false));
static_assert(!GameplayUsesSdkScale(0.f, true, false));
static_assert(GameplayUsesSdkScale(100.f, true, false));
static_assert(GameplayUsesSdkScale(66.6667f, true, false));
static_assert(!GameplayUsesSdkScale(66.6667f, true, true));
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

int main() { return 0; }
