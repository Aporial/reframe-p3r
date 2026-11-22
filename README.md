# RE:FRAME — Persona 3 Reload

![Release 1.0.0](https://img.shields.io/badge/release-1.0.0-2469ad) ![Windows x64](https://img.shields.io/badge/platform-Windows%20x64-2469ad) ![DirectX 12](https://img.shields.io/badge/renderer-DirectX%2012-107c10) [![License: MIT](https://img.shields.io/badge/license-MIT-blue)](LICENSE)

Modern anti-aliasing and upscaling for Persona 3 Reload, seamlessly integrated into the game's native graphics menu.

- **Native in-game graphics settings** for NVIDIA DLAA/DLSS, AMD FSR and Intel XeSS, plus the game's Original TAA.
- **Automatic render scale management** and **automatic language detection** for all 13 supported interface languages.
- **Launch directly from Steam.** No ReShade, overlays, mod loaders or external applications are needed to run the mod.

![Persona 3 Reload's native graphics settings with RE:FRAME options](docs/images/settings.png)

By **Aporia**. This is a standalone mod for Persona 3 Reload. Future RE:FRAME mods for other games will be separate projects.

[Modes](#modes) · [DLSS models](#dlss-models) · [Requirements](#requirements) · [Installation](#installation) · [Settings](#settings) · [Limitations](#limitations) · [Build](#build) · [License](#license)

## Modes

Availability depends on the GPU, driver and the modes reported by the runtime. Native AA renders at 100%. Each upscaler queries its own recommended internal resolution.

| Mode | NVIDIA DLSS | AMD FSR | Intel XeSS |
| --- | --- | --- | --- |
| Native AA | DLAA | Native AA | Native AA |
| Ultra Quality Plus | — | — | When available |
| Ultra Quality | — | — | When available |
| Quality | ✓ | ✓ | ✓ |
| Balanced | ✓ | ✓ | ✓ |
| Performance | ✓ | ✓ | ✓ |

**Original TAA** keeps the game's built-in anti-aliasing and manual Rendering Scale. There are up to 14 SDK modes across the three vendors, plus Original TAA.

## DLSS models

| Setting | Behavior |
| --- | --- |
| **Auto (Recommended)** | Requests K for DLAA, Quality and Balanced, and M for Performance. |
| **DLSS 4 — Standard (K)** | Requests DLSS 4's transformer model. |
| **DLSS 4.5 — Optimized (M)** | Requests the DLSS 4.5 model recommended for Performance. |
| **DLSS 4.5 — Detail (L)** | Requests the DLSS 4.5 model NVIDIA recommends for 4K Ultra Performance. RE:FRAME exposes the model separately from quality modes. |

M and L can cost more GPU time on GeForce RTX 20 and 30 series. Model choices are requests to the DLSS runtime. [NVIDIA DLSS 4.5 model guidance](https://www.nvidia.com/en-us/geforce/news/dlss-4-5-super-resolution-available-now/)

## Requirements

- A legally purchased copy of **Persona 3 Reload on Steam**, app ID **2161700**, build **22672075**.
- Windows 10 or 11 x64, DirectX 12 and a driver that supports the backend you select.
- XeSS requires the Microsoft Visual C++ x64 Redistributable **14.40.33810 or later**. Intel lists Windows 10 build 19043 or later and Windows 11 build 22000 or later for this SDK. [Intel XeSS deployment requirements](https://github.com/intel/xess/blob/v3.0.2/doc/xess_sr_developer_guide_english.md#deployment) · [Microsoft Visual C++ Redistributable](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist)

| Backend | GPU requirements |
| --- | --- |
| NVIDIA DLAA / DLSS | Compatible NVIDIA GeForce RTX GPU and a driver accepted by NGX. [NVIDIA DLSS](https://www.nvidia.com/en-us/geforce/technologies/dlss/) |
| AMD FSR | FSR 4.1.2 on hardware reported as compatible by the AMD SDK, including supported Radeon RX 6000, 7000 and 9000 series. On other compatible GPUs the mod can use the cross-vendor FSR 3.1.6 path. [AMD FSR SDK](https://gpuopen.com/amd-fsr-sdk/) |
| Intel XeSS | D3D12-capable Intel Iris Xe or Arc hardware, or a compatible GPU from another vendor with Shader Model 6.4 and DP4a acceleration. [Intel XeSS requirements](https://github.com/intel/xess/blob/v3.0.2/doc/xess_sr_developer_guide_english.md#requirements) |

See the [compatibility notes](docs/COMPATIBILITY.md) for the distinction between SDK requirements and mod testing.

### Interface languages

English, French, Italian, German, Spanish (Spain), Japanese, Korean, Polish, Portuguese (Brazil), Russian, Simplified Chinese, Traditional Chinese and Turkish. The mod follows the game's interface language and uses English as a fallback. [Persona 3 Reload on Steam](https://store.steampowered.com/app/2161700/Persona_3_Reload/)

## Installation

1. Close the game. In Steam, open **Properties → Installed Files → Browse**.
2. Download `REFRAME-P3R-v1.0.0.zip` and its SHA-256 file from the release. Compare the hash if desired.
3. Check `P3R\Binaries\Win64` for an existing `d3d12.dll`. Remove a previous installation with its own uninstaller or back up any third-party proxy before proceeding.
4. Extract the ZIP directly into the game root, the folder that already contains `P3R`. Launch the game normally from Steam.

### Updating

Close the game and back up its existing mod directory. Run the previous version's removal script, then extract the new ZIP. The new release reads `P3RGraphicsEnhanced\Settings.ini` if `ReFrame\Settings.ini` is absent. Keep the older settings file through the first launch. The next settings change writes to the new directory.

### Uninstalling

Close the game. In `P3R\Binaries\Win64\ReFrame`, run `.\Remove-ReFrame.ps1`. Add `-WhatIf` to preview. The script checks the manifest, allowed paths and hashes, and keeps generated settings and logs.

## Settings

Open the native **Graphics** page. The mod adds rows directly below **Rendering Scale**.

| Setting | What it controls |
| --- | --- |
| **Anti-Aliasing Technology** | Original TAA, NVIDIA DLSS, AMD FSR or Intel XeSS, where supported. |
| **Quality Mode** | Native AA or an available upscaling preset. Only shown for an SDK backend. |
| **DLSS Model** | Auto, K, M or L. Only shown for NVIDIA DLSS. |
| **Rendering Scale** | Editable for Original TAA. For SDK modes it shows the selected target and the mod manages it. |

Confirm or cancel changes through the game's normal prompt. Returning to Original TAA lets you confirm a new manual scale in the same transaction.

## Limitations

- Support is scoped to the listed Steam build. A game update can change the menu or renderer interfaces the mod uses.
- Original TAA handles unrecognized and secondary passes, including planar reflections. RE:FRAME focuses on anti-aliasing and upscaling.
- Earlier development builds verified DLAA, DLSS, FSR 3.1.6 and cross-vendor XeSS on an RTX 5060. Version 1.0.0 has been compiled and statically checked. In-game validation for this release, Radeon FSR 4 and Intel-optimized XeSS is pending.

## Troubleshooting

If the menu rows are missing, check the Steam build and verify that `d3d12.dll` is next to `P3R.exe`. If a backend is absent, check its runtime, GPU and driver. The mod logs detection, fallback and the actual viewport to `P3R\Binaries\Win64\ReFrame\ReFrame.log`. See [Troubleshooting](docs/TROUBLESHOOTING.md).

## Build

Open the **x64 Native Tools Command Prompt for Visual Studio 2022** with the C++ workload and Windows SDK installed. Then run:

```powershell
powershell -NoProfile -File scripts\Fetch-Dependencies.ps1
powershell -NoProfile -File build.ps1
powershell -NoProfile -File scripts\New-Release.ps1
```

See [Building](docs/BUILDING.md), [Architecture](docs/ARCHITECTURE.md), [Changelog](CHANGELOG.md) and [Contributing](CONTRIBUTING.md).

## License

Copyright © 2026 Aporia. RE:FRAME’s original source code is licensed under the [MIT License](LICENSE). Bundled SDK runtimes, the NVIDIA NGX loader, MinHook/HDE, and other third-party components retain their respective terms and notices. The game and its assets remain subject to their owners’ terms. See [Third-party notices](THIRD_PARTY_NOTICES.md) and [vendor licenses](licenses/).

## Disclaimer

RE:FRAME is an independent, unofficial fan-made project.
It is not affiliated with, endorsed by, or sponsored by ATLUS, SEGA, NVIDIA, AMD, or Intel.
All trademarks belong to their respective owners.

RE:FRAME requires a legally purchased copy of Persona 3 Reload. It does not distribute game files, bypass DRM, or unlock DLC. The screenshot documents the interface and depicts assets from the game.

See the applicable game terms for modification and distribution conditions. [Persona 3 Reload Steam EULA](https://store.steampowered.com/eula/2161700_eula_1)
