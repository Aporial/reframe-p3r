# RE:FRAME — Persona 3 Reload v1.1.5 · City Map Super Resolution

The city map now uses your selected DLSS, FSR or XeSS upscaling mode when its render passes can be verified. The 3D background fills the map area and lines up with the map labels. If RE:FRAME cannot correct the background, that visit returns to the game's render scale. The fix is enabled by default.

## Download and install

Download **REFRAME-P3R-v1.1.5.zip** and its **.sha256** file from the release assets. Close the game and run the previous version's `Remove-ReFrame.ps1` if updating. Extract the new ZIP into the Steam game folder that contains `P3R`. The update preserves your saved settings and targets **Steam build 22672075**, Windows x64 and DirectX 12.

[English](https://github.com/Aporial/reframe-p3r/blob/v1.1.5/docs/INSTALLATION.md) · [Français](https://github.com/Aporial/reframe-p3r/blob/v1.1.5/docs/i18n/INSTALLATION.fr-FR.md) · [Italiano](https://github.com/Aporial/reframe-p3r/blob/v1.1.5/docs/i18n/INSTALLATION.it-IT.md) · [Deutsch](https://github.com/Aporial/reframe-p3r/blob/v1.1.5/docs/i18n/INSTALLATION.de-DE.md) · [Español](https://github.com/Aporial/reframe-p3r/blob/v1.1.5/docs/i18n/INSTALLATION.es-ES.md) · [日本語](https://github.com/Aporial/reframe-p3r/blob/v1.1.5/docs/i18n/INSTALLATION.ja-JP.md) · [한국어](https://github.com/Aporial/reframe-p3r/blob/v1.1.5/docs/i18n/INSTALLATION.ko-KR.md) · [Polski](https://github.com/Aporial/reframe-p3r/blob/v1.1.5/docs/i18n/INSTALLATION.pl-PL.md) · [Português (Brasil)](https://github.com/Aporial/reframe-p3r/blob/v1.1.5/docs/i18n/INSTALLATION.pt-BR.md) · [Русский](https://github.com/Aporial/reframe-p3r/blob/v1.1.5/docs/i18n/INSTALLATION.ru-RU.md) · [简体中文](https://github.com/Aporial/reframe-p3r/blob/v1.1.5/docs/i18n/INSTALLATION.zh-CN.md) · [繁體中文](https://github.com/Aporial/reframe-p3r/blob/v1.1.5/docs/i18n/INSTALLATION.zh-TW.md) · [Türkçe](https://github.com/Aporial/reframe-p3r/blob/v1.1.5/docs/i18n/INSTALLATION.tr-TR.md) · [Українська](https://github.com/Aporial/reframe-p3r/blob/v1.1.5/docs/i18n/INSTALLATION.uk-UA.md)

## Validation

The packaged 1.1.5 DLL recorded the map correction in FSR 3.1.6 Performance on a GeForce RTX 5060 at 1920×1080. The map rendered internally at 960×540. The r4 test build also recorded successful map reconstruction in DLSS, FSR and XeSS Quality and in DLSS Performance. The packaged DLL's post-close behavior and fallback still need direct checks. The Windows x64 build, CPU checks, ZIP manifest and uninstaller checks passed. [City map testing](https://github.com/Aporial/reframe-p3r/blob/v1.1.5/docs/TOWN_MAP_UPSCALING_TEST.md) · [Compatibility](https://github.com/Aporial/reframe-p3r/blob/v1.1.5/docs/COMPATIBILITY.md)

## License

RE:FRAME is an independent fan-made project. The ZIP includes the project MIT license and the separate terms for AMD FidelityFX CAS, the vendor runtimes and MinHook. [Third-party notices](https://github.com/Aporial/reframe-p3r/blob/v1.1.5/THIRD_PARTY_NOTICES.md)
