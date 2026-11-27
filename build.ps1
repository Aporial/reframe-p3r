[CmdletBinding()]
param(
    [string]$DependencyRoot = (Join-Path $PSScriptRoot '.deps'),
    [string]$BuildDirectory = (Join-Path $PSScriptRoot 'build/Release'),
    [switch]$GpuSmokeTest,
    [switch]$SharpnessTests,
    [switch]$ViewScaleTests
)
$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot
. (Join-Path $root 'scripts/Dependencies.ps1')
Assert-ReframeDependencies $DependencyRoot
$dependencies = (Resolve-Path -LiteralPath $DependencyRoot).Path
$nvidia = Join-Path $dependencies 'nvidia-v310.9.16'
$minHook = Join-Path $dependencies 'MinHook-1.3.4'
$fsr = Join-Path $dependencies 'fsr-v2.4.0/Kits/FidelityFX'
$xess = Join-Path $dependencies 'xess-v3.0.2'
$nvidiaLib = Join-Path $nvidia 'lib/Windows_x86_64/x64/nvsdk_ngx_s.lib'

if ($env:VSCMD_ARG_TGT_ARCH -ne 'x64') { throw 'Open the x64 Native Tools Command Prompt for Visual Studio.' }
$cl = (Get-Command cl.exe -CommandType Application).Source
$rc = (Get-Command rc.exe -CommandType Application).Source
$fxc = (Get-Command fxc.exe -CommandType Application).Source
New-Item -ItemType Directory -Force -Path $BuildDirectory | Out-Null
$out = (Resolve-Path -LiteralPath $BuildDirectory).Path
$includes = @('include/reframe','src/backends','src/config','src/menu','src/localization','src/render','src/unreal') |
    ForEach-Object { "/I$(Join-Path $root $_)" }
$includes += @("/I$out","/I$minHook/include","/I$nvidia/include","/I$fsr/api/include","/I$fsr/upscalers/include","/I$xess/inc")

function Invoke-Tool([string]$Tool, [string[]]$Arguments) {
    & $Tool @Arguments
    if ($LASTEXITCODE -ne 0) { throw "$(Split-Path -Leaf $Tool) exited with code $LASTEXITCODE." }
}

Push-Location -LiteralPath $out
try {
    Invoke-Tool $fxc @('/nologo','/T','cs_5_0','/E','main','/O3','/Fh','motion_cs.h','/Vn','MotionCs',
                       (Join-Path $root 'shaders/motion_vectors.hlsl'))
    Invoke-Tool $fxc @('/nologo','/T','cs_5_0','/E','main','/O3','/Fh','sharpness_cs.h','/Vn','SharpnessCs',
                       (Join-Path $root 'shaders/sharpness.hlsl'))
    Invoke-Tool $rc @('/nologo',"/I$root/include/reframe",'/fo','version.res',(Join-Path $root 'src/core/version.rc'))
    $c = @('buffer.c','hook.c','trampoline.c','hde/hde64.c') | ForEach-Object { Join-Path $minHook "src/$_" }
    Invoke-Tool $cl (@('/nologo','/c','/O2','/MT','/W3',"/I$minHook/include") + $c)
    Invoke-Tool $cl @('/nologo','/c','/O2','/MT','/W4',(Join-Path $root 'src/d3d12/d3d12_slots.c'))
    $cpp = Get-ChildItem -Recurse -File -Filter '*.cpp' -LiteralPath (Join-Path $root 'src') | Sort-Object FullName |
        ForEach-Object FullName
    $options = @('/nologo','/O2','/MT','/std:c++20','/EHsc','/W4','/permissive-','/utf-8','/DNDEBUG',
                 '/DWIN32_LEAN_AND_MEAN','/DNOMINMAX')
    $libraries = @($nvidiaLib,'user32.lib','advapi32.lib','shell32.lib','shlwapi.lib','crypt32.lib','bcrypt.lib',
                   'dxgi.lib','dxguid.lib','version.lib')
    $objects = @('buffer.obj','hook.obj','trampoline.obj','hde64.obj','d3d12_slots.obj','version.res')
    Invoke-Tool $cl ($options + @('/LD') + $includes + $cpp + $objects + $libraries +
        @('/link',"/DEF:$root/src/d3d12/d3d12.def",'/MACHINE:X64','/OUT:d3d12.dll','/INCREMENTAL:NO'))
    if ($GpuSmokeTest) {
        $test = @('tests/gpu_smoke.cpp','src/core/platform.cpp','src/backends/ngp.cpp','src/backends/fsr.cpp',
                  'src/backends/xess.cpp','src/render/sharpness.cpp') | ForEach-Object { Join-Path $root $_ }
        Invoke-Tool $cl ($options + $includes + $test + $libraries +
            @('/link','/MACHINE:X64','/OUT:gpu_smoke.exe','/INCREMENTAL:NO'))
    }
    if ($ViewScaleTests) {
        Invoke-Tool $cl ($options + $includes + @((Join-Path $root 'tests/view_scale.cpp'),
            '/link','/MACHINE:X64','/OUT:view_scale_tests.exe','/INCREMENTAL:NO'))
        Invoke-Tool (Join-Path $out 'view_scale_tests.exe') @()
    }
    if ($SharpnessTests) {
        $test = @('tests/sharpness_tests.cpp','src/config/settings.cpp','src/core/platform.cpp',
                  'src/render/sharpness.cpp') | ForEach-Object { Join-Path $root $_ }
        Invoke-Tool $cl ($options + $includes + $test + @('dxgi.lib','dxguid.lib',
            '/link','/MACHINE:X64','/OUT:sharpness_tests.exe','/INCREMENTAL:NO'))
    }
} finally {
    Pop-Location
}
