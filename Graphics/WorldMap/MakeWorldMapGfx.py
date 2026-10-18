#!/usr/bin/env python3
"""
Convierte los 4 PNG del World Map a los formatos binarios de FE8U.
Requiere Python 3 y Pillow (pip install pillow). Uso: python MakeWorldMapGfx.py

Formatos (deducidos de la ROM FE8U vanilla):
  * Imagen principal: 480x320, 4bpp sin comprimir, tiles en orden de filas (60 por fila).
  * Mapa de paletas: 4 bits por tile (banco 0-3), nibble BAJO primero, filas de 64
    entradas (60 reales + 4 de relleno), LZ77 (tipo 0x10). 1280 bytes.
  * Paletas: 4 bancos x 16 colores BGR555.
  * Eventos: 240x160 (30x20 tiles). Imagen LZ77: tile 0 vacio + 600 tiles (en orden de
    filas, de arriba a abajo; el tile de la fila r, columna c es r*30+c+1) + 8 vacios
    (19488 bytes, como vanilla). TSA LZ77: cabecera (29, 19) + 600 entradas u16
    (banco<<12 | tile), 1202 bytes. OJO: el TSA se guarda DE ABAJO HACIA ARRIBA
    (la primera fila del archivo es la fila inferior de la imagen), como en vanilla.
  * Minimapa: 64x64, 4bpp, tiles en orden de filas, LZ77. 2048 bytes. Paleta de 16 colores.
"""
import os, struct, sys
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))

def load(name, size, ncolors):
    im = Image.open(os.path.join(HERE, name))
    if im.mode != 'P':
        sys.exit(f'{name}: debe ser PNG indexado (modo P), es {im.mode}')
    w, h = im.size
    if (w, h) != size:
        sys.exit(f'{name}: tamano {w}x{h}, se esperaba {size[0]}x{size[1]}')
    px = list(im.getdata())
    if max(px) >= ncolors:
        sys.exit(f'{name}: usa el indice {max(px)}, el maximo permitido es {ncolors-1}')
    pal = im.getpalette()
    return px, pal

def bgr555(pal, n):
    out = bytearray()
    for i in range(n):
        r, g, b = pal[i*3:i*3+3] if i*3+2 < len(pal) else (0, 0, 0)
        out += struct.pack('<H', (r >> 3) | ((g >> 3) << 5) | ((b >> 3) << 10))
    return bytes(out)

def tiles_of(px, w, h, x0=0, y0=0, tw=None, th=None):
    """Devuelve tiles de 8x8 (listas de 64 indices) en orden de filas."""
    tw = tw or w//8; th = th or h//8
    res = []
    for ty in range(th):
        for tx in range(tw):
            res.append([px[(y0+ty*8+y)*w + x0+tx*8+x] for y in range(8) for x in range(8)])
    return res

def bank_of(tile, name):
    banks = {i >> 4 for i in tile if i & 15}
    if len(banks) > 1:
        sys.exit(f'{name}: un tile mezcla colores de varios bancos de 16 ({sorted(banks)}). '
                 'Usa la herramienta de reduccion de color de FEBuilder.')
    bad = {i for i in tile if i & 15 == 0 and i != 0}
    if bad:
        sys.exit(f'{name}: un tile usa el color 0 de un banco 1-3 ({sorted(bad)}); ese slot es transparente.')
    return banks.pop() if banks else 0

def pack4(tile):
    return bytes((tile[i] & 15) | ((tile[i+1] & 15) << 4) for i in range(0, 64, 2))

