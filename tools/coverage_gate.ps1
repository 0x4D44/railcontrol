Param(
  [Parameter(Mandatory=$true)][string]$CoberturaXml,
  [Parameter(Mandatory=$true)][double]$MinLineRate,
  [switch]$Hard
)

if (!(Test-Path $CoberturaXml)) {
  Write-Host "coverage_gate: file not found: $CoberturaXml"
  exit 0 # soft gate by default
}

try {
  [xml]$doc = Get-Content -Raw $CoberturaXml
} catch {
  Write-Host "coverage_gate: failed to parse XML: $CoberturaXml"
  exit 0
}

$lineRate = $null
if ($doc.coverage -and $doc.coverage.'line-rate') {
  $lineRate = [double]::Parse($doc.coverage.'line-rate')
} else {
  # Fallback: compute from all <line> nodes (hits > 0)
  $lines = Select-Xml -Xml $doc -XPath "//line"
  if ($lines) {
    $total = [double]($lines.Count)
    $covered = [double]((@($lines | Where-Object { $_.Node.hits -as [int] -gt 0 }).Count))
    if ($total -gt 0) { $lineRate = $covered / $total }
  }
}

if ($null -eq $lineRate) {
  Write-Host "coverage_gate: could not determine line rate"
  exit 0
}

Write-Host ("coverage_gate: line-rate={0:P1} threshold={1:P1}" -f $lineRate, $MinLineRate)

if ($lineRate -lt $MinLineRate) {
  if ($Hard) {
    Write-Error "coverage_gate: threshold not met (line-rate=$lineRate, min=$MinLineRate)"
    exit 2
  } else {
    Write-Host "coverage_gate: WARNING threshold not met"
    exit 0
  }
}

exit 0
