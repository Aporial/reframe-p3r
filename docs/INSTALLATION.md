# RE:FRAME — Persona 3 Reload 1.0.0

By Aporia. For Persona 3 Reload on Steam, app ID 2161700, build 22672075, on Windows x64 with DirectX 12.

1. Close the game. Use Steam **Properties → Installed Files → Browse**.
2. Check `P3R\Binaries\Win64` for an existing `d3d12.dll`. Use the owning mod's uninstall instructions first.
3. Extract `REFRAME-P3R-v1.0.0.zip` into the root that contains the `P3R` folder. Launch from Steam.
4. In Graphics, select a technology and quality beneath **Rendering Scale** and confirm changes. Original TAA uses the manual slider. The SDK modes manage their own scale.

For upgrades from the development versions, retain `P3RGraphicsEnhanced\Settings.ini` until the new version saves to `ReFrame\Settings.ini`. The XeSS runtime needs Microsoft Visual C++ Redistributable x64 14.40.33810 or later.

To uninstall, close the game and run `Remove-ReFrame.ps1` from `P3R\Binaries\Win64\ReFrame`. It checks the release manifest and file hashes. The `-WhatIf` switch previews removals.

The companion `.sha256` file contains the ZIP checksum. `Licenses/REFRAME-MIT.txt` covers only RE:FRAME’s original source code. The bundled NVIDIA, AMD, Intel and MinHook components retain their respective terms in `Licenses`. The NVIDIA components are distributed with this mod under the NVIDIA RTX SDKs license.

RE:FRAME is an independent, unofficial fan-made project.
It is not affiliated with, endorsed by, or sponsored by ATLUS, SEGA, NVIDIA, AMD, or Intel.
All trademarks belong to their respective owners.

RE:FRAME requires a legally purchased copy of Persona 3 Reload. It does not distribute game files, bypass DRM, or unlock DLC.
