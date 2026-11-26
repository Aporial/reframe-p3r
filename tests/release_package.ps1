[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Archive
)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$archivePath = (Resolve-Path -LiteralPath $Archive).Path
$checksum = ((Get-Content -LiteralPath "$archivePath.sha256" -Raw).Trim() -split '\s+')[0]
if ((Get-FileHash -LiteralPath $archivePath -Algorithm SHA256).Hash -ne $checksum) {
    throw 'Archive checksum mismatch.'
}

$testRoot = Join-Path $root ('build/PackageTest-' + [guid]::NewGuid().ToString('N'))
$pristine = Join-Path $testRoot 'pristine'
Expand-Archive -LiteralPath $archivePath -DestinationPath $pristine
$bin = Join-Path $pristine 'P3R/Binaries/Win64'
$manifestPath = Join-Path $bin 'ReFrame/Installed.json'
$manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
if ($manifest.Project -cne 'reframe-p3r' -or (Get-Item -LiteralPath (Join-Path $bin 'd3d12.dll')).VersionInfo.ProductVersion -ne $manifest.Version) {
    throw 'Unexpected release manifest or DLL version.'
}
$expected = [Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
[void]$expected.Add('P3R/Binaries/Win64/ReFrame/Installed.json')
foreach ($entry in $manifest.Files) {
    if ($entry.Name -notmatch '^(d3d12\.dll|ReFrame/(Licenses/|i18n/)?[A-Za-z0-9_.-]+)$' -or
        !$expected.Add("P3R/Binaries/Win64/$($entry.Name)")) { throw "Unexpected path: $($entry.Name)" }
    if ((Get-FileHash -LiteralPath (Join-Path $bin $entry.Name) -Algorithm SHA256).Hash -cne $entry.SHA256) {
        throw "Incorrect manifest hash: $($entry.Name)"
    }
}
$actual = Get-ChildItem -LiteralPath $pristine -Recurse -File -Force | ForEach-Object {
    $_.FullName.Substring($pristine.Length + 1).Replace('\', '/')
}
if (!$expected.SetEquals([string[]]$actual)) { throw 'ZIP contains missing or unlisted files.' }

$readme = Get-Content -LiteralPath (Join-Path $bin 'ReFrame/README.md') -Raw -Encoding UTF8
$guides = @(Get-ChildItem -LiteralPath (Join-Path $root 'docs/i18n') -Filter 'INSTALLATION.*.md' -File)
if ($guides.Count -ne 13) { throw 'Expected 13 translated guides in addition to English.' }
foreach ($guide in $guides) {
    $path = "i18n/$($guide.Name)"
    if (!$readme.Contains("($path)") -or !$expected.Contains("P3R/Binaries/Win64/ReFrame/$path")) {
        throw "Missing packaged guide or language link: $path"
    }
    $doc = Get-Content -LiteralPath (Join-Path $bin "ReFrame/$path") -Raw -Encoding UTF8
    if (!$doc.Contains('REFRAME-P3R-v' + $manifest.Version + '.zip') -or !$doc.Contains('FidelityFX CAS')) {
        throw "Guide has stale release details: $path"
    }
}

# This executable placeholder meets the uninstaller's location check. It is never run.
'Fixture only' | Set-Content -LiteralPath (Join-Path $bin 'P3R.exe') -Encoding ascii
'[Graphics]', 'Sharpness=50' | Set-Content -LiteralPath (Join-Path $bin 'ReFrame/Settings.ini') -Encoding ascii
'Test log' | Set-Content -LiteralPath (Join-Path $bin 'ReFrame/ReFrame.log') -Encoding ascii
$modified = Join-Path $testRoot 'modified'
Copy-Item -LiteralPath $bin -Destination $modified -Recurse

$uninstall = Join-Path $bin 'ReFrame/Remove-ReFrame.ps1'
& $uninstall -WhatIf 6>$null
foreach ($entry in $manifest.Files) {
    if (!(Test-Path -LiteralPath (Join-Path $bin $entry.Name) -PathType Leaf)) {
        throw "WhatIf removed a file: $($entry.Name)"
    }
}
& $uninstall
foreach ($entry in @($manifest.Files.Name) + @('ReFrame/Installed.json')) {
    if (Test-Path -LiteralPath (Join-Path $bin $entry)) { throw "Uninstall left a packaged file: $entry" }
}
foreach ($entry in @('P3R.exe','ReFrame/Settings.ini','ReFrame/ReFrame.log')) {
    if (!(Test-Path -LiteralPath (Join-Path $bin $entry) -PathType Leaf)) {
        throw "Uninstall removed a file outside the manifest: $entry"
    }
}

$editedGuide = Join-Path $modified 'ReFrame/i18n/INSTALLATION.uk-UA.md'
'Local edit' | Add-Content -LiteralPath $editedGuide
& (Join-Path $modified 'ReFrame/Remove-ReFrame.ps1') 3>$null
if (!(Test-Path -LiteralPath $editedGuide) -or !(Test-Path -LiteralPath (Join-Path $modified 'ReFrame/Installed.json')) -or
    (Test-Path -LiteralPath (Join-Path $modified 'd3d12.dll'))) {
    throw 'Uninstall did not preserve the edited guide or retire the original proxy correctly.'
}
"Verified package SHA-256, $($manifest.Files.Count) files, 14 guide languages and uninstall behavior."
