/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "core/rng.h"

void setUp(void) {}
void tearDown(void) {}

static void test_same_seed_same_sequence(void)
{
    tb_rng a, b;
    tb_rng_seed(&a, 2026, 12);
    tb_rng_seed(&b, 2026, 12);
    for (int i = 0; i < 1000; i++) {
        TEST_ASSERT_EQUAL_UINT32(tb_rng_u32(&a), tb_rng_u32(&b));
    }
}

static void test_known_first_value_is_stable(void)
{
    /* Golden value: guards against accidental algorithm changes, which
     * would break replays and the daily dungeon seed. */
    tb_rng r;
    tb_rng_seed(&r, 42u, 54u);
    TEST_ASSERT_EQUAL_HEX32(0xa15c02b7u, tb_rng_u32(&r));
}

static void test_different_streams_differ(void)
{
    tb_rng a, b;
    tb_rng_seed(&a, 1, 1);
    tb_rng_seed(&b, 1, 2);
    TEST_ASSERT_NOT_EQUAL(tb_rng_u32(&a), tb_rng_u32(&b));
}

static void test_range_bounds_and_coverage(void)
{
    tb_rng r;
    tb_rng_seed(&r, 7, 0);
    int seen[6] = {0};
    for (int i = 0; i < 6000; i++) {
        int32_t v = tb_rng_range(&r, 1, 6);
        TEST_ASSERT_TRUE(v >= 1 && v <= 6);
        seen[v - 1]++;
    }
    for (int i = 0; i < 6; i++) {
        TEST_ASSERT_TRUE(seen[i] > 800);
    }
    TEST_ASSERT_EQUAL_INT32(5, tb_rng_range(&r, 5, 5));
    TEST_ASSERT_EQUAL_INT32(5, tb_rng_range(&r, 5, 1));
    int32_t full = tb_rng_range(&r, INT32_MIN, INT32_MAX);
    (void)full;
    int32_t neg = tb_rng_range(&r, -10, -5);
    TEST_ASSERT_TRUE(neg >= -10 && neg <= -5);
}

static void test_float_and_chance(void)
{
    tb_rng r;
    tb_rng_seed(&r, 99, 3);
    int hits = 0;
    for (int i = 0; i < 10000; i++) {
        float f = tb_rng_float01(&r);
        TEST_ASSERT_TRUE(f >= 0.0f && f < 1.0f);
        hits += tb_rng_chance(&r, 0.25f);
    }
    TEST_ASSERT_TRUE(hits > 2200 && hits < 2800);
    TEST_ASSERT_FALSE(tb_rng_chance(&r, 0.0f));
    TEST_ASSERT_TRUE(tb_rng_chance(&r, 1.0f));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_same_seed_same_sequence);
    RUN_TEST(test_known_first_value_is_stable);
    RUN_TEST(test_different_streams_differ);
    RUN_TEST(test_range_bounds_and_coverage);
    RUN_TEST(test_float_and_chance);
    return UNITY_END();
}
