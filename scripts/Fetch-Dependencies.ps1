[CmdletBinding()]
param([string]$DependencyRoot = (Join-Path $PSScriptRoot '..\.deps'))

$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'Dependencies.ps1')
$curl = (Get-Command curl.exe -CommandType Application -ErrorAction Stop).Source

foreach ($dependency in (Get-ReframeDependencyLock)) {
    $directory = Join-Path $DependencyRoot $dependency.directory
    foreach ($entry in $dependency.files.PSObject.Properties) {
        $destination = Join-Path $directory $entry.Name
        if ((Test-Path -LiteralPath $destination -PathType Leaf) -and
            (Get-FileHash -LiteralPath $destination -Algorithm SHA256).Hash -eq $entry.Value) { continue }

        New-Item -ItemType Directory -Force -Path (Split-Path -Parent $destination) | Out-Null
        $temporary = "$destination.$([guid]::NewGuid().ToString('N')).part"
        $uri = "https://raw.githubusercontent.com/$($dependency.repository)/$($dependency.commit)/$($entry.Name)"
        try {
            & $curl --fail --location --silent --show-error --retry 2 --output $temporary $uri
            if ($LASTEXITCODE -ne 0) { throw "Download failed: $uri" }
            if ((Get-FileHash -LiteralPath $temporary -Algorithm SHA256).Hash -ne $entry.Value) {
                throw "SHA-256 mismatch: $uri"
            }
            Move-Item -LiteralPath $temporary -Destination $destination -Force
            Write-Host "Verified $($dependency.directory)/$($entry.Name)"
        } finally {
            if (Test-Path -LiteralPath $temporary -PathType Leaf) { Remove-Item -LiteralPath $temporary }
        }
    }
}
Assert-ReframeDependencies $DependencyRoot
