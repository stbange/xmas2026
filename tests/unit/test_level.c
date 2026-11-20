/* SPDX-License-Identifier: MIT */
/* The level framework (manifests, registry, building) and day 1 as a
 * whole: its golden path is an automated game test. */
#include "unity.h"

#include "game/level.h"

#include <stdlib.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

static tb_level_manifest valid(void)
{
    tb_level_manifest m;
    memset(&m, 0, sizeof(m));
    m.day = 5;
    m.mechanics[0] = "a";
    m.mechanics[1] = "b";
    m.mechanic_count = 2;
    m.loaners[0] = "x";
    m.loaner_count = 1;
    m.trophy = "Trophy";
    m.palette = "p";
    m.music = "m";
    m.minutes = 20;
    return m;
}

static void expect_invalid(const tb_level_manifest *m, const char *field)
{
    char err[128];
    TEST_ASSERT_FALSE(tb_level_manifest_validate(m, err, sizeof(err)));
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(err, field), err);
}

/* @scenario level-framework: Missing manifest field */
static void test_manifest_missing_trophy_names_level_and_field(void)
{
    tb_level_manifest m = valid();
    char err[128];
    TEST_ASSERT_TRUE(tb_level_manifest_validate(&m, err, sizeof(err)));
    m.trophy = NULL;
    expect_invalid(&m, "level 5: trophy");
    m.trophy = "";
    expect_invalid(&m, "trophy");
    m = valid();
    m.palette = NULL;
    expect_invalid(&m, "palette");
    m = valid();
    m.music = NULL;
    expect_invalid(&m, "music");
    m = valid();
    m.loaner_count = 0;
    expect_invalid(&m, "loaners");
    m = valid();
    m.day = 0;
    expect_invalid(&m, "day");
    m.day = 25;
    expect_invalid(&m, "day");
    m = valid();
    m.minutes = 5;
    expect_invalid(&m, "minutes");
    m.minutes = 45;
    expect_invalid(&m, "minutes");
}

/* @scenario level-framework: Mechanic count */
static void test_manifest_needs_two_or_three_mechanics(void)
{
    tb_level_manifest m = valid();
    m.mechanic_count = 1;
    expect_invalid(&m, "mechanics");
    m.mechanic_count = 4;
    expect_invalid(&m, "mechanics");
    m.mechanic_count = 3;
    char err[64];
    TEST_ASSERT_TRUE(tb_level_manifest_validate(&m, err, sizeof(err)));
}

/* @scenario player-core: Level validation */
static void test_manifest_at_most_two_extra_bars(void)
{
    tb_level_manifest m = valid();
    m.bar_count = 3;
    expect_invalid(&m, "bars");
}

/* @scenario level-framework: Placeholder course */
static void test_registry_and_placeholders(void)
{
    TEST_ASSERT_NULL(tb_level_def_get(0));
    TEST_ASSERT_NULL(tb_level_def_get(25));
    const tb_level_def *d1 = tb_level_def_get(1);
    TEST_ASSERT_FALSE(d1->placeholder);
    TEST_ASSERT_EQUAL_PTR(d1, tb_level_day01());
    for (int day = 2; day <= 24; day++) {
        const tb_level_def *d = tb_level_def_get(day);
        TEST_ASSERT_TRUE(d->placeholder);
        TEST_ASSERT_EQUAL_INT(day, d->manifest.day);
        TEST_ASSERT_EQUAL_PTR(d, tb_level_def_get(day)); /* built once */
        TEST_ASSERT_NULL(d->script);
    }
    /* A placeholder is the test arena with a bauble, floppies and an exit. */
    tb_brush *store = calloc(TB_LEVEL_MAX_BRUSHES, sizeof(tb_brush));
    tb_geometry g;
    const tb_level_def *d = tb_level_def_get(9);
    TEST_ASSERT_TRUE(tb_level_build(d, &g, store));
    TEST_ASSERT_EQUAL_INT(TB_TEST_ARENA_BRUSHES, g.count);
    TEST_ASSERT_EQUAL_INT(TB_LEVEL_MAX_BRUSHES, g.cap);
    static tb_sim sim;
    tb_sim_config cfg;
    tb_level_sim_config(d, &g, 1, TB_DIFF_COLA, &cfg);
    tb_sim_init(&sim, &cfg);
    tb_sim_add_player(&sim, 0);
    char err[64];
    TEST_ASSERT_NULL(tb_level_begin(d, &sim, err, sizeof(err)));
    TEST_ASSERT_EQUAL_STRING("", err);
    int bauble = 0, floppies = 0;
    for (int i = 0; i < TB_MAX_ENTITIES; i++) {
        bauble += sim.entities[i].type == TB_ENT_PICKUP && sim.entities[i].kind == TB_PICKUP_BAUBLE;
        floppies += sim.entities[i].type == TB_ENT_PICKUP && sim.entities[i].kind == TB_PICKUP_FLOPPY;
    }
    TEST_ASSERT_EQUAL_INT(1, bauble);
    TEST_ASSERT_EQUAL_INT(3, floppies);
    free(store);
}

