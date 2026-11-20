/* SPDX-License-Identifier: MIT */
/* The sandboxed Lua level-script host. */
#include "unity.h"

#include "game/script.h"

#include <string.h>

static tb_brush g_store[TB_TEST_ARENA_BRUSHES];
static tb_geometry g_geo;
static tb_sim g_sim;
static tb_script *g_s;
static char g_err[256];

void setUp(void)
{
    tb_geometry_build_test_arena(&g_geo, g_store);
    tb_sim_config cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.geometry = &g_geo;
    cfg.seed = 11;
    tb_sim_init(&g_sim, &cfg);
    tb_sim_add_player(&g_sim, 0);
    g_s = NULL;
    g_err[0] = '\0';
}

void tearDown(void)
{
    tb_script_destroy(g_s);
}

static tb_script *load(const char *src)
{
    g_s = tb_script_create(src, strlen(src), "=test", &g_sim, g_err, sizeof(g_err));
    return g_s;
}

/* @scenario level-framework: Sandboxed script */
static void test_sandbox_has_no_io_os_or_loaders(void)
{
    TEST_ASSERT_NOT_NULL(load("blocked = 0\n"
                              "for _, n in ipairs({'io', 'os', 'package', 'debug', 'load', 'loadfile',\n"
                              "                    'dofile', 'require', 'collectgarbage'}) do\n"
                              "  if _ENV[n] == nil then blocked = blocked + 1 end\n"
                              "end\n"
                              "has_string = string.format('%d', 3) == '3' and 1 or 0\n"
                              "has_table = table.concat({1, 2}, ',') == '1,2' and 1 or 0\n"
                              "no_seed = math.randomseed == nil and 1 or 0\n"));
    TEST_ASSERT_EQUAL_DOUBLE(9.0, tb_script_global_number(g_s, "blocked", -1));
    TEST_ASSERT_EQUAL_DOUBLE(1.0, tb_script_global_number(g_s, "has_string", -1));
    TEST_ASSERT_EQUAL_DOUBLE(1.0, tb_script_global_number(g_s, "has_table", -1));
    TEST_ASSERT_EQUAL_DOUBLE(1.0, tb_script_global_number(g_s, "no_seed", -1));
    TEST_ASSERT_EQUAL_DOUBLE(-1.0, tb_script_global_number(g_s, "missing", -1));
}

static void test_binary_chunks_are_rejected(void)
{
    const char bin[] = "\x1bLua";
    g_s = tb_script_create(bin, sizeof(bin) - 1, "=bin", &g_sim, g_err, sizeof(g_err));
    TEST_ASSERT_NULL(g_s);
    TEST_ASSERT_NOT_NULL(strstr(g_err, "binary"));
}

/* @scenario level-framework: Script error */
static void test_errors_are_reported_and_silence_the_script(void)
{
    TEST_ASSERT_NULL(load("this is not lua"));
    TEST_ASSERT_NOT_NULL(strstr(g_err, "test"));
    TEST_ASSERT_NULL(load("error('boom at load')"));
    TEST_ASSERT_NOT_NULL(strstr(g_err, "boom at load"));
    TEST_ASSERT_NULL(load("error({})"));
    TEST_ASSERT_EQUAL_STRING("error object is not a string", g_err);

    TEST_ASSERT_NOT_NULL(load("function on_start() error('boom') end\n"
                              "function on_used() calls = (calls or 0) + 1 end"));
    TEST_ASSERT_FALSE(tb_script_start(g_s));
    TEST_ASSERT_TRUE(tb_script_failed(g_s));
    TEST_ASSERT_NOT_NULL(strstr(tb_script_error(g_s), "boom"));
    TEST_ASSERT_FALSE(tb_script_start(g_s));
    tb_sim_push_event(&g_sim, TB_EVENT_USED, 0, 1, "x");
    TEST_ASSERT_FALSE(tb_script_tick(g_s));
    TEST_ASSERT_EQUAL_DOUBLE(-1.0, tb_script_global_number(g_s, "calls", -1)); /* stays silent */
}

