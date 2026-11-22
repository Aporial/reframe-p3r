# Third-party notices

The [MIT License](LICENSE) applies exclusively to the original RE:FRAME source code, copyright © 2026 Aporia. Each third-party component retains its own license terms and copyright notices, including components linked into the proxy. Official upstream dependencies are fetched separately for source builds using [the pinned lock](scripts/dependencies.lock.json). The release ZIP includes the project license alongside the original vendor notices.

| Component | Version | Use and distribution |
| --- | --- | --- |
| NVIDIA DLSS and NGX | 310.9.16 | The NGX static loader is linked into the proxy. The unmodified `nvngx_dlss.dll` ships with the mod. Distribution as part of an application is subject to the full NVIDIA RTX SDK terms, including downstream protections. [License](licenses/NVIDIA.txt) |
| AMD FSR SDK | 2.4.0 | The two official signed D3D12 loader and upscaler DLLs ship unmodified. Both paths are named in the SDK's license file. The complete AMD license and third-party notices are included. [License](licenses/AMD.md) · [Third-party notices](licenses/AMD-ThirdParty.md) |
| Intel XeSS SDK | 3.0.2 | `libxess.dll` ships as an unmodified binary under Intel's Simplified Software License. The accompanying notices list the applicable terms for bundled third-party software. [License](licenses/Intel.txt) · [Third-party notices](licenses/Intel-ThirdParty.txt) |
| MinHook and HDE | 1.3.4 | Statically compiled into the proxy. Its license includes the MinHook and HDE copyright notices and redistribution conditions. [License](licenses/MinHook.txt) |
| Microsoft Visual C++ and Windows SDK | MSVC v143 and SDK 10.0.26100.0 | Build inputs under Microsoft's terms. The proxy uses the statically linked C++ runtime. XeSS may need Microsoft's separately installed x64 redistributable. The archive contains neither the SDK nor Microsoft's redistributable installer. [Microsoft redistribution guidance](https://learn.microsoft.com/en-us/cpp/windows/redistributing-visual-cpp-files?view=msvc-170) |

### NVIDIA distribution terms

The applicable NVIDIA RTX SDKs license is included in full. Keep it with the bundled runtime. The NVIDIA runtime is provided as a component of this Persona 3 Reload integration for compatible NVIDIA GPUs. The SDK license includes downstream distribution obligations in section 2 and a notification requirement before commercial release in the RTX supplement, section 4. [Pinned NVIDIA license](https://github.com/NVIDIA/DLSS/blob/v310.9.16/LICENSE.txt)

### AMD and Intel runtime terms

Keep the original DLL bytes and include the vendor notices in any package containing these runtimes. See the complete conditions in the bundled license files. [Pinned AMD license](https://github.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK/blob/v2.4.0/docs/license.md) · [Pinned Intel license](https://github.com/intel/xess/blob/v3.0.2/LICENSE.txt)
