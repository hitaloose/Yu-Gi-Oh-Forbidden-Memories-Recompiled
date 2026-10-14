# Yu-Gi-Oh! Forbidden Memories Recompiled for Nintendo Switch

**Yu-Gi-Oh! Forbidden Memories** (PS1, USA) as a native homebrew for a Nintendo Switch with
custom firmware (Atmosphère). Bring your own disc image; no game data is included but the
text of the European translations.

This is an unofficial fork of [Yu-Gi-Oh! Forbidden Memories Recompiled](https://github.com/Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled), which rebuilt
the game from C that matches the original byte for byte and ported it to Windows, Linux,
macOS and Android. This fork adds the Switch port only, and follows the upstream: each of its
stable releases is released here for the Switch. Problems with the Switch build belong here,
not upstream.

![Your field at 4x resolution with 3D Monsters](docs/screenshots/field.jpg)

## Features

- Up to 4x internal resolution, widescreen and HD text
- **3D Monsters**, **AI Hard Mode**, **Hand Camera** and **Yamyi Mods** built in
- Data mods, the **Forbidden Memories HD** pack included (redrawn cards, frames and portraits)
- Save slots, save states, fusion helper, card drop rates, fast forward
- **Game > Language**: the European releases' own English, French, German, Italian and Spanish

![HD Free Duel portraits](docs/screenshots/portraits.jpg)

## Install

1. Download `yfm-redecomp-<version>-switch.zip` from [Releases](https://github.com/hitaloose/Yu-Gi-Oh-Forbidden-Memories-Recompiled/releases/latest).
2. Unpack it onto the SD card's root (or copy `yfm-redecomp-<version>-switch.nro` alone into
   `sdmc:/switch/`: it makes `sdmc:/switch/yfm-redecomp/` with `game/`, `mods/` and `user/` on
   its first start).
3. Put your USA disc's `.bin` in `sdmc:/switch/yfm-redecomp/game/`.
4. Start the homebrew **in place of a game**: hold **R** while starting any game. From the
   album it gets too little memory.

To update, unpack the new release over the old one: saves are kept in `user/`.

### Controls

- **R3** (press the right stick) shows or hides the menu bar, which starts hidden. While it
  shows, **L** / **R** switch menus, the D-pad moves and the controller drives the menus,
  not the game. Taps on the bar work too.
- **L3** (press the left stick) turns fast forward on, and off on the next press.

### Mods

3D Monsters, AI Hard Mode, Hand Camera and Yamyi Mods are built into the `.nro`. Data mods
go in `sdmc:/switch/yfm-redecomp/mods/` and work as on PC. For the HD pack, download
`yfm-redecomp-hd-mod-<version>.zip` from the [upstream releases](https://github.com/Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled/releases), unpack its
`mods/assets-hd` there and pick **Internal 2x** or more. Other mods with code do not run on
the Switch.

### Bring your save from an emulator

Copy the memory card file (`.mcr`, `.mcd`, `.srm`, `.gme`, `.mcs`, `.psv`, ...) into
`sdmc:/switch/yfm-redecomp/user/saves/` and pick **LOAD** on the title screen: the save is
copied into an empty slot. Emulator save states are not memory cards; save in game first.

## Versions

A release is named `vX.Y.Z-switch.N`: the upstream release `vX.Y.Z` it is built on, and `N`,
the Switch build of it (`.1` first, then fixes of the Switch port only). A workflow watches the
upstream and drafts `vX.Y.Z-switch.1` when it releases; it is published once tried on a Switch.
See [Nintendo Switch](notes/switch.md), "Fork releases".

## Build

```sh
MEMORIES_VERSION=v0.3.0-switch.1 python3 tools/pc/build.py --target switch
python3 tools/pc/package_switch.py --version v0.3.0-switch.1
```

needs macOS on Apple Silicon and devkitPro (devkitA64, libnx, the Switch portlibs). The
toolchain, the memory layout and the packaging: [Nintendo Switch](notes/switch.md).

The PC, macOS and Android builds, the matching decompilation and their docs are the
upstream's and stay in this tree unchanged; see the [upstream README](https://github.com/Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled#readme) and
[notes/](notes/).

<details>
<summary>Decompilation progress</summary>

<!-- BEGIN GENERATED PROGRESS -->

| Metric | Current |
|---|---:|
| Game C-decompilation targets matched | **1,134 / 1,134 (100.00%)** |
| Game C-decompilation target bytes matched | **356,080 (`0x56EF0`) / 356,080 (`0x56EF0`) (100.00%)** |
| Remaining game C-decompilation targets | 0 functions, 0 (`0x0`) |
| Evidence-backed handwritten game assembly | 61 functions, 40,116 (`0x9CB4`) |
| Total game-owned functions | 1,195 |
| Preserved Psy-Q CRT/SDK assembly | 591 functions, 117,348 (`0x1CA64`) |
| Total discovered functions | 1,786 |
| Embedded/unassigned resident text | 1,780 (`0x6F4`) |

Runtime overlay modules:

| Module | Matching C functions | Matching C bytes |
|---|---:|---:|
| `free_duel` | 9 / 9 (100.00%) | 4,140 (`0x102C`) / 4,140 (`0x102C`) (100.00%) |
| `main_menu` | 31 / 31 (100.00%) | 17,724 (`0x453C`) / 17,724 (`0x453C`) (100.00%) |
| `overworld_after_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `overworld_before_coup` | 15 / 15 (100.00%) | 6,184 (`0x1828`) / 6,184 (`0x1828`) (100.00%) |
| `password` | 27 / 27 (100.00%) | 10,884 (`0x2A84`) / 10,884 (`0x2A84`) (100.00%) |

_Generated from `config/slus_01411/functions.csv` and `config/slus_01411/overlays/*_functions.csv` by `tools/project/progress.py`._

<!-- END GENERATED PROGRESS -->

</details>

## Community

The upstream's [Discord](https://discord.gg/Gn6Mag52q) is about the game and the PC port.
Report Switch problems in this fork's [issues](https://github.com/hitaloose/Yu-Gi-Oh-Forbidden-Memories-Recompiled/issues).

## License

The port (`src/pc/`, `tools/pc/`, `tests/pc/`, `mods/`, `examples/` and the build and play
scripts) is [MIT](LICENSE): keep the copyright notice and please link back to the
[upstream](https://github.com/Unchiga/Yu-Gi-Oh-Forbidden-Memories-Recompiled). The decompilation builds on [memories-decomp](https://github.com/krystalgamer/memories-decomp)
by its authors. Yu-Gi-Oh! and Forbidden Memories belong to Konami.
