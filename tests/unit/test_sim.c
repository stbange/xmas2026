/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "sim/sim.h"

#include <string.h>

static tb_brush g_store[TB_TEST_ARENA_BRUSHES];
static tb_geometry g_geo;
static tb_sim g_sim;
static tb_input g_in[TB_MAX_PLAYERS];

static void make_sim(tb_difficulty diff, int players)
{
    tb_sim_config cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.geometry = &g_geo;
    cfg.seed = 99;
    cfg.difficulty = diff;
    for (int i = 0; i < TB_MAX_PLAYERS; i++) {
        cfg.spawn[i] = tb_v3(0, (float)i * -2.0f, 0.01f);
    }
    tb_sim_init(&g_sim, &cfg);
    for (int i = 0; i < players; i++) {
        TEST_ASSERT_TRUE(tb_sim_add_player(&g_sim, i));
    }
}

void setUp(void)
{
    tb_geometry_build_test_arena(&g_geo, g_store);
    memset(g_in, 0, sizeof(g_in));
    make_sim(TB_DIFF_COLA, 1);
}
void tearDown(void) {}

static void step(int ticks)
{
    for (int i = 0; i < ticks; i++) {
        tb_sim_step(&g_sim, g_in);
    }
}

static tb_player *P(int i)
{
    return &g_sim.players[i];
}

/* ---- entities ---- */

static void test_handles_and_stale_references(void)
{
    tb_handle h = tb_sim_add_target(&g_sim, tb_v3(3, 0, 0.9f), 50);
    TEST_ASSERT_NOT_NULL(tb_sim_get(&g_sim, h));
    tb_sim_remove(&g_sim, h);
    TEST_ASSERT_NULL(tb_sim_get(&g_sim, h));
    tb_handle h2 = tb_sim_add_target(&g_sim, tb_v3(3, 0, 0.9f), 50);
    TEST_ASSERT_NOT_EQUAL(h, h2); /* same slot, new generation */
    TEST_ASSERT_NULL(tb_sim_get(&g_sim, h));
    TEST_ASSERT_NULL(tb_sim_get(&g_sim, 0));
    TEST_ASSERT_NULL(tb_sim_get(&g_sim, 0xFFFFu));
    tb_sim_remove(&g_sim, 0);
}

static void test_entity_table_full(void)
{
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        TEST_ASSERT_NOT_EQUAL(0, tb_sim_add_target(&g_sim, tb_v3(-15, -15, 0.9f), 1));
    }
    TEST_ASSERT_EQUAL_UINT32(0, tb_sim_add_target(&g_sim, tb_v3(0, 0, 0), 1));
    TEST_ASSERT_EQUAL_UINT32(0, tb_sim_add_pickup(&g_sim, TB_PICKUP_HEALTH, tb_v3(0, 0, 0), 0, 1, false));
    TEST_ASSERT_EQUAL_UINT32(0,
                             tb_sim_add_trigger(&g_sim, TB_TRIGGER_EXIT, tb_v3(0, 0, 0), tb_v3(1, 1, 1), 0));
}

static void test_players(void)
{
    TEST_ASSERT_EQUAL_INT(1, tb_sim_player_count(&g_sim));
    TEST_ASSERT_FALSE(tb_sim_add_player(&g_sim, 0));
    TEST_ASSERT_FALSE(tb_sim_add_player(&g_sim, -1));
    TEST_ASSERT_FALSE(tb_sim_add_player(&g_sim, TB_MAX_PLAYERS));
    TEST_ASSERT_EQUAL_INT32(100, P(0)->vitals.health);
}

/* ---- weapons ---- */

static void test_hitscan_shotgun_kills_target(void)
{
    tb_handle t = tb_sim_add_target(&g_sim, tb_v3(3, 0, 1.6f), 50); /* centred on the eye line */
    tb_inventory_give_weapon(&P(0)->inv, TB_WEAPON_TEST_SHOTGUN);
    tb_inventory_add_ammo(&P(0)->inv, TB_AMMO_SHELLS, 5);
    g_in[0].weapon = 3;
    g_in[0].buttons = TB_BTN_FIRE;
    step(1);
    TEST_ASSERT_NULL(tb_sim_get(&g_sim, t)); /* 7 pellets x 8 */
    TEST_ASSERT_EQUAL_INT32(4, P(0)->inv.ammo[TB_AMMO_SHELLS]);
}

