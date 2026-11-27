# RE:FRAME — Persona 3 Reload

[English](../../README.md) · [Français](README.fr-FR.md) · [Italiano](README.it-IT.md) · [Deutsch](README.de-DE.md) · [Español (España)](README.es-ES.md) · [日本語](README.ja-JP.md) · [한국어](README.ko-KR.md) · [Polski](README.pl-PL.md) · [Português (Brasil)](README.pt-BR.md) · [Русский](README.ru-RU.md) · **[简体中文](README.zh-CN.md)** · [繁體中文](README.zh-TW.md) · [Türkçe](README.tr-TR.md) · [Українська](README.uk-UA.md)

<!-- source-readme: 8deef3923bb5be1cd0937c169e899628281472fb -->

![版本 1.1.4](https://img.shields.io/badge/release-1.1.4-2469ad) ![Windows x64](https://img.shields.io/badge/platform-Windows%20x64-2469ad) ![DirectX 12](https://img.shields.io/badge/renderer-DirectX%2012-107c10) [![MIT 许可证](https://img.shields.io/badge/license-MIT-blue)](../../LICENSE)

在 **Persona 3 Reload** 中减少锯齿和闪烁，无需把渲染比例调到 200%。使用 DLAA 或 FSR、XeSS 的原生抗锯齿，可以按屏幕分辨率渲染并平滑边缘。如果更在意帧率，可以选择超分辨率模式。

**[RE:FRAME 已发布版本](https://github.com/Aporial/reframe-p3r/releases/latest)**

![Persona 3 Reload 的游戏内图形菜单，显示 RE:FRAME 选项](../images/settings.png)

*RE:FRAME 的选项直接显示在游戏的图形设置中。*

## 安装

1. 关闭游戏，从 [GitHub Releases](https://github.com/Aporial/reframe-p3r/releases/latest) 下载 **REFRAME-P3R-v1.1.4.zip**。
2. 在 Steam 中右键点击 **Persona 3 Reload**，选择 **属性 → 已安装文件 → 浏览**。
3. 将 ZIP 解压到已经包含 `P3R` 的游戏根目录。解压后，`d3d12.dll` 会放在 `P3R\Binaries\Win64` 中，与 `P3R.exe` 同级。
4. 像平时一样通过 Steam 启动游戏。
5. 打开图形设置，在渲染比例（**Rendering Scale**）正下方选择 **抗锯齿技术**。如果有可选的画质模式，请选好后确认更改。

**`P3R\Binaries\Win64` 中已经有 `d3d12.dll`？** 这个文件可能属于其他模组。先确认它由哪个模组使用，按该模组的说明卸载，再解压 ZIP。

ZIP 已包含模组和所需的图形运行库。直接在游戏菜单中设置即可，无需 ReShade、外部模组加载器、叠加层、独立启动器，也无需手动修改配置文件。

### 更新与卸载

- **更新** · 关闭游戏，运行旧版的卸载脚本，再解压新版 ZIP。要保留开发版设置，请保留 `P3RGraphicsEnhanced\Settings.ini`，直到 RE:FRAME 将设置保存到 `ReFrame\Settings.ini`。
- **卸载** · 关闭游戏，在 `P3R\Binaries\Win64\ReFrame` 中运行 `Remove-ReFrame.ps1`。脚本会校验文件哈希，并保留生成的设置和日志。加上 `-WhatIf` 可预览操作。 [安装和卸载指南](INSTALLATION.zh-CN.md)

[安装](#%E5%AE%89%E8%A3%85) · [为什么不直接把渲染比例调到 200%？](#%E4%B8%BA%E4%BB%80%E4%B9%88%E4%B8%8D%E7%9B%B4%E6%8E%A5%E6%8A%8A%E6%B8%B2%E6%9F%93%E6%AF%94%E4%BE%8B%E8%B0%83%E5%88%B0-200) · [应该选哪个模式？](#%E5%BA%94%E8%AF%A5%E9%80%89%E5%93%AA%E4%B8%AA%E6%A8%A1%E5%BC%8F) · [主要功能](#%E4%B8%BB%E8%A6%81%E5%8A%9F%E8%83%BD) · [支持的显卡](#%E6%94%AF%E6%8C%81%E7%9A%84%E6%98%BE%E5%8D%A1) · [高级设置和模式](#%E9%AB%98%E7%BA%A7%E8%AE%BE%E7%BD%AE%E5%92%8C%E6%A8%A1%E5%BC%8F) · [兼容性与限制](#%E5%85%BC%E5%AE%B9%E6%80%A7%E4%B8%8E%E9%99%90%E5%88%B6) · [故障排查](#%E6%95%85%E9%9A%9C%E6%8E%92%E6%9F%A5) · [从源代码构建](#%E4%BB%8E%E6%BA%90%E4%BB%A3%E7%A0%81%E6%9E%84%E5%BB%BA) · [许可证](#%E8%AE%B8%E5%8F%AF%E8%AF%81)

## 为什么不直接把渲染比例调到 200%？

在 1920×1080 下，物体边缘的阶梯状锯齿和细节闪烁可能比较明显。提高渲染比例能缓解这些问题，但也会增加显卡的负担。

| 渲染比例 | 1080p 屏幕上游戏实际渲染的画面 |
| --- | --- |
| **100%** | 1920×1080, 每帧约 210 万像素 |
| **200%** | 3840×2160, 每帧约 830 万像素 |

**调到 200% 后，宽度和高度都翻倍，每帧要计算的像素变为四倍。** 原生抗锯齿利用连续帧的信息，在 100% 渲染比例下平滑边缘。它也会占用 GPU 资源，与 200% 渲染相比的效果因场景而异。

超分辨率先用少于屏幕显示的像素渲染，再重建画面。当 GPU 是性能瓶颈时，这可能提高帧率。

## 应该选哪个模式？

- **原生 TAA** · 游戏自带的抗锯齿方式，可手动设置渲染比例。
- **NVIDIA DLAA** · 在 100% 渲染比例下平滑边缘。如果你有兼容的 RTX 显卡并优先考虑画质，可以先试这个模式。
- **AMD FSR 原生抗锯齿** · 在兼容硬件上以 100% 渲染比例运行的 AMD 抗锯齿。
- **Intel XeSS 原生抗锯齿** · 在兼容硬件上以 100% 渲染比例运行的 Intel 抗锯齿。
- **DLSS / FSR / XeSS 画质** · 适合作为尝试超分辨率的起点。游戏以较低分辨率渲染，再重建到你选择的输出分辨率。
- **平衡 / 性能** · 渲染分辨率低于 **画质** 模式，可进一步减轻 GPU 负担。细节可能会显得没那么锐利。

| 目标 | 推荐选项 |
| --- | --- |
| 在 RTX 上优先考虑画质 | NVIDIA DLAA |
| 没有 RTX 也想在屏幕分辨率下抗锯齿  |  FSR 或 XeSS 的 **原生抗锯齿**（如可用） |
| 提高帧率  |  DLSS、FSR 或 XeSS 的 **画质** 模式 |
| 优先追求性能  |  **性能**，想兼顾画质可以选 **平衡** |

可用选项和实际效果因 GPU、驱动和场景而异。菜单会显示系统报告为可用的选项。

## 主要功能

- 通过 NVIDIA DLAA/DLSS、AMD FSR 和 Intel XeSS 提供原生分辨率抗锯齿和超分辨率。
- 在游戏内设置，使用 SDK 模式时自动管理渲染比例。
- 自动识别 13 种界面语言。
- 通过 ZIP 安装，并附带卸载脚本。直接从 Steam 启动，沿用游戏菜单和操作方式。

## 系统要求

RE:FRAME 需要在 Steam 合法购买的 Persona 3 Reload、64 位 Windows 10 或 11、DirectX 12 和兼容的显卡驱动。1.1.4 面向 **Steam 构建版本 22672075**（应用 ID **2161700**）。

### 支持的显卡

| 技术 | 支持的 GPU |
| --- | --- |
| **NVIDIA DLAA / DLSS** | 使用受支持驱动的兼容 GeForce RTX 显卡。 [NVIDIA DLSS](https://www.nvidia.com/en-us/geforce/technologies/dlss/) |
| **AMD FSR** | 在受支持的 Radeon RX 6000、7000 和 9000 系列显卡上，SDK 可能提供 FSR 4.1.2。其他满足 SDK 要求的 GPU 可使用 FSR 3.1.6。 [AMD FSR SDK](https://gpuopen.com/amd-fsr-sdk/) |
| **Intel XeSS** | 兼容的 Intel Iris Xe 和 Arc 显卡，以及其他厂商支持 Shader Model 6.4 和 DP4a 加速的兼容 GPU。 [Intel XeSS 要求](https://github.com/intel/xess/blob/v3.0.2/doc/xess_sr_developer_guide_english.md#requirements) |

XeSS 需要 Microsoft Visual C++ x64 可再发行组件 **14.40.33810 或更高版本**。Intel 为这个 SDK 列出的最低要求是 Windows 10 构建版本 19043 或 Windows 11 构建版本 22000。 [Intel XeSS 部署指南](https://github.com/intel/xess/blob/v3.0.2/doc/xess_sr_developer_guide_english.md#deployment) · [Microsoft Visual C++ 可再发行组件](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist)

## 高级设置和模式

在 **抗锯齿技术** 中选择技术，然后选择可用的 **画质模式**。使用 NVIDIA DLSS 时会出现 **DLSS 模型**。选择 **原生 TAA** 时可以手动调整渲染比例，由 SDK 管理时则显示其目标比例。在游戏的确认窗口中应用或取消变更。切回 **原生 TAA** 时，可以同时调整手动比例，再一起确认。

**锐度**在 DLAA、DLSS、FSR 或 XeSS 完成重建后、游戏绘制 HUD 前调整场景细节。范围为 0%（默认，关闭）到 100%，每档 5%。应用后即时生效，无需重启。切换技术或画质模式时保留同一个数值。原生 TAA 会隐藏此控件。

### 可用模式

| 模式 | NVIDIA DLSS | AMD FSR | Intel XeSS |
| --- | --- | --- | --- |
| 原生抗锯齿 (100%) | DLAA | 原生抗锯齿 | 原生抗锯齿 |
| 超高画质+ | — | — | 如可用 |
| 超高画质 | — | — | 如可用 |
| 画质 | ✓ | ✓ | ✓ |
| 平衡 | ✓ | ✓ | ✓ |
| 性能 | ✓ | ✓ | ✓ |

三种 SDK 合计最多提供 14 个模式。此外还可以选择 **原生 TAA**。

### DLSS 模型

| 模型 | 请求的模型 |
| --- | --- |
| **自动（推荐）** | DLAA、**画质** 和 **平衡** 请求 K，**性能** 请求 M。 |
| **DLSS 4 — 标准 (K)** | DLSS 4 的 Transformer 模型。 |
| **DLSS 4.5 — 优化 (M)** | NVIDIA 为 **性能** 模式推荐的 DLSS 4.5 模型。 |
| **DLSS 4.5 — 细节 (L)** | NVIDIA 为 4K 超级性能模式推荐的 DLSS 4.5 模型。模型和画质模式是分开选择的。 |

在 RTX 20 和 30 系列上，M 和 L 可能耗费更多 GPU 时间。 [NVIDIA 的 DLSS 模型指南](https://www.nvidia.com/en-us/geforce/news/dlss-4-5-super-resolution-available-now/)

### 界面语言

英语、法语、意大利语、德语、西班牙语（西班牙）、日语、韩语、波兰语、葡萄牙语（巴西）、俄语、简体中文、繁体中文和土耳其语。RE:FRAME 跟随游戏语言，未支持的语言使用英语。 [Steam 上的 Persona 3 Reload](https://store.steampowered.com/app/2161700/Persona_3_Reload/)

## 兼容性与限制

- **游戏更新** · 尚未验证 Steam 构建版本 22672075 之外的版本、其他商店版本、非 Windows 运行环境，或与其他 D3D12 代理混用。
- **回退** · 游戏会在无法识别的渲染阶段以及平面反射等次要视图中使用 **原生 TAA**。
- **测试** · 已在 RTX 5060 上以 1080p 分别验证 DLSS、FSR 和 XeSS 性能模式下的游戏内锐度。离线测试覆盖该显卡可用的全部 14 种 SDK 模式。Radeon 上的 FSR 4 和针对 Intel Arc 优化的 XeSS 仍需在对应硬件上测试。 [测试范围与兼容性](../COMPATIBILITY.md)

## 故障排查

- **找不到 RE:FRAME 的选项？** 请检查 Steam 构建版本，并确认 `d3d12.dll` 与 `P3R.exe` 位于同一目录。
- **没有想用的抗锯齿技术？** 请检查显卡兼容性、驱动和 `P3R\Binaries\Win64\ReFrame` 中的运行库。使用 XeSS 时也要检查 Visual C++ 可再发行组件。
- **无法修改渲染比例？** 使用 DLSS、FSR 或 XeSS 时这是正常现象。要手动设置，请选择 **原生 TAA**。

更多帮助请参阅[故障排查](../TROUBLESHOOTING.md)。模组会将实际渲染分辨率和回退情况记录到 `P3R\Binaries\Win64\ReFrame\ReFrame.log`。

## 从源代码构建

安装 C++ 工作负载和 Windows SDK 后，打开 **Visual Studio 2022 的 x64 Native Tools Command Prompt**，运行以下命令。

```powershell
powershell -NoProfile -File scripts\Fetch-Dependencies.ps1
powershell -NoProfile -File build.ps1
powershell -NoProfile -File scripts\New-Release.ps1
```

[构建指南](../BUILDING.md) · [架构](../ARCHITECTURE.md) · [更新日志](../../CHANGELOG.md) · [贡献指南](../../CONTRIBUTING.md)

## 许可证

Copyright © 2026 Aporia。MIT 许可证适用于 RE:FRAME 的原创源代码。随附的 SDK 运行库、NVIDIA NGX 加载器、MinHook/HDE 和其他第三方组件适用各自的条款和声明。游戏及其素材仍受相应权利人的条款约束。 [MIT 许可证](../../LICENSE) · [第三方声明](../../THIRD_PARTY_NOTICES.md) · [供应商许可证](../../licenses/)

## 免责声明

RE:FRAME 是由玩家制作的独立、非官方项目，与 ATLUS、SEGA、NVIDIA、AMD 或 Intel 无关联，未获得这些公司的认可或赞助。所有商标归各自权利人所有。

使用 RE:FRAME 需要合法购买 Persona 3 Reload。模组不分发游戏文件，不绕过 DRM，也不解锁 DLC。截图用于说明界面，其中包含游戏素材。 [Persona 3 Reload Steam 用户许可协议](https://store.steampowered.com/eula/2161700_eula_1)
