#pragma once
#include "frame.hpp"
#include <array>

namespace reframe {
struct GraphicsConfig {
    Settings settings{Technology::Taa};
    Technology last_vendor;
};

GraphicsConfig LoadGraphicsConfig(const std::array<bool, 4>& available, Technology default_vendor);
bool SaveGraphicsConfig(Settings settings, Technology last_vendor);
bool DiagnosticEnabled(const wchar_t* key);
bool BenchmarkEnabled();
}
