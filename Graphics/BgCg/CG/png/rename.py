import os

contador = 0
log = []

for nombre in os.listdir("."):
    nuevo = None
    if nombre.startswith("Cg_fe11_c"):
        nuevo = nombre.replace("Cg_fe11_c", "CgFE11c", 1)
    elif nombre.startswith("Cg_fe11_evt"):
        nuevo = nombre.replace("Cg_fe11_evt", "CgFE11evt", 1)
    elif nombre.startswith("Cg_fe11_i"):
        nuevo = nombre.replace("Cg_fe11_i", "CgFE11i", 1)
    elif nombre.startswith("Cg_fe11_misc"):
        nuevo = nombre.replace("Cg_fe11_misc", "CgFE11misc", 1)
    elif nombre.startswith("Cg_fe11_op"):
        nuevo = nombre.replace("Cg_fe11_op", "CgFE11op", 1)
    elif nombre.startswith("Cg_fe11_p"):
        nuevo = nombre.replace("Cg_fe11_p", "CgFE11p", 1)

    if nuevo and nuevo != nombre:
        os.rename(nombre, nuevo)
        log.append(f"{nombre} → {nuevo}")
        contador += 1

# Mostrar log
for linea in log:
    print("Renombrado:", linea)

print(f"\nTotal de archivos renombrados: {contador}")

# Guardar respaldo en log.txt
with open("renamed_log.txt", "w", encoding="utf-8") as f:
    f.write("\n".join(log))
    f.write(f"\n\nTotal: {contador}")
