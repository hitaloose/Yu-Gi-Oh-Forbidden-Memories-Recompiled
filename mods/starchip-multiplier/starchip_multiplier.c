/* The StarChip multiplier mod (mod.json beside this file), from the one
 * upstream PR #181 proposed with the event it added: the end-of-duel prize,
 * as MEMORIES_EVENT_STARCHIP hands it over (Mods_AwardStarchips), times the
 * "multiplier" setting. The results screen still draws the prize it worked
 * out before the event; only what the save gains is multiplied, and the
 * host still caps the balance (999999, or a mod's "limits" "starchips"). */
#include "pc/mods/modapi.h"

#define MULTIPLIER_MAX 100
#define PRIZE_MAX 999999

static const MemoriesModHost *host;

static void starchip(MemoriesModEvent *event)
{
    long long prize;
    int multiplier;

    if (event->phase != MEMORIES_BEFORE)
        return;
    multiplier = host->setting(host, "multiplier", 5);
    if (multiplier < 1)
        multiplier = 1;
    if (multiplier > MULTIPLIER_MAX)
        multiplier = MULTIPLIER_MAX;
    prize = (long long)event->a * multiplier;
    if (prize < 0)
        prize = 0;
    if (prize > PRIZE_MAX)
        prize = PRIZE_MAX;
    event->a = (int)prize;
}

int MemoriesModInit(const MemoriesModHost *from, MemoriesMod *mod)
{
    if (from->api < 6)
        return 0;
    host = from;
    mod->api = 6;
    return host->subscribe(host, MEMORIES_EVENT_STARCHIP, 0, starchip) != 0;
}
