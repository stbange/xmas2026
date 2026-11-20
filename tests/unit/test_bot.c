/* SPDX-License-Identifier: MIT */
/* The golden-path test bot, step by step, in the test arena. */
#include "unity.h"

#include "game/bot.h"

#include <string.h>

static tb_brush g_store[TB_TEST_ARENA_BRUSHES];
static tb_geometry g_geo;
static tb_sim g_sim;
static tb_bot g_bot;

void setUp(void)
{
    tb_geometry_build_test_arena(&g_geo, g_store);
    tb_sim_config cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.geometry = &g_geo;
    cfg.seed = 5;
    cfg.difficulty = TB_DIFF_PIZZA;
    cfg.spawn[0] = tb_v3(0, 0, 0.01f);
    tb_sim_init(&g_sim, &cfg);
    tb_sim_add_player(&g_sim, 0);
}
void tearDown(void) {}

/* Runs until the bot is done or failed, at most `ticks`; returns ticks used. */
static uint32_t run(const tb_bot_step *steps, int count, uint32_t ticks)
{
    tb_bot_init(&g_bot, steps, count);
    tb_input in[TB_MAX_PLAYERS];
    memset(in, 0, sizeof(in));
    uint32_t t = 0;
    while (t < ticks && !g_bot.done && !g_bot.failed) {
        in[0] = tb_bot_think(&g_bot, &g_sim, 0);
        tb_sim_step(&g_sim, in);
        t++;
    }
    return t;
}

static void test_goto_wait_and_select(void)
{
    tb_inventory_give_weapon(&g_sim.players[0].inv, TB_WEAPON_TEST_SHOTGUN);
    const tb_bot_step steps[] = {
        {TB_BOT_GOTO, {-8, 2, 0}, 0},
        {TB_BOT_WAIT, {0, 0, 0}, 30},
        {TB_BOT_SELECT, {0, 0, 0}, 3},
    };
    run(steps, 3, 2000);
    TEST_ASSERT_TRUE(g_bot.done);
    TEST_ASSERT_FALSE(g_bot.failed);
    TEST_ASSERT_FLOAT_WITHIN(0.6f, -8.0f, g_sim.players[0].move.origin.x);
    tb_input in[TB_MAX_PLAYERS] = {{0}};
    in[0] = tb_bot_think(&g_bot, &g_sim, 0); /* done: idles */
    TEST_ASSERT_EQUAL_INT8(0, in[0].forward);
    tb_sim_step(&g_sim, in);
    TEST_ASSERT_EQUAL_INT(TB_WEAPON_TEST_SHOTGUN, tb_inventory_current(&g_sim.players[0].inv));
}

static void test_use_presses_once(void)
{
    tb_sim_add_usable(&g_sim, TB_USE_VENDING, tb_v3(1.2f, -0.5f, 1.2f), tb_v3(1.6f, 0.5f, 2), "cola", NULL,
                      0);
    g_sim.players[0].vitals.health = 50;
    const tb_bot_step steps[] = {{TB_BOT_USE, {1.4f, 0, 1.6f}, 0}};
    run(steps, 1, 100);
    TEST_ASSERT_TRUE(g_bot.done);
    TEST_ASSERT_EQUAL_INT(55, g_sim.players[0].vitals.health);
}

static void test_clear_fights_until_the_area_is_empty(void)
{
    tb_inventory_give_weapon(&g_sim.players[0].inv, TB_WEAPON_TEST_SHOTGUN);
    tb_inventory_add_ammo(&g_sim.players[0].inv, TB_AMMO_SHELLS, 50);
    tb_inventory_select(&g_sim.players[0].inv, 3);
    tb_sim_add_enemy(&g_sim, TB_ENEMY_LIZARD, tb_v3(14, -14, 0), NULL);  /* out of sight at first */
    tb_sim_add_enemy(&g_sim, TB_ENEMY_TROOPER, tb_v3(-15, 15, 0), NULL); /* far from the area */
    const tb_bot_step steps[] = {{TB_BOT_CLEAR, {12, -12, 0}, 3000}};
    run(steps, 1, 4000);
    TEST_ASSERT_TRUE_MESSAGE(g_bot.done, g_bot.reason);
    TEST_ASSERT_EQUAL_INT(1, tb_sim_enemy_count(&g_sim)); /* the far one is left alone */
}