static void test_hitscan_blocked_by_wall(void)
{
    /* Target behind the pillar. */
    P(0)->move.origin = tb_v3(-5, 2, 0.01f);
    g_in[0].yaw = 16384; /* north */
    tb_handle t = tb_sim_add_target(&g_sim, tb_v3(-5, 9, 0.9f), 50);
    tb_inventory_give_weapon(&P(0)->inv, TB_WEAPON_TEST_SHOTGUN);
    tb_inventory_add_ammo(&P(0)->inv, TB_AMMO_SHELLS, 5);
    g_in[0].weapon = 3;
    g_in[0].buttons = TB_BTN_FIRE;
    step(1);
    TEST_ASSERT_EQUAL_INT32(50, tb_sim_get(&g_sim, t)->vitals.health);
}

static void test_snowball_tap_and_charged_stun(void)
{
    tb_handle t = tb_sim_add_target(&g_sim, tb_v3(4, 0, 0.9f), 100);
    g_in[0].buttons = TB_BTN_FIRE;
    step(1);
    g_in[0].buttons = 0;
    step(30);
    TEST_ASSERT_EQUAL_INT32(90, tb_sim_get(&g_sim, t)->vitals.health);
    g_in[0].buttons = TB_BTN_FIRE;
    step(70);
    g_in[0].buttons = 0;
    step(30);
    tb_entity *e = tb_sim_get(&g_sim, t);
    TEST_ASSERT_EQUAL_INT32(50, e->vitals.health);
    TEST_ASSERT_TRUE(e->stun_ticks > 0);
    step(100);
    TEST_ASSERT_EQUAL_UINT32(0, e->stun_ticks);
}

static void test_projectile_hits_wall_and_expires(void)
{
    P(0)->move.origin = tb_v3(15, 0, 0.01f);
    g_in[0].buttons = TB_BTN_FIRE;
    step(1);
    g_in[0].buttons = 0;
    step(1);
    int projectiles = 0;
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        projectiles += g_sim.entities[i].type == TB_ENT_PROJECTILE;
    }
    TEST_ASSERT_EQUAL_INT(1, projectiles);
    step(20);
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        TEST_ASSERT_NOT_EQUAL(TB_ENT_PROJECTILE, g_sim.entities[i].type);
    }
    /* Fired straight up into nothing: removed when its time runs out. */
    tb_handle h = tb_sim_spawn(&g_sim, TB_ENT_PROJECTILE, tb_v3(0, 0, 3), tb_v3(0.1f, 0.1f, 0.1f));
    tb_sim_get(&g_sim, h)->ttl = 2;
    step(2);
    TEST_ASSERT_NULL(tb_sim_get(&g_sim, h));
}

static void test_launcher_splash_hurts_targets_and_self(void)
{
    P(0)->move.origin = tb_v3(18, 0, 0.01f);
    tb_handle near = tb_sim_add_target(&g_sim, tb_v3(19, 2, 0.9f), 100);
    tb_handle far = tb_sim_add_target(&g_sim, tb_v3(10, 15, 0.9f), 100);
    tb_inventory_give_weapon(&P(0)->inv, TB_WEAPON_TEST_LAUNCHER);
    tb_inventory_add_ammo(&P(0)->inv, TB_AMMO_ROCKETS, 1);
    g_in[0].weapon = 3;
    g_in[0].buttons = TB_BTN_FIRE;
    step(1);
    g_in[0].buttons = 0;
    step(20);
    TEST_ASSERT_TRUE(tb_sim_get(&g_sim, near)->vitals.health < 100);
    TEST_ASSERT_EQUAL_INT32(100, tb_sim_get(&g_sim, far)->vitals.health);
    TEST_ASSERT_TRUE(P(0)->vitals.health < 100);
}

static void test_rocket_direct_hit(void)
{
    tb_handle t = tb_sim_add_target(&g_sim, tb_v3(4, 0, 1.0f), 200);
    tb_inventory_give_weapon(&P(0)->inv, TB_WEAPON_TEST_LAUNCHER);
    tb_inventory_add_ammo(&P(0)->inv, TB_AMMO_ROCKETS, 1);
    g_in[0].weapon = 3;
    g_in[0].buttons = TB_BTN_FIRE;
    step(1);
    g_in[0].buttons = 0;
    step(20);
    TEST_ASSERT_TRUE(tb_sim_get(&g_sim, t)->vitals.health <= 200 - 60); /* direct + splash */
}

