/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "core/mathx.h"

void setUp(void) {}
void tearDown(void) {}

#define EPS 1e-5f

static void test_scalars(void)
{
    TEST_ASSERT_EQUAL_FLOAT(0.0f, tb_clampf(-1.0f, 0.0f, 1.0f));
    TEST_ASSERT_EQUAL_FLOAT(1.0f, tb_clampf(2.0f, 0.0f, 1.0f));
    TEST_ASSERT_EQUAL_FLOAT(0.5f, tb_clampf(0.5f, 0.0f, 1.0f));
    TEST_ASSERT_EQUAL_FLOAT(5.0f, tb_lerpf(0.0f, 10.0f, 0.5f));
    TEST_ASSERT_TRUE(tb_approx_eqf(1.0f, 1.000001f, EPS));
    TEST_ASSERT_FALSE(tb_approx_eqf(1.0f, 1.1f, EPS));
    TEST_ASSERT_EQUAL_FLOAT(1.0f, tb_sign(3.0f));
    TEST_ASSERT_EQUAL_FLOAT(-1.0f, tb_sign(-3.0f));
    TEST_ASSERT_EQUAL_FLOAT(0.0f, tb_sign(0.0f));
}

static void test_wrap_angle(void)
{
    TEST_ASSERT_FLOAT_WITHIN(EPS, 0.0f, tb_wrap_anglef(2.0f * TB_PI));
    TEST_ASSERT_FLOAT_WITHIN(EPS, -TB_PI / 2.0f, tb_wrap_anglef(3.0f * TB_PI / 2.0f));
    TEST_ASSERT_FLOAT_WITHIN(EPS, TB_PI / 2.0f, tb_wrap_anglef(-3.0f * TB_PI / 2.0f));
    TEST_ASSERT_FLOAT_WITHIN(EPS, 0.5f, tb_wrap_anglef(0.5f));
}

static void test_vec3_ops(void)
{
    tb_vec3 a = tb_v3(1, 2, 3), b = tb_v3(4, 5, 6);
    TEST_ASSERT_TRUE(tb_v3_approx_eq(tb_v3(5, 7, 9), tb_v3_add(a, b), EPS));
    TEST_ASSERT_TRUE(tb_v3_approx_eq(tb_v3(3, 3, 3), tb_v3_sub(b, a), EPS));
    TEST_ASSERT_TRUE(tb_v3_approx_eq(tb_v3(2, 4, 6), tb_v3_scale(a, 2), EPS));
    TEST_ASSERT_EQUAL_FLOAT(32.0f, tb_v3_dot(a, b));
    TEST_ASSERT_TRUE(tb_v3_approx_eq(tb_v3(0, 0, 1), tb_v3_cross(tb_v3(1, 0, 0), tb_v3(0, 1, 0)), EPS));
    TEST_ASSERT_EQUAL_FLOAT(5.0f, tb_v3_len(tb_v3(3, 4, 0)));
    TEST_ASSERT_TRUE(tb_v3_approx_eq(tb_v3(0.6f, 0.8f, 0), tb_v3_normalize(tb_v3(3, 4, 0)), EPS));
    TEST_ASSERT_TRUE(tb_v3_approx_eq(tb_v3(0, 0, 0), tb_v3_normalize(tb_v3(0, 0, 0)), EPS));
    TEST_ASSERT_TRUE(tb_v3_approx_eq(tb_v3(2.5f, 3.5f, 4.5f), tb_v3_lerp(a, b, 0.5f), EPS));
}

static void test_vec3_approx_eq_checks_each_axis(void)
{
    tb_vec3 a = tb_v3(1, 1, 1);
    TEST_ASSERT_FALSE(tb_v3_approx_eq(a, tb_v3(2, 1, 1), EPS));
    TEST_ASSERT_FALSE(tb_v3_approx_eq(a, tb_v3(1, 2, 1), EPS));
    TEST_ASSERT_FALSE(tb_v3_approx_eq(a, tb_v3(1, 1, 2), EPS));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_scalars);
    RUN_TEST(test_wrap_angle);
    RUN_TEST(test_vec3_ops);
    RUN_TEST(test_vec3_approx_eq_checks_each_axis);
    return UNITY_END();
}
