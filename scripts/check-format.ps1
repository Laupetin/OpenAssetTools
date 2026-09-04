#!powershell

$ErrorActionPreference = "Stop"

$repositoryRoot = Split-Path -Parent $PSScriptRoot
$sourceDirectories = @(
    (Join-Path $repositoryRoot "src")
    (Join-Path $repositoryRoot "test")
)

$clangFormat = if ($env:CLANG_FORMAT_BIN) {
    $env:CLANG_FORMAT_BIN
}
else {
    "clang-format"
}

$files = @(
    Get-ChildItem -Path $sourceDirectories -Recurse -File |
        Where-Object { $_.Extension -in ".h", ".cpp" }
)

# Stay comfortably below Windows' native process command-line limit.
$batchSize = 100

for ($offset = 0; $offset -lt $files.Count; $offset += $batchSize) {
    $lastIndex = [Math]::Min($offset + $batchSize - 1, $files.Count - 1)
    $batch = $files[$offset..$lastIndex].FullName

    & $clangFormat -Werror -ferror-limit=1 --dry-run $batch

    if ($LASTEXITCODE -ne 0) {
        exit $LASTEXITCODE
    }
}