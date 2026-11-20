/* SPDX-License-Identifier: MIT */
/* Day 1: Baublegum City (docs/concept/levels/01-baublegum-city.md).
 * A night-time mall: crash on the roof, throw the breaker, clear the
 * concourse (toilet, cola, arcade, cinema), shrink through the toy store's
 * vent, jetpack over the atrium after the earthquake and beat the
 * Bargain-Bin Battlelord on the parking deck. The story beats live in
 * content/levels/01/level.lua. */
#include "game/level.h"

extern const char tb_level01_lua[];
extern const size_t tb_level01_lua_len;

#define S  TB_CONTENTS_SOLID
#define PC TB_CONTENTS_PLAYERCLIP

typedef struct box {
    float x0, y0, z0, x1, y1, z1;
    uint32_t contents;
} box;

static const box k_world[] = {
    /* Ground slab (the chasm interrupts it) and the chasm floor. */
    {20, -16, -1, 124, 16, 0, S},
    {134, -16, -1, 207, 16, 0, S},
    {124, -16, -13, 134, 16, -12, S},
    /* Roof: deck, parapets, the stairwell housing's west wall with its door gap. */
    {0, -8, 3, 20, 8, 4, S},
    {-0.5f, -8.5f, 3, 0, 8.5f, 6.5f, S},
    {0, 8, 3, 20, 8.5f, 6.5f, S},
    {0, -8.5f, 3, 20, -8, 6.5f, S},
    {20, -8.5f, 0, 20.5f, -1.5f, 8.5f, S},
    {20, 1.5f, 0, 20.5f, 8.5f, 8.5f, S},
    {20, -1.5f, 7, 20.5f, 1.5f, 8.5f, S},
    /* Stairwell: walls, ceiling, landing; steps follow in code. */
    {20.5f, -2, 0, 30, -1.5f, 8, S},
    {20.5f, 1.5f, 0, 30, 2, 8, S},
    {20.5f, -2, 8, 30, 2, 8.5f, S},
    {20.5f, -1.5f, 0, 22, 1.5f, 4, S},
    /* Concourse: ceiling, long walls, west wall around the stairwell. */
    {29.5f, -12.5f, 8, 70.5f, 12.5f, 9, S},
    {30, -12.5f, 0, 70, -12, 8, S},
    {30, 12, 0, 70, 12.5f, 8, S},
    {29.5f, -12, 0, 30, -2, 8, S},
    {29.5f, 2, 0, 30, 12, 8, S},
    /* East wall to the toy store with its doorway. */
    {70, -12, 0, 70.5f, -2, 8, S},
    {70, 2, 0, 70.5f, 12, 8, S},
    {70, -2, 3.5f, 70.5f, 2, 8, S},
    /* Cinema: front wall with doorway, west wall. */
    {56, 4, 0, 60, 4.4f, 8, S},
    {63, 4, 0, 70, 4.4f, 8, S},
    {60, 4, 3, 63, 4.4f, 8, S},
    {56, 4.4f, 0, 56.4f, 12, 8, S},
    /* Machines (player clips; their looks are props). */
    {31, -11.6f, 0, 32, -10.6f, 0.8f, PC},
    {40, 11, 0, 41.5f, 12, 2, PC},
    {50, 11, 0, 51, 12, 1.8f, PC},
    /* Toy store: ceiling, walls, shelves. */
    {70.5f, -12.5f, 6, 95, 12.5f, 7, S},
    {70.5f, -12.5f, 0, 95, -12, 6, S},
    {70.5f, 12, 0, 95, 12.5f, 6, S},
    {76, -9, 0, 84, -8, 2.2f, S},
    {76, 3, 0, 84, 4, 2.2f, S},
    /* The block between toy store and atrium with the mouse-sized vent. */
    {95, -14, 0, 110, -0.5f, 14, S},
    {95, 0.5f, 0, 110, 14, 14, S},
    {95, -0.5f, 0.6f, 110, 0.5f, 14, S},
    /* Atrium: ceiling, walls down into the chasm, the high ledge. */
    {110, -14.5f, 14, 150.5f, 14.5f, 15, S},
    {110, -14.5f, -13, 150, -14, 14, S},
    {110, 14, -13, 150, 14.5f, 14, S},
    {144, 10, 6, 150, 14, 6.5f, S},
    /* Parking deck: wall with entrance, ceiling, walls, pillars. */
    {150, -16.5f, 0, 150.5f, -3, 14, S},
    {150, 3, 0, 150.5f, 16.5f, 14, S},
    {150, -3, 4, 150.5f, 3, 14, S},
    {150.5f, -16.5f, 5, 200.5f, 16.5f, 6, S},
    {150.5f, -16.5f, 0, 200, -16, 5, S},
    {150.5f, 16, 0, 200, 16.5f, 5, S},
    {165, -9, 0, 166.5f, -7.5f, 5, S},
    {165, 7.5f, 0, 166.5f, 9, 5, S},
    {176, -9, 0, 177.5f, -7.5f, 5, S},
    {176, 7.5f, 0, 177.5f, 9, 5, S},
    {170, -1, 0, 171.5f, 1, 1.2f, S}, /* a crashed shopping trolley pile */
    /* East wall with the exit door, and the exit room. */
    {200, -16.5f, 0, 200.5f, -2, 5, S},
    {200, 2, 0, 200.5f, 16.5f, 5, S},
    {200, -2, 3, 200.5f, 2, 5, S},
    {200.5f, -3.5f, 0, 207, -3, 5, S},
    {200.5f, 3, 0, 207, 3.5f, 5, S},
    {206.5f, -3, 0, 207, 3, 5, S},
    {200.5f, -3.5f, 5, 207, 3.5f, 6, S},
};

