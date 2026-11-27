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
- For 1.1.0, the CPU/INI regressions and the synthetic HDR FP16 shader fixture passed on GeForce RTX 5060 and WARP. All 14 available SDK modes on the RTX 5060 passed standalone dispatch tests at 1920×1080. The test covered FSR 3.1.6 and cross-vendor XeSS.
- The manual game test on Steam build 22672075 confirmed that Apply changed Sharpness in Performance modes at 1920×1080 on the RTX 5060. At 100%, the game logged CAS for 720/720 sampled frames with FSR 3.1.6 and 240/240 apiece with DLSS and XeSS. At 0%, sampled FSR frames bypassed CAS. The tester saw an image difference after the binding fix. Menu layout across every language, HUD and scene transitions, and controlled game frametime comparisons remain open. [Manual Sharpness checks](SHARPNESS_TESTING.md)

- The city map fix was exercised on Steam build 22672075 at 1080p on the RTX 5060 with test build 1.1.3. The map used a full 1920×1080 stock view in FSR 3.1.6 Quality, DLSS Quality and cross-vendor XeSS Quality. The journal also confirmed that FSR and DLSS resumed 1280×720 → 1920×1080 reconstruction after closing the map. Release 1.1.4 uses the same rendering code and was checked offline. Other quality modes and output resolutions await manual checks. [City map verification](CITY_MAP_TESTING.md)

- Test build 1.1.5 r4 reconstructed the city map at 1920×1080 from reduced DLSS, FSR and XeSS inputs. The packaged 1.1.5 DLL also recorded the map correction with FSR 3.1.6 Performance at 960×540 → 1920×1080 using its defaults. [City map Super Resolution test](TOWN_MAP_UPSCALING_TEST.md)

### Sharpness fixture on GeForce RTX 5060

These are means from 24 synthetic D3D12 samples at 50% Sharpness. GPU time includes CAS and one copy. CPU time covers recording after warm-up. Scratch allocation is per output binding and excludes the scene, SDK and fixture buffers. The D3D12 debug layer was unavailable on the test machine.

| Output | GPU time | CPU recording | Scratch allocation |
| --- | ---: | ---: | ---: |
| 1280×720 | 0.042 ms | 5.6 µs | 7.50 MiB |
| 1920×1080 | 0.084 ms | 6.7 µs | 16.88 MiB |
| 2560×1440 | 0.146 ms | 6.9 µs | 30.00 MiB |
| 3840×2160 | 0.325 ms | 8.7 µs | 63.75 MiB |
| 3440×1440 | 0.196 ms | 7.8 µs | 40.31 MiB |

## Manual checks for 1.0.0

- Steam launch on build 22672075 and game-thread navigation in the main and pause menus.
- DLAA and each SDK's available modes. Compare the menu target against the actual viewport log.
- With Original TAA at 75%, apply DLAA. Return to Original TAA and choose 100% in the same save. Repeat with one AMD and one Intel mode.
- Cancel a draft, restart with a saved selection, and verify settings fallback from a 0.2.5 INI.
- Resolution and window-mode changes, camera cuts, battle effects, reflections, thin geometry and native-resolution UI.
- Focus, labels and truncation in all 13 languages, particularly long translations and CJK glyphs.
- On supported hardware, check Radeon FSR 4 and Intel-optimized XeSS availability and rendering.
