# City map Super Resolution

RE:FRAME 1.1.5 supports upscaling the city map scene with the selected DLSS, FSR or XeSS mode. The city map markers and interface keep their output-resolution layout. The correction is enabled by default.

## Cause and correction

The map background could be clipped to the internal render extent with SDK upscaling. The early 1.1.5 captures showed that the SDK, the final background draw and its input texture already used the full output extent. The blur and vignette material still used the reciprocal internal view and scene-buffer sizes for its sample bounds. RE:FRAME checks for that material and its expected bindings, then uses the output dimensions in a private copy of its View constants for a single draw. Original TAA uses the game's view setup. See [architecture](ARCHITECTURE.md) for the resource lifetime and fallback.

## In-game results

The test system ran Steam build 22672075 on a GeForce RTX 5060 at 1920×1080. The r4 test log recorded an SDK dispatch and the corresponding correction while the map was open in each mode below.

| Mode | Internal viewport | Output | SDK recording after map close in r4 |
| --- | --- | --- | --- |
| DLSS Quality | 1280×720 | 1920×1080 | Confirmed |
| FSR 3.1.6 Quality | 1280×720 | 1920×1080 | Confirmed |
| XeSS Quality | 1130×636 | 1920×1080 | Confirmed |
| DLSS Performance | 960×540 | 1920×1080 | Confirmed |
| FSR 3.1.6 Performance | 960×540 | 1920×1080 | Log ends with the map open |

The player confirmed that the DLSS Quality background filled the map and completed the FSR and XeSS follow-up checks. The XeSS viewport occupied part of a 1132×636 input allocation.

The shipped 1.1.5 DLL matched the in-game test DLL by SHA-256. With the mod's defaults, a separate game session recorded the correction at 960×540 to 1920×1080 in FSR 3.1.6 Performance, with further FSR frames during the visit. The log ends while the map is open. Map exit and the stock-scale fallback still need direct checks on the shipped DLL. Other resolutions and vendor-specific Radeon and Intel Arc paths also await manual checks.

## Manual check

1. At 1920×1080, select DLSS, FSR or XeSS Quality in Graphics. Apply the setting and open the city map.
2. Move the location selection. Check that the background fills the map and the labels stay aligned.
3. Close the map and leave the 3D scene active for five seconds. In the log, check for a corrected background draw, a map-close entry and subsequent SDK frames at the selected scale.