/* Movers, added last so their brush indices are known. */
enum { MOVER_STAIR_DOOR = 0, MOVER_SCREEN, MOVER_BRIDGE, MOVER_EXIT, MOVER_COUNT };
static const box k_movers[MOVER_COUNT] = {
    {20, -1.5f, 4, 20.5f, 1.5f, 7, S},
    {57, 10.4f, 0.6f, 69.5f, 10.7f, 4.5f, S},
    {124, -1.5f, -0.5f, 134, 1.5f, 0, S},
    {200, -2, 0, 200.5f, 2, 3, S},
};

#define STEPS 15
static int g_mover_base;

static bool add(tb_geometry *g, const box *b)
{
    return tb_geometry_add_box(g, tb_v3(b->x0, b->y0, b->z0), tb_v3(b->x1, b->y1, b->z1), b->contents);
}

static bool build_geometry(tb_geometry *g)
{
    bool ok = true;
    for (size_t i = 0; i < sizeof(k_world) / sizeof(k_world[0]) && ok; i++) {
        ok = add(g, &k_world[i]);
    }
    /* Stairs down from the landing (z 4) to the concourse (z 0). */
    for (int i = 0; i < STEPS && ok; i++) {
        box step = {22.0f + 0.375f * (float)i,     -1.5f, 0, 22.0f + 0.375f * (float)(i + 1), 1.5f,
                    4.0f - 0.25f * (float)(i + 1), S};
        ok = add(g, &step);
    }
    g_mover_base = g->count;
    for (int i = 0; i < MOVER_COUNT && ok; i++) {
        ok = add(g, &k_movers[i]);
    }
    return ok;
}

static tb_handle usable(tb_sim *s, tb_usable_kind kind, tb_vec3 lo, tb_vec3 hi, const char *name,
                        const char *target, int32_t value)
{
    return tb_sim_add_usable(s, kind, lo, hi, name, target, value);
}

