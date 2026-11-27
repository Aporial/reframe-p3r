# Changelog

## 1.1.4 — City Map Fix

- Fixed clipping of the city map's 3D background with Super Resolution enabled. The map uses the game's scale and upscale method, and gameplay resumes with the selected SDK scale when the map closes.
- Scoped temporal upsampling to the local-player view that uses the SDK scale. Independent views keep their game-selected upscale methods.
- Verified the map cycle in test build 1.1.3 with FSR Quality and DLSS Quality at 1080p. Release 1.1.4 ships the same rendering code. [Test coverage](docs/CITY_MAP_TESTING.md)

## 1.1.0 — Sharpness Update

- Added a shared Sharpness control for DLAA, DLSS, FSR, and XeSS, including Native AA modes. The native Graphics menu offers 0% to 100% in 5% increments and 13 languages.
- Added an optional FidelityFX CAS-based D3D12 pass after successful temporal reconstruction and before game composition. At 0% the renderer bypasses the pass. If the pass cannot record, composition receives the reconstructed frame.
- Sharpness applies through the existing settings transaction and persists in `ReFrame/Settings.ini`. Changes to Sharpness alone leave temporal feature identity and history intact.
- CAS reuses bindings after their command lists and fences retire. The frame cadence log shows how many frames received CAS.
- Added offline regressions and verified Sharpness in-game with DLSS, FSR, and XeSS Performance at 1080p on an RTX 5060. [Test coverage](docs/COMPATIBILITY.md)
- Included localized release and installation guides in the ZIP. The uninstaller accepts the 1.1.0 manifest and verifies each packaged file before removal. [Release and installation guide](docs/INSTALLATION.md)

## 1.0.0

First public release candidate for Persona 3 Reload. The project is now **RE:FRAME — Persona 3 Reload**, by Aporia.

- Native graphics menu with DLAA, DLSS, FSR, XeSS and Original TAA. Includes 13 interface languages and automatic SDK render scale.
- Carries forward the 0.2.5 Rendering Scale fixes, including confirming a manual scale in the same save as a return to Original TAA.
- Loads earlier development settings when the new INI is absent and writes new settings to the RE:FRAME directory.
- Licenses the original RE:FRAME source code under [MIT](LICENSE). Bundled third-party components retain their own terms.
- Ships as a Windows x64 ZIP with a SHA-256 file, license notices and a hash-gated uninstaller. Shader file dumps and diagnostic hotkeys default to off.

See [Compatibility](docs/COMPATIBILITY.md) for earlier in-game evidence and 1.0.0 manual checks.
