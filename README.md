
# FE1-Fiukes
Romhack de Fire Emblem: The Sacred Stones (FE8) que busca ser la "versión definitiva" de los juegos de Marth, empezando por el Libro 1 (FE1 + FE3 + FE11).
El enfoque es ser fiel a los originales aprovechando las ventajas del motor de FE8: los mapas se basan en los de FE11 y se recrean con tilesets de GBA, y el proyecto usa el Skill System de FE8 como base.

Estado: en desarrollo. Todavía no hay una versión jugable pública.

Autores
HauntRS
FiukeBnuy
Base técnica

Este proyecto parte del FE8 Skill System (Buildfile), creado por la comunidad de Fire Emblem Universe:

Repositorio original: https://github.com/FireEmblemUniverse/SkillSystem_FE8
Se instala y compila con Event Assembler. FEBuilderGBA no es un método de instalación soportado para esta base.
Cómo compilar

Importante: este repositorio no incluye ROMs. Necesitas tu propia copia legal del juego.

Descarga o clona este repositorio.
Consigue una ROM limpia de FE8 (versión USA), nómbrala FE8_clean.gba y colócala en la carpeta raíz del proyecto.
Ejecuta MAKE_HACK_full.cmd para generar la ROM del hack.
Ejecuta MAKE_HACK_full.cmd cada vez que cambies texto o tablas. Si no tocaste ninguno de los dos, puedes usar MAKE_HACK_quick.cmd, que es más rápido.

La ROM generada y los archivos temporales (.gba, .sav, .sym, .nlz) quedan fuera del repositorio gracias al .gitignore.

Estructura del proyecto
Carpeta	Contenido
ASM	Código ensamblador propio
EngineHacks	Hacks de motor, incluido el Skill System
Events	Eventos de capítulo
Maps	Mapas
Graphics	Gráficos (tilesets, retratos, etc.)
Music	Música
Tables	Tablas de datos (personajes, clases, armas, etc.)
Text	Diálogos y textos
Tools	Herramientas de compilación
Créditos

Este proyecto se apoya en el trabajo de mucha gente de la comunidad. Consulta CREDITS.md para la lista completa. Agradecimientos especiales a:

Circleseverywhere, por crear y publicar el Skill System original.
Los colaboradores del Skill System y de las herramientas de la comunidad de Fire Emblem Universe.
Los autores de los tilesets de la comunidad usados en los mapas.

Fire Emblem es propiedad de Nintendo e Intelligent Systems. Este es un proyecto de fans sin fines de lucro, sin relación con ellos.

Licencia

Consulta el archivo LICENSE.