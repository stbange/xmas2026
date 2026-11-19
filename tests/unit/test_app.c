/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "app/app.h"

void setUp(void) {}
void tearDown(void) {}

static tb_args headless_args(uint64_t ticks, uint64_t seed)
{
    tb_args a = {0};
    a.headless = true;
    a.max_ticks = ticks;
    a.seed = seed;
    return a;
}

static uint64_t run_headless(uint64_t ticks, uint64_t seed)
{
    tb_args a = headless_args(ticks, seed);
    static tb_app app;
    tb_app_init(&app, &a);
    while (tb_app_frame(&app, 0.0, NULL) == TB_APP_RUNNING) {
    }
    TEST_ASSERT_EQUAL_UINT64(ticks, tb_app_tick(&app));
    return tb_app_checksum(&app);
}

static void test_headless_runs_are_deterministic(void)
{
    uint64_t a = run_headless(600, 7);
    uint64_t b = run_headless(600, 7);
    TEST_ASSERT_EQUAL_HEX64(a, b);
    TEST_ASSERT_NOT_EQUAL(a, run_headless(600, 8));
    TEST_ASSERT_NOT_EQUAL(a, run_headless(601, 7));
}

static void test_headless_simulates_one_tick_per_frame(void)
{
    tb_args a = headless_args(0, 1);
    static tb_app app;
    tb_app_init(&app, &a);
    uint32_t ticks = 0;
    TEST_ASSERT_EQUAL_INT(TB_APP_RUNNING, tb_app_frame(&app, 123.0, &ticks));
    TEST_ASSERT_EQUAL_UINT32(1, ticks);
}

static void test_realtime_mode_uses_fixed_steps(void)
{
    tb_args a = {0};
    a.seed = 3;
    static tb_app app;
    tb_app_init(&app, &a);
    uint32_t ticks = 99;
    tb_app_frame(&app, 0.0, &ticks);
    TEST_ASSERT_EQUAL_UINT32(0, ticks);
    tb_app_frame(&app, 0.5 + 1e-9, &ticks);
    TEST_ASSERT_EQUAL_UINT32(8, ticks); /* capped at 8 steps per frame */
    tb_app_frame(&app, 0.5 + 2.5 / 60.0, &ticks);
    TEST_ASSERT_EQUAL_UINT32(2, ticks);
}

static void test_quit_request_ends_the_app(void)
{
    tb_args a = headless_args(0, 1);
    static tb_app app;
    tb_app_init(&app, &a);
    tb_app_request_quit(&app);
    TEST_ASSERT_EQUAL_INT(TB_APP_DONE, tb_app_frame(&app, 0.0, NULL));
}

static void test_snow_wraps_around(void)
{
    tb_args a = headless_args(60 * 60 * 3, 5); /* three simulated minutes */
    static tb_app app;
    tb_app_init(&app, &a);
    while (tb_app_frame(&app, 0.0, NULL) == TB_APP_RUNNING) {
    }
    for (int i = 0; i < TB_APP_SNOWFLAKES; i++) {
        TEST_ASSERT_TRUE(app.snow[i].y >= 0.0f && app.snow[i].y <= 1.0f);
    }
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_headless_runs_are_deterministic);
    RUN_TEST(test_headless_simulates_one_tick_per_frame);
    RUN_TEST(test_realtime_mode_uses_fixed_steps);
    RUN_TEST(test_quit_request_ends_the_app);
    RUN_TEST(test_snow_wraps_around);
    return UNITY_END();
}