static void test_clear_times_out(void)
{
    tb_sim_add_enemy(&g_sim, TB_ENEMY_BATTLELORD, tb_v3(10, 10, 0), NULL);
    const tb_bot_step steps[] = {{TB_BOT_CLEAR, {10, 10, 0}, 20}};
    run(steps, 1, 100);
    TEST_ASSERT_TRUE(g_bot.failed);
    TEST_ASSERT_EQUAL_STRING("step 0: timed out", g_bot.reason);
    tb_input in = tb_bot_think(&g_bot, &g_sim, 0); /* failed: idles */
    TEST_ASSERT_EQUAL_UINT16(0, in.buttons);
}

static void test_jet_to_climbs_then_flies(void)
{
    tb_sim_add_pickup(&g_sim, TB_PICKUP_ITEM, tb_v3(0, 0, 0.5f), TB_ITEM_JETPACK, 0, true);
    /* Onto the ramp-top platform (z 3) from the floor. */
    const tb_bot_step steps[] = {{TB_BOT_GOTO, {0, 7, 0}, 0}, {TB_BOT_JET_TO, {0, 18, 3}, 0}};
    run(steps, 2, 3000);
    TEST_ASSERT_TRUE_MESSAGE(g_bot.done, g_bot.reason);
    TEST_ASSERT_TRUE(g_sim.players[0].move.origin.z > 2.9f);
}

static void test_resumes_at_checkpoint_after_dying(void)
{
    const tb_bot_step steps[] = {
        {TB_BOT_GOTO, {2, 0, 0}, 0},
        {TB_BOT_CHECKPOINT, {0, 0, 0}, 0},
        {TB_BOT_WAIT, {0, 0, 0}, 50},
        {TB_BOT_GOTO, {4, 0, 0}, 0},
    };
    tb_bot_init(&g_bot, steps, 4);
    tb_input in[TB_MAX_PLAYERS];
    memset(in, 0, sizeof(in));
    for (int t = 0; t < 200 && g_bot.index < 2; t++) {
        in[0] = tb_bot_think(&g_bot, &g_sim, 0);
        tb_sim_step(&g_sim, in);
    }
    TEST_ASSERT_EQUAL_INT(2, g_bot.index);
    TEST_ASSERT_EQUAL_INT(1, g_bot.resume);
    tb_sim_damage_player(&g_sim, 0, 1000);
    in[0] = tb_bot_think(&g_bot, &g_sim, 0); /* dead: back to the checkpoint step, idle */
    TEST_ASSERT_EQUAL_INT(1, g_bot.index);
    TEST_ASSERT_EQUAL_INT8(0, in[0].forward);
}

static void test_goto_times_out_when_blocked(void)
{
    const tb_bot_step wall[] = {{TB_BOT_GOTO, {0, 30, 0}, 0}}; /* outside the arena */
    run(wall, 1, TB_BOT_STEP_TIMEOUT + 10);
    TEST_ASSERT_TRUE(g_bot.failed);
}

static void test_gives_up_after_too_many_deaths(void)
{
    const tb_bot_step steps[] = {{TB_BOT_WAIT, {0, 0, 0}, 100000}};
    tb_bot_init(&g_bot, steps, 1);
    g_sim.players[0].deaths = TB_BOT_MAX_DEATHS;
    tb_bot_think(&g_bot, &g_sim, 0);
    TEST_ASSERT_TRUE(g_bot.failed);
    TEST_ASSERT_EQUAL_STRING("step 0: died too often", g_bot.reason);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_goto_wait_and_select);
    RUN_TEST(test_use_presses_once);
    RUN_TEST(test_clear_fights_until_the_area_is_empty);
    RUN_TEST(test_clear_times_out);
    RUN_TEST(test_jet_to_climbs_then_flies);
    RUN_TEST(test_resumes_at_checkpoint_after_dying);
    RUN_TEST(test_goto_times_out_when_blocked);
    RUN_TEST(test_gives_up_after_too_many_deaths);
    return UNITY_END();
}
