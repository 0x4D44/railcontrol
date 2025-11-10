Param(
  [ValidateSet('Debug','Release')][string]$Config = 'Debug',
  [switch]$Coverage,
  [switch]$MinimalSmoke
)

$ErrorActionPreference = 'Stop'

if ($MinimalSmoke) { $env:RAILCORE_SMOKE_MODE = 'minimal' }

if ($Coverage) {
  $env:COVERAGE = '1'
}

& cmd /c "build.bat $Config"
if ($LASTEXITCODE -ne 0) { throw "Build/tests failed with exit code $LASTEXITCODE" }

Write-Host "Done. Outputs under build/msvc/$Config" 

