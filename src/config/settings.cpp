#include "settings.hpp"
#include "platform.hpp"

namespace reframe {
GraphicsConfig LoadGraphicsConfig(const std::array<bool, 4>& available, Technology default_vendor) {
    const auto& directory = DataDirectory();
    auto file = directory / L"Settings.ini";
    if (!std::filesystem::exists(file))
        file = directory.parent_path() / L"P3RGraphicsEnhanced" / L"Settings.ini";
    const auto read = [&](const wchar_t* key, UINT fallback) {
        return GetPrivateProfileIntW(L"Graphics", key, static_cast<INT>(fallback), file.c_str());
    };
    GraphicsConfig config{{Technology::Taa}, default_vendor};
    const auto last = read(L"LastBackend", static_cast<UINT>(default_vendor));
    if (last > 0 && last < available.size() && available[last])
        config.last_vendor = static_cast<Technology>(last);
    const auto active = read(L"Technology", static_cast<UINT>(config.last_vendor));
    if (active < available.size() && available[active])
        config.settings.technology = static_cast<Technology>(active);
    if (config.settings.technology != Technology::Taa) config.last_vendor = config.settings.technology;
    const auto quality = read(L"Quality", 0);
    if (quality < static_cast<UINT>(Quality::Count)) config.settings.quality = static_cast<Quality>(quality);
    const auto model = read(L"Model", 0);
    if (model <= static_cast<UINT>(Model::L)) config.settings.model = static_cast<Model>(model);
    return config;
}

bool SaveGraphicsConfig(Settings settings, Technology last_vendor) {
    const auto& directory = DataDirectory();
    if (directory.empty()) return false;
    const auto file = directory / L"Settings.ini";
    const auto write = [&](const wchar_t* key, auto value) {
        return WritePrivateProfileStringW(L"Graphics", key, std::to_wstring(static_cast<UINT>(value)).c_str(), file.c_str());
    };
    return (write(L"Technology", settings.technology) & write(L"LastBackend", last_vendor) &
            write(L"Quality", settings.quality) & write(L"Model", settings.model)) != 0;
}

bool DiagnosticEnabled(const wchar_t* key) {
    return GetPrivateProfileIntW(L"Diagnostic", key, 0, (DataDirectory() / L"Diagnostic.ini").c_str()) == 1;
}

bool BenchmarkEnabled() {
    return GetPrivateProfileIntW(L"Benchmark", L"Uncapped", 0, (DataDirectory() / L"Benchmark.ini").c_str()) == 1;
}
}
