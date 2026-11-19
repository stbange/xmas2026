/* SPDX-License-Identifier: MIT */
/* Session flow: hall -> briefing -> level -> results -> hall, driven
 * through the app with player inputs (a small walking bot). */
#include "unity.h"

#include "app/app.h"

#include <math.h>
#include <string.h>

static tb_app g_app;

static tb_calendar_state calendar(tb_portal_mask open, int day)
{
    tb_calendar_state c;
    memset(&c, 0, sizeof(c));
    c.open = open;
    c.persist = open;
    c.effective_local = (tb_days_from_civil(2026, 12, day > 0 ? day : 1) - (day > 0 ? 0 : 5)) * 86400 + 36000;
    return c;
}

static void start(tb_portal_mask open, int day)
{
    tb_args a;
    memset(&a, 0, sizeof(a));
    a.headless = true;
    a.seed = 7;
    tb_app_init(&g_app, &a);
    tb_save s;
    tb_save_defaults(&s);
    tb_calendar_state c = calendar(open, day);
    tb_app_set_progress(&g_app, &s, &c);
}

void setUp(void)
{
    start(0x7Fu, 7);
}
void tearDown(void) {}

static void tick(int n, tb_input in)
{
    tb_app_set_input(&g_app, 0, &in);
    for (int i = 0; i < n; i++) {
        tb_app_frame(&g_app, 0.0, NULL);
    }
}

static tb_input idle(void)
{
    tb_input in;
    memset(&in, 0, sizeof(in));
    return in;
}

static tb_input press(uint16_t buttons)
{
    tb_input in = idle();
    in.buttons = buttons;
    return in;
}

static tb_player *me(void)
{
    return &g_app.sim.players[0];
}

/* Walks straight towards (x, y); returns true when within 0.4 m. */
static bool walk_to(float x, float y, int max_ticks)
{
    for (int t = 0; t < max_ticks; t++) {
        tb_vec3 o = me()->move.origin;
        float dx = x - o.x, dy = y - o.y;
        if (dx * dx + dy * dy < 0.16f) {
            return true;
        }
        tb_input in = idle();
        in.forward = 127;
        in.yaw = tb_angle_to_short(atan2f(dy, dx));
        tick(1, in);
    }
    return false;
}

/* @scenario festive-hall: Finding a door */
static void test_door_17_is_on_the_balcony_and_reachable(void)
{
    const tb_hall_door *d = &g_app.hall.doors[16];
    TEST_ASSERT_EQUAL_INT(17, d->day);
    TEST_ASSERT_EQUAL_FLOAT(TB_HALL_BALCONY_Z, d->centre.z);
    const float path[][2] = {{-10, -10},      {-28.5f, -9}, {-28.5f, 2},
                             {-28.5f, 13.5f}, {-20, 16},    {d->centre.x, 17.0f}};
    for (size_t i = 0; i < sizeof(path) / sizeof(path[0]); i++) {
        TEST_ASSERT_TRUE_MESSAGE(walk_to(path[i][0], path[i][1], 600), "bot got stuck");
    }
    TEST_ASSERT_FLOAT_WITHIN(0.1f, TB_HALL_BALCONY_Z, me()->move.origin.z);
    const tb_hall_door *near = tb_hall_nearest_door(&g_app.hall, me()->move.origin, 4.0f);
    TEST_ASSERT_NOT_NULL(near);
    TEST_ASSERT_EQUAL_INT(17, near->day);
}

static void test_closed_door_does_not_start_a_level(void)
{
    const tb_hall_door *d = &g_app.hall.doors[8]; /* day 9: closed */
    me()->move.origin = d->spawn;
    tb_input in = idle();
    in.forward = 127;
    in.yaw = tb_angle_to_short(-TB_PI / 2.0f); /* south, into the door */
    tick(120, in);
    TEST_ASSERT_EQUAL_INT(TB_FLOW_HALL, g_app.flow);
    TEST_ASSERT_TRUE(me()->move.origin.y > d->vol_max.y); /* blocked in front of it */
}

static void enter_door(int day)
{
    const tb_hall_door *d = &g_app.hall.doors[day - 1];
    me()->move.origin = d->spawn;
    tb_input in = idle();
    in.forward = 127;
    in.yaw = tb_angle_to_short(d->normal.y > 0 ? -TB_PI / 2.0f : TB_PI / 2.0f);
    for (int t = 0; t < 200 && g_app.flow == TB_FLOW_HALL; t++) {
        tick(1, in);
    }
}

static void finish_level(void)
{
    /* Pick up the bauble, then stand in the exit. */
    tb_vec3 bauble = tb_v3(0, 18, 3.01f), exitp = tb_v3(0, -18.5f, 0.01f);
    me()->move.origin = bauble;
    tick(2, idle());
    TEST_ASSERT_TRUE(g_app.sim.bauble_collected);
    me()->move.origin = exitp;
    tick(2, idle());
}