/* @scenario level-framework: Runaway script */
static void test_instruction_budget_and_memory_cap(void)
{
    TEST_ASSERT_NOT_NULL(load("function on_start() while true do end end"));
    TEST_ASSERT_FALSE(tb_script_start(g_s));
    TEST_ASSERT_NOT_NULL(strstr(tb_script_error(g_s), "instruction budget"));
    tb_script_destroy(g_s);
    TEST_ASSERT_NOT_NULL(
        load("function on_start() local t = {} for i = 1, 1e7 do t[i] = ('x'):rep(64) .. i end end"));
    TEST_ASSERT_FALSE(tb_script_start(g_s));
    TEST_ASSERT_TRUE(strstr(tb_script_error(g_s), "memory") || strstr(tb_script_error(g_s), "budget"));
}

static void test_callbacks_receive_events(void)
{
    TEST_ASSERT_NOT_NULL(
        load("t, u, k, p = 0, 0, 0, 0\n"
             "function on_trigger(n, v, pl) if n == 'gate' and pl == 0 then t = t + 1 end end\n"
             "function on_used(n, kind) if n == 'cab' then u = kind end end\n"
             "function on_killed(n, kind) if n == 'liz' then k = kind + 10 end end\n"
             "function on_pickup(n, kind) p = kind end\n"));
    TEST_ASSERT_TRUE(tb_script_start(g_s)); /* no on_start: fine */
    tb_sim_push_event(&g_sim, TB_EVENT_TRIGGER, 0, 0, "gate");
    tb_sim_push_event(&g_sim, TB_EVENT_USED, 0, TB_USE_ARCADE, "cab");
    tb_sim_push_event(&g_sim, TB_EVENT_KILLED, -1, TB_ENEMY_LIZARD, "liz");
    tb_sim_push_event(&g_sim, TB_EVENT_PICKUP, 0, TB_PICKUP_FLOPPY, "");
    TEST_ASSERT_TRUE(tb_script_tick(g_s));
    TEST_ASSERT_EQUAL_DOUBLE(1, tb_script_global_number(g_s, "t", -1));
    TEST_ASSERT_EQUAL_DOUBLE(TB_USE_ARCADE, tb_script_global_number(g_s, "u", -1));
    TEST_ASSERT_EQUAL_DOUBLE(TB_ENEMY_LIZARD + 10, tb_script_global_number(g_s, "k", -1));
    TEST_ASSERT_EQUAL_DOUBLE(TB_PICKUP_FLOPPY, tb_script_global_number(g_s, "p", -1));
    tb_script_destroy(g_s);
    /* A script without callbacks ignores events. */
    TEST_ASSERT_NOT_NULL(load("x = 1"));
    TEST_ASSERT_TRUE(tb_script_tick(g_s));
}

static void test_event_arguments(void)
{
    TEST_ASSERT_NOT_NULL(load("function on_used(n, k, p) name = n; kind = k; player = p end"));
    tb_sim_push_event(&g_sim, TB_EVENT_USED, 2, TB_USE_VENDING, "cola");
    tb_script_tick(g_s);
    TEST_ASSERT_EQUAL_DOUBLE(TB_USE_VENDING, tb_script_global_number(g_s, "kind", -1));
    TEST_ASSERT_EQUAL_DOUBLE(2, tb_script_global_number(g_s, "player", -1));
}