static void test_melee_button_uses_candy_cane(void)
{
    tb_handle t = tb_sim_add_target(&g_sim, tb_v3(1.2f, 0, 0.9f), 100);
    g_in[0].buttons = TB_BTN_MELEE;
    step(5); /* held: only one swing */
    TEST_ASSERT_EQUAL_INT32(75, tb_sim_get(&g_sim, t)->vitals.health);
    g_in[0].buttons = 0;
    step(1);
    g_in[0].buttons = TB_BTN_MELEE;
    step(1);
    TEST_ASSERT_EQUAL_INT32(50, tb_sim_get(&g_sim, t)->vitals.health);
}

static void test_no_weapon_in_hand(void)
{
    make_sim(TB_DIFF_COLA, 0);
    g_sim.cfg.body_replaced = true;
    tb_sim_add_player(&g_sim, 0);
    g_in[0].buttons = TB_BTN_FIRE;
    step(3); /* nothing to fire with: no crash, no projectile */
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        TEST_ASSERT_EQUAL_INT(TB_ENT_NONE, g_sim.entities[i].type);
    }
}

/* ---- pickups ---- */

static void test_pickups(void)
{
    tb_vec3 here = tb_v3(0, 0, 0.5f);
    tb_handle full = tb_sim_add_pickup(&g_sim, TB_PICKUP_HEALTH, here, 0, 25, false);
    step(1);
    TEST_ASSERT_NOT_NULL(tb_sim_get(&g_sim, full)); /* full health: stays */
    tb_sim_damage_player(&g_sim, 0, 30);
    step(1);
    TEST_ASSERT_NULL(tb_sim_get(&g_sim, full));
    TEST_ASSERT_EQUAL_INT32(95, P(0)->vitals.health);

    tb_sim_add_pickup(&g_sim, TB_PICKUP_HEALTH, here, 0, 50, true); /* overcharge */
    tb_sim_add_pickup(&g_sim, TB_PICKUP_ARMOUR, here, 0, 50, false);
    tb_sim_add_pickup(&g_sim, TB_PICKUP_WEAPON, here, TB_WEAPON_TEST_SHOTGUN, 8, false);
    tb_sim_add_pickup(&g_sim, TB_PICKUP_AMMO, here, TB_AMMO_ROCKETS, 3, false);
    tb_sim_add_pickup(&g_sim, TB_PICKUP_ITEM, here, TB_ITEM_JETPACK, 0, true);
    step(1);
    TEST_ASSERT_EQUAL_INT32(145, P(0)->vitals.health);
    TEST_ASSERT_EQUAL_INT32(50, P(0)->vitals.armour);
    TEST_ASSERT_TRUE(tb_inventory_has_weapon(&P(0)->inv, TB_WEAPON_TEST_SHOTGUN));
    TEST_ASSERT_EQUAL_INT32(8, P(0)->inv.ammo[TB_AMMO_SHELLS]);
    TEST_ASSERT_EQUAL_INT32(3, P(0)->inv.ammo[TB_AMMO_ROCKETS]);
    TEST_ASSERT_TRUE(P(0)->inv.items[TB_ITEM_JETPACK]);

    /* Duplicate weapon without ammo: left lying. Armour when full: left. */
    tb_handle dup = tb_sim_add_pickup(&g_sim, TB_PICKUP_WEAPON, here, TB_WEAPON_TEST_SHOTGUN, 0, false);
    P(0)->vitals.armour = TB_ARMOUR_MAX;
    tb_handle arm = tb_sim_add_pickup(&g_sim, TB_PICKUP_ARMOUR, here, 0, 10, false);
    tb_handle bad = tb_sim_add_pickup(&g_sim, TB_PICKUP_WEAPON, here, 99, 1, false);
    tb_handle weird = tb_sim_add_pickup(&g_sim, (tb_pickup_kind)42, here, 0, 0, false);
    step(1);
    TEST_ASSERT_NOT_NULL(tb_sim_get(&g_sim, dup));
    TEST_ASSERT_NOT_NULL(tb_sim_get(&g_sim, arm));
    TEST_ASSERT_NOT_NULL(tb_sim_get(&g_sim, bad));
    TEST_ASSERT_NOT_NULL(tb_sim_get(&g_sim, weird));
}

