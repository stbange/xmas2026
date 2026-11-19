/* SPDX-License-Identifier: MIT */
#include "unity.h"

#include "sim/combat.h"

void setUp(void) {}
void tearDown(void) {}

/* @scenario player-core: Armour absorbs damage */
static void test_armour_absorbs_half(void)
{
    tb_vitals v = {100, 50, 0};
    TEST_ASSERT_EQUAL_INT32(10, tb_vitals_damage(&v, 20));
    TEST_ASSERT_EQUAL_INT32(90, v.health);
    TEST_ASSERT_EQUAL_INT32(40, v.armour);
}

static void test_armour_runs_out(void)
{
    tb_vitals v = {100, 5, 0};
    tb_vitals_damage(&v, 40);
    TEST_ASSERT_EQUAL_INT32(0, v.armour);
    TEST_ASSERT_EQUAL_INT32(65, v.health);
    TEST_ASSERT_EQUAL_INT32(0, tb_vitals_damage(&v, 0));
    TEST_ASSERT_EQUAL_INT32(0, tb_vitals_damage(&v, -5));
    TEST_ASSERT_EQUAL_INT32(65, tb_vitals_damage(&v, 500)); /* never below 0 */
    TEST_ASSERT_TRUE(tb_vitals_dead(&v));
}

/* @scenario player-core: Pizza damage */
static void test_pizza_halves_damage(void)
{
    tb_vitals v;
    tb_vitals_init(&v);
    tb_vitals_damage(&v, tb_difficulty_scale_damage(TB_DIFF_PIZZA, 40));
    TEST_ASSERT_EQUAL_INT32(80, v.health);
}

static void test_difficulty_scaling_and_names(void)
{
    TEST_ASSERT_EQUAL_INT32(40, tb_difficulty_scale_damage(TB_DIFF_COLA, 40));
    TEST_ASSERT_EQUAL_INT32(60, tb_difficulty_scale_damage(TB_DIFF_COFFEE, 40));
    TEST_ASSERT_EQUAL_INT32(80, tb_difficulty_scale_damage(TB_DIFF_NIGHTMARE, 40));
    TEST_ASSERT_EQUAL_INT32(5, tb_difficulty_scale_damage(TB_DIFF_PIZZA, 9)); /* rounds */
    TEST_ASSERT_EQUAL_INT32(40, tb_difficulty_scale_damage((tb_difficulty)9, 40));
    TEST_ASSERT_EQUAL_STRING("Pizza", tb_difficulty_name(TB_DIFF_PIZZA));
    TEST_ASSERT_EQUAL_STRING("Cola", tb_difficulty_name(TB_DIFF_COLA));
    TEST_ASSERT_EQUAL_STRING("Coffee at 4 AM", tb_difficulty_name(TB_DIFF_COFFEE));
    TEST_ASSERT_EQUAL_STRING("Nightmare LAN", tb_difficulty_name(TB_DIFF_NIGHTMARE));
    TEST_ASSERT_EQUAL_STRING("?", tb_difficulty_name((tb_difficulty)9));
}

static void test_heal_limits_and_overcharge_drain(void)
{
    tb_vitals v;
    tb_vitals_init(&v);
    TEST_ASSERT_EQUAL_INT32(0, tb_vitals_heal(&v, 25, TB_HEALTH_MAX)); /* already full */
    v.health = 90;
    TEST_ASSERT_EQUAL_INT32(10, tb_vitals_heal(&v, 25, TB_HEALTH_MAX));
    TEST_ASSERT_EQUAL_INT32(50, tb_vitals_heal(&v, 50, TB_HEALTH_OVERCHARGE));
    TEST_ASSERT_EQUAL_INT32(150, v.health);
    TEST_ASSERT_EQUAL_INT32(0, tb_vitals_heal(&v, -1, TB_HEALTH_OVERCHARGE));
    for (int i = 0; i < 60 * 10; i++) {
        tb_vitals_tick(&v, 60);
    }
    TEST_ASSERT_EQUAL_INT32(140, v.health); /* 1 per second */
    v.health = 100;
    v.drain_ticks = 30;
    tb_vitals_tick(&v, 60);
    TEST_ASSERT_EQUAL_INT32(0, v.drain_ticks);
    v.health = 0;
    TEST_ASSERT_EQUAL_INT32(0, tb_vitals_heal(&v, 10, TB_HEALTH_MAX)); /* the dead stay dead */
}

static void test_armour_pickup_limits(void)
{
    tb_vitals v;
    tb_vitals_init(&v);
    TEST_ASSERT_EQUAL_INT32(80, tb_vitals_add_armour(&v, 80));
    TEST_ASSERT_EQUAL_INT32(20, tb_vitals_add_armour(&v, 80));
    TEST_ASSERT_EQUAL_INT32(0, tb_vitals_add_armour(&v, 10));
    v.armour = 0;
    TEST_ASSERT_EQUAL_INT32(0, tb_vitals_add_armour(&v, 0));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_armour_absorbs_half);
    RUN_TEST(test_armour_runs_out);
    RUN_TEST(test_pizza_halves_damage);
    RUN_TEST(test_difficulty_scaling_and_names);
    RUN_TEST(test_heal_limits_and_overcharge_drain);
    RUN_TEST(test_armour_pickup_limits);
    return UNITY_END();
}
