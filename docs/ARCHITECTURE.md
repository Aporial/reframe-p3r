# Architecture

RE:FRAME is a Windows x64 D3D12 proxy and rendering integration scoped to the Steam build listed in [Compatibility](COMPATIBILITY.md).

| Area | Responsibility |
| --- | --- |
| `src/core` and `include/reframe` | DLL metadata, module paths, logging, common frame and setting types |
| `src/d3d12` | System D3D12 forwarding, device and command-list hooks, pipeline tracking and input validation |
| `src/render` and `shaders` | Frame dispatch, motion and exposure preparation, render scale and SDK context lifetime |
| `src/backends` | NVIDIA NGX/DLSS, AMD FSR and Intel XeSS capability queries and dispatch adapters |
| `src/menu` and `src/localization` | Native graphics rows, focus, controller and mouse input, translations |
| `src/config` and `src/unreal` | INI persistence, legacy settings fallback and supported-build engine variable bindings |

## Rendering

The DLL forwards the game's D3D12 entry points to the Windows system library. It tracks compute pipeline creation and dispatch to recognize the game build's TAA variants, and uses a full bytecode fingerprint to establish identity. The main-view checks verify viewport size, bound resources and render target formats. A compute shader prepares motion vectors and exposure. The selected SDK receives scene color, depth, motion, jitter and exposure and writes to the TAA output before later post-processing and UI. Original TAA remains the path for unrecognized shaders, secondary views and unsupported inputs.

Each backend reports availability and the target internal size. Native AA requires full output resolution. A transition can continue with Original TAA while the actual viewport catches up. The render path owns SDK context recreation and history resets. D3D12 submission tracking and fences guard context and resource lifetimes.

## Menu and persistence

The mod uses the game's native graphics list and widgets. The entries follow the current draft, while changes are committed through the normal Apply or Cancel flow. Rendering Scale is editable for an Original TAA draft. With SDK modes it shows the selected SDK target, and the renderer checks the actual frame size. Returning from an SDK to Original TAA releases the override and applies the native scale chosen in the same settings transaction.

The four existing graphics keys and their numeric IDs are preserved. When the new `ReFrame/Settings.ini` is absent, the loader reads the previous `P3RGraphicsEnhanced/Settings.ini`. Writes go to the new directory. Diagnostic and benchmark options are read only from `ReFrame`.