static void test_collectibles_count_for_present_players(void)
{
    make_sim(TB_DIFF_COLA, 2);
    tb_sim_add_pickup(&g_sim, TB_PICKUP_FLOPPY, tb_v3(0, 0, 0.5f), 1, 0, false);
    tb_sim_add_pickup(&g_sim, TB_PICKUP_FLOPPY, tb_v3(0, 0, 0.5f), 7, 0, false); /* invalid index */
    step(1);
    TEST_ASSERT_EQUAL_HEX8(0x2, P(0)->floppies);
    TEST_ASSERT_EQUAL_HEX8(0x2, P(1)->floppies);
    tb_sim_add_player(&g_sim, 2); /* joins late */
    tb_sim_add_pickup(&g_sim, TB_PICKUP_FLOPPY, tb_v3(0, 0, 0.5f), 0, 0, false);
    step(1);
    TEST_ASSERT_EQUAL_HEX8(0x3, P(0)->floppies);
    TEST_ASSERT_EQUAL_HEX8(0x1, P(2)->floppies);
}

/* @scenario player-core: Results screen */
static void test_results_screen(void)
{
    tb_sim_add_pickup(&g_sim, TB_PICKUP_BAUBLE, tb_v3(0, 0, 0.5f), 0, 0, false);
    tb_sim_add_pickup(&g_sim, TB_PICKUP_FLOPPY, tb_v3(0, 0, 0.5f), 0, 0, false);
    tb_sim_add_pickup(&g_sim, TB_PICKUP_FLOPPY, tb_v3(0, 0, 0.5f), 2, 0, false);
    step(1);
    char text[128];
    tb_sim_results_text(&g_sim, 0, text, sizeof(text));
    TEST_ASSERT_EQUAL_STRING("Memory Bauble: found\nFloppies 2/3\nGolden Joystick: not yet", text);
    tb_sim_complete_challenge(&g_sim);
    tb_sim_results_text(&g_sim, 0, text, sizeof(text));
    TEST_ASSERT_NOT_NULL(strstr(text, "Golden Joystick: won"));
    make_sim(TB_DIFF_COLA, 1);
    tb_sim_results_text(&g_sim, 0, text, sizeof(text));
    TEST_ASSERT_NOT_NULL(strstr(text, "Memory Bauble: missing\nFloppies 0/3"));
}

/* ---- death, checkpoints, revive ---- */

/* @scenario player-core: Death after a checkpoint */
static void test_death_after_checkpoint(void)
{
    tb_sim_add_trigger(&g_sim, TB_TRIGGER_CHECKPOINT, tb_v3(-16, -16, 0), tb_v3(-14, -14, 3), 3);
    tb_sim_add_trigger(&g_sim, TB_TRIGGER_CHECKPOINT, tb_v3(14, 14, 0), tb_v3(16, 16, 3), 1);
    tb_inventory_add_ammo(&P(0)->inv, TB_AMMO_SHELLS, 12);
    tb_sim_damage_player(&g_sim, 0, 20);
    P(0)->move.origin = tb_v3(-15, -15, 0.01f);
    step(1);
    TEST_ASSERT_EQUAL_INT32(3, P(0)->checkpoint.id);
    P(0)->move.origin = tb_v3(15, 15, 0.01f); /* older checkpoint: ignored */
    step(1);
    TEST_ASSERT_EQUAL_INT32(3, P(0)->checkpoint.id);

    P(0)->inv.ammo[TB_AMMO_SHELLS] = 1;
    tb_sim_damage_player(&g_sim, 0, 1000);
    TEST_ASSERT_EQUAL_INT(TB_PLAYER_DEAD, P(0)->state); /* solo: no revive */
    tb_sim_damage_player(&g_sim, 0, 10);                /* dead players take no damage */
    step(TB_SOLO_RESPAWN_TICKS);
    TEST_ASSERT_EQUAL_INT(TB_PLAYER_ALIVE, P(0)->state);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, -15.0f, P(0)->move.origin.x);
    TEST_ASSERT_EQUAL_INT32(80, P(0)->vitals.health);
    TEST_ASSERT_EQUAL_INT32(12, P(0)->inv.ammo[TB_AMMO_SHELLS]);
    TEST_ASSERT_EQUAL_UINT32(1, P(0)->deaths);
    tb_sim_damage_player(&g_sim, 3, 10); /* inactive slot: ignored */
}

/* @scenario player-core: Revive in time */
static void test_coop_revive_in_time(void)
{
    make_sim(TB_DIFF_COLA, 2);
    tb_sim_damage_player(&g_sim, 0, 1000);
    TEST_ASSERT_EQUAL_INT(TB_PLAYER_DOWNED, P(0)->state);
    P(1)->move.origin = tb_v3(1, 0, 0.01f);
    g_in[1].buttons = TB_BTN_USE;
    step(TB_REVIVE_HOLD_TICKS);
    TEST_ASSERT_EQUAL_INT(TB_PLAYER_ALIVE, P(0)->state);
    TEST_ASSERT_EQUAL_INT32(TB_REVIVE_HEALTH, P(0)->vitals.health);
}