static void test_day01_definition(void)
{
    const tb_level_def *d = tb_level_day01();
    char err[128];
    TEST_ASSERT_TRUE_MESSAGE(tb_level_manifest_validate(&d->manifest, err, sizeof(err)), err);
    TEST_ASSERT_EQUAL_STRING("Gift-Shrinker", d->manifest.trophy);
    TEST_ASSERT_EQUAL_INT(3, d->manifest.mechanic_count);
    TEST_ASSERT_TRUE(d->light_count <= TB_LEVEL_MAX_LIGHTS);
    TEST_ASSERT_NOT_NULL(d->script);
    TEST_ASSERT_TRUE(d->script_len > 100);
    TEST_ASSERT_EQUAL_size_t(strlen(d->script), d->script_len);
    TEST_ASSERT_TRUE(d->golden_count > 10);
    for (int i = 0; i < d->light_count; i++) {
        TEST_ASSERT_TRUE(d->lights[i].group >= 0 && d->lights[i].group < TB_LIGHT_GROUPS);
    }
    tb_brush *store = calloc(TB_LEVEL_MAX_BRUSHES, sizeof(tb_brush));
    tb_geometry g;
    TEST_ASSERT_TRUE(tb_level_build(d, &g, store));
    TEST_ASSERT_TRUE(g.count > 50);
    /* Too little storage: the build reports it. */
    tb_geometry small;
    tb_geometry_init(&small, store, 8);
    TEST_ASSERT_FALSE(d->build_geometry(&small));
    free(store);
}

static void test_spawn_points_line_up_beside_the_view(void)
{
    tb_level_def d = *tb_level_day01();
    tb_sim_config cfg;
    tb_geometry g = {0};
    d.spawn_yaw = 0.0f; /* looking +x: players line up along y */
    tb_level_sim_config(&d, &g, 3, TB_DIFF_PIZZA, &cfg);
    TEST_ASSERT_EQUAL_FLOAT(d.spawn.x, cfg.spawn[2].x);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, d.spawn.y + 2.4f, cfg.spawn[2].y);
    TEST_ASSERT_EQUAL_UINT64(3, cfg.seed);
    TEST_ASSERT_EQUAL_INT(TB_DIFF_PIZZA, cfg.difficulty);
}

static void test_broken_script_is_reported(void)
{
    tb_level_def d = *tb_level_day01();
    d.script = "function on_start() error('kaputt') end";
    d.script_len = strlen(d.script);
    tb_golden_result r;
    TEST_ASSERT_FALSE(tb_level_run_golden(&d, 1, TB_DIFF_COLA, &r));
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(r.reason, "kaputt"), r.reason);
    d.script = "syntax error here";
    d.script_len = strlen(d.script);
    TEST_ASSERT_FALSE(tb_level_run_golden(&d, 1, TB_DIFF_COLA, &r));
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(r.reason, "script"), r.reason);
}

