function Get-ReframeDependencyLock {
    $lock = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'dependencies.lock.json') -Raw | ConvertFrom-Json
    if ($lock.schema -ne 1) { throw 'Unsupported dependency lock schema.' }
    $lock.dependencies
}

function Assert-ReframeDependencies([string]$DependencyRoot) {
    foreach ($dependency in (Get-ReframeDependencyLock)) {
        $directory = Join-Path $DependencyRoot $dependency.directory
        foreach ($entry in $dependency.files.PSObject.Properties) {
            $path = Join-Path $directory $entry.Name
            if (!(Test-Path -LiteralPath $path -PathType Leaf) -or
                (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash -ne $entry.Value) {
                throw "Missing or invalid pinned dependency: $($dependency.directory)/$($entry.Name). Run scripts\Fetch-Dependencies.ps1."
            }
        }
    }
}