static void test_coop_revive_window_expires(void)
{
    make_sim(TB_DIFF_COLA, 2);
    tb_sim_damage_player(&g_sim, 0, 1000);
    g_in[1].buttons = TB_BTN_USE; /* too far away (2 m) */
    step(TB_REVIVE_WINDOW_TICKS);
    TEST_ASSERT_EQUAL_INT(TB_PLAYER_DEAD, P(0)->state);
    step(TB_COOP_RESPAWN_TICKS);
    TEST_ASSERT_EQUAL_INT(TB_PLAYER_ALIVE, P(0)->state);
    TEST_ASSERT_EQUAL_INT32(100, P(0)->vitals.health);
}

static void test_downed_dies_when_team_is_down(void)
{
    make_sim(TB_DIFF_COLA, 2);
    tb_sim_damage_player(&g_sim, 0, 1000);
    tb_sim_damage_player(&g_sim, 1, 1000);
    TEST_ASSERT_EQUAL_INT(TB_PLAYER_DEAD, P(1)->state); /* nobody left to revive */
    step(1);
    TEST_ASSERT_EQUAL_INT(TB_PLAYER_DEAD, P(0)->state);
}

/* ---- level progress ---- */

static void test_level_completion_needs_bauble_and_everyone_at_exit(void)
{
    make_sim(TB_DIFF_COLA, 2);
    tb_sim_add_trigger(&g_sim, TB_TRIGGER_EXIT, tb_v3(14, -16, 0), tb_v3(16, -14, 3), 0);
    P(0)->move.origin = tb_v3(15, -15, 0.01f);
    step(1);
    TEST_ASSERT_FALSE(g_sim.level_complete); /* no bauble yet */
    tb_sim_add_pickup(&g_sim, TB_PICKUP_BAUBLE, tb_v3(0, -2, 0.5f), 0, 0, false);
    step(1);
    TEST_ASSERT_TRUE(g_sim.bauble_collected); /* player 2 picked it up */
    TEST_ASSERT_FALSE(g_sim.level_complete);  /* player 2 not at the exit */
    P(1)->move.origin = tb_v3(15.5f, -15, 0.01f);
    step(1);
    TEST_ASSERT_TRUE(g_sim.level_complete);
    TEST_ASSERT_TRUE(P(0)->completed && P(1)->completed);
    TEST_ASSERT_TRUE(P(0)->bauble && P(1)->bauble);
}

static void test_leave_level_and_checksum(void)
{
    tb_inventory_give_weapon(&P(0)->inv, TB_WEAPON_TEST_SHOTGUN);
    uint64_t before = tb_sim_checksum(&g_sim);
    tb_sim_leave_level(&g_sim);
    TEST_ASSERT_FALSE(tb_inventory_has_weapon(&P(0)->inv, TB_WEAPON_TEST_SHOTGUN));
    TEST_ASSERT_NOT_EQUAL(before, tb_sim_checksum(&g_sim));
    tb_sim_add_target(&g_sim, tb_v3(3, 0, 0.9f), 10);
    TEST_ASSERT_NOT_EQUAL(before, tb_sim_checksum(&g_sim));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_handles_and_stale_references);
    RUN_TEST(test_entity_table_full);
    RUN_TEST(test_players);
    RUN_TEST(test_hitscan_shotgun_kills_target);
    RUN_TEST(test_hitscan_blocked_by_wall);
    RUN_TEST(test_snowball_tap_and_charged_stun);
    RUN_TEST(test_projectile_hits_wall_and_expires);
    RUN_TEST(test_launcher_splash_hurts_targets_and_self);
    RUN_TEST(test_rocket_direct_hit);
    RUN_TEST(test_melee_button_uses_candy_cane);
    RUN_TEST(test_no_weapon_in_hand);
    RUN_TEST(test_pickups);
    RUN_TEST(test_collectibles_count_for_present_players);
    RUN_TEST(test_results_screen);
    RUN_TEST(test_death_after_checkpoint);
    RUN_TEST(test_coop_revive_in_time);
    RUN_TEST(test_coop_revive_window_expires);
    RUN_TEST(test_downed_dies_when_team_is_down);
    RUN_TEST(test_level_completion_needs_bauble_and_everyone_at_exit);
    RUN_TEST(test_leave_level_and_checksum);
    return UNITY_END();
}
