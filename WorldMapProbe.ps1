# WorldMapProbe.ps1
# Lee (no modifica) tu FE8_clean.gba y reporta lo necesario para instalar el World Map.
# Uso:
#   powershell -ExecutionPolicy Bypass -File WorldMapProbe.ps1 "C:\ruta\a\FE8_clean.gba"
# Genera WorldMapProbe.txt junto al script. Pegame su contenido.

param([Parameter(Mandatory=$true)][string]$Rom)

$b = [System.IO.File]::ReadAllBytes($Rom)
$out = New-Object System.Collections.Generic.List[string]
function Say([string]$s) { $out.Add($s); Write-Host $s }

Say ("ROM: {0} bytes" -f $b.Length)

# Bloques conocidos de FE8U (direcciones de ROM, sin 08)
$blocks = [ordered]@{
    'Main image (4bpp)'      = 0xA83364
    'Main palette DARK'      = 0xA95F64
    'Main palette NORMAL'    = 0xA95FE4
    'Main palette MAP (lz)'  = 0xA96064
    'Events image (lz)'      = 0xA99140
    'Events TSA (lz)'        = 0xA9DF74
    'Events palette'         = 0xA9E4C4
    'Mini map image (lz)'    = 0xAA1280
    'Mini map palette'       = 0xAA188C
}

# 1) Donde esta escrito cada puntero (una sola pasada, solo posiciones alineadas a 4)
$targets = @{}
foreach ($k in $blocks.Keys) { $targets[[uint32](0x08000000 + $blocks[$k])] = $k }
$hits = @{}
foreach ($k in $blocks.Keys) { $hits[$k] = New-Object System.Collections.Generic.List[string] }
for ($i = 0; $i -le $b.Length - 4; $i += 4) {
    if ($b[$i + 3] -ne 8) { continue }
    $v = [BitConverter]::ToUInt32($b, $i)
    if ($targets.ContainsKey($v)) { $hits[$targets[$v]].Add(('0x{0:X}' -f $i)) }
}
Say ""
Say "== PUNTEROS (offset donde aparece el puntero) =="
foreach ($k in $blocks.Keys) {
    $list = $hits[$k] -join ', '
    if ($list -eq '') { $list = '(ninguno)' }
    Say ("{0,-24} 0x{1:X}  <-  {2}" -f $k, $blocks[$k], $list)
}

# 2) Datos LZ77: tamano descomprimido, tamano comprimido y volcado
# (todo con [int] explicito: en PowerShell los desplazamientos sobre [byte] pierden bits)
function Lz77([int]$off) {
    if ($b[$off] -ne 0x10) { return $null }
    $size = [int]$b[$off + 1] + 256 * [int]$b[$off + 2] + 65536 * [int]$b[$off + 3]
    $o = New-Object byte[] $size
    $p = $off + 4
    $n = 0
    while ($n -lt $size) {
        $flags = [int]$b[$p]; $p++
        for ($bit = 7; $bit -ge 0 -and $n -lt $size; $bit--) {
            if (($flags -band (1 -shl $bit)) -ne 0) {
                $b1 = [int]$b[$p]; $b2 = [int]$b[$p + 1]; $p += 2
                $len = ($b1 -shr 4) + 3
                $disp = (($b1 -band 15) * 256) + $b2 + 1
                for ($k = 0; $k -lt $len -and $n -lt $size; $k++) { $o[$n] = $o[$n - $disp]; $n++ }
            } else {
                $o[$n] = $b[$p]; $p++; $n++
            }
        }
    }
    return @{ Data = $o; Packed = ($p - $off); Size = $size }
}

function HexLines($data, [int]$count, [int]$perLine) {
    $lines = New-Object System.Collections.Generic.List[string]
    for ($i = 0; $i -lt $count; $i += $perLine) {
        $e = [Math]::Min($i + $perLine, $count) - 1
        $lines.Add(($data[$i..$e] | ForEach-Object { '{0:X2}' -f $_ }) -join ' ')
    }
    return $lines
}

Say ""
Say "== BLOQUES COMPRIMIDOS =="
foreach ($k in $blocks.Keys) {
    if ($k -notlike '*(lz)*') { continue }
    $raw = ($b[$blocks[$k]..($blocks[$k] + 7)] | ForEach-Object { '{0:X2}' -f $_ }) -join ' '
    $r = Lz77 $blocks[$k]
    if ($r -eq $null) { Say ("{0}: NO empieza con 0x10. Primeros bytes crudos: {1}" -f $k, $raw); continue }
    Say ("{0}: descomprimido={1} bytes, comprimido={2} bytes  (crudo: {3})" -f $k, $r.Size, $r.Packed, $raw)
}

# Volcados completos (son pequenos)
foreach ($k in @('Main palette MAP (lz)', 'Events TSA (lz)')) {
    $r = Lz77 $blocks[$k]
    if ($r -ne $null) {
        Say ""
        Say ("== VOLCADO {0} ({1} bytes, 32 por linea) ==" -f $k, $r.Size)
        foreach ($l in (HexLines $r.Data $r.Size 32)) { Say $l }
    }
}
foreach ($k in @('Mini map image (lz)', 'Events image (lz)')) {
    $r = Lz77 $blocks[$k]
    if ($r -ne $null) {
        Say ""
        Say ("== INICIO {0} (primeros 64 de {1} bytes) ==" -f $k, $r.Size)
        foreach ($l in (HexLines $r.Data ([Math]::Min(64, $r.Size)) 32)) { Say $l }
    }
}

$out | Set-Content -Path (Join-Path $PSScriptRoot 'WorldMapProbe.txt') -Encoding ASCII
Say ""
Say "Listo. Tambien guardado en WorldMapProbe.txt"
