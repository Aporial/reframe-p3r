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

### Sharpness

The accepted TAA/TAAU pass operates on the pre-tonemap main view. The Steam build's RGBA16F scene input and UAV output are validated alongside the engine's View pre-exposure and the origin-zero viewport. This path also supplies the later tonemapper when the game presents in SDR. The DLSS adapter sets HDR and supplies NGX pre-exposure. FSR 4.1.2 and 3.1.6 receive HDR color and pre-exposure, and return color in the input domain. XeSS receives reciprocal pre-exposure as its multiplier and reverses that scaling on output. FSR's optional sharpen pass and NGX's legacy sharpening flag are disabled. The pinned XeSS SR API exposes no separate sharpening control. Each SDK keeps its own history. The common pass consumes the resolved display rect after the SDK returns success and runs before the engine's composition and independent UI passes. [DLSS programming guide](https://raw.githubusercontent.com/NVIDIA/DLSS/main/doc/DLSS_Programming_Guide_Release.pdf) · [FSR 4 integration](https://gpuopen.com/manuals/fsr_sdk/techniques/super-resolution-ml/) · [FSR 3 integration](https://gpuopen.com/manuals/fsr_sdk/techniques/super-resolution-upscaler/) · [XeSS integration](https://www.intel.com/content/www/us/en/developer/articles/technical/xess-sr-developer-guide.html)

The CAS adaptation uses the upstream sharpen-only cross filter, with diagonal samples in the soft extrema. It computes an adaptive weight from a bounded luminance signal using View pre-exposure as the reference. It applies the luminance change in the original linear domain, bounds it by the local extrema, limits per-pixel gain, and preserves chromaticity and alpha. User strength fades in the filter and sweeps the CAS tuning range up to AMD's maximum peak of −1/5. The path uses FP32 math and preserves native FP16 storage for the validated game output. [FidelityFX CAS reference](https://github.com/GPUOpen-Effects/FidelityFX-CAS)

At a nonzero setting the renderer lazily prepares a compute PSO, descriptors and a scratch UAV. Distinct game outputs can have separate scratch textures, each of which follows the queue ordering of its paired game output. Four bindings per temporal feature bound the resource cache. When a new main-view output arrives and the cache is full, the least recently used retired binding can be reassigned. A binding becomes eligible when no command list references it and all tracked queue fences for it have completed. Matching size and format reuse the scratch texture when the output changes. A UAV-to-SRV transition orders reconstruction before CAS reads. After dispatch, the pass transitions both resources for a single rect copy back to the game output, then restores the UAV state expected by the game. The shader clamps all reads to the valid rect and leaves the allocated padding untouched. The cache is cleared after the checks used for temporal context retirement. Invalid formats, busy bindings, or pipeline and allocation errors skip CAS before it records any commands, so the game receives the ordinary SDK output. At 0% the renderer skips CAS allocation and recording, including the copy. The frame cadence log counts recorded CAS passes and reports the reason for any skipped pass.

## Menu and persistence

The mod uses the game's native graphics list and widgets. The entries follow the current draft, while changes are committed through the normal Apply or Cancel flow. Rendering Scale is editable for an Original TAA draft. With SDK modes it shows the selected SDK target, and the renderer checks the actual frame size. Returning from an SDK to Original TAA releases the override and applies the native scale chosen in the same settings transaction.

The four existing graphics keys and their numeric IDs are preserved. The shared `Sharpness` key stores an integer percentage and defaults to 0 when absent. The native slider is visible for the SDK modes and uses 5% steps. Sharpness is part of the draft and saved selection. Temporal plans and features receive only technology, quality and model. Applying a sharpness-only draft updates the render constant and saves the selection without touching timing or temporal settings. When `ReFrame/Settings.ini` is absent, the loader reads `P3RGraphicsEnhanced/Settings.ini`. Writes go to the new directory. Diagnostic and benchmark options are read only from `ReFrame`.
