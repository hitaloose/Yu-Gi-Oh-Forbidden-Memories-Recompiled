#!/usr/bin/env python3
"""Pack the Nintendo Switch homebrew for sharing, into dist/.

    python3 tools/pc/build.py --target switch      # tmp/pc/switch/memories.nro
    python3 tools/pc/package_switch.py --version v0.3.0

Two files, both from the same .nro:

  yfm-redecomp-<version>-switch.nro  the program alone: copied into
      sdmc:/switch/ it makes sdmc:/switch/yfm-redecomp/ (game/, mods/,
      user/) on its first start (src/pc/platform/switch.c).
  yfm-redecomp-<version>-switch.zip  unpacked onto the SD card's root:
      switch/yfm-redecomp.nro, and switch/yfm-redecomp/ with the official
      European languages' text, the mods the release ships, an empty
      game/ for the player's disc image, README.txt and LICENSE.

The Switch build loads no code: the repository's code mods are linked into
the .nro (MEMORIES_BUILTIN_CODE_MODS, src/pc/mods/builtin_mods.h), and
their folders here hold only mod.json and data. A code mod goes in only
when the .nro beside its builtin_mods.c links it (a --no-builtin-mods build
ships the data mods alone). Nothing from the game's discs is included."""

import argparse
import hashlib
import json
import re
import subprocess
import sys
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
NAME = "yfm-redecomp"
FOLDER = f"switch/{NAME}"
GAME_README = """Put your raw image of Yu-Gi-Oh! Forbidden Memories (USA, SLUS-01411) here:
the .bin file of a .bin/.cue pair. Any file name ending in .bin will do.
Mods go in ../mods, one folder per mod.
"""
README = """Yu-Gi-Oh! Forbidden Memories Re-Decomp for the Nintendo Switch ({version})
==========================================================================

A homebrew for a Switch running custom firmware (Atmosphere), started from
the Homebrew Menu. It does not include the game: you need your own copy.

Installing
----------
Unpack this archive onto the root of the SD card, so that you have:

    sdmc:/switch/yfm-redecomp.nro
    sdmc:/switch/yfm-redecomp/game/
    sdmc:/switch/yfm-redecomp/mods/

(The .nro alone in sdmc:/switch/ is enough too: it makes the folders on
its first start.) Put your raw image of Forbidden Memories (USA,
SLUS-01411), the .bin of a .bin/.cue pair, in sdmc:/switch/yfm-redecomp/game/.

Start it in place of a game (hold R while you start any game, then choose
it in the Homebrew Menu): started from the album, homebrew gets too little
memory.

Controls
--------
R3 (press the right stick) shows the menu bar; L and R switch menus, the
D-pad moves, and the game's confirm and
back buttons choose and go back. L3 turns fast forward on and off.
The touch screen works on the bar as well.

Files
-----
Saves and settings: sdmc:/switch/yfm-redecomp/user/
Log of the last start: sdmc:/switch/yfm-redecomp/log.txt

Mods
----
Turn mods on and off in the Mods menu. The ones in
sdmc:/switch/yfm-redecomp/mods/ come with this release; their code
(3D Monsters, AI Hard Mode, Hand Camera, Yamyi Mods) is part of the .nro.
Data mods (texture packs, card and drop changes) work: put each one's
folder in the same mods/ folder. Other mods that contain code do not run
on the Switch.
"""


def nro_check(path):
    """The file is a homebrew NRO (its header's magic)."""
    data = path.read_bytes()
    if len(data) < 0x80 or data[0x10:0x14] != b"NRO0":
        raise SystemExit(f"{path} is not a Switch .nro; build it with tools/pc/build.py --target switch")
    return data


def linked_libraries(nro):
    """The code mods the .nro links: the libraries of the builtin_mods.c
    the build wrote beside it (tools/pc/build_arm64.py)."""
    table = nro.parent / "builtin_mods.c"
    if not table.is_file():
        return set()
    return set(re.findall(r'^    \{"([^"]+)", MemoriesModInit__', table.read_text(), re.M))


def shipped_mods(linked=frozenset()):
    """The repository's own mods: their tracked files, never a locally
    installed mod, and no source. A mod that carries a library goes in only
    when the .nro links it."""
    tracked = subprocess.check_output(["git", "ls-files", "-z", "mods"], cwd=ROOT, text=True).split("\0")
    files = [Path(name) for name in tracked if name]
    code = set()
    for name in files:
        if name.name == "mod.json":
            manifest = json.loads((ROOT / name).read_text(encoding="utf-8-sig"))
            library = manifest.get("library")
            if library and library.removesuffix(".o") not in linked:
                code.add(name.parent)
    return [name for name in files
            if name.parent not in code and not any(parent in code for parent in name.parents)
            and name.suffix not in (".c", ".h", ".o", ".dylib")]


def package(nro, out, version):
    data = nro_check(nro)
    out.mkdir(parents=True, exist_ok=True)
    plain = out / f"{NAME}-{version}-switch.nro"
    plain.write_bytes(data)
    archive = out / f"{NAME}-{version}-switch.zip"
    with zipfile.ZipFile(archive, "w", zipfile.ZIP_DEFLATED, compresslevel=9) as zipped:
        zipped.writestr(f"switch/{NAME}.nro", data)
        zipped.writestr(f"{FOLDER}/README.txt", README.format(version=version))
        zipped.write(ROOT / "LICENSE", f"{FOLDER}/LICENSE")
        zipped.writestr(f"{FOLDER}/game/README.txt", GAME_README)
        zipped.writestr(f"{FOLDER}/mods/", "")
        for language in sorted((ROOT / "languages").glob("*.txt")):
            zipped.write(language, f"{FOLDER}/languages/{language.name}")
        for name in shipped_mods(linked_libraries(nro)):
            zipped.write(ROOT / name, f"{FOLDER}/{name.as_posix()}")
    for path in (plain, archive):
        digest = hashlib.sha256(path.read_bytes()).hexdigest()
        path.with_name(path.name + ".sha256").write_text(f"{digest}  {path.name}\n")
    return plain, archive


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--version", required=True, help="the release label, as in v0.3.0")
    parser.add_argument("--nro", type=Path, default=ROOT / "tmp/pc/switch/memories.nro")
    parser.add_argument("--out", type=Path, default=ROOT / "dist")
    args = parser.parse_args()
    for path in package(args.nro, args.out, args.version):
        print(path)
    return 0


if __name__ == "__main__":
    sys.exit(main())
