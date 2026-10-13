/* Code mods linked into the game (MEMORIES_BUILTIN_CODE_MODS, the Switch,
 * src/pc/mods/builtin_mods.h): a mod's "library" names a row of the game's
 * table instead of a file, its globals are registered when it loads, and a
 * code mod the game was not built with stays off with the reason beside it. */
#define _POSIX_C_SOURCE 200809L
#include "pc/mods/builtin_mods.h"
#include "pc/mods/exports.h"
#include "pc/mods/mods.h"
#include "pc/mods/object_loader.h"
#include "pc/debug/symbols.h"
#include "pc/platform/settings.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "pc/compat/posix.h"
#include "scratch.h"

#ifndef MEMORIES_BUILTIN_CODE_MODS
#error "built with MEMORIES_BUILTIN_CODE_MODS (CMakeLists.txt)"
#endif

/* --- the port around the mod system ---------------------------------- */

int Log_Enabled(int channel) { (void)channel; return 0; }
int Log_Wanted(int channel) { (void)channel; return 0; }
void Log_Printf(int channel, const char *format, ...) { (void)channel; (void)format; }
unsigned short Platform_Pad(int port) { (void)port; return 0; }
int Symbols_Add(const SymbolsEntry *entries, size_t count) { (void)entries; (void)count; return 0; }
int Memories_DiscReadSectors(int lba, int sectors, void *out)
{
    (void)lba;
    memset(out, 0, (size_t)sectors * 2048);
    return sectors;
}
int Memories_DiscSectorCount(void) { return 10000; }
int Memories_DiscOriginalFileInfo(const char *path, int *lba, unsigned *size)
{
    (void)path; (void)lba; (void)size;
    return -1;
}
int Memories_DiscFileInfo(const char *path, int *lba, unsigned *size) { return Memories_DiscOriginalFileInfo(path, lba, size); }
int Memories_DiscFileStart(const char *path) { (void)path; return -1; }

const MemoriesModExport Memories_ModExports[] = {{"D_80010000", (void *)0x80010000u}};
const unsigned Memories_ModExportCount = 1;

/* --- the game's table (build_arm64.py writes it as builtin_mods.c) --- */

static int inits, registered, unregistered;
static void frame(void) {}
static int linked_init(const MemoriesModHost *host, MemoriesMod *mod)
{
    assert(host);
    inits++;
    mod->api = MEMORIES_MOD_API;
    mod->frame = frame;
    return 1;
}
static void linked_register(void) { registered++; }
static void linked_unregister(void) { unregistered++; }

const MemoriesBuiltinMod Memories_BuiltinMods[] = {
    {"linked", linked_init, linked_register, linked_unregister, 0x1234abcdu},
};
const unsigned Memories_BuiltinModCount = 1;

/* Its folder, which the game writes into the mods folder before Mods_Load. */
static const unsigned char linked_json[] =
    "{ \"id\": \"linked\", \"library\": \"linked.o\", \"enabled\": true, \"restart\": false }";
const MemoriesBuiltinModFile Memories_BuiltinModFiles[] = {
    {"linked/mod.json", linked_json, sizeof(linked_json) - 1},
};
const unsigned Memories_BuiltinModFileCount = 1;

/* --- fixtures -------------------------------------------------------- */

static char root[SCRATCH_MAX];

static void write_text(const char *relative, const char *text)
{
    char path[2048];
    FILE *file;
    snprintf(path, sizeof(path), "%s/%s", root, relative);
    file = fopen(path, "wb");
    assert(file);
    assert(fwrite(text, 1, strlen(text), file) == strlen(text));
    assert(!fclose(file));
}

static void make_dir(const char *relative)
{
    char path[2048];
    snprintf(path, sizeof(path), "%s/%s", root, relative);
    assert(!mkdir(path, 0777));
}

static int find(const char *id)
{
    int i;
    for (i = 0; i < Mods_Count(); i++) {
        if (!strcmp(Mods_Id(i), id)) return i;
    }
    return -1;
}

int main(void)
{
    char settings[1024], error[160];
    LoadedObject object;
    int linked, named, missing;

    assert(!strcmp(OBJECT_LOADER_TARGET, "switch"));
    /* Rows by name, ".o" or not. */
    assert(BuiltinMods_Find("linked") == &Memories_BuiltinMods[0]);
    assert(BuiltinMods_Find("linked.o") == &Memories_BuiltinMods[0]);
    assert(!BuiltinMods_Find("link") && !BuiltinMods_Find("linked.dylib") && !BuiltinMods_Find(NULL));

    /* Loading registers the mod's globals, and freeing it gives them back. */
    assert(!ObjectLoader_LoadBuiltin("linked", &object, error, sizeof(error)));
    assert(registered == 1 && object.hash == 0x1234abcdu);
    {
        union { MemoriesModEntry function; void *pointer; } entry = {linked_init};
        assert(ObjectLoader_Symbol(&object, "MemoriesModInit") == entry.pointer);
    }
    assert(!ObjectLoader_Symbol(&object, "MemoriesModRegisterGlobals"));
    ObjectLoader_Free(&object);
    assert(unregistered == 1 && !object.builtin);
    assert(ObjectLoader_LoadBuiltin("absent", &object, error, sizeof(error)) && strstr(error, "not built into"));
    registered = unregistered = 0;

    assert(scratch_dir(root, sizeof(root), "memories-mods"));
    make_dir("mods");
    /* The linked-in mod's folder comes from the game: written where it is
     * missing, left where it is the same, and rewritten where it differs. */
    {
        char mods_dir[2048];
        snprintf(mods_dir, sizeof(mods_dir), "%s/mods", root);
        assert(BuiltinMods_Install(mods_dir) == 1);
        assert(BuiltinMods_Install(mods_dir) == 0);
        write_text("mods/linked/mod.json", "{ \"id\": \"stale\" }");
        assert(BuiltinMods_Install(mods_dir) == 1);
        assert(BuiltinMods_Install(mods_dir) == 0);
    }
    /* "libraries" names this game's row outright. */
    make_dir("mods/named");
    write_text("mods/named/mod.json",
               "{ \"id\": \"named\", \"library\": \"elsewhere\", \"libraries\": { \"switch\": \"linked\" },"
               "  \"enabled\": true, \"restart\": false }");
    /* A code mod that is not linked in: off, with the reason, not broken. */
    make_dir("mods/missing");
    write_text("mods/missing/mod.json",
               "{ \"id\": \"missing\", \"library\": \"missing\", \"enabled\": true, \"restart\": false }");

    snprintf(settings, sizeof(settings), "%s/settings.txt", root);
    write_text("settings.txt", "");
    assert(!setenv("MEMORIES_SETTINGS", settings, 1));
    assert(!setenv("MEMORIES_USER_DIR", root, 1));
    Settings_Load();
    Mods_Load();

    linked = find("linked");
    named = find("named");
    missing = find("missing");
    assert(linked >= 0 && named >= 0 && missing >= 0);
    assert(Mods_Active(linked) && Mods_Active(named));
    assert(Mods_CodeHash(linked) == 0x1234abcdu);
    assert(inits == 2 && registered == 2);
    assert(!Mods_Active(missing) && strstr(Mods_Status(missing), "not built with"));

    Mods_Shutdown();
    return 0;
}
