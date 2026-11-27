[CmdletBinding(SupportsShouldProcess)]
param()

$ErrorActionPreference = 'Stop'
$data = $PSScriptRoot
$bin = Split-Path -Parent $data
if ((Split-Path -Leaf $data) -ne 'ReFrame' -or !(Test-Path -LiteralPath (Join-Path $bin 'P3R.exe') -PathType Leaf)) {
    throw 'Run this script from P3R\Binaries\Win64\ReFrame.'
}
if (Get-Process -Name P3R -ErrorAction SilentlyContinue) { throw 'Close Persona 3 Reload before uninstalling.' }

function Get-SafeReframeItem([string]$Path) {
    $entry = Get-Item -LiteralPath $Path -Force -ErrorAction SilentlyContinue
    if ($entry -and ($entry.Attributes -band [IO.FileAttributes]::ReparsePoint)) {
        throw "Refusing to follow a symbolic link or junction: $Path"
    }
    $entry
}

foreach ($folder in @($bin, $data, (Join-Path $data 'Licenses'), (Join-Path $data 'i18n'))) {
    $entry = Get-SafeReframeItem $folder
    if ($entry -and !$entry.PSIsContainer) { throw "Expected a directory: $folder" }
}
$manifestPath = Join-Path $data 'Installed.json'
$manifestItem = Get-SafeReframeItem $manifestPath
if (!$manifestItem -or $manifestItem.PSIsContainer) { throw 'The RE:FRAME manifest is missing.' }
$manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
$allowed = @(
    'd3d12.dll',
    'ReFrame/nvngx_dlss.dll',
    'ReFrame/amd_fidelityfx_loader_dx12.dll',
    'ReFrame/amd_fidelityfx_upscaler_dx12.dll',
    'ReFrame/libxess.dll',
    'ReFrame/README.md',
    'ReFrame/Remove-ReFrame.ps1',
    'ReFrame/Licenses/REFRAME-MIT.txt',
    'ReFrame/Licenses/NVIDIA.txt',
    'ReFrame/Licenses/AMD.md',
    'ReFrame/Licenses/AMD-ThirdParty.md',
    'ReFrame/Licenses/Intel.txt',
    'ReFrame/Licenses/Intel-ThirdParty.txt',
    'ReFrame/Licenses/MinHook.txt'
)
switch -CaseSensitive ($manifest.Version) {
    '1.0.0' {}
    { $_ -cin @('1.1.0','1.1.1','1.1.2','1.1.3','1.1.4','1.1.5') } {
        $allowed += 'ReFrame/Licenses/AMD-FidelityFX-CAS.txt'
        if ($manifest.Version -ceq '1.1.0' -and @($manifest.Files).Count -eq ($allowed.Count + 1)) {
            $allowed += 'ReFrame/Sharpness-Testing.md' # The 1.1.0 test archive.
        } else {
            foreach ($language in @('de-DE','es-ES','fr-FR','it-IT','ja-JP','ko-KR','pl-PL','pt-BR','ru-RU','tr-TR','uk-UA','zh-CN','zh-TW')) {
                $allowed += "ReFrame/i18n/INSTALLATION.$language.md"
            }
        }
    }
    default { throw 'The RE:FRAME manifest version is unsupported.' }
}
if ($manifest.Project -cne 'reframe-p3r' -or @($manifest.Files).Count -ne $allowed.Count) {
    throw 'The manifest is not a RE:FRAME release manifest.'
}
$seen = [Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
$payload = foreach ($entry in $manifest.Files) {
    if ($entry.Name -cnotin $allowed -or $entry.SHA256 -cnotmatch '^[0-9A-F]{64}$' -or !$seen.Add($entry.Name)) {
        throw 'The manifest contains an unexpected path or checksum.'
    }
    $file = Join-Path $bin $entry.Name
    $item = Get-SafeReframeItem $file
    if ($item -and $item.PSIsContainer) { throw "Expected a file: $file" }
    if (!$item) { continue }
    $matchesHash = (Get-FileHash -LiteralPath $file -Algorithm SHA256).Hash -ceq $entry.SHA256
    if ($entry.Name -ceq 'd3d12.dll' -and (!$matchesHash -or $item.VersionInfo.InternalName -cne 'ReFrame')) {
        throw 'The installed d3d12.dll has been replaced. No files were removed.'
    }
    [pscustomobject]@{ Path = $file; Matches = $matchesHash; Name = $entry.Name }
}

$complete = $true
$uninstaller = $null
foreach ($entry in $payload) {
    if ($entry.Name -ceq 'ReFrame/Remove-ReFrame.ps1') { $uninstaller = $entry; continue }
    if (!$entry.Matches) {
        Write-Warning "Keeping changed file: $($entry.Path)"
        $complete = $false
        continue
    }
    if ($PSCmdlet.ShouldProcess($entry.Path, 'Remove RE:FRAME file')) {
        Remove-Item -LiteralPath $entry.Path
    } else { $complete = $false }
}
if ($complete -and $uninstaller -and !$uninstaller.Matches) {
    Write-Warning 'Keeping the modified uninstaller and its manifest.'
    $complete = $false
}
if ($complete -and $PSCmdlet.ShouldProcess($manifestPath, 'Remove RE:FRAME manifest and uninstaller')) {
    if ($uninstaller) { Remove-Item -LiteralPath $uninstaller.Path }
    Remove-Item -LiteralPath $manifestPath
}