static void populate(tb_sim *s)
{
    /* Roof. */
    tb_sim_add_pickup(s, TB_PICKUP_WEAPON, tb_v3(6, 0, 4.5f), TB_WEAPON_MIGHTY_BOOT, 0, true);
    usable(s, TB_USE_SWITCH, tb_v3(19.6f, -5, 4.8f), tb_v3(20, -4, 5.8f), "breaker", "stair_door", 2);
    tb_sim_add_enemy(s, TB_ENEMY_TROOPER, tb_v3(15, 5, 4), NULL);
    tb_sim_add_mover(s, g_mover_base + MOVER_STAIR_DOOR, tb_v3(0, 3.2f, 0), 2.5f, "stair_door", false);

    /* Concourse. */
    tb_sim_add_pickup(s, TB_PICKUP_WEAPON, tb_v3(21.2f, 0, 4.5f), TB_WEAPON_TEST_SHOTGUN, 24, true);
    tb_sim_add_pickup(s, TB_PICKUP_ARMOUR, tb_v3(36, -6, 0.5f), 0, 25, false);
    tb_sim_add_pickup(s, TB_PICKUP_AMMO, tb_v3(46, 8, 0.5f), TB_AMMO_SHELLS, 16, false);
    tb_sim_add_pickup(s, TB_PICKUP_HEALTH, tb_v3(60, -10, 0.5f), 0, 25, false);
    tb_sim_add_pickup(s, TB_PICKUP_HEALTH, tb_v3(33, 6, 0.5f), 0, 25, false);
    tb_sim_add_pickup(s, TB_PICKUP_ARMOUR, tb_v3(33, -6, 0.5f), 0, 25, false);
    tb_sim_add_trigger(s, TB_TRIGGER_CHECKPOINT, tb_v3(28, -2, 0), tb_v3(31, 2, 4), 2);
    usable(s, TB_USE_TOILET, tb_v3(31, -11.6f, 0), tb_v3(32, -10.6f, 0.8f), "toilet", NULL, 0);
    usable(s, TB_USE_VENDING, tb_v3(40, 11, 0), tb_v3(41.5f, 12, 2), "vending", NULL, 0);
    usable(s, TB_USE_ARCADE, tb_v3(50, 11, 0), tb_v3(51, 12, 1.8f), "arcade", NULL, 0);
    usable(s, TB_USE_BUTTON, tb_v3(56.4f, 5, 1.2f), tb_v3(56.8f, 6, 2), "projector", "screen", 0);
    tb_sim_add_mover(s, g_mover_base + MOVER_SCREEN, tb_v3(0, 0, 3.3f), 2.0f, "screen", false);
    tb_sim_add_pickup(s, TB_PICKUP_FLOPPY, tb_v3(63, 11.3f, 0.5f), 0, 0, false);
    tb_sim_add_enemy(s, TB_ENEMY_TROOPER, tb_v3(45, -6, 0), NULL);
    tb_sim_add_enemy(s, TB_ENEMY_TROOPER, tb_v3(55, -8, 0), NULL);
    tb_sim_add_enemy(s, TB_ENEMY_TROOPER, tb_v3(50, 6, 0), NULL);
    tb_sim_add_enemy(s, TB_ENEMY_LIZARD, tb_v3(66, -6, 0), NULL);

    /* Toy store. */
    tb_sim_add_relay(s, tb_v3(71, -12, 0), tb_v3(73, 12, 6), "toystore", NULL, true);
    tb_sim_add_trigger(s, TB_TRIGGER_CHECKPOINT, tb_v3(70.5f, -3, 0), tb_v3(73, 3, 4), 3);
    tb_sim_add_pickup(s, TB_PICKUP_WEAPON, tb_v3(75, 8, 0.5f), TB_WEAPON_PIPE_BAUBLES, 6, true);
    tb_sim_add_pickup(s, TB_PICKUP_AMMO, tb_v3(86, -10, 0.5f), TB_AMMO_PIPES, 4, false);
    tb_sim_add_pickup(s, TB_PICKUP_AMMO, tb_v3(88, 10, 0.5f), TB_AMMO_SHELLS, 16, false);
    tb_sim_add_enemy(s, TB_ENEMY_BRAIN, tb_v3(82, 6, 3), NULL);
    tb_sim_add_enemy(s, TB_ENEMY_BRAIN, tb_v3(88, -5, 3), NULL);
    tb_sim_add_enemy(s, TB_ENEMY_TROOPER, tb_v3(90, 7, 0), NULL);
    tb_sim_add_trigger(s, TB_TRIGGER_SHRINK, tb_v3(91, -2, 0), tb_v3(112, 2, 2), 0);

    /* Atrium. */
    tb_sim_add_trigger(s, TB_TRIGGER_CHECKPOINT, tb_v3(110, -14, 0), tb_v3(116, 14, 4), 4);
    tb_sim_add_pickup(s, TB_PICKUP_WEAPON, tb_v3(113, 0, 0.5f), TB_WEAPON_GIFT_SHRINKER, 20, true);
    tb_sim_add_pickup(s, TB_PICKUP_ITEM, tb_v3(114, 8, 0.5f), TB_ITEM_JETPACK, 0, true);
    tb_sim_add_relay(s, tb_v3(117, -14, 0), tb_v3(119, 14, 6), "quake", NULL, true);
    tb_sim_add_mover(s, g_mover_base + MOVER_BRIDGE, tb_v3(0, 0, -11), 6.0f, "bridge", false);
    tb_entity *chasm =
        tb_sim_get(s, tb_sim_add_trigger(s, TB_TRIGGER_HURT, tb_v3(124, -14, -12), tb_v3(134, 14, -4), 0));
    if (chasm) {
        chasm->value = 1000;
    }
    tb_sim_add_pickup(s, TB_PICKUP_ITEM, tb_v3(138, -8, 0.5f), TB_ITEM_HOLO_SANTA, 0, true);
    tb_sim_add_pickup(s, TB_PICKUP_FLOPPY, tb_v3(147, 12, 7.0f), 2, 0, false);
    tb_sim_add_pickup(s, TB_PICKUP_HEALTH, tb_v3(146, -12, 0.5f), 0, 25, false);
    tb_sim_add_enemy(s, TB_ENEMY_LIZARD, tb_v3(142, 6, 0), NULL);
    tb_sim_add_enemy(s, TB_ENEMY_LIZARD, tb_v3(146, -6, 0), NULL);

    /* Parking deck: the boss arrives by script. */
    tb_sim_add_trigger(s, TB_TRIGGER_CHECKPOINT, tb_v3(150.5f, -3, 0), tb_v3(153, 3, 4), 5);
    tb_sim_add_relay(s, tb_v3(156, -16, 0), tb_v3(158, 16, 5), "deck", NULL, true);
    tb_sim_add_pickup(s, TB_PICKUP_ARMOUR, tb_v3(154, -12, 0.5f), 0, 50, false);
    tb_sim_add_pickup(s, TB_PICKUP_HEALTH, tb_v3(154, 12, 0.5f), 0, 25, false);
    tb_sim_add_pickup(s, TB_PICKUP_AMMO, tb_v3(160, 4, 0.5f), TB_AMMO_SHELLS, 24, false);
    tb_sim_add_pickup(s, TB_PICKUP_AMMO, tb_v3(168, -12, 0.5f), TB_AMMO_PIPES, 6, false);
    tb_sim_add_pickup(s, TB_PICKUP_HEALTH, tb_v3(172, 12, 0.5f), 0, 25, false);
    tb_sim_add_mover(s, g_mover_base + MOVER_EXIT, tb_v3(0, 0, -2.95f), 2.0f, "exit_door", false);
    tb_sim_add_trigger(s, TB_TRIGGER_EXIT, tb_v3(202, -3, 0), tb_v3(206.5f, 3, 3), 0);
}

