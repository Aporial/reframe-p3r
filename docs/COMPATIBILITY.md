# Compatibility

## Game and platform

| Item | Scope |
| --- | --- |
| Game | Persona 3 Reload on Steam, app ID 2161700 |
| Supported build | 22672075 |
| Runtime | Windows x64, DirectX 12 |
| Unsupported configurations | Other game builds or stores, non-Windows runtimes, and stacking D3D12 proxies have no verified support |

See the [README](../README.md#requirements) for per-backend GPU and runtime requirements. The game and backend capability checks determine what the menu offers.

## Evidence

- Earlier development builds exercised the 14 SDK modes on NVIDIA GeForce RTX 5060 at 1920×1080. The FSR path reported 3.1.6 and XeSS used its cross-vendor implementation.
- Earlier builds exercised the native menu and persisted a vendor selection after restart. The 0.2.4 log verified full-resolution DLAA. The 0.2.5 manual Rendering Scale commit correction was compiled and checked with a CPU fixture. It did not receive a confirmed in-game result.
- For 1.0.0, the DLL, metadata, pinned dependencies and archive were checked offline. Version 1.0.0 has no game or GPU test results.
- Radeon FSR 4.1.2 and Intel's optimized XeSS path are SDK capabilities that still require mod testing on compatible hardware.

## Manual checks for 1.0.0

- Steam launch on build 22672075 and game-thread navigation in the main and pause menus.
- DLAA and each SDK's available modes. Compare the menu target against the actual viewport log.
- With Original TAA at 75%, apply DLAA. Return to Original TAA and choose 100% in the same save. Repeat with one AMD and one Intel mode.
- Cancel a draft, restart with a saved selection, and verify settings fallback from a 0.2.5 INI.
- Resolution and window-mode changes, camera cuts, battle effects, reflections, thin geometry and native-resolution UI.
- Focus, labels and truncation in all 13 languages, particularly long translations and CJK glyphs.
- On supported hardware, check Radeon FSR 4 and Intel-optimized XeSS availability and rendering.
