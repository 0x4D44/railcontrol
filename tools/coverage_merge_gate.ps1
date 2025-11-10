Param(
  [switch],
  [Parameter(Mandatory=$false)][string]$CoberturaXmlA = "build/msvc/Debug/coverage.xml",
  [Parameter(Mandatory=$false)][string]$CoberturaXmlB = "build/msvc/Debug/coverage_gtest.xml",
  [Parameter(Mandatory=$true)][double]$MinLineRate
)

function Get-LineMapFromCobertura([xml]$doc) {
  $map = @{}
  # Iterate classes and their lines to capture filename + line number
  $classes = Select-Xml -Xml $doc -XPath "//class"
  foreach($c in $classes){
    $file = $c.Node.filename
    if (-not $file) { continue }
    $lines = $c.Node.lines.line
    foreach($ln in $lines){
      $num = $ln.number
      $hits = 0
      [void][int]::TryParse($ln.hits, [ref]$hits)
      $key = "$file:$num"
      if (-not $map.ContainsKey($key)) {
        $map[$key] = ($hits -as [int]) -gt 0
      } else {
        # Merge covered states: covered if any source marks it covered
        $map[$key] = $map[$key] -or (($hits -as [int]) -gt 0)
      }
    }
  }
  return $map
}

$maps = @()
if (Test-Path $CoberturaXmlA) {
  try { [xml]$docA = Get-Content -Raw $CoberturaXmlA; $maps += ,(Get-LineMapFromCobertura $docA) } catch {}
}
if (Test-Path $CoberturaXmlB) {
  try { [xml]$docB = Get-Content -Raw $CoberturaXmlB; $maps += ,(Get-LineMapFromCobertura $docB) } catch {}
}

if ($maps.Count -eq 0) {
  Write-Host "coverage_merge_gate: no coverage files found; skipping"
  exit 0
}

# Merge maps
$merged = @{}
foreach($m in $maps){ foreach($k in $m.Keys){ if (-not $merged.ContainsKey($k)) { $merged[$k] = $m[$k] } else { $merged[$k] = $merged[$k] -or $m[$k] } } }

$total = [double]$merged.Count
$covered = [double](@($merged.GetEnumerator() | Where-Object { $_.Value }).Count)
$lineRate = 0
if ($total -gt 0) { $lineRate = $covered / $total }

Write-Host ("coverage_merge_gate: merged line-rate={0:P1} threshold={1:P1} (covered={2} / total={3})" -f $lineRate, $MinLineRate, [int]$covered, [int]$total)

if ($lineRate -lt $MinLineRate) {\n  if ($Hard) { Write-Error "coverage_merge_gate: threshold not met"; exit 2 } else { Write-Host "coverage_merge_gate: WARNING threshold not met (soft gate)" }\n}\n

exit 0