/* Light groups: 0 always on, 2 stairwell (breaker), 3 parking alarm. */
static const tb_level_light k_lights[] = {
    {{10, 7.5f, 6}, {2.2f, 0.4f, 1.8f}, 14, 0},    /* BAUBLEGUM neon sign */
    {{2, -7, 5}, {0.3f, 1.4f, 2.0f}, 12, 0},       /* roof floodlight */
    {{25, 0, 7}, {1.6f, 1.2f, 0.6f}, 10, 2},       /* stairwell */
    {{36, 0, 6}, {2.0f, 1.7f, 1.2f}, 20, 0},       /* concourse */
    {{50, -4, 6}, {1.7f, 2.0f, 2.3f}, 20, 0},      /* concourse */
    {{40.7f, 10, 2.5f}, {0.4f, 0.8f, 2.0f}, 6, 0}, /* vending glow */
    {{50.5f, 10, 2}, {1.8f, 0.4f, 1.6f}, 5, 0},    /* arcade */
    {{63, 8, 6}, {0.8f, 0.6f, 1.8f}, 10, 0},       /* cinema */
    {{65, -4, 6}, {2.3f, 1.4f, 0.8f}, 18, 0},      /* concourse */
    {{78, 0, 5}, {2.0f, 1.2f, 0.6f}, 14, 0},       /* toy store */
    {{90, 0, 5}, {0.6f, 2.0f, 0.8f}, 14, 0},       /* toy store */
    {{92, 0, 1}, {0.4f, 2.2f, 1.2f}, 5, 0},        /* Shrink-O-Matic */
    {{115, 0, 8}, {1.8f, 1.8f, 2.4f}, 26, 0},      /* atrium */
    {{130, 0, 9}, {2.6f, 1.2f, 0.8f}, 28, 0},      /* atrium */
    {{144, 6, 8}, {0.9f, 2.0f, 2.8f}, 22, 0},      /* atrium ledge */
    {{129, 0, -8}, {2.4f, 0.6f, 0.2f}, 12, 0},     /* chasm glow */
    {{160, 0, 4.5f}, {1.4f, 1.4f, 1.0f}, 14, 0},   /* parking */
    {{175, 0, 4.5f}, {1.4f, 1.4f, 1.0f}, 14, 0},   /* parking */
    {{190, 0, 4.5f}, {1.4f, 1.4f, 1.0f}, 14, 0},   /* parking */
    {{186, 0, 4.5f}, {2.5f, 0.2f, 0.2f}, 18, 3},   /* boss alarm */
    {{203, 0, 4}, {2.0f, 1.8f, 1.0f}, 8, 0},       /* exit */
};

