# Troubleshooting

## The menu entries are missing

Check that the mod DLL is beside `P3R.exe` in `P3R\Binaries\Win64`, and that the game is Steam build 22672075. If another mod already uses `d3d12.dll`, use its uninstall instructions before installing RE:FRAME.

## One of the SDKs is absent

Check its runtime files in `P3R\Binaries\Win64\ReFrame` and update the GPU driver for the active adapter. XeSS needs the Microsoft x64 Visual C++ Redistributable listed in the README. Restart the game after a driver or runtime change.

## Rendering Scale is locked or differs from the viewport

With an SDK mode selected, the in-menu value is the SDK target. After applying and returning to the scene, check the `evaluated` or `Main-view cadence` entries in `ReFrame.log` for actual dimensions. Choose Original TAA to regain the manual slider. If the log says the viewport failed to converge, record the old manual scale, chosen backend and mode, and output resolution.

## Artifact, crash or fallback

Try the same scene with Original TAA. Report the Steam build, GPU, driver, exact mode and steps along with a relevant, redacted log excerpt. Remove machine-specific paths and account data before posting.

Diagnostics are opt-in. For developer-directed tracing, `ReFrame\Diagnostic.ini` can enable `Hotkeys=1` or `ShaderDumps=1` under `[Diagnostic]`. F7, F8 and F9 are developer shortcuts. Shader captures can contain game material and should stay local. `ReFrame\Benchmark.ini` supports `Uncapped=1` under `[Benchmark]` for a temporary benchmark session.

## Restore the original game

Close the game and run `ReFrame\Remove-ReFrame.ps1` from the installation. Use `-WhatIf` to preview. If the proxy hash differs from the manifest, identify which mod owns that DLL before removing anything.