# ---------------- LZ77 (tipo 0x10, BIOS). Sin desplazamiento 1: seguro para VRAM ----------------
def lz77(data):
    n = len(data)
    out = bytearray([0x10, n & 255, (n >> 8) & 255, (n >> 16) & 255])
    chains = {}
    i = 0
    def find(i):
        best_l, best_d = 0, 0
        if i + 3 > n: return 0, 0
        key = data[i:i+3]
        for j in reversed(chains.get(key, [])[-64:]):
            d = i - j
            if d > 4096: break
            if d < 2: continue
            l = 0
            while l < 18 and i + l < n and data[j + l] == data[i + l]: l += 1
            if l > best_l: best_l, best_d = l, d
            if l == 18: break
        return best_l, best_d
    def add(i):
        if i + 3 <= n: chains.setdefault(data[i:i+3], []).append(i)
    while i < n:
        flags, chunk = 0, bytearray()
        for bit in range(7, -1, -1):
            if i >= n: break
            l, d = find(i)
            if l >= 3:
                flags |= 1 << bit
                chunk += bytes([((l-3) << 4) | ((d-1) >> 8), (d-1) & 255])
                for k in range(l): add(i+k)
                i += l
            else:
                chunk.append(data[i]); add(i); i += 1
        out.append(flags); out += chunk
    while len(out) % 4: out.append(0)
    return bytes(out)

def unlz77(b):
    size = b[1] | (b[2] << 8) | (b[3] << 16); o = bytearray(); p = 4
    while len(o) < size:
        f = b[p]; p += 1
        for bit in range(7, -1, -1):
            if len(o) >= size: break
            if f & (1 << bit):
                b1, b2 = b[p], b[p+1]; p += 2
                ln = (b1 >> 4) + 3; d = ((b1 & 15) << 8 | b2) + 1
                for _ in range(ln): o.append(o[-d])
            else:
                o.append(b[p]); p += 1
    return bytes(o)

def write(name, data):
    open(os.path.join(HERE, name), 'wb').write(data)
    print(f'  {name:32s} {len(data):6d} bytes')

# =============================== MAPA PRINCIPAL ===============================
px, pal = load('1__World_Map.png', (480, 320), 64)
px2, pal2 = load('2__Dark_World_Map.png', (480, 320), 64)
if px != px2:
    sys.exit('1__World_Map y 2__Dark_World_Map deben tener los mismos pixeles (solo cambia la paleta).')
tl = tiles_of(px, 480, 320)
img = b''.join(pack4(t) for t in tl)
banks = [bank_of(t, '1__World_Map.png') for t in tl]
pm = bytearray()
for ty in range(40):
    row = banks[ty*60:(ty+1)*60] + [0, 0, 0, 0]
    pm += bytes(row[i] | (row[i+1] << 4) for i in range(0, 64, 2))   # nibble bajo primero
assert len(img) == 76800 and len(pm) == 1280
print('Mapa principal'); 
write('WorldMap_Image.dmp', img)
write('WorldMap_PaletteNormal.dmp', bgr555(pal, 64))
write('WorldMap_PaletteDark.dmp', bgr555(pal2, 64))
write('WorldMap_PaletteMap.lz.dmp', lz77(bytes(pm)))

# =============================== MAPA DE EVENTOS ===============================
epx, epal = load('3__For_Events_World_Map.png', (256, 160), 64)
if any(epx[y*256 + x] for y in range(160) for x in range(240, 256)):
    sys.exit('3__For_Events_World_Map.png: las columnas 240-255 deben estar vacias (indice 0).')
etl = tiles_of(epx, 256, 160, tw=30, th=20)          # filas de arriba a abajo
ebanks = [bank_of(t, '3__For_Events_World_Map.png') for t in etl]
slot = [pack4(t) for t in etl]                        # tile numero i+1 = posicion i
tsa = bytearray([29, 19])
for file_row in range(20):                            # el TSA va de abajo hacia arriba
    r = 19 - file_row
    for c in range(30):
        i = r*30 + c
        tsa += struct.pack('<H', (ebanks[i] << 12) | (i + 1))
eimg = bytes(32) + b''.join(slot) + bytes(8*32)
assert len(eimg) == 19488 and len(tsa) == 1202
print('Mapa de eventos')
write('Events_Image.lz.dmp', lz77(eimg))
write('Events_TSA.lz.dmp', lz77(bytes(tsa)))
write('Events_Palette.dmp', bgr555(epal, 48) + bytes(32))