static const tb_level_prop k_props[] = {
    {{4, 7.6f, 5}, {16, 8, 6.4f}, {255, 60, 200, 255}, true},           /* BAUBLEGUM sign */
    {{31, -11.6f, 0}, {32, -10.6f, 0.8f}, {235, 235, 240, 255}, false}, /* toilet */
    {{40, 11, 0}, {41.5f, 12, 2}, {200, 30, 40, 255}, false},           /* cola machine */
    {{40.2f, 10.95f, 1.0f}, {41.3f, 11, 1.8f}, {120, 200, 255, 255}, true},
    {{50, 11, 0}, {51, 12, 1.8f}, {40, 40, 90, 255}, false}, /* arcade cabinet */
    {{50.1f, 10.95f, 1.1f}, {50.9f, 11, 1.6f}, {120, 255, 120, 255}, true},
    {{19.8f, -5, 4.8f}, {20, -4, 5.8f}, {230, 200, 40, 255}, false}, /* breaker */
    {{56.4f, 5, 1.2f}, {56.6f, 6, 2}, {255, 80, 60, 255}, true},     /* projector button */
    {{91, -2, 0}, {93, 2, 0.05f}, {80, 255, 160, 255}, true},        /* Shrink-O-Matic pad */
    {{91, -2.2f, 0}, {91.3f, -1.9f, 2.5f}, {200, 200, 220, 255}, false},
    {{91, 1.9f, 0}, {91.3f, 2.2f, 2.5f}, {200, 200, 220, 255}, false},
    {{76, -9, 2.2f}, {84, -8, 2.4f}, {255, 120, 40, 255}, false}, /* toy shelves */
    {{76, 3, 2.2f}, {84, 4, 2.4f}, {60, 160, 255, 255}, false},
    {{199.9f, -2.5f, 3.2f}, {200, 2.5f, 3.8f}, {40, 255, 80, 255}, true}, /* EXIT sign */
};

