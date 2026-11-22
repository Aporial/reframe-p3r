#include "platform.hpp"
#include <cstdarg>
#include <cstdio>
#include <mutex>

namespace {
HMODULE self = nullptr;
}

BOOL WINAPI DllMain(HINSTANCE module, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) self = module;
    return TRUE;
}

namespace reframe {
const std::filesystem::path& DataDirectory() {
    static const auto directory = [] {
        wchar_t name[32768]{};
        const auto count = GetModuleFileNameW(self, name, static_cast<DWORD>(std::size(name)));
        if (!count || count >= std::size(name)) return std::filesystem::path();
        return std::filesystem::path(name).parent_path() / L"ReFrame";
    }();
    return directory;
}

void Log(const char* format, ...) noexcept {
    try {
        static std::mutex mutex;
        std::lock_guard lock(mutex);
        const auto& directory = DataDirectory();
        if (directory.empty()) return;
        std::error_code error;
        std::filesystem::create_directories(directory, error);
        FILE* file = nullptr;
        if (_wfopen_s(&file, (directory / L"ReFrame.log").c_str(), L"a") != 0) return;
        SYSTEMTIME time{};
        GetLocalTime(&time);
        std::fprintf(file, "%02u:%02u:%02u.%03u ", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
        va_list args;
        va_start(args, format);
        std::vfprintf(file, format, args);
        va_end(args);
        std::fputc('\n', file);
        std::fclose(file);
    } catch (...) {
    }
}

FARPROC SystemD3D12(const char* symbol) noexcept {
    static const HMODULE library = [] {
        wchar_t name[MAX_PATH]{};
        const UINT count = GetSystemDirectoryW(name, MAX_PATH);
        if (!count || count + 11 >= MAX_PATH) return HMODULE{};
        wcscat_s(name, L"\\d3d12.dll");
        return LoadLibraryExW(name, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }();
    return library ? GetProcAddress(library, symbol) : nullptr;
}
}