/* @scenario festive-hall: Return after finishing */
static void test_full_round_trip_through_a_portal(void)
{
    enter_door(5);
    TEST_ASSERT_EQUAL_INT(TB_FLOW_BRIEFING, g_app.flow);
    TEST_ASSERT_EQUAL_INT(5, g_app.level_day);
    tick(2, press(TB_BTN_USE)); /* skip the briefing */
    TEST_ASSERT_EQUAL_INT(TB_FLOW_LEVEL, g_app.flow);
    TEST_ASSERT_TRUE(g_app.world_changed);

    /* A floppy on the way. */
    me()->move.origin = tb_v3(6.5f, 0, 0.41f);
    tick(2, idle());
    finish_level();
    TEST_ASSERT_EQUAL_INT(TB_FLOW_RESULTS, g_app.flow);
    TEST_ASSERT_TRUE(g_app.save_dirty);
    TEST_ASSERT_TRUE(g_app.save.completed & (1u << 4));
    TEST_ASSERT_EQUAL_HEX8(0x1, g_app.save.floppies[4]);
    TEST_ASSERT_TRUE(g_app.save.best_time_ms[4] > 0);
    TEST_ASSERT_NOT_NULL(strstr(g_app.results, "DAY 5 - Tides of Eggnog"));
    TEST_ASSERT_NOT_NULL(strstr(g_app.results, "Floppies 1/3"));

    tick(2, press(TB_BTN_FIRE));
    TEST_ASSERT_EQUAL_INT(TB_FLOW_HALL, g_app.flow);
    const tb_hall_door *d = &g_app.hall.doors[4];
    TEST_ASSERT_FLOAT_WITHIN(0.5f, d->spawn.x, me()->move.origin.x);
    TEST_ASSERT_FLOAT_WITHIN(0.5f, d->spawn.y, me()->move.origin.y);
    TEST_ASSERT_EQUAL_INT(1, tb_hall_tree_baubles(g_app.save.completed));
    TEST_ASSERT_NOT_NULL(strstr(g_app.message, "Bauble 5"));
    TEST_ASSERT_TRUE(g_app.message_ticks > 0);
}

static void test_briefing_and_results_continue_by_themselves(void)
{
    enter_door(2);
    tick((int)TB_BRIEFING_TICKS, idle());
    TEST_ASSERT_EQUAL_INT(TB_FLOW_LEVEL, g_app.flow);
    g_app.sim.challenge_done = true;
    finish_level();
    TEST_ASSERT_TRUE(g_app.save.challenges & (1u << 1));
    tick((int)TB_RESULTS_TICKS, idle());
    TEST_ASSERT_EQUAL_INT(TB_FLOW_HALL, g_app.flow);
    tick((int)TB_MESSAGE_TICKS, idle());
    TEST_ASSERT_EQUAL_UINT32(0, g_app.message_ticks);
}

static void test_replaying_keeps_best_time(void)
{
    enter_door(3);
    tick(2, press(TB_BTN_JUMP));
    tick(600, idle()); /* slow run: 10 s */
    finish_level();
    uint32_t slow = g_app.save.best_time_ms[2];
    tick(2, press(TB_BTN_USE));
    enter_door(3);
    tick(2, press(TB_BTN_USE));
    finish_level();
    TEST_ASSERT_TRUE(g_app.save.best_time_ms[2] < slow);
    TEST_ASSERT_NOT_NULL(strstr(g_app.results, "(replay)"));
    tick(2, press(TB_BTN_USE));
    enter_door(3);
    tick(2, press(TB_BTN_USE));
    tick(900, idle()); /* slower than the best */
    uint32_t best = g_app.save.best_time_ms[2];
    finish_level();
    TEST_ASSERT_EQUAL_UINT32(best, g_app.save.best_time_ms[2]);
}

static void test_doors_open_while_in_the_hall(void)
{
    start(0x1u, 1);
    me()->move.origin = tb_v3(3, 3, 0.01f);
    tb_save s = g_app.save;
    tb_calendar_state c = calendar(0x3u, 2); /* midnight: door 2 opens */
    tb_app_set_progress(&g_app, &s, &c);
    TEST_ASSERT_TRUE(tb_portal_is_open(g_app.open, 2));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 3.0f, me()->move.origin.x); /* player stays put */
    TEST_ASSERT_EQUAL_INT(2, g_app.calendar_day);
    tb_app_set_progress(&g_app, &s, &c); /* unchanged: no rebuild */
}

static void test_calendar_change_during_a_level_waits(void)
{
    enter_door(1);
    tick(2, press(TB_BTN_USE));
    tb_save s = g_app.save;
    tb_calendar_state c = calendar(0xFFu, 8);
    tb_app_set_progress(&g_app, &s, &c);
    TEST_ASSERT_EQUAL_INT(TB_FLOW_LEVEL, g_app.flow); /* the level keeps running */
    TEST_ASSERT_EQUAL_HEX32(0xFFu, g_app.open);
}

static void test_quit_and_checksum(void)
{
    uint64_t c1 = tb_app_checksum(&g_app);
    tick(1, idle());
    TEST_ASSERT_NOT_EQUAL(c1, tb_app_checksum(&g_app));
    tb_app_request_quit(&g_app);
    TEST_ASSERT_EQUAL_INT(TB_APP_DONE, tb_app_frame(&g_app, 0.0, NULL));
    tb_app_set_input(&g_app, -1, NULL); /* ignored */
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_door_17_is_on_the_balcony_and_reachable);
    RUN_TEST(test_closed_door_does_not_start_a_level);
    RUN_TEST(test_full_round_trip_through_a_portal);
    RUN_TEST(test_briefing_and_results_continue_by_themselves);
    RUN_TEST(test_replaying_keeps_best_time);
    RUN_TEST(test_doors_open_while_in_the_hall);
    RUN_TEST(test_calendar_change_during_a_level_waits);
    RUN_TEST(test_quit_and_checksum);
    return UNITY_END();
}