static const tb_bot_step k_golden[] = {
    {TB_BOT_GOTO, {6, 0, 4}, 0}, /* the Mighty Boot */
    {TB_BOT_SELECT, {0, 0, 0}, 2},
    {TB_BOT_CLEAR, {12, 0, 4}, 1200},
    {TB_BOT_GOTO, {18.3f, -4.5f, 4}, 0},
    {TB_BOT_USE, {19.8f, -4.5f, 5.3f}, 0}, /* breaker */
    {TB_BOT_WAIT, {0, 0, 0}, 120},
    {TB_BOT_GOTO, {19, 0, 4}, 0},
    {TB_BOT_GOTO, {21.5f, 0, 4}, 0}, /* pump shotgun */
    {TB_BOT_SELECT, {0, 0, 0}, 4},
    {TB_BOT_GOTO, {29, 0, 0}, 0},
    {TB_BOT_CHECKPOINT, {0, 0, 0}, 0},
    {TB_BOT_GOTO, {33, -6, 0}, 0}, /* armour */
    {TB_BOT_GOTO, {36, -6, 0}, 0}, /* more armour */
    {TB_BOT_CLEAR, {50, 0, 0}, 2400},
    {TB_BOT_GOTO, {31.8f, -9.9f, 0}, 0},
    {TB_BOT_USE, {31.5f, -11.1f, 0.6f}, 0}, /* toilet */
    {TB_BOT_GOTO, {40.75f, 9.9f, 0}, 0},
    {TB_BOT_USE, {40.75f, 11.2f, 1.2f}, 0}, /* cola */
    {TB_BOT_GOTO, {50.5f, 9.9f, 0}, 0},
    {TB_BOT_USE, {50.5f, 11.2f, 1.2f}, 0}, /* arcade x3 */
    {TB_BOT_WAIT, {0, 0, 0}, 40},
    {TB_BOT_USE, {50.5f, 11.2f, 1.2f}, 0},
    {TB_BOT_WAIT, {0, 0, 0}, 40},
    {TB_BOT_USE, {50.5f, 11.2f, 1.2f}, 0},
    {TB_BOT_GOTO, {50.5f, 9.0f, 0}, 0}, /* floppy 2 */
    {TB_BOT_GOTO, {46, 8, 0}, 0},       /* shells */
    {TB_BOT_GOTO, {61.5f, 2, 0}, 0},
    {TB_BOT_GOTO, {61.5f, 6, 0}, 0},
    {TB_BOT_GOTO, {57.6f, 5.5f, 0}, 0},
    {TB_BOT_USE, {56.6f, 5.5f, 1.6f}, 0}, /* projector */
    {TB_BOT_WAIT, {0, 0, 0}, 120},
    {TB_BOT_GOTO, {63, 9, 0}, 0},
    {TB_BOT_GOTO, {63, 11.3f, 0}, 0}, /* floppy 1 */
    {TB_BOT_GOTO, {61.5f, 6, 0}, 0},
    {TB_BOT_GOTO, {61.5f, 2, 0}, 0},
    {TB_BOT_GOTO, {68, 0, 0}, 0},
    {TB_BOT_GOTO, {72, 0, 0}, 0},
    {TB_BOT_CHECKPOINT, {0, 0, 0}, 0},
    {TB_BOT_GOTO, {75, 8, 0}, 0}, /* pipe baubles */
    {TB_BOT_CLEAR, {85, 0, 0}, 2400},
    {TB_BOT_GOTO, {88, 10, 0}, 0},
    {TB_BOT_GOTO, {86, 0, 0}, 0},
    {TB_BOT_GOTO, {92, 0, 0}, 0},  /* Shrink-O-Matic */
    {TB_BOT_GOTO, {111, 0, 0}, 0}, /* through the vent */
    {TB_BOT_GOTO, {113, 0, 0}, 0}, /* Gift-Shrinker */
    {TB_BOT_GOTO, {114, 8, 0}, 0}, /* jetpack */
    {TB_BOT_CHECKPOINT, {0, 0, 0}, 0},
    {TB_BOT_GOTO, {118, 0, 0}, 0}, /* earthquake */
    {TB_BOT_WAIT, {0, 0, 0}, 150},
    {TB_BOT_SELECT, {0, 0, 0}, 4},
    {TB_BOT_GOTO, {121, 0, 0}, 0},
    {TB_BOT_JET_TO, {139, 0, 0}, 0},
    {TB_BOT_CLEAR, {140, 0, 0}, 2400},
    {TB_BOT_GOTO, {138, -8, 0}, 0}, /* Holo-Santa */
    {TB_BOT_GOTO, {146, -12, 0}, 0},
    {TB_BOT_GOTO, {147, 6, 0}, 0},
    {TB_BOT_WAIT, {0, 0, 0}, 30},
    {TB_BOT_JET_TO, {147, 12, 6.5f}, 0}, /* floppy 3 on the ledge */
    {TB_BOT_GOTO, {147, 12, 6.5f}, 0},
    {TB_BOT_WAIT, {0, 0, 0}, 30},
    {TB_BOT_GOTO, {147, 6, 0}, 0},
    {TB_BOT_WAIT, {0, 0, 0}, 60},
    {TB_BOT_GOTO, {152, 0, 0}, 0},
    {TB_BOT_GOTO, {154, -12, 0}, 0}, /* armour */
    {TB_BOT_GOTO, {154, 12, 0}, 0},  /* health */
    {TB_BOT_GOTO, {160, 4, 0}, 0},   /* shells */
    {TB_BOT_CHECKPOINT, {0, 0, 0}, 0},
    {TB_BOT_CLEAR, {186, 0, 0}, 5400}, /* the Battlelord */
    {TB_BOT_GOTO, {168, 3, 0}, 0},
    {TB_BOT_GOTO, {186, 0, 0}, 0}, /* Memory Bauble */
    {TB_BOT_WAIT, {0, 0, 0}, 120},
    {TB_BOT_GOTO, {199, 0, 0}, 0},
    {TB_BOT_GOTO, {204, 0, 0}, 0},
};

