/* SPDX-License-Identifier: MIT */
#include "game/levels.h"

static const tb_level_info g_levels[24] = {
    {1, "Baublegum City", "Alien boars took over the mall. Kick, shrink and stomp your way to the roof.",
     "Duke Nukem 3D"},
    {2, "Sky Sled over Frostfjord", "Fly the sky sled, claim the glimmer and grow your gingerbread castle.",
     "Magic Carpet 2"},
    {3, "Operation Tinselium", "Harvest Tinselium, capture their refinery, then call the orbital strike.",
     "Command & Conquer"},
    {4, "Tomb of the Frozen Star", "Jump, climb and swim to the star chamber. Mind the Tyranno-Yule.",
     "Tomb Raider"},
    {5, "Tides of Eggnog", "Command your elves, build the fleet and storm the ogre fortress.", "Warcraft II"},
    {6, "Iron Reindeer", "Mind your heat, twist your torso and take down the Snow-Titan.", "MechWarrior 2"},
    {7, "de_december", "Five rounds against the Krampus Krew. Buy wisely. Defuse the coal bomb.",
     "Counter-Strike"},
    {8, "Mistletoe Mesa", "The sample went wrong. Find the scientists and get to the surface.", "Half-Life"},
    {9, "Frostbite Ridge CTF", "Ski, jet and disc your way to the Christmas Flag. Bring it home.",
     "Starsiege: Tribes"},
    {10, "Neon Yule 2096", "Lead your squad, persuade the crowd and topple JollyCorp.", "Syndicate Wars"},
    {11, "Descent into the Gingerbread Mines", "Six directions, one reactor, 50 seconds to get out.",
     "Descent"},
    {12, "Below the Chapel of Tinsel", "Four floors down, loot on every corner. Ah, fresh dough!", "Diablo"},
    {13, "Phobos Gift Depot", "Keycards, infighting and the Big Festive Gun. KRAMPUS.EXE is here.", "Doom"},
    {14, "Song of the Frost Tree", "Charge your swing, ring your spells and make the tree sing again.",
     "Secret of Mana"},
    {15, "XMAS-COM: Yule Defense", "Pause time, plan your squad's moves and keep everyone alive.",
     "X-COM: UFO Defense"},
    {16, "The Elder Yule", "Four runes, one gate, and a truth you will not like.", "Quake"},
    {17, "Knight of the Twelve Nights", "Deflect, push and choose: Nice or Naughty?",
     "Star Wars Jedi Knight: Dark Forces II"},
    {18, "Three Choirs", "Stim, siege, burn the creep, then power the pylons.", "StarCraft"},
    {19, "Unwrapped", "Wake up on a crashed prison ship and step out into the valley.", "Unreal"},
    {20, "Citadel Yuletide", "KRAMPUS.EXE has NOEL. Lower the security, jack in, get NOEL back.",
     "System Shock"},
    {21, "Ages of Advent", "Four ages, one Wonder. Fa-la-la-la-laaa.", "Age of Empires"},
    {22, "Bloody Silent Night", "Home for the holidays. Bring flares and dynamite.", "Blood"},
    {23, "Total Jubilation", "Build the army that will fight on Christmas Eve.", "Total Annihilation"},
    {24, "Code Red-Nose", "Chronoshift past the Iron Curtain. End this. Then choose.",
     "Command & Conquer: Red Alert"},
};

const tb_level_info *tb_level_info_get(int day)
{
    return (day >= 1 && day <= 24) ? &g_levels[day - 1] : 0;
}

tb_story_act tb_level_act(int day)
{
    return day <= 8 ? TB_ACT_BOOT : (day <= 16 ? TB_ACT_DEFRAG : TB_ACT_SHUTDOWN);
}

tb_story_act tb_hall_act(int calendar_day, uint32_t completed_mask)
{
    if (completed_mask & (1u << 23)) {
        return TB_ACT_RESTORED;
    }
    return tb_level_act(calendar_day > 24 ? 24 : calendar_day);
}

const char *tb_story_act_name(tb_story_act act)
{
    static const char *const names[] = {"BOOT", "DEFRAG", "SHUTDOWN", "RESTORED"};
    return (act >= TB_ACT_BOOT && act <= TB_ACT_RESTORED) ? names[act] : "?";
}
