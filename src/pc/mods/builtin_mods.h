#ifndef MEMORIES_PC_MODS_BUILTIN_MODS_H
#define MEMORIES_PC_MODS_BUILTIN_MODS_H
/* Code mods linked into the game (MEMORIES_BUILTIN_CODE_MODS): the Switch,
 * whose homebrew has no executable memory of its own to load a mod's object
 * into. tools/pc/build_arm64.py builds each of the repository's code mods
 * (mods/<id>, mod.json's "library") through the translated mod pipeline
 * (build_mod_arm64.build_builtin), renames its entry points after the mod,
 * and writes this table into builtin_mods.c. A mod's folder still holds its
 * mod.json and data; its "library" names a row here instead of a file. */
#include <stdint.h>
#include "pc/mods/modapi.h"

typedef struct MemoriesBuiltinMod {
    const char *library;   /* mod.json's "library", without ".o" */
    MemoriesModEntry init;              /* its MemoriesModInit */
    void (*register_globals)(void);     /* the mod's globals into guest memory */
    void (*unregister_globals)(void);
    uint32_t hash;                      /* of the mod's linked object, for save states */
} MemoriesBuiltinMod;

/* Weak: the tests and a build without mods (--no-builtin-mods) have none. */
extern const MemoriesBuiltinMod Memories_BuiltinMods[] __attribute__((weak));
extern const unsigned Memories_BuiltinModCount __attribute__((weak));

/* The row whose library is `library` (".o" ignored), or NULL. */
const MemoriesBuiltinMod *BuiltinMods_Find(const char *library);

/* The mods' folders, as the repository has them (mod.json and data, no
 * source), so that the .nro alone is a complete install: a path under the
 * mods folder ("3d-monsters/mod.json") and its bytes. */
typedef struct {
    const char *path;
    const unsigned char *data;
    unsigned size;
} MemoriesBuiltinModFile;

extern const MemoriesBuiltinModFile Memories_BuiltinModFiles[] __attribute__((weak));
extern const unsigned Memories_BuiltinModFileCount __attribute__((weak));

/* Writes each of the files into `mods_directory` where it is missing or
 * differs, as a new game's shipped mods replace the old ones: before
 * Mods_Load. Returns how many were written, or -1 if one could not be. */
int BuiltinMods_Install(const char *mods_directory);

#endif
