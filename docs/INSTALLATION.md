# RE:FRAME — Persona 3 Reload 1.1.0 · Sharpness Update

**English** · [Français](i18n/INSTALLATION.fr-FR.md) · [Italiano](i18n/INSTALLATION.it-IT.md) · [Deutsch](i18n/INSTALLATION.de-DE.md) · [Español](i18n/INSTALLATION.es-ES.md) · [日本語](i18n/INSTALLATION.ja-JP.md) · [한국어](i18n/INSTALLATION.ko-KR.md) · [Polski](i18n/INSTALLATION.pl-PL.md) · [Português (Brasil)](i18n/INSTALLATION.pt-BR.md) · [Русский](i18n/INSTALLATION.ru-RU.md) · [简体中文](i18n/INSTALLATION.zh-CN.md) · [繁體中文](i18n/INSTALLATION.zh-TW.md) · [Türkçe](i18n/INSTALLATION.tr-TR.md) · [Українська](i18n/INSTALLATION.uk-UA.md)

By Aporia. For a legally purchased copy of Persona 3 Reload on Steam, app ID **2161700**, build **22672075**. Requires Windows x64 and DirectX 12.

## What's new

**Sharpness** uses FidelityFX CAS to adjust scene detail after DLAA, DLSS, FSR or XeSS reconstruction and before the HUD is drawn. The default is 0% (off), and the range is 0–100% in 5% steps. Apply the change in the Graphics menu. The setting takes effect without restarting and is shared across technologies and quality modes. Original TAA hides the control.

## Install or update

1. Close the game. In Steam, open **Properties → Installed Files → Browse**.
2. If `P3R\Binaries\Win64` already contains a `d3d12.dll`, follow the owning mod's removal instructions. When upgrading from a 1.1.0 RE:FRAME test build, back up its files and extract the release over that installation.
3. Compare the ZIP's SHA-256 against the companion `.sha256` file. Extract **REFRAME-P3R-v1.1.0.zip** into the folder that contains `P3R`. Launch through Steam and select a technology, quality mode and Sharpness in Graphics.

Keep `ReFrame\Settings.ini` to preserve your settings. If upgrading from an older development build, keep `P3RGraphicsEnhanced\Settings.ini` until RE:FRAME saves to its new folder. XeSS requires the Microsoft Visual C++ x64 Redistributable 14.40.33810 or later.

## Uninstall

Close the game and run `Remove-ReFrame.ps1` from `P3R\Binaries\Win64\ReFrame`. Use `-WhatIf` to preview file removal. The script checks the manifest and file hashes and keeps generated settings and logs.

## Check Sharpness

Apply 0% and then 100% in the same scene with the same technology, quality and camera position. Leave the menu after each change. Compare hair and fine textures, then move the camera and check edges, HUD and subtitles. Check that Cancel discards a draft change and Apply persists the value after restart.

Open `P3R\Binaries\Win64\ReFrame\ReFrame.log` for the active backend and CAS coverage. A stable 240-frame sample should show `240 frames (0 CAS)` at 0% and `240 frames (240 CAS)` when the filter runs on every sampled frame.

Sharpness was verified in-game at 1080p in DLSS, FSR and XeSS Performance on an RTX 5060. Offline tests covered all 14 available SDK modes on that GPU. Radeon FSR 4 and Intel-optimized XeSS on Arc still need dedicated hardware testing.

## License

RE:FRAME is an independent fan-made project. The archive includes the RE:FRAME MIT license and the separate FidelityFX CAS, vendor runtime and MinHook notices in `Licenses`.