# =============================== MINIMAPA ===============================
mpx, mpal = load('4__Mini_Map_World_Map.png', (64, 64), 16)
mimg = b''.join(pack4(t) for t in tiles_of(mpx, 64, 64))
assert len(mimg) == 2048
print('Minimapa')
write('MiniMap_Image.lz.dmp', lz77(mimg))
write('MiniMap_Palette.dmp', bgr555(mpal, 16))

# =============================== AUTO-COMPROBACION ===============================
def rgb(palbytes, i):
    v = struct.unpack_from('<H', palbytes, i*2)[0]
    return ((v & 31) << 3, ((v >> 5) & 31) << 3, ((v >> 10) & 31) << 3)
def rd(n): return open(os.path.join(HERE, n), 'rb').read()

# principal: reconstruye desde los binarios y compara con el PNG (nibble 0 = transparente = color 0)
for pf, pngpx, png_pal in (('WorldMap_PaletteNormal.dmp', px, pal), ('WorldMap_PaletteDark.dmp', px2, pal2)):
    pb = rd(pf); d = rd('WorldMap_Image.dmp'); m = unlz77(rd('WorldMap_PaletteMap.lz.dmp'))
    assert len(m) == 1280
    bad = 0
    for ty in range(40):
        for tx in range(60):
            e = ty*64 + tx; byte = m[e // 2]; bank = (byte & 15) if e % 2 == 0 else (byte >> 4)
            tile = d[(ty*60+tx)*32:(ty*60+tx+1)*32]
            for y in range(8):
                for x in range(8):
                    v = tile[y*4 + x//2]; nib = (v & 15) if x % 2 == 0 else (v >> 4)
                    got = rgb(pb, 0) if nib == 0 else rgb(pb, bank*16 + nib)
                    want = png_pal[pngpx[(ty*8+y)*480 + tx*8+x]*3:][:3]
                    want = tuple((c >> 3) << 3 for c in want)
                    bad += got != want
    print(f'comprobacion {pf}: {"OK" if bad == 0 else str(bad)+" pixeles distintos"}')
# eventos
pb = rd('Events_Palette.dmp'); d = unlz77(rd('Events_Image.lz.dmp')); t = unlz77(rd('Events_TSA.lz.dmp'))
assert t[:2] == bytes([29, 19]) and len(d) == 19488
bad = 0
for file_row in range(20):
    r = 19 - file_row                                 # fila real de la imagen
    for c in range(30):
        ent = struct.unpack_from('<H', t, 2 + (file_row*30+c)*2)[0]; bank, tile = ent >> 12, ent & 0x3FF
        tl_ = d[tile*32:(tile+1)*32]
        for y in range(8):
            for x in range(8):
                v = tl_[y*4 + x//2]; nib = (v & 15) if x % 2 == 0 else (v >> 4)
                got = rgb(pb, 0) if nib == 0 else rgb(pb, bank*16 + nib)
                want = tuple((cc >> 3) << 3 for cc in epal[epx[(r*8+y)*256 + c*8+x]*3:][:3])
                bad += got != want
print(f'comprobacion eventos: {"OK" if bad == 0 else str(bad)+" pixeles distintos"}')
# minimapa
pb = rd('MiniMap_Palette.dmp'); d = unlz77(rd('MiniMap_Image.lz.dmp')); bad = 0
for ty in range(8):
    for tx in range(8):
        tl_ = d[(ty*8+tx)*32:(ty*8+tx+1)*32]
        for y in range(8):
            for x in range(8):
                v = tl_[y*4 + x//2]; nib = (v & 15) if x % 2 == 0 else (v >> 4)
                want = tuple((cc >> 3) << 3 for cc in mpal[mpx[(ty*8+y)*64 + tx*8+x]*3:][:3])
                bad += rgb(pb, nib) != want
print(f'comprobacion minimapa: {"OK" if bad == 0 else str(bad)+" pixeles distintos"}')
