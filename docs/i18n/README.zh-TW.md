# RE:FRAME — Persona 3 Reload

[English](../../README.md) · [Français](README.fr-FR.md) · [Italiano](README.it-IT.md) · [Deutsch](README.de-DE.md) · [Español (España)](README.es-ES.md) · [日本語](README.ja-JP.md) · [한국어](README.ko-KR.md) · [Polski](README.pl-PL.md) · [Português (Brasil)](README.pt-BR.md) · [Русский](README.ru-RU.md) · [简体中文](README.zh-CN.md) · **[繁體中文](README.zh-TW.md)** · [Türkçe](README.tr-TR.md) · [Українська](README.uk-UA.md)

<!-- source-readme: 8deef3923bb5be1cd0937c169e899628281472fb -->

![版本 1.1.4](https://img.shields.io/badge/release-1.1.4-2469ad) ![Windows x64](https://img.shields.io/badge/platform-Windows%20x64-2469ad) ![DirectX 12](https://img.shields.io/badge/renderer-DirectX%2012-107c10) [![MIT 授權](https://img.shields.io/badge/license-MIT-blue)](../../LICENSE)

在 **Persona 3 Reload** 中減少鋸齒和閃爍，不必把渲染比例拉到 200%。DLAA 或 FSR、XeSS 的原生反鋸齒 能在螢幕原本的解析度下平滑邊緣。如果想提高幀率，可以選擇超解析度模式。

**[RE:FRAME 已發佈版本](https://github.com/Aporial/reframe-p3r/releases/latest)**

![Persona 3 Reload 的遊戲內圖形選單，顯示 RE:FRAME 設定](../images/settings.png)

*RE:FRAME 的選項直接顯示在遊戲的圖形設定中。*

## 安裝

1. 關閉遊戲，從 [GitHub Releases](https://github.com/Aporial/reframe-p3r/releases/latest) 下載 **REFRAME-P3R-v1.1.4.zip**。
2. 在 Steam 中以滑鼠右鍵點選 **Persona 3 Reload**，選擇 **內容 → 已安裝檔案 → 瀏覽**。
3. 將 ZIP 解壓縮到已有 `P3R` 的遊戲根資料夾。解壓縮後，`d3d12.dll` 會與 `P3R.exe` 一同放在 `P3R\Binaries\Win64` 內。
4. 像平常一樣從 Steam 啟動遊戲。
5. 開啟圖形設定，選擇渲染比例（**Rendering Scale**）正下方的 **反鋸齒技術**。如果有畫質模式可以選，選好後確認變更。

**`P3R\Binaries\Win64` 裡已經有 `d3d12.dll`？** 該檔案可能是其他模組使用的。請先確認屬於哪個模組，依其說明解除安裝後，再解壓縮 ZIP。

ZIP 已包含模組和必要的圖形執行階段程式庫。直接在遊戲選單設定即可，無須 ReShade、外部模組載入器、覆蓋介面、獨立啟動器或手動編輯設定檔。

### 更新與解除安裝

- **更新** · 關閉遊戲，執行舊版的解除安裝指令碼，再解壓縮新版 ZIP。若要沿用開發版本的設定，請保留 `P3RGraphicsEnhanced\Settings.ini`，直到 RE:FRAME 將設定儲存至 `ReFrame\Settings.ini`。
- **解除安裝** · 關閉遊戲，在 `P3R\Binaries\Win64\ReFrame` 中執行 `Remove-ReFrame.ps1`。指令碼會核對檔案雜湊，並保留產生的設定和記錄。加上 `-WhatIf` 可以預覽操作。 [安裝與解除安裝指南](INSTALLATION.zh-TW.md)

[安裝](#%E5%AE%89%E8%A3%9D) · [為什麼不直接把渲染比例設為 200%？](#%E7%82%BA%E4%BB%80%E9%BA%BC%E4%B8%8D%E7%9B%B4%E6%8E%A5%E6%8A%8A%E6%B8%B2%E6%9F%93%E6%AF%94%E4%BE%8B%E8%A8%AD%E7%82%BA-200) · [該選哪個模式？](#%E8%A9%B2%E9%81%B8%E5%93%AA%E5%80%8B%E6%A8%A1%E5%BC%8F) · [主要功能](#%E4%B8%BB%E8%A6%81%E5%8A%9F%E8%83%BD) · [支援的顯示卡](#%E6%94%AF%E6%8F%B4%E7%9A%84%E9%A1%AF%E7%A4%BA%E5%8D%A1) · [進階設定與模式](#%E9%80%B2%E9%9A%8E%E8%A8%AD%E5%AE%9A%E8%88%87%E6%A8%A1%E5%BC%8F) · [相容性與限制](#%E7%9B%B8%E5%AE%B9%E6%80%A7%E8%88%87%E9%99%90%E5%88%B6) · [疑難排解](#%E7%96%91%E9%9B%A3%E6%8E%92%E8%A7%A3) · [從原始碼建置](#%E5%BE%9E%E5%8E%9F%E5%A7%8B%E7%A2%BC%E5%BB%BA%E7%BD%AE) · [授權](#%E6%8E%88%E6%AC%8A)

## 為什麼不直接把渲染比例設為 200%？

在 1920×1080 下，輪廓的鋸齒和細節的閃爍可能很明顯。提高渲染比例能減輕這些問題，但也會增加顯示卡的負擔。

| 渲染比例 | 1080p 螢幕上遊戲實際渲染的畫面 |
| --- | --- |
| **100%** | 1920×1080, 每幀約 210 萬像素 |
| **200%** | 3840×2160, 每幀約 830 萬像素 |

**設為 200% 時，寬度和高度都加倍，每幀須計算的像素變成四倍。** 原生反鋸齒利用連續畫面的資訊，在 100% 渲染比例下平滑邊緣。此處理也會消耗 GPU 資源，與 200% 渲染的效果差異視場景而定。

超解析度先以少於螢幕顯示的像素渲染，再重建畫面。GPU 成為效能瓶頸時，這樣可能提高 FPS。

## 該選哪個模式？

- **原生 TAA** · 遊戲內建的反鋸齒方式，渲染比例可手動調整。
- **NVIDIA DLAA** · 在 100% 渲染比例下讓邊緣更平滑。如果你有相容的 RTX 顯示卡又重視畫質，可以先試這個模式。
- **AMD FSR 原生反鋸齒** · 在相容硬體上以 100% 渲染比例使用的 AMD 反鋸齒。
- **Intel XeSS 原生反鋸齒** · 在相容硬體上以 100% 渲染比例使用的 Intel 反鋸齒。
- **DLSS / FSR / XeSS 畫質** · 適合初次嘗試超解析度。遊戲先以較低解析度渲染，再重建至你選擇的輸出解析度。
- **平衡 / 效能** · 渲染解析度低於 **畫質** 模式，進一步減輕 GPU 負擔。細節可能沒那麼清晰。

| 目標 | 建議選項 |
| --- | --- |
| 在 RTX 上優先考量畫質 | NVIDIA DLAA |
| 沒有 RTX 也想在螢幕解析度下反鋸齒  |  FSR 或 XeSS 的 **原生反鋸齒**（若可用） |
| 提高 FPS  |  DLSS、FSR 或 XeSS 的 **畫質** 模式 |
| 效能優先  |  **效能**，或選擇 **平衡** 兼顧畫質 |

可用選項和實際效果因 GPU、驅動程式和場景而異。選單會顯示系統回報為可用的選項。

## 主要功能

- 透過 NVIDIA DLAA/DLSS、AMD FSR 和 Intel XeSS 提供原生解析度反鋸齒和超解析度。
- 在遊戲選單直接設定，SDK 模式會自動調整渲染比例。
- 自動偵測 13 種介面語言。
- 透過 ZIP 安裝，附有解除安裝指令碼。從 Steam 啟動，並使用遊戲原有的選單與操作方式。

## 系統需求

RE:FRAME 需要在 Steam 合法購買的 Persona 3 Reload、64 位元 Windows 10 或 11、DirectX 12 以及適用的顯示卡驅動程式。1.1.4 適用於 **Steam 組建 22672075**（應用程式 ID **2161700**）。

### 支援的顯示卡

| 技術 | 支援的 GPU |
| --- | --- |
| **NVIDIA DLAA / DLSS** | 搭配相容驅動程式的 GeForce RTX 顯示卡。 [NVIDIA DLSS](https://www.nvidia.com/en-us/geforce/technologies/dlss/) |
| **AMD FSR** | 在受支援的 Radeon RX 6000、7000 和 9000 系列顯示卡上，SDK 可能提供 FSR 4.1.2。其他符合 SDK 需求的 GPU 可以使用 FSR 3.1.6。 [AMD FSR SDK](https://gpuopen.com/amd-fsr-sdk/) |
| **Intel XeSS** | 相容的 Intel Iris Xe 和 Arc，以及支援 Shader Model 6.4 和 DP4a 加速的其他廠牌相容 GPU。 [Intel XeSS 需求](https://github.com/intel/xess/blob/v3.0.2/doc/xess_sr_developer_guide_english.md#requirements) |

XeSS 需要 Microsoft Visual C++ x64 可轉散發套件 **14.40.33810 或更新版本**。Intel 列出的此 SDK 最低需求為 Windows 10 組建 19043 或 Windows 11 組建 22000。 [Intel XeSS 部署指南](https://github.com/intel/xess/blob/v3.0.2/doc/xess_sr_developer_guide_english.md#deployment) · [Microsoft Visual C++ 可轉散發套件](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist)

## 進階設定與模式

在 **反鋸齒技術** 中選擇技術，再選擇可用的 **畫質模式**。使用 NVIDIA DLSS 時會顯示 **DLSS 模型**。使用 **原生 TAA** 時可手動調整渲染比例。若由 SDK 管理，則會顯示其目標比例。透過遊戲的確認視窗套用或取消變更。切回 **原生 TAA** 時可以同時設定新的手動比例，再一起確認。

**銳利度**在 DLAA、DLSS、FSR 或 XeSS 完成重建後、遊戲繪製 HUD 前調整場景細節。範圍為 0%（預設，關閉）到 100%，每格 5%。套用後立即生效，無需重新啟動。切換技術或畫質模式時會保留同一設定值。原生 TAA 會隱藏此選項。

### 可用模式

| 模式 | NVIDIA DLSS | AMD FSR | Intel XeSS |
| --- | --- | --- | --- |
| 原生反鋸齒 (100%) | DLAA | 原生反鋸齒 | 原生反鋸齒 |
| 超高畫質+ | — | — | 若可用 |
| 超高畫質 | — | — | 若可用 |
| 畫質 | ✓ | ✓ | ✓ |
| 平衡 | ✓ | ✓ | ✓ |
| 效能 | ✓ | ✓ | ✓ |

三種 SDK 合計最多提供 14 種模式。此外也可選擇 **原生 TAA**。

### DLSS 模型

| 模型 | 要求使用的模型 |
| --- | --- |
| **自動（建議）** | DLAA、**畫質** 和 **平衡** 會要求使用 K，**效能** 則要求使用 M。 |
| **DLSS 4 — 標準 (K)** | DLSS 4 的 Transformer 模型。 |
| **DLSS 4.5 — 最佳化 (M)** | NVIDIA 為 **效能** 模式建議使用的 DLSS 4.5 模型。 |
| **DLSS 4.5 — 細節 (L)** | NVIDIA 為 4K 超高效能建議使用的 DLSS 4.5 模型。模型與畫質模式分別選擇。 |

在 RTX 20 和 30 系列上，M 和 L 可能需要較多 GPU 處理時間。 [NVIDIA 的 DLSS 模型指南](https://www.nvidia.com/en-us/geforce/news/dlss-4-5-super-resolution-available-now/)

### 介面語言

英語、法語、義大利語、德語、西班牙語（西班牙）、日語、韓語、波蘭語、葡萄牙語（巴西）、俄語、簡體中文、繁體中文和土耳其語。RE:FRAME 依遊戲語言顯示，遇到不支援的語言會改用英語。 [Steam 上的 Persona 3 Reload](https://store.steampowered.com/app/2161700/Persona_3_Reload/)

## 相容性與限制

- **遊戲更新** · Steam 組建 22672075 以外的版本、其他商店版本、非 Windows 執行環境，以及與其他 D3D12 代理併用，皆尚未驗證。
- **回到原生模式** · 遊戲對無法辨識的渲染階段及平面反射等次要視圖使用 **原生 TAA**。
- **測試** · 已在 RTX 5060 上以 1080p 驗證 DLSS、FSR 和 XeSS 效能模式的遊戲內銳利度。離線測試涵蓋該顯示卡可用的全部 14 種 SDK 模式。Radeon 上的 FSR 4 和針對 Intel Arc 最佳化的 XeSS 仍須在相應硬體上測試。 [測試範圍與相容性](../COMPATIBILITY.md)

## 疑難排解

- **找不到 RE:FRAME 的設定？** 請檢查 Steam 組建版本，並確認 `d3d12.dll` 和 `P3R.exe` 放在同一個資料夾。
- **沒有想用的反鋸齒技術？** 請確認顯示卡相容性、驅動程式和 `P3R\Binaries\Win64\ReFrame` 內的執行階段程式庫。若要用 XeSS，也請檢查 Visual C++ 可轉散發套件。
- **不能調整渲染比例？** 使用 DLSS、FSR 或 XeSS 時是正常的。若要自行調整，請選擇 **原生 TAA**。

如需更多協助，請參閱[疑難排解](../TROUBLESHOOTING.md)。模組會把實際渲染解析度和回到原生模式的情況寫入 `P3R\Binaries\Win64\ReFrame\ReFrame.log`。

## 從原始碼建置

安裝 C++ 工作負載和 Windows SDK 後，開啟 **Visual Studio 2022 的 x64 Native Tools Command Prompt**，執行下列命令。

```powershell
powershell -NoProfile -File scripts\Fetch-Dependencies.ps1
powershell -NoProfile -File build.ps1
powershell -NoProfile -File scripts\New-Release.ps1
```

[建置指南](../BUILDING.md) · [架構](../ARCHITECTURE.md) · [變更記錄](../../CHANGELOG.md) · [貢獻指南](../../CONTRIBUTING.md)

## 授權

Copyright © 2026 Aporia。MIT 授權適用於 RE:FRAME 的原創原始碼。隨附的 SDK 執行階段程式庫、NVIDIA NGX 載入器、MinHook/HDE 和其他第三方元件適用各自的條款和聲明。遊戲與其素材仍受各自權利人的條款約束。 [MIT 授權](../../LICENSE) · [第三方聲明](../../THIRD_PARTY_NOTICES.md) · [供應商授權](../../licenses/)

## 免責聲明

RE:FRAME 是玩家製作的獨立非官方專案，與 ATLUS、SEGA、NVIDIA、AMD 或 Intel 無關，也未得到這些公司的認可或贊助。所有商標均屬各自的權利人所有。

使用 RE:FRAME 需要合法購買 Persona 3 Reload。模組不散布遊戲檔案、不規避 DRM，也不解鎖 DLC。螢幕擷取畫面用於說明介面，其中包含遊戲素材。 [Persona 3 Reload 的 Steam 使用者授權合約](https://store.steampowered.com/eula/2161700_eula_1)