static bool no_room(tb_geometry *g)
{
    (void)g;
    return false;
}

static void test_golden_run_reports_failures(void)
{
    tb_level_def d = *tb_level_day01();
    static const tb_bot_step lost[] = {{TB_BOT_GOTO, {3, 0, 4}, 0}, {TB_BOT_GOTO, {100, 50, 0}, 0}};
    d.golden_path = lost;
    d.golden_count = 2;
    tb_golden_result r;
    TEST_ASSERT_FALSE(tb_level_run_golden(&d, 1, TB_DIFF_COLA, &r));
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(r.reason, "timed out"), r.reason);
    TEST_ASSERT_EQUAL_INT(1, r.step);
    d.golden_ticks = 30; /* out of time before the bot gives up */
    TEST_ASSERT_FALSE(tb_level_run_golden(&d, 1, TB_DIFF_COLA, &r));
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(r.reason, "out of time"), r.reason);
    d = *tb_level_day01();
    d.build_geometry = no_room;
    TEST_ASSERT_FALSE(tb_level_run_golden(&d, 1, TB_DIFF_COLA, &r));
    TEST_ASSERT_EQUAL_STRING("geometry does not fit", r.reason);
}

/* @scenario homage-levels: Baublegum City golden path */
static void test_day01_golden_path_finds_everything(void)
{
    const tb_level_def *d = tb_level_day01();
    tb_golden_result r;
    for (uint64_t seed = 1; seed <= 3; seed++) {
        TEST_ASSERT_TRUE_MESSAGE(tb_level_run_golden(d, seed, TB_DIFF_COLA, &r), r.reason);
        TEST_ASSERT_TRUE(r.bauble);
        TEST_ASSERT_EQUAL_HEX8(0x7, r.floppies);
        TEST_ASSERT_TRUE(r.ticks < d->golden_ticks);
    }
    /* Deterministic: the same seed gives the same run. */
    tb_golden_result again;
    TEST_ASSERT_TRUE(tb_level_run_golden(d, 3, TB_DIFF_COLA, &again));
    TEST_ASSERT_EQUAL_UINT32(r.ticks, again.ticks);
    TEST_ASSERT_EQUAL_UINT64(r.checksum, again.checksum);
}

/* @scenario homage-levels: Interior Decorator */
static void test_day01_interior_decorator_challenge(void)
{
    tb_golden_result r;
    TEST_ASSERT_TRUE_MESSAGE(tb_level_run_golden(tb_level_day01(), 1, TB_DIFF_COLA, &r), r.reason);
    TEST_ASSERT_TRUE(r.challenge);
}

static void test_day01_is_beatable_on_every_difficulty(void)
{
    /* Nightmare is allowed to beat the bot sometimes; seed 1 is a known win. */
    tb_golden_result r;
    for (int diff = TB_DIFF_PIZZA; diff <= TB_DIFF_NIGHTMARE; diff++) {
        TEST_ASSERT_TRUE_MESSAGE(tb_level_run_golden(tb_level_day01(), 1, (tb_difficulty)diff, &r), r.reason);
    }
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_manifest_missing_trophy_names_level_and_field);
    RUN_TEST(test_manifest_needs_two_or_three_mechanics);
    RUN_TEST(test_manifest_at_most_two_extra_bars);
    RUN_TEST(test_registry_and_placeholders);
    RUN_TEST(test_day01_definition);
    RUN_TEST(test_spawn_points_line_up_beside_the_view);
    RUN_TEST(test_broken_script_is_reported);
    RUN_TEST(test_golden_run_reports_failures);
    RUN_TEST(test_day01_golden_path_finds_everything);
    RUN_TEST(test_day01_interior_decorator_challenge);
    RUN_TEST(test_day01_is_beatable_on_every_difficulty);
    return UNITY_END();
}