static void test_tb_api_acts_on_the_simulation(void)
{
    int base = tb_sim_enemy_count(&g_sim);
    TEST_ASSERT_NOT_NULL(load("function on_start()\n"
                              "  tb.say('hello')\n"
                              "  tb.objective('DO IT')\n"
                              "  ok_enemy = tb.spawn_enemy('trooper', 5, 5, 0, 'grunt') and 1 or 0\n"
                              "  tb.spawn_enemy('brain', -5, 5, 2)\n"
                              "  ok_pickup = tb.spawn_pickup('floppy', 1, 1, 0.5, 2) and 1 or 0\n"
                              "  tb.spawn_pickup('weapon', 2, 1, 0.5, 6, 8, true)\n"
                              "  tb.shake(30); tb.shake(10)\n"
                              "  tb.light(4, false)\n"
                              "  fired = tb.fire('grunt')\n"
                              "  enemies = tb.enemies()\n"
                              "  now = tb.tick()\n"
                              "  print('script says hi')\n"
                              "  tb.challenge_done()\n"
                              "end\n"));
    TEST_ASSERT_TRUE(tb_script_start(g_s));
    TEST_ASSERT_EQUAL_STRING("hello", tb_script_take_say(g_s));
    TEST_ASSERT_NULL(tb_script_take_say(g_s));
    TEST_ASSERT_EQUAL_STRING("DO IT", tb_script_objective(g_s));
    TEST_ASSERT_EQUAL_DOUBLE(1, tb_script_global_number(g_s, "ok_enemy", 0));
    TEST_ASSERT_EQUAL_DOUBLE(1, tb_script_global_number(g_s, "ok_pickup", 0));
    TEST_ASSERT_EQUAL_DOUBLE(1, tb_script_global_number(g_s, "fired", 0));
    TEST_ASSERT_EQUAL_DOUBLE(base + 2, tb_script_global_number(g_s, "enemies", 0));
    TEST_ASSERT_EQUAL_DOUBLE(0, tb_script_global_number(g_s, "now", -1));
    TEST_ASSERT_EQUAL_INT(base + 2, tb_sim_enemy_count(&g_sim));
    TEST_ASSERT_EQUAL_UINT32(30, g_sim.shake_ticks);
    TEST_ASSERT_FALSE(tb_sim_light_on(&g_sim, 4));
    TEST_ASSERT_TRUE(g_sim.challenge_done);
    tb_entity *grunt = tb_sim_get(&g_sim, tb_sim_find(&g_sim, "grunt"));
    TEST_ASSERT_EQUAL_UINT8(TB_AI_CHASE, grunt->ai_state); /* fired = woken up */
}

static void test_api_argument_errors(void)
{
    static const char *const bad[] = {
        "tb.spawn_enemy('dragon', 0, 0, 0)",
        "tb.spawn_pickup('cake', 0, 0, 0)",
        "tb.light(99, true)",
        "tb.say()",
        "math.random(5, 1)",
    };
    for (size_t i = 0; i < sizeof(bad) / sizeof(bad[0]); i++) {
        TEST_ASSERT_NULL_MESSAGE(load(bad[i]), bad[i]);
    }
}

static void test_random_comes_from_the_simulation(void)
{
    const char *src = "a = math.random(); b = math.random(6); c = math.random(10, 12); d = math.random(3, 3)";
    TEST_ASSERT_NOT_NULL(load(src));
    double a = tb_script_global_number(g_s, "a", -1), b = tb_script_global_number(g_s, "b", -1);
    double c = tb_script_global_number(g_s, "c", -1);
    TEST_ASSERT_TRUE(a >= 0.0 && a < 1.0);
    TEST_ASSERT_TRUE(b >= 1 && b <= 6);
    TEST_ASSERT_TRUE(c >= 10 && c <= 12);
    TEST_ASSERT_EQUAL_DOUBLE(3, tb_script_global_number(g_s, "d", -1));
    tb_script_destroy(g_s);
    /* Same simulation seed and state: same numbers. */
    setUp();
    TEST_ASSERT_NOT_NULL(load(src));
    TEST_ASSERT_EQUAL_DOUBLE(a, tb_script_global_number(g_s, "a", -1));
    TEST_ASSERT_EQUAL_DOUBLE(b, tb_script_global_number(g_s, "b", -1));
}

static void test_shake_ignores_non_positive(void)
{
    TEST_ASSERT_NOT_NULL(load("tb.shake(-5); tb.shake(0)"));
    TEST_ASSERT_EQUAL_UINT32(0, g_sim.shake_ticks);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_sandbox_has_no_io_os_or_loaders);
    RUN_TEST(test_binary_chunks_are_rejected);
    RUN_TEST(test_errors_are_reported_and_silence_the_script);
    RUN_TEST(test_instruction_budget_and_memory_cap);
    RUN_TEST(test_callbacks_receive_events);
    RUN_TEST(test_event_arguments);
    RUN_TEST(test_tb_api_acts_on_the_simulation);
    RUN_TEST(test_api_argument_errors);
    RUN_TEST(test_random_comes_from_the_simulation);
    RUN_TEST(test_shake_ignores_non_positive);
    return UNITY_END();
}
