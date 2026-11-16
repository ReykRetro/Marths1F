# Dark Dragon Rebirth

![Chapter 1 map](Docs/ch01_preview.png)
<!-- Upload a screenshot of the Chapter 1 map to Docs/ and adjust the file name. -->

A romhack of **Fire Emblem: The Sacred Stones (FE8)** that aims to be the "definitive version" of Marth's story, starting with **Book 1**. It combines FE1, FE3 and FE11, taking advantage of the FE8 engine's potential.

> **Status:** in development. There is no public playable version yet.
> **Current progress:** Prologue through Chapter [3]. <!-- update this -->
>
> Project thread on Fire Emblem Universe: https://feuniverse.us/t/fe8-dark-dragon-rebirth/23605

*Development codenames: **FE1-Fiukes** and **Marths1F** (this repository).*

## What makes this hack different?

- **Three games in one:** brings together FE1 (*Ankoku Ryū to Hikari no Tsurugi*), FE3 (*Monshō no Nazo*) and FE11 (*Shin Ankoku Ryū to Hikari no Tsurugi*), taking features from each to create a new gameplay experience.
- **Modern style:** visuals and gameplay close to FE11, with maps recreated using GBA tilesets that respect the original geography of each chapter.
- **Mount and dismount system.**
- **Support conversations.**
- **Skill System:** uses the FE8 Skill System as a base to add skills. [Describe here which skills or mechanics you add.]
- **Design decisions:** [State here which version of each character, class and weapon you treat as canon, and how you unify the legendary weapons.]

## Screenshots

<!-- Add 2 or 3 images: Chapter 1 map, battle screen, unit menu. -->

## Team (Burrow Team)

- **Fiuke_Bnuy:** developer, artist and lead developer.
- **HauntRS:** developer, technical support and community manager.

Want to collaborate? Message either of us on Fire Emblem Universe.

## How to build

> **Important:** this repository **does not include ROMs**. You need your own legal copy of the game.

1. Download or clone this repository.
2. Get a clean **FE8 (US version)** ROM, name it `FE8_clean.gba` and place it in the project's root folder.
3. Run `MAKE_HACK_full.cmd` to generate the hack's ROM.
4. Run `MAKE_HACK_full.cmd` again whenever you change text or tables. If you changed neither, you can use `MAKE_HACK_quick.cmd`, which is faster.

The generated ROM and temporary files (`.gba`, `.sav`, `.sym`, `.nlz`) are kept out of the repository thanks to `.gitignore`.

## Project structure

| Folder | Contents |
| --- | --- |
| `ASM` | Custom or third-party assembly code |
| `Docs` | Project documentation |
| `EngineHacks` | Engine hacks, including the Skill System |
| `EventAssembler` | Build tool |
| `Events` | Chapter events |
| `Graphics` | Graphics (tilesets, portraits, world map, etc.) |
| `Maps` | Maps |
| `Music` | Music |
| `Tables` | Data tables (characters, classes, weapons, etc.) |
| `Text` | Dialogue and text |
| `Tools` | Helper tools |

## Technical base

This project is built on the **FE8 Skill System** (Buildfile), created by the Fire Emblem Universe community:
https://github.com/FireEmblemUniverse/SkillSystem_FE8

It is installed and built with Event Assembler. FEBuilderGBA is not a supported installation method for this base.

## Inspiration

The project is heavily influenced by [*Fire Emblem: Dark Dragon Reborn*](https://feuniverse.us/t/fire-emblem-dark-dragon-reborn-complete/11733/1) by **Merpin**, a remake of the original NES game with gameplay inspiration from the DS remake.

## Credits

This project relies on the work of many people in the community. See [`CREDITS.md`](CREDITS.md) for the full list. Special thanks to:

- **Circleseverywhere**, for creating and releasing the original Skill System.
- **Merpin**, for *Dark Dragon Reborn*, which serves as an inspiration.
- The contributors to the Skill System and to the tools of the Fire Emblem Universe community.
- The authors of the community tilesets used in the maps.

## Legal notice

Fire Emblem is owned by Nintendo and Intelligent Systems. This is a non-profit fan project, unaffiliated with them.

## License

See the [`LICENSE`](LICENSE) file.
Some third-party assets (tilesets, music, graphics) may have their own terms; check `CREDITS.md`.