static tb_level_def g_def;

const tb_level_def *tb_level_day01(void)
{
    if (g_def.manifest.day == 0) {
        tb_level_def d = {
            .manifest =
                {
                    .day = 1,
                    .mechanics = {"interactive world", "shrink and stomp", "jetpack and Holo-Santa"},
                    .mechanic_count = 3,
                    .loaners = {"Mighty Boot", "Pump Shotgun", "Pipe Baubles", "Gift-Shrinker", "Jetpack",
                                "Holo-Santa"},
                    .loaner_count = 6,
                    .trophy = "Gift-Shrinker",
                    .bars = {"Jet fuel"},
                    .bar_count = 1,
                    .actions = {"Use", "Jetpack", "Holo-Santa"},
                    .action_count = 3,
                    .comfort = TB_COMFORT_MODERATE,
                    .palette = "baublegum-neon",
                    .music = "mall-rat-rock",
                    .minutes = 20,
                },
            .build_geometry = build_geometry,
            .populate = populate,
            .lights = k_lights,
            .light_count = (int)(sizeof(k_lights) / sizeof(k_lights[0])),
            .props = k_props,
            .prop_count = (int)(sizeof(k_props) / sizeof(k_props[0])),
            .script = tb_level01_lua,
            .script_len = tb_level01_lua_len,
            .spawn = {3, 0, 4.01f},
            .spawn_yaw = 0.0f,
            .floor = {90, 80, 95, 255},
            .wall = {140, 120, 160, 255},
            .ceiling = {50, 45, 70, 255},
            .ambient = {0.34f, 0.28f, 0.44f},
            .sun_color = {0.5f, 0.45f, 0.8f},
            .fog_color = {0.10f, 0.03f, 0.12f},
            .fog_start = 18.0f,
            .fog_end = 90.0f,
            .golden_path = k_golden,
            .golden_count = (int)(sizeof(k_golden) / sizeof(k_golden[0])),
            .golden_ticks = 20u * 60u * TB_SIM_HZ_TICKS,
            .objective = "GET OFF THE ROOF",
        };
        g_def = d;
    }
    return &g_def;
}
