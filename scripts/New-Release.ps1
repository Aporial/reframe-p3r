[CmdletBinding()]
param(
    [string]$DependencyRoot = (Join-Path $PSScriptRoot '..\.deps'),
    [string]$BuildDirectory = (Join-Path $PSScriptRoot '..\build\Release'),
    [string]$OutputDirectory = (Join-Path $PSScriptRoot '..\dist')
)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
. (Join-Path $PSScriptRoot 'Dependencies.ps1')
Assert-ReframeDependencies $DependencyRoot
$versionHeader = Get-Content -Raw -LiteralPath (Join-Path $root 'include/reframe/version.h')
if ($versionHeader -notmatch '(?m)^#define REFRAME_VERSION_STRING "(\d+\.\d+\.\d+)"') { throw 'No release version found.' }
$version = $Matches[1]
$dll = Join-Path $BuildDirectory 'd3d12.dll'
$metadata = (Get-Item -LiteralPath $dll).VersionInfo
if ($metadata.ProductVersion -ne $version -or $metadata.InternalName -ne 'ReFrame') {
    throw 'Build the matching x64 Release DLL first.'
}

$archiveName = "REFRAME-P3R-v$version.zip"
$archive = Join-Path $OutputDirectory $archiveName
$stage = Join-Path $OutputDirectory "REFRAME-P3R-v$version-stage"
if ((Test-Path -LiteralPath $stage) -or (Test-Path -LiteralPath $archive)) {
    throw 'The release stage or archive already exists. Use a new output directory.'
}
$bin = Join-Path $stage 'P3R/Binaries/Win64'
New-Item -ItemType Directory -Path (Join-Path $bin 'ReFrame/Licenses') -Force | Out-Null
New-Item -ItemType Directory -Path (Join-Path $bin 'ReFrame/i18n') -Force | Out-Null
$files = [ordered]@{
    'd3d12.dll' = $dll
    'ReFrame/nvngx_dlss.dll' = (Join-Path $DependencyRoot 'nvidia-v310.9.16/lib/Windows_x86_64/rel/nvngx_dlss.dll')
    'ReFrame/amd_fidelityfx_loader_dx12.dll' = (Join-Path $DependencyRoot 'fsr-v2.4.0/Kits/FidelityFX/signedbin/amd_fidelityfx_loader_dx12.dll')
    'ReFrame/amd_fidelityfx_upscaler_dx12.dll' = (Join-Path $DependencyRoot 'fsr-v2.4.0/Kits/FidelityFX/signedbin/amd_fidelityfx_upscaler_dx12.dll')
    'ReFrame/libxess.dll' = (Join-Path $DependencyRoot 'xess-v3.0.2/bin/libxess.dll')
    'ReFrame/README.md' = (Join-Path $root 'docs/INSTALLATION.md')
    'ReFrame/Remove-ReFrame.ps1' = (Join-Path $PSScriptRoot 'Remove-ReFrame.ps1')
    'ReFrame/Licenses/REFRAME-MIT.txt' = (Join-Path $root 'LICENSE')
    'ReFrame/Licenses/NVIDIA.txt' = (Join-Path $DependencyRoot 'nvidia-v310.9.16/LICENSE.txt')
    'ReFrame/Licenses/AMD.md' = (Join-Path $DependencyRoot 'fsr-v2.4.0/docs/license.md')
    'ReFrame/Licenses/AMD-ThirdParty.md' = (Join-Path $DependencyRoot 'fsr-v2.4.0/3rdpartynotice.md')
    'ReFrame/Licenses/AMD-FidelityFX-CAS.txt' = (Join-Path $root 'licenses/AMD-FidelityFX-CAS.txt')
    'ReFrame/Licenses/Intel.txt' = (Join-Path $DependencyRoot 'xess-v3.0.2/LICENSE.txt')
    'ReFrame/Licenses/Intel-ThirdParty.txt' = (Join-Path $DependencyRoot 'xess-v3.0.2/third-party-programs.txt')
    'ReFrame/Licenses/MinHook.txt' = (Join-Path $DependencyRoot 'MinHook-1.3.4/LICENSE.txt')
}
$guideLanguages = @('de-DE','es-ES','fr-FR','it-IT','ja-JP','ko-KR','pl-PL','pt-BR','ru-RU','tr-TR','uk-UA','zh-CN','zh-TW')
foreach ($language in $guideLanguages) {
    $guide = "i18n/INSTALLATION.$language.md"
    $files["ReFrame/$guide"] = Join-Path $root "docs/$guide"
}
$manifest = [ordered]@{
    Project = 'reframe-p3r'
    Version = $version
    Files = @(
        foreach ($entry in $files.GetEnumerator()) {
            $target = Join-Path $bin $entry.Key
            if ($entry.Key -eq 'ReFrame/README.md' -or $entry.Key -like 'ReFrame/i18n/INSTALLATION.*.md') {
                $guide = Get-Content -LiteralPath $entry.Value -Raw -Encoding UTF8
                $guide = $guide -replace 'REFRAME-P3R-v\d+\.\d+\.\d+\.zip', $archiveName
                $guide = $guide -replace '(?m)^(# [^\r\n]+?[ \t])\d+\.\d+\.\d+(?=[ \t·\r\n])', ('${1}' + $version)
                [IO.File]::WriteAllText($target, $guide, [Text.UTF8Encoding]::new($false))
            } else {
                Copy-Item -LiteralPath $entry.Value -Destination $target
            }
            [ordered]@{ Name = $entry.Key; SHA256 = (Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash }
        }
    )
}
$manifest | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $bin 'ReFrame/Installed.json') -Encoding UTF8
Compress-Archive -LiteralPath (Join-Path $stage 'P3R') -DestinationPath $archive -CompressionLevel Optimal
$sha = (Get-FileHash -LiteralPath $archive -Algorithm SHA256).Hash.ToLowerInvariant()
"$sha  $archiveName" | Set-Content -LiteralPath "$archive.sha256" -Encoding ascii
Get-Item -LiteralPath $archive,"$archive.sha256"
