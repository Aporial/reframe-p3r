# RE:FRAME — Persona 3 Reload

**[English](README.md)** · [Français](docs/i18n/README.fr-FR.md) · [Italiano](docs/i18n/README.it-IT.md) · [Deutsch](docs/i18n/README.de-DE.md) · [Español (España)](docs/i18n/README.es-ES.md) · [日本語](docs/i18n/README.ja-JP.md) · [한국어](docs/i18n/README.ko-KR.md) · [Polski](docs/i18n/README.pl-PL.md) · [Português (Brasil)](docs/i18n/README.pt-BR.md) · [Русский](docs/i18n/README.ru-RU.md) · [简体中文](docs/i18n/README.zh-CN.md) · [繁體中文](docs/i18n/README.zh-TW.md) · [Türkçe](docs/i18n/README.tr-TR.md) · [Українська](docs/i18n/README.uk-UA.md)

![Release 1.1.5](https://img.shields.io/badge/release-1.1.5-2469ad) ![Windows x64](https://img.shields.io/badge/platform-Windows%20x64-2469ad) ![DirectX 12](https://img.shields.io/badge/renderer-DirectX%2012-107c10) [![License: MIT](https://img.shields.io/badge/license-MIT-blue)](LICENSE)

Smooth jagged edges and reduce shimmering in **Persona 3 Reload** without setting Rendering Scale to 200%. Use DLAA, FSR Native AA, or XeSS Native AA for cleaner visuals at your screen’s resolution. If you want more performance, choose an upscaling mode.

**[Published RE:FRAME releases](https://github.com/Aporial/reframe-p3r/releases/latest)**

![Persona 3 Reload’s native Graphics menu showing the RE:FRAME settings](docs/images/settings.png)

*RE:FRAME’s options appear in Persona 3 Reload’s own Graphics menu.*

## Installation

1. Close the game and download **REFRAME-P3R-v1.1.5.zip** from [GitHub Releases](https://github.com/Aporial/reframe-p3r/releases/latest).
2. In Steam, right-click **Persona 3 Reload** and choose **Properties → Installed Files → Browse**.
3. Extract the ZIP into the folder that already contains `P3R`. The ZIP puts `d3d12.dll` next to `P3R.exe` in `P3R\Binaries\Win64`.
4. Launch the game normally through Steam.
5. Open **Graphics** and select **Anti-Aliasing Technology** just below **Rendering Scale**. Pick a quality mode if one is available, then confirm your changes.

**Already have a `d3d12.dll` in `P3R\Binaries\Win64`?** Another mod may use it. Check which mod owns it and follow that mod’s removal instructions before extracting the ZIP.

The ZIP includes the mod and its graphics runtimes. Setup takes place in the normal Graphics menu, with no ReShade, external mod loader, overlay, separate launcher, or manual config edits.

### Updating and uninstalling

- **Update** · Close the game, run the previous version’s uninstaller, and extract the new ZIP. To keep settings from a development build, leave `P3RGraphicsEnhanced\Settings.ini` in place until RE:FRAME saves to `ReFrame\Settings.ini`.
- **Uninstall** · Close the game and run `Remove-ReFrame.ps1` from `P3R\Binaries\Win64\ReFrame`. The script checks file hashes and keeps generated settings and logs. Add `-WhatIf` to preview. [Installation and removal guide](docs/INSTALLATION.md)

[Installation](#installation) · [Why 100%?](#why-not-just-use-200-rendering-scale) · [Choose a mode](#which-mode-should-i-use) · [Features](#features) · [Hardware](#supported-hardware) · [Advanced settings](#advanced-settings-and-modes) · [Compatibility](#compatibility-and-limitations) · [Troubleshooting](#troubleshooting) · [Build](#building-from-source) · [License](#license)

## Why not just use 200% Rendering Scale?

At 1920×1080, jagged edges and shimmering thin details can be easy to spot. Raising the game’s Rendering Scale is one way to smooth them out, but it increases the amount of work your GPU has to do.

| Rendering Scale | What the game renders on a 1080p screen |
| --- | --- |
| **100%** | 1920×1080, about 2.1 million pixels per frame |
| **200%** | 3840×2160, about 8.3 million pixels per frame |

**200% doubles both dimensions and renders four times as many pixels.** Native AA gives you another way to smooth edges at 100% by combining information from consecutive frames. It has its own GPU cost, and how it compares with 200% depends on the scene.

Upscaling renders fewer pixels than your screen displays. It can raise FPS when the GPU is the bottleneck.

## Which mode should I use?

- **Original TAA** · The game’s built-in anti-aliasing with manual Rendering Scale.
- **NVIDIA DLAA** · Smoother edges at 100% scale. Start here on a compatible RTX GPU when image quality is your priority.
- **AMD FSR Native AA** · AMD’s anti-aliasing at 100% scale on supported hardware.
- **Intel XeSS Native AA** · Intel’s anti-aliasing at 100% scale on supported hardware.
- **DLSS / FSR / XeSS Quality** · Start here for upscaling. It renders at a lower resolution and reconstructs the picture to your selected output resolution.
- **Balanced / Performance** · Render at a lower resolution than Quality to reduce GPU load further. Fine details may look softer.

| Goal | Recommended option |
| --- | --- |
| Image quality first on RTX | NVIDIA DLAA |
| Native AA without RTX | FSR Native AA or XeSS Native AA, where available |
| More FPS | DLSS, FSR, or XeSS **Quality** |
| Maximum performance | **Performance**, or **Balanced** for a middle ground |

Availability and results depend on your GPU, driver, and the scene. The menu shows the options your system reports as available.

## Features

- Native-resolution anti-aliasing and upscaling through NVIDIA DLAA/DLSS, AMD FSR, and Intel XeSS.
- Game-native graphics controls with automatic Rendering Scale for SDK modes.
- Automatic language detection for 13 interface languages.
- ZIP installation and removal. Launch from Steam with the game’s own menu and controls.

## Requirements

RE:FRAME requires a legally purchased copy of Persona 3 Reload on Steam, Windows 10 or 11 x64, DirectX 12, and a suitable graphics driver. Version 1.1.5 targets **Steam build 22672075** (app ID **2161700**).

### Supported hardware

| Technology | GPU support |
| --- | --- |
| **NVIDIA DLAA / DLSS** | Compatible GeForce RTX GPUs with a supported driver. [NVIDIA DLSS](https://www.nvidia.com/en-us/geforce/technologies/dlss/) |
| **AMD FSR** | The SDK can offer FSR 4.1.2 on supported Radeon RX 6000, 7000, and 9000 series GPUs. FSR 3.1.6 is available on other GPUs that meet the SDK’s requirements. [AMD FSR SDK](https://gpuopen.com/amd-fsr-sdk/) |
| **Intel XeSS** | Compatible Intel Iris Xe and Arc GPUs, plus supported GPUs from other vendors with Shader Model 6.4 and DP4a acceleration. [Intel XeSS requirements](https://github.com/intel/xess/blob/v3.0.2/doc/xess_sr_developer_guide_english.md#requirements) |

XeSS needs the Microsoft Visual C++ x64 Redistributable **14.40.33810 or later**. Intel lists Windows 10 build 19043 and Windows 11 build 22000 as the minimum versions for this SDK. [Intel XeSS deployment](https://github.com/intel/xess/blob/v3.0.2/doc/xess_sr_developer_guide_english.md#deployment) · [Microsoft Visual C++ Redistributable](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist)

## Advanced settings and modes

Choose a backend with **Anti-Aliasing Technology**, then choose an available **Quality Mode**. The **DLSS Model** row appears for NVIDIA DLSS. **Rendering Scale** is editable with Original TAA and shows the selected SDK target when a backend manages it. Use the game’s normal prompt to confirm or cancel changes. You can set a new manual scale while switching back to Original TAA in the same save.

**Sharpness** adjusts scene detail after DLAA, DLSS, FSR or XeSS reconstruction, before the game draws the HUD. It ranges from 0% to 100% in 5% steps and defaults to 0% (off). Apply the setting to see the change without restarting. The same value is saved across backends and quality modes. Original TAA hides the control.

### Available modes

| Mode | NVIDIA DLSS | AMD FSR | Intel XeSS |
| --- | --- | --- | --- |
| Native AA (100%) | DLAA | Native AA | Native AA |
| Ultra Quality Plus | — | — | When available |
| Ultra Quality | — | — | When available |
| Quality | ✓ | ✓ | ✓ |
| Balanced | ✓ | ✓ | ✓ |
| Performance | ✓ | ✓ | ✓ |

The three SDKs offer up to 14 modes in total. Original TAA is also available.

### DLSS models

| Model | What it requests |
| --- | --- |
| **Auto (Recommended)** | K for DLAA, Quality, and Balanced. M for Performance. |
| **DLSS 4 — Standard (K)** | The DLSS 4 transformer model. |
| **DLSS 4.5 — Optimized (M)** | The DLSS 4.5 model NVIDIA recommends for Performance. |
| **DLSS 4.5 — Detail (L)** | The DLSS 4.5 model NVIDIA recommends for 4K Ultra Performance. The model setting is separate from the quality preset. |

M and L can take more GPU time on RTX 20 and 30 series cards. [NVIDIA’s DLSS model guidance](https://www.nvidia.com/en-us/geforce/news/dlss-4-5-super-resolution-available-now/)

### Interface languages

English, French, Italian, German, Spanish (Spain), Japanese, Korean, Polish, Portuguese (Brazil), Russian, Simplified Chinese, Traditional Chinese, and Turkish. RE:FRAME follows the game’s language and falls back to English. [Persona 3 Reload on Steam](https://store.steampowered.com/app/2161700/Persona_3_Reload/)

## Compatibility and limitations

- **Game updates** · Support for Steam builds other than 22672075 is unverified. Other stores, non-Windows runtimes, and stacking D3D12 proxies are also unverified.
- **Fallback** · The game uses Original TAA for unrecognized render passes and secondary views such as planar reflections.
- **Testing** · Sharpness was verified in-game at 1080p in DLSS, FSR and XeSS Performance on an RTX 5060. Offline tests covered all 14 available SDK modes on that GPU. Radeon FSR 4 and Intel-optimized XeSS on Arc still need dedicated hardware testing. [Test coverage and compatibility](docs/COMPATIBILITY.md) · [Sharpness test scenario](docs/SHARPNESS_TESTING.md)

## Troubleshooting

- **No RE:FRAME options?** Check the Steam build and confirm that `d3d12.dll` is next to `P3R.exe`.
- **No option for your preferred technology?** Check hardware support, your driver, and the runtime files in `P3R\Binaries\Win64\ReFrame`. For XeSS, also check the Visual C++ Redistributable.
- **Rendering Scale disabled?** That is expected under DLSS, FSR, or XeSS. Select Original TAA to adjust it by hand.

For more help, see [Troubleshooting](docs/TROUBLESHOOTING.md). The mod records the actual render size and any fallback in `P3R\Binaries\Win64\ReFrame\ReFrame.log`.

## Building from source

From the **x64 Native Tools Command Prompt for Visual Studio 2022** with the C++ workload and Windows SDK installed, run:

```powershell
powershell -NoProfile -File scripts\Fetch-Dependencies.ps1
powershell -NoProfile -File build.ps1
powershell -NoProfile -File scripts\New-Release.ps1
```

[Build guide](docs/BUILDING.md) · [Architecture](docs/ARCHITECTURE.md) · [Changelog](CHANGELOG.md) · [Contributing](CONTRIBUTING.md)

## License

Copyright © 2026 Aporia. The MIT License covers RE:FRAME’s original source code. Bundled SDK runtimes, the NVIDIA NGX loader, MinHook/HDE, and other third-party components retain their own terms and notices. The game and its assets remain subject to their owners’ terms. [MIT License](LICENSE) · [Third-party notices](THIRD_PARTY_NOTICES.md) · [Vendor licenses](licenses/)

## Disclaimer

RE:FRAME is an independent, unofficial fan-made project.
It is not affiliated with, endorsed by, or sponsored by ATLUS, SEGA, NVIDIA, AMD, or Intel.
All trademarks belong to their respective owners.

RE:FRAME requires a legally purchased copy of Persona 3 Reload. It does not distribute game files, bypass DRM, or unlock DLC. The screenshot documents the interface and depicts assets from the game. [Persona 3 Reload Steam EULA](https://store.steampowered.com/eula/2161700_eula_1)
