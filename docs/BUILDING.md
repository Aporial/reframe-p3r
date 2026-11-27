# Building RE:FRAME for Persona 3 Reload

## Install the tools

- Install Windows x64 and [Visual Studio 2022 or Build Tools 2022](https://visualstudio.microsoft.com/downloads/) with **Desktop development with C++**.
- Select the **MSVC v143 x64/x86 C++ build tools** and **Windows 11 SDK 10.0.26100.0**. The SDK supplies the Direct3D 12 headers, libraries, Shader Model 5 compiler (`fxc.exe`), and resource compiler (`rc.exe`).
- Use **x64 Native Tools Command Prompt for VS 2022**. Check that `cl.exe`, `fxc.exe`, `rc.exe`, `curl.exe` and Windows PowerShell resolve. The release uses C++20.
- Work in a writable copy of the repository. The build needs neither the game nor a GPU.

## Fetch and verify dependencies

```powershell
powershell -NoProfile -File scripts\Fetch-Dependencies.ps1
```

[scripts/dependencies.lock.json](../scripts/dependencies.lock.json) records the immutable Git revisions and SHA-256 values for all downloaded inputs.

| Dependency | Version | Commit |
| --- | --- | --- |
| NVIDIA DLSS / NGX | 310.9.16 | `9981c6350dc58c0ce21f024fd65e31e851c31689` |
| AMD FSR SDK | 2.4.0 | `7539ae8b6568ad29ed78a753404351daf3b5cca7` |
| Intel XeSS SDK | 3.0.2 | `8fe81bdbbaf00b3c1b733fd0d830c333dc84e6f0` |
| MinHook | 1.3.4 | `c3fcafdc10146beb5919319d0683e44e3c30d537` |

The script downloads from those revisions into `.deps`. It verifies each SHA-256 before replacing a cache entry. Build and package scripts verify the inputs again. You can provide the same external `-DependencyRoot` to all three scripts.

## Release x64

```powershell
powershell -NoProfile -File build.ps1
```

The build uses FXC to compile `shaders/motion_vectors.hlsl` and `shaders/sharpness.hlsl` as `cs_5_0` and writes the bytecode headers into the build directory. RC compiles DLL metadata from `include/reframe/version.h`. MSVC builds optimized x64 code with `/O2` and `/MT` and links MinHook and the NGX static loader. The result is `build\Release\d3d12.dll`. To put intermediates and the DLL outside the checkout, pass `-BuildDirectory`.

The optional `-GpuSmokeTest` switch builds `gpu_smoke.exe`. Pass `dlss`, `fsr` or `xess`, a directory containing the pinned runtime DLLs, and a quality index. Add `--sharpness` at the end to run CAS after the SDK dispatch. The switch compiles the fixture without running it.

`-SharpnessTests` builds `sharpness_tests.exe`. Run it without arguments for the CPU checks. Use `--warp` for software D3D12 or `--hardware` for the high-performance adapter. The GPU fixture reports the isolated CAS and copy time, CPU recording overhead, and scratch allocation at common output sizes.

Run `build.ps1 -ViewScaleTests` to compile and run the CPU regression for view-scale routing, including the city map and nested pauses.

## Release ZIP

```powershell
powershell -NoProfile -File scripts\New-Release.ps1
```

The script stages an allowlisted payload, writes a hash manifest and generates `dist\REFRAME-P3R-v1.1.4.zip` and `dist\REFRAME-P3R-v1.1.4.zip.sha256`. The archive starts at `P3R\Binaries\Win64`. Pass the same `-BuildDirectory` and `-DependencyRoot` overrides if you used them to build. To build a fresh package when a stage already exists, use a new `-OutputDirectory`.

The ZIP includes the project’s [MIT License](../LICENSE) and separate vendor notices. Review the [third-party notices](../THIRD_PARTY_NOTICES.md) before distributing the result.

Run `tests\release_package.ps1 -Archive dist\REFRAME-P3R-v1.1.4.zip` to verify the archive checksum, localized guides, manifest and uninstaller against disposable fixtures in the build directory.
