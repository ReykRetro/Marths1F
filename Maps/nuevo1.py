# -*- coding: utf-8 -*-
"""
Revisa todos los .tmx de la carpeta actual.
Si tienen solo la estructura básica con <property name="Main">,
les agrega el bloque de propiedades extra.
Antes de sobrescribir, guarda una copia .bak.
"""

import os
import re
import shutil

# Bloque de propiedades que quieres insertar
extra_props = """   <property name="Anims1" value="0x12"/>
   <property name="Anims2" value="0x0"/>
   <property name="ChapterID" value="0x0"/>
   <property name="Main" value=""/>
   <property name="MapChangesID" value="0x0"/>
   <property name="MapID" value="0x0"/>
   <property name="ObjectType" value="0xe"/>
   <property name="PaletteID" value="0xf"/>
   <property name="TileConfig" value="0x10"/>"""

def procesar_tmx(ruta):
    with open(ruta, "r", encoding="utf-8") as f:
        contenido = f.read()

    # Detectar si solo tiene la propiedad Main y no las demás
    if re.search(r'<property name="Main"', contenido) and not re.search(r'Anims1', contenido):
        # Guardar copia de respaldo
        respaldo = ruta + ".bak"
        shutil.copyfile(ruta, respaldo)

        # Reemplazar el bloque <properties> con el nuevo
        nuevo = re.sub(
            r'<properties>\s*<property name="Main" value\s*=\s*".*?"/>\s*</properties>',
            f"<properties>\n{extra_props}\n  </properties>",
            contenido,
            flags=re.DOTALL
        )
        with open(ruta, "w", encoding="utf-8") as f:
            f.write(nuevo)
        print(f"Archivo modificado: {ruta} (respaldo en {respaldo})")
    else:
        print(f"Archivo sin cambios: {ruta}")

# Procesar todos los .tmx en la carpeta actual
for archivo in os.listdir("."):
    if archivo.endswith(".tmx"):
        procesar_tmx(archivo)
