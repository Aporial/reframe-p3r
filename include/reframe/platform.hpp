#pragma once
#include <windows.h>
#include <d3d12.h>
#include <filesystem>
#include "version.h"

namespace reframe {
inline constexpr char mod_version[] = REFRAME_VERSION_STRING;
inline constexpr char dlss_sdk[] = "310.9.16";
const std::filesystem::path& DataDirectory();
void Log(const char* format, ...) noexcept;
FARPROC SystemD3D12(const char* symbol) noexcept;
void AttachRenderer(ID3D12Device* device) noexcept;
void AttachD3d12Interface(IUnknown* object, REFIID iid) noexcept;
}
