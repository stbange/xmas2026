/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "core/timestep.h"

void setUp(void) {}
void tearDown(void) {}

static void test_first_frame_simulates_nothing(void)
{
    tb_timestep ts;
    tb_timestep_init(&ts, 60, 8);
    TEST_ASSERT_EQUAL_UINT32(0, tb_timestep_advance(&ts, 10.0));
}

static void test_steps_match_elapsed_time(void)
{
    tb_timestep ts;
    tb_timestep_init(&ts, 60, 8);
    tb_timestep_advance(&ts, 0.0);
    TEST_ASSERT_EQUAL_UINT32(1, tb_timestep_advance(&ts, 1.0 / 60.0 + 1e-9));
    TEST_ASSERT_EQUAL_UINT32(2, tb_timestep_advance(&ts, 3.0 / 60.0 + 1e-9));
    TEST_ASSERT_EQUAL_UINT32(0, tb_timestep_advance(&ts, 3.5 / 60.0));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.5f, tb_timestep_alpha(&ts));
    TEST_ASSERT_EQUAL_UINT64(3, ts.tick);
}

static void test_backlog_is_capped(void)
{
    tb_timestep ts;
    tb_timestep_init(&ts, 60, 4);
    tb_timestep_advance(&ts, 0.0);
    TEST_ASSERT_EQUAL_UINT32(4, tb_timestep_advance(&ts, 10.0));
    TEST_ASSERT_EQUAL_FLOAT(0.0f, tb_timestep_alpha(&ts));
    /* Exactly max steps of backlog: no drop needed. */
    TEST_ASSERT_EQUAL_UINT32(4, tb_timestep_advance(&ts, 10.0 + 4.0 / 60.0 + 1e-9));
}

static void test_time_going_backwards_is_ignored(void)
{
    tb_timestep ts;
    tb_timestep_init(&ts, 60, 4);
    tb_timestep_advance(&ts, 5.0);
    TEST_ASSERT_EQUAL_UINT32(0, tb_timestep_advance(&ts, 4.0));
    TEST_ASSERT_EQUAL_UINT32(1, tb_timestep_advance(&ts, 4.0 + 1.0 / 60.0 + 1e-9));
}

static void test_defaults_for_zero_parameters(void)
{
    tb_timestep ts;
    tb_timestep_init(&ts, 0, 0);
    TEST_ASSERT_EQUAL_DOUBLE(1.0 / 60.0, ts.step);
    TEST_ASSERT_EQUAL_UINT32(1, ts.max_steps);
}

static void test_alpha_clamps(void)
{
    tb_timestep ts;
    tb_timestep_init(&ts, 60, 4);
    ts.accumulator = -1.0;
    TEST_ASSERT_EQUAL_FLOAT(0.0f, tb_timestep_alpha(&ts));
    ts.accumulator = 1.0;
    TEST_ASSERT_TRUE(tb_timestep_alpha(&ts) < 1.0f);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_first_frame_simulates_nothing);
    RUN_TEST(test_steps_match_elapsed_time);
    RUN_TEST(test_backlog_is_capped);
    RUN_TEST(test_time_going_backwards_is_ignored);
    RUN_TEST(test_defaults_for_zero_parameters);
    RUN_TEST(test_alpha_clamps);
    return UNITY_END();
}
