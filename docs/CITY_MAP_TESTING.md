# City map scaling

## Issue and fix

With Super Resolution enabled, the city map's 3D background could be clipped to the internal render extent in the upper-left corner. Pins, labels and HUD controls still used the output layout. Original TAA at 75% displayed the full map, which established that the game can compose the map correctly at a reduced stock scale.

The legacy viewport driver selects the scale before the local-player view is created. Scoping the scale to the gameplay driver in test build 1.1.1 still clipped the map. Test build 1.1.2 also tried to recognize the map camera at view construction. The manual test still reproduced the issue and the journal recorded no camera match.

The fix reads the game's `TownMap` pause state when the viewport constructs its scale driver. A map with Super Resolution selected uses the game's scale and upscale method. The selected SDK scale applies again when gameplay resumes. Outside the map, TAAU is selected for the local-player view associated with the SDK scale driver. Other view families keep their engine-selected upscale methods, and the game retains control of `r.TemporalAA.Upsampling`. [Unreal view families](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/FSceneViewFamily) · [Unreal primary scale methods](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/EPrimaryScreenPercentageMethod)

## Verified in-game

The tester confirmed that the clipping was gone with test build 1.1.3 on Steam build 22672075 at 1920×1080 on a GeForce RTX 5060. The journal recorded these two map cycles with FSR 3.1.6 and NVIDIA DLSS.

| Mode | Gameplay before opening the map | Map render and output at stock scale | Gameplay after closing the map |
| --- | --- | --- | --- |
| FSR Quality | 1280×720 → 1920×1080 | 1920×1080 → 1920×1080 | 1280×720 → 1920×1080 |
| DLSS Quality (model K) | 1280×720 → 1920×1080 | 1920×1080 → 1920×1080 | 1280×720 → 1920×1080 |

Release 1.1.4 ships the same rendering code. Its Windows x64 build, CPU regressions, ZIP and uninstaller are verified offline. XeSS, other quality modes, other output resolutions and additional screens await dedicated in-game checks of this fix.

## Manual checks

1. At a location with access to the city map, check Original TAA at 75% as the control. The background, pins and HUD should align.
2. Open and close the map in FSR Quality, DLSS Quality and XeSS Quality. Compare the 3D background with the control, including the bottom and right edges. Check that gameplay resumes with the selected mode.
3. Repeat with Native AA, other quality modes and a second output resolution. Check a menu with a 3D preview if available.

In `P3R/Binaries/Win64/ReFrame/ReFrame.log`, look for `TownMap pause active; stock view scale` and then `Gameplay view family uses` and SDK cadence after closing the map.
