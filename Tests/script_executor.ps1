# Get all .ll files in current directory
$files = Get-ChildItem -Filter "*.ll" | Select-Object -ExpandProperty Name
$iterations = 5

$outputFile = "benchmark_summary.txt"
$totalWithPass = 0
$totalWithoutPass = 0
$totalWarnings = 0
$errorFiles = 0
$fileCount = 0

"" | Out-File $outputFile

foreach ($file in $files) {
    Write-Host "File: $file"
    $fileCount++

    $output = & opt $file -passes="function(suspicious-ptr)" 2>&1
    $exitCode = $LASTEXITCODE

    if ($exitCode -ne 0) {
        $errorFiles++
    }

    $warningCount = ($output | Select-String "\[SuspiciousPtr\]").Count
    $totalWarnings += $warningCount

    # ---- WITH pass timing ----
    $withPassTicks = 0
    for ($i = 0; $i -lt $iterations; $i++) {
        $withPassTicks += (Measure-Command {
            opt $file -passes="function(suspicious-ptr)" > $null
        }).Ticks
    }
    $withPassAvg = $withPassTicks / $iterations
    $totalWithPass += $withPassAvg

    # ---- WITHOUT pass timing ----
    $withoutPassTicks = 0
    for ($i = 0; $i -lt $iterations; $i++) {
        $withoutPassTicks += (Measure-Command {
            opt $file > $null
        }).Ticks
    }
    $withoutPassAvg = $withoutPassTicks / $iterations
    $totalWithoutPass += $withoutPassAvg

    Add-Content $outputFile "File: $file"
    Add-Content $outputFile "  Avg ticks WITH pass:    $withPassAvg"
    Add-Content $outputFile "  Avg ticks WITHOUT pass: $withoutPassAvg"
    Add-Content $outputFile "  Warnings found:         $warningCount"
    Add-Content $outputFile ""
}

$avgWithPass = $totalWithPass / $fileCount
$avgWithoutPass = $totalWithoutPass / $fileCount
$percentDiff = (($avgWithPass - $avgWithoutPass) / $avgWithoutPass) * 100

Add-Content $outputFile "--------------------------------------------------"
Add-Content $outputFile "Files tested:               $fileCount"
Add-Content $outputFile "Overall avg WITH pass:      $avgWithPass"
Add-Content $outputFile "Overall avg WITHOUT pass:   $avgWithoutPass"
Add-Content $outputFile ("Performance difference:     {0:N2}%" -f $percentDiff)
Add-Content $outputFile "Total warnings found:       $totalWarnings"
Add-Content $outputFile "Errors:                     $totalWarnings / $fileCount"