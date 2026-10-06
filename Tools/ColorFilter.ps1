# Repinta la salida de ColorzCore:
#   "error"  en rojo, "warning" en amarillo, y la linea
#   "Errors occurred; no changes written." completa en rojo.
# Todo lo demas se imprime sin color.
while ($null -ne ($line = [Console]::In.ReadLine())) {
    if ($line -match '^(error)(:.*)$') {
        Write-Host $matches[1] -ForegroundColor Red -NoNewline
        Write-Host $matches[2]
    }
    elseif ($line -match '^(warning)(:.*)$') {
        Write-Host $matches[1] -ForegroundColor Yellow -NoNewline
        Write-Host $matches[2]
    }
    elseif ($line -match '^Errors occurred') {
        Write-Host $line -ForegroundColor Red
    }
    else {
        Write-Host $line
    }
}
