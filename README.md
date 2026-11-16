# Dark Dragon Rebirth

![Mapa del Capítulo 1](Docs/ch01_preview.png)
<!-- Sube una captura del mapa del Capítulo 1 a Docs/ y ajusta el nombre del archivo. -->

Romhack de **Fire Emblem: The Sacred Stones (FE8)** que busca ser la "versión definitiva" de la historia de Marth, empezando por el **Libro 1**. Combina FE1, FE3 y FE11 aprovechando el potencial del motor de FE8.

> **Estado:** en desarrollo. Todavía no hay una versión jugable pública.
> **Progreso actual:** Prólogo hasta el Capítulo [3]. <!-- actualiza este dato -->
>
> Hilo del proyecto en Fire Emblem Universe: https://feuniverse.us/t/fe8-dark-dragon-rebirth/23605

*Nombres clave de desarrollo: **FE1-Fiukes** y **Marths1F** (este repositorio).*

## ¿Qué hace diferente a este hack?

- **Tres juegos en uno:** reúne FE1 (*Ankoku Ryū to Hikari no Tsurugi*), FE3 (*Monshō no Nazo*) y FE11 (*Shin Ankoku Ryū to Hikari no Tsurugi*), tomando funciones de cada uno para crear una nueva experiencia de juego.
- **Estilo moderno:** apariencia y jugabilidad cercanas a FE11, con mapas recreados con tilesets de GBA que respetan la geografía original de cada capítulo.
- **Sistema de montar y desmontar.**
- **Conversaciones de apoyo.**
- **Skill System:** usa el Skill System de FE8 como base para incorporar habilidades. [Describe aquí qué habilidades o mecánicas añades.]
- **Decisiones de diseño:** [Indica aquí qué versión de cada personaje, clase y arma tomas como canónica, y cómo unificas las armas legendarias.]

## Capturas

<!-- Agrega 2 o 3 imágenes: mapa del Capítulo 1, pantalla de batalla, menú de unidad. -->

## Equipo (Burrow Team)

- **Fiuke_Bnuy:** desarrollador, artista y líder de desarrollo.
- **HauntRS:** desarrollador, soporte técnico y community manager.

¿Quieres colaborar? Escribe a cualquiera de los dos por Fire Emblem Universe.

## Cómo compilar

> **Importante:** este repositorio **no incluye ROMs**. Necesitas tu propia copia legal del juego.

1. Descarga o clona este repositorio.
2. Consigue una ROM limpia de **FE8 (versión USA)**, nómbrala `FE8_clean.gba` y colócala en la carpeta raíz del proyecto.
3. Ejecuta `MAKE_HACK_full.cmd` para generar la ROM del hack.
4. Vuelve a ejecutar `MAKE_HACK_full.cmd` cada vez que cambies texto o tablas. Si no tocaste ninguno de los dos, puedes usar `MAKE_HACK_quick.cmd`, que es más rápido.

La ROM generada y los archivos temporales (`.gba`, `.sav`, `.sym`, `.nlz`) quedan fuera del repositorio gracias al `.gitignore`.

## Estructura del proyecto

| Carpeta | Contenido |
| --- | --- |
| `ASM` | Código ensamblador propio |
| `Docs` | Documentación del proyecto |
| `EngineHacks` | Hacks de motor, incluido el Skill System |
| `EventAssembler` | Herramienta de compilación |
| `Events` | Eventos de capítulo |
| `Graphics` | Gráficos (tilesets, retratos, mapa del mundo, etc.) |
| `Maps` | Mapas |
| `Music` | Música |
| `Tables` | Tablas de datos (personajes, clases, armas, etc.) |
| `Text` | Diálogos y textos |
| `Tools` | Herramientas auxiliares |

## Base técnica

Este proyecto parte del **FE8 Skill System** (Buildfile), creado por la comunidad de Fire Emblem Universe:
https://github.com/FireEmblemUniverse/SkillSystem_FE8

Se instala y compila con Event Assembler. FEBuilderGBA no es un método de instalación soportado para esta base.

## Inspiración

El proyecto está muy influenciado por [*Fire Emblem: Dark Dragon Reborn*](https://feuniverse.us/t/fire-emblem-dark-dragon-reborn-complete/11733/1) de **Merpin**, un remake del juego original de NES con inspiración jugable del remake de DS.

## Créditos

Este proyecto se apoya en el trabajo de mucha gente de la comunidad. Consulta [`CREDITS.md`](CREDITS.md) para la lista completa. Agradecimientos especiales a:

- **Circleseverywhere**, por crear y publicar el Skill System original.
- **Merpin**, por *Dark Dragon Reborn*, que sirve de inspiración.
- Los colaboradores del Skill System y de las herramientas de la comunidad de Fire Emblem Universe.
- Los autores de los tilesets de la comunidad usados en los mapas.

## Aviso legal

Fire Emblem es propiedad de Nintendo e Intelligent Systems. Este es un proyecto de fans sin fines de lucro, sin relación con ellos.

## Licencia

Consulta el archivo [`LICENSE`](LICENSE). Algunos recursos de terceros (tilesets, música, gráficos) pueden tener condiciones propias; revisa `CREDITS.md`.
