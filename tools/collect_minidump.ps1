param(
  [string]$OutputPath = ""
)

function Resolve-OutputPath {
  param(
    [string]$UserPath
  )

  if ([string]::IsNullOrWhiteSpace($UserPath)) {
    $timestamp = Get-Date -Format "yyyyMMdd-HHmmss"
    return Join-Path -Path (Get-Location) -ChildPath ("railcontrol-diagnostics-$timestamp.zip")
  }

  if (Test-Path -LiteralPath $UserPath) {
    if ((Get-Item -LiteralPath $UserPath).PSIsContainer) {
      $timestamp = Get-Date -Format "yyyyMMdd-HHmmss"
      return Join-Path -Path $UserPath -ChildPath ("railcontrol-diagnostics-$timestamp.zip")
    }
  }

  $directory = Split-Path -Parent $UserPath
  if (-not [string]::IsNullOrWhiteSpace($directory)) {
    if (-not (Test-Path -LiteralPath $directory)) {
      New-Item -ItemType Directory -Path $directory -Force | Out-Null
    }
  }

  return $UserPath
}

$basePath = Join-Path -Path $env:LOCALAPPDATA -ChildPath "RailControl"
if (-not (Test-Path -LiteralPath $basePath)) {
  Write-Warning "No diagnostics directory found at $basePath"
  exit 1
}

$logDir = Join-Path -Path $basePath -ChildPath "Logs"
$dumpDir = Join-Path -Path $basePath -ChildPath "Dumps"

$artifactPaths = @()

if (Test-Path -LiteralPath $logDir) {
  $artifactPaths += Get-ChildItem -Path $logDir -Filter "memory_guard.log*" -ErrorAction SilentlyContinue
  $artifactPaths += Get-ChildItem -Path $logDir -Filter "*.dmp" -ErrorAction SilentlyContinue
  $artifactPaths += Get-ChildItem -Path $logDir -Filter "ptrack_debug.log*" -ErrorAction SilentlyContinue
}

if (Test-Path -LiteralPath $dumpDir) {
  $artifactPaths += Get-ChildItem -Path $dumpDir -Filter "*.dmp" -ErrorAction SilentlyContinue
}

$artifactPaths = $artifactPaths | Where-Object { $_ -ne $null } | Select-Object -Unique

if ($artifactPaths.Count -eq 0) {
  Write-Warning "No guard logs or dump files were found under $basePath."
  exit 1
}

$tempWorkspace = New-Item -ItemType Directory -Path ([System.IO.Path]::GetTempPath()) -Name ("railcontrol_diag_" + [System.Guid]::NewGuid().ToString("N"))
try {
  foreach ($artifact in $artifactPaths) {
    $targetDir = Join-Path -Path $tempWorkspace.FullName -ChildPath ($artifact.Directory.Name)
    if (-not (Test-Path -LiteralPath $targetDir)) {
      New-Item -ItemType Directory -Path $targetDir -Force | Out-Null
    }
    Copy-Item -LiteralPath $artifact.FullName -Destination $targetDir -Force
  }

  $resolvedOutput = Resolve-OutputPath -UserPath $OutputPath
  if (Test-Path -LiteralPath $resolvedOutput) {
    Remove-Item -LiteralPath $resolvedOutput -Force
  }

  Compress-Archive -Path (Join-Path -Path $tempWorkspace.FullName -ChildPath "*") -DestinationPath $resolvedOutput -Force
  Write-Host "Diagnostics archived to $resolvedOutput"
}
finally {
  Remove-Item -LiteralPath $tempWorkspace.FullName -Recurse -Force -ErrorAction SilentlyContinue
}
